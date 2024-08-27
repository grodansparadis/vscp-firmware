/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2024 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is part of VSCP - Very Simple Control Protocol
 * https://www.vscp.org
 *
 * ******************************************************************************
 */

#include <inttypes.h>
#include <string.h>
#include <vscp-firmware-helper.h>
#include <vscp.h>
#include <vscp-class.h>
#include <vscp-type.h>

#include "vscp-firmware-level2.h"

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

/* Constants */

/* Globals */

// Pointer to configuration data.
static vscp_frmw2_firmware_configt_t* g_pconfig;

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_init
//

int
vscp_frmw2_init(vscp_frmw2_firmware_configt_t* const pcfg)
{
  int rv;

  if (NULL != pcfg) {
    // Save configuration
    g_pconfig = pcfg;
  }

  vscpEventEx exrply;
  vscp_frmw2_setup_event_ex(&exrply);

  if (VSCP_LEVEL1 == g_pconfig->m_level) {
    /*!
      We do not use the GUID in level I for communication
      instead we use a 8-bit or 16-bit nickname.
    */
    // g_pconfig->m_guid[14] = (g_pconfig->m_nickname >> 8) & 0xff;
    // g_pconfig->m_guid[15] = g_pconfig->m_nickname & 0xff;
  }
  else {
    // In level 2 we use the set GUID
  }

  // We are in limbo still
  g_pconfig->m_state = FRMW2_STATE_NONE;

  g_pconfig->m_alarm_status       = 0; // No alarms yet
  g_pconfig->m_errorCounter       = 0; // No errors yet
  g_pconfig->m_page_select        = 0;
  g_pconfig->m_reset_device_flags = 0; // VSCP_TYPE_PROTOCOL_RESET_DEVICE

  if (VSCP_FRMW2_UNASSIGNED == g_pconfig->m_probe_timeout) {
    // Set default
    g_pconfig->m_probe_timeout = VSCP_PROBE_TIMEOUT;
  }

  if (VSCP_FRMW2_UNASSIGNED == g_pconfig->m_probe_timeout_count) {
    // Set default
    g_pconfig->m_probe_timeout_count = VSCP_PROBE_TIMEOUT_COUNT;
  }

  if (0xffff == g_pconfig->m_nickname) {

    g_pconfig->m_probe_nickname = 0; // First check for a segment controller

    // Get a starttime
    g_pconfig->m_timer1 = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);

    // Send probe
    vscp_frmw2_setup_event_ex(&exrply);
    exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
    exrply.vscp_type  = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
    if (g_pconfig->m_bUse16BitNickname) {
      exrply.sizeData = 2;
      exrply.data[0]  = 0; // We probe segment controller
      exrply.data[1]  = 0;
    }
    else {
      exrply.sizeData = 1;
      exrply.data[0]  = 0; // We probe segment controller
    }

    // Send new node online
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply))) {
      return rv;
    }

    // We are now in probe state (waiting for segment controller probe)
    g_pconfig->m_state    = FRMW2_STATE_PROBE;
    g_pconfig->m_substate = FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING;
  }
  else {
    vscp_frmw2_send_probe(OPT_NEW_NODE_ONLINE);

    // Send heartbeat
    g_pconfig->m_last_heartbeat = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);
    vscp_frmw2_send_heartbeat();

    // Send caps
    if ((VSCP_LEVEL2 == g_pconfig->m_level)) {
      g_pconfig->m_last_caps = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);
      vscp_frmw2_send_caps();
    }

    g_pconfig->m_state = FRMW2_STATE_ACTIVE;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_send_probe
//

int
vscp_frmw2_send_probe(int bNewNodeOnLine)
{
  vscpEventEx ex;

  // Send probe
  vscp_frmw2_setup_event_ex(&ex);
  ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  ex.vscp_type  = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;

  if (bNewNodeOnLine) {
    ex.sizeData = 0;
  }
  else {
    if (g_pconfig->m_bUse16BitNickname) {
      ex.sizeData = 2;
      ex.data[0]  = (g_pconfig->m_nickname >> 8) & 0xff;
      ex.data[1]  = g_pconfig->m_nickname & 0xff;
    }
    else {
      ex.sizeData = 1;
      ex.data[0]  = g_pconfig->m_nickname;
    }
  }

  // Send probe
  return vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_nickname_discovery
//

int
vscp_frmw2_nickname_discovery(const vscpEventEx* const pex)
{
  int rv = VSCP_ERROR_SUCCESS;

  // If there is an event we should check if we should react
  // on it
  if (NULL != pex) {

    if ((VSCP_CLASS1_PROTOCOL == pex->vscp_class) &&
        (VSCP_TYPE_PROTOCOL_PROBE_ACK == pex->vscp_type)) {
      // No check for nickname if we are probing for as egment controller
      if (FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING != g_pconfig->m_substate) {
        if (g_pconfig->m_bUse16BitNickname) {
          // 16-bit nickname
          if (construct_unsigned16(pex->GUID[14], pex->GUID[15]) != g_pconfig->m_probe_nickname) {
            // not to us
            return VSCP_ERROR_SUCCESS;
          }
        }
        else {
          // 8-bit nickname
          if (pex->GUID[15] != g_pconfig->m_probe_nickname) {
            // Not to us
            return VSCP_ERROR_SUCCESS;
          }
        }
      }

      // We got a response from a node with the same nickname
      if (FRMW2_STATE_PROBE == g_pconfig->m_state) {
        if (FRMW2_SUBSTATE_NONE == g_pconfig->m_substate) {
          ; // We do nothing
        }
        else if (FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING == g_pconfig->m_substate) {
          // Segment controller responded
          g_pconfig->m_state  = FRMW2_STATE_PREACTIVE;
          g_pconfig->m_timer1 = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);
        }
        else if (FRMW2_SUBSTATE_PROBE_WAITING == g_pconfig->m_substate) {

          // Try next nickname
          g_pconfig->m_probe_nickname++;

          // reset test time and count
          g_pconfig->m_probe_timeout_count = 0;
          g_pconfig->m_timer1              = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);

          // Check to see if we have tested all nicknames
          if ((g_pconfig->m_bUse16BitNickname && (0xffff == g_pconfig->m_nickname)) ||
              (!g_pconfig->m_bUse16BitNickname && (0xff == g_pconfig->m_nickname))) {
            // Nothing more we can do.
            g_pconfig->m_state = FRMW2_STATE_ERROR;
          }
        }
      }
    }
  }
  else {
    // No event
    if (FRMW2_STATE_PROBE == g_pconfig->m_state) {
      if (FRMW2_SUBSTATE_NONE == g_pconfig->m_substate) {
        g_pconfig->m_probe_timeout_count = 0;
      SEND_PROBE:
        g_pconfig->m_timer1 = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);

        // Send probe
        if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_send_probe(OPT_PROBE))) {
          return rv;
        }
        if (g_pconfig->m_probe_nickname) {
          // Standard probe
          g_pconfig->m_substate = FRMW2_SUBSTATE_PROBE_WAITING;
        }
        else {
          // If we probe the segment controller (nickname = 0) - Longer timeout
          g_pconfig->m_substate = FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING;
        }
      }
      else if (FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING == g_pconfig->m_substate) {
        // Timeout
        if ((vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata) - g_pconfig->m_timer1) > VSCP_SEGCTRL_PROBE_TIMEOUT) {
          // revert to normal probe
          g_pconfig->m_probe_nickname      = 1; // Start with first non segment ctrl node
          g_pconfig->m_substate            = FRMW2_SUBSTATE_PROBE_WAITING;
          g_pconfig->m_probe_timeout_count = 0;
          g_pconfig->m_timer1              = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);
          goto SEND_PROBE;
        }
      }
      else if (FRMW2_SUBSTATE_PROBE_WAITING == g_pconfig->m_substate) {
        // Timeout
        if ((vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata) - g_pconfig->m_timer1) > VSCP_PROBE_TIMEOUT) {
          // If we have probed three times we are done
          if (g_pconfig->m_probe_timeout_count >= VSCP_PROBE_TIMEOUT_COUNT) {

            // We use this nickname
            g_pconfig->m_nickname = g_pconfig->m_probe_nickname;

            g_pconfig->m_state    = FRMW2_STATE_ACTIVE;
            g_pconfig->m_substate = FRMW2_SUBSTATE_NONE;
            // Set id
            // g_pconfig->m_guid[15] = g_pconfig->m_nickname & 0xff;
            // g_pconfig->m_guid[14] = (g_pconfig->m_nickname >> 8) & 0xff;
            return vscp_frmw2_send_probe(OPT_NEW_NODE_ONLINE);
          }
          else {
            g_pconfig->m_probe_timeout_count++;
            g_pconfig->m_timer1 = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);
            goto SEND_PROBE;
          }
        }
      }
    }
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_nickname_wait
//

