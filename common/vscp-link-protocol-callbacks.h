///////////////////////////////////////////////////////////////////////////////
//                        VSCP Link Protocol Callbacks
///////////////////////////////////////////////////////////////////////////////

/*
  All callbacks are deifned in this file so it can be copies or used as is. At the
  end is assigment of the callbacks to the op structure.
*/

#ifndef _VSCP_LINK_PROTOCOL_CALLBACKS_H_
#define _VSCP_LINK_PROTOCOL_CALLBACKS_H_

#include "vscp-link-protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Callbacks for a VSCP link protocol implementation
 */

/**
 * @fn vscp_link_callback_welcome
 * @brief This callback is executed after a new connection is made to welcome a user.
 *
 * @param pctx Pointer to user data
 * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
 *
 * Send welcome message for a new connection. This can be a multiline response and
 * the only requirement for the response is that the last line of the response
 * should be "+OK\r\n"
 */

int
vscp_link_callback_welcome(vscp_link_ctx_t *pctx);

/**
 * @fn vscp_link_callback_quit
 * @brief This callback is executed when the 'quit' command is received.
 *
 * @param pctx Pointer to user data
 * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
 *
 * The callback should shutdown the connection with client after sending
 * the VSCP_LINK_MSG_GOODBY response or some other response starting with
 * "+OK "
 */

int
vscp_link_callback_quit(vscp_link_ctx_t *pctx);

/**
 * @fn vscp_link_callback_write_client
 * @brief Send message to client.
 *
 * @param pctx Pointer to user data
 * @param pmsg Pointer to message to send
 * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
 *
 * Send null terminated data to client. The callback should send the data
 * and return a positive response if it was successful in doing so and a
 * negative response if not.
 */

int
vscp_link_callback_write_client(vscp_link_ctx_t *pctx, const char *msg);

/**
 * @fn vscp_link_callback_disconnect
 * @brief Disconnect client
 *
 * @param pctx Pointer to user data.
 * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
 */

int
vscp_link_callback_disconnect(vscp_link_ctx_t *pctx);

/**
 * @fn vscp_link_callback_help
 * @brief This callback is executed when the 'help' command is received.
 *
 * Print command help. This can be a multiline reponse and
 * the last line should be "+OK\r\n"
 *
 * @param pctx Pointer to user data.
 * @param arg command data after "help " or NULL if just "help"
 *            Can be used to give command specific help.
 * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
 */

int
vscp_link_callback_help(vscp_link_ctx_t *pctx, const char *arg);

/**
 * @fn vscp_link_callback_event_received
 * @brief Event has ben received from client.
 *
 * @param pctx Pointer to user data.
 * @param pex Pointer to received event ex.
 * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
 */

int
vscp_link_callback_event_received(vscp_link_ctx_t *pctx, const vscpEvent *pev);

/**
 * @fn vscp_link_callback_get_interface_count
 * @brief Get number of defined interfaces.
 *
 * @param pctx Pointer to user data.
 * @return Number of interfaces is returned. If no interfaces are defined
 *         zero is returned (as expected).
 */

uint16_t
vscp_link_callback_get_interface_count(vscp_link_ctx_t *pctx);

/**
 * @fn vscp_link_callback_get_interface
 * @brief Get one interface GUID.
 *
 * @param pctx Pointer to user data.
 * @param index Index of interface to get.
 * @param pif Pointer to interface information structure that wil get data for the interface.
 * @return VSCP_ERROR_SUCCESS if an interface is returned. If not VSCP_ERROR_UNKNOWN_ITEM
 *         is returned.
 */

int
vscp_link_callback_get_interface(vscp_link_ctx_t *pctx, uint16_t index, vscp_interface_info_t *pif);

/**
 * @brief Close interface
 *
 * @param pctx Pointer to user data
 * @param user pguid Pointer to GUID for interface to close.
 *
 * @return VSCP_ERROR_SUCCESS if the interface gest closed.
 *  VSCP_ERROR_NOT_SUPPORTED if not supported. Other error
 *  codes if an error occurs.
 */
int
vscp_link_callback_close_interface(vscp_link_ctx_t *pctx, uint8_t *pguid);

