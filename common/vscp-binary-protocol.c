/* ******************************************************************************
 * VSCP (Very Simple Control Protocol)
 * http://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (C) 2000-2026 Ake Hedman,
 * The VSCP Project <info@grodansparadis.com>
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
 *  This file is part of VSCP - Very Simple Control Protocol
 *  http://www.vscp.org
 *
 * ******************************************************************************
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include <vscp.h>
#include <vscp-firmware-helper.h>
#ifdef VSCP_FWHLP_BINARY_FRAME_SUPPORT
#include <vscp-crc.h>
#endif

#include "vscp-binary-protocol.h"

#define VSCP_ERROR_UNKNOWN_COMMAND 72 /* This command is not available */

// ---------------------------------------------------------------------------
// Validate that pctx and the mandatory reply callback are non-NULL.
// ---------------------------------------------------------------------------
static inline int
vscp_binary_check_ctx(const vscp_binary_ctx_t *pctx)
{
  if (NULL == pctx || NULL == pctx->ops || NULL == pctx->ops->reply) {
    return VSCP_ERROR_INIT_MISSING;
  }
  return VSCP_ERROR_SUCCESS;
}


// ---------------------------------------------------------------------------
// Authentication. Applications that log in clients must set
// ops->check_authenticated. Without it every client is treated as
// authenticated (no login on this interface).
// ---------------------------------------------------------------------------
static bool
vscp_binary_is_authenticated(vscp_binary_ctx_t *pctx)
{
  if (NULL == pctx->ops->check_authenticated) {
    return true;
  }
  return (VSCP_ERROR_SUCCESS == pctx->ops->check_authenticated(pctx));
}

// Commands that are allowed before the client has logged in
static bool
vscp_binary_is_public_command(uint16_t command)
{
  switch (command) {
    case VSCP_BINARY_COMMAND_CODE_NOOP:
    case VSCP_BINARY_COMMAND_CODE_QUIT:
    case VSCP_BINARY_COMMAND_CODE_USER:
    case VSCP_BINARY_COMMAND_CODE_PASS:
    case VSCP_BINARY_COMMAND_CODE_CHALLENGE:
    case VSCP_BINARY_COMMAND_CODE_VERSION:
      return true;
    default:
      return false;
  }
}

