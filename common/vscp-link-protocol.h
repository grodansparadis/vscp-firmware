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

#define MAX_INQUEUE  10 // Max # events in in-queue
#define MAX_OUTQUEUE 10 // Max # events in out-queue

#define VSCP_LINK_MAX_USER_NAME_LENGTH   32 // Max length of user name
#define VSCP_LINK_MAX_PASSWORD_LENGTH    80 // Max length of password

#define VSCP_LINK_MSG_WELCOME       "Welcome to the VSCP daemon.\r\n"
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

#define VSCP_LINK_MSG_INVALID_PATH              "-OK - Invalid path.\r\n"
#define VSCP_LINK_MSG_FAILED_TO_GENERATE_SID    "-OK - Failed to generate sid.\r\n"

/* Standard help text (edit projedefs to change to custom)*/
#define VSCP_LINK_STD_HELP                                                      \
  "Help for the VSCP tcp/ip link interface\r\n"                                 \
  "====================================================================\r\n"    \
  "To get more information about a specific command issue 'help <command>'\r\n" \
  "noop              - No operation. Does nothing.\r\n"                         \
  "quit              - Close the connection.\r\n"                               \
  "user 'username'   - Username for login. \r\n"                                \
  "pass 'password'   - Password for login.  \r\n"                               \
  "challenge 'token' - Get session id.  \r\n"                                   \
  "send 'event'      - Send an event.   \r\n"                                   \
  "retr 'count'      - Retrieve n events from input queue.   \r\n"               \
  "rcvloop           - Will Retrieve events in an endless loop until "          \
  "the connection is closed by the client or 'quitloop' is sent.\r\n"           \
  "quitloop          - Terminate 'rcvloop'.\r\n"                                \
  "cdat/chkdata      - Check if there is data in the input queue.\r\n"          \
  "clra/clrall       - Clear input queue.\r\n"                                  \
  "stat              - Get statistical information.\r\n"                        \
  "info              - Get status info.\r\n"                                    \
  "chid              - Get channel id.\r\n"                                     \
  "sgid/setguid      - Set GUID for channel.\r\n"                               \
  "ggid/getguid      - Get GUID for channel.\r\n"                               \
  "vers/version      - Get VSCP daemon version.\r\n"                            \
  "sflt/setfilter    - Set incoming event filter.\r\n"                          \
  "smsk/setmask      - Set incoming event mask.\r\n"                            \
  "help [command]    - This command.\r\n"                                       \
  "test              - Do test sequence. Only used for debugging.\r\n"          \
  "wcyd/whatcanyoudo - Check server capabilities. \r\n"                         \
  "+OK\r\n"

// Command specific help
#define VSCP_LINK_STD_HELP_NOOP "'noop' always returns '+OK'\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_QUIT "'quit' close the connection.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_USER "'user name' Username for login.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_PASS "'pass password' Password for login.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_CHALLENGE "'challenge token' Get session id.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_SEND "'send head,class,type,obid,datetime,timestamp,GUID,data1,data2,data3,....,' Send an event.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_RETR "'retr count' Retrieve n events from input queue. Defult is one event.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_RCVLOOP "'rcvloop' Will retrieve events in an endless loop until " \
                                    "the connection is closed by the client or QUITLOOP is sent.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_QUITLOOP "'quitloop' Terminate RCVLOOP.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_CDTA "'cdta / chkdata' Check if there is data in the input queue.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_CLRA "'clra / clrall' Clear input queue.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_STAT "'stat' Get statistical information.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_INFO "'info' Get status info.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_CHID "'chid' Get channel id.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_SGID "'sgid / setguid' Set GUID for channel.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_GGID "'ggid / getguid' Get GUID for channel.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_VERS "'vers / version' Get VSCP device version.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_SFLT "'sflt / setfilter' Set incoming event filter.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_SMSK "'smsk / setmask' Set incoming event mask.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_TEST "'test' Do test sequence. Only used for debugging.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_WCYD "'wcyd / whatcanyoudo' Get server capabilities.\r\n+OK\r\n"
#define VSCP_LINK_STD_HELP_HELP "'help <operation>' This command.\r\n+OK\r\n"

#define VSCP_LINK_MALLOC(s)   malloc(s)
#define VSCP_LINK_REMALLOC(s) remalloc(s)
#define VSCP_LINK_FREE(x)     free(x)