int
vscp_frmw2_nickname_wait(const vscpEventEx* const pex)
{
  int rv = VSCP_ERROR_SUCCESS;

  // If segment controller does not respond withing set timeout period
  // go back to probe state
  if ((vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata) - g_pconfig->m_timer1) > VSCP_SEGCTRL_RESPONSE_TIMEOUT) {
    g_pconfig->m_nickname = 1; // Skip probing of segment controller this time
    g_pconfig->m_state    = FRMW2_STATE_NONE;
    g_pconfig->m_substate = FRMW2_SUBSTATE_NONE;
    return VSCP_ERROR_SUCCESS;
  }

  // If no event to handle noting more to do for now
  if (NULL == pex) {
    return VSCP_ERROR_SUCCESS;
  }

  if (VSCP_CLASS1_PROTOCOL == pex->vscp_class) {

    if (VSCP_TYPE_PROTOCOL_SET_NICKNAME == pex->vscp_type) {
      // 8-bit nickname
      if (2 == ADJSIZEX) {
        // Addressed to us?
        if (construct_unsigned16(0, EXDTA(0)) == g_pconfig->m_nickname) {

          // Set nickname
          g_pconfig->m_nickname = EXDTA(1);

          // Inform app. that nickname changed
          vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_NICKNAME_ID_MSB);
          vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_NICKNAME_ID_LSB);

          vscpEventEx exrply;
          vscp_frmw2_setup_event_ex(&exrply);
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_NICKNAME_ACCEPTED;
          rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
        }
      }
      // 16-bit nickname
      else if (4 == ADJSIZEX) {
        // Addressed to us?
        if (construct_unsigned16(EXDTA(0), EXDTA(1)) == g_pconfig->m_nickname) {

          // Set nickname
          g_pconfig->m_nickname = construct_unsigned16(EXDTA(2), EXDTA(3));

          // Inform app. that nickname changed
          vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_NICKNAME_ID_MSB);
          vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_NICKNAME_ID_LSB);

          vscpEventEx exrply;
          vscp_frmw2_setup_event_ex(&exrply);
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_NICKNAME_ACCEPTED;
          rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
        }
      }
      else {
        rv = VSCP_ERROR_INVALID_FRAME;
      }
    }
  }

  return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_work
//

