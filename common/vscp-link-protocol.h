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

#include <vscp.h>
#include <vscp-firmware-helper.h>

#ifdef __cplusplus
{
#endif

  /**
   * @file vscp-link-protocol.h
   * @defgroup vscp-link-protocol VSCP Link Protocol
   *
   * VSCP firmware link protocol API
   *
   * @{
   */

#define MAX_INQUEUE  10 // Max # events in in-queue
#define MAX_OUTQUEUE 10 // Max # events in out-queue

#define VSCP_LINK_MAX_USER_NAME_LENGTH 32 // Max length of user name
#define VSCP_LINK_MAX_PASSWORD_LENGTH  80 // Max length of password

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

  /*!
   * @fn vscp_link_connect
   * @brief Connect link
   *
   * @param pdata Pointer to context
   * @return VSCP_ERROR_TRUE on success, VSCP error code on failure
   */

  int
  vscp_link_connect(const void* pdata);

  /**
   * @fn vscp_link_disconnect
   * @brief Disconnect link
   *
   * @param pdata Pointer to context
   * @return VSCP_ERROR_TRUE on success, VSCP error code on failure
   */

  int
  vscp_link_disconnect(const void* pdata);

  /**
   * @fn vscp_link_idle_worker
   * @brief Do idle work
   *
   * @param pdata Pointer to context
   * @return VSCP_ERROR_TRUE on success, VSCP error code on failure
   */

  int
  vscp_link_idle_worker(const void* pdata);

  /**
   * @fn vscp_link_parser
   * @brief Parse command lines
   *
   * @param pdata Pointer to context
   * @param pbuf Pointer to command buffer holding part or hole incoming command.
   * @param pnext Pointer to pointer that is set to character after command (after CRLF).
   * @return VSCP_ERROR_TRUE on success, VSCP error code on failure
   */

  int
  vscp_link_parser(const void* pdata, char* pbuf, char** pnext);

  ///////////////////////////////////////////////////////////////////////////////
  //                          Command handlers
  ///////////////////////////////////////////////////////////////////////////////

  /**
    @fn vscp_link_doCmdNoop
    @brief noop command - Just return +OK\r\n

    @param pdata Pointer to user data
    @param cmd Command string after actual command (arguments)
    @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
  */

  int
  vscp_link_doCmdNoop(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdHelp
   * @brief Provide user help
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   *             Can give specific help on argument
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   *
   * Can give help about all commands if no argument is given or
   * just one command if a command name is given as parameter.
   */

  int
  vscp_link_doCmdHelp(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdQuit
   * @brief Quit client session
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdQuit(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdUser
   * @brief user name for login
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdUser(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdPassword
   * @brief password for login
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdPassword(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdChallenge
   * @brief challenge for login
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdChallenge(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdSend
   * @brief Send a VSCP event
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdSend(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdRetrieve
   * @brief Retrieve a VSCP events if any.
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdRetrieve(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdRcvLoop
   * @brief Enter receive loop
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdRcvLoop(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdQuitLoop
   * @brief Quit receive loop
   *
   *  @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdQuitLoop(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdCheckData
   * @brief Check if there are events ready to be received
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdCheckData(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdClearAll
   * @brief Clear all events in input queue
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdClearAll(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdStatistics
   * @brief Get VSCP_link_statistics
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdStatistics(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdInfo
   * @brief Get status information
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdInfo(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdGetChannelId
   * @brief Get channel id
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdGetChannelId(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdGetGUID
   * @brief Get channel GUID
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdGetGUID(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdSetGUID
   * @brief Set channel GUID
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdSetGUID(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdGetVersion
   * @brief Get version
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdGetVersion(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdSetFilter
   * @brief Set receive event filter
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdSetFilter(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdSetMask
   * @brief Set receive event mask
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdSetMask(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdTest
   * @brief test command - Do test and return result. The command
   *         is optional and if not implemented return "+OK\r\n"
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdTest(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdWhatCanYouDo
   * @brief What can you do dommand. Return info about what this node can do.
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdWhatCanYouDo(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdCommandAgain
   * @brief + command - Do last command again
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdCommandAgain(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdInterface
   * @brief interface command
   *     Old versions accepter 'list' and 'close' as argument.
   *     These arguments has been deprecated.
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   *             list
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_doCmdInterface(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdShutdown
   * @brief Shutdown the device into a safe state
   *
   * @param pdata Pointer to user data
   * @param cmd Command string after actual command (arguments)
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   *
   * The command can do nothing more than returning '+OK\r\n' if
   * not implemented.
   */

  int
  vscp_link_doCmdShutdown(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdRestart
   * @brief Restart the device
   *
   *  @param pdata Pointer to user data
   *  @param cmd Command string after actual command (arguments)
   *  @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   *
   *  The command can do nothing more than returning '+OK\r\n' if
   *  not implemented.
   */

  int
  vscp_link_doCmdRestart(const void* pdata, const char* pcmd);

  // --------------------------------------------------------------------------
  //                                  Binary
  // --------------------------------------------------------------------------

  /**
   * @fn vscp_link_doCmdbRetr
   * @brief Binary retreive
   *
   * @param pdata Pointer to user data
   *  @param cmd Command string after actual command (arguments)
   *  @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */
  int
  vscp_link_doCmdbRetr(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdbSend
   * @brief Binary send
   *
   * @param pdata Pointer to user data
   *  @param cmd Command string after actual command (arguments)
   *  @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */
  int
  vscp_link_doCmdbSend(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdbRcvLoop
   * @brief Enter binary receive loop
   *
   * @param pdata Pointer to user data
   *  @param cmd Command string after actual command (arguments)
   *  @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */
  int
  vscp_link_doCmdbRcvLoop(const void* pdata, const char* pcmd);

  /**
   * @fn vscp_link_doCmdSec
   * @brief Set security level
   *
   * @param pdata Pointer to user data
   *  @param cmd Command string after actual command (arguments)
   *  @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */
  int
  vscp_link_doCmdSec(const void* pdata, const char* pcmd);

  ///////////////////////////////////////////////////////////////////////////////
  //                             Callbacks
  ///////////////////////////////////////////////////////////////////////////////

  /**
   * @name Callbacks for a VSCP link protocol implementation
   */

  /**
   * @fn vscp_link_callback_welcome
   * @brief This callback is executed after a new connection is made to welcome a user.
   *
   * @param pdata Pointer to user data
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   *
   * Send welcome message for a new connection. This can be a multiline response and
   * the only requirement for the response is that the last line of the response
   * should be "+OK\r\n"
   */

  int
  vscp_link_callback_welcome(const void* pdata);

  /**
   * @fn vscp_link_callback_quit
   * @brief This callback is executed when the 'quit' command is received.
   *
   * @param pdata Pointer to user data
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   *
   * The callback should shutdown the connection with client after sending
   * the VSCP_LINK_MSG_GOODBY response or some other response starting with
   * "+OK "
   */

  int
  vscp_link_callback_quit(const void* pdata);

  /**
   * @fn vscp_link_callback_write_client
   * @brief Send message to client.
   *
   * @param pdata Pointer to user data
   * @param pmsg Pointer to message to send
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   *
   * Send null terminated data to client. The callback should send the data
   * and return a positive response if it was successful in doing so and a
   * negative response if not.
   */

  int
  vscp_link_callback_write_client(const void* pdata, const char* msg);

  /**
   * @fn vscp_link_callback_disconnect_client
   * @brief Disconnect client
   *
   * @param pdata Pointer to user data.
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_callback_disconnect_client(const void* pdata);

  /**
   * @fn vscp_link_callback_help
   * @brief This callback is executed when the 'help' command is received.
   *
   * Print command help. This can be a multiline reponse and
   * the last line should be "+OK\r\n"
   *
   * @param pdata Pointer to user data.
   * @param arg command data after "help " or NULL if just "help"
   *            Can be used to give command specific help.
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_callback_help(const void* pdata, const char* arg);

  /**
   * @fn vscp_link_callback_event_received
   * @brief Event has ben received from client.
   *
   * @param pdata Pointer to user data.
   * @param pex Pointer to received event ex.
   * @return VSCP_ERROR_SUCCESS if all is OK, errorcode otherwise.
   */

  int
  vscp_link_callback_event_received(const void* pdata, const vscpEvent* pev);

  /**
   * @fn vscp_link_callback_get_interface_count
   * @brief Get number of defined interfaces.
   *
   * @param pdata Pointer to user data.
   * @return Number of interfaces is returned. If no interfaces are defined
   *         zero is returned (as expected).
   */

  uint16_t vscp_link_callback_get_interface_count(const void* pdata);

  /**
   * @fn vscp_link_callback_get_interface
   * @brief Get one interface GUID.
   *
   * @param pdata Pointer to user data.
   * @param index Index of interface to get.
   * @param pif Pointer to interface information structure that wil get data for the interface.
   * @return VSCP_ERROR_SUCCESS if an interface is returned. If not VSCP_ERROR_UNKNOWN_ITEM
   *         is returned.
   */

  int
  vscp_link_callback_get_interface(const void* pdata, uint16_t index, vscp_interface_info_t* pif);

  /**
   * @brief Close interface
   * 
   * @param pdata Pointer to user data
   * @param user pguid Pointer to GUID for interface to close.
   * 
   * @return VSCP_ERROR_SUCCESS if the interface gest closed. 
   *  VSCP_ERROR_NOT_SUPPORTED if not supported. Other error 
   *  codes if an error occurs.
   */
  int
  vscp_link_callback_close_interface(const void* pdata, uint8_t *pguid);

  /**
   * @fn vscp_link_callback_check_user
   * @brief Check username
   *
   * @param pdata Pointer to user data
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
  vscp_link_callback_check_user(const void* pdata, const char* user);

  /**
   * @fn vscp_link_callback_check_password
   * @brief Check password
   *
   * @param pdata Pointer to user data
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
  vscp_link_callback_check_password(const void* pdata, const char* user);

  /**
   * @fn vscp_link_callback_challenge
   * @brief Dop challenge sequency
   *
   * @param pdata Pointer to user data
   * @param password Password to check
   * @return Return VSCP_ERROR_SUCCESS if logged in error code else.
   */

  int
  vscp_link_callback_challenge(const void* pdata, const char* password);

  /**
   * @fn vscp_link_callback_check_authenticated
   * @brief Check if client is authenticated
   *
   *  @param pdata Pointer to user data
   * @return Return VSCP_ERROR_SUCCESS if validated.
   */

  int
  vscp_link_callback_check_authenticated(const void* pdata);

  /**
   * @fn vscp_link_callback_check_privilege
   * @brief Check if client has enough rights to use command
   *
   *  @param pdata Pointer to user data
   *  @param priv Privilege level 0-15 needed to use command.
   * @return Return VSCP_ERROR_SUCCESS if privileged (>= priv).
   */

  int
  vscp_link_callback_check_privilege(const void* pdata, uint8_t priv);

  /**
   * @fn vscp_link_callback_test
   * @brief Do test command
   *
   *  @param pdata Pointer to user data
   * @param arg String argument to test command.
   *  @return Return VSCP_ERROR_SUCCESS if logged in error code else.
   */

  int
  vscp_link_callback_test(const void* pdata, const char* arg);

  /**
   * @fn vscp_link_callback_send
   * @brief Send event
   *
   * @param pdata Pointer to user data
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
  vscp_link_callback_send(const void* pdata, vscpEvent* pev);

  /**
   * @fn vscp_link_callback_retr
   * @brief Get event ('retr').
   *
   * @param pdata Pointer to user data
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
  vscp_link_callback_retr(const void* pdata, vscpEvent** pev);

  /**
   * @fn vscp_link_callback_enable_rcvloop
   * @brief Enable/disable rcvloop functionality
   *
   * @param pdata Pointer to user data
   *  @param bEnable Enable/disable rcvloop functionality
   *  @return Return VSCP_ERROR_SUCCESS if logged in error code else.
   */

  int
  vscp_link_callback_enable_rcvloop(const void* pdata, int bEnable);

  /**
   *  @fn vscp_link_callback_get_rcvloop_status
   *  @brief Get rcvloop status
   *
   *  @param pdata Pointer to user data
   *  @param pRcvLoop Pointer to integer that get rcvloop status (non zero if active).
   *  @return Return none zero if rcvloop is active
   */

  int
  vscp_link_callback_get_rcvloop_status(const void* pdata, int* pRcvLoop);

  /**
   * @fn vscp_link_callback_chkData
   * @brief Check # events in the output queue
   *
   * @param pdata Pointer to user data
   * @param pcount Pointer to variable that will get number of events in queue
   *  @return Return VSCP_ERROR_SUCCESS if logged in error code else.
   */

  int
  vscp_link_callback_chkData(const void* pdata, uint16_t* pcount);

  /**
   * @fn vscp_link_callback_clrAll
   * @brief Clear the output queue
   *
   * @param pdata Pointer to user data
   * @return Return VSCP_ERROR_SUCCESS if logged in error code else.
   */

  int
  vscp_link_callback_clrAll(const void* pdata);

  /**
   * @fn vscp_link_callback_get_channel_id
   * @brief Get channel id
   *
   * @param pdata Pointer to user data
   * @param chid Pointer to variable that will get channel id
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_get_channel_id(const void* pdata, uint16_t* pchid);

  /**
   * @fn vscp_link_callback_set_guid
   * @brief Set device GUID
   *
   * @param pdata Pointer to user data
   * @param pguid Pointer to GUID to set
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_set_guid(const void* pdata, uint8_t* pguid);

  /**
   * @fn vscp_link_callback_get_guid
   * @brief Get device GUID
   *
   * @param pdata Pointer to user data
   * @param pguid Pointer to GUID that will get device GUID
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_get_guid(const void* pdata, uint8_t* pguid);

  /**
   * @fn vscp_link_callback_get_version
   * @brief Get device version
   *
   * @param pdata Pointer to user data
   * @param pversion Pointer to four byte version array that will get device version
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_get_version(const void* pdata, uint8_t* pversion);

  /**
   * @fn vscp_link_callback_setFilter
   * @brief Set filter part of filter
   *
   * @param pdata Pointer to user data
   * @param pfilter Filter data. Mask data not used.
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_setFilter(const void* pdata, vscpEventFilter* pfilter);

  /**
   * @fn vscp_link_callback_setMask
   * @brief Set mask part of filter
   *
   * @param pdata Pointer to user data
   * @param pfilter Mask data. Filter data not used.
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_setMask(const void* pdata, vscpEventFilter* pfilter);

  /**
   * @fn vscp_link_callback_statistics
   * @brief Get statistics info
   *
   * @param pdata Pointer to user data
   * @param pStatistics Pointer to statistics structure
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_statistics(const void* pdata, VSCPStatistics* pStatistics);

  /**
   * @fn vscp_link_callback_info
   * @brief Set mask part of filter
   *
   * @param pdata Pointer to user data
   * @param pStatus Pointer to status structure
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_info(const void* pdata, VSCPStatus* pstatus);

  /**
   *
   * @fn vscp_link_callback_rcvloop
   * @brief Callback for active rcvloop
   *
   * @param pdata Pointer to user data
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
  vscp_link_callback_rcvloop(const void* pdata, vscpEvent** pev);

  /**
   * @fn vscp_link_callback_wcyd
   * @brief Get what can you do info
   *
   * @param pdata Pointer to user data
   * @param pwcyd Pointer to capabilities integer
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_wcyd(const void* pdata, uint64_t* pwcyd);

  /**
   * @fn vscp_link_callback_shutdown
   * @brief Shutdown the system to a safe state
   *
   * @param pdata Pointer to context
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   *
   * If not implemented just return VSCP_ERROR_SUCCESS. And yes you
   * probably don't want to implement it.
   */

  int
  vscp_link_callback_shutdown(const void* pdata);

  /**
   * @fn vscp_link_callback_restart
   * @brief Restart the system
   *
   * @param pdata Pointer to context
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   *
   * If not implemented just return VSCP_ERROR_SUCCESS. And yes you
   * probably don't want to implement it.
   */

  int
  vscp_link_callback_restart(const void* pdata);

  // --------------------------------------------------------------------------
  //                                 Binary
  // --------------------------------------------------------------------------

  /**
   * @fn vscp_link_callback_bretr
   * @brief Get binary encoded frame
   *
   * @param pdata Pointer to context
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */
  int
  vscp_link_callback_bretr(const void* pdata);

  /**
   * @fn vscp_link_callback_bsend
   * @brief Send binary encoded frame
   *
   * @param pdata Pointer to context
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */
  int
  vscp_link_callback_bsend(const void* pdata);

  /**
   * @fn vscp_link_callback_brcvloop
   * @brief Enter binary receive loop
   *
   * @param pdata Pointer to context
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_brcvloop(const void* pdata);

  /**
   * @fn vscp_link_callback_sec
   * @brief Set security level.
   * Argument is
   * 0 - No security
   * 1 - Encrypt with AES-128
   * 2 - Encrypt with AES-192
   * 3 - Encrypt with AES-256
   *
   * @param pdata Pointer to context
   * @return Return VSCP_ERROR_SUCCESS on success, else error code.
   */

  int
  vscp_link_callback_sec(const void* pdata);

  /**
  @}
  */

#ifdef __cplusplus
}
#endif

#endif // _VSCP_LINK_PROTOCOL_H_

// ============ END OF CALLBACKS ============