/**
    Convert ASCII substring to unsigned long number
    Copyright (C) 1998, 1999  Henry Spencer.
    http://www.koders.com/c/fid83F5660A86069B2E2D29B9D3FC9013F76A9BCEB7.aspx
    
    \param src NULL terminated string to convert.
    \param srclen length of string, 0 means strlen( src )
    \param base The base for the conversion, 0 means figure it out.
    \param resultp Pointer that holds unsigned long result after conversion.
    \return  VSCP_ERROR_SUCCESS for success, else VSCP_ERROR_ERROR.
    
*/
int vscp_link_a2ul(const char *src, uint8_t srclen, uint8_t base, uint32_t *resultp);

/**
    Convert decimal byte to hex string
    \param d Decimal byte to convert
    \param pBuf Pointer to string buffer that will hold the result.
    \param len Number of digits to convert.
*/
void vscp_link_dec2hex(uint8_t d, char * pBuf, uint16_t len);


/**
    Convert hexadecimal integer to a decimal value
    \param pHex Pointer to hexadecimal string.
    \return Converted value in decimal form.
*/
uint32_t vscp_link_hex2dec(const char *pHex);

/**
    Read a value (hex or decimal)
    \param pString Pointe to value. A hex value should be
    be preceded by "0x" or "0X"
    \return Converted value in decimal form.  
*/
uint32_t vscp_link_readStringValue(const char *pString);

/**
  Find substring other string and rteturn pointer to it
  \param haystack String to search in.
  \param needle String to search for.
  \return Pointer to substring or NULL if not found.
*/
char *vscp_link_stristr(const char *haystack, const char *needle);

/**
    Get VSCP priority
    \param pEvent Pointer to VSCP event to set priority for.
    \return Priority (0-7) for event.
*/
unsigned char
vscp_link_getEventPriority(const vscpEvent *pEvent);

/**
    Check filter/mask to check if filter should be delivered

    filter ^ bit    mask    out
    ============================
        0           0       1    filter == bit, mask = don't care result = true
        0           1       1    filter == bit, mask = valid, result = true
        1           0       1    filter != bit, mask = don't care, result = true
        1           1       0    filter != bit, mask = valid, result = false

    Mask tells *which* bits that are of interest means
    it always returns true if bit set to zero (0=don't care).

    Filter tells the value for valid bits. If filter bit is == 1 the bits
    must be equal to get a true filter return.

    So a nill mask will let everything through

    A filter pointer set to NULL will let every event through.

    \return true if message should be delivered false if not.
    */
int
vscp_link_doLevel2Filter(const vscpEvent *pEvent, const vscpEventFilter *pFilter);

/**
    Called by the system when a new connection is made.
    \param pdata  Pointer to user data
    \return VSCP_ERROR_SUCCESS if command was executed correctly,
*/

int
vscp_link_connect(const void* pdata);

/**
    Called by the system when a connection is closed.
    \param pdata  Pointer to user data
    \return VSCP_ERROR_SUCCESS if command was executed correctly,
*/

int
vscp_link_disconnect(const void* pdata);

/**
    Called by the system when no command is available
    \param pdata  Pointer to user data
    \return VSCP_ERROR_SUCCESS if command was executed correctly,
*/
int 
vscp_link_idle_worker(const void* pdata);

/**
  \brief Parse a command from the client
  \param pdata  Pointer to user data
  \param cmd    Pointer to string that holds command to parse.
  \return VSCP_ERROR_SUCCESS if command was executed correctly,
    VSCP_ERROR_MISSING if command is not recognized, possible other error
    return if command was not executed correctly.
*/

int
vscp_link_parser(const void* pdata, const char* cmd);

/**
  \brief Parse GUID string into GUID array
  \param guid     Pointer to GUID array of 16 bytes to fill.
  \param strguid  Pointer to string that holds GUID to parse.
  \param endptr   Pointer to character after last parsed character.
                    Can be NULL.
  \return VSCP_ERROR_SUCCESS if GUID was parsed correctly,
*/

int
vscp_link_parseGuid(uint8_t* guid, const char* strguid, char** endptr);

/**
  \brief Write GUID to string
  \param strguid Buffer that the GUID string will be written to. This buffer
    must be at least 48 bytes long to hold GUID string on the form
    "FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF" including a terminating
    null character.
  \param guid Pointer to GUID array of 16 bytes to write.
  \return VSCP_ERROR_SUCCESS if GUID was written correctly,
*/

int
vscp_link_writeGuidToString(char* strguid, const uint8_t* guid);

