/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
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
#include <stdlib.h>
#include <string.h>
#include <vscp-class.h>
#include <vscp-firmware-helper.h>
#include <vscp-type.h>
#include <vscp.h>

#include "vscp-firmware-level2.h"

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#define unused(x) ((void) x)

/* Constants */

/* Globals */

// ---------------------------------------------------------------------------
// Safe memory copy: copies min(src_size, dst_size) bytes to ensure the
// destination can always hold the source data (CWE-120 mitigation).
// ---------------------------------------------------------------------------
static size_t
vscp_frmw2_memcpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
  size_t n         = (src_size < dst_size) ? src_size : dst_size;
  uint8_t *d       = (uint8_t *) dst;
  const uint8_t *s = (const uint8_t *) src;
  for (size_t i = 0; i < n; i++) {
    d[i] = s[i];
  }
  return n;
}

// ---------------------------------------------------------------------------
// Validate that pctx and all mandatory ops callbacks are non-NULL.
// Mandatory: get_milliseconds, get_timestamp, send_event, set_event_time,
//            read_reg, write_reg, stdreg_change
// ---------------------------------------------------------------------------
static inline int
vscp_frmw2_check_ctx(const vscp_frmw2_firmware_context_t *pctx)
{
  if (NULL == pctx || NULL == pctx->ops || NULL == pctx->ops->get_milliseconds || NULL == pctx->ops->get_timestamp ||
      NULL == pctx->ops->send_event || NULL == pctx->ops->set_event_time || NULL == pctx->ops->read_reg ||
      NULL == pctx->ops->write_reg || NULL == pctx->ops->stdreg_change) {
    return VSCP_ERROR_INIT_MISSING;
  }
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_init
//

int
vscp_frmw2_init(vscp_frmw2_firmware_context_t *const pctx)
{
  int rv;

  if (VSCP_ERROR_SUCCESS != vscp_frmw2_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }

  pctx->alarm_status       = 0; // No alarms yet
  pctx->errorCounter       = 0; // No errors yet
  pctx->page_select        = 0;
  pctx->reset_device_flags = 0; // VSCP_TYPE_PROTOCOL_RESET_DEVICE

  /* NOTE: This function owns evrply.pdata; it is freed on all return paths. */
  vscp_event_t evrply;
  evrply.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == evrply.pdata) {
    return VSCP_ERROR_MEMORY;
  }
  vscp_frmw2_setup_event(pctx, &evrply);

  if (VSCP_LEVEL1 == pctx->level) {
    /*!
      We do not use the GUID in level I for communication
      instead we use a 8-bit or 16-bit nickname.
    */
    // pctx->guid[14] = (pctx->nickname >> 8) & 0xff;
    // pctx->guid[15] = pctx->nickname & 0xff;

    // We are in limbo still
    pctx->state = FRMW2_STATE_NONE;

    if (VSCP_FRMW2_UNASSIGNED == pctx->probe_timeout) {
      // Set default
      pctx->probe_timeout = VSCP_PROBE_TIMEOUT;
    }

    if (VSCP_FRMW2_UNASSIGNED == pctx->probe_timeout_count) {
      // Set default
      pctx->probe_timeout_count = VSCP_PROBE_TIMEOUT_COUNT;
    }

    if (0xffff == pctx->nickname) {

      pctx->probe_nickname = 0; // First check for a segment controller

      // Get a starttime
      pctx->timer = pctx->ops->get_milliseconds(pctx);

      // Send probe
      vscp_frmw2_setup_event(pctx, &evrply);
      evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
      evrply.vscp_type  = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
      if (pctx->bUse16BitNickname) {
        evrply.sizeData = 2;
        evrply.pdata[0] = 0; // We probe segment controller
        evrply.pdata[1] = 0;
      }
      else {
        evrply.sizeData = 1;
        evrply.pdata[0] = 0; // We probe segment controller
      }

      // Send new node online
      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &evrply))) {
        free(evrply.pdata);
        return rv;
      }

      // We are now in probe state (waiting for segment controller probe)
      pctx->state    = FRMW2_STATE_PROBE;
      pctx->substate = FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING;
    }
    else {
      vscp_frmw2_send_probe(pctx, OPT_NEW_NODE_ONLINE);

      // Send heartbeat
      pctx->last_heartbeat = pctx->ops->get_milliseconds(pctx);
      vscp_frmw2_send_heartbeat(pctx);

      // Send caps
      if ((VSCP_LEVEL2 == pctx->level)) {
        pctx->last_caps = pctx->ops->get_milliseconds(pctx);
        vscp_frmw2_send_caps(pctx);
      }

      pctx->state = FRMW2_STATE_ACTIVE;
    }
  }
  else {
    // In level 2 we use the set GUID

    // We go to active right away if level II
    pctx->state = FRMW2_STATE_ACTIVE;
  }

  free(evrply.pdata);
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_send_probe
//

int
vscp_frmw2_send_probe(vscp_frmw2_firmware_context_t *pctx, int bNewNodeOnLine)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  /* NOTE: This function owns ev.pdata; it is freed on all return paths. */
  vscp_event_t ev;
  ev.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == ev.pdata) {
    return VSCP_ERROR_MEMORY;
  }

  // Send probe
  vscp_frmw2_setup_event(pctx, &ev);
  ev.vscp_class = VSCP_CLASS1_PROTOCOL;
  ev.vscp_type  = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;

  if (bNewNodeOnLine) {
    ev.sizeData = 0;
  }
  else {
    if (pctx->bUse16BitNickname) {
      ev.sizeData = 2;
      ev.pdata[0] = (pctx->nickname >> 8) & 0xff;
      ev.pdata[1] = pctx->nickname & 0xff;
    }
    else {
      ev.sizeData = 1;
      ev.pdata[0] = pctx->nickname;
    }
  }

  // Send probe
  int result = pctx->ops->send_event(pctx, &ev);
  free(ev.pdata);
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_nickname_discovery
//