int
vscp_frmw2_work(const vscpEventEx* const pex)
{
  int rv = VSCP_ERROR_SUCCESS;

  // Check for init button press   TODO

  g_pconfig->m_vscp_class = 0;
  g_pconfig->m_offset     = 0;
  memset(g_pconfig->m_ifguid, 0, 16);

  if (NULL != pex) {

    // If there is an event we should check if we should react
    // on it

    g_pconfig->m_vscp_class = pex->vscp_class;

    /*
      Check if we have a proxy event, if we have
      remove GUID and make it a standard level I event
    */
    if ((pex->vscp_class >= 512) && (pex->vscp_class < 1024)) {
      memcpy(g_pconfig->m_ifguid, pex->data, 16); // Save interface GUID
      g_pconfig->m_offset = 16;                   // Data is at offset 16
      g_pconfig->m_vscp_class -= 512;             // Make it a standard level I event
    }
  }

  switch (g_pconfig->m_state) {

    // Just go to probe
    case FRMW2_STATE_NONE:
      g_pconfig->m_state    = FRMW2_STATE_PROBE;
      g_pconfig->m_substate = FRMW2_SUBSTATE_NONE;
      return vscp_frmw2_nickname_discovery(NULL);

    // Probe has been sent (waiting for answer)
    case FRMW2_STATE_PROBE:
      return vscp_frmw2_nickname_discovery(pex);

    // Happens when segment controller take probe. Wait for nickname
    case FRMW2_STATE_PREACTIVE:
      return vscp_frmw2_nickname_wait(pex);

    // Normal working state
    case FRMW2_STATE_ACTIVE:
      if (NULL != pex) {
        return vscp_frmw2_handle_protocol_event(pex);
      }
      break;

    // No free address or other error.
    case FRMW2_STATE_ERROR:
      break;
  }

  if (g_pconfig->m_interval_heartbeat &&
      (vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata) - g_pconfig->m_last_heartbeat) > g_pconfig->m_interval_heartbeat) {
    // Send heartbeat
    g_pconfig->m_last_heartbeat = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);
    vscp_frmw2_send_heartbeat();
  }

  if ((VSCP_LEVEL2 == g_pconfig->m_level) &&
      g_pconfig->m_interval_caps &&
      (vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata) - g_pconfig->m_last_caps) > g_pconfig->m_interval_caps) {
    // Send caps
    g_pconfig->m_last_caps = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);
    vscp_frmw2_send_caps();
  }

  // Check if the DM is interested in this event
  if (NULL != pex) {
    vscp_frmw2_feed_dm(pex);
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_handle_protocol_event
//

int
vscp_frmw2_handle_protocol_event(const vscpEventEx* const pex)
{
  int rv = VSCP_ERROR_SUCCESS;

  if (NULL == pex) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (VSCP_CLASS1_PROTOCOL == pex->vscp_class) {

    switch (pex->vscp_type) {

      case VSCP_TYPE_PROTOCOL_SEGCTRL_HEARTBEAT:
        // We do nothing
        break;

      case VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE:
        // 8-bit node id
        if (1 == ADJSIZEX) {
          // If addressed to us?
          if (construct_unsigned16(EXDTA(0), EXDTA(1)) == g_pconfig->m_nickname) {
            // Confirm probe
            vscpEventEx exrply;
            vscp_frmw2_setup_event_ex(&exrply);
            exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
            exrply.vscp_type  = VSCP_TYPE_PROTOCOL_PROBE_ACK;
            rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
          }
        }
        else if (2 == ADJSIZEX) {
          // If addressed to us?
          if (construct_unsigned16(EXDTA(0), EXDTA(1)) == g_pconfig->m_nickname) {
            // Confirm probe
            vscpEventEx exrply;
            vscp_frmw2_setup_event_ex(&exrply);
            exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
            exrply.vscp_type  = VSCP_TYPE_PROTOCOL_PROBE_ACK;
            rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
          }
        }
        else if (16 == ADJSIZEX) {
          // 16-bit node id
          if (0 == memcmp(g_pconfig->m_guid, &EXDTA(0), 16)) {
            vscpEventEx exrply;
            vscp_frmw2_setup_event_ex(&exrply);
            exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
            exrply.vscp_type  = VSCP_TYPE_PROTOCOL_PROBE_ACK;
            rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
          }
        }
        else {
          rv = VSCP_ERROR_INVALID_FRAME;
        }
        break;

      case VSCP_TYPE_PROTOCOL_SET_NICKNAME:
        // 8-bit nickname
        if (2 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(0, EXDTA(0)) == g_pconfig->m_nickname) {

            // Set new nickname
            g_pconfig->m_nickname = EXDTA(1);

            // Inform app. that nickname changed
            vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_NICKNAME_ID_MSB);
            vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_NICKNAME_ID_LSB);

            vscpEventEx exrply;
            vscp_frmw2_setup_event_ex(&exrply);
            exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
            exrply.vscp_type  = VSCP_TYPE_PROTOCOL_NICKNAME_ACCEPTED;
            rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
          }
        }
        // 16-bit nickname
        else if (4 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(EXDTA(0), EXDTA(1)) == g_pconfig->m_nickname) {

            // Set new nickname
            g_pconfig->m_nickname = construct_unsigned16(EXDTA(2), EXDTA(3));

            // Inform app. that nickname changed
            vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_NICKNAME_ID_MSB);
            vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_NICKNAME_ID_LSB);

            vscpEventEx exrply;
            vscp_frmw2_setup_event_ex(&exrply);
            exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
            exrply.vscp_type  = VSCP_TYPE_PROTOCOL_NICKNAME_ACCEPTED;
            rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
          }
        }
        else {
          rv = VSCP_ERROR_INVALID_FRAME;
        }
        break;

      case VSCP_TYPE_PROTOCOL_DROP_NICKNAME: {

        uint8_t flags = 0x40; // restore defaults as default
        uint32_t tm   = 0;

        // 8-bit nickname
        if ((ADJSIZEX >= 1) && (ADJSIZEX <= 3)) {
          // Addressed to us?
          if (construct_unsigned16(0, EXDTA(0)) == g_pconfig->m_nickname) {

            // Get flags
            if (ADJSIZEX > 1) {
              flags = EXDTA(1);
            }
            // Get time to start in seconds
            if (ADJSIZEX > 2) {
              tm = EXDTA(2) * 1000;
            }
          }
        }
        // 16-bit nickname
        else if (4 == ADJSIZEX) {
          flags = EXDTA(2);
          tm    = EXDTA(3) * 1000;
        }
        else {
          rv = VSCP_ERROR_INVALID_FRAME;
        }

        // Handle the event
        if (VSCP_ERROR_SUCCESS == rv) {

          // Lock device if bit 7 is set
          if (flags & 0x80) {
            while (1) {
              // Feed watchdog
              vscp_frmw2_callback_feed_watchdog(g_pconfig->m_puserdata);
            }
          }

          // Restore defaults
          if (flags & 0x40) {
            vscp_frmw2_callback_restore_defaults(g_pconfig->m_puserdata);
          }

          // Start over and find nickname again...
          if (!(flags & 0x20)) {
            g_pconfig->m_nickname = 0xffff;
          }

          // If we should delay before restart
          if (tm) {
            g_pconfig->m_timer1 = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);
            while ((vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata) - g_pconfig->m_timer1) < tm) {
              // Feed watchdog
              vscp_frmw2_callback_feed_watchdog(g_pconfig->m_puserdata);
            }
          }
          vscp_frmw2_init(NULL);
        }
      } break;

      case VSCP_TYPE_PROTOCOL_READ_REGISTER: {

        uint8_t val;
        uint8_t reg;

        // 8-bit id
        if (2 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(0, EXDTA(0)) == g_pconfig->m_nickname) {
            reg = EXDTA(1);
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(reg, &val))) {
              ;
            }
          }
        }
        // 16-bit id
        else if (3 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(EXDTA(0), EXDTA(1)) == g_pconfig->m_nickname) {
            reg = EXDTA(2);
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(reg, &val))) {
              ;
            }
          }
        }
        else {
          rv = VSCP_ERROR_INVALID_FRAME;
        }

        // Send response
        if (VSCP_ERROR_SUCCESS == rv) {
          vscpEventEx exrply;
          vscp_frmw2_setup_event_ex(&exrply);
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          exrply.sizeData   = 2;
          exrply.data[0]    = reg;
          exrply.data[1]    = val;
          rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
        }
      } break;

      case VSCP_TYPE_PROTOCOL_WRITE_REGISTER: {

        uint8_t val;
        uint8_t reg;

        // 8-bit id
        if (3 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(0, EXDTA(0)) == g_pconfig->m_nickname) {
            reg = EXDTA(1);
            val = EXDTA(2);
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_write_reg(reg, val))) {
              ;
            }
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(reg, &val))) {
              ;
            }
          }
        }
        // 16-bit id
        else if (4 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(EXDTA(0), EXDTA(1)) == g_pconfig->m_nickname) {
            reg = EXDTA(2);
            val = EXDTA(3);
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_write_reg(reg, val))) {
              ;
            }
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(reg, &val))) {
              ;
            }
          }
        }
        else {
          rv = VSCP_ERROR_INVALID_FRAME;
        }

        // Send response
        if (VSCP_ERROR_SUCCESS == rv) {
          vscpEventEx exrply;
          vscp_frmw2_setup_event_ex(&exrply);
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          exrply.sizeData   = 2;
          exrply.data[0]    = reg;
          exrply.data[0]    = val;
          rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
        }

      } break;

      case VSCP_TYPE_PROTOCOL_RW_RESPONSE:
        // We do nothing
        break;

      case VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER: {
        // Get boot algorithm
        if ((8 == ADJSIZEX) &&
            (EXDTA(0) == (g_pconfig->m_nickname & 0xff)) &&
            (EXDTA(1) == g_pconfig->m_bootloader_algorithm) &&
            (EXDTA(2) == g_pconfig->m_guid[0]) &&
            (EXDTA(3) == g_pconfig->m_guid[3]) &&
            (EXDTA(4) == g_pconfig->m_guid[4]) &&
            (EXDTA(5) == g_pconfig->m_guid[5]) &&
            (EXDTA(6) == ((g_pconfig->m_page_select >> 8) & 0xff)) &&
            (EXDTA(7) == ((g_pconfig->m_page_select) & 0xff))) {
          vscp_frmw2_callback_enter_bootloader(g_pconfig->m_puserdata);
        }
      } break;

      case VSCP_TYPE_PROTOCOL_RESET_DEVICE: {
        if (5 == ADJSIZEX) {

          switch (EXDTA(0)) {
            case 0:
              if ((EXDTA(1) == g_pconfig->m_guid[15]) &&
                  (EXDTA(2) == g_pconfig->m_guid[14]) &&
                  (EXDTA(3) == g_pconfig->m_guid[13]) &&
                  (EXDTA(4) == g_pconfig->m_guid[12])) {
                g_pconfig->m_reset_device_flags |= 0b0001;
                // One second to receive the rest of the events
                g_pconfig->m_timer1 = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);
              }
              break;

            case 1:
              if ((EXDTA(1) == g_pconfig->m_guid[11]) &&
                  (EXDTA(2) == g_pconfig->m_guid[10]) &&
                  (EXDTA(3) == g_pconfig->m_guid[9]) &&
                  (EXDTA(4) == g_pconfig->m_guid[8])) {
                g_pconfig->m_reset_device_flags |= 0b0010;
              }
              break;

            case 2:
              if ((EXDTA(1) == g_pconfig->m_guid[7]) &&
                  (EXDTA(2) == g_pconfig->m_guid[6]) &&
                  (EXDTA(3) == g_pconfig->m_guid[5]) &&
                  (EXDTA(4) == g_pconfig->m_guid[4])) {
                g_pconfig->m_reset_device_flags |= 0b0100;
              }
              break;

            case 3:
              if ((EXDTA(1) == g_pconfig->m_guid[3]) &&
                  (EXDTA(2) == g_pconfig->m_guid[2]) &&
                  (EXDTA(3) == g_pconfig->m_guid[1]) &&
                  (EXDTA(4) == g_pconfig->m_guid[0])) {
                g_pconfig->m_reset_device_flags |= 0b1000;
              }
              break;

            default:
              g_pconfig->m_reset_device_flags = 0;
              break;
          }

          if (0b1111 == g_pconfig->m_reset_device_flags) {
            g_pconfig->m_reset_device_flags = 0;
            vscp_frmw2_callback_restore_defaults(g_pconfig->m_puserdata);
            vscp_frmw2_init(NULL);
            // vscp_frmw2_callback_reset(g_pconfig->m_puserdata);
          }
          else {
            // all events must be received within the specified time
            if ((vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata) - g_pconfig->m_timer1) > VSCP_GUID_RESET_TIMEOUT) {
              // Timeout Start over
              g_pconfig->m_reset_device_flags = 0;
            }
          }
        }
      } break;

      case VSCP_TYPE_PROTOCOL_PAGE_READ:
        if ((3 == ADJSIZEX) &&
            (g_pconfig->m_nickname == EXDTA(0))) {
          vscp_frmw2_page_read(EXDTA(1), EXDTA(2)); // index, count
        }
        else if ((4 == ADJSIZEX) && (g_pconfig->m_nickname == construct_unsigned16(EXDTA(0), EXDTA(1)))) {
          vscp_frmw2_page_read(EXDTA(3), EXDTA(3)); // index, count
        }
        break;

      case VSCP_TYPE_PROTOCOL_PAGE_WRITE:
        // Don't handle 16-bit node-id
        if ((ADJSIZEX > 3) &&
            (g_pconfig->m_nickname == EXDTA(0))) {
          vscp_frmw2_page_write(EXDTA(1), ADJSIZEX - 2, &EXDTA(2)); // index, count, data
        }
        break;

      case VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE:
        // We do nothing
        break;

      case VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_PROBE:
        if (g_pconfig->m_bHighEndServerResponse) {
          vscpEventEx exrply;
          vscp_frmw2_setup_event_ex(&exrply);
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_RESPONSE;
          exrply.sizeData   = 8;
          exrply.data[0]    = (g_pconfig->m_high_end_srv_caps >> 8) & 0xff;
          exrply.data[1]    = g_pconfig->m_high_end_srv_caps & 0xff;
          exrply.data[2]    = (g_pconfig->m_high_end_ip_address >> 24) & 0xff;
          exrply.data[3]    = (g_pconfig->m_high_end_ip_address >> 16) & 0xff;
          exrply.data[4]    = (g_pconfig->m_high_end_ip_address >> 8) & 0xff;
          exrply.data[5]    = g_pconfig->m_high_end_ip_address & 0xff;
          exrply.data[6]    = (g_pconfig->m_high_end_srv_port >> 8) & 0xff;
          exrply.data[7]    = g_pconfig->m_high_end_srv_port & 0xff;
          rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
        }
        break;

      case VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_RESPONSE:
        // We do nothing
        break;

      case VSCP_TYPE_PROTOCOL_INCREMENT_REGISTER: {

        uint8_t val;
        vscpEventEx exrply;
        vscp_frmw2_setup_event_ex(&exrply);

        if ((2 == ADJSIZEX) &&
            (g_pconfig->m_nickname == EXDTA(0))) {

          vscp_frmw2_read_reg(EXDTA(1), &val);
          val++;
          vscp_frmw2_write_reg(EXDTA(1), val);

          // Send confirmation
          vscp_frmw2_setup_event_ex(&exrply);
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          exrply.sizeData   = 2;
          exrply.data[0]    = EXDTA(1);
          exrply.data[1]    = val;
          rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
        }
        else if ((3 == ADJSIZEX) && (g_pconfig->m_nickname == construct_unsigned16(EXDTA(0), EXDTA(1)))) {

          vscp_frmw2_read_reg(EXDTA(2), &val);
          val++;
          vscp_frmw2_write_reg(EXDTA(2), val);

          // Send confirmation
          vscp_frmw2_setup_event_ex(&exrply);
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          exrply.sizeData   = 2;
          exrply.data[0]    = EXDTA(1);
          exrply.data[1]    = val;
          rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
        }
      } break;

      case VSCP_TYPE_PROTOCOL_DECREMENT_REGISTER: {

        uint8_t val;
        vscpEventEx exrply;
        vscp_frmw2_setup_event_ex(&exrply);

        if ((2 == ADJSIZEX) &&
            (g_pconfig->m_nickname == EXDTA(0))) {

          vscp_frmw2_read_reg(EXDTA(1), &val);
          val--;
          vscp_frmw2_write_reg(EXDTA(1), val);

          // Send confirmation
          vscp_frmw2_setup_event_ex(&exrply);
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          exrply.sizeData   = 2;
          exrply.data[0]    = EXDTA(1);
          exrply.data[1]    = val;
          rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
        }
        else if ((3 == ADJSIZEX) && (g_pconfig->m_nickname == construct_unsigned16(EXDTA(0), EXDTA(1)))) {

          vscp_frmw2_read_reg(EXDTA(2), &val);
          val--;
          vscp_frmw2_write_reg(EXDTA(2), val);

          // Send confirmation
          vscp_frmw2_setup_event_ex(&exrply);
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          exrply.sizeData   = 2;
          exrply.data[0]    = EXDTA(1);
          exrply.data[1]    = val;
          rv                = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
        }
      } break;

      case VSCP_TYPE_PROTOCOL_WHO_IS_THERE:
        if (0 == ADJSIZEX) {
          vscp_frmw2_whois_response();
        }
        else if ((1 == ADJSIZEX) && ((0xff == EXDTA(0)) || (g_pconfig->m_nickname == EXDTA(0)))) {
          return vscp_frmw2_whois_response();
        }
        else if ((2 == ADJSIZEX) &&
                 ((0xffff == construct_unsigned16(EXDTA(0), EXDTA(1))) || (g_pconfig->m_nickname == construct_unsigned16(EXDTA(0), EXDTA(1))))) {
          return vscp_frmw2_whois_response();
        }
        break;

      case VSCP_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE:
        // We do nothing
        break;

      case VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO:
        rv = vscp_frmw2_report_dmatrix();
        break;

      case VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF: {

        uint16_t uppermax;
        uint16_t nframes;
        vscpEventEx exrply;
        vscp_frmw2_setup_event_ex(&exrply);
        if (VSCP_LEVEL1 == g_pconfig->m_level) {
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF_RESPONSE;
          uppermax          = 7;
        }
        else {
          exrply.vscp_class = VSCP_CLASS2_PROTOCOL;
          exrply.vscp_type  = VSCP2_TYPE_PROTOCOL_GET_EMBEDDED_MDF_RESPONSE;
          uppermax          = 511;
          nframes           = strlen(g_pconfig->m_pInternalMdf) / 508;
          if (strlen(g_pconfig->m_pInternalMdf) % 508) {
            nframes++;
          }
        }

        if (NULL != g_pconfig->m_pInternalMdf) {
          uint16_t idx  = 0;
          uint32_t pos  = (VSCP_LEVEL1 == g_pconfig->m_level) ? 2 : 4;
          const char* p = g_pconfig->m_pInternalMdf;
          do {
            exrply.data[pos] = *p;
            pos++;
            if (pos > uppermax) {
              exrply.sizeData = uppermax + 1;
              exrply.data[0]  = (idx >> 8) & 0xff;
              exrply.data[1]  = idx & 0xff;
              if (VSCP_LEVEL2 == g_pconfig->m_level) {
                exrply.data[2] = (nframes >> 8) & 0xff;
                exrply.data[3] = nframes & 0xff;
              }
              if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply))) {
                return rv;
              }
              pos = (VSCP_LEVEL1 == g_pconfig->m_level) ? 2 : 4;
              idx++;
            }
          } while (*(++p));

          if (VSCP_LEVEL1 == g_pconfig->m_level) {
            if (2 != pos) {
              exrply.sizeData = 2 + pos - 2;
              exrply.data[0]  = (idx >> 8) & 0xff;
              exrply.data[1]  = idx & 0xff;
              if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply))) {
                return rv;
              }
            }
          }
          else {
            if (4 != pos) {
              exrply.sizeData = 4 + pos - 4;
              exrply.data[0]  = (idx >> 8) & 0xff;
              exrply.data[1]  = idx & 0xff;
              exrply.data[2]  = (nframes >> 8) & 0xff;
              exrply.data[3]  = nframes & 0xff;
              if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply))) {
                return rv;
              }
            }
          }
        }
        else {
          exrply.sizeData = 0;
          rv              = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
        }
      } break;

      case VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_READ: {
        if ((4 == ADJSIZEX) && (g_pconfig->m_nickname == EXDTA(0))) {
          rv = vscp_frmw2_extended_page_read(EXDTA(0), construct_unsigned16(EXDTA(1), EXDTA(2)), EXDTA(3), 1);
        }
        if ((5 == ADJSIZEX) && (g_pconfig->m_nickname == EXDTA(0))) {
          rv = vscp_frmw2_extended_page_read(EXDTA(0), construct_unsigned16(EXDTA(1), EXDTA(2)), EXDTA(3), EXDTA(4));
        }
        if ((6 == ADJSIZEX) &&
            (g_pconfig->m_nickname == construct_unsigned16(EXDTA(0), EXDTA(1)))) {
          rv = vscp_frmw2_extended_page_read(construct_unsigned16(EXDTA(0), EXDTA(1)), construct_unsigned16(EXDTA(2), EXDTA(3)), EXDTA(4), EXDTA(5));
        }
      } break;

      case VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE: {
        uint8_t buf[4];
        uint8_t cnt = ADJSIZEX - 4;

        for (int i = 0; i < cnt; i++) {
          buf[i] = EXDTA(4 + i);
        }
        rv = vscp_frmw2_extended_page_write(EXDTA(0), construct_unsigned16(EXDTA(1), EXDTA(2)), EXDTA(3), cnt, buf);
      } break;

      case VSCP_TYPE_PROTOCOL_GET_EVENT_INTEREST: {

        vscpEventEx exrply;
        vscp_frmw2_setup_event_ex(&exrply);

        if (NULL == g_pconfig->m_pEventsOfInterest) {
          // All events is of interest
          exrply.sizeData   = 0;
          exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          exrply.vscp_type  = VSCP_TYPE_PROTOCOL_GET_EVENT_INTEREST_RESPONSE;
          if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply))) {
            return rv;
          }
        }
        else {
          if (VSCP_LEVEL1 == g_pconfig->m_level) {
            uint16_t idx = 0;
            while (g_pconfig->m_pEventsOfInterest[idx]) {
              exrply.sizeData = 5;
              exrply.data[0]  = idx; // Frame index
              exrply.data[1]  = (g_pconfig->m_pEventsOfInterest[idx] >> 16) & 0xff;
              exrply.data[2]  = g_pconfig->m_pEventsOfInterest[idx] & 0xff;
              exrply.data[3]  = (g_pconfig->m_pEventsOfInterest[idx] >> 16) & 0xff;
              exrply.data[4]  = g_pconfig->m_pEventsOfInterest[idx] & 0xff;
              if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply))) {
                return rv;
              }
              idx++;
            }
          }
          else {
            // Level II
            uint16_t pos  = 0;
            uint16_t size = 0;

            // Find number of events
            while (g_pconfig->m_pEventsOfInterest[pos]) {
              size++;
            }

            uint8_t nframes = size / 256;
            if (size % 256) {
              nframes++;
            }

            for (uint8_t idx = 0; idx < nframes; idx++) {

              exrply.sizeData = 0;
              for (uint16_t j = 0; j < 256; j++) {

                // If we are done
                if ((idx * 256 + j) > size) {
                  break;
                }

                exrply.data[2 + j] = (g_pconfig->m_pEventsOfInterest[pos + j] >> 16) & 0xffff;
                exrply.data[3 + j] = g_pconfig->m_pEventsOfInterest[pos + j] & 0xffff;

                exrply.sizeData += 2;
              }

              if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply))) {
                return rv;
              }
            }
          }
        }
      } break;
    }
  }
  // Level II
  else if (VSCP_CLASS2_PROTOCOL == pex->vscp_class) {

    switch (pex->vscp_type) {

      case VSCP2_TYPE_PROTOCOL_READ_REGISTER: {

        // Must be at least GUID + start reg
        if (pex->sizeData < 22) {
          return VSCP_ERROR_PARAMETER;
        }

        // Get start position
        uint32_t startpos = construct_unsigned32(pex->data[16],
                                                 pex->data[17],
                                                 pex->data[18],
                                                 pex->data[19]);

        // If size is given get it
        uint16_t cnt = construct_unsigned16(pex->data[20], pex->data[21]);
        if (cnt > (512 - 4)) {
          cnt = (512 - 4);
        }
        rv = vscp_frmw2_vscp2_register_read(startpos, cnt);
      } break;

      case VSCP2_TYPE_PROTOCOL_WRITE_REGISTER: {

        // Must be at least GUID + start reg + one data byte
        if (pex->sizeData < 23) {
          return VSCP_ERROR_PARAMETER;
        }

        // Get start position
        uint32_t startpos = construct_unsigned32(pex->data[16],
                                                 pex->data[17],
                                                 pex->data[18],
                                                 pex->data[19]);

        rv = vscp_frmw2_vscp2_register_write(startpos, pex->sizeData - 20, (const uint8_t*)&pex->data[16 + 4]);
      } break;

      case VSCP2_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE: {
        char devname[64];
        char url[32];

        vscpEventEx exrply;
        vscp_frmw2_setup_event_ex(&exrply);

        memcpy(exrply.data, g_pconfig->m_guid, 16);
        memcpy(&exrply.data[16], g_pconfig->m_mdfurl, 32);
        rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
      } break;
    }
  }

  return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_setup_event_ex