// Copy a command argument to a null terminated string. The argument is not
// null terminated within len in the frame.
static void
vscp_binary_arg_to_str(char *str, size_t size, const uint8_t *parg, size_t len)
{
  size_t n = 0;
  if (NULL != parg) {
    n = (len < size - 1) ? len : size - 1;
    memcpy(str, parg, n);
  }
  str[n] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
// vscp_handle_binary_command
//

int
vscp_handle_binary_command(vscp_binary_ctx_t *pctx, uint16_t command, const uint8_t *parg, size_t len)
{
  int rv;

  if (VSCP_ERROR_SUCCESS != vscp_binary_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }

  if (!vscp_binary_is_public_command(command) && !vscp_binary_is_authenticated(pctx)) {
    return pctx->ops->reply(pctx, command, VSCP_ERROR_INVALID_PERMISSION, NULL, 0);
  }

  if (VSCP_BINARY_COMMAND_CODE_NOOP == command) {
    // No operation, just reply with success
    return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_QUIT == command) {
    // Quit command, close connection
    return pctx->ops->quit(pctx);
  }
  else if (VSCP_BINARY_COMMAND_CODE_USER == command) {
    char user[VSCP_BINARY_MAX_USERNAME_LENGTH + 1];
    if (NULL == pctx->ops->user) {
      return pctx->ops->reply(pctx, command, VSCP_ERROR_NOT_SUPPORTED, NULL, 0);
    }
    vscp_binary_arg_to_str(user, sizeof(user), parg, len);
    rv = pctx->ops->user(pctx, user);
    return pctx->ops->reply(pctx, command, rv, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_PASS == command) {
    char password[VSCP_BINARY_MAX_PASSWORD_LENGTH + 1];
    if (NULL == pctx->ops->password) {
      return pctx->ops->reply(pctx, command, VSCP_ERROR_NOT_SUPPORTED, NULL, 0);
    }
    vscp_binary_arg_to_str(password, sizeof(password), parg, len);
    rv = pctx->ops->password(pctx, password);
    // Do not leave the password on the stack
    volatile char *pw = password;
    for (size_t i = 0; i < sizeof(password); i++) {
      pw[i] = 0;
    }
    pctx->ops->reply(pctx, command, rv, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_CHALLENGE == command) {
    uint8_t challenge[16];
    rv = pctx->ops->challenge(pctx, challenge, sizeof(challenge));
    return pctx->ops->reply(pctx, command, rv, challenge, sizeof(challenge));
  }
  else if (VSCP_BINARY_COMMAND_CODE_SEND == command) {
    vscp_event_t *pev = (vscp_event_t *) calloc(1, sizeof(vscpEvent));
    if (NULL == pev) {
      return pctx->ops->reply(pctx, command, VSCP_ERROR_MEMORY, NULL, 0);
    }

    // Parse event data from parg and fill ex structure
    if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_getEventFromFrame(pev, parg, len))) {
      vscp_fwhlp_deleteEvent(&pev);
      return pctx->ops->reply(pctx, command, rv, NULL, 0);
    }

    int rv = pctx->ops->send_event(pctx, pev);
    vscp_fwhlp_deleteEvent(&pev);

    return pctx->ops->reply(pctx, command, rv, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_RETR == command) {
    uint16_t count = 1; // Default to retrieving one event

    // If the channel is open for asynchronous events then RETR command is not needed and
    // should return VSCP_ERROR_UNSUPPORTED indicating that events will be sent asynchronously.
    if (pctx->ops->is_open(pctx)) {
      return pctx->ops->reply(pctx, command, VSCP_ERROR_UNSUPPORTED, NULL, 0);
    }

    // Check argument length
    if (len >= 2) {
      count = (parg[0] << 8) | parg[1];
    }

    for (int i = 0; i < count; i++) {
      int rv;
      vscp_event_t *pev = NULL;
      if (VSCP_ERROR_SUCCESS == (rv = pctx->ops->get_event(pctx, &pev))) {

        // Send event data back to client
        size_t buflen = vscp_fwhlp_getFrameSizeFromEvent(pev);

        // Allocate buffer for frame data
        uint8_t *buf = (uint8_t *) malloc(buflen);
        if (NULL == buf) {
          vscp_fwhlp_deleteEvent(&pev);
          return pctx->ops->reply(pctx, command, VSCP_ERROR_MEMORY, NULL, 0);
        }

        // Frame to buffer (this has encodingbyte and crc which we do not need here)
        if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_writeEventToFrame(buf, buflen, 0, pev))) {
          free(buf);
          vscp_fwhlp_deleteEvent(&pev);
          return pctx->ops->reply(pctx, command, rv, NULL, 0);
        }

        // Send reply with event data (point beyond leading byte. That byte + crc is subtracted from lenth)
        if (VSCP_ERROR_SUCCESS !=
            (rv = pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, buf + 1, buflen - 3))) {
          free(buf);
          vscp_fwhlp_deleteEvent(&pev);
          return rv;
        }

        free(buf);
        vscp_fwhlp_deleteEvent(&pev);
      }
      else if (VSCP_ERROR_FIFO_EMPTY == rv) {
        // No more events to retrieve
        break;
      }
      else {
        // Error occurred while retrieving event
        return pctx->ops->reply(pctx, command, rv, NULL, 0);
      }
    } // for

    // All was OK
    return pctx->ops->reply(pctx, command, 0, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_OPEN == command) {
    rv = pctx->ops->open(pctx);
    return pctx->ops->reply(pctx, command, rv, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_CLOSE == command) {
    rv = pctx->ops->close(pctx);
    return pctx->ops->reply(pctx, command, rv, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_CHKDATA == command) {
    uint32_t count;
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->check_data(pctx, &count))) {
      return pctx->ops->reply(pctx, command, rv, NULL, 0);
    }
    else {
      uint8_t buf[4];
      buf[0] = (count >> 24) & 0xFF;
      buf[1] = (count >> 16) & 0xFF;
      buf[2] = (count >> 8) & 0xFF;
      buf[3] = count & 0xFF;

      // Send reply with count of events ready to be received
      return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, buf, sizeof(buf));
    }
  }
  else if (VSCP_BINARY_COMMAND_CODE_CLEAR == command) {
    rv = pctx->ops->clrall(pctx);
    return pctx->ops->reply(pctx, command, rv, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_STAT == command) {
    vscp_statistics_t statistics;
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->statistics(pctx, &statistics))) {
      return pctx->ops->reply(pctx, command, rv, NULL, 0);
    }
    else {
      uint8_t buf[28];
      // Fill buf with statistical information from statistics structure
      buf[0] = (statistics.x >> 24) & 0xFF;
      buf[1] = (statistics.x >> 16) & 0xFF;
      buf[2] = (statistics.x >> 8) & 0xFF;
      buf[3] = statistics.x & 0xFF;

      buf[4] = (statistics.y >> 24) & 0xFF;
      buf[5] = (statistics.y >> 16) & 0xFF;
      buf[6] = (statistics.y >> 8) & 0xFF;
      buf[7] = statistics.y & 0xFF;

      buf[8]  = (statistics.z >> 24) & 0xFF;
      buf[9]  = (statistics.z >> 16) & 0xFF;
      buf[10] = (statistics.z >> 8) & 0xFF;
      buf[11] = statistics.z & 0xFF;

      buf[12] = (statistics.cntReceiveData >> 24) & 0xFF;
      buf[13] = (statistics.cntReceiveData >> 16) & 0xFF;
      buf[14] = (statistics.cntReceiveData >> 8) & 0xFF;
      buf[15] = statistics.cntReceiveData & 0xFF;

      buf[16] = (statistics.cntReceiveFrames >> 24) & 0xFF;
      buf[17] = (statistics.cntReceiveFrames >> 16) & 0xFF;
      buf[18] = (statistics.cntReceiveFrames >> 8) & 0xFF;
      buf[19] = statistics.cntReceiveFrames & 0xFF;

      buf[20] = (statistics.cntTransmitData >> 24) & 0xFF;
      buf[21] = (statistics.cntTransmitData >> 16) & 0xFF;
      buf[22] = (statistics.cntTransmitData >> 8) & 0xFF;
      buf[23] = statistics.cntTransmitData & 0xFF;

      buf[24] = (statistics.cntTransmitFrames >> 24) & 0xFF;
      buf[25] = (statistics.cntTransmitFrames >> 16) & 0xFF;
      buf[26] = (statistics.cntTransmitFrames >> 8) & 0xFF;
      buf[27] = statistics.cntTransmitFrames & 0xFF;

      // Send reply with statistical information
      return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, buf, sizeof(buf));
    }
  }
  else if (VSCP_BINARY_COMMAND_CODE_INFO == command) {
    vscp_status_t status;
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->info(pctx, &status))) {
      return pctx->ops->reply(pctx, command, rv, NULL, 0);
    }
    else {
      uint8_t buf[12];
      // Fill buf with status information from status structure
      buf[0] = (status.channel_status >> 24) & 0xFF;
      buf[1] = (status.channel_status >> 16) & 0xFF;
      buf[2] = (status.channel_status >> 8) & 0xFF;
      buf[3] = status.channel_status & 0xFF;

      buf[4] = (status.lasterrorcode >> 24) & 0xFF;
      buf[5] = (status.lasterrorcode >> 16) & 0xFF;
      buf[6] = (status.lasterrorcode >> 8) & 0xFF;
      buf[7] = status.lasterrorcode & 0xFF;

      buf[8]  = (status.lasterrorsubcode >> 24) & 0xFF;
      buf[9]  = (status.lasterrorsubcode >> 16) & 0xFF;
      buf[10] = (status.lasterrorsubcode >> 8) & 0xFF;
      buf[11] = status.lasterrorsubcode & 0xFF;

      // Send reply with status information
      return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, buf, sizeof(buf));
    }
  }
  else if (VSCP_BINARY_COMMAND_CODE_GETCHID == command) {
    uint32_t chid;
    rv = pctx->ops->get_chid(pctx, &chid);
    if (VSCP_ERROR_SUCCESS != rv) {
      return pctx->ops->reply(pctx, command, rv, NULL, 0);
    }
    else {
      // Send reply with channel id
      uint8_t buf[4];
      buf[0] = (chid >> 24) & 0xFF;
      buf[1] = (chid >> 16) & 0xFF;
      buf[2] = (chid >> 8) & 0xFF;
      buf[3] = chid & 0xFF;
      return pctx->ops->reply(pctx, command, rv, buf, sizeof(buf));
    }
  }
  else if (VSCP_BINARY_COMMAND_CODE_SETGUID == command) {
    uint8_t guid[16];
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->set_guid(pctx, guid))) {
      return rv;
    }
    // Send reply with GUID
    return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_GETGUID == command) {
    uint8_t guid[16];
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->get_guid(pctx, guid))) {
      return pctx->ops->reply(pctx, command, rv, NULL, 0);
    }
    // Send reply with GUID
    return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, guid, 16);
  }
  else if (VSCP_BINARY_COMMAND_CODE_VERSION == command) {
    uint8_t version[10]; // 2 + 2 + 2 + 4
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->get_version(pctx, version))) {
      return pctx->ops->reply(pctx, command, rv, version, sizeof(version));
    }
    // Send reply with version
    return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, version, sizeof(version));
  }
  else if (VSCP_BINARY_COMMAND_CODE_SETFILTER == command) {
    vscpEventFilter filter;

    /*
      len can be 21 or 42 as it is possible to send filter AND mask in the same command
    */
    if (len < 21) {
      return pctx->ops->reply(pctx, command, VSCP_ERROR_PARAMETER, NULL, 0);
    }

    // read filter part of argument
    filter.filter_priority = parg[0];
    filter.filter_class    = (parg[1] << 8) | parg[2];
    filter.filter_type     = (parg[3] << 8) | parg[4];
    memcpy(filter.filter_GUID, parg + 5, 16);

    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->setfilter(pctx, &filter))) {
      return pctx->ops->reply(pctx, command, rv, NULL, 0);
    }

    if (len >= 42) {
      // read mask part of argument if present (mask part is ignored by callback but can be used by client to keep track
      // of mask)
      filter.mask_priority = parg[21];
      filter.mask_class    = (parg[22] << 8) | parg[23];
      filter.mask_type     = (parg[24] << 8) | parg[25];
      memcpy(filter.mask_GUID, parg + 26, 16);

      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->setmask(pctx, &filter))) {
        return pctx->ops->reply(pctx, command, rv, NULL, 0);
      }
    }

    return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_SETMASK == command) {
    vscpEventFilter filter;
    if (len < 21) {
      return pctx->ops->reply(pctx, command, VSCP_ERROR_PARAMETER, NULL, 0);
    }

    // read mask part of argument
    filter.mask_priority = parg[0];
    filter.mask_class    = (parg[1] << 8) | parg[2];
    filter.mask_type     = (parg[3] << 8) | parg[4];
    memcpy(filter.mask_GUID, parg + 5, 16);

    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->setmask(pctx, &filter))) {
      return pctx->ops->reply(pctx, command, rv, NULL, 0);
    }

    return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_INTERFACE == command) {
    /*!
      Argument = 0 - Get interface count
      Argument = 1 - Get interface information interface. (index as second argument)
      Argument = 2 - Close interface (index as second argument)
      Argument = 3 - Open interface (index as second argument)
    */

    if (len == 1 && parg[0] == 0) {
      // Get interface count
      uint16_t count;
      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->get_interface_count(pctx, &count))) {
        return pctx->ops->reply(pctx, command, rv, NULL, 0);
      }
      // Send reply with count
      uint8_t buf[2];
      buf[0] = (count >> 8) & 0xFF;
      buf[1] = count & 0xFF;
      return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, buf, sizeof(buf));
    }
    else if (len >= 3 && parg[0] == 1) {
      // Get interface information
      vscp_interface_info_t ifinfo;

      // Interface index
      uint16_t idx = (parg[1] << 8) | parg[2];

      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->get_interface(pctx, idx, &ifinfo))) {
        return pctx->ops->reply(pctx, command, rv, NULL, 0);
      }

      uint8_t buf[2 + 2 + 16 + 64] = { 0 };
      buf[0]                       = (ifinfo.idx >> 8) & 0xFF;
      buf[1]                       = ifinfo.idx & 0xFF;
      buf[2]                       = (ifinfo.idx >> 8) & 0xFF;
      buf[3]                       = ifinfo.idx & 0xFF;
      memcpy(buf + 4, ifinfo.guid, 16);
      memcpy(buf + 20, ifinfo.description, 64); // Interface description
      return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, buf, sizeof(buf));
    }
    else if (len >= 3 && parg[0] == 2) {
      // Close interface

      // Interface index
      uint16_t idx = (parg[1] << 8) | parg[2];

      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->interface_close(pctx, idx))) {
        return pctx->ops->reply(pctx, command, rv, NULL, 0);
      }
      // Send reply confirming interface was closed
      return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, NULL, 0);
    }
    else if (len >= 3 && parg[0] == 3) {
      // Open interface

      // Interface index
      uint16_t idx = (parg[1] << 8) | parg[2];

      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->interface_open(pctx, idx))) {
        return pctx->ops->reply(pctx, command, rv, NULL, 0);
      }
      // Send reply confirming interface was opened    return pctx->ops->reply(pctx, command,
      // VSCP_ERROR_SUCCESS, NULL, 0);
      return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, NULL, 0);
    }

    return pctx->ops->reply(pctx, command, VSCP_ERROR_PARAMETER, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_TEST == command) {
    return pctx->ops->test(pctx, parg, len);
  }
  else if (VSCP_BINARY_COMMAND_CODE_TEXT == command) {
    // We normally do nothing here except resetting binary flag
    rv = pctx->ops->text(pctx);
    return pctx->ops->reply(pctx, command, rv, NULL, 0);
  }
  else if (VSCP_BINARY_COMMAND_CODE_WCYD == command) {
    uint64_t capabilities = 0;
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->wcyd(pctx, &capabilities))) {
      return pctx->ops->reply(pctx, command, rv, NULL, 0);
    }
    // Send reply with capabilities
    uint8_t buf[8];
    for (int i = 0; i < 8; i++) {
      buf[i] = (capabilities >> (56 - i * 8)) & 0xFF;
    }
    return pctx->ops->reply(pctx, command, VSCP_ERROR_SUCCESS, buf, sizeof(buf));
  }
  else if (VSCP_BINARY_COMMAND_CODE_SHUTDOWN == command) {
    // Reply and optional shutdown are handled inside the callback
    return pctx->ops->shutdown(pctx);
  }
  else if (VSCP_BINARY_COMMAND_CODE_RESTART == command) {
    // Reply is sent inside the callback before esp_restart() is called
    return pctx->ops->restart(pctx);
  }
  else if (command >= VSCP_BINARY_COMMAND_CODE_USER_START) {
    // User defined command, pass to callback
    return pctx->ops->user_command(pctx, command, parg, len);
  }
  else {
    // Unknown command
    return VSCP_ERROR_UNKNOWN_COMMAND;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_handle_binary_event
//

int
vscp_handle_binary_event(vscp_binary_ctx_t *pctx, vscp_event_t *pEvent)
{
  int rv;

  if (VSCP_ERROR_SUCCESS != vscp_binary_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }

  if (NULL == pEvent) {
    return VSCP_ERROR_PARAMETER;
  }

  // Only logged in clients may send events
  if (!vscp_binary_is_authenticated(pctx)) {
    rv = VSCP_ERROR_INVALID_PERMISSION;
  }
  else if (NULL == pctx->ops->send_event) {
    rv = VSCP_ERROR_NOT_SUPPORTED;
  }
  else {
    rv = pctx->ops->send_event(pctx, pEvent);
  }

  // Event is deleted in caller routine

  // Fill the reply buffer with data
  uint8_t buf[4] = { 0 };
  buf[0]         = (pEvent->head >> 8) & 0xFF;
  buf[1]         = pEvent->head & 0xFF;
  buf[2]         = (pEvent->crc >> 8) & 0xFF;
  buf[3]         = pEvent->crc & 0xFF;

  return pctx->ops->reply(pctx, VSCP_BINARY_COMMAND_CODE_EVENT_CONFIRM, rv, buf, sizeof(buf));
}

/* ── TCP record layer ─────────────────────────────────────────────────────── */

#if defined(VSCP_FWHLP_CRYPTO_SUPPORT) && defined(VSCP_FWHLP_BINARY_FRAME_SUPPORT)

static void
vscp_binary_wipe(void *p, size_t len)
{
  volatile uint8_t *v = (volatile uint8_t *) p;
  while (len--) {
    *v++ = 0;
  }
}

int
vscp_binary_set_authentication(vscp_binary_ctx_t *pctx,
                               uint8_t algorithm,
                               const uint8_t *aesKey,
                               size_t aesKeyLen,
                               const uint8_t *hmacKey)
{
  size_t expected;

  if ((NULL == pctx) || (NULL == aesKey) || (NULL == hmacKey)) {
    return VSCP_ERROR_PARAMETER;
  }

  switch (algorithm) {
    case VSCP_ENCRYPTION_AES128:
      expected = 16;
      break;
    case VSCP_ENCRYPTION_AES192:
      expected = 24;
      break;
    case VSCP_ENCRYPTION_AES256:
      expected = 32;
      break;
    default:
      return VSCP_ERROR_PARAMETER;
  }

  if (aesKeyLen != expected) {
    return VSCP_ERROR_PARAMETER;
  }

  // Never use the AES key as HMAC key
  if (0 == memcmp(aesKey, hmacKey, expected)) {
    return VSCP_ERROR_PARAMETER;
  }

  vscp_binary_clear_authentication(pctx);
  memcpy(pctx->aes_key, aesKey, expected);
  memcpy(pctx->hmac_key, hmacKey, sizeof(pctx->hmac_key));
  pctx->auth_algorithm = algorithm;
  pctx->auth           = 1;
  return VSCP_ERROR_SUCCESS;
}

void
vscp_binary_clear_authentication(vscp_binary_ctx_t *pctx)
{
  if (NULL == pctx) {
    return;
  }
  pctx->auth           = 0;
  pctx->auth_algorithm = 0;
  vscp_binary_wipe(pctx->aes_key, sizeof(pctx->aes_key));
  vscp_binary_wipe(pctx->hmac_key, sizeof(pctx->hmac_key));
}

#endif

#ifdef VSCP_FWHLP_BINARY_FRAME_SUPPORT

int
vscp_binary_handle_record(vscp_binary_ctx_t *pctx, uint8_t *buf, size_t len)
{
  if (VSCP_ERROR_SUCCESS != vscp_binary_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }

  if ((NULL == buf) || (0 == len)) {
    return VSCP_ERROR_INVALID_FRAME;
  }

#ifdef VSCP_FWHLP_CRYPTO_SUPPORT
  if (pctx->auth) {
    // Only authentic version 1 frames with the configured algorithm
    if ((buf[0] & 0x0f) != pctx->auth_algorithm) {
      return VSCP_ERROR_INVALID_FRAME;
    }
    size_t plainLen = vscp_fwhlp_decryptFrameV1(buf, len, buf, len, pctx->aes_key, pctx->hmac_key);
    if (0 == plainLen) {
      return VSCP_ERROR_INVALID_FRAME;
    }
    buf[0] &= 0xf0; // Decrypted now
    len = plainLen;
  }
  else
#endif
    if (buf[0] & 0x0f) {
    // Encrypted frames need authentication to be configured
    return VSCP_ERROR_INVALID_FRAME;
  }

  const uint8_t type = buf[0] >> 4;

  // Command: type(1) + command(2) [+ arguments] + CRC(2)
  if (0x0e == type) {
    if (len < 5) {
      return VSCP_ERROR_INVALID_FRAME;
    }
    const uint16_t expected = (uint16_t) (((uint16_t) buf[len - 2] << 8) | buf[len - 1]);
    if (crcSlow((unsigned char const *) buf + 1, (int) (len - 3)) != expected) {
      return VSCP_ERROR_INVALID_FRAME;
    }
    const uint16_t command = (uint16_t) (((uint16_t) buf[1] << 8) | buf[2]);
    return vscp_handle_binary_command(pctx, command, (len > 5) ? (buf + 3) : NULL, len - 5);
  }

  // Reply from the client (e.g. event confirmation) - nothing to do
  if (0x0f == type) {
    return (len >= 7) ? VSCP_ERROR_SUCCESS : VSCP_ERROR_INVALID_FRAME;
  }

  // Event (frame format 0 or 1)
  if (type <= 1) {
    vscp_event_t *pEvent = (vscp_event_t *) calloc(1, sizeof(vscp_event_t));
    if (NULL == pEvent) {
      return VSCP_ERROR_MEMORY;
    }
    // Checks size and CRC
    int rv = vscp_fwhlp_getEventFromFrame(pEvent, buf, len);
    if (VSCP_ERROR_SUCCESS != rv) {
      vscp_fwhlp_deleteEvent(&pEvent);
      return VSCP_ERROR_INVALID_FRAME;
    }
    rv = vscp_handle_binary_event(pctx, pEvent);
    vscp_fwhlp_deleteEvent(&pEvent);
    return rv;
  }

  return VSCP_ERROR_INVALID_FRAME;
}

size_t
vscp_binary_wrap_record(vscp_binary_ctx_t *pctx,
                        uint8_t *out,
                        size_t capacity,
                        const uint8_t *frame,
                        size_t len)
{
  size_t innerLen;

  if ((NULL == pctx) || (NULL == out) || (NULL == frame) || (0 == len) || (capacity < 3)) {
    return 0;
  }

#ifdef VSCP_FWHLP_CRYPTO_SUPPORT
  if (pctx->auth) {
    innerLen = vscp_fwhlp_encryptFrameV1(out + 2,
                                         capacity - 2,
                                         frame,
                                         len,
                                         pctx->aes_key,
                                         pctx->hmac_key,
                                         NULL, // Random IV
                                         pctx->auth_algorithm);
    if (0 == innerLen) {
      return 0;
    }
  }
  else
#endif
  {
    if ((len > 0xffff) || (capacity < (len + 2))) {
      return 0;
    }
    memmove(out + 2, frame, len);
    innerLen = len;
  }

  out[0] = (uint8_t) (innerLen >> 8);
  out[1] = (uint8_t) innerLen;
  return innerLen + 2;
}

#endif // VSCP_FWHLP_BINARY_FRAME_SUPPORT
