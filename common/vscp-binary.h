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

/*
 This is the header file for the VSCP Binary Protocol. This protocol is a simple
 binary protocol designed for efficient communication between a VSCP device and a
 client over a VSCP supported transport.
*/

#ifndef __VSCP_BINARY_H__
#define __VSCP_BINARY_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VSCP_BINARY_MAX_USERNAME_LENGTH 32
#define VSCP_BINARY_MAX_PASSWORD_LENGTH 32

/* Binary command codes */
#define VSCP_BINARY_COMMAND_CODE_NOOP      0x0000
#define VSCP_BINARY_COMMAND_CODE_QUIT      0x0001
#define VSCP_BINARY_COMMAND_CODE_USER      0x0002
#define VSCP_BINARY_COMMAND_CODE_PASS      0x0003
#define VSCP_BINARY_COMMAND_CODE_CHALLENGE 0x0004
#define VSCP_BINARY_COMMAND_CODE_SEND      0x0005
#define VSCP_BINARY_COMMAND_CODE_RETR      0x0006
#define VSCP_BINARY_COMMAND_CODE_OPEN      0x0007
#define VSCP_BINARY_COMMAND_CODE_CLOSE     0x0008
#define VSCP_BINARY_COMMAND_CODE_CHKDATA   0x0009
#define VSCP_BINARY_COMMAND_CODE_CLEAR     0x000A
#define VSCP_BINARY_COMMAND_CODE_STAT      0x000B
#define VSCP_BINARY_COMMAND_CODE_INFO      0x000C
#define VSCP_BINARY_COMMAND_CODE_GETCHID   0x000D
#define VSCP_BINARY_COMMAND_CODE_SETGUID   0x000E
#define VSCP_BINARY_COMMAND_CODE_GETGUID   0x000F
#define VSCP_BINARY_COMMAND_CODE_VERSION   0x0010
#define VSCP_BINARY_COMMAND_CODE_SETFILTER 0x0011
#define VSCP_BINARY_COMMAND_CODE_SETMASK   0x0012
#define VSCP_BINARY_COMMAND_CODE_INTERFACE 0x0013
#define VSCP_BINARY_COMMAND_CODE_TEST      0x001E
#define VSCP_BINARY_COMMAND_CODE_WCYD      0x001F
#define VSCP_BINARY_COMMAND_CODE_SHUTDOWN  0x0020
#define VSCP_BINARY_COMMAND_CODE_RESTART   0x0021
#define VSCP_BINARY_COMMAND_CODE_TEXT      0x0022

#define VSCP_BINARY_COMMAND_CODE_USER_START    0xFF00
#define VSCP_BINARY_COMMAND_CODE_EVENT_CONFIRM 0xFFFF

/* ── Forward declaration ──────────────────────────────────────────────────── */
struct vscp_binary_ctx;

/* ── Operations table ─────────────────────────────────────────────────────── */

/**
 * @brief VSCP binary protocol operations table.
 *
 * Populate all function pointers and assign a pointer to this struct to
 * vscp_binary_ctx_t::ops before calling vscp_handle_binary_command().
 * The first argument to every callback is the context pointer.
 *
 * Mandatory field — vscp_handle_binary_command() returns VSCP_ERROR_INIT_MISSING
 * if this is NULL:
 *   reply
 *
 * All other fields are optional; set unused pointers to NULL.
 */
