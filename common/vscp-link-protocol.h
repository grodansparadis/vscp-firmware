// FILE: vscp-link-protocol.h

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2022 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
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

#include "vscp.h"

#define MAX_INQUEUE         10      // Max # events in in-queue
#define MAX_OUTQUEUE        10      // Max # events in out-queue

#define VSCP_LINK_MSG_WELCOME                         "Welcome to the VSCP daemon.\r\n"
#define VSCP_LINK_MSG_OK                              "+OK - Success.\r\n"
#define VSCP_LINK_MSG_GOODBY                          "+OK - Connection closed by client.\r\n"
#define VSCP_LINK_MSG_GOODBY2                         "+OK - Connection closed.\r\n"
#define VSCP_LINK_MSG_USENAME_OK                      "+OK - User name accepted, password please\r\n"
#define VSCP_LINK_MSG_PASSWORD_OK                     "+OK - Ready to work.\r\n"
#define VSCP_LINK_MSG_QUEUE_CLEARED                   "+OK - All events cleared.\r\n"
#define VSCP_LINK_MSG_RECEIVE_LOOP                    "+OK - Receive loop entered. QUITLOOP to terminate.\r\n"
#define VSCP_LINK_MSG_QUIT_LOOP                       "+OK - Quit receive loop.\r\n"

#define VSCP_LINK_MSG_ERROR                           "-OK - Error\r\n"
#define VSCP_LINK_MSG_UNKNOWN_COMMAND                 "-OK - Unknown command\r\n"
#define VSCP_LINK_MSG_PARAMETER_ERROR                 "-OK - Invalid parameter or format\r\n"
#define VSCP_LINK_MSG_BUFFER_FULL                     "-OK - Buffer Full\r\n"
#define VSCP_LINK_MSG_NO_MSG                          "-OK - No event(s) available\r\n"

#define VSCP_LINK_MSG_PASSWORD_ERROR                  "-OK - Invalid username or password.\r\n"
#define VSCP_LINK_MSG_NOT_ACCREDITED                  "-OK - Need to log in to perform this command.\r\n"
#define VSCP_LINK_MSG_INVALID_USER                    "-OK - Invalid user.\r\n"
#define VSCP_LINK_MSG_NEED_USERNAME                   "-OK - Need a Username before a password can be entered.\r\n"

#define VSCP_LINK_MSG_MAX_NUMBER_OF_CLIENTS           "-OK - Max number of clients connected.\r\n"
#define VSCP_LINK_MSG_INTERNAL_ERROR                  "-OK - Server Internal error.\r\n"
#define VSCP_LINK_MSG_INTERNAL_MEMORY_ERROR           "-OK - Internal Memory error.\r\n"
#define VSCP_LINK_MSG_INVALID_REMOTE_ERROR            "-OK - Invalid or unknown peer.\r\n"

#define VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR             "-OK - User need higher privilege level to perform this operation.\r\n"
#define VSCP_LINK_MSG_INTERFACE_NOT_FOUND             "-OK - Interface not found.\r\n"
#define VSCP_LINK_MSG_UNABLE_TO_SEND_EVENT            "-OK - Unable to send event.\r\n"

#define VSCP_LINK_MSG_VARIABLE_NOT_DEFINED            "-OK - Variable is not defined.\r\n"
#define VSCP_LINK_MSG_VARIABLE_MUST_BE_EVENT_TYPE     "-OK - Variable must be of event type.\r\n"
#define VSCP_LINK_MSG_VARIABLE_NOT_STOCK              "-OK - Operation does not work with stock variables.\r\n"
#define VSCP_LINK_MSG_VARIABLE_NO_SAVE                "-OK - Variable could not be saved.\r\n"
#define VSCP_LINK_MSG_VARIABLE_NOT_NUMERIC            "-OK - Variable is not numeric.\r\n"
#define VSCP_LINK_MSG_VARIABLE_UNABLE_ADD             "-OK - Unable to add variable.\r\n"