//

void
vscp_frmw2_setup_event_ex(vscpEventEx* const pex)
{
  // Check pointer
  if (NULL == pex) {
    return;
  }

  pex->sizeData = 0;
  memset(pex->data, 0, 512);
  pex->head = VSCP_PRIORITY_NORMAL;
  memcpy(pex->GUID, g_pconfig->m_guid, 16);
  /*!
    If level I we use the GUID as a space for the nickname
  */
  if (VSCP_LEVEL1 == g_pconfig->m_level) {
    pex->GUID[14] = (g_pconfig->m_nickname >> 8) & 0xff;
    pex->GUID[15] = g_pconfig->m_nickname & 0xff;
  }
  pex->vscp_class = VSCP_CLASS2_LEVEL1_PROTOCOL;
  pex->vscp_type  = VSCP2_TYPE_PROTOCOL_READ_WRITE_RESPONSE;
  vscp_frmw2_callback_set_event_time(g_pconfig->m_puserdata, pex);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_vscp2_register_read
//

int
vscp_frmw2_vscp2_register_read(uint32_t startreg, uint16_t count)
{
  int rv;

  vscpEventEx ex;
  vscp_frmw2_setup_event_ex(&ex);

  // Read the registers
  for (uint32_t pos = startreg; pos < (startreg + count); pos++) {
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(pos, &ex.data[pos - startreg + 4]))) {
      return rv;
    }
  }

  ex.vscp_class = VSCP_CLASS2_PROTOCOL;
  ex.vscp_type  = VSCP2_TYPE_PROTOCOL_READ_WRITE_RESPONSE;
  ex.sizeData   = count + 1;

  // Data
  ex.sizeData = count + 4;
  ex.data[0]  = (uint8_t)((startreg >> 24) & 0xff);
  ex.data[1]  = (uint8_t)((startreg >> 16) & 0xff);
  ex.data[2]  = (uint8_t)((startreg >> 8) & 0xff);
  ex.data[3]  = (uint8_t)(startreg & 0xff);

  // Send event
  return vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_vscp2_register_write