/**
  \brief Parse a filter string and write data to a filter structure
  \param pfilter Pointer to filter structure to fill.
  \param strfilter Pointer to string that holds filter to parse.
  \return VSCP_ERROR_SUCCESS if filter was parsed correctly.
*/
int
vscp_link_parseFilter(vscpEventFilter* pfilter, const char* strfilter);

/**
  \brief Parse a filter mask and write data to a filter structure
  \param pfilter Pointer to filter structure to fill.
  \param strmask Pointer to string that holds mask to parse.
  \return VSCP_ERROR_SUCCESS if mask was parsed correctly.
*/
int
vscp_link_parseMask(vscpEventFilter* pfilter, const char* strmask);

/**
  \brief Parse an event on string form and write data to an event structure
  \param pev Pointer to event structure to fill.
  \param buf Pointer to string that holds event to parse.
  \return VSCP_ERROR_SUCCESS if event was parsed correctly.
*/
int
vscp_link_parseEvent(vscpEvent* pev, const char* buf);

/**
  \brief Parse an event on string form and write data to an event ex structure
  \param pev Pointer to event ex structure to fill.
  \param buf Pointer to string that holds event to parse.
  \return VSCP_ERROR_SUCCESS if event was parsed correctly.
*/
int
vscp_link_parseEventEx(vscpEventEx* pex, const char* streventex);

/**
  \brief Write event to string
  \param buf String buffer that will get event on string form
  \param size Size of string buffer
  \param pev Pointer to event
  \return VSCP_ERROR_SUCCESS if event was written correctly.
*/
int
vscp_link_eventToString(char* buf, size_t len, const vscpEvent* pev);

/**
  \brief Allocate a new event with zero data.
  \return Pointer to the new event if successful, NULL if not.
*/
vscpEvent*
vscp_link_newEvent(void);

/**
  \brief Delete an event and it's data.
  \param pev Address of event to delete.
  \return VSCP_ERROR_SUCCESS if event was deleted correctly,
*/
int
vscp_link_deleteEvent(vscpEvent** pev);

///////////////////////////////////////////////////////////////////////////////
// Command handlers
///////////////////////////////////////////////////////////////////////////////

/**
  \brief noop command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdNoop(const void* pdata, const char* cmd);

/**
  \brief noop command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
              Can give specific help on argument
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdHelp(const void* pdata, const char* cmd);

/**
  \brief noop command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdQuit(const void* pdata, const char* cmd);

/**
  \brief user command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdUser(const void* pdata, const char* cmd);

/**
  \brief password command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdPassword(const void* pdata, const char* cmd);

/**
  \brief challange command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdChallenge(const void* pdata, const char* cmd);

/**
  \brief send command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdSend(const void* pdata, const char* cmd);

/**
  \brief retr command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdRetrieve(const void* pdata, const char* cmd);

/**
  \brief rcvloop command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdRcvLoop(const void* pdata, const char* cmd);

/**
  \brief quitloop command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdQuitLoop(const void* pdata, const char* cmd);


/**
  \brief chkdata command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdCheckData(const void* pdata, const char* cmd);

/**
  \brief clrall command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdClearAll(const void* pdata, const char* cmd);

/**
  \brief stat command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdStatistics(const void* pdata, const char* cmd);

/**
  \brief info command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdInfo(const void* pdata, const char* cmd);

/**
  \brief getid command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdGetChannelId(const void* pdata, const char* cmd);

/**
  \brief getguid command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdGetGUID(const void* pdata, const char* cmd);

/**
  \brief setguid command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdSetGUID(const void* pdata, const char* cmd);

/**
  \brief vers command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdGetVersion(const void* pdata, const char* cmd);

/**
  \brief filter command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdSetFilter(const void* pdata, const char* cmd);

/**
  \brief mask command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdSetMask(const void* pdata, const char* cmd);

/**
  \brief test command - Do test and return result. The command
          is optional and if not implemented return "+OK\r\n"
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdTest(const void* pdata, const char* cmd);

/**
  \brief wcyd command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdWhatCanYouDo(const void* pdata, const char* cmd);

/**
  \brief + command - Just return +OK\r\n
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdCommandAgain(const void* pdata, const char* cmd);

/**
  \brief interface command - Just return +OK\r\n
      Old versions accepter 'list' and 'close' as argument.
      These arguments has been deprecated.
  \param pdata Pointer to user data
  \param cmd Command string after actual command (arguments)
              list
  \return VSCP_ERROR_SUCCESS if all was OK.
*/
int
vscp_link_doCmdInterface(const void* pdata, const char* cmd);






