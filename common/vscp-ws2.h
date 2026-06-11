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
  The VSCP Websocket Server (WS2) protocol is a JSON-based protocol designed
  for communication between a VSCP device and a client over a WebSocket connection.
  The protocol allows clients to send commands to the device and receive responses,
  as well as receive events from the device in real-time.

  It is documented here
  https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_websocket?id=ws2-description
 */

#ifndef __VSCP_WS2_H__
#define __VSCP_WS2_H__

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "vscp.h"
#include "vscp-firmware-helper.h"
#include "vscp-ws-common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VSCP_WS2_PROTOCOL         1  /*!< Protocol identifier (ws1=0, ws2=1) */
#define VSCP_WS2_MAX_PACKET_SIZE  4096

#define VSCP_WS2_PROTOCOL_VERSION         1
#define VSCP_WS2_PROTOCOL_MINOR_VERSION   0
#define VSCP_WS2_PROTOCOL_RELEASE_VERSION 0
#define VSCP_WS2_PROTOCOL_BUILD_VERSION   0

/* Forward declaration */
struct vscp_ws2_ctx;

/*!
  Operations table for the WS2 protocol handler.

  Mandatory (vscp_ws2_init returns VSCP_ERROR_INIT_MISSING if NULL):
    reply, generate_sid

  All other fields are optional; set unused pointers to NULL.
*/
typedef struct vscp_ws2_ops {
  /*! Mandatory. Send a JSON text frame to the WebSocket client. */
  int (*reply)(struct vscp_ws2_ctx *pctx, const char *json_response);

  /*! Mandatory. Generate a 16-byte random session ID. */
  int (*generate_sid)(struct vscp_ws2_ctx *pctx, uint8_t *sid, size_t size);

  /*! Optional. Called at the end of vscp_ws2_init(). May be NULL. */
  int (*init)(struct vscp_ws2_ctx *pctx);

  /*! Optional. Called at the start of vscp_ws2_clearup(). May be NULL. */
  int (*cleanup)(struct vscp_ws2_ctx *pctx);

  /*! Optional. Return the 128-bit AES key for credential decryption. May be NULL (AUTH fails). */
  const uint8_t *(*get_primary_key)(struct vscp_ws2_ctx *pctx);

  /*!
    Optional. Validate decrypted credentials. Must populate pctx->user and send the
    AUTH1 JSON reply via pctx->ops->reply(). Returns VSCP_ERROR_SUCCESS on success.
  */
  int (*validate_user)(struct vscp_ws2_ctx *pctx,
                       const uint8_t *pcrypto,
                       uint8_t        crypto_len,
                       const uint8_t *psid);

  /*! Optional. Check whether the user may receive pEvent. NULL = allow all. */
  int (*is_allowed_event)(struct vscp_ws2_ctx *pctx, vscp_event_t *pEvent);

  /*! Optional. Check whether a connection from pip is allowed. NULL = allow all. */
  int (*is_allowed_connection)(struct vscp_ws2_ctx *pctx, const char *pip);

  /*! Optional. Deliver an outbound event to the client. May be NULL. */
  int (*send_event)(struct vscp_ws2_ctx *pctx, vscp_event_t *pEvent);

  /*! Optional. Handle the OPEN command. NULL = framework defaults to success. */
  int (*open)(struct vscp_ws2_ctx *pctx);

  /*! Optional. Handle the CLOSE command. NULL = framework defaults to success. */
  int (*close)(struct vscp_ws2_ctx *pctx);

  /*! Optional. Persist/apply the event filter. NULL = filter stored in ctx only. */
  int (*setfilter)(struct vscp_ws2_ctx *pctx, const vscpEventFilter *pfilter);

  /*! Optional. Clear the receive queue. NULL = positive reply sent without action. */
  int (*clrqueue)(struct vscp_ws2_ctx *pctx);
} vscp_ws2_ops_t;

/*!
  Connection context for a single WS2 session.
*/
typedef struct vscp_ws2_ctx {
  const vscp_ws2_ops_t *ops;            /*!< Pointer to the ops table. Must be set before vscp_ws2_init(). */
  void                 *pdata;          /*!< Application-defined per-connection data. */
  bool                  bAuthenticated; /*!< Set to true after successful AUTH. */
  bool                  bOpen;          /*!< Set to true after OPEN, false after CLOSE. */
  uint8_t               sid[16];        /*!< Session ID / AES IV. */
  vscpEventFilter       filter;         /*!< Active event filter. */
  vscp_ws_user_t        user;           /*!< Authenticated user information. */
} vscp_ws2_ctx_t;

/* -------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

/*!
  Initialize a WS2 session. Generates a SID, sends the AUTH0 challenge, and
  calls ops->init() if set.
  @param pctx  Allocated (and zeroed) vscp_ws2_ctx_t with ops set.
  @param pdata Stored in pctx->pdata; may be NULL.
  @return VSCP_ERROR_SUCCESS or an error code.
*/
int
vscp_ws2_init(vscp_ws2_ctx_t *pctx, void *pdata);

/*!
  Clean up a WS2 session. Calls ops->cleanup() if set.
  @return VSCP_ERROR_SUCCESS or an error code.
*/
int
vscp_ws2_clearup(vscp_ws2_ctx_t *pctx, void *pdata);

/*!
  Handle a text (JSON) frame received from the client.
  @param pctx   Connection context.
  @param packet Received JSON frame bytes (need not be null-terminated).
  @param len    Length in bytes.
  @return VSCP_ERROR_SUCCESS or an error code.
*/
int
vscp_ws2_handle_request(vscp_ws2_ctx_t *pctx, const char *packet, size_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __VSCP_WS2_H__ */
