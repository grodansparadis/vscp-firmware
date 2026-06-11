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

/*!
 The VSCP Websocket Server (WS1) protocol is a simple text-based protocol designed
 for communication between a VSCP device and a client over a WebSocket connection.
 The protocol allows clients to send commands to the device and receive responses,
 as well as receive events from the device in real-time.

 It is documented here
 https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_websocket?id=ws1-description
*/

#ifndef __VSCP_WS1_H__
#define __VSCP_WS1_H__

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "vscp.h"
#include "vscp-firmware-helper.h"
#include "vscp-binary-protocol.h"
#include "vscp-ws-common.h"

#ifdef __cplusplus
extern "C" {
#endif

// ws1 is 0 and ws2 is 1, this is used in the session context to determine which protocol
// is being used for a given connection
#define VSCP_WS1_PROTOCOL 0

// Defines in vscp-projdefs.h that should be used by the ws1 protocol implementation
// VSCP_WS1_MAX_CLIENTS - Maximum number of clients that can be connected at the same time

#define VSCP_WS1_MAX_PACKET_SIZE 512
#define VSCP_WS1_MAX_CLIENTS     10

// ws1 packet types
#define VSCP_WS1_PKT_TYPE_UNKNOWN           0
#define VSCP_WS1_PKT_TYPE_COMMAND           1
#define VSCP_WS1_PKT_TYPE_EVENT             2
#define VSCP_WS1_PKT_TYPE_POSITIVE_RESPONSE 3
#define VSCP_WS1_PKT_TYPE_NEGATIVE_RESPONSE 4

/*!
  Define the version for the ws1 protocol supported
  by this driver
*/
#define VSCP_WS1_PROTOCOL_VERSION         1
#define VSCP_WS1_PROTOCOL_MINOR_VERSION   0
#define VSCP_WS1_PROTOCOL_RELEASE_VERSION 0
#define VSCP_WS1_PROTOCOL_BUILD_VERSION   0

// VSCP ws1 error codes
#define VSCP_WS1_ERROR_NONE                0  // No error
#define VSCP_WS1_ERROR_SYNTAX              1  // Syntax error
#define VSCP_WS1_ERROR_UNKNOWN_COMMAND     2  // Unknown command
#define VSCP_WS1_ERROR_TX_BUFFER_FULL      3  // Transmit buffer full
#define VSCP_WS1_ERROR_MEMORY              4  // Problem allocating memory
#define VSCP_WS1_ERROR_NOT_AUTHORIZED      5  // Not authorized
#define VSCP_WS1_ERROR_NOT_AUTHORIZED_SEND 6  // Not authorized to send events
#define VSCP_WS1_ERROR_NOT_ALLOWED         7  // Not allowed to do that
#define VSCP_WS1_ERROR_PARSE               8  // Parse error, invalid format
#define VSCP_WS1_ERROR_UNKNOWN_TYPE        9  // Unknown ws1 message type, only know "COMMAND" and "EVENT"
#define VSCP_WS1_ERROR_GENERAL             10 // General error or exception

#define VSCP_WS1_STR_ERROR_NONE                      "Everything is OK."
#define VSCP_WS1_STR_ERROR_SYNTAX                    "Syntax error."
#define VSCP_WS1_STR_ERROR_UNKNOWN_COMMAND           "Unknown command."
#define VSCP_WS1_STR_ERROR_TX_BUFFER_FULL            "Transmit buffer full."
#define VSCP_WS1_STR_ERROR_MEMORY_ALLOCATION         "Having problems to allocate memory."
#define VSCP_WS1_STR_ERROR_NOT_AUTHORISED            "Not authorised."
#define VSCP_WS1_STR_ERROR_NOT_ALLOWED_TO_SEND_EVENT "Not allowed to send event."
#define VSCP_WS1_STR_ERROR_NOT_ALLOWED_TO_DO_THAT    "Not allowed to do that (check privileges)"
#define VSCP_WS1_STR_ERROR_PARSE_FORMAT              "Parse error, invalid format."
#define VSCP_WS1_STR_ERROR_UNKNOWN_TYPE              "Unknown type, only know 'COMMAND' and 'EVENT'."
#define VSCP_WS1_STR_ERROR_GENERAL                   "Exception or other general error."

/* Forward declaration for use inside the ops struct */
struct vscp_ws1_ctx;

/*!
  Operations table for the WS1 protocol handler. Populate and assign this struct to a
  vscp_ws1_ctx_t before calling vscp_ws1_init().

  Mandatory fields (vscp_ws1_init returns VSCP_ERROR_INIT_MISSING if NULL):
    reply, generate_sid

  All other fields are optional; set unused pointers to NULL.
*/
typedef struct vscp_ws1_ops {
  /*!
    Mandatory. Send a text frame reply to the WebSocket client.
    @param pctx  Connection context.
    @param response Null-terminated reply string.
    @return VSCP_ERROR_SUCCESS on success.
  */
  int (*reply)(struct vscp_ws1_ctx *pctx, const char *response);

  /*!
    Mandatory. Generate a 16-byte random session ID that is sent to the client
    as the AUTH0 challenge on init.
    @param pctx  Connection context.
    @param sid   Buffer to fill (size bytes).
    @param size  Must be >= 16.
    @return VSCP_ERROR_SUCCESS on success.
  */
  int (*generate_sid)(struct vscp_ws1_ctx *pctx, uint8_t *sid, size_t size);

  /*!
    Optional. Called at the end of vscp_ws1_init() for application-specific setup.
    May be NULL.
  */
  int (*init)(struct vscp_ws1_ctx *pctx);

  /*!
    Optional. Called at the start of vscp_ws1_clearup() for application-specific teardown.
    May be NULL.
  */
  int (*cleanup)(struct vscp_ws1_ctx *pctx);

  /*!
    Mandatory for the AUTH command. Return the 128-bit pre-shared AES key used to
    decrypt the client-supplied credential blob.
    @return Pointer to a 16-byte key buffer, or NULL on error.
  */
  const uint8_t *(*get_primary_key)(struct vscp_ws1_ctx *pctx);

  /*!
    Mandatory for the AUTH command. Validate the decrypted credentials. On success
    the callback must populate pctx->user and send the AUTH1 response via
    pctx->ops->reply(). Returns VSCP_ERROR_SUCCESS when authentication passes.
    @param pctx        Connection context.
    @param pcrypto     AES-128-encrypted credential blob (binary).
    @param crypto_len  Length of pcrypto in bytes.
    @param psid        16-byte session ID used as the AES IV.
  */
  int (*validate_user)(struct vscp_ws1_ctx *pctx,
                       const uint8_t *pcrypto,
                       uint8_t        crypto_len,
                       const uint8_t *psid);

  /*!
    Optional. Check whether the authenticated user is permitted to receive pEvent.
    Return VSCP_ERROR_SUCCESS to allow, any other value to deny.  May be NULL
    (all events allowed).
  */
  int (*is_allowed_event)(struct vscp_ws1_ctx *pctx, vscp_event_t *pEvent);

  /*!
    Optional. Check whether a connection from the given IP address is allowed.
    Return VSCP_ERROR_SUCCESS to allow.  May be NULL (all connections allowed).
    @param pip  Null-terminated IP address string.
  */
  int (*is_allowed_connection)(struct vscp_ws1_ctx *pctx, const char *pip);

  /*!
    Optional. Deliver an outbound event from the device to the client. May be NULL.
  */
  int (*send_event)(struct vscp_ws1_ctx *pctx, vscp_event_t *pEvent);

  /*!
    Optional. Handle the COPYRIGHT command. If NULL, a default "not supported"
    reply is sent.
  */
  int (*copyright)(struct vscp_ws1_ctx *pctx);

  /*!
    Optional. Handle the OPEN command (open event stream). If NULL, the
    framework sets bOpen = true and replies "+;OPEN".
  */
  int (*open)(struct vscp_ws1_ctx *pctx);

  /*!
    Optional. Handle the CLOSE command. If NULL, the framework sets bOpen = false
    and replies "+;CLOSE".
  */
  int (*close)(struct vscp_ws1_ctx *pctx);

  /*!
    Optional. Persist or apply the event filter parsed from a SETFILTER command.
    If NULL, the filter is stored only in pctx->filter.
    @param pfilter  Parsed filter (also already copied into pctx->filter).
  */
  int (*setfilter)(struct vscp_ws1_ctx *pctx, const vscpEventFilter *pfilter);

  /*!
    Optional. Handle the CLRQUEUE command (clear the receive queue). If NULL, a
    positive reply is sent without any queue management.
  */
  int (*clrqueue)(struct vscp_ws1_ctx *pctx);
} vscp_ws1_ops_t;

/*!
  Connection context for a single WS1 WebSocket session. Allocate one per
  connection, set the ops pointer, then call vscp_ws1_init().
*/
typedef struct vscp_ws1_ctx {
  const vscp_ws1_ops_t *ops;         /*!< Pointer to the ops table (must be set before vscp_ws1_init). */
  void                 *pdata;       /*!< Application-defined per-connection data. */
  bool                  bAuthenticated; /*!< Set to true after successful AUTH. */
  bool                  bOpen;          /*!< Set to true after OPEN, false after CLOSE. */
  uint8_t               sid[16];        /*!< Session ID / AES IV sent as AUTH0 challenge. */
  vscpEventFilter       filter;         /*!< Active event filter for this connection. */
  vscp_ws_user_t        user;           /*!< Authenticated user information. */
  vscp_binary_ctx_t     binary_ctx;     /*!< Binary-protocol sub-context (set binary_ctx.ops before use). */
} vscp_ws1_ctx_t;

/* -------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

/*!
  Initialize the WS1 protocol handler. Generates a session ID, sends the AUTH0
  challenge to the client, and calls ops->init() if provided.

  Mandatory ops: reply, generate_sid.

  @param pctx  Pointer to an allocated (and zeroed) vscp_ws1_ctx_t. ops must be set.
  @param pdata Stored in pctx->pdata; may be NULL.
  @return VSCP_ERROR_SUCCESS or an error code.
*/
int
vscp_ws1_init(vscp_ws1_ctx_t *pctx, void *pdata);

/*!
  Clean up resources associated with a WS1 connection. Calls ops->cleanup() if set.
  @param pctx  Connection context.
  @param pdata Unused; kept for API consistency.
  @return VSCP_ERROR_SUCCESS or an error code.
*/
int
vscp_ws1_clearup(vscp_ws1_ctx_t *pctx, void *pdata);

/*!
  Generate a 16-byte session ID using the platform random source.  Can be
  called as the generate_sid ops implementation if a simple rand()-based
  source is acceptable.
  @param pctx  Connection context.
  @param sid   Output buffer.
  @param size  Must be >= 16.
  @return VSCP_ERROR_SUCCESS.
*/
int
vscp_ws1_generate_sid(vscp_ws1_ctx_t *pctx, uint8_t *sid, size_t size);

/*!
  Handle a text-framed WS1 protocol packet received from the client.
  @param pctx   Connection context.
  @param packet Received text frame (need not be null-terminated; len gives its length).
  @param len    Length of packet in bytes.
  @return VSCP_ERROR_SUCCESS or an error code.
*/
int
vscp_ws1_handle_text_protocol_request(vscp_ws1_ctx_t *pctx, const char *packet, uint16_t len);

/*!
  Handle a binary-framed WS1 protocol packet received from the client.
  @param pctx   Connection context.
  @param packet Received binary frame bytes.
  @param len    Length of packet in bytes.
  @return VSCP_ERROR_SUCCESS or an error code.
*/
int
vscp_ws1_handle_binary_protocol_request(vscp_ws1_ctx_t *pctx, const uint8_t *packet, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __VSCP_WS1_H__ */