int
vscp_frmw2_nickname_discovery(vscp_frmw2_firmware_context_t *pctx, const vscpEvent *const pex)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv = VSCP_ERROR_SUCCESS;

  // If there is an event we should check if we should react
  // on it
  if (NULL != pex) {

    if (pex->sizeData > 0 && NULL == pex->pdata) {
      return VSCP_ERROR_INVALID_POINTER;
    }

    if ((VSCP_CLASS1_PROTOCOL == pex->vscp_class) && (VSCP_TYPE_PROTOCOL_PROBE_ACK == pex->vscp_type)) {
      // No check for nickname if we are probing for as egment controller
      if (FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING != pctx->substate) {
        if (pctx->bUse16BitNickname) {
          // 16-bit nickname
          if (construct_unsigned16(pex->GUID[14], pex->GUID[15]) != pctx->probe_nickname) {
            // not to us
            return VSCP_ERROR_SUCCESS;
          }
        }
        else {
          // 8-bit nickname
          if (pex->GUID[15] != pctx->probe_nickname) {
            // Not to us
            return VSCP_ERROR_SUCCESS;
          }
        }
      }

      // We got a response from a node with the same nickname
      if (FRMW2_STATE_PROBE == pctx->state) {
        if (FRMW2_SUBSTATE_NONE == pctx->substate) {
          ; // We do nothing
        }
        else if (FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING == pctx->substate) {
          // Segment controller responded
          pctx->state  = FRMW2_STATE_PREACTIVE;
          pctx->timer = pctx->ops->get_milliseconds(pctx);
        }
        else if (FRMW2_SUBSTATE_PROBE_WAITING == pctx->substate) {

          // Try next nickname
          pctx->probe_nickname++;

          // reset test time and count
          pctx->probe_timeout_count = 0;
          pctx->timer              = pctx->ops->get_milliseconds(pctx);

          // Check to see if we have tested all nicknames
          if ((pctx->bUse16BitNickname && (0xffff == pctx->nickname)) ||
              (!pctx->bUse16BitNickname && (0xff == pctx->nickname))) {
            // Nothing more we can do.
            pctx->state = FRMW2_STATE_ERROR;
          }
        }
      }
    }
  }
  else {
    // No event
    if (FRMW2_STATE_PROBE == pctx->state) {
      if (FRMW2_SUBSTATE_NONE == pctx->substate) {
        pctx->probe_timeout_count = 0;
      SEND_PROBE:
        pctx->timer = pctx->ops->get_milliseconds(pctx);

        // Send probe
        if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_send_probe(pctx, OPT_PROBE))) {
          return rv;
        }
        if (pctx->probe_nickname) {
          // Standard probe
          pctx->substate = FRMW2_SUBSTATE_PROBE_WAITING;
        }
        else {
          // If we probe the segment controller (nickname = 0) - Longer timeout
          pctx->substate = FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING;
        }
      }
      else if (FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING == pctx->substate) {
        // Timeout
        if ((pctx->ops->get_milliseconds(pctx) - pctx->timer) > VSCP_SEGCTRL_PROBE_TIMEOUT) {
          // revert to normal probe
          pctx->probe_nickname      = 1; // Start with first non segment ctrl node
          pctx->substate            = FRMW2_SUBSTATE_PROBE_WAITING;
          pctx->probe_timeout_count = 0;
          pctx->timer              = pctx->ops->get_milliseconds(pctx);
          goto SEND_PROBE;
        }
      }
      else if (FRMW2_SUBSTATE_PROBE_WAITING == pctx->substate) {
        // Timeout
        if ((pctx->ops->get_milliseconds(pctx) - pctx->timer) > VSCP_PROBE_TIMEOUT) {
          // If we have probed three times we are done
          if (pctx->probe_timeout_count >= VSCP_PROBE_TIMEOUT_COUNT) {

            // We use this nickname
            pctx->nickname = pctx->probe_nickname;

            pctx->state    = FRMW2_STATE_ACTIVE;
            pctx->substate = FRMW2_SUBSTATE_NONE;
            // Set id
            // pctx->guid[15] = pctx->nickname & 0xff;
            // pctx->guid[14] = (pctx->nickname >> 8) & 0xff;
            return vscp_frmw2_send_probe(pctx, OPT_NEW_NODE_ONLINE);
          }
          else {
            pctx->probe_timeout_count++;
            pctx->timer = pctx->ops->get_milliseconds(pctx);
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
vscp_frmw2_nickname_wait(vscp_frmw2_firmware_context_t *pctx, const vscpEvent *const pex)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  /* NOTE: Each event allocated within this function (evrply/evrply2) is freed within its own block before return. */
  int rv = VSCP_ERROR_SUCCESS;

  // If segment controller does not respond withing set timeout period
  // go back to probe state
  if ((pctx->ops->get_milliseconds(pctx) - pctx->timer) > VSCP_SEGCTRL_RESPONSE_TIMEOUT) {
    pctx->nickname = 1; // Skip probing of segment controller this time
    pctx->state    = FRMW2_STATE_NONE;
    pctx->substate = FRMW2_SUBSTATE_NONE;
    return VSCP_ERROR_SUCCESS;
  }

  // If no event to handle noting more to do for now
  if (NULL == pex) {
    return VSCP_ERROR_SUCCESS;
  }

  if (pex->sizeData > 0 && NULL == pex->pdata) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (VSCP_CLASS1_PROTOCOL == pex->vscp_class) {

    if (VSCP_TYPE_PROTOCOL_SET_NICKNAME == pex->vscp_type) {
      // 8-bit nickname
      if (2 == ADJSIZEX) {
        // Addressed to us?
        if (construct_unsigned16(0, EXDTA(0)) == pctx->nickname) {

          // Set nickname
          pctx->nickname = EXDTA(1);

          // Inform app. that nickname changed
          pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_NICKNAME_ID_MSB);
          pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_NICKNAME_ID_LSB);

          vscp_event_t evrply;
          evrply.pdata = calloc(VSCP_MAX_DATA, 1);
          if (NULL == evrply.pdata) {
            return VSCP_ERROR_MEMORY;
          }
          vscp_frmw2_setup_event(pctx, &evrply);
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_NICKNAME_ACCEPTED;
          rv                = pctx->ops->send_event(pctx, &evrply);
          free(evrply.pdata);
        }
      }
      // 16-bit nickname
      else if (4 == ADJSIZEX) {
        // Addressed to us?
        if (construct_unsigned16(EXDTA(0), EXDTA(1)) == pctx->nickname) {

          // Set nickname
          pctx->nickname = construct_unsigned16(EXDTA(2), EXDTA(3));

          // Inform app. that nickname changed
          pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_NICKNAME_ID_MSB);
          pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_NICKNAME_ID_LSB);

          vscp_event_t evrply2;
          evrply2.pdata = calloc(VSCP_MAX_DATA, 1);
          if (NULL == evrply2.pdata) {
            return VSCP_ERROR_MEMORY;
          }
          vscp_frmw2_setup_event(pctx, &evrply2);
          evrply2.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply2.vscp_type  = VSCP_TYPE_PROTOCOL_NICKNAME_ACCEPTED;
          rv                 = pctx->ops->send_event(pctx, &evrply2);
          free(evrply2.pdata);
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
vscp_frmw2_work(vscp_frmw2_firmware_context_t *pctx, const vscpEvent *const pex)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv = VSCP_ERROR_SUCCESS;

  // Check for init button press   TODO

  pctx->vscp_class = 0;
  pctx->offset     = 0;
  memset(pctx->ifguid, 0, 16);

  if (NULL != pex) {

    // If there is an event we should check if we should react
    // on it

    pctx->vscp_class = pex->vscp_class;

    /*
      Check if we have a proxy event, if we have
      remove GUID and make it a standard level I event
    */
    if ((pex->vscp_class >= 512) && (pex->vscp_class < 1024)) {
      if (NULL != pex->pdata && pex->sizeData >= sizeof(pctx->ifguid)) {
        memcpy(pctx->ifguid, pex->pdata, sizeof(pctx->ifguid)); // Save interface GUID
      }
      pctx->offset = 16;       // Data is at offset 16
      pctx->vscp_class -= 512; // Make it a standard level I event
    }
  }

  switch (pctx->state) {

    // Just go to probe
    case FRMW2_STATE_NONE:
      pctx->state    = FRMW2_STATE_PROBE;
      pctx->substate = FRMW2_SUBSTATE_NONE;
      return vscp_frmw2_nickname_discovery(pctx, NULL);

    // Probe has been sent (waiting for answer)
    case FRMW2_STATE_PROBE:
      return vscp_frmw2_nickname_discovery(pctx, pex);

    // Happens when segment controller take probe. Wait for nickname
    case FRMW2_STATE_PREACTIVE:
      return vscp_frmw2_nickname_wait(pctx, pex);

    // Normal working state
    case FRMW2_STATE_ACTIVE:
      if (NULL != pex) {
        return vscp_frmw2_handle_protocol_event(pctx, pex);
      }
      break;

    // No free address or other error.
    case FRMW2_STATE_ERROR:
      break;
  }

  if (pctx->interval_heartbeat &&
      (pctx->ops->get_milliseconds(pctx) - pctx->last_heartbeat) > pctx->interval_heartbeat) {
    // Send heartbeat
    pctx->last_heartbeat = pctx->ops->get_milliseconds(pctx);
    vscp_frmw2_send_heartbeat(pctx);
  }

  if ((VSCP_LEVEL2 == pctx->level) && pctx->interval_caps &&
      (pctx->ops->get_milliseconds(pctx) - pctx->last_caps) > pctx->interval_caps) {
    // Send caps
    pctx->last_caps = pctx->ops->get_milliseconds(pctx);
    vscp_frmw2_send_caps(pctx);
  }

  // Check if the DM is interested in this event
  if (NULL != pex) {
    vscp_frmw2_feed_dm(pctx, pex);
  }

  return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_handle_protocol_event
//

int
vscp_frmw2_handle_protocol_event(vscp_frmw2_firmware_context_t *pctx, const vscpEvent *const pex)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  /* NOTE: Each switch-case block that allocates event.pdata with calloc is responsible
   *       for freeing it before exiting the case (including early-return error paths). */
  int rv = VSCP_ERROR_SUCCESS;

  if (NULL == pex) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (pex->sizeData > 0 && NULL == pex->pdata) {
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
          if (construct_unsigned16(EXDTA(0), EXDTA(1)) == pctx->nickname) {
            // Confirm probe
            vscp_event_t evrply;
            evrply.pdata = calloc(VSCP_MAX_DATA, 1);
            if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
            vscp_frmw2_setup_event(pctx, &evrply);
            evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
            evrply.vscp_type  = VSCP_TYPE_PROTOCOL_PROBE_ACK;
            rv                = pctx->ops->send_event(pctx, &evrply);
            free(evrply.pdata);
          }
        }
        else if (2 == ADJSIZEX) {
          // If addressed to us?
          if (construct_unsigned16(EXDTA(0), EXDTA(1)) == pctx->nickname) {
            // Confirm probe
            vscp_event_t evrply;
            evrply.pdata = calloc(VSCP_MAX_DATA, 1);
            if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
            vscp_frmw2_setup_event(pctx, &evrply);
            evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
            evrply.vscp_type  = VSCP_TYPE_PROTOCOL_PROBE_ACK;
            rv                = pctx->ops->send_event(pctx, &evrply);
            free(evrply.pdata);
          }
        }
        else if (16 == ADJSIZEX) {
          // 16-bit node id
          if (0 == memcmp(pctx->guid, &EXDTA(0), 16)) {
            vscp_event_t evrply;
            evrply.pdata = calloc(VSCP_MAX_DATA, 1);
            if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
            vscp_frmw2_setup_event(pctx, &evrply);
            evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
            evrply.vscp_type  = VSCP_TYPE_PROTOCOL_PROBE_ACK;
            rv                = pctx->ops->send_event(pctx, &evrply);
            free(evrply.pdata);
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
          if (construct_unsigned16(0, EXDTA(0)) == pctx->nickname) {

            // Set new nickname
            pctx->nickname = EXDTA(1);

            // Inform app. that nickname changed
            pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_NICKNAME_ID_MSB);
            pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_NICKNAME_ID_LSB);

            vscp_event_t evrply;
            evrply.pdata = calloc(VSCP_MAX_DATA, 1);
            if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
            vscp_frmw2_setup_event(pctx, &evrply);
            evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
            evrply.vscp_type  = VSCP_TYPE_PROTOCOL_NICKNAME_ACCEPTED;
            rv                = pctx->ops->send_event(pctx, &evrply);
            free(evrply.pdata);
          }
        }
        // 16-bit nickname
        else if (4 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(EXDTA(0), EXDTA(1)) == pctx->nickname) {

            // Set new nickname
            pctx->nickname = construct_unsigned16(EXDTA(2), EXDTA(3));

            // Inform app. that nickname changed
            pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_NICKNAME_ID_MSB);
            pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_NICKNAME_ID_LSB);

            vscp_event_t evrply2;
            evrply2.pdata = calloc(VSCP_MAX_DATA, 1);
            if (NULL == evrply2.pdata) { rv = VSCP_ERROR_MEMORY; break; }
            vscp_frmw2_setup_event(pctx, &evrply2);
            evrply2.vscp_class = VSCP_CLASS1_PROTOCOL;
            evrply2.vscp_type  = VSCP_TYPE_PROTOCOL_NICKNAME_ACCEPTED;
            rv                 = pctx->ops->send_event(pctx, &evrply2);
            free(evrply2.pdata);
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
          if (construct_unsigned16(0, EXDTA(0)) == pctx->nickname) {

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
              pctx->ops->feed_watchdog(pctx);
            }
          }

          // Restore defaults
          if (flags & 0x40) {
            pctx->ops->restore_defaults(pctx);
          }

          // Start over and find nickname again...
          if (!(flags & 0x20)) {
            pctx->nickname = 0xffff;
          }

          // If we should delay before restart
          if (tm) {
            pctx->timer = pctx->ops->get_milliseconds(pctx);
            while ((pctx->ops->get_milliseconds(pctx) - pctx->timer) < tm) {
              // Feed watchdog
              pctx->ops->feed_watchdog(pctx);
            }
          }
          vscp_frmw2_init(pctx);
        }
      } break;

      case VSCP_TYPE_PROTOCOL_READ_REGISTER: {

        uint8_t val;
        uint8_t reg;

        // 8-bit id
        if (2 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(0, EXDTA(0)) == pctx->nickname) {
            reg = EXDTA(1);
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(pctx, reg, &val))) {
              ;
            }
          }
          else {
            // Not to us
            return VSCP_ERROR_SUCCESS;
          }
        }
        // 16-bit id
        else if (3 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(EXDTA(0), EXDTA(1)) == pctx->nickname) {
            reg = EXDTA(2);
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(pctx, reg, &val))) {
              ;
            }
          }
          else {
            // Not to us
            return VSCP_ERROR_SUCCESS;
          }
        }
        else {
          return VSCP_ERROR_INVALID_FRAME;
        }

        // Send response
        if (VSCP_ERROR_SUCCESS == rv) {
          vscp_event_t evrply;
          evrply.pdata = calloc(VSCP_MAX_DATA, 1);
          if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
          vscp_frmw2_setup_event(pctx, &evrply);
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          evrply.sizeData   = 2;
          evrply.pdata[0]   = reg;
          evrply.pdata[1]   = val;
          rv                = pctx->ops->send_event(pctx, &evrply);
          free(evrply.pdata);
        }
      } break;

      case VSCP_TYPE_PROTOCOL_WRITE_REGISTER: {

        uint8_t val;
        uint8_t reg;

        // 8-bit id
        if (3 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(0, EXDTA(0)) == pctx->nickname) {
            reg = EXDTA(1);
            val = EXDTA(2);
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_write_reg(pctx, reg, val))) {
              ;
            }
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(pctx, reg, &val))) {
              ;
            }
          }
          else {
            // Not to us
            return VSCP_ERROR_SUCCESS;
          }
        }
        // 16-bit id
        else if (4 == ADJSIZEX) {
          // Addressed to us?
          if (construct_unsigned16(EXDTA(0), EXDTA(1)) == pctx->nickname) {
            reg = EXDTA(2);
            val = EXDTA(3);
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_write_reg(pctx, reg, val))) {
              ;
            }
            if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(pctx, reg, &val))) {
              ;
            }
          }
          else {
            // Not to us
            return VSCP_ERROR_SUCCESS;
          }
        }
        else {
          rv = VSCP_ERROR_INVALID_FRAME;
        }

        // Send response
        if (VSCP_ERROR_SUCCESS == rv) {
          vscp_event_t evrply;
          evrply.pdata = calloc(VSCP_MAX_DATA, 1);
          if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
          vscp_frmw2_setup_event(pctx, &evrply);
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          evrply.sizeData   = 2;
          evrply.pdata[0]   = reg;
          evrply.pdata[0]   = val;
          rv                = pctx->ops->send_event(pctx, &evrply);
          free(evrply.pdata);
        }

      } break;

      case VSCP_TYPE_PROTOCOL_RW_RESPONSE:
        // We do nothing
        break;

      case VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER: {

        if ((8 == ADJSIZEX) && (EXDTA(0) == (pctx->nickname & 0xff)) && (EXDTA(1) == pctx->bootloader_algorithm) &&
            (EXDTA(2) == pctx->guid[0]) && (EXDTA(3) == pctx->guid[3]) && (EXDTA(4) == pctx->guid[5]) &&
            (EXDTA(5) == pctx->guid[7]) && (EXDTA(6) == ((pctx->page_select >> 8) & 0xff)) &&
            (EXDTA(7) == ((pctx->page_select) & 0xff))) {

          // Positive response is sent by bootloader

          pctx->ops->enter_bootloader(pctx);
        }
        else {
          // Send negative response
          vscp_event_t evrply;
          evrply.pdata = calloc(VSCP_MAX_DATA, 1);
          if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
          vscp_frmw2_setup_event(pctx, &evrply);
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_NACK_BOOT_LOADER;
          evrply.sizeData   = 0;
          rv                = pctx->ops->send_event(pctx, &evrply);
          free(evrply.pdata);
        }
      } break;

      case VSCP_TYPE_PROTOCOL_RESET_DEVICE: {
        if (5 == ADJSIZEX) {

          switch (EXDTA(0)) {
            case 0:
              if ((EXDTA(1) == pctx->guid[15]) && (EXDTA(2) == pctx->guid[14]) && (EXDTA(3) == pctx->guid[13]) &&
                  (EXDTA(4) == pctx->guid[12])) {
                pctx->reset_device_flags |= 0x01;
                // One second to receive the rest of the events
                pctx->timer = pctx->ops->get_milliseconds(pctx);
              }
              break;

            case 1:
              if ((EXDTA(1) == pctx->guid[11]) && (EXDTA(2) == pctx->guid[10]) && (EXDTA(3) == pctx->guid[9]) &&
                  (EXDTA(4) == pctx->guid[8])) {
                pctx->reset_device_flags |= 0x02;
              }
              break;

            case 2:
              if ((EXDTA(1) == pctx->guid[7]) && (EXDTA(2) == pctx->guid[6]) && (EXDTA(3) == pctx->guid[5]) &&
                  (EXDTA(4) == pctx->guid[4])) {
                pctx->reset_device_flags |= 0x04;
              }
              break;

            case 3:
              if ((EXDTA(1) == pctx->guid[3]) && (EXDTA(2) == pctx->guid[2]) && (EXDTA(3) == pctx->guid[1]) &&
                  (EXDTA(4) == pctx->guid[0])) {
                pctx->reset_device_flags |= 0x08;
              }
              break;

            default:
              pctx->reset_device_flags = 0;
              break;
          }

          if (0x0f == pctx->reset_device_flags) {
            pctx->reset_device_flags = 0;
            pctx->ops->restore_defaults(pctx);
            vscp_frmw2_init(pctx);
            // pctx->ops->reset(pctx);
          }
          else {
            // all events must be received within the specified time
            if ((pctx->ops->get_milliseconds(pctx) - pctx->timer) > VSCP_GUID_RESET_TIMEOUT) {
              // Timeout Start over
              pctx->reset_device_flags = 0;
            }
          }
        }
      } break;

      case VSCP_TYPE_PROTOCOL_PAGE_READ:
        if ((3 == ADJSIZEX) && (pctx->nickname == EXDTA(0))) {
          vscp_frmw2_page_read(pctx, EXDTA(1), EXDTA(2)); // index, count
        }
        else if ((4 == ADJSIZEX) && (pctx->nickname == construct_unsigned16(EXDTA(0), EXDTA(1)))) {
          vscp_frmw2_page_read(pctx, EXDTA(3), EXDTA(3)); // index, count
        }
        break;

      case VSCP_TYPE_PROTOCOL_PAGE_WRITE:
        // Don't handle 16-bit node-id
        if ((ADJSIZEX > 3) && (pctx->nickname == EXDTA(0))) {
          vscp_frmw2_page_write(pctx, EXDTA(1), ADJSIZEX - 2, &EXDTA(2)); // index, count, data
        }
        break;

      case VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE:
        // We do nothing
        break;

      case VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_PROBE:
        if (pctx->bHighEndServerResponse) {
          vscp_event_t evrply;
          evrply.pdata = calloc(VSCP_MAX_DATA, 1);
          if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
          vscp_frmw2_setup_event(pctx, &evrply);
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_RESPONSE;
          evrply.sizeData   = 8;
          evrply.pdata[0]   = (pctx->high_end_srv_caps >> 8) & 0xff;
          evrply.pdata[1]   = pctx->high_end_srv_caps & 0xff;
          evrply.pdata[2]   = (pctx->high_end_ip_address >> 24) & 0xff;
          evrply.pdata[3]   = (pctx->high_end_ip_address >> 16) & 0xff;
          evrply.pdata[4]   = (pctx->high_end_ip_address >> 8) & 0xff;
          evrply.pdata[5]   = pctx->high_end_ip_address & 0xff;
          evrply.pdata[6]   = (pctx->high_end_srv_port >> 8) & 0xff;
          evrply.pdata[7]   = pctx->high_end_srv_port & 0xff;
          rv                = pctx->ops->send_event(pctx, &evrply);
          free(evrply.pdata);
        }
        break;

      case VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_RESPONSE:
        // We do nothing
        break;

      case VSCP_TYPE_PROTOCOL_INCREMENT_REGISTER: {

        uint8_t val;
        vscp_event_t evrply;
        evrply.pdata = calloc(VSCP_MAX_DATA, 1);
        if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
        vscp_frmw2_setup_event(pctx, &evrply);

        if ((2 == ADJSIZEX) && (pctx->nickname == EXDTA(0))) {

          vscp_frmw2_read_reg(pctx, EXDTA(1), &val);
          val++;
          vscp_frmw2_write_reg(pctx, EXDTA(1), val);

          // Send confirmation
          vscp_frmw2_setup_event(pctx, &evrply);
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          evrply.sizeData   = 2;
          evrply.pdata[0]   = EXDTA(1);
          evrply.pdata[1]   = val;
          rv                = pctx->ops->send_event(pctx, &evrply);
        }
        else if ((3 == ADJSIZEX) && (pctx->nickname == construct_unsigned16(EXDTA(0), EXDTA(1)))) {

          vscp_frmw2_read_reg(pctx, EXDTA(2), &val);
          val++;
          vscp_frmw2_write_reg(pctx, EXDTA(2), val);

          // Send confirmation
          vscp_frmw2_setup_event(pctx, &evrply);
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          evrply.sizeData   = 2;
          evrply.pdata[0]   = EXDTA(1);
          evrply.pdata[1]   = val;
          rv                = pctx->ops->send_event(pctx, &evrply);
        }
        free(evrply.pdata);
      } break;

      case VSCP_TYPE_PROTOCOL_DECREMENT_REGISTER: {

        uint8_t val;
        vscp_event_t evrply;
        evrply.pdata = calloc(VSCP_MAX_DATA, 1);
        if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
        vscp_frmw2_setup_event(pctx, &evrply);

        if ((2 == ADJSIZEX) && (pctx->nickname == EXDTA(0))) {

          vscp_frmw2_read_reg(pctx, EXDTA(1), &val);
          val--;
          vscp_frmw2_write_reg(pctx, EXDTA(1), val);

          // Send confirmation
          vscp_frmw2_setup_event(pctx, &evrply);
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          evrply.sizeData   = 2;
          evrply.pdata[0]   = EXDTA(1);
          evrply.pdata[1]   = val;
          rv                = pctx->ops->send_event(pctx, &evrply);
        }
        else if ((3 == ADJSIZEX) && (pctx->nickname == construct_unsigned16(EXDTA(0), EXDTA(1)))) {

          vscp_frmw2_read_reg(pctx, EXDTA(2), &val);
          val--;
          vscp_frmw2_write_reg(pctx, EXDTA(2), val);

          // Send confirmation
          vscp_frmw2_setup_event(pctx, &evrply);
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
          evrply.sizeData   = 2;
          evrply.pdata[0]   = EXDTA(1);
          evrply.pdata[1]   = val;
          rv                = pctx->ops->send_event(pctx, &evrply);
        }
        free(evrply.pdata);
      } break;

      case VSCP_TYPE_PROTOCOL_WHO_IS_THERE:
        if (0 == ADJSIZEX) {
          vscp_frmw2_whois_response(pctx);
        }
        else if ((1 == ADJSIZEX) && ((0xff == EXDTA(0)) || (pctx->nickname == EXDTA(0)))) {
          return vscp_frmw2_whois_response(pctx);
        }
        else if ((2 == ADJSIZEX) && ((0xffff == construct_unsigned16(EXDTA(0), EXDTA(1))) ||
                                     (pctx->nickname == construct_unsigned16(EXDTA(0), EXDTA(1))))) {
          return vscp_frmw2_whois_response(pctx);
        }
        break;

      case VSCP_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE:
        // We do nothing
        break;

      case VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO:
        rv = vscp_frmw2_report_dmatrix(pctx);
        break;

      case VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF: {

        uint16_t uppermax;
        uint16_t nframes;
        vscp_event_t evrply;
        evrply.pdata = calloc(VSCP_MAX_DATA, 1);
        if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
        vscp_frmw2_setup_event(pctx, &evrply);
        if (VSCP_LEVEL1 == pctx->level) {
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF_RESPONSE;
          uppermax          = 7;
          nframes           = strlen(pctx->pInternalMdf) / 6;
          if (strlen(pctx->pInternalMdf) % 6) {
            nframes++;
          }
        }
        else {
          evrply.vscp_class = VSCP_CLASS2_PROTOCOL;
          evrply.vscp_type  = VSCP2_TYPE_PROTOCOL_GET_EMBEDDED_MDF_RESPONSE;
          uppermax          = 511;
          nframes           = strlen(pctx->pInternalMdf) / 508;
          if (strlen(pctx->pInternalMdf) % 508) {
            nframes++;
          }
        }

        if (NULL != pctx->pInternalMdf) {
          uint16_t idx  = 0;
          uint32_t pos  = (VSCP_LEVEL1 == pctx->level) ? 2 : 4;
          const char *p = pctx->pInternalMdf;
          do {
            evrply.pdata[pos] = *p;
            pos++;
            if (pos > uppermax) {
              evrply.sizeData = uppermax + 1;
              evrply.pdata[0] = (idx >> 8) & 0xff;
              evrply.pdata[1] = idx & 0xff;
              if (VSCP_LEVEL2 == pctx->level) {
                evrply.pdata[2] = (nframes >> 8) & 0xff;
                evrply.pdata[3] = nframes & 0xff;
              }
              if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &evrply))) {
                free(evrply.pdata);
                return rv;
              }
              pos = (VSCP_LEVEL1 == pctx->level) ? 2 : 4;
              idx++;
            }
          } while (*(++p));

          if (VSCP_LEVEL1 == pctx->level) {
            if (2 != pos) {
              evrply.sizeData = 2 + pos - 2;
              evrply.pdata[0] = (idx >> 8) & 0xff;
              evrply.pdata[1] = idx & 0xff;
              if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &evrply))) {
                free(evrply.pdata);
                return rv;
              }
            }
          }
          else {
            if (4 != pos) {
              evrply.sizeData = 4 + pos - 4;
              evrply.pdata[0] = (idx >> 8) & 0xff;
              evrply.pdata[1] = idx & 0xff;
              evrply.pdata[2] = (nframes >> 8) & 0xff;
              evrply.pdata[3] = nframes & 0xff;
              if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &evrply))) {
                free(evrply.pdata);
                return rv;
              }
            }
          }
        }
        else {
          evrply.sizeData = 0;
          rv              = pctx->ops->send_event(pctx, &evrply);
        }
        free(evrply.pdata);
      } break;

      case VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_READ: {
        if ((4 == ADJSIZEX) && (pctx->nickname == EXDTA(0))) {
          rv = vscp_frmw2_extended_page_read(pctx, EXDTA(0), construct_unsigned16(EXDTA(1), EXDTA(2)), EXDTA(3), 1);
        }
        if ((5 == ADJSIZEX) && (pctx->nickname == EXDTA(0))) {
          rv =
            vscp_frmw2_extended_page_read(pctx, EXDTA(0), construct_unsigned16(EXDTA(1), EXDTA(2)), EXDTA(3), EXDTA(4));
        }
        if ((6 == ADJSIZEX) && (pctx->nickname == construct_unsigned16(EXDTA(0), EXDTA(1)))) {
          rv = vscp_frmw2_extended_page_read(pctx,
                                             construct_unsigned16(EXDTA(0), EXDTA(1)),
                                             construct_unsigned16(EXDTA(2), EXDTA(3)),
                                             EXDTA(4),
                                             EXDTA(5));
        }
      } break;

      case VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE: {
        uint8_t buf[4];
        uint8_t cnt = ADJSIZEX - 4;

        for (int i = 0; i < cnt; i++) {
          buf[i] = EXDTA(4 + i);
        }
        rv =
          vscp_frmw2_extended_page_write(pctx, EXDTA(0), construct_unsigned16(EXDTA(1), EXDTA(2)), EXDTA(3), cnt, buf);
      } break;

      case VSCP_TYPE_PROTOCOL_GET_EVENT_INTEREST: {

        vscp_event_t evrply;
        evrply.pdata = calloc(VSCP_MAX_DATA, 1);
        if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
        vscp_frmw2_setup_event(pctx, &evrply);

        if (NULL == pctx->pEventsOfInterest) {
          // All events is of interest
          evrply.sizeData   = 0;
          evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
          evrply.vscp_type  = VSCP_TYPE_PROTOCOL_GET_EVENT_INTEREST_RESPONSE;
          if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &evrply))) {
            free(evrply.pdata);
            return rv;
          }
        }
        else {
          if (VSCP_LEVEL1 == pctx->level) {
            uint16_t idx = 0;
            while (pctx->pEventsOfInterest[idx]) {
              evrply.sizeData = 5;
              evrply.pdata[0] = idx; // Frame index
              evrply.pdata[1] = (pctx->pEventsOfInterest[idx] >> 16) & 0xff;
              evrply.pdata[2] = pctx->pEventsOfInterest[idx] & 0xff;
              evrply.pdata[3] = (pctx->pEventsOfInterest[idx] >> 16) & 0xff;
              evrply.pdata[4] = pctx->pEventsOfInterest[idx] & 0xff;
              if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &evrply))) {
                free(evrply.pdata);
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
            while (pctx->pEventsOfInterest[pos]) {
              size++;
            }

            uint8_t nframes = size / 256;
            if (size % 256) {
              nframes++;
            }

            for (uint8_t idx = 0; idx < nframes; idx++) {

              evrply.sizeData = 0;
              for (uint16_t j = 0; j < 256; j++) {

                // If we are done
                if ((idx * 256 + j) > size) {
                  break;
                }

                evrply.pdata[2 + j] = (pctx->pEventsOfInterest[pos + j] >> 16) & 0xffff;
                evrply.pdata[3 + j] = pctx->pEventsOfInterest[pos + j] & 0xffff;

                evrply.sizeData += 2;
              }

              if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &evrply))) {
                free(evrply.pdata);
                return rv;
              }
            }
          }
        }
        free(evrply.pdata);
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

        if (NULL == pex->pdata) {
          return VSCP_ERROR_INVALID_POINTER;
        }

        // Get start position
        uint32_t startpos = construct_unsigned32(pex->pdata[16], pex->pdata[17], pex->pdata[18], pex->pdata[19]);

        // If size is given get it
        uint16_t cnt = construct_unsigned16(pex->pdata[20], pex->pdata[21]);
        if (cnt > (512 - 4)) {
          cnt = (512 - 4);
        }
        rv = vscp_frmw2_vscp2_register_read(pctx, startpos, cnt);
      } break;

      case VSCP2_TYPE_PROTOCOL_WRITE_REGISTER: {

        // Must be at least GUID + start reg + one data byte
        if (pex->sizeData < 23) {
          return VSCP_ERROR_PARAMETER;
        }

        if (NULL == pex->pdata) {
          return VSCP_ERROR_INVALID_POINTER;
        }

        // Get start position
        uint32_t startpos = construct_unsigned32(pex->pdata[16], pex->pdata[17], pex->pdata[18], pex->pdata[19]);

        rv = vscp_frmw2_vscp2_register_write(pctx, startpos, pex->sizeData - 20, (const uint8_t *) &pex->pdata[16 + 4]);
      } break;

      case VSCP2_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE: {
        // char devname[64];
        // char url[32];

        vscp_event_t evrply;
        evrply.pdata = calloc(VSCP_MAX_DATA, 1);
        if (NULL == evrply.pdata) { rv = VSCP_ERROR_MEMORY; break; }
        vscp_frmw2_setup_event(pctx, &evrply);

        vscp_frmw2_memcpy(evrply.pdata, VSCP_MAX_DATA, pctx->guid, sizeof(pctx->guid));
        vscp_frmw2_memcpy(&evrply.pdata[sizeof(pctx->guid)],
                          VSCP_MAX_DATA - sizeof(pctx->guid),
                          pctx->mdfurl,
                          sizeof(pctx->mdfurl));
        rv = pctx->ops->send_event(pctx, &evrply);
        free(evrply.pdata);
      } break;
    }
  }

  return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_setup_event
