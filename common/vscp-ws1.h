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
#include "vscp.h"

// Defines in vscp-projdefs.h that should be used by the ws1 protocol implementation
// VSCP_WS1_MAX_CLIENTS - Maximum number of clients that can be connected at the same time

#define VSCP_WS1_MAX_PACKET_SIZE 512
#define VSCP_WS1_MAX_CLIENTS     10
#define VSCP_WS1_MAX_PACKET_SIZE 512 // Maximum size of received data packet

/*!
  Define the version for the ws1 protocol supported
  by this driver
*/
#define VSCP_WS1_PROTOCOL_VERSION         1
#define VSCP_WS1_PROTOCOL_MINOR_VERSION   0
#define VSCP_WS1_PROTOCOL_RELEASE_VERSION 0
#define VSCP_WS1_PROTOCOL_BUILD_VERSION   0

// VSCP ws1 error codes
#define VSCP_WS1_ERROR_NONE                0 // No error
#define VSCP_WS1_ERROR_SYNTAX              1 // Syntax error
#define VSCP_WS1_ERROR_UNKNOWN_COMMAND     2 // Unknown command
#define VSCP_WS1_ERROR_TX_BUFFER_FULL      3 // Transmit buffer full
#define VSCP_WS1_ERROR_MEMORY              4 // Problem allocating memory
#define VSCP_WS1_ERROR_NOT_AUTHORIZED      5 // Not authorized
#define VSCP_WS1_ERROR_NOT_AUTHORIZED_SEND 6 // Not authorized to send events
#define VSCP_WS1_ERROR_NOT_ALLOWED         7 // Not allowed to do that
#define VSCP_WS1_ERROR_PARSE               8 // Parse error, invalid format
#define VSCP_WS1_ERROR_UNKNOWN_TYPE        9 // Unknown ws1 message type, only know "COMMAND" and "EVENT"

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

struct _ws1_user {
  char username[32];
  char password[32];
  char fullname[64];
  uint32_t filtermask;
  uint32_t rights;
  char remotes[256];
  char events[256];
  char note[256];
} typedef ws1_user_t;

struct _ws1_connection_context {
  uint8_t sid[16];                    // Session ID for authentication and encryption
  bool bOpen;                         // Flag for open/closed channel
  bool bAuthenticated;                // Whether the client is authenticated
  vscpEventFilter filter;             // Filter/mask for VSCP events for this connection
  ws1_user_t pUser;                   // User information for this connection
  char buf[VSCP_WS1_MAX_PACKET_SIZE]; // Buffer for building responses
  void *pData; // Pointer to user data for this connection, can be used to store connection-specific information
} typedef ws1_connection_context_t;

/*!
  Initialize the WS1 protocol handler. This function should be called before any other functions
  in this module are used.
  @param ctx Pointer to a ws1_connection_context_t structure that will be initialized.
  This structure will be used to store connection-specific information for the WS1 protocol handler.
  @return Returns VSCP_ERROR_SUCCESS if the initialization was successful, or an appropriate error code if there was a
  failure.
*/
int
vscp_ws1_init(m_ws1_connection_context_t *ctx);

/*!
  Clean up any resources used by the WS1 protocol handler. This function should be called when the 
  protocol handler is no longer needed.
  @return Returns VSCP_ERROR_SUCCESS if the cleanup was successful, or an appropriate error code if there was a failure.
*/
int
vscp_ws1_clearup();

/*!
  Function called when a command is received via the WS1 protocol.
  The callback should process the command and send an appropriate reply using the
  vscp_ws1_callback_reply function.
  @fn vscp_ws1_callback_command
  @param command The command that was received. This will be a null-terminated string.
  @return Returns VSCP_ERROR_SUCCESS if the command was successfully processed,
  or an appropriate error code if there was a failure.
*/
int
vscp_ws1_handle_command(const char *command);

///////////////////////////////////////////////////////////////////////////////
//                               CALLBACKS
///////////////////////////////////////////////////////////////////////////////

/*!
  @fn vscp_ws1_get_user
  @brief Callback function to get user information based on username. This function should
  look up the user information for the given username and populate the provided ws1_user_t
  structure with the user's details.
  @param pUser Pointer to a ws1_user_t structure that will be populated with the user's information.
  @param username The username for which to retrieve the user information. This will be a
  null-terminated string.
  @param pdata Pointer to user data that can be used to store connection-specific information.
  @return Returns VSCP_ERROR_SUCCESS if the user information was successfully retrieved,
  or an appropriate error code if there was a failure (e.g. user not found).
*/
int
vscp_ws1_callback_get_user(ws1_user_t *pUser, const char *username, void *pdata);