//

int
vscp_frmw2_vscp2_register_write(uint32_t startreg, uint16_t cnt, const uint8_t* const pdata)
{
  int rv;

  for (uint32_t pos = startreg; pos < (startreg + cnt); pos++) {
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_write_reg(g_pconfig->m_puserdata, 0, pos, pdata[pos - startreg]))) {
      return rv;
    }
  }

  return vscp_frmw2_vscp2_register_read(startreg, cnt);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_read_reg
//

int
vscp_frmw2_read_reg(uint32_t reg, uint8_t* pval)
{
  int rv;

  if (reg < (VSCP_STD_REGISTER_START + ADJSTDREG)) {
    // User register
    return vscp_frmw2_callback_read_reg(g_pconfig->m_puserdata, g_pconfig->m_page_select, reg, pval);
  }
  else {
    // * * * standard registers * * *
    if ((VSCP_STD_REGISTER_ALARM_STATUS + ADJSTDREG) == reg) {
      // Writing/reading alarm resets it regardless of value
      *pval                     = g_pconfig->m_alarm_status;
      g_pconfig->m_alarm_status = 0;
      // Inform app. that standard register has been set to zero
      vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_ALARM_STATUS);
    }
    else if ((VSCP_STD_REGISTER_MAJOR_VERSION + ADJSTDREG) == reg) {
      *pval = FRMW2_MAJOR_VERSION;
    }
    else if ((VSCP_STD_REGISTER_MINOR_VERSION + ADJSTDREG) == reg) {
      *pval = FRMW2_MINOR_VERSION;
    }
    else if ((VSCP_STD_REGISTER_ERROR_COUNTER + ADJSTDREG) == reg) {
      // Writing/reading error counter resets it regardless of value
      *pval                     = g_pconfig->m_errorCounter;
      g_pconfig->m_errorCounter = 0;
      // Inform app that standard register has been set to zero
      vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_ERROR_COUNTER);
    }
    else if (((VSCP_STD_REGISTER_USER_ID + ADJSTDREG) >= reg) && (VSCP_STD_REGISTER_USER_ID + ADJSTDREG + 3 <= reg)) {
      *pval = *(&g_pconfig->m_userId + (reg - VSCP_STD_REGISTER_USER_ID + ADJSTDREG));
    }
    else if (((VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG) == reg) && (VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG + 3 <= reg)) {
      *pval = *(&g_pconfig->m_manufacturerId + (reg - VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG));
    }
    else if (((VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG) == reg) && (VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG + 3 <= reg)) {
      *pval = *(&g_pconfig->m_manufacturerId + (reg - VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG));
    }
    else if ((VSCP_STD_REGISTER_NICKNAME_ID_LSB + ADJSTDREG) == reg) {
      *pval = g_pconfig->m_nickname & 0xff;
    }
    else if ((VSCP_STD_REGISTER_NICKNAME_ID_MSB + ADJSTDREG) == reg) {
      *pval = (g_pconfig->m_nickname >> 8) & 0xff;
    }
    else if ((VSCP_STD_REGISTER_PAGE_SELECT_MSB + ADJSTDREG) == reg) {
      *pval = (g_pconfig->m_page_select >> 8) & 0xff;
    }
    else if ((VSCP_STD_REGISTER_PAGE_SELECT_LSB + ADJSTDREG) == reg) {
      *pval = g_pconfig->m_page_select & 0xff;
    }
    else if ((VSCP_STD_REGISTER_FIRMWARE_MAJOR + ADJSTDREG) == reg) {
      *pval = g_pconfig->m_firmware_major_version;
    }
    else if ((VSCP_STD_REGISTER_FIRMWARE_MINOR + ADJSTDREG) == reg) {
      *pval = g_pconfig->m_firmware_minor_version;
    }
    else if ((VSCP_STD_REGISTER_FIRMWARE_SUBMINOR + ADJSTDREG) == reg) {
      *pval = g_pconfig->m_firmware_sub_minor_version;
    }
    else if ((VSCP_STD_REGISTER_BOOT_LOADER + ADJSTDREG) == reg) {
      *pval = g_pconfig->m_bootloader_algorithm;
    }
    else if ((VSCP_STD_REGISTER_BUFFER_SIZE + ADJSTDREG) == reg) {
      *pval = 0; // Deprecated register value
    }
    else if ((VSCP_STD_REGISTER_PAGES_COUNT + ADJSTDREG) == reg) {
      *pval = 0; // Deprecated register value
    }
    else if (reg >= (VSCP_STD_REGISTER_FAMILY_CODE + ADJSTDREG) && (reg <= (VSCP_STD_REGISTER_FAMILY_CODE + ADJSTDREG + 3))) {
      *pval = *(&g_pconfig->m_standard_device_family_code + reg - VSCP_STD_REGISTER_FAMILY_CODE + ADJSTDREG);
    }
    else if (reg >= (VSCP_STD_REGISTER_DEVICE_TYPE + ADJSTDREG) && (reg <= (VSCP_STD_REGISTER_DEVICE_TYPE + ADJSTDREG + 3))) {
      *pval = *(&g_pconfig->m_standard_device_type_code + reg - VSCP_STD_REGISTER_DEVICE_TYPE + ADJSTDREG);
    }
    else if ((VSCP_STD_REGISTER_NODE_RESET + ADJSTDREG) == reg) {
      *pval = 0; // always read as zero
    }
    else if ((VSCP_STD_REGISTER_FIRMWARE_CODE_MSB + ADJSTDREG) == reg) {
      *pval = (g_pconfig->m_firmware_device_code >> 8) & 0xff;
    }
    else if ((VSCP_STD_REGISTER_FIRMWARE_CODE_LSB + ADJSTDREG) == reg) {
      *pval = g_pconfig->m_firmware_device_code & 0xff;
    }
    // * * * Unused register range * * *
    else if ((reg > (VSCP_STD_REGISTER_NICKNAME_ID_MSB + ADJSTDREG)) && (reg < (VSCP_STD_REGISTER_GUID) + ADJSTDREG)) {
      *pval = 0;
    }
    else if ((reg >= (VSCP_STD_REGISTER_GUID + ADJSTDREG)) && (reg < (VSCP_STD_REGISTER_GUID + ADJSTDREG + 16))) {
      *pval = g_pconfig->m_guid[reg - VSCP_STD_REGISTER_GUID + ADJSTDREG];
    }
    else if ((reg >= (VSCP_STD_REGISTER_DEVICE_URL + ADJSTDREG)) && (reg < (VSCP_STD_REGISTER_DEVICE_URL + ADJSTDREG + 16))) {
      *pval = g_pconfig->m_mdfurl[reg - VSCP_STD_REGISTER_DEVICE_URL + ADJSTDREG];
    }
    else {
      // This standard register is not available
    }
  } // standard register

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_write_reg
//