//

void
vscp_frmw2_setup_event(vscp_frmw2_firmware_context_t *pctx, vscpEvent *const pev)
{
  // Check pointers
  if (NULL == pctx || NULL == pev) {
    return;
  }

  pev->sizeData = 0;
  if (NULL != pev->pdata) {
    memset(pev->pdata, 0, VSCP_MAX_DATA);
  }
  pev->head = VSCP_PRIORITY_NORMAL;
  vscp_frmw2_memcpy(pev->GUID, sizeof(pev->GUID), pctx->guid, sizeof(pctx->guid));
  /*!
    If level I we use the GUID as a space for the nickname
  */
  if (VSCP_LEVEL1 == pctx->level) {
    pev->GUID[14] = (pctx->nickname >> 8) & 0xff;
    pev->GUID[15] = pctx->nickname & 0xff;
  }
  pev->vscp_class = VSCP_CLASS2_LEVEL1_PROTOCOL;
  pev->vscp_type  = VSCP2_TYPE_PROTOCOL_READ_WRITE_RESPONSE;
  pctx->ops->set_event_time(pctx, pev);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_vscp2_register_read
//

int
vscp_frmw2_vscp2_register_read(vscp_frmw2_firmware_context_t *pctx, uint32_t startreg, uint16_t count)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv;

  /* NOTE: This function owns ex.pdata; it is freed on all return paths. */
  vscp_event_t ex;
  ex.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == ex.pdata) {
    return VSCP_ERROR_MEMORY;
  }
  vscp_frmw2_setup_event(pctx, &ex);

  // Read the registers
  for (uint32_t pos = startreg; pos < (startreg + count); pos++) {
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(pctx, pos, &ex.pdata[pos - startreg + 4]))) {
      free(ex.pdata);
      return rv;
    }
  }

  ex.vscp_class = VSCP_CLASS2_PROTOCOL;
  ex.vscp_type  = VSCP2_TYPE_PROTOCOL_READ_WRITE_RESPONSE;
  ex.sizeData   = count + 1;

  // Data
  ex.sizeData = count + 4;
  ex.pdata[0] = (uint8_t) ((startreg >> 24) & 0xff);
  ex.pdata[1] = (uint8_t) ((startreg >> 16) & 0xff);
  ex.pdata[2] = (uint8_t) ((startreg >> 8) & 0xff);
  ex.pdata[3] = (uint8_t) (startreg & 0xff);

  // Send event
  int result = pctx->ops->send_event(pctx, &ex);
  free(ex.pdata);
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_vscp2_register_write
//