///////////////////////////////////////////////////////////////////////////////
//                             Callbacks
///////////////////////////////////////////////////////////////////////////////






// Send welcome message for new connection. This can be a multiline reponse and
// the only requirement on the response is that the last line should be "+OK\r\n"
int
vscp_link_callback_welcome(const void* pdata);

/**
  Shutdown connection with client after sending VSCP_LINK_MSG_GOODBY. Return 
  VSCP_ERROR_SUCCESS if all is OK
*/
int
vscp_link_callback_quit(const void* pdata);

// Send null terminated data to client
int
vscp_link_callback_write_client(const void* pdata, const char* msg);

// Read a command line form client. Can get max len chars.
int
vscp_link_callback_read_client(const void* pdata, const char* msg, size_t len);

// Disconnect client
int
vscp_link_callback_disconnect_client(const void* pdata);

// Print command help. This can be a multiline reponse and
// the last line should be "+OK\r\n"
// \param arg command data after "help " or NULL if just "help"
//          Can be used to give command specific help.
int
vscp_link_callback_help(const void* pdata, const char* arg);

// Event has ben received from client.
int
vscp_link_callback_event_received(const void* pdata, const vscpEvent* ev);

// Fetch one event from output queue (if any)
int
vscp_link_callback_event_get(const void* pdata, const vscpEvent* ev);

/**
  \brief Get number of defined interfaces.
  \return Number of interfaces is returned. If no interfaces are defined
          zero is returned (as expected).
*/

uint16_t vscp_link_callback_get_interface_count(const void* pdata);

/**
  \brief Get one interface GUID. 
  \param pdata Pointer to user data.
  \param index Index of interface to get.
  \param piface Pinter to 16-byte buffer that will get interface GUID.
                MSB first.
  \return VSCP_ERROR_SUCCESS if an interface is returned. If not VSCP_ERROR_UNKNOWN_ITEM
          is returned.
*/

int
vscp_link_callback_get_interface(const void* pdata, uint16_t index, const uint8_t* piface);

/**
  \brief Check username
  \param pdata Pointer to user data
  \param user Username to check

  There is two ways to implement this command handler

  1.) Save username without checking it and always respond with
      VSCP_LINK_MSG_USENAME_OK. This is the preferred way and don't give 
      information on valid usernames to clients.
  2.) Check username and respond with VSCP_LINK_MSG_USENAME_OK
      only if it is valid. Send VSCP_LINK_MSG_GOODBY and return
      VSCP_ERROR_ERROR. In this case the connection will be closed.

*/

int
vscp_link_callback_check_user(const void* pdata, const char* user);

/**
  \brief Check password
  \param pdata Pointer to user data
  \param password Password to check
  \return Return VSCP_ERROR_SUCCESS if logged in error code else.
  
  This is the point where a client logs in to the system. Write 
  VSCP_LINK_MSG_NEED_USERNAME to clinet if no user name has been entered
  prioor to password command. 
  Write VSCP_LINK_MSG_PASSWORD_ERROR to client if the password is not correct.
  Write VSCP_LINK_MSG_PASSWORD_OK to client if logged in.
  
*/

int
vscp_link_callback_check_password(const void* pdata, const char* user);

/**
  \brief Dop challenge sequency
  \param pdata Pointer to user data
  \param password Password to check
  \return Return VSCP_ERROR_SUCCESS if logged in error code else.
*/

int
vscp_link_callback_challenge(const void* pdata, const char* user);

/**
  \brief Check if client is authenticated
  \param pdata Pointer to user data
  \return Return VSCP_ERROR_SUCCESS if validated.
*/

int
vscp_link_callback_check_authenticated(const void* pdata);

/**
  \brief Check if client has enough rights to use command
  \param pdata Pointer to user data
  \param priv Privilege level 0-15 needed to use command
  \return Return VSCP_ERROR_SUCCESS if privileged.
*/

int
vscp_link_callback_check_privilege(const void* pdata, uint8_t priv);

/**
  \brief Do test command
  \param pdata Pointer to user data
  \param password Password to check
  \return Return VSCP_ERROR_SUCCESS if logged in error code else.
*/

int
vscp_link_callback_test(const void* pdata, const char* user);