/**
 * @fn vscp_link_callback_check_user
 * @brief Check username
 *
 * @param pctx Pointer to user data
 * @param user Username to check
 *
 * There is two ways to implement this command handler
 *
 * 1.) Save username without checking it and always respond with
 *     VSCP_LINK_MSG_USERNAME_OK. This is the preferred way and don't give
 *     information on valid usernames to clients.
 * 2.) Check username and respond with VSCP_LINK_MSG_USENAME_OK
 *     only if it is valid. Send VSCP_LINK_MSG_GOODBY and return
 *     VSCP_ERROR_ERROR. In this case the connection will be closed.
 */

int
vscp_link_callback_check_user(vscp_link_ctx_t *pctx, const char *user);

/**
 * @fn vscp_link_callback_check_password
 * @brief Check password
 *
 * @param pctx Pointer to user data
 * @param password Password to check
 * @return Return VSCP_ERROR_SUCCESS if logged in error code else.
 *
 * This is the point where a client logs in to the system. Write
 * VSCP_LINK_MSG_NEED_USERNAME to client if no user name has been entered
 * prior to password command.
 * Write VSCP_LINK_MSG_PASSWORD_ERROR to client if the password is not correct.
 * Write VSCP_LINK_MSG_PASSWORD_OK to client if logged in.
 */

int
vscp_link_callback_check_password(vscp_link_ctx_t *pctx, const char *user);

/**
 * @fn vscp_link_callback_challenge
 * @brief Dop challenge sequency
 *
 * @param pctx Pointer to user data
 * @param password Password to check
 * @return Return VSCP_ERROR_SUCCESS if logged in error code else.
 */

int
vscp_link_callback_challenge(vscp_link_ctx_t *pctx, const char *password);

/**
 * @fn vscp_link_callback_check_authenticated
 * @brief Check if client is authenticated
 *
 *  @param pctx Pointer to user data
 * @return Return VSCP_ERROR_SUCCESS if validated.
 */

int
vscp_link_callback_check_authenticated(vscp_link_ctx_t *pctx);

/**
 * @fn vscp_link_callback_check_privilege
 * @brief Check if client has enough rights to use command
 *
 *  @param pctx Pointer to user data
 *  @param priv Privilege level 0-15 needed to use command.
 * @return Return VSCP_ERROR_SUCCESS if privileged (>= priv).
 */

int
vscp_link_callback_check_privilege(vscp_link_ctx_t *pctx, uint8_t priv);

/**
 * @fn vscp_link_callback_test
 * @brief Do test command
 *
 *  @param pctx Pointer to user data
 * @param arg String argument to test command.
 *  @return Return VSCP_ERROR_SUCCESS if logged in error code else.
 */

int
vscp_link_callback_test(vscp_link_ctx_t *pctx, const char *arg);

/**
 * @fn vscp_link_callback_send
 * @brief Send event
 *
 * @param pctx Pointer to user data
 * @param pex Pointer to event ex to send. The callback get ownership of the event
 * and is responsible for releasing it.
 * @return Return VSCP_ERROR_SUCCESS if logged in error code else.
 *
 * VSCP_ERROR_INVALID_PERMISSION - VSCP_LINK_MSG_NOT_ACCREDITED is not logged in.
 * VSCP_ERROR_INVALID_PERMISSION - VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR is returned if the user
 *     is not allowed to use send
 * VSCP_ERROR_TRM_FULL - VSCP_LINK_MSG_BUFFER_FULL is returned if the output buffer is full.
 * VSCP_LINK_MSG_UNABLE_TO_SEND_EVENT is returned if the event could not be sent.
 */

int
vscp_link_callback_send(vscp_link_ctx_t *pctx, vscpEvent *pev);

/**
 * @fn vscp_link_callback_retr
 * @brief Get event ('retr').
 *
 * @param pctx Pointer to user data
 * @param pev Pointer to pointer of event that will get new allocated event. If
 *            VSCP_ERROR_SUCCESS is returned this will point to a newly allocated
              event and it is up to the calling routine to release the memory.
              If no event is available the pointer will be NULL.
 * @return Return VSCP_ERROR_SUCCESS if logged in error code else.
 *
 * VSCP_ERROR_INVALID_HANDLE - (msg=VSCP_LINK_MSG_NOT_ACCREDITED) is not logged in.
 * VSCP_ERROR_INVALID_PERMISSION - (msg=VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR) is returned
 *    if the user is not allowed to use send
 * VSCP_ERROR_RCV_EMPTY - (msg=VSCP_LINK_MSG_NO_MSG) is returned if no event is available.
 */

int
vscp_link_callback_retr(vscp_link_ctx_t *pctx, vscpEvent **pev);