int
vscp_frmw2_vscp2_register_write(vscp_frmw2_firmware_context_t *pctx,
                                uint32_t startreg,
                                uint16_t cnt,
                                const uint8_t *const pdata)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv;

  for (uint32_t pos = startreg; pos < (startreg + cnt); pos++) {
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->write_reg(pctx, 0, pos, pdata[pos - startreg]))) {
      return rv;
    }
  }

  return vscp_frmw2_vscp2_register_read(pctx, startreg, cnt);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_read_reg
//

int
vscp_frmw2_read_reg(vscp_frmw2_firmware_context_t *pctx, uint32_t reg, uint8_t *pval)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv;

  if (reg < (VSCP_STD_REGISTER_START + ADJSTDREG)) {
    // User register
    rv = pctx->ops->read_reg(pctx, pctx->page_select, reg, pval);
    // OOB register error is OK but always read as zero
    if (VSCP_ERROR_INDEX_OOB == rv) {
      *pval = 0;
      return VSCP_ERROR_SUCCESS;
    }
    return rv;
  }
  else {
    // * * * standard registers * * *
    if ((VSCP_STD_REGISTER_ALARM_STATUS + ADJSTDREG) == reg) {
      // Writing/reading alarm resets it regardless of value
      *pval              = pctx->alarm_status;
      pctx->alarm_status = 0;
      // Inform app. that standard register has been set to zero
      pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_ALARM_STATUS);
    }
    else if ((VSCP_STD_REGISTER_MAJOR_VERSION + ADJSTDREG) == reg) {
      *pval = FRMW2_MAJOR_VERSION;
    }
    else if ((VSCP_STD_REGISTER_MINOR_VERSION + ADJSTDREG) == reg) {
      *pval = FRMW2_MINOR_VERSION;
    }
    else if ((VSCP_STD_REGISTER_ERROR_COUNTER + ADJSTDREG) == reg) {
      // Writing/reading error counter resets it regardless of value
      *pval              = pctx->errorCounter;
      pctx->errorCounter = 0;
      // Inform app that standard register has been set to zero
      pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_ERROR_COUNTER);
    }
    else if (((VSCP_STD_REGISTER_USER_ID + ADJSTDREG) >= reg) && (VSCP_STD_REGISTER_USER_ID + ADJSTDREG + 3 <= reg)) {
      *pval = *(&pctx->userId + (reg - VSCP_STD_REGISTER_USER_ID + ADJSTDREG));
    }
    else if (((VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG) == reg) &&
             (VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG + 3 <= reg)) {
      *pval = *(&pctx->manufacturerId + (reg - VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG));
    }
    else if (((VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG) == reg) &&
             (VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG + 3 <= reg)) {
      *pval = *(&pctx->manufacturerId + (reg - VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG));
    }
    else if ((VSCP_STD_REGISTER_NICKNAME_ID_LSB + ADJSTDREG) == reg) {
      *pval = pctx->nickname & 0xff;
    }
    else if ((VSCP_STD_REGISTER_NICKNAME_ID_MSB + ADJSTDREG) == reg) {
      *pval = (pctx->nickname >> 8) & 0xff;
    }
    else if ((VSCP_STD_REGISTER_PAGE_SELECT_MSB + ADJSTDREG) == reg) {
      *pval = (pctx->page_select >> 8) & 0xff;
    }
    else if ((VSCP_STD_REGISTER_PAGE_SELECT_LSB + ADJSTDREG) == reg) {
      *pval = pctx->page_select & 0xff;
    }
    else if ((VSCP_STD_REGISTER_FIRMWARE_MAJOR + ADJSTDREG) == reg) {
      *pval = pctx->firmware_major_version;
    }
    else if ((VSCP_STD_REGISTER_FIRMWARE_MINOR + ADJSTDREG) == reg) {
      *pval = pctx->firmware_minor_version;
    }
    else if ((VSCP_STD_REGISTER_FIRMWARE_SUBMINOR + ADJSTDREG) == reg) {
      *pval = pctx->firmware_sub_minor_version;
    }
    else if ((VSCP_STD_REGISTER_BOOT_LOADER + ADJSTDREG) == reg) {
      *pval = pctx->bootloader_algorithm;
    }
    else if ((VSCP_STD_REGISTER_BUFFER_SIZE + ADJSTDREG) == reg) {
      *pval = 0; // Deprecated register value
    }
    else if ((VSCP_STD_REGISTER_PAGES_COUNT + ADJSTDREG) == reg) {
      *pval = 0; // Deprecated register value
    }
    else if (reg >= (VSCP_STD_REGISTER_FAMILY_CODE + ADJSTDREG) &&
             (reg <= (VSCP_STD_REGISTER_FAMILY_CODE + ADJSTDREG + 3))) {
      *pval = *(&pctx->standard_device_family_code + reg - VSCP_STD_REGISTER_FAMILY_CODE + ADJSTDREG);
    }
    else if (reg >= (VSCP_STD_REGISTER_DEVICE_TYPE + ADJSTDREG) &&
             (reg <= (VSCP_STD_REGISTER_DEVICE_TYPE + ADJSTDREG + 3))) {
      *pval = *(&pctx->standard_device_type_code + reg - VSCP_STD_REGISTER_DEVICE_TYPE + ADJSTDREG);
    }
    else if ((VSCP_STD_REGISTER_NODE_RESET + ADJSTDREG) == reg) {
      *pval = 0; // always read as zero
    }
    else if ((VSCP_STD_REGISTER_FIRMWARE_CODE_MSB + ADJSTDREG) == reg) {
      *pval = (pctx->firmware_device_code >> 8) & 0xff;
    }
    else if ((VSCP_STD_REGISTER_FIRMWARE_CODE_LSB + ADJSTDREG) == reg) {
      *pval = pctx->firmware_device_code & 0xff;
    }
    // * * * Unused register range * * *
    else if ((reg > (VSCP_STD_REGISTER_NICKNAME_ID_MSB + ADJSTDREG)) && (reg < (VSCP_STD_REGISTER_GUID) + ADJSTDREG)) {
      *pval = 0;
    }
    else if ((reg >= (VSCP_STD_REGISTER_GUID + ADJSTDREG)) && (reg < (VSCP_STD_REGISTER_GUID + ADJSTDREG + 16))) {
      *pval = pctx->guid[reg - VSCP_STD_REGISTER_GUID + ADJSTDREG];
    }
    else if ((reg >= (VSCP_STD_REGISTER_DEVICE_URL + ADJSTDREG)) &&
             (reg < (VSCP_STD_REGISTER_DEVICE_URL + ADJSTDREG + 32))) {
      *pval = pctx->mdfurl[reg - VSCP_STD_REGISTER_DEVICE_URL + ADJSTDREG];
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
vscp_frmw2_write_reg(vscp_frmw2_firmware_context_t *pctx, uint32_t reg, uint8_t val)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv = VSCP_ERROR_ERROR; // To handle non existent registers

  if (reg < VSCP_STD_REGISTER_START + ADJSTDREG) {
    // User register - Let usr code handle
    rv = pctx->ops->write_reg(pctx, 0, reg, val);
    // OOB error is OK but always read as zero
    if (VSCP_ERROR_INDEX_OOB == rv) {
      val = ~val;
      rv  = VSCP_ERROR_SUCCESS;
    }
  }
  // * * * Standard registers * * *
  else if ((VSCP_STD_REGISTER_ALARM_STATUS + ADJSTDREG) == reg) {
    // Writing/reading alarm resets it regardless of value
    pctx->alarm_status = 0;
    // Inform app. that standard register has been set to zero
    pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_ALARM_STATUS);
  }
  else if ((reg >= (VSCP_STD_REGISTER_USER_ID + ADJSTDREG)) && (reg <= VSCP_STD_REGISTER_USER_ID + ADJSTDREG)) {
    uint8_t *p = (uint8_t *) &pctx->userId;
    // pctx->userId[reg - VSCP_STD_REGISTER_USER_ID + ADJSTDREG] = val;
    p[reg - VSCP_STD_REGISTER_USER_ID + ADJSTDREG] = val;
    // Inform app that standard register has been changes
    rv = pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_USER_ID);
  }

  // Write manufacturer id configuration information
  else if ((reg >= (VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG)) &&
           (reg <= (VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG + 3))) {
    if (pctx->bEnableWriteProtectedLocations) {
      // page register must be 0xffff for writes to be possible
      if (0xffff != pctx->page_select) {
        rv = VSCP_ERROR_NOT_SUPPORTED;
      }
      else {
        uint8_t *p                                            = (uint8_t *) &pctx->manufacturerId;
        p[reg - VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG] = val;
        // pctx->manufacturerId[reg - VSCP_STD_REGISTER_USER_MANDEV_ID + ADJSTDREG] = val;
        rv = VSCP_ERROR_SUCCESS;
      }
      rv = pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_USER_MANDEV_ID);
    }
  }
  else if ((reg >= (VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG)) &&
           (reg <= (VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG + 3))) {
    if (pctx->bEnableWriteProtectedLocations) {
      // page register must be 0xffff for writes to be possible
      if (0xffff != pctx->page_select) {
        rv = VSCP_ERROR_NOT_SUPPORTED;
      }
      else {
        uint8_t *p                                               = (uint8_t *) &pctx->manufacturerSubId;
        p[reg - VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG] = val;
        // pctx->manufacturerSubId[reg - VSCP_STD_REGISTER_USER_MANSUBDEV_ID + ADJSTDREG] = val;
        rv = VSCP_ERROR_SUCCESS;
      }
      rv = pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_USER_MANSUBDEV_ID);
    }
  }
  else if ((reg >= (VSCP_STD_REGISTER_GUID + ADJSTDREG)) && (reg <= (VSCP_STD_REGISTER_GUID + ADJSTDREG + 15))) {
    if (pctx->bEnableWriteProtectedLocations) {
      if (0xffff != pctx->page_select) {
        return VSCP_ERROR_NOT_SUPPORTED;
      }
      else {
        pctx->guid[reg - VSCP_STD_REGISTER_GUID + ADJSTDREG] = val;
        rv                                                   = VSCP_ERROR_SUCCESS;
      }
      rv = pctx->ops->stdreg_change(pctx, VSCP_STD_REGISTER_GUID);
    }
  }
  else if (reg == (VSCP_STD_REGISTER_NODE_RESET + ADJSTDREG)) {

    uint32_t timer;
    timer = pctx->ops->get_milliseconds(pctx);

    if (0x55 == val) {
      pctx->timer = timer;
    }
    else if (0xaa == val) {
      if ((timer - pctx->timer) < 1000) {
        pctx->timer = 0;
        return pctx->ops->restore_defaults(pctx);
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
vscp_frmw2_send_heartbeat(vscp_frmw2_firmware_context_t *pctx)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // int rv;

  /* NOTE: This function owns evrply.pdata; it is freed on all return paths. */
  vscp_event_t evrply;
  evrply.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == evrply.pdata) {
    return VSCP_ERROR_MEMORY;
  }
  vscp_frmw2_setup_event(pctx, &evrply);

  // Construct reply event
  if (VSCP_LEVEL1 == pctx->level) {
    evrply.vscp_class = VSCP_CLASS1_INFORMATION;
    evrply.vscp_type  = VSCP_TYPE_INFORMATION_NODE_HEARTBEAT;
    evrply.sizeData   = 0;
  }
  else {
    // Level II should add node name in data
    evrply.vscp_class = VSCP_CLASS2_INFORMATION;
    evrply.vscp_type  = VSCP2_TYPE_INFORMATION_HEART_BEAT;
    memset(evrply.pdata, 0, VSCP_MAX_DATA);
    vscp_frmw2_memcpy(evrply.pdata, VSCP_MAX_DATA, pctx->deviceName, sizeof(pctx->deviceName));
    evrply.sizeData = strlen((const char *) evrply.pdata);
  }

  // Send event
  int result = pctx->ops->send_event(pctx, &evrply);
  free(evrply.pdata);
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_send_caps
//

int
vscp_frmw2_send_caps(vscp_frmw2_firmware_context_t *pctx)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  /* NOTE: This function owns ex.pdata; it is freed on all return paths. */
  vscp_event_t ex;
  ex.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == ex.pdata) {
    return VSCP_ERROR_MEMORY;
  }
  vscp_frmw2_setup_event(pctx, &ex);

  // Construct reply event
  ex.vscp_class = VSCP_CLASS2_PROTOCOL;
  ex.vscp_type  = VSCP2_TYPE_PROTOCOL_HIGH_END_SERVER_CAPS;

  // Data
  ex.sizeData = 104;
  memset(ex.pdata, 0, ex.sizeData);

  // Capabilities
  ex.pdata[0] = 0x40; // Node have a standard decision matrix.
  ex.pdata[1] = 0x00;
  ex.pdata[2] = 0x00;
  ex.pdata[3] = 0x00;
  ex.pdata[4] = 0x00;
  ex.pdata[5] = 0x00;
  ex.pdata[6] = 0x80; // Have VSCP TCP server with VCSP link interface.
  ex.pdata[7] = 0x38; // IP4 support, SLL support
                      // Accepts two or more simultaneous connections on TCP/IP interface.

  // GUID
  vscp_frmw2_memcpy(ex.pdata + 8, VSCP_MAX_DATA - 8, pctx->guid, sizeof(pctx->guid));

  // ip address
  vscp_frmw2_memcpy(ex.pdata + 24, VSCP_MAX_DATA - 24, pctx->ipaddr, sizeof(pctx->ipaddr));

  // Device name
  vscp_frmw2_memcpy(ex.pdata + 40, VSCP_MAX_DATA - 40, pctx->deviceName, sizeof(pctx->deviceName));

  // Send event
  int result = pctx->ops->send_event(pctx, &ex);
  free(ex.pdata);
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_send_high_end_server_probe
//

int
vscp_frmw2_send_high_end_server_probe(vscp_frmw2_firmware_context_t *pctx)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  /* NOTE: This function owns ex.pdata; it is freed on all return paths. */
  vscp_event_t ex;
  ex.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == ex.pdata) {
    return VSCP_ERROR_MEMORY;
  }
  vscp_frmw2_setup_event(pctx, &ex);

  // Construct reply event
  vscp_frmw2_setup_event(pctx, &ex);
  ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  ex.vscp_type  = VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_PROBE;
  ex.sizeData   = 0;

  // Data
  ex.sizeData = 0;

  // Send event
  int result = pctx->ops->send_event(pctx, &ex);
  free(ex.pdata);
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_page_read
//