typedef struct vscp_binary_ops {

  /* ── Mandatory ────────────────────────────────────────────────────────── */

  /** Send a response frame to the client. */
  int (*reply)(struct vscp_binary_ctx *pctx,
               uint16_t command, uint16_t error,
               const uint8_t *parg, size_t len);

  /* ── Session ──────────────────────────────────────────────────────────── */

  /** Close the connection. */
  int (*quit)(struct vscp_binary_ctx *pctx);
  /** Validate username. */
  int (*user)(struct vscp_binary_ctx *pctx, const char *user);
  /** Validate password. */
  int (*password)(struct vscp_binary_ctx *pctx, const char *password);
  /** Generate / verify challenge. */
  int (*challenge)(struct vscp_binary_ctx *pctx, uint8_t *challenge, size_t challenge_len);
  /** Return VSCP_ERROR_SUCCESS if already authenticated. */
  int (*check_authenticated)(struct vscp_binary_ctx *pctx);

  /* ── Event I/O ────────────────────────────────────────────────────────── */

  /** Send an event from the client into the system. */
  int (*send_event)(struct vscp_binary_ctx *pctx, const vscp_event_t *pev);
  /** Retrieve one event from the input queue (non-async path). */
  int (*get_event)(struct vscp_binary_ctx *pctx, vscp_event_t **ppev);
  /** Retrieve one event as EventEx from the input queue (non-async path). */
  int (*get_eventex)(struct vscp_binary_ctx *pctx, vscp_event_ex_t *pex);
  /** Push an async event to the client. */
  int (*send_asyncevent)(struct vscp_binary_ctx *pctx, vscp_event_t *pev);

  /* ── Channel open/close ───────────────────────────────────────────────── */

  /** Open the async event channel. */
  int (*open)(struct vscp_binary_ctx *pctx);
  /** Close the async event channel. */
  int (*close)(struct vscp_binary_ctx *pctx);
  /** Return true if the async event channel is open. */
  bool (*is_open)(struct vscp_binary_ctx *pctx);

  /* ── Queue management ─────────────────────────────────────────────────── */

  /** Return number of events waiting in the input queue. */
  int (*check_data)(struct vscp_binary_ctx *pctx, uint32_t *pcount);
  /** Clear the input queue. */
  int (*clrall)(struct vscp_binary_ctx *pctx);

  /* ── Device metadata ──────────────────────────────────────────────────── */

  /** Return the channel id. */
  int (*get_chid)(struct vscp_binary_ctx *pctx, uint32_t *pchid);
  /** Set the device GUID. */
  int (*set_guid)(struct vscp_binary_ctx *pctx, uint8_t *pguid);
  /** Return the device GUID. */
  int (*get_guid)(struct vscp_binary_ctx *pctx, uint8_t *pguid);
  /** Return firmware version (10 bytes: major/minor/patch/build). */
  int (*get_version)(struct vscp_binary_ctx *pctx, uint8_t *pversion);
  /** Return statistical counters. */
  int (*statistics)(struct vscp_binary_ctx *pctx, vscp_statistics_t *pstats);
  /** Return channel status. */
  int (*info)(struct vscp_binary_ctx *pctx, vscp_status_t *pstatus);

  /* ── Filter / mask ────────────────────────────────────────────────────── */

  /** Set the event filter. */
  int (*setfilter)(struct vscp_binary_ctx *pctx, const vscpEventFilter *pfilter);
  /** Set the event mask. */
  int (*setmask)(struct vscp_binary_ctx *pctx, const vscpEventFilter *pfilter);

  /* ── Interface management ─────────────────────────────────────────────── */

  /** Return number of available interfaces. */
  int (*get_interface_count)(struct vscp_binary_ctx *pctx, uint16_t *pcount);
  /** Return info for interface at index. */
  int (*get_interface)(struct vscp_binary_ctx *pctx, uint16_t index, vscp_interface_info_t *pif);
  /** Open interface at index. */
  int (*interface_open)(struct vscp_binary_ctx *pctx, uint16_t idx);
  /** Close interface at index. */
  int (*interface_close)(struct vscp_binary_ctx *pctx, uint16_t idx);

  /* ── Diagnostics / control ────────────────────────────────────────────── */

  /** Run built-in test. */
  int (*test)(struct vscp_binary_ctx *pctx, const uint8_t *arg, size_t len);
  /** Return what-can-you-do capability bitmask. */
  int (*wcyd)(struct vscp_binary_ctx *pctx, uint64_t *pwcyd);
  /** Shut down the device. */
  int (*shutdown)(struct vscp_binary_ctx *pctx);
  /** Restart the device. */
  int (*restart)(struct vscp_binary_ctx *pctx);

  /* ── Mode switching ───────────────────────────────────────────────────── */

  /** Switch connection back to text (VSCP-link) mode. */
  int (*text)(struct vscp_binary_ctx *pctx);

  /* ── User-defined commands ────────────────────────────────────────────── */

  /** Handle any command code >= VSCP_BINARY_COMMAND_CODE_USER_START. */
  int (*user_command)(struct vscp_binary_ctx *pctx,
                      uint16_t command,
                      const uint8_t *parg, size_t len);

} vscp_binary_ops_t;

/* ── Context struct ───────────────────────────────────────────────────────── */

/**
 * @brief Per-connection context for the VSCP binary protocol.
 *
 * Allocate one instance per connection. Set ops to your operations table
 * before passing the context to vscp_handle_binary_command().
 */
typedef struct vscp_binary_ctx {
  /** Operations table. Must not be NULL and ops->reply must not be NULL. */
  const vscp_binary_ops_t *ops;
} vscp_binary_ctx_t;

/* ── Public API ───────────────────────────────────────────────────────────── */

/**
 * @brief Handle an incoming binary command.
 *
 * @param pctx    Pointer to context. Must have ops->reply set.
 * @param command Command code.
 * @param parg    Pointer to argument bytes (may be NULL if len == 0).
 * @param len     Length of argument.
 * @return VSCP_ERROR_SUCCESS on success, error code otherwise.
 */
int vscp_handle_binary_command(vscp_binary_ctx_t *pctx,
                               uint16_t command,
                               const uint8_t *parg,
                               size_t len);

/**
 * @brief Handle an inbound event (client -> device path).
 *
 * @param pctx   Pointer to context.
 * @param pEvent Pointer to the incoming event.
 * @return VSCP_ERROR_SUCCESS on success, error code otherwise.
 */
int vscp_handle_binary_event(vscp_binary_ctx_t *pctx, vscp_event_t *pEvent);

#ifdef __cplusplus
}
#endif

#endif /* __VSCP_BINARY_H__ */
