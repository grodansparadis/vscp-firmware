/* ******************************************************************************
 * VSCP (Very Simple Control Protocol)
 * http://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2026 Ake Hedman,
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

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include "vscp.h"

#include "vscp-ws1.h"

uint8_t m_key[16] = { 0 };
uint8_t m_sid[16] = { 0 };
bool m_bOpen;             // Flag for open/closed channel
vscpEventFilter m_filter; // Filter/mask for VSCP

vscp_ws1_connection_context_t m_ws1_connection_context;

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_init
//

int
vscp_ws1_init(m_ws1_connection_context_t *ctx)
{
  // Initialize the connection context
  memset(ctx, 0, sizeof(m_ws1_connection_context_t));
  ctx->bAuthenticated = false;
  ctx->m_bOpen        = false;

  // Clear the global VSCP filter
  memset(&ctx->m_filter, 0, sizeof(ctx->m_filter));

  // Generate a random SID for the session
  vscp_ws1_generate_sid(ctx->sid, sizeof(ctx->sid));

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_clearup
//

int
vscp_ws1_clearup()
{
  retun VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// handle_ws1_command
//

int
vscp_ws1_generate_sid(uint8_t *sid, size_t size)
{
  // Generate a random SID (session ID) for authentication and encryption
  // In a real implementation, this should be done using a secure random generator
  for (size_t i = 0; i < size; i++) {
    sid[i] = rand() % 256;
  }
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_handle_command
//

int
vscp_ws1_handle_command(char *pCommand, const char *p)
{
  // Make sure command is upper case for easier handling
  for (char *c = pCommand; *c; c++) {
    *c = toupper(*c);
  }

  if (strcmp(pCommand, "NOOP") == 0) {
    vscp_ws1_callback_reply("+;NOOP"); // Send positive reply
  }
  else if (strcmp(pCommand, "VERSION") == 0) {
    sprintf(buf,
            "+;VERSION;%d.%d.%d.%d",
            VSCP_WS1_PROTOCOL_VERSION,
            VSCP_WS1_PROTOCOL_MINOR_VERSION,
            VSCP_WS1_PROTOCOL_RELEASE_VERSION,
            VSCP_WS1_PROTOCOL_BUILD_VERSION);
    vscp_ws1_callback_reply(buf); // Send positive reply with version information
  }
  else if (strcmp(pCommand, "COPYRIGHT") == 0) {
    vscp_ws1_callback_copyright();
  }
  else if (strcmp(pCommand, "AUTH") == 0) {
    vscp_ws1_callback_auth(p);
  }
  else if (strcmp(pCommand, "CHALLENGE") == 0) {
    vscp_ws1_callback_challenge(p);
  }
  else if (strcmp(pCommand, "OPEN") == 0) {
    vscp_ws1_callback_open();
  }
  else if (strcmp(pCommand, "CLOSE") == 0) {
    vscp_ws1_callback_close();
  }
  else if (strcmp(pCommand, "SETFILTER") == 0) {
    vscp_ws1_callback_setfilter(p);
  }
  else if (strcmp(pCommand, "SF") == 0) {
    vscp_ws1_callback_setfilter();
  }
  else if (strcmp(pCommand, "CLRQUEUE") == 0) {
    vscp_ws1_callback_clrqueue();
  }
  else if (strcmp(pCommand, "CLRQ") == 0) {
    ;
  }
  else {
    ESP_LOGW(TAG, "Unknown command: %s", pCommand);
    return VSCP_ERROR_INVALID_COMMAND;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws1_handle_protocol_request
//
// Packets are
// Command:        'C' ; command ; optional data that may be separated by additional semicolons.
// Reply:          '+' ; 'command'
// Negative reply: '-' ; 'command' ; Error code ; Error in real text
// Event:          'E' ; head , vscp_class , vscp_type ,obid, datetime, timestamp, GUID, data
//

int
vscp_ws1_handle_protocol_request(const char *packet)
{
  uint8_t packet_type = WS1_PKT_TYPE_UNKNOWN;
  char *pCommand; // Pointer to command part of packet

  ESP_LOGI(TAG, "Handling protocol WS1");

  char *p = packet;

  // Command
  if (*p == 'C') {
    paket_type = WS1_PKT_TYPE_COMMAND;
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

    ESP_LOGI(TAG, "Received command: %s", pCommand);
    handle_ws1_command(pCommand, p);
  }
  // Received event
  else if (*p == 'E') {
    packet_type = WS1_PKT_TYPE_EVENT;
    vscp_ws1_callback_event();
  }
  // Positive respone
  else if (*p == '+') {
    packet_type = WS1_PKT_TYPE_POSITIVE_RESPONSE;
  }
  // Negative response
  else if (*p == '-') {
    packet_type = WS1_PKT_TYPE_NEGATIVE_RESPONSE;
  }
  // Unknown packet type
  else {
    // Unknown packet type
    return VSCP_ERROR_INVALID_FRAME;
  }

  return VSCP_ERROR_SUCCESS;
}