int
vscp_frmw2_page_read(vscp_frmw2_firmware_context_t *pctx, uint32_t offset, uint8_t count)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv = VSCP_ERROR_SUCCESS;
  uint8_t i;
  uint8_t seq = 0;
  uint8_t val;

  /* NOTE: This function owns rplyex.pdata; it is freed on all return paths. */
  vscp_event_t rplyex;
  rplyex.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == rplyex.pdata) {
    return VSCP_ERROR_MEMORY;
  }
  vscp_frmw2_setup_event(pctx, &rplyex);

  for (i = 0; i < count; i++) {

    vscp_frmw2_read_reg(pctx, offset + i, &val);
    rplyex.pdata[(i % 7) + 1] = val;

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
      rplyex.pdata[0]   = seq;

      // send the event
      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &rplyex))) {
        free(rplyex.pdata);
        return rv;
      }

      offset++;
    }
  }

  free(rplyex.pdata);
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
vscp_frmw2_page_write(vscp_frmw2_firmware_context_t *pctx, uint32_t offset, uint8_t count, const uint8_t *const pbuf)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv;
  uint8_t i;
  uint8_t val;

  /* NOTE: This function owns ex.pdata; it is freed on all return paths. */
  vscp_event_t ex;
  ex.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == ex.pdata) {
    return VSCP_ERROR_MEMORY;
  }
  vscp_frmw2_setup_event(pctx, &ex);

  for (i = 0; i < count; i++) {

    // Write VSCP register
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_write_reg(pctx, offset + i, pbuf[i]))) {
      free(ex.pdata);
      return rv;
    }

    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(pctx, i, &val))) {
      free(ex.pdata);
      return rv;
    }

    ex.pdata[1 + i] = val;
  }

  ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  ex.vscp_type  = VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE;
  ex.sizeData   = count + 1;
  ex.pdata[0]   = 0; // index

  // send the event
  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &ex))) {
    free(ex.pdata);
    return rv;
  }

  free(ex.pdata);
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_frmw2_whois_response
//