/*!
  @fn vscp_ws1_callback_get_context
  @brief Callback function to get the connection context for a given session ID (SID).
  This function should look up the connection context for the given SID and populate the
  provided ws1_connection_context_t structure with the connection's details.
  @param pctx Pointer to a ws1_connection_context_t structure that will be populated with the
  connection's information.
  @param psid A pointer to a 128 bit session ID (SID) that is used to identify the connection.
  The SID is generated by the server and sent to the client as part of the authentication process.
  The client should use the SID as the IV for encrypting credentials and may also use it to identify
  the session.
  @param pdata Pointer to user data that can be used to store connection-specific information.
  @return Returns VSCP_ERROR_SUCCESS if the connection context was successfully retrieved,
  or an appropriate error code if there was a failure (e.g. connection not found).

  Some protocol handlers may allow for the context to be stored in the protocol handler itself and
  then this callback can be used to retrieve the context based on the SID. This allows for more
  efficient retrieval of the context without having to look it up in a global list of connections.
  If the protocol handler does not support storing the context internally, then this callback can be
  used to look up the context in a global list of connections based on the SID.
*/
int
vscp_ws1_callback_get_context(ws1_connection_context_t **pctx, void *pdata);

/*!
  @fn vscp_ws1_callback_get_key
  @brief Callback function to get the 128 bit encryption key for a given session. This function should
  populate the provided buffer with the encryption key.
  @param key Pointer to a buffer that will be populated with the encryption key.
  The buffer should be at least 16 bytes in size to hold the 128 bit key.
  @param pdata Pointer to user data that can be used to store connection-specific information.
  @return Returns VSCP_ERROR_SUCCESS if the encryption key was successfully retrieved,
  or an appropriate error code if there was a failure (e.g. connection not found, key not available).
*/
int
vscp_ws1_callback_get_key(uint8_t *key, void *pdata, void *pdata);

/*!
  Send a reply to a command received via the WS1 protocol. The reply will be sent
  asynchronously and the caller will not be blocked while waiting for the reply to be sent.
  @param response The response to send. This should be a null-terminated string.
  @param pdata Pointer to user data that can be used to store connection-specific information.
  @note The response should not include the leading '+' or '-' character, as this
  will be added automatically based on the success or failure of the command.
  @return Returns VSCP_ERROR_SUCCESS if the reply was successfully queued for sending,
  or an appropriate error code if there was a failure.
*/
int
vscp_ws1_callback_reply(const char *response, void *pdata);

/*!
  @fn vscp_ws1_callback_event
  @brief Callback function that is called when an event is received. The callback
  should process the event and respond appropriately.
  @param pEvent Pointer to the VSCP event that was received.
  @return Returns VSCP_ERROR_SUCCESS if the event was successfully processed and sent,
  or an appropriate error code if there was a failure.
*/
int
vscp_ws1_callback_event(vscp_event_t *pEvent, void *pdata);

/*!
  @fn vscp_ws1_callback_copyright
  @brief Callback function for the COPYRIGHT command. This command should return the copyright
  information for the VSCP firmware running on the device as something like
  "+;COPYRIGHT;Copyright (C) 2027 Company Name. All rights reserved.".
  @param pdata Pointer to user data that can be used to store connection-specific information.
  @return Returns VSCP_ERROR_SUCCESS if the copyright information was successfully sent,
  or an appropriate error code if there was a failure.
*/
int
vscp_ws1_callback_copyright(void *pdata);