/**
 * @fn vscp_link_callback_enable_rcvloop
 * @brief Enable/disable rcvloop functionality
 *
 * @param pctx Pointer to user data
 *  @param bEnable Enable/disable rcvloop functionality
 *  @return Return VSCP_ERROR_SUCCESS if logged in error code else.
 */

int
vscp_link_callback_enable_rcvloop(vscp_link_ctx_t *pctx, int bEnable);

/**
 *  @fn vscp_link_callback_get_rcvloop_status
 *  @brief Get rcvloop status
 *
 *  @param pctx Pointer to user data
 *  @param pRcvLoop Pointer to integer that get rcvloop status (non zero if active).
 *  @return Return none zero if rcvloop is active
 */

int
vscp_link_callback_get_rcvloop_status(vscp_link_ctx_t *pctx, int *pRcvLoop);

/**
 * @fn vscp_link_callback_chkData
 * @brief Check # events in the output queue
 *
 * @param pctx Pointer to user data
 * @param pcount Pointer to variable that will get number of events in queue
 *  @return Return VSCP_ERROR_SUCCESS if logged in error code else.
 */

int
vscp_link_callback_chkData(vscp_link_ctx_t *pctx, uint16_t *pcount);

/**
 * @fn vscp_link_callback_clrAll
 * @brief Clear the output queue
 *
 * @param pctx Pointer to user data
 * @return Return VSCP_ERROR_SUCCESS if logged in error code else.
 */

int
vscp_link_callback_clrAll(vscp_link_ctx_t *pctx);

/**
 * @fn vscp_link_callback_get_channel_id
 * @brief Get channel id
 *
 * @param pctx Pointer to user data
 * @param chid Pointer to variable that will get channel id
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_link_callback_get_channel_id(vscp_link_ctx_t *pctx, uint16_t *pchid);

/**
 * @fn vscp_link_callback_set_guid
 * @brief Set device GUID
 *
 * @param pctx Pointer to user data
 * @param pguid Pointer to GUID to set
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_link_callback_set_guid(vscp_link_ctx_t *pctx, uint8_t *pguid);

/**
 * @fn vscp_link_callback_get_guid
 * @brief Get device GUID
 *
 * @param pctx Pointer to user data
 * @param pguid Pointer to GUID that will get device GUID
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_link_callback_get_guid(vscp_link_ctx_t *pctx, uint8_t *pguid);

/**
 * @fn vscp_link_callback_get_version
 * @brief Get device version
 *
 * @param pctx Pointer to user data
 * @param pversion Pointer to four byte version array that will get device version
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_link_callback_get_version(vscp_link_ctx_t *pctx, uint8_t *pversion);

/**
 * @fn vscp_link_callback_setFilter
 * @brief Set filter part of filter
 *
 * @param pctx Pointer to user data
 * @param pfilter Filter data. Mask data not used.
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_link_callback_setFilter(vscp_link_ctx_t *pctx, vscpEventFilter *pfilter);

/**
 * @fn vscp_link_callback_setMask
 * @brief Set mask part of filter
 *
 * @param pctx Pointer to user data
 * @param pfilter Mask data. Filter data not used.
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_link_callback_setMask(vscp_link_ctx_t *pctx, vscpEventFilter *pfilter);

/**
 * @fn vscp_link_callback_statistics
 * @brief Get statistics info
 *
 * @param pctx Pointer to user data
 * @param pStatistics Pointer to statistics structure
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_link_callback_statistics(vscp_link_ctx_t *pctx, vscp_statistics_t *pStatistics);

/**
 * @fn vscp_link_callback_info
 * @brief Set mask part of filter
 *
 * @param pctx Pointer to user data
 * @param pStatus Pointer to status structure
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_link_callback_info(vscp_link_ctx_t *pctx, vscp_status_t *pstatus);

/**
 *
 * @fn vscp_link_callback_rcvloop
 * @brief Callback for active rcvloop
 *
 * @param pctx Pointer to user data
 * @param pex Pointer to pointer of event that will get data if available
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * VSCP_ERROR_SUCCESS - Event is available
 * VSCP_ERROR_RCV_EMPTY - No event available
 * VSCP_ERROR_TIMEOUT - Time to send '+OK\r\n
 * Other error on error condition
 *
 * This function is called periodically when the rcvloop is active. It should send events in the
 * transmit fifo to the client and send a '+OK\r\n' response each second to the client.
 */