int
vscp_frmw2_whois_response(vscp_frmw2_firmware_context_t *pctx)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv;
  uint8_t i, j, k = 0;

  /* NOTE: This function owns ex.pdata; it is freed on all return paths. */
  vscp_event_t ex;
  ex.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == ex.pdata) {
    return VSCP_ERROR_MEMORY;
  }
  vscp_frmw2_setup_event(pctx, &ex);

  if (VSCP_LEVEL1 == pctx->level) {

    ex.vscp_class = VSCP_CLASS1_PROTOCOL;
    ex.vscp_type  = VSCP_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE;
    ex.sizeData   = 8;

    for (i = 0; i < 3; i++) // fill up with GUID
    {
      ex.pdata[0] = i;

      for (j = 1; j < 8; j++) {
        ex.pdata[j] = pctx->guid[15 - k++];
        if (k > 16)
          break;
      }

      if (k > 16)
        break;

      // send the event
      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &ex))) {
        free(ex.pdata);
        return rv;
      }
    }

    for (j = 0; j < 5; j++) // fill up previous event with MDF
    {
      if (pctx->mdfurl[j] > 0)
        ex.pdata[3 + j] = pctx->mdfurl[j];
      else
        ex.pdata[3 + j] = 0;
    }

    // send the event
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &ex))) {
      free(ex.pdata);
      return rv;
    }

    k = 5;                  // start offset
    for (i = 3; i < 7; i++) // fill up with the rest of GUID
    {
      ex.pdata[0] = i;

      for (j = 1; j < 8; j++) {
        ex.pdata[j] = pctx->mdfurl[k++];
      }
      // send the event
      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &ex))) {
        free(ex.pdata);
        return rv;
      }
    }
  }
  // Level II
  else {
    ex.vscp_class = VSCP_CLASS2_PROTOCOL;
    ex.vscp_type  = VSCP2_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE;
    ex.sizeData   = 16 + MIN(strlen((const char *) pctx->mdfurl), 32);
    memset(ex.pdata, 9, 512);
    vscp_frmw2_memcpy(ex.pdata, VSCP_MAX_DATA, pctx->guid, sizeof(pctx->guid));
    if (pctx->mdfurl[0]) {
      vscp_frmw2_memcpy(ex.pdata + sizeof(pctx->guid),
                        VSCP_MAX_DATA - sizeof(pctx->guid),
                        pctx->mdfurl,
                        MIN(strlen((const char *) pctx->mdfurl), sizeof(pctx->mdfurl)));
    }

    // send the event
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &ex))) {
      free(ex.pdata);
      return rv;
    }
  }
  free(ex.pdata);
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_frmw2_extended_page_read
//