int
vscp_frmw2_write_reg(uint32_t reg, uint8_t val)
{
  int rv = VSCP_ERROR_ERROR; // To handle non existent registers

  if (reg < VSCP_STD_REGISTER_START + ADJSTDREG) {
    // User register - Let usr code handle
    rv = vscp_frmw2_callback_write_reg(g_pconfig->m_puserdata, 0, reg, val);
  }
  // * * * Standard registers * * *
  else if ((VSCP_STD_REGISTER_ALARM_STATUS + ADJSTDREG) == reg) {
    // Writing/reading alarm resets it regardless of value
    g_pconfig->m_alarm_status = 0;
    // Inform app. that standard register has been set to zero
    vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_ALARM_STATUS);
  }
  else if ((reg >= (VSCP_STD_REGISTER_USER_ID + ADJSTDREG)) && (reg <= VSCP_STD_REGISTER_USER_ID + ADJSTDREG)) {
    uint8_t* p = (uint8_t*)&g_pconfig->m_userId;
    // g_pconfig->m_userId[reg - VSCP_STD_REGISTER_USER_ID + ADJSTDREG] = val;
    p[reg - VSCP_STD_REGISTER_USER_ID + ADJSTDREG] = val;
    // Inform app that standard register has been changes
    rv = vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_USER_ID);
  }

  // Write manufacturer id configuration information
  else if ((reg >= (VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG)) && (reg <= (VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG + 3))) {
    if (g_pconfig->m_bEnableWriteProtectedLocations) {
      // page register must be 0xffff for writes to be possible
      if (0xffff != g_pconfig->m_page_select) {
        rv = VSCP_ERROR_NOT_SUPPORTED;
      }
      else {
        uint8_t* p                                            = (uint8_t*)&g_pconfig->m_manufacturerId;
        p[reg - VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG] = val;
        // g_pconfig->m_manufacturerId[reg - VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG] = val;
        rv = VSCP_ERROR_SUCCESS;
      }
      rv = vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_USER_MANDEV_ID);
    }
  }
  else if ((reg >= (VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG)) && (reg <= (VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG + 3))) {
    if (g_pconfig->m_bEnableWriteProtectedLocations) {
      // page register must be 0xffff for writes to be possible
      if (0xffff != g_pconfig->m_page_select) {
        rv = VSCP_ERROR_NOT_SUPPORTED;
      }
      else {
        uint8_t* p                                               = (uint8_t*)&g_pconfig->m_manufacturerSubId;
        p[reg - VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG] = val;
        // g_pconfig->m_manufacturerId[reg - VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG] = val;
        rv = VSCP_ERROR_SUCCESS;
      }
      rv = vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_USER_MANSUBDEV_ID);
    }
  }
  else if ((reg >= (VSCP_STD_REGISTER_GUID + ADJSTDREG)) && (reg <= (VSCP_STD_REGISTER_GUID + ADJSTDREG + 15))) {
    if (g_pconfig->m_bEnableWriteProtectedLocations) {
      if (0xffff != g_pconfig->m_page_select) {
        return VSCP_ERROR_NOT_SUPPORTED;
      }
      else {
        g_pconfig->m_guid[reg - VSCP_STD_REGISTER_GUID + ADJSTDREG] = val;
        rv                                                          = VSCP_ERROR_SUCCESS;
      }
      rv = vscp_frmw2_callback_stdreg_change(g_pconfig->m_puserdata, VSCP_STD_REGISTER_GUID);
    }
  }
  else if (reg == (VSCP_STD_REGISTER_NODE_RESET + ADJSTDREG)) {

    uint32_t timer;
    timer = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);

    if (0x55 == val) {
      g_pconfig->m_timer1 = timer;
    }
    else if (0xaa == val) {
      if ((timer - g_pconfig->m_timer1) < 1000) {
        g_pconfig->m_timer1 = 0;
        return vscp_frmw2_callback_restore_defaults(g_pconfig->m_puserdata);
      }
      else {
        rv = VSCP_ERROR_TIMEOUT;
      }
    }
  }

  return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_send_heartbeat
//

