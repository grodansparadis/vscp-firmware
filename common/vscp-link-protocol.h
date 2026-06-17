// FILE: vscp-link-protocol.h

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
 *	This file is part of VSCP - Very Simple Control Protocol
 *	https://www.vscp.org
 *
 * ******************************************************************************
 */

#ifndef _VSCP_LINK_PROTOCOL_H_
#define _VSCP_LINK_PROTOCOL_H_

#include "vscp.h"
#include "vscp-fifo.h"
#include "vscp-firmware-helper.h"

#ifdef __cplusplus
extern "C" {
#endif

  /**
   * @file vscp-link-protocol.h
   * @defgroup vscp-link-protocol VSCP Link Protocol
   *
   * VSCP firmware link protocol API
   *
   * @{
   */

/*!
  @brief Major version.

  This is the major version of this vscp link protocol code.
*/
#define VSCP_LINK__MAJOR_VERSION 2

/*!
  @brief Minor Version-

  This is the minor version of this vscp link protocol code.
*/
#define VSCP_LINK__MINOR_VERSION 0   

/*!
  Max buffer for level II events. The buffer size is needed to
  convert an event to string. To handle all level II events
  512*5 + 110 = 2670 bytes is needed. In reality this is
  seldom needed so the value can be set to a lower value. In this
  case one should check the max data size for events that are of
  interest and set the max size accordingly
*/
#define VSCP_LINK_MAX_BUF (2680u)

#define VSCP_LINK_MAX_INQUEUE  10u // Max # events in in-queue
#define VSCP_LINK_MAX_OUTQUEUE 10u // Max # events in out-queue

#define VSCP_LINK_MAX_USER_NAME_LENGTH 32u // Max length of user name
#define VSCP_LINK_MAX_PASSWORD_LENGTH  80u // Max length of password

#define VSCP_LINK_MSG_WELCOME       "Welcome to the VSCP Link Protocol Interface.\r\n"
#define VSCP_LINK_MSG_OK            "+OK - Success.\r\n"
#define VSCP_LINK_MSG_GOODBY        "+OK - Goodby. Connection closed.\r\n"
#define VSCP_LINK_MSG_USENAME_OK    "+OK - User name accepted, password please\r\n"
#define VSCP_LINK_MSG_PASSWORD_OK   "+OK - Ready to work.\r\n"
#define VSCP_LINK_MSG_QUEUE_CLEARED "+OK - All events cleared.\r\n"
#define VSCP_LINK_MSG_RECEIVE_LOOP  "+OK - Receive loop entered. QUITLOOP to terminate.\r\n"
#define VSCP_LINK_MSG_QUIT_LOOP     "+OK - Quit receive loop.\r\n"

#define VSCP_LINK_MSG_ERROR           "-OK - Error\r\n"
#define VSCP_LINK_MSG_UNKNOWN_COMMAND "-OK - Unknown command\r\n"
#define VSCP_LINK_MSG_PARAMETER_ERROR "-OK - Invalid parameter or format\r\n"
#define VSCP_LINK_MSG_NOT_SUPPORTED   "-OK - Not supported\r\n"
#define VSCP_LINK_MSG_BUFFER_FULL     "-OK - Buffer Full\r\n"
#define VSCP_LINK_MSG_NO_MSG          "-OK - No event(s) available\r\n"

#define VSCP_LINK_MSG_PASSWORD_ERROR "-OK - Invalid username or password.\r\n"
#define VSCP_LINK_MSG_NOT_ACCREDITED "-OK - Need to log in to perform this command.\r\n"
#define VSCP_LINK_MSG_INVALID_USER   "-OK - Invalid user.\r\n"
#define VSCP_LINK_MSG_NEED_USERNAME  "-OK - Need a Username before a password can be entered.\r\n"

#define VSCP_LINK_MSG_MAX_NUMBER_OF_CLIENTS "-OK - Max number of clients connected.\r\n"
#define VSCP_LINK_MSG_INTERNAL_ERROR        "-OK - Server Internal error.\r\n"
#define VSCP_LINK_MSG_INTERNAL_MEMORY_ERROR "-OK - Internal Memory error.\r\n"
#define VSCP_LINK_MSG_INVALID_REMOTE_ERROR  "-OK - Invalid or unknown peer.\r\n"

#define VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR  "-OK - User need higher privilege level to perform this operation.\r\n"
#define VSCP_LINK_MSG_INTERFACE_NOT_FOUND  "-OK - Interface not found.\r\n"
#define VSCP_LINK_MSG_UNABLE_TO_SEND_EVENT "-OK - Unable to send event.\r\n"

#define VSCP_LINK_MSG_INVALID_PATH           "-OK - Invalid path.\r\n"
#define VSCP_LINK_MSG_FAILED_TO_GENERATE_SID "-OK - Failed to generate sid.\r\n"

#define VSCP_LINK_MSG_BINARY_ACK             "+OK - Binary mode enabled\r\n"
#define VSCP_LINK_MSG_BINARY_NACK            "-OK - Binary mode not supported\r\n"

/* Standard help text (edit projedefs to change to custom)*/

/*!
 * @brief Standard help text
 */
#define VSCP_LINK_STD_HELP_TEXT                                                 \
  "Help for the VSCP link protocol interface\r\n"                               \
  "====================================================================\r\n"    \
  "To get more information about a specific command issue 'help <command>'\r\n" \
  "noop              - No operation. Does nothing.\r\n"                         \
  "quit              - Close the connection.\r\n"                               \
  "user 'username'   - Username for login. \r\n"                                \
  "pass 'password'   - Password for login.  \r\n"                               \
  "challenge 'token' - Get session id.  \r\n"                                   \
  "send 'event'      - Send an event.   \r\n"                                   \
  "retr 'count'      - Retrieve n events from input queue.   \r\n"              \
  "rcvloop           - Start receive loop. \r\n"                                \
  "quitloop          - Terminate 'rcvloop'.\r\n"                                \
  "cdat/chkdata      - Check if there is data in the input queue.\r\n"          \
  "clra/clrall       - Clear input queue.\r\n"                                  \
  "stat              - Get statistical information.\r\n"                        \
  "info              - Get status info.\r\n"                                    \
  "chid              - Get channel id.\r\n"                                     \
  "sgid/setguid      - Set GUID for channel.\r\n"                               \
  "ggid/getguid      - Get GUID for channel.\r\n"                               \
  "vers/version      - Get VSCP device version.\r\n"                            \
  "sflt/setfilter    - Set incoming event filter.\r\n"                          \
  "smsk/setmask      - Set incoming event mask.\r\n"                            \
  "interface         - List interfaces on device.\r\n"                          \
  "interface list    - List interfaces on device.\r\n"                          \
  "interface close <guid> - Close interface <guid>.\r\n"                        \
  "help [command]    - This command.\r\n"                                       \
  "test              - Do test sequence. Only used for debugging.\r\n"          \
  "wcyd/whatcanyoudo - Check server capabilities. \r\n"                         \
  "shutdown          - Shutdown the device. \r\n"                               \
  "restart           - Restart the device. \r\n"                                \
  "binary            - Enter binary mode (if available). \r\n"                  \
  "+OK\r\n"

// Command specific help
#define VSCP_LINK_STD_HELP_NOOP      "'noop' always returns '+OK'\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_QUIT      "'quit' close the connection.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_USER      "'user name' Username for login.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_PASS      "'pass password' Password for login.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_CHALLENGE "'challenge token' Get session id.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_SEND      "'send head,class,type,obid,datetime,timestamp,GUID,data1,data2,data3,....,' Send an event.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_RETR      "'retr count' Retrieve n events from input queue. Defult is one event.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_RCVLOOP   "'rcvloop' Will retrieve events in an endless loop until " \
                                   "the connection is closed by the client or 'quit' or 'quitloop' is received.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_QUITLOOP  "'quitloop' Terminate receive loop.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_CDTA      "'cdta / chkdata' Check if there is data in the input queue.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_CLRA      "'clra / clrall' Clear input queue.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_STAT      "'stat' Get statistical information.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_INFO      "'info' Get status info.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_CHID      "'chid' Get channel id.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_SGID      "'sgid / setguid' Set GUID for channel.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_GGID      "'ggid / getguid' Get GUID for channel.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_VERS      "'vers / version' Get VSCP device version.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_SFLT      "'sflt / setfilter' Set incoming event filter.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_SMSK      "'smsk / setmask' Set incoming event mask.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_TEST      "'test' Do test sequence. Only used for debugging.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_INTERFACE "'interface' or 'interface list' List interfaces on device. 'interface close <guid> Close interface <guid>'\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_WCYD      "'wcyd / whatcanyoudo' Get server capabilities.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_HELP      "'help <operation>' This command.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_RESTART   "'restart' Restart the device.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_SHUTDOWN  "'shutdown' Shutdown the device.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_BINARY    "'binary' Enter binary mode, all communication will be binary if confirmed.\r\n+OK\r\n"



/* Forward declaration so vscp_link_ctx can hold a pointer to the ops table
   that is defined later in this same header. */
typedef struct vscp_link_ops vscp_link_ops_t;

typedef struct vscp_link_ctx {
  struct vscp_link_ctx *next;                // Linkage to list of connections
  const vscp_link_ops_t *ops;                // Operations table for this connection
  int id;                                    // Context ID
  int sock;                                  // Socket
  uint8_t guid[16];                          // GUID for client
  char user[VSCP_LINK_MAX_USER_NAME_LENGTH]; // Username storage
  vscp_fifo_t fifoEventsIn;                  // VSCP event receive fifo
  vscp_fifo_t fifoEventsOut;                 // VSCP event send fifo
  unsigned bValidated : 1;                   // User is validated
  unsigned bRcvLoop : 1;                     // Receive loop is enabled
  unsigned bBinary : 1;                      // Binary mode
  unsigned privLevel : 4;                    // User privilege level 0-15
  vscpEventFilter filter;                    // Filter for events sent to client
  vscp_statistics_t statistics;              // VSCP Statistics
  vscp_status_t status;                      // VSCP status
  uint32_t last_rcvloop_time;                // Time in milliseconds of last received event
  void *user_data; // Pointer to transport-specific data (e.g. socket handle, mg_connection*)
} vscp_link_ctx_t;


/**
 * @brief VSCP link protocol operations table.
 *
 * Pass a pointer to a populated instance of this struct to
 * vscp_link_init(). Fields left NULL cause the library to use
 * its built-in default behaviour for that operation.
 *
 * Mandatory fields (must not be NULL):
 *   write_client, disconnect, check_authenticated, check_privilege,
 *   send, retr
 */
typedef struct vscp_link_ops {

  /* ── Transport ─────────────────────────────────────────────── */
  /** Send null-terminated string to client. */
  int (*write_client)(vscp_link_ctx_t *ctx, const char *msg);
  /** Handle 'disconnect' command. NULL → disconnect the client. */
  int (*disconnect)(vscp_link_ctx_t *ctx);
  /** Handle 'quit' command. NULL → sends VSCP_LINK_MSG_GOODBY and disconnects. */
  int (*quit)(vscp_link_ctx_t *ctx);
  /** Send welcome banner. NULL → sends VSCP_LINK_MSG_OK. */
  int (*welcome)(vscp_link_ctx_t *ctx);

  /* ── Authentication ─────────────────────────────────────────── */
  int (*check_user)(vscp_link_ctx_t *ctx, const char *user);
  int (*check_password)(vscp_link_ctx_t *ctx, const char *pass);
  int (*challenge)(vscp_link_ctx_t *ctx, const char *arg);
  /** Returns VSCP_ERROR_SUCCESS if authenticated, error otherwise. */
  int (*check_authenticated)(vscp_link_ctx_t *ctx);
  /** Returns VSCP_ERROR_SUCCESS if privLevel >= priv, error otherwise. */
  int (*check_privilege)(vscp_link_ctx_t *ctx, uint8_t priv);

  /* ── Event I/O ──────────────────────────────────────────────── */
  /** Consume an event sent by the remote client. Library owns pev on entry. */
  int (*send)(vscp_link_ctx_t *ctx, vscp_event_t *pev);
  int (*chkdata)(vscp_link_ctx_t *ctx, uint16_t *pcount);
  int (*clrall)(vscp_link_ctx_t *ctx);
  /** Produce one event for the remote client. Sets *ppev or returns VSCP_ERROR_RCV_EMPTY. */
  int (*retr)(vscp_link_ctx_t *ctx, vscp_event_t **ppev);
  /** Called repeatedly during rcvloop. May return VSCP_ERROR_TIMEOUT to emit keepalive. */
  int (*rcvloop)(vscp_link_ctx_t *ctx, vscp_event_t **ppev);
  int (*enable_rcvloop)(vscp_link_ctx_t *ctx, int enable);
  int (*get_rcvloop_status)(vscp_link_ctx_t *ctx, int *pStatus);

  /* ── Node information ───────────────────────────────────────── */
  int (*get_guid)(vscp_link_ctx_t *ctx, uint8_t *pguid);
  int (*set_guid)(vscp_link_ctx_t *ctx, uint8_t *pguid);
  int (*get_version)(vscp_link_ctx_t *ctx, uint8_t *pversion);
  int (*get_channel_id)(vscp_link_ctx_t *ctx, uint16_t *pchid);
  int (*wcyd)(vscp_link_ctx_t *ctx, uint64_t *pwcyd);
  int (*statistics)(vscp_link_ctx_t *ctx, vscp_statistics_t *ps);
  int (*info)(vscp_link_ctx_t *ctx, vscp_status_t *ps);

  /* ── Filter / buffer ────────────────────────────────────────── */
  int (*set_filter)(vscp_link_ctx_t *ctx, vscpEventFilter *pf);
  int (*set_mask)(vscp_link_ctx_t *ctx, vscpEventFilter *pf);  
  

  /* ── Interfaces ─────────────────────────────────────────────── */
  uint16_t (*get_interface_count)(vscp_link_ctx_t *ctx);
  int (*get_interface)(vscp_link_ctx_t *ctx, uint16_t idx, vscp_interface_info_t *pif);
  int (*close_interface)(vscp_link_ctx_t *ctx, uint8_t *pguid);

  /* ── Misc ────────────────────────────────────────────────────── */
  int (*help_custom)(vscp_link_ctx_t *ctx, const char *arg);
  int (*test)(vscp_link_ctx_t *ctx, const char *arg);
  int (*shutdown)(vscp_link_ctx_t *ctx);
  int (*restart)(vscp_link_ctx_t *ctx);

  /* ── Binary mode ─────────────────────────────────────────────── */
  int (*binary)(vscp_link_ctx_t *ctx);

} vscp_link_ops_t;

  /*!
   * @fn vscp_link_connect
   * @brief Connect link
   *
   * @param pctx Pointer to context
   * @return VSCP_ERROR_TRUE on success, VSCP error code on failure
   */

  int
  vscp_link_connect(vscp_link_ctx_t* pctx);

  /**
   * @fn vscp_link_disconnect
   * @brief Disconnect link
   *
   * @param pctx Pointer to context
   * @return VSCP_ERROR_TRUE on success, VSCP error code on failure
   */

  int
  vscp_link_disconnect(vscp_link_ctx_t* pctx);

  /**
   * @fn vscp_link_idle_worker
   * @brief Do idle work
   *
   * @param pctx Pointer to context
   * @return VSCP_ERROR_TRUE on success, VSCP error code on failure
   */

  int
  vscp_link_idle_worker(vscp_link_ctx_t* pctx);

  /**
   * @fn vscp_link_parser
   * @brief Parse command lines
   *
   * @param pctx Pointer to context
   * @param pbuf Pointer to null terminated string holding a full command. 
   * Not including the command terminator (e.g. \r\n).
   * @return VSCP_ERROR_TRUE on success, VSCP error code on failure
   */

  int
  vscp_link_parser(vscp_link_ctx_t* pctx, char* pbuf);

  /**
  @}
  */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _VSCP_LINK_PROTOCOL_H_