int
vscp_frmw2_extended_page_read(vscp_frmw2_firmware_context_t *pctx,
                              uint16_t nodeid,
                              uint16_t page,
                              uint8_t startoffset,
                              uint16_t cnt)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv;
  uint16_t page_save;
  uint8_t byte = 0;
  uint8_t bytes_this_time, cb;

  unused(nodeid);

  /* NOTE: This function owns ex.pdata; it is freed on all return paths. */
  vscp_event_t ex;
  ex.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == ex.pdata) {
    return VSCP_ERROR_MEMORY;
  }
  vscp_frmw2_setup_event(pctx, &ex);

  // Save the current page
  page_save = pctx->page_select;

  // set temporary page
  pctx->page_select = page;

  // Construct response event
  vscp_frmw2_setup_event(pctx, &ex);
  ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  ex.vscp_type  = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;
  ex.pdata[0]   = 0; // index of event, this is the first
  ex.pdata[1]   = (page >> 8) & 0xff;
  ex.pdata[2]   = page & 0xff;

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
    ex.pdata[3] = startoffset + byte; // first register in this event

    // Put up to four registers to data space
    for (cb = 0; cb < bytes_this_time; cb++) {
      uint8_t val;
      if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_read_reg(pctx, startoffset + byte + cb, &val))) {
        // Restore the saved page
        pctx->page_select = page_save;
        free(ex.pdata);
        return rv;
      }
      ex.pdata[(4 + cb)] = val;
    }

    // Microseconds is used for a short delay
    uint64_t tm = pctx->ops->get_timestamp(pctx);

    // send the event
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send_event(pctx, &ex))) {
      // Restore the saved page
      pctx->page_select = page_save;
      free(ex.pdata);
      return rv;
    }

    // wait at bit before next message is sent
    // prevent overflow of bus or receiver
    // 100 usec should be sufficient, but requires separate timer
    while (pctx->ops->get_timestamp(pctx) < (tm + 100)) {
      ;
    }

    // increment byte by bytes_this_time and the event number by one
    byte += bytes_this_time;

    // increment the index
    ex.pdata[0] += 1;

  } while (byte < cnt);

  // Restore the saved page
  pctx->page_select = page_save;
  free(ex.pdata);
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_frmw2_extended_page_read
//

