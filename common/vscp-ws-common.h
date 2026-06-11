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
  Shared types used by the WS1 and WS2 WebSocket protocol handlers.
*/

#ifndef __VSCP_WS_COMMON_H__
#define __VSCP_WS_COMMON_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum field sizes for authenticated-user information */
#define VSCP_WS_USER_MAX_USERNAME_SIZE 64
#define VSCP_WS_USER_MAX_FULLNAME_SIZE 64
#define VSCP_WS_USER_MAX_NOTE_SIZE     256
#define VSCP_WS_USER_MAX_REMOTES       16 /*!< Maximum number of allowed remote hosts per user */
#define VSCP_WS_USER_MAX_EVENTS        16 /*!< Maximum number of allowed event (class,type) pairs per user */

/*!
  Holds the information about an authenticated user that is populated by
  the validate_user callback and used by the WS1/WS2 framework to build
  the AUTH1 response and enforce access control.
*/
typedef struct vscp_ws_user {
  char     username[VSCP_WS_USER_MAX_USERNAME_SIZE]; /*!< Login name */
  char     fullname[VSCP_WS_USER_MAX_FULLNAME_SIZE]; /*!< Display name */
  uint64_t rights;                                   /*!< 63-bit permission bitmask */
  uint8_t  allowed_remotes[VSCP_WS_USER_MAX_REMOTES][16]; /*!< IPv6 addresses (zero-terminated list) */
  uint8_t  events[VSCP_WS_USER_MAX_EVENTS][2];            /*!< Allowed (class, type) pairs; {0,0} terminates */
  char     note[VSCP_WS_USER_MAX_NOTE_SIZE];               /*!< Free-text note */
} vscp_ws_user_t;

#ifdef __cplusplus
}
#endif

#endif /* __VSCP_WS_COMMON_H__ */