/**
  \brief Send event
  \param pdata Pointer to user data
  \param pev Pointer to event to send
  \return Return VSCP_ERROR_SUCCESS if logged in error code else.

  VSCP_ERROR_INVALID_PERMISSION - VSCP_LINK_MSG_NOT_ACCREDITED is not logged in.
  VSCP_ERROR_INVALID_PERMISSION - VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR is returned if the user is not allowed to use send
  VSCP_ERROR_TRM_FULL - VSCP_LINK_MSG_BUFFER_FULL is returned if the output buffer is full.
  VSCP_LINK_MSG_UNABLE_TO_SEND_EVENT is returned if the event could not be sent.
*/

int
vscp_link_callback_send(const void* pdata, vscpEvent *pev);

/**
  \brief Get event
  \param pdata Pointer to user data
  \param pev Pointer to event that will get event data
  \return Return VSCP_ERROR_SUCCESS if logged in error code else.

  VSCP_ERROR_INVALID_HANDLE - (msg=VSCP_LINK_MSG_NOT_ACCREDITED) is not logged in.
  VSCP_ERROR_INVALID_PERMISSION - (msg=VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR) is returned if the user is not allowed to use send
  VSCP_ERROR_RCV_EMPTY - (msg=VSCP_LINK_MSG_NO_MSG) is returned if no event is available.
*/

int
vscp_link_callback_retr(const void* pdata, vscpEvent **pev);

/**
  \brief Enable/disable rcvloop functionality
  \param pdata Pointer to user data
  \return Return VSCP_ERROR_SUCCESS if logged in error code else.
*/
int
vscp_link_callback_enable_rcvloop(const void* pdata, int bEnable);

/**
  \brief Get rcvloop status
  \param pdata Pointer to user data
  \return Return none zero if rcvloop is active
*/
int
vscp_link_callback_get_rcvloop_status(const void* pdata);

/**
  \brief Check # events in the output queue
  \param pdata Pointer to user data
  \param pcount Pointer to variable that will get number of events in queue
  \return Return VSCP_ERROR_SUCCESS if logged in error code else.
*/
int
vscp_link_callback_chkData(const void* pdata, uint16_t* pcount);


/**
  \brief Clear the output queue
  \param pdata Pointer to user data
  \return Return VSCP_ERROR_SUCCESS if logged in error code else.
*/
int
vscp_link_callback_clrAll(const void* pdata);

/**
  \brief Get channel id
  \param pdata Pointer to user data
  \param chid Pointer to variable that will get channel id
  \return Return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_link_callback_get_channel_id(const void* pdata, uint16_t *pchid);

/**
  \brief Set device GUID
  \param pdata Pointer to user data
  \param pguid Pointer to GUID to set
  \return Return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_link_callback_set_guid(const void* pdata, uint8_t *pguid);

/**
  \brief Get device GUID
  \param pdata Pointer to user data
  \param pguid Pointer to GUID that will get device GUID
  \return Return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_link_callback_get_guid(const void* pdata, uint8_t *pguid);

/**
  \brief Get device version
  \param pdata Pointer to user data
  \param pguid Pointer to four byte version array that will get device version
  \return Return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_link_callback_get_version(const void* pdata, uint8_t *pversion);

/**
  \brief Set filter part of filter
  \param pdata Pointer to user data
  \param pfilter Filter data. Mask data not used.
  \return Return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_link_callback_setFilter(const void* pdata, vscpEventFilter *pfilter);

/**
  \brief Set mask part of filter
  \param pdata Pointer to user data
  \param pfilter Mask data. Filter data not used.
  \return Return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_link_callback_setMask(const void* pdata, vscpEventFilter *pfilter);

/**
  \brief Get statistics info
  \param pdata Pointer to user data
  \param pStatistics Pointer to statistics structure
  \return Return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_link_callback_statistics(const void* pdata, VSCPStatistics *pStatistics);

/**
  \brief Set mask part of filter
  \param pdata Pointer to user data
  \param pStatus Pointer to status structure
  \return Return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_link_callback_info(const void* pdata, VSCPStatus *pstatus);

/**
  \brief Callback for active rcvloop
  \param pdata Pointer to user data
  \param pev Pointer to VSCP event that will get data if available
  \return Return VSCP_ERROR_SUCCESS on success, else error code.

  VSCP_ERROR_SUCCESS - Event is available
  VSCP_ERROR_RCV_EMPTY - No event available
  VSCP_ERROR_TIMEOUT - Time to send '+OK\r\n
  Other error on error condition

  This function is called when the rcvloop is active. It should send events in the
  transmit fifo to the client and send a '+OK\r\n' response each second to the client.
*/
int
vscp_link_callback_rcvloop(const void* pdata, vscpEvent *pev);
