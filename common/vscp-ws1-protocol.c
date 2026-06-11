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

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#ifdef VSCP_WS1_PLATFORM_ESP_IDF
#include <esp_log.h>
#include "esp_log_buffer.h"
#define WS1_LOGI(tag, fmt, ...) WS1_LOGI(tag, fmt, ##__VA_ARGS__)
#define WS1_LOGE(tag, fmt, ...) WS1_LOGE(tag, fmt, ##__VA_ARGS__)
#else
#define WS1_LOGI(tag, fmt, ...) ((void)(tag))
#define WS1_LOGE(tag, fmt, ...) ((void)(tag))
#define ESP_LOG_BUFFER_HEXDUMP(tag, buf, len, lvl) ((void)(tag))
#define ESP_LOG_INFO 3
#endif

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include "vscp.h"
#include "vscp-crc.h"

#include "vscp-binary-protocol.h"
#include "vscp-ws1-protocol.h"

#define TAG __func__

#define VSCP_WS1_SID_SIZE           16
#define VSCP_WS1_MAX_CRYPTO_BIN_LEN 128

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_check_ctx
// Validate that the context and mandatory ops are set.
// Returns VSCP_ERROR_SUCCESS if valid, VSCP_ERROR_INIT_MISSING otherwise.
//

static inline int
vscp_ws1_check_ctx(const vscp_ws1_ctx_t *pctx)
{
  if ((NULL == pctx) || (NULL == pctx->ops) || (NULL == pctx->ops->reply)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  return VSCP_ERROR_SUCCESS;
}

/* Forward declaration — vscp_ws1_handle_command is defined after the binary handler */
static int
vscp_ws1_handle_command(const char *pCommand, const char *parg, vscp_ws1_ctx_t *pctx);

//

static bool
vscp_ws1_is_hex_string(const char *str)
{
  if (NULL == str) {
    return false;
  }

  for (const char *p = str; *p; ++p) {
    if (!isxdigit((unsigned char) *p)) {
      return false;
    }
  }

  return true;
}

static bool
vscp_ws1_is_zero_padding(const uint8_t *buf, size_t start, size_t end)
{
  if (NULL == buf) {
    return false;
  }

  for (size_t idx = start; idx < end; ++idx) {
    if (0 != buf[idx]) {
      return false;
    }
  }

  return true;
}

static size_t
vscp_ws1_get_decrypted_binary_frame_len(const uint8_t *buf, size_t max_len)
{
  if ((NULL == buf) || (0 == max_len)) {
    return 0;
  }

  const uint8_t frame_type = buf[0] & 0xF0;

  if ((0xE0 == frame_type) || (0xF0 == frame_type)) {
    const size_t min_len = (0xE0 == frame_type) ? 5 : 7;
    for (size_t candidate = min_len; candidate <= max_len; ++candidate) {
      if (!vscp_ws1_is_zero_padding(buf, candidate, max_len)) {
        continue;
      }

      if (0 == crcFast((unsigned char const *) buf + 1, candidate - 1)) {
        return candidate;
      }
    }

    return 0;
  }

  if (0x00 == frame_type) {
    const size_t calc_frame_size = 1 +
                                   VSCP_BINARY_PACKET_FRAME0_HEADER_LENGTH +
                                   2 +
                                   ((uint16_t) buf[VSCP_BINARY_PACKET_FRAME0_POS_SIZE_MSB] << 8) +
                                   buf[VSCP_BINARY_PACKET_FRAME0_POS_SIZE_LSB];

    if ((calc_frame_size > max_len) || !vscp_ws1_is_zero_padding(buf, calc_frame_size, max_len)) {
      return 0;
    }

    crc crc_frame = ((uint16_t) buf[calc_frame_size - 2] << 8) + buf[calc_frame_size - 1];
    if ((buf[VSCP_BINARY_PACKET_FRAME0_POS_HEAD_LSB] & VSCP_HEADER_NO_CRC) &&
        (VSCP_NOCRC_CALC_DUMMY_CRC == crc_frame)) {
      return calc_frame_size;
    }

    if (0 == crcFast((unsigned char const *) buf + 1, calc_frame_size - 1)) {
      return calc_frame_size;
    }
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_init
//

int
vscp_ws1_init(vscp_ws1_ctx_t *pctx, void *pdata)
{
  // Validate mandatory ops before touching the context
  if ((NULL == pctx) || (NULL == pctx->ops) ||
      (NULL == pctx->ops->reply) || (NULL == pctx->ops->generate_sid)) {
    return VSCP_ERROR_INIT_MISSING;
  }

  // Save ops pointer: memset will clear the struct so we restore it after
  const vscp_ws1_ops_t *saved_ops = pctx->ops;

  int rv;
  char buf[VSCP_WS1_MAX_PACKET_SIZE] = { 0 };

  // Initialize the connection context
  memset(pctx, 0, sizeof(vscp_ws1_ctx_t));
  pctx->ops            = saved_ops;
  pctx->pdata          = pdata;
  pctx->bAuthenticated = false;
  pctx->bOpen          = false;

  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->generate_sid(pctx, pctx->sid, sizeof(pctx->sid)))) {
    WS1_LOGE(TAG, "Failed to generate SID with error %d", rv);
    return rv;
  }

  // Send initial sid
  WS1_LOGI(TAG, "Sending generated SID");
  sprintf(buf,
          "+;AUTH0;%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
          pctx->sid[0],
          pctx->sid[1],
          pctx->sid[2],
          pctx->sid[3],
          pctx->sid[4],
          pctx->sid[5],
          pctx->sid[6],
          pctx->sid[7],
          pctx->sid[8],
          pctx->sid[9],
          pctx->sid[10],
          pctx->sid[11],
          pctx->sid[12],
          pctx->sid[13],
          pctx->sid[14],
          pctx->sid[15]);
  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->reply(pctx, buf))) {
    WS1_LOGE(TAG, "Failed to send initial SID reply rv=%d", rv);
    return rv;
  }

  if ((NULL != pctx->ops->init) &&
      (VSCP_ERROR_SUCCESS != (rv = pctx->ops->init(pctx)))) {
    WS1_LOGE(TAG, "Failed to perform WS1 callback init");
    return rv;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_clearup
//

int
vscp_ws1_clearup(vscp_ws1_ctx_t *pctx, void *pdata)
{
  if (VSCP_ERROR_SUCCESS != vscp_ws1_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (NULL != pctx->ops->cleanup) {
    int rv;
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->cleanup(pctx))) {
      WS1_LOGE(TAG, "Failed to perform WS1 callback cleanup");
      return rv;
    }
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_generate_sid
//

int
vscp_ws1_generate_sid(vscp_ws1_ctx_t *pctx, uint8_t *sid, size_t size)
{
  // Generate a random SID (session ID) for authentication and encryption
  // In a real implementation, this should be done using a secure random generator
  for (size_t i = 0; i < size; i++) {
    sid[i] = rand() % 256;
  }
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_handle_text_protocol_request
//
// Packets are
// Command:        'C' ; command ; optional data that may be separated by additional semicolons.
// Reply:          '+' ; 'command'
// Negative reply: '-' ; 'command' ; Error code ; Error in real text
// Event:          'E' ; head , vscp_class , vscp_type ,obid, datetime, timestamp, GUID, data
//

int
vscp_ws1_handle_text_protocol_request(vscp_ws1_ctx_t *pctx, const char *pframe, uint16_t len)
{
  if (VSCP_ERROR_SUCCESS != vscp_ws1_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  // uint8_t frame_type = VSCP_WS1_PKT_TYPE_UNKNOWN;
  char frame_buf[VSCP_WS1_MAX_PACKET_SIZE];
  char *pCommand; // Pointer to command part of packet

  WS1_LOGI(TAG, "Handling text protocol WS1");

  if (NULL == pframe || 0 == len || len >= sizeof(frame_buf)) {
    return VSCP_ERROR_INVALID_FRAME;
  }

  memcpy(frame_buf, pframe, len);
  frame_buf[len] = '\0';
  char *p        = frame_buf;

  // Command
  if (*p == 'C') {
    // frame_type = VSCP_WS1_PKT_TYPE_COMMAND;
    p++;
    if (';' != *p) {
      // Malformed packet, command part must be separated by ';'
      return VSCP_ERROR_INVALID_FRAME;
    }
    p++;
    if (!*p) {
      // No command part
      return VSCP_ERROR_INVALID_FRAME;
    }

    // Point at command part of packet
    pCommand = p;

    // Find next ';' or end of string to determine end of command part
    while (*p && *p != ';') {
      p++;
    }
    if (*p) {
      *p = 0; // Null-terminate command part
      p++;
    }

    // p now point to optional data part of packet (if any) or end of string

    WS1_LOGI(TAG, "Received command: %s arg: %s", pCommand, p ? p : "(none)");
    vscp_ws1_handle_command(pCommand, p, pctx);
  }
  // Received event
  else if (*p == 'E') {
    // frame_type        = VSCP_WS1_PKT_TYPE_EVENT;
    // TODO: parse event from text frame and call ops->send_event
    // (event parsing from text not yet implemented)
    (void) 0;
  }
  // Positive respone
  else if (*p == '+') {
    // frame_type = VSCP_WS1_PKT_TYPE_POSITIVE_RESPONSE;
  }
  // Negative response
  else if (*p == '-') {
    // frame_type = VSCP_WS1_PKT_TYPE_NEGATIVE_RESPONSE;
  }
  // Unknown packet type
  else {
    // Unknown packet type
    return VSCP_ERROR_INVALID_FRAME;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_handle_binary_protocol_request
//
// VSCP general binary protocol
// https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_over_binary
//

int
vscp_ws1_handle_binary_protocol_request(vscp_ws1_ctx_t *pctx, const uint8_t *pframe, size_t len)
{
  int rv;

  WS1_LOGI(TAG, "Handling binary protocol WS1");

  WS1_LOGI(TAG, "Dumping binary command buffer:");
  ESP_LOG_BUFFER_HEXDUMP(TAG, pframe, len, ESP_LOG_INFO);

  const size_t pbuf_size = 1 + VSCP_BINARY_PACKET_FRAME0_HEADER_LENGTH + 2 + 512;
  uint8_t *pbuf = (uint8_t *) calloc(1, pbuf_size); // Allocate buffer for decrypted frame
  if (NULL == pbuf) {
    WS1_LOGE(TAG, "Failed to allocate memory for binary frame buffer");
    return VSCP_ERROR_MEMORY;
  }

  if ((NULL == pframe) || (0 == len) || (len > pbuf_size)) {
    free(pbuf);
    WS1_LOGE(TAG,
             "Invalid binary frame received: len=%zu (max %zu) %s",
             len,
             pbuf_size,
             (NULL == pframe) ? "frame is NULL" : "frame is not NULL");
    return VSCP_ERROR_INVALID_FRAME;
  }

  // We only support frame format 0,14 and 15 in this implementation
  if ((0 != (pframe[0] & 0xf0)) && (0XF0 != (pframe[0] & 0xf0)) && (0xe0 != (pframe[0] & 0xf0))) {
    free(pbuf);
    WS1_LOGE(TAG, "Unsupported binary frame format");
    return VSCP_ERROR_INVALID_FRAME;
  }

  size_t frame_len = len;

  if (pframe[0] & 0x0f) {
    // Encrypted frame: low nibble of byte 0 selects algorithm; trailing 16 bytes are the IV
    if (VSCP_ERROR_SUCCESS !=
        (rv = vscp_fwhlp_decryptFrame(
           pbuf,
           pframe,
           len - 16,                              // Exclude appended IV from data length
           pctx->ops->get_primary_key(pctx),
           pframe + len - 16,                     // IV is the last 16 bytes
           VSCP_ENCRYPTION_FROM_TYPE_BYTE))) {
      free(pbuf);
      WS1_LOGE(TAG, "Failed to decrypt binary frame with error %d", rv);
      return rv;
    }

    frame_len = vscp_ws1_get_decrypted_binary_frame_len(pbuf, len - 16);
    if (0 == frame_len) {
      free(pbuf);
      WS1_LOGE(TAG, "Failed to determine decrypted binary frame length");
      return VSCP_ERROR_INVALID_FRAME;
    }
  }
  else {
    // No encryption: copy frame as-is into pbuf
    memcpy(pbuf, pframe, len);
  }

  // Minimum length depends on frame type
  size_t min_len;
  if (0xe0 == (pbuf[0] & 0xf0)) {
    min_len = 5; // type(1) + cmd(2) + crc(2)
  }
  else if (0xf0 == (pbuf[0] & 0xf0)) {
    min_len = 7; // type(1) + cmd(2) + error(2) + crc(2)
  }
  else {
    min_len = 1 + VSCP_BINARY_PACKET_FRAME0_HEADER_LENGTH + 2; // event frame
  }

  if (frame_len < min_len) {
    free(pbuf);
    WS1_LOGE(TAG, "Binary frame too short: %zu < %zu", frame_len, min_len);
    return VSCP_ERROR_INVALID_FRAME;
  }

  // Command
  if (0xe0 == (pbuf[0] & 0xf0)) {

    const uint8_t *parg = pbuf + 3; // Point at argument part of packet (after type and command bytes)
    // frame_type = VSCP_WS1_PKT_TYPE_COMMAND;

    uint16_t command = (uint16_t) pbuf[1] << 8 | (uint8_t) pbuf[2];
    size_t arg_len   = (frame_len >= 5) ? (frame_len - 5) : 0; // exclude type(1) + cmd(2) + crc(2)

    // Pass command and argument to command handler callback
    WS1_LOGI(TAG, "Received binary command: 0x%04X arg_len=%zu", command, arg_len);
    vscp_handle_binary_command(&pctx->binary_ctx, command, parg, arg_len);
  }
  // Reply
  else if (0xf0 == (pbuf[0] & 0xf0)) {
    // frame_type = VSCP_WS1_PKT_TYPE_POSITIVE_RESPONSE;
  }
  // Event
  else if (0x00 == (pbuf[0] & 0xf0)) {
    // frame_type        = VSCP_WS1_PKT_TYPE_EVENT;
    vscp_event_t *pEvent = calloc(1, sizeof(vscpEvent));
    if (NULL == pEvent) {
      free(pbuf);
      return VSCP_ERROR_MEMORY;
    }

    // Parse event data from packet (p should be in the format "E;head;
    int rv = vscp_fwhlp_getEventFromFrame(pEvent, pbuf, frame_len);
    if (VSCP_ERROR_SUCCESS != rv) {
      vscp_fwhlp_deleteEvent(&pEvent);
      free(pbuf);
      WS1_LOGE(TAG, "Failed to parse event from binary frame with error %d", rv);
      return rv;
    }

    // Not needed anymore
    free(pbuf);

    rv = vscp_handle_binary_event(&pctx->binary_ctx, pEvent);

    vscp_fwhlp_deleteEvent(&pEvent);
    if (VSCP_ERROR_SUCCESS != rv) {
      WS1_LOGE(TAG, "Failed to handle event from binary frame with error %d", rv);
      return rv;
    }

    return VSCP_ERROR_SUCCESS;
  }
  // Positive respone
  // else if (*p == '+') {
  //   frame_type = VSCP_WS1_PKT_TYPE_POSITIVE_RESPONSE;
  // }
  // // Negative response
  // else if (*p == '-') {
  //   frame_type = VSCP_WS1_PKT_TYPE_NEGATIVE_RESPONSE;
  // }
  // Unknown packet type
  else {
    // Unknown packet type
    return VSCP_ERROR_INVALID_FRAME;
  }

  free(pbuf);
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_handle_command
//

static int
vscp_ws1_handle_command(const char *pCommand, const char *parg, vscp_ws1_ctx_t *pctx)
{
  int rv;
  char buf[512] = { 0 };
  char command_buf[VSCP_WS1_MAX_PACKET_SIZE];

  size_t command_len = strnlen(pCommand, sizeof(command_buf));
  if ((0 == command_len) || (command_len >= sizeof(command_buf))) {
    return VSCP_ERROR_INVALID_SYNTAX;
  }

  memcpy(command_buf, pCommand, command_len + 1);

  // Make sure command is upper case for easier handling
  for (char *c = command_buf; *c; c++) {
    *c = (char) toupper((unsigned char) *c);
  }

  if (strcmp(command_buf, "NOOP") == 0) {
    pctx->ops->reply(pctx, "+;NOOP"); // Send positive reply
  }
  else if (strcmp(command_buf, "VERSION") == 0) {
    sprintf(buf,
            "+;VERSION;%d.%d.%d.%d",
            VSCP_WS1_PROTOCOL_VERSION,
            VSCP_WS1_PROTOCOL_MINOR_VERSION,
            VSCP_WS1_PROTOCOL_RELEASE_VERSION,
            VSCP_WS1_PROTOCOL_BUILD_VERSION);
    pctx->ops->reply(pctx, buf); // Send positive reply with version information
  }
  else if (strcmp(command_buf, "COPYRIGHT") == 0) {
    if (NULL != pctx->ops->copyright) {
      pctx->ops->copyright(pctx);
    }
    else {
      pctx->ops->reply(pctx, "-;COPYRIGHT;1,Not supported");
    }
  }
  // Authentication command with session ID and encrypted credentials
  // Expected format for parg: "sid;crypto" where both values are hex strings
  else if (strcmp(command_buf, "AUTH") == 0) {
    char argbuf[VSCP_WS1_MAX_PACKET_SIZE];
    char *sid_hex;
    char *crypto_hex;
    uint8_t sid[16]                             = { 0 };
    uint8_t crypto[VSCP_WS1_MAX_CRYPTO_BIN_LEN] = { 0 };
    size_t sid_hex_len;
    size_t crypto_hex_len;
    size_t crypto_bin_len;

    if ((NULL == parg) || ('\0' == *parg)) {
      WS1_LOGE(TAG, "AUTH missing arguments");
      pctx->ops->reply(pctx, "-;AUTH;8,Not authorized");
      return VSCP_ERROR_INVALID_SYNTAX;
    }

    if (strnlen(parg, sizeof(argbuf)) >= sizeof(argbuf)) {
      WS1_LOGE(TAG, "AUTH arguments too long");
      pctx->ops->reply(pctx, "-;AUTH;8,Not authorized");
      return VSCP_ERROR_INVALID_SYNTAX;
    }

    strcpy(argbuf, parg);

    sid_hex    = argbuf;
    crypto_hex = strchr(argbuf, ';');
    if (NULL == crypto_hex) {
      WS1_LOGE(TAG, "AUTH invalid format, expected sid;crypto");
      pctx->ops->reply(pctx, "-;AUTH;8,Not authorized");
      return VSCP_ERROR_INVALID_SYNTAX;
    }

    *crypto_hex = '\0';
    ++crypto_hex;

    if (('\0' == *sid_hex) || ('\0' == *crypto_hex) || (NULL != strchr(crypto_hex, ';'))) {
      WS1_LOGE(TAG, "AUTH invalid format, expected exactly two arguments");
      pctx->ops->reply(pctx, "-;AUTH;8,Not authorized");
      return VSCP_ERROR_INVALID_SYNTAX;
    }

    sid_hex_len    = strlen(sid_hex);
    crypto_hex_len = strlen(crypto_hex);

    if ((2 * VSCP_WS1_SID_SIZE != sid_hex_len) || ((sid_hex_len & 1U) != 0U) || !vscp_ws1_is_hex_string(sid_hex)) {
      WS1_LOGE(TAG, "AUTH invalid SID hex");
      pctx->ops->reply(pctx, "-;AUTH;8,Not authorized");
      return VSCP_ERROR_INVALID_SYNTAX;
    }

    if ((0 == crypto_hex_len) || ((crypto_hex_len & 1U) != 0U) ||
        (crypto_hex_len > (2U * VSCP_WS1_MAX_CRYPTO_BIN_LEN)) || !vscp_ws1_is_hex_string(crypto_hex)) {
      WS1_LOGE(TAG, "AUTH invalid crypto hex");
      pctx->ops->reply(pctx, "-;AUTH;8,Not authorized");
      return VSCP_ERROR_INVALID_SYNTAX;
    }

    if (16 != (rv = vscp_fwhlp_hex2bin(sid, sizeof(sid), sid_hex))) {
      WS1_LOGE(TAG, "Failed to convert AUTH SID from hex. len returned: %d", rv);
      pctx->ops->reply(pctx, "-;AUTH;8,Not authorized");
      return VSCP_ERROR_INVALID_FORMAT;
    }

    if (16 != (crypto_bin_len = vscp_fwhlp_hex2bin(crypto + 1, sizeof(crypto), crypto_hex))) {
      WS1_LOGE(TAG, "Failed to convert AUTH crypto from hex %d", crypto_bin_len);
      pctx->ops->reply(pctx, "-;AUTH;8,Not authorized");
      return VSCP_ERROR_INVALID_FORMAT;
    }

    // Attempt to validate user with provided credentials  and session ID
    if (NULL == pctx->ops->validate_user) {
      pctx->ops->reply(pctx, "-;AUTH;8,Not authorized");
      return VSCP_ERROR_INIT_MISSING;
    }
    rv = pctx->ops->validate_user(pctx, crypto, crypto_bin_len + 1, sid);
    if (VSCP_ERROR_SUCCESS != rv) {
      WS1_LOGE(TAG, "Client authentication failed");
      pctx->ops->reply(pctx, "-;AUTH;8,Not authorized");
      return rv;
    }

    // Validation is successful, mark connection as authenticated
    pctx->bAuthenticated = true;
    WS1_LOGI(TAG, "Client authenticated successfully");

    // Send positive reply
    char wrkbuf[80] = { 0 };
    strcpy(buf, "+;AUTH;");
    strcat(buf, pctx->user.username);
    strcat(buf, ";;");
#ifndef VSCP_WS1_DISABLE_USER_FULL_NAME
    strcat(buf, pctx->user.fullname);
#endif
    // Filter
    strcat(buf, ";");
    vscp_fwhlp_writeFilterToString(wrkbuf, sizeof(wrkbuf), &pctx->filter);
    strcat(buf, wrkbuf);
    // Mask
    strcat(buf, ";");
    vscp_fwhlp_writeMaskToString(wrkbuf, sizeof(wrkbuf), &pctx->filter);
    strcat(buf, wrkbuf);
    // Rights
    strcat(buf, ";");
    sprintf(wrkbuf, "%llu", pctx->user.rights);
    strcat(buf, wrkbuf);
    // Remotes (allowed remote IP addresses)
    strcat(buf, ";");
    for (int i = 0; i < 16; i++) {
      // Check for end of list (all zeros)
      bool is_zero = true;
      for (int j = 0; j < 16; j++) {
        if (pctx->user.allowed_remotes[i][j] != 0) {
          is_zero = false;
          break;
        }
      }
      if (is_zero) {
        break;
      }
      if (i > 0) {
        strcat(buf, ",");
      }
      // Check if IPv4 (first 10 bytes zero, bytes 10-11 are 0xFF or first 12 bytes zero)
      bool is_ipv4 = true;
      for (int j = 0; j < 10; j++) {
        if (pctx->user.allowed_remotes[i][j] != 0) {
          is_ipv4 = false;
          break;
        }
      }
      if (is_ipv4 && (pctx->user.allowed_remotes[i][10] == 0 || pctx->user.allowed_remotes[i][10] == 0xFF) &&
          (pctx->user.allowed_remotes[i][11] == 0 || pctx->user.allowed_remotes[i][11] == 0xFF)) {
        // IPv4 address in last 4 bytes
        sprintf(wrkbuf,
                "%u.%u.%u.%u",
                pctx->user.allowed_remotes[i][12],
                pctx->user.allowed_remotes[i][13],
                pctx->user.allowed_remotes[i][14],
                pctx->user.allowed_remotes[i][15]);
      }
      else {
        // IPv6 address
        sprintf(wrkbuf,
                "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X",
                pctx->user.allowed_remotes[i][0],
                pctx->user.allowed_remotes[i][1],
                pctx->user.allowed_remotes[i][2],
                pctx->user.allowed_remotes[i][3],
                pctx->user.allowed_remotes[i][4],
                pctx->user.allowed_remotes[i][5],
                pctx->user.allowed_remotes[i][6],
                pctx->user.allowed_remotes[i][7],
                pctx->user.allowed_remotes[i][8],
                pctx->user.allowed_remotes[i][9],
                pctx->user.allowed_remotes[i][10],
                pctx->user.allowed_remotes[i][11],
                pctx->user.allowed_remotes[i][12],
                pctx->user.allowed_remotes[i][13],
                pctx->user.allowed_remotes[i][14],
                pctx->user.allowed_remotes[i][15]);
      }
      strcat(buf, wrkbuf);
    }
    // Events
    strcat(buf, ";");
    for (int i = 0; i < 16; i++) {
      // Are we done
      if (pctx->user.events[i][0] && pctx->user.events[i][1]) {
        break;
      }
      if (i > 0) {
        strcat(buf, ",");
      }
      sprintf(wrkbuf, "%02X:%02X", pctx->user.events[i][0], pctx->user.events[i][1]);
      strcat(buf, wrkbuf);
    }
    // Note
    strcat(buf, ";");
#ifndef VSCP_WS1_DISABLE_USER_NOTES
    strcat(buf, pctx->user.note);
#endif

    WS1_LOGI(TAG, "Authentication successful, sending reply: %s", buf);

    pctx->ops->reply(pctx, buf);
  }
  else if (strcmp(command_buf, "CHALLENGE") == 0) {
    sprintf(buf,
            "+;AUTH0;%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
            pctx->sid[0],
            pctx->sid[1],
            pctx->sid[2],
            pctx->sid[3],
            pctx->sid[4],
            pctx->sid[5],
            pctx->sid[6],
            pctx->sid[7],
            pctx->sid[8],
            pctx->sid[9],
            pctx->sid[10],
            pctx->sid[11],
            pctx->sid[12],
            pctx->sid[13],
            pctx->sid[14],
            pctx->sid[15]);

    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->reply(pctx, buf))) {
      WS1_LOGE(TAG, "Failed to send CHALLENGE reply rv=%d", rv);
      return rv;
    }
  }
  else if (strcmp(command_buf, "OPEN") == 0) {
    if ((NULL != pctx->ops->open) &&
        (VSCP_ERROR_SUCCESS != (rv = pctx->ops->open(pctx)))) {
      WS1_LOGE(TAG, "Failed to open WS1 connection rv=%d", rv);
      sprintf(buf, "-;OPEN;%d,%s", VSCP_WS1_ERROR_GENERAL, VSCP_WS1_STR_ERROR_GENERAL);
      pctx->ops->reply(pctx, buf);
      return rv;
    }
    pctx->bOpen = true;
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->reply(pctx, "+;OPEN"))) {
      WS1_LOGE(TAG, "Failed to send OPEN reply rv=%d", rv);
      return rv;
    }
  }
  else if (strcmp(command_buf, "CLOSE") == 0) {
    if ((NULL != pctx->ops->close) &&
        (VSCP_ERROR_SUCCESS != (rv = pctx->ops->close(pctx)))) {
      WS1_LOGE(TAG, "Failed to close WS1 connection rv=%d", rv);
      sprintf(buf, "-;CLOSE;%d,%s", VSCP_WS1_ERROR_GENERAL, VSCP_WS1_STR_ERROR_GENERAL);
      pctx->ops->reply(pctx, buf);
      return rv;
    }
    pctx->bOpen = false;
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->reply(pctx, "+;CLOSE"))) {
      WS1_LOGE(TAG, "Failed to send CLOSE reply rv=%d", rv);
      return rv;
    }
  }
  else if ((strcmp(command_buf, "SETFILTER") == 0) || (strcmp(command_buf, "SF") == 0)) {
    vscpEventFilter filter;
    memset(&filter, 0, sizeof(filter));

    if ((NULL == parg) || ('\0' == *parg)) {
      WS1_LOGE(TAG, "SETFILTER missing filter argument");
      sprintf(buf, "-;SETFILTER;%d,%s", VSCP_WS1_ERROR_SYNTAX, VSCP_WS1_STR_ERROR_SYNTAX);
      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->reply(pctx, buf))) {
        WS1_LOGE(TAG, "Failed to send SETFILTER reply rv=%d", rv);
        return rv;
      }
      return VSCP_ERROR_INVALID_SYNTAX;
    }

    if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_parseFilter(&filter, parg))) {
      WS1_LOGE(TAG, "SETFILTER invalid filter string");
      sprintf(buf, "-;SETFILTER;%d,%s", VSCP_WS1_ERROR_SYNTAX, VSCP_WS1_STR_ERROR_SYNTAX);
      if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->reply(pctx, buf))) {
        WS1_LOGE(TAG, "Failed to send SETFILTER reply rv=%d", rv);
        return rv;
      }
      return VSCP_ERROR_INVALID_SYNTAX;
    }

    /* Always update local filter; callback may additionally persist it */
    memcpy(&pctx->filter, &filter, sizeof(filter));
    if ((NULL != pctx->ops->setfilter) &&
        (VSCP_ERROR_SUCCESS != (rv = pctx->ops->setfilter(pctx, &filter)))) {
      sprintf(buf, "-;SETFILTER;%d,%s", VSCP_WS1_ERROR_GENERAL, VSCP_WS1_STR_ERROR_GENERAL);
      pctx->ops->reply(pctx, buf);
      return rv;
    }

    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->reply(pctx, "+;SF"))) {
      WS1_LOGE(TAG, "Failed to send SETFILTER reply rv=%d", rv);
      return rv;
    }
  }
  else if ((strcmp(command_buf, "CLRQUEUE") == 0) || (strcmp(command_buf, "CLRQ") == 0)) {

    if ((NULL != pctx->ops->clrqueue) &&
        (VSCP_ERROR_SUCCESS != (rv = pctx->ops->clrqueue(pctx)))) {
      WS1_LOGE(TAG, "Failed to clear WS1 event queue rv=%d", rv);
      sprintf(buf, "-;CLRQUEUE;%d,%s", VSCP_WS1_ERROR_GENERAL, VSCP_WS1_STR_ERROR_GENERAL);
      pctx->ops->reply(pctx, buf);
      return rv;
    }

    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->reply(pctx, "+;CLRQ"))) {
      WS1_LOGE(TAG, "Failed to send CLRQUEUE reply rv=%d", rv);
      return rv;
    }
  }
  else {
    WS1_LOGE(TAG, "Unknown command: %s", pCommand);
    sprintf(buf, "-;%s;%d,%s", pCommand, VSCP_WS1_ERROR_UNKNOWN_COMMAND, VSCP_WS1_STR_ERROR_UNKNOWN_COMMAND);
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->reply(pctx, buf))) {
      WS1_LOGE(TAG, "Failed to send UNKNOWN_COMMAND reply rv=%d", rv);
      return rv;
    }
    return VSCP_ERROR_INVALID_SYNTAX;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_handle_binary_command
//

static int
vscp_ws1_handle_binary_command(vscp_ws1_ctx_t *pctx, uint16_t command, const uint8_t *parg)
{
  return VSCP_ERROR_SUCCESS;
}