int
vscp_frmw2_send_heartbeat(void)
{
  int rv;

  vscpEventEx exrply;
  vscp_frmw2_setup_event_ex(&exrply);

  // Construct reply event
  if (VSCP_LEVEL1 == g_pconfig->m_level) {
    exrply.vscp_class = VSCP_CLASS1_INFORMATION;
    exrply.vscp_type  = VSCP_TYPE_INFORMATION_NODE_HEARTBEAT;
    exrply.sizeData   = 0;
  }
  else {
    // Level II should add node name in data
    exrply.vscp_class = VSCP_CLASS2_INFORMATION;
    exrply.vscp_type  = VSCP2_TYPE_INFORMATION_HEART_BEAT;
    memset(exrply.data, 0, 512);
    memcpy(exrply.data, g_pconfig->m_deviceName, 64);
    exrply.sizeData = strlen((const char*)exrply.data);
  }

  // Send event
  return vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_send_caps
//

int
vscp_frmw2_send_caps(void)
{
  vscpEventEx ex;
  vscp_frmw2_setup_event_ex(&ex);

  // Construct reply event
  ex.vscp_class = VSCP_CLASS2_PROTOCOL;
  ex.vscp_type  = VSCP2_TYPE_PROTOCOL_HIGH_END_SERVER_CAPS;

  // Data
  ex.sizeData = 104;
  memset(ex.data, 0, ex.sizeData);

  // Capabilities
  ex.data[0] = 0x40; // Node have a standard decision matrix.
  ex.data[1] = 0x00;
  ex.data[2] = 0x00;
  ex.data[3] = 0x00;
  ex.data[4] = 0x00;
  ex.data[5] = 0x00;
  ex.data[6] = 0x80; // Have VSCP TCP server with VCSP link interface.
  ex.data[7] = 0x38; // IP4 support, SLL support
                     // Accepts two or more simultaneous connections on TCP/IP interface.

  // GUID
  memcpy(ex.data + 8, g_pconfig->m_guid, 16);

  // ip address
  memcpy((ex.data + 24), g_pconfig->m_ipaddr, 16);

  // Device name
  memcpy((ex.data + 40), g_pconfig->m_deviceName, 64);

  // Send event
  return vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_send_high_end_server_probe
//

int
vscp_frmw2_send_high_end_server_probe(void)
{
  vscpEventEx ex;
  vscp_frmw2_setup_event_ex(&ex);

  // Construct reply event
  vscp_frmw2_setup_event_ex(&ex);
  ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  ex.vscp_type  = VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_PROBE;
  ex.sizeData   = 0;

  // Data
  ex.sizeData = 0;

  // Send event
  return vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_send_high_end_server_probe
//

int
vscp_frmw2_page_read(uint32_t offset, uint8_t count)
{
  int rv = VSCP_ERROR_SUCCESS;
  uint8_t i;
  uint8_t seq = 0;
  uint8_t val;

  vscpEventEx rplyex;
  vscp_frmw2_setup_event_ex(&rplyex);

  for (i = 0; i < count; i++) {

    vscp_frmw2_read_reg(offset + i, &val);
    rplyex.data[(i % 7) + 1] = val;

    if ((i % 7) == 6 || i == (count - 1)) {
      uint8_t nsize;
      if ((i % 7) == 6) {
        nsize = 7;
      }
      else {
        nsize = (i % 7) + 1;
      }

      rplyex.vscp_class = VSCP_CLASS1_PROTOCOL;
      rplyex.vscp_type  = VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE;
      rplyex.sizeData   = nsize + 1;
      rplyex.data[0]    = seq;

      // send the event
      if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &rplyex))) {
        return rv;
      }

      offset++;
    }
  }

  return rv;
}