#define VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR             "-OK - User need higher privilege level to perform this operation.\r\n"
#define VSCP_LINK_MSG_INTERFACE_NOT_FOUND             "-OK - Interface not found.\r\n"

#define VSCP_LINK_MSG_VARIABLE_NOT_DEFINED            "-OK - Variable is not defined.\r\n"
#define VSCP_LINK_MSG_MOT_ALLOWED_TO_SEND_EVENT       "-OK - Not allowed to sen this event (contact admin).\r\n"
#define VSCP_LINK_MSG_INVALID_PATH                    "-OK - Invalid path.\r\n"
#define VSCP_LINK_MSG_FAILED_TO_GENERATE_SID          "-OK - Failed to generate sid.\r\n"

#define VSCP_LINK_MALLOC(s)     malloc(s)
#define VSCP_LINK_REMALLOC(s)   remalloc(s)
#define VSCP_LINK_FREE(x)       free(x)

/*!
  @brief Parse a command from the client
  @param pdata  Pointer to user data
  @param cmd    Pointer to string that holds command to parse.
  @return VSCP_ERROR_SUCCESS if command was executed correctly, 
    VSCP_ERROR_MISSING if command is not recognized, possible other error
    return if command was not executed correctly.
*/

int vscp_link_parser(const void *pdata, const char *cmd);


/*!
  @brief Send a (reply) message to the client
  @param pdata  Pointer to user data
  @param msg    Pointer to null terminated string that holds message to send.
  @return VSCP_ERROR_SUCCESS if message was sent correctly,
*/

int vscp_link_reply(const void *pdata, const char *msg);

/*!
  @brief Parse GUID string into GUID array
  @param guid     Pointer to GUID array of 16 bytes to fill.
  @param strguid  Pointer to string that holds GUID to parse.
  @param endptr   Pointer to character after last parsed character. 
                    Can be NULL.
  @return VSCP_ERROR_SUCCESS if GUID was parsed correctly,
*/

int parseGuid(uint8_t *guid, const char *strguid, char **endptr );

/*!
  @brief Write GUID to string
  @param strguid Buffer that the GUID string will be written to. This buffer 
    must be at least 48 bytes long to hold GIOF string on the form
    "FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF" including a terminating
    null character.
  @param guid    Pointer to GUID array of 16 bytes to write.
  @return VSCP_ERROR_SUCCESS if GUID was parsed correctly,
*/
int writeGuidToString(char *strguid, const uint8_t *guid);

/*!
  @brief Parse a filter string and write data to a filter structure
  @param pfilter Pointer to filter structure to fill. 
  @param strfilter Pointer to string that holds filter to parse.
  @return VSCP_ERROR_SUCCESS if filter was parsed correctly.
*/
int parseFilter(vscpEventFilter *pfilter, const char *strfilter);

/*!
  @brief Parse a filter mask and write data to a filter structure
  @param pfilter Pointer to filter structure to fill. 
  @param strmask Pointer to string that holds mask to parse.
  @return VSCP_ERROR_SUCCESS if mask was parsed correctly.
*/
int parseMask(vscpEventFilter *pfilter, const char *strmask);

/*!
  @brief Parse an event on string form and write data to an event structure
  @param pev Pointer to event structure to fill. 
  @param buf Pointer to string that holds event to parse.
  @return VSCP_ERROR_SUCCESS if event was parsed correctly.
*/
int parseEvent(vscpEvent *pev, const char *buf);

/*!
  @brief Parse an event on string form and write data to an event ex structure
  @param pev Pointer to event ex structure to fill. 
  @param buf Pointer to string that holds event to parse.
  @return VSCP_ERROR_SUCCESS if event was parsed correctly.
*/
int parseEventEx(vscpEventEx *pex, const char *streventex);

/*!
  @brief Write event to string
  @param buf String buffer that will get event on string form
  @param size Size of string buffer
  @param pev Pointer to event
  @return VSCP_ERROR_SUCCESS if event was written correctly.
*/
int eventToString(char *buf, size_t len, const vscpEvent *pev);