int
vscp_frmw2_extended_page_write(vscp_frmw2_firmware_context_t *pctx,
                               uint16_t nodeid,
                               uint16_t page,
                               uint8_t offset,
                               uint16_t cnt,
                               const uint8_t *const pbuf)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv = VSCP_ERROR_SUCCESS;
  uint8_t i;
  uint16_t page_save;

  // Save the current page
  page_save = pctx->page_select;

  // Assign the requested page
  pctx->page_select = page;

  for (i = 0; i < cnt; i++) {
    if (VSCP_ERROR_SUCCESS != (rv = vscp_frmw2_write_reg(pctx, offset + i, pbuf[i]))) {
      pctx->page_select = page_save;
      return rv;
    }
  }

  // Restore the saved page
  pctx->page_select = page_save;

  return vscp_frmw2_extended_page_read(pctx, nodeid, page, offset, cnt);

  // ex.sizeData   = 4 + cnt;
  // ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  // ex.vscp_type  = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;
  // ex.data[0]    = 0;                  // index (only one event)
  // ex.data[1]    = (page >> 8) & 0xff; // mirror page msb
  // ex.data[2]    = page & 0xff;        // mirror page lsb
  // ex.data[3]    = offset;             // Register base offset

  // // send the event
  // return pctx->ops->send_event(pctx, &ex);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_report_dmatrix
//

int
vscp_frmw2_report_dmatrix(vscp_frmw2_firmware_context_t *pctx)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  /* NOTE: This function owns evrply.pdata; it is freed on all return paths. */
  vscp_event_t evrply;
  evrply.pdata = calloc(VSCP_MAX_DATA, 1);
  if (NULL == evrply.pdata) {
    return VSCP_ERROR_MEMORY;
  }
  vscp_frmw2_setup_event(pctx, &evrply);

  if (VSCP_LEVEL1 == pctx->level) {
    evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
    evrply.vscp_type  = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;

    evrply.sizeData = 4;
    evrply.pdata[0] = pctx->nDmRows;     // Size = number of DM rows
    evrply.pdata[1] = pctx->regOffsetDm; // Start offset in page of DM
    evrply.pdata[2] = (pctx->pageDm >> 8) & 0xff;
    evrply.pdata[3] = pctx->pageDm & 0xff;
  }
  else {
    // Level II response
    evrply.vscp_class = VSCP_CLASS1_PROTOCOL;
    evrply.vscp_type  = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;

    evrply.sizeData = 22;
    vscp_frmw2_memcpy(evrply.pdata, VSCP_MAX_DATA, pctx->guid, sizeof(pctx->guid)); // GUID
    evrply.pdata[16] = pctx->sizeDmRow;                                             // Row size
    evrply.pdata[17] = pctx->nDmRows;                                               // Size = number of DM rows
    evrply.pdata[18] = (pctx->regOffsetDm >> 24) & 0xff;
    evrply.pdata[19] = (pctx->regOffsetDm >> 16) & 0xff;
    evrply.pdata[20] = (pctx->regOffsetDm >> 8) & 0xff;
    evrply.pdata[21] = pctx->regOffsetDm & 0xff;
  }

  // send the event
  int result = pctx->ops->send_event(pctx, &evrply);
  free(evrply.pdata);
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_feed_level1_dm
//
// oaddr  flags  class-mask  class-filter  type-mask  type-filter  action  action-parameter
//

int
vscp_frmw2_feed_level1_dm(vscp_frmw2_firmware_context_t *pctx, const vscpEvent *const pex)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv = VSCP_ERROR_SUCCESS;

  if (pex->sizeData > 0 && NULL == pex->pdata) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  for (uint8_t row = 0; row < pctx->nDmRows; row++) {

    // Is the DM row enabled?
    if (LEVEL1_DM_FLAGS(row) & VSCP_LEVEL1_DM_FLAG_ENABLED) {

      // Should the originating id be checked and if so is it the same?
      if ((LEVEL1_DM_FLAGS(row) & VSCP_LEVEL1_DM_FLAG_CHECK_OADDR) && (pex->GUID[15] != LEVEL1_DM_OADDR(row))) {
        continue;
      }

      // Check if zone should match and if so if it match (255 is wildcard)
      if (LEVEL1_DM_FLAGS(row) & VSCP_LEVEL1_DM_FLAG_CHECK_ZONE) {
        if (pex->sizeData > 1 && 255 != pex->pdata[1]) {
          if (pex->pdata[1] != pctx->zone) {
            continue;
          }
        }
      }

      // Check if subzone should match and if so if it match (255 is wildcard)
      if (LEVEL1_DM_FLAGS(row) & VSCP_LEVEL1_DM_FLAG_CHECK_SUBZONE) {
        if (pex->sizeData > 2 && 255 != pex->pdata[2]) {
          if (pex->pdata[2] != pctx->subzone) {
            continue;
          }
        }
      }

      if (!((LEVEL1_DM_CLASS_FILTER(row) ^ pex->vscp_class) & LEVEL1_DM_CLASS_MASK(row)) &&
          !((LEVEL1_DM_TYPE_FILTER(row) ^ pex->vscp_type) & LEVEL1_DM_TYPE_MASK(row))) {

        // OK Trigger this action
        if (VSCP_ERROR_SUCCESS !=
            (rv = pctx->ops->dm_action(pctx, pex, LEVEL1_DM_ACTION(row), &LEVEL1_DM_ACTION_PARAM(row)))) {
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
vscp_frmw2_feed_level2_dm(vscp_frmw2_firmware_context_t *pctx, const vscpEvent *const pex)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv = VSCP_ERROR_SUCCESS;

  if (pex->sizeData > 0 && NULL == pex->pdata) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  for (uint8_t row = 0; row < pctx->nDmRows; row++) {

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
        ((pex->GUID[0] != LEVEL2_DM_OADDR(row, 0)) || (pex->GUID[1] != LEVEL2_DM_OADDR(row, 1)) ||
         (pex->GUID[2] != LEVEL2_DM_OADDR(row, 2)) || (pex->GUID[3] != LEVEL2_DM_OADDR(row, 3)) ||
         (pex->GUID[4] != LEVEL2_DM_OADDR(row, 4)) || (pex->GUID[5] != LEVEL2_DM_OADDR(row, 5)) ||
         (pex->GUID[6] != LEVEL2_DM_OADDR(row, 6)) || (pex->GUID[7] != LEVEL2_DM_OADDR(row, 7)) ||
         (pex->GUID[8] != LEVEL2_DM_OADDR(row, 8)) || (pex->GUID[9] != LEVEL2_DM_OADDR(row, 9)) ||
         (pex->GUID[10] != LEVEL2_DM_OADDR(row, 10)) || (pex->GUID[11] != LEVEL2_DM_OADDR(row, 11)) ||
         (pex->GUID[12] != LEVEL2_DM_OADDR(row, 12)) || (pex->GUID[13] != LEVEL2_DM_OADDR(row, 13)) ||
         (pex->GUID[14] != LEVEL2_DM_OADDR(row, 14)) || (pex->GUID[15] != LEVEL2_DM_OADDR(row, 15)))) {
      continue;
    }

    // Check if index should match and if so if it match (255 is wildcard)
    if (LEVEL2_DM_FLAGS(row) & VSCP_LEVEL2_DM_FLAG_CHECK_INDEX) {
      if (pex->sizeData > 1 && 255 != pex->pdata[1]) {
        if (pex->pdata[1] != pctx->index) {
          continue;
        }
      }
    }

    // Check if zone should match and if so if it match (255 is wildcard)
    if (LEVEL2_DM_FLAGS(row) & VSCP_LEVEL2_DM_FLAG_CHECK_ZONE) {
      if (pex->sizeData > 1 && 255 != pex->pdata[1]) {
        if (pex->pdata[1] != pctx->zone) {
          continue;
        }
      }
    }

    // Check if subzone should match and if so if it match (255 is wildcard)
    if (LEVEL2_DM_FLAGS(row) & VSCP_LEVEL2_DM_FLAG_CHECK_SUBZONE) {
      if (pex->sizeData > 2 && 255 != pex->pdata[2]) {
        if (pex->pdata[2] != pctx->subzone) {
          continue;
        }
      }
    }

    // OK Trigger this action
    if (VSCP_ERROR_SUCCESS !=
        (rv = pctx->ops->dm_action(pctx, pex, LEVEL2_DM_ACTION(row), &LEVEL2_DM_ACTION_PARAM(row)))) {
      break;
    }

  } // for each row
  return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_frmw2_feed_dm
//

int
vscp_frmw2_feed_dm(vscp_frmw2_firmware_context_t *pctx, const vscpEvent *const pex)
{
  if (NULL == pctx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  int rv = VSCP_ERROR_SUCCESS;

  if (NULL != pex) {
    if (VSCP_LEVEL1 == pctx->level) {
      rv = vscp_frmw2_feed_level1_dm(pctx, pex);
    }
    else {
      rv = vscp_frmw2_feed_level2_dm(pctx, pex);
    }
  }

  return rv;
}