int
vscp_link_callback_rcvloop(vscp_link_ctx_t *pctx, vscpEvent **pev);

/**
 * @fn vscp_link_callback_wcyd
 * @brief Get what can you do info
 *
 * @param pctx Pointer to user data
 * @param pwcyd Pointer to capabilities integer
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_link_callback_wcyd(vscp_link_ctx_t *pctx, uint64_t *pwcyd);

/**
 * @fn vscp_link_callback_shutdown
 * @brief Shutdown the system to a safe state
 *
 * @param pctx Pointer to context
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * If not implemented just return VSCP_ERROR_SUCCESS. And yes you
 * probably don't want to implement it.
 */

int
vscp_link_callback_shutdown(vscp_link_ctx_t *pctx);

/**
 * @fn vscp_link_callback_restart
 * @brief Restart the system
 *
 * @param pctx Pointer to context
 * @return Return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * If not implemented just return VSCP_ERROR_SUCCESS. And yes you
 * probably don't want to implement it.
 */

int
vscp_link_callback_restart(vscp_link_ctx_t *pctx);

// --------------------------------------------------------------------------
//                                 Binary
// --------------------------------------------------------------------------

/**
   * @fn vscp_link_callback_binary
   * @brief Go to binary mode
   *
   * @param pctx Pointer to user data
   * @return VSCP_ERROR_SUCCESS if binary mode is supported, errorcode otherwise.
   */
int
vscp_link_callback_binary(vscp_link_ctx_t *pctx);


/* ─────────────────────────────────────────────────────────────────────────────
 * Default ops table
 *
 * Wires every vscp_link_ops_t field to its corresponding callback above.
 * Pass &vscp_link_default_ops (or a copy with selective overrides) to
 * vscp_link_init().
 * ───────────────────────────────────────────────────────────────────────────*/
static const vscp_link_ops_t vscp_link_default_ops = {
  /* ── Transport ──────────────────────────────────────────────── */
  .write_client        = vscp_link_callback_write_client,
  .disconnect          = vscp_link_callback_disconnect,
  .quit                = vscp_link_callback_quit,
  .welcome             = vscp_link_callback_welcome,

  /* ── Authentication ─────────────────────────────────────────── */
  .check_user          = vscp_link_callback_check_user,
  .check_password      = vscp_link_callback_check_password,
  .challenge           = vscp_link_callback_challenge,
  .check_authenticated = vscp_link_callback_check_authenticated,
  .check_privilege     = vscp_link_callback_check_privilege,

  /* ── Event I/O ──────────────────────────────────────────────── */
  .send                = vscp_link_callback_send,
  .chkdata             = vscp_link_callback_chkData,
  .clrall              = vscp_link_callback_clrAll,
  .retr                = vscp_link_callback_retr,
  .rcvloop             = vscp_link_callback_rcvloop,
  .enable_rcvloop      = vscp_link_callback_enable_rcvloop,
  .get_rcvloop_status  = vscp_link_callback_get_rcvloop_status,

  /* ── Node information ───────────────────────────────────────── */
  .get_guid            = vscp_link_callback_get_guid,
  .set_guid            = vscp_link_callback_set_guid,
  .get_version         = vscp_link_callback_get_version,
  .get_channel_id      = vscp_link_callback_get_channel_id,
  .wcyd                = vscp_link_callback_wcyd,
  .statistics          = vscp_link_callback_statistics,
  .info                = vscp_link_callback_info,

  /* ── Filter / buffer ────────────────────────────────────────── */
  .set_filter          = vscp_link_callback_setFilter,
  .set_mask            = vscp_link_callback_setMask,

  /* ── Interfaces ─────────────────────────────────────────────── */
  .get_interface_count = vscp_link_callback_get_interface_count,
  .get_interface       = vscp_link_callback_get_interface,
  .close_interface     = vscp_link_callback_close_interface,

  /* ── Misc ────────────────────────────────────────────────────── */
  .help_custom         = vscp_link_callback_help,
  .test                = vscp_link_callback_test,
  .shutdown            = vscp_link_callback_shutdown,
  .restart             = vscp_link_callback_restart,

  /* ── Binary mode ─────────────────────────────────────────────── */
  .binary              = vscp_link_callback_binary,
};

/**
@}
*/

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _VSCP_LINK_PROTOCOL_CALLBACKS_H_

// ============ END OF CALLBACKS ============