/*!
  @fn vscp_ws1_callback_auth
  @brief Callback function for the AUTH command. This command should authenticate the client
  using the provided credentials and return a positive reply if authentication was successful,
  or a negative reply if authentication failed.
  @param psid A pointer to a 128 bit session ID that is used as the IV for encrypting the credentials and may also be
  used as a session identifier for the connection. The sid value is received by the server as part of the AUTH0 response
  that the server sends out on connection or after a CHALLENGE command. The client should use the sid value as the
  IV for encrypting the credentials and may also use it to identify the session.
  @param pcrypto A pointer to the encrypted credentials provided by the client for authentication. This will be a
  null-terminated string and may contain additional data separated by semicolons.
    The encrypted credentials should be encrypted using the session ID (SID) as the IV and a pre-shared encryption key.
  @param pdata Pointer to user data that can be used to store connection-specific information.
  @return Returns VSCP_ERROR_SUCCESS if authentication was successful, or an appropriate error code if there was a
  failure.

  The server should respond with

  "+;AUTH1;userid;name;password;fullname;filtermask;rights;remotes;events;note"

  if the credentials are valid. The fields in the response are as follows:
  - userid: A unique identifier for the user (e.g. a numeric ID or UUID).
  - name: The username of the authenticated user.
  - password: The password of the authenticated user (this should be encrypted or hashed in a secure manner).
  - fullname: The full name of the authenticated user.
  - filtermask: The event filter mask for the authenticated user, represented as a hexadecimal string (e.g. "0x1F").
  - rights: The permissions or rights assigned to the authenticated user, represented as a hexadecimal string (e.g.
  "0x0F").
  - remotes: The remote hosts or clients that the authenticated user is allowed to connect from, represented as a
  comma-separated list of IP addresses or hostnames (e.g. "192.168.1.100,192.168.1.101").
  - events: The events that the authenticated user is allowed to receive, represented as a comma-separated list of event
  types or IDs (e.g. "1,2,3").
  - note: Additional notes or information about the authenticated user.
*/
int
vscp_ws1_callback_auth(const uint8_t *psid, const uint8_t *pcrypto, void *pdata);

/*!
  @fn vscp_ws1_callback_challenge
  @brief Callback function for the CHALLENGE command. This command should generate a challenge
  response based on the provided data and return it to the client.
  @param p The data provided by the client for generating the challenge response. This will be a
  null-terminated string and may contain additional data separated by semicolons.
    The data may include information such as the client's capabilities, requested authentication method, or other
    relevant information that can be used to generate an appropriate challenge response.
  @param pdata Pointer to user data that can be used to store connection-specific information.
  @return Returns VSCP_ERROR_SUCCESS if the challenge response was successfully generated and sent,
  or an appropriate error code if there was a failure.
*/
int
vscp_ws1_callback_challenge(const char *p, void *pdata);

/*!
  @fn vscp_ws1_callback_open
  @brief Callback function for the OPEN command. This command should open the connection for receiving
  events and return a positive reply if the connection was successfully opened, or a negative reply if
  there was a failure.
    @param pdata Pointer to user data that can be used to store connection-specific information.
    @note When the connection is opened, the server should start sending events to the client based on the
    event filter that has been set for the connection. The server should also be prepared to receive commands
    from the client and respond to them appropriately while the connection is open.
  @return Returns VSCP_ERROR_SUCCESS if the connection was successfully opened, or an appropriate error code if there
  was a failure.
*/
int
vscp_ws1_callback_open(void *pdata);

/*!
  @fn vscp_ws1_callback_close
  @brief Callback function for the CLOSE command. This command should close the connection for receiving
  events and return a positive reply if the connection was successfully closed, or a negative reply if there was a
  failure.
    @param pdata Pointer to user data that can be used to store connection-specific information.
    @note When the connection is closed, the server should stop sending events to the client and should not
    expect to receive any more commands from the client until a new connection is opened. The server should also
    clean up any resources associated with the connection as needed.
    @return Returns VSCP_ERROR_SUCCESS if the connection was successfully closed, or an appropriate error code if there
    was a failure.
*/
int
vscp_ws1_callback_close(void *pdata);

/*!
  @fn vscp_ws1_callback_setfilter
  @brief Callback function for the SETFILTER command. This command should set the event filter based on the provided
  data and return a positive reply if the filter was successfully set, or a negative reply if there was a failure.
  @param pfilter The filter data provided by the client for setting the event filter. This will be a null-terminated
  string and may contain additional data separated by semicolons.
  @param pdata Pointer to user data that can be used to store connection-specific information.
  @return Returns VSCP_ERROR_SUCCESS if the event filter was successfully set, or an appropriate error code if there was
  a failure.
*/
int
vscp_ws1_callback_setfilter(const char *pfilter, void *pdata);

/*!
  @fn vscp_ws1_callback_clrqueue
  @brief Callback function for the CLRQUEUE command. This command should clear the event queue and return a positive
  reply if the queue was successfully cleared, or a negative reply if there was a failure.
    @param pdata Pointer to user data that can be used to store connection-specific information.
    @note When the event queue is cleared, any events that were queued for sending to the client should be discarded,
    and the server should not send those events to the client. The server should also ensure that any resources
    associated with the queued events are properly cleaned up.
    @
  @return Returns VSCP_ERROR_SUCCESS if the event queue was successfully cleared, or an appropriate error code if there
  was a failure.
*/
int
vscp_ws1_callback_clrqueue(void *pdata);

#endif /* __VSCP_WS2_H__ */