/*!
  @brief Do a level I page write

  @param offset Offset into register page
  @param count Number of registers to write
  @param buf Pointer to buffer of
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp_frmw2_page_write(uint32_t offset, uint8_t count, const uint8_t* const pbuf)
{
  int rv;
  uint8_t i;
  uint8_t val;

  vscpEventEx ex;
  vscp_frmw2_setup_event_ex(&ex);

  for (i = 0; i < count; i++) {

    // Write VSCP register
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_write_reg(offset + i, pbuf[i]))) {
      return rv;
    }

    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(i, &val))) {
      return rv;
    }

    ex.data[1 + i] = val;
  }

  ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  ex.vscp_type  = VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE;
  ex.sizeData   = count + 1;
  ex.data[0]    = 0; // index

  // send the event
  if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex))) {
    return rv;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_frmw2_whois_response
//

int
vscp_frmw2_whois_response(void)
{
  int rv;
  uint8_t i, j, k = 0;

  vscpEventEx ex;
  vscp_frmw2_setup_event_ex(&ex);

  if (VSCP_LEVEL1 == g_pconfig->m_level) {

    ex.vscp_class = VSCP_CLASS1_PROTOCOL;
    ex.vscp_type  = VSCP_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE;
    ex.sizeData   = 8;

    for (i = 0; i < 3; i++) // fill up with GUID
    {
      ex.data[0] = i;

      for (j = 1; j < 8; j++) {
        ex.data[j] = g_pconfig->m_guid[15 - k++];
        if (k > 16)
          break;
      }

      if (k > 16)
        break;

      // send the event
      if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex))) {
        return rv;
      }
    }

    for (j = 0; j < 5; j++) // fill up previous event with MDF
    {
      if (g_pconfig->m_mdfurl[j] > 0)
        ex.data[3 + j] = g_pconfig->m_mdfurl[j];
      else
        ex.data[3 + j] = 0;
    }

    // send the event
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex))) {
      return rv;
    }

    k = 5;                  // start offset
    for (i = 3; i < 7; i++) // fill up with the rest of GUID
    {
      ex.data[0] = i;

      for (j = 1; j < 8; j++) {
        ex.data[j] = g_pconfig->m_mdfurl[k++];
      }
      // send the event
      if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex))) {
        return rv;
      }
    }
  }
  // Level II
  else {

    ex.vscp_class = VSCP_CLASS2_PROTOCOL;
    ex.vscp_type  = VSCP2_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE;
    ex.sizeData   = 16 + MIN(strlen((const char*)g_pconfig->m_mdfurl), 32);
    memset(ex.data, 9, 512);
    memcpy(ex.data, g_pconfig->m_guid, 16);
    if (g_pconfig->m_mdfurl[0]) {
      memcpy(ex.data + 16, g_pconfig->m_mdfurl, MIN(strlen((const char*)g_pconfig->m_mdfurl), 32));
    }

    // send the event
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex))) {
      return rv;
    }
  }
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_frmw2_extended_page_read
//

int
vscp_frmw2_extended_page_read(uint16_t nodeid, uint16_t page, uint8_t startoffset, uint16_t cnt)
{
  int rv;
  uint16_t page_save;
  uint8_t byte = 0;
  uint8_t bytes_this_time, cb;

  vscpEventEx ex;
  vscp_frmw2_setup_event_ex(&ex);

  // Save the current page
  page_save = g_pconfig->m_page_select;

  // set temporary page
  g_pconfig->m_page_select = page;

  // Construct response event
  vscp_frmw2_setup_event_ex(&ex);
  ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  ex.vscp_type  = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;
  ex.data[0]    = 0; // index of event, this is the first
  ex.data[1]    = (page >> 8) & 0xff;
  ex.data[2]    = page & 0xff;

  do {
    // calculate bytes to transfer in this event
    if ((cnt - byte) >= 4) {
      bytes_this_time = 4;
    }
    else {
      bytes_this_time = (cnt - byte);
    }

    // define length of this event
    ex.sizeData = 4 + bytes_this_time;
    ex.data[3]  = startoffset + byte; // first register in this event

    // Put up to four registers to data space
    for (cb = 0; cb < bytes_this_time; cb++) {
      uint8_t val;
      if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(startoffset + byte + cb, &val))) {
        // Restore the saved page
        g_pconfig->m_page_select = page_save;
        return rv;
      }
      ex.data[(4 + cb)] = val;
    }

    // Location is important as sending use microseconds to
    uint32_t tm = vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata);

    // send the event
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex))) {
      // Restore the saved page
      g_pconfig->m_page_select = page_save;
      return rv;
    }

    // wait at bit before next message is sent
    // prevent overflow of bus or receiver
    // 100 usec should be sufficient, but requires separate timer
    while (vscp_frmw2_callback_get_timestamp(g_pconfig->m_puserdata) < (tm + 100)) {
      ;
    }

    // increment byte by bytes_this_time and the event number by one
    byte += bytes_this_time;

    // increment the index
    ex.data[0] += 1;

  } while (byte < cnt);

  // Restore the saved page
  g_pconfig->m_page_select = page_save;
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_frmw2_extended_page_read
//

int
vscp_frmw2_extended_page_write(uint16_t nodeid,
                               uint16_t page,
                               uint8_t offset,
                               uint16_t cnt,
                               const uint8_t* const pbuf)
{
  int rv = VSCP_ERROR_SUCCESS;
  uint8_t i;
  uint16_t page_save;

  // Construct response event
  vscpEventEx ex;
  vscp_frmw2_setup_event_ex(&ex);
  ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  ex.vscp_type  = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;

  // Save the current page
  page_save = g_pconfig->m_page_select;

  // Assign the requested page
  g_pconfig->m_page_select = page;

  for (i = 0;
       i < cnt;
       i++) {
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_write_reg(offset + i, pbuf[i]))) {
      g_pconfig->m_page_select = page_save;
      return rv;
    }
  }

  // Restore the saved page
  g_pconfig->m_page_select = page_save;

  return vscp_frmw2_extended_page_read(nodeid, page, offset, cnt);

  // ex.sizeData   = 4 + cnt;
  // ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  // ex.vscp_type  = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;
  // ex.data[0]    = 0;                  // index (only one event)
  // ex.data[1]    = (page >> 8) & 0xff; // mirror page msb
  // ex.data[2]    = page & 0xff;        // mirror page lsb
  // ex.data[3]    = offset;             // Register base offset

  // // send the event
  // return vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &ex);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_report_dmatrix
//

int
vscp_frmw2_report_dmatrix(void)
{
  vscpEventEx exrply;
  vscp_frmw2_setup_event_ex(&exrply);

  if (VSCP_LEVEL1 == g_pconfig->m_level) {
    exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
    exrply.vscp_type  = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;

    exrply.sizeData = 4;
    exrply.data[0]  = g_pconfig->m_nDmRows;     // Size = number of DM rows
    exrply.data[1]  = g_pconfig->m_regOffsetDm; // Start offset in page of DM
    exrply.data[2]  = (g_pconfig->m_pageDm >> 8) & 0xff;
    exrply.data[3]  = g_pconfig->m_pageDm & 0xff;
  }
  else {
    // Level II response
    exrply.vscp_class = VSCP_CLASS1_PROTOCOL;
    exrply.vscp_type  = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;

    exrply.sizeData = 22;
    memcpy(exrply.data, g_pconfig->m_guid, 16); // GUID
    exrply.data[16] = g_pconfig->m_sizeDmRow;   // Row size
    exrply.data[17] = g_pconfig->m_nDmRows;     // Size = number of DM rows
    exrply.data[18] = (g_pconfig->m_regOffsetDm >> 24) & 0xff;
    exrply.data[19] = (g_pconfig->m_regOffsetDm >> 16) & 0xff;
    exrply.data[20] = (g_pconfig->m_regOffsetDm >> 8) & 0xff;
    exrply.data[21] = g_pconfig->m_regOffsetDm & 0xff;
  }

  // send the event
  return vscp_frmw2_callback_send_event_ex(g_pconfig->m_puserdata, &exrply);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_feed_level1_dm
//
// oaddr  flags  class-mask  class-filter  type-mask  type-filter  action  action-parameter
//

int
vscp_frmw2_feed_level1_dm(const vscpEventEx* const pex)
{
  int rv = VSCP_ERROR_SUCCESS;

  for (uint8_t row = 0; row < g_pconfig->m_nDmRows; row++) {

    // Is the DM row enabled?
    if (LEVEL1_DM_FLAGS(row) & VSCP_LEVEL1_DM_FLAG_ENABLED) {

      // Should the originating id be checked and if so is it the same?
      if ((LEVEL1_DM_FLAGS(row) & VSCP_LEVEL1_DM_FLAG_CHECK_OADDR) &&
          (pex->GUID[15] != LEVEL1_DM_OADDR(row))) {
        continue;
      }

      // Check if zone should match and if so if it match (255 is wildcard)
      if (LEVEL1_DM_FLAGS(row) & VSCP_LEVEL1_DM_FLAG_CHECK_ZONE) {
        if (255 != pex->data[1]) {
          if (pex->data[1] != g_pconfig->m_zone) {
            continue;
          }
        }
      }

      // Check if subzone should match and if so if it match (255 is wildcard)
      if (LEVEL1_DM_FLAGS(row) & VSCP_LEVEL1_DM_FLAG_CHECK_SUBZONE) {
        if (255 != pex->data[2]) {
          if (pex->data[2] != g_pconfig->m_subzone) {
            continue;
          }
        }
      }

      if (!((LEVEL1_DM_CLASS_FILTER(row) ^ pex->vscp_class) & LEVEL1_DM_CLASS_MASK(row)) &&
          !((LEVEL1_DM_TYPE_FILTER(row) ^ pex->vscp_type) & LEVEL1_DM_TYPE_MASK(row))) {

        // OK Trigger this action
        if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_dm_action(g_pconfig->m_puserdata,
                                                                      pex,
                                                                      LEVEL1_DM_ACTION(row),
                                                                      &LEVEL1_DM_ACTION_PARAM(row)))) {
          break;
        }

      } // Filter/mask
    } // Row enabled
  } // for each row
  return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_feed_level2_dm
//
// control  class  type  origin  action  action-parameter
//

int
vscp_frmw2_feed_level2_dm(const vscpEventEx* const pex)
{
  int rv = VSCP_ERROR_SUCCESS;

  for (uint8_t row = 0; row < g_pconfig->m_nDmRows; row++) {

    // End of matrix?
    if (LEVEL2_DM_FLAGS(row) & VSCP_LEVEL2_DM_FLAG_END_MARK) {
      break;
    }

    // Is the DM row enabled?
    if (!(LEVEL2_DM_FLAGS(row) & VSCP_LEVEL2_DM_FLAG_ENABLED)) {
      continue;
    }

    // Should the originating id be checked and if so is it the same?
    if ((LEVEL2_DM_FLAGS(row) & VSCP_LEVEL1_DM_FLAG_CHECK_OADDR) &&
          (pex->GUID[0] != LEVEL2_DM_OADDR(row, 0)) ||
        (pex->GUID[1] != LEVEL2_DM_OADDR(row, 1)) ||
        (pex->GUID[2] != LEVEL2_DM_OADDR(row, 2)) ||
        (pex->GUID[3] != LEVEL2_DM_OADDR(row, 3)) ||
        (pex->GUID[4] != LEVEL2_DM_OADDR(row, 4)) ||
        (pex->GUID[5] != LEVEL2_DM_OADDR(row, 5)) ||
        (pex->GUID[6] != LEVEL2_DM_OADDR(row, 6)) ||
        (pex->GUID[7] != LEVEL2_DM_OADDR(row, 7)) ||
        (pex->GUID[8] != LEVEL2_DM_OADDR(row, 8)) ||
        (pex->GUID[9] != LEVEL2_DM_OADDR(row, 9)) ||
        (pex->GUID[10] != LEVEL2_DM_OADDR(row, 10)) ||
        (pex->GUID[11] != LEVEL2_DM_OADDR(row, 11)) ||
        (pex->GUID[12] != LEVEL2_DM_OADDR(row, 12)) ||
        (pex->GUID[13] != LEVEL2_DM_OADDR(row, 13)) ||
        (pex->GUID[14] != LEVEL2_DM_OADDR(row, 14)) ||
        (pex->GUID[15] != LEVEL2_DM_OADDR(row, 15))) {
      continue;
    }

    // Check if index should match and if so if it match (255 is wildcard)
    if (LEVEL2_DM_FLAGS(row) & VSCP_LEVEL2_DM_FLAG_CHECK_INDEX) {
      if (255 != pex->data[1]) {
        if (pex->data[1] != g_pconfig->m_index) {
          continue;
        }
      }
    }

    // Check if zone should match and if so if it match (255 is wildcard)
    if (LEVEL2_DM_FLAGS(row) & VSCP_LEVEL2_DM_FLAG_CHECK_ZONE) {
      if (255 != pex->data[1]) {
        if (pex->data[1] != g_pconfig->m_zone) {
          continue;
        }
      }
    }

    // Check if subzone should match and if so if it match (255 is wildcard)
    if (LEVEL2_DM_FLAGS(row) & VSCP_LEVEL2_DM_FLAG_CHECK_SUBZONE) {
      if (255 != pex->data[2]) {
        if (pex->data[2] != g_pconfig->m_subzone) {
          continue;
        }
      }
    }

    // OK Trigger this action
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_callback_dm_action(g_pconfig->m_puserdata,
                                                                  pex,
                                                                  LEVEL2_DM_ACTION(row),
                                                                  &LEVEL2_DM_ACTION_PARAM(row)))) {
      break;
    }

  } // for each row
  return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_feed_dm
//

int
vscp_frmw2_feed_dm(const vscpEventEx* const pex)
{
  int rv = VSCP_ERROR_SUCCESS;

  if (NULL != pex) {
    if (VSCP_LEVEL1 == g_pconfig->m_level) {
      rv = vscp_frmw2_feed_level1_dm(pex);
    }
    else {
      rv = vscp_frmw2_feed_level1_dm(pex);
    }
  }

  return rv;
}