/*!
  @brief Allocate a new event with zero data.
  @return Pointer to the new event if successful, NULL if not.
*/
vscpEvent *newEvent(void);

/*!
  @brief Delete an event and it's data.
  @param pev Address of event to delete.
  @return VSCP_ERROR_SUCCESS if event was deleted correctly,
*/
int deleteEvent( vscpEvent **pev );



///////////////////////////////////////////////////////////////////////////////
// Command handlers
///////////////////////////////////////////////////////////////////////////////

int vscp_link_doCmdNoop(const void *pdata, const char *cmd);
int vscp_link_doCmdHelp(const void *pdata, const char *cmd);
int vscp_link_doCmdQuit(const void *pdata, const char *cmd);
int vscp_link_doCmdUser(const void *pdata, const char *cmd);
int vscp_link_doCmdPassword(const void *pdata, const char *cmd);
int vscp_link_doCmdChallenge(const void *pdata, const char *cmd);
int vscp_link_doCmdSend(const void *pdata, const char *cmd);
int vscp_link_doCmdRetrive(const void *pdata, const char *cmd);
int vscp_link_doCmdRcvLoop(const void *pdata, const char *cmd);
int vscp_link_doCmdQuitLoop(const void *pdata, const char *cmd);
int vscp_link_doCmdCmdCheckData(const void *pdata, const char *cmd);
int vscp_link_doCmdCheckData(const void *pdata, const char *cmd);
int vscp_link_doCmdClearAll(const void *pdata, const char *cmd);
int vscp_link_doCmdClearAll(const void *pdata, const char *cmd);
int vscp_link_doCmdStatistics(const void *pdata, const char *cmd);
int vscp_link_doCmdInfo(const void *pdata, const char *cmd);
int vscp_link_doCmdGetChannelId(const void *pdata, const char *cmd);
int vscp_link_doCmdSetChannelId(const void *pdata, const char *cmd);
int vscp_link_doCmdSetChannelId(const void *pdata, const char *cmd);
int vscp_link_doCmdGetGUID(const void *pdata, const char *cmd);
int vscp_link_doCmdGetVersion(const void *pdata, const char *cmd);
int vscp_link_doCmdSetFilter(const void *pdata, const char *cmd);
int vscp_link_doCmdSetMask(const void *pdata, const char *cmd);
int vscp_link_doCmdWhatCanYouDo(const void *pdata, const char *cmd);
int vscp_link_doCmdCommandAgain(const void *pdata, const char *cmd);
int vscp_link_doCmdInterface(const void *pdata, const char *cmd);

///////////////////////////////////////////////////////////////////////////////
// Callbacks
///////////////////////////////////////////////////////////////////////////////

// Send welcome message for new connection. This can be a multiline reponse and
// the last line should be "+OK\r\n" 
int vscp_link_callback_welcome(const void *pdata, const char *msg);

// Send null terminated data to client
int vscp_link_callback_writeClient(const void *pdata, const char *msg);

// Read a command line form client. Can get max len chars.
int vscp_link_callback_readClient(const void *pdata, const char *msg, size_t len);

// Disconnect client
int vscp_link_callback_DisconnetClient(const void *pdata);

// Print command help. This can be a multiline reponse and
// the last line should be "+OK\r\n"
// @param cmd command after "help " or NULL if just "help"
//          Can be used to give command specific help.   
int vscp_link_callback_Help(const void *pdata, const char *cmd);

int vscp_link_callback_checkUser(const void *pdata, const char *user);

int vscp_link_callback_checkPassword(const void *pdata, const char *user);

// Event has ben received from client.
int vscp_link_callback_eventReceived(const void *pdata, const vscpEvent *ev);

// Fetch one event from output queue (if any)
int vscp_link_callback_eventGet(const void *pdata, const vscpEvent *ev);

