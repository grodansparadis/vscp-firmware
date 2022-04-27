// FILE: vscp_link.h

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

///////////////////////////////////////////////////////////////////////////////
// vscp_link_parser
// 
// @param cmd  Command to parse.
// 

int vscp_link_parser( const char *cmd );

///////////////////////////////////////////////////////////////////////////////
// vscp_link_parser
//
// @param msg  Nessage to reply.
//

int vscp_link_reply( const char *msg );


int parseGuid( const char *strguid, char **endptr, uint8_t *guid );

int writeGuidToString( const uint8_t *guid, char *strguid );

vscpEvent *newEvent( void );

int deleteEvent( vscpEvent **pev );

int parseFilter( const char *strfilter, vscpEventFilter *evfilter );

int parseMask( const char *strmask, vscpEventFilter *evfilter );

int parseEvent( const char *strevent, vscpEvent *ev );

int parseEventEx( const char *streventex, vscpEventEx *evex );

int eventToString( const vscpEvent *ev, char *strevent, size_t len );

///////////////////////////////////////////////////////////////////////////////
// Command handlers
///////////////////////////////////////////////////////////////////////////////

int vscp_link_doCmdNoop( const char *cmd );
int vscp_link_doCmdHelp( const char *cmd );
int vscp_link_doCmdQuit( const char *cmd );
int vscp_link_doCmdUser( const char *cmd );
int vscp_link_doCmdPassword( const char *cmd );
int vscp_link_doCmdChallenge( const char *cmd );
int vscp_link_doCmdSend( const char *cmd );;
int vscp_link_doCmdRetrive( const char *cmd );
int vscp_link_doCmdRcvLoop( const char *cmd );
int vscp_link_doCmdQuitLoop( const char *cmd );
int vscp_link_doCmdCmdCheckData( const char *cmd );
int vscp_link_doCmdCheckData( const char *cmd );
int vscp_link_doCmdClearAll( const char *cmd );
int vscp_link_doCmdClearAll( const char *cmd );
int vscp_link_doCmdStatistics( const char *cmd );
int vscp_link_doCmdInfo( const char *cmd );
int vscp_link_doCmdGetChannelId( const char *cmd );
int vscp_link_doCmdSetChannelId( const char *cmd );
int vscp_link_doCmdSetChannelId( const char *cmd );
int vscp_link_doCmdGetGUID( const char *cmd );
int vscp_link_doCmdGetVersion( const char *cmd );
int vscp_link_doCmdSetFilter( const char *cmd );
int vscp_link_doCmdSetMask( const char *cmd );
int vscp_link_doCmdWhatCanYouDo( const char *cmd );
int vscp_link_doCmdCommandAgain( const char *cmd );
int vscp_link_doCmdDM( const char *cmd );
#ifdef VSCP_LINK_ENABLE_CMD_VAR
int vscp_link_doCmdVariable( const char *cmd );
#endif
#ifdef VSCP_LINK_ENABLE_CMD_INTERFACE
int vscp_link_doCmdInterface( const char *cmd );
#endif        

///////////////////////////////////////////////////////////////////////////////
// Callbacks
///////////////////////////////////////////////////////////////////////////////

// Send welcome message for new connection. This can be a multiline reponse and
// the last line should be "+OK\r\n" 
int vscp_link_callback_welcome( const char *msg );

// Send null terminated data to client
int vscp_link_callback_writeClient( const char *msg );

// Read a command line form client. Can get max len chars.
int vscp_link_callback_readClient( const char *msg, size_t len );

// Disconnect client
int vscp_link_callback_DisconnetClient( void );

// Print command help. This can be a multiline reponse and
// the last line should be "+OK\r\n"
// @param cmd command after "help " or NULL if just "help"
//          Can be used to give command specific help.   
int vscp_link_callback_Help( const char *cmd );

int vscp_link_callback_checkUser( const char *user );

int vscp_link_callback_checkPassword( const char *user );

// Event has ben received from client.
int vscp_link_callback_eventReceived( const vscpEvent *ev );

// Fetch one event from output queue (if any)
int vscp_link_callback_eventGet( const vscpEvent *ev );

