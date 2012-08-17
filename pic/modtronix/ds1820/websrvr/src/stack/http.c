/*********************************************************************
 *
 *               HTTP Implementation for Microchip TCP/IP Stack
 *
 **********************************************************************
 * FileName:        Http.c
 * Dependencies:    string.h
 *                  Stacktsk.h
 *                  Http.h
 *                  MPFS.h
 *                  TCP.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -O -Zg -E -C
 *
 *
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/14/01     Original
 * Nilesh Rajbharti     9/12/01     Released (Rev. 1.0)
 * Nilesh Rajbharti     2/9/02      Cleanup
 * Nilesh Rajbharti     5/22/02     Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti     7/9/02      Rev 2.1 (Fixed HTTPParse bug)
********************************************************************/
#define THIS_IS_HTTP_SERVER

#include <string.h>

#include "stacktsk.h"
#include "http.h"
#include "mpfs.h"
#include "tcp.h"


#if !defined(STACK_USE_HTTP_SERVER)
#error HTTP Server module is not enabled.
#error Remove this file from your project to reduce your code size.
#endif




#if (MAC_TX_BUFFER_SIZE <= 130 || MAC_TX_BUFFER_SIZE > 1500 )
#error HTTP : Invalid MAC_TX_BUFFER_SIZE value specified.
#endif


/*
 * Each dynamic variable within a CGI file should be preceeded with
 * this character.
 */
#define HTTP_VAR_ESC_CHAR       '%'
#define HTTP_DYNAMIC_FILE_TYPE  (HTTP_CGI)

/*
 * HTTP File Types
 */
#define HTTP_TXT        (0)
#define HTTP_HTML       (1)
#define HTTP_GIF        (2)
#define HTTP_CGI        (3)
#define HTTP_JPG        (4)
#define HTTP_JAVA       (5)
#define HTTP_WAV        (6)
#define HTTP_UNKNOWN    (7)


#define FILE_EXT_LEN    (3)
typedef struct _FILE_TYPES
{
    char fileExt[FILE_EXT_LEN+1];
} FILE_TYPES;




/*
 * Each entry in this structure must be in UPPER case.
 * Order of these entries must match with those defined
 * by "HTTP File Types" defines.
 */
static ROM FILE_TYPES httpFiles[] =
{
    { "TXT" },          // HTTP_TXT
    { "HTM" },          // HTTP_HTML
    { "GIF" },          // HTTP_GIF
    { "CGI" },          // HTTP_CGI
    { "JPG" },          // HTTP_JPG
    { "CLA" },          // HTTP_JAVA
    { "WAV" }           // HTTP_WAV
};
#define TOTAL_FILE_TYPES        ( sizeof(httpFiles)/sizeof(httpFiles[0]) )


typedef struct _HTTP_CONTENT
{
    ROM char typeString[20];
} HTTP_CONTENT;

/*
 * Content entry order must match with those "HTTP File Types" define's.
 */
static ROM HTTP_CONTENT httpContents[] =
{
    { "text/plain" },            // HTTP_TXT
    { "text/html" },             // HTTP_HTML
    { "image/gif" },             // HTTP_GIF
    { "text/html" },             // HTTP_CGI
    { "image/jpeg" },            // HTTP_JPG
    { "application/java-vm" },   // HTTP_JAVA
    { "audio/x-wave" }           // HTTP_WAV
};
#define TOTAL_HTTP_CONTENTS     ( sizeof(httpContents)/sizeof(httpConetents[0]) )

/*
 * HTTP FSM states for each connection.
 */
typedef enum _SM_HTTP
{
    SM_HTTP_IDLE,
    SM_HTTP_GET,
    SM_HTTP_NOT_FOUND,
    SM_HTTP_GET_READ,
    SM_HTTP_GET_PASS,
    SM_HTTP_GET_DLE,
    SM_HTTP_GET_HANDLE,
    SM_HTTP_GET_HANDLE_NEXT,
    SM_HTTP_GET_VAR,
    SM_HTTP_DISCONNECT,
    SM_HTTP_DISCONNECT_WAIT,
    SM_HTTP_HEADER,
    SM_HTTP_DISCARD
} SM_HTTP;


/*
 * Supported HTTP Commands
 */
typedef enum _HTTP_COMMAND
{
    HTTP_GET,
    HTTP_POST,
    HTTP_NOT_SUPPORTED,
    HTTP_INVALID_COMMAND
} HTTP_COMMAND;



/*
 * HTTP Connection Info - one for each connection.
 */
typedef struct _HTTP_INFO
{
    TCP_SOCKET socket;
    MPFS file;
    SM_HTTP smHTTP;
    BYTE smHTTPGet;
    WORD VarRef;
    BYTE bProcess;
    BYTE Variable;
    BYTE fileType;
} HTTP_INFO;
typedef BYTE HTTP_HANDLE;


typedef enum
{
    HTTP_NOT_FOUND,
    HTTP_OK,
    HTTP_HEADER_END,
    HTTP_NOT_AVAILABLE
} HTTP_MESSAGES;

/*
 * Following message order must match with that of HTTP_MESSAGES
 * enum.
 */
static ROM char *HTTPMessages[] =
{
    "HTTP/1.0 404 Not found\r\n\r\nNot found.\r\n",
    "HTTP/1.0 200 OK\r\n\Content-type: ",
    "\r\n\r\n",
    "HTTP/1.0 503 \r\n\r\nService Unavailable\r\n"
};


/*
 * Standard HTTP messages.
 */
ROM BYTE HTTP_OK_STRING[]                       =           \
                        "HTTP/1.0 200 OK\r\nContent-type: ";
#define HTTP_OK_STRING_LEN                                  \
                        (sizeof(HTTP_OK_STRING)-1)

ROM BYTE HTTP_HEADER_END_STRING[]               =           \
                        "\r\n\r\n";
#define HTTP_HEADER_END_STRING_LEN                          \
                        (sizeof(HTTP_HEADER_END_STRING)-1)

/*
 * HTTP Command Strings
 */
ROM BYTE HTTP_GET_STRING[]                      =           \
                        "GET";
#define HTTP_GET_STRING_LEN                                 \
                        (sizeof(HTTP_GET_STRING)-1)

/*
 * Default HTML file.
 */
ROM BYTE HTTP_DEFAULT_FILE_STRING[]             =           \
                        "INDEX.HTM";
#define HTTP_DEFAULT_FILE_STRING_LEN                        \
                        (sizeof(HTTP_DEFAULT_FILE_STRING)-1)


/*
 * Maximum nuber of arguments supported by this HTTP Server.
 */
#define MAX_HTTP_ARGS       (5)

/*
 * Maximum HTML Command String length.
 */
#define MAX_HTML_CMD_LEN    (80)


static HTTP_INFO HCB[MAX_HTTP_CONNECTIONS];


static void HTTPProcess(HTTP_HANDLE h);
static HTTP_COMMAND HTTPParse(BYTE *string,
                              BYTE** arg,
                              BYTE* argc,
                              BYTE* type);
static BOOL SendFile(HTTP_INFO* ph);




/*********************************************************************
 * Function:        void HTTPInit(void)
 *
 * PreCondition:    TCP must already be initialized.
 *
 * Input:           None
 *
 * Output:          HTTP FSM and connections are initialized
 *
 * Side Effects:    None
 *
 * Overview:        Set all HTTP connections to Listening state.
 *                  Initialize FSM for each connection.
 *
 * Note:            This function is called only one during lifetime
 *                  of the application.
 ********************************************************************/
void HTTPInit(void)
{
    BYTE i;

    for ( i = 0; i <  MAX_HTTP_CONNECTIONS; i++ )
    {
        HCB[i].socket = TCPListen(HTTP_PORT);
        HCB[i].smHTTP = SM_HTTP_IDLE;
    }
}



/*********************************************************************
 * Function:        void HTTPServer(void)
 *
 * PreCondition:    HTTPInit() must already be called.
 *
 * Input:           None
 *
 * Output:          Opened HTTP connections are served.
 *
 * Side Effects:    None
 *
 * Overview:        Browse through each connections and let it
 *                  handle its connection.
 *                  If a connection is not finished, do not process
 *                  next connections.  This must be done, all
 *                  connections use some static variables that are
 *                  common.
 *
 * Note:            This function acts as a task (similar to one in
 *                  RTOS).  This function performs its task in
 *                  co-operative manner.  Main application must call
 *                  this function repeatdly to ensure all open
 *                  or new connections are served on time.
 ********************************************************************/
void HTTPServer(void)
{
    BYTE conn;

    for ( conn = 0;  conn < MAX_HTTP_CONNECTIONS; conn++ )
        HTTPProcess(conn);
}


/*********************************************************************
 * Function:        static BOOL HTTPProcess(HTTP_HANDLE h)
 *
 * PreCondition:    HTTPInit() called.
 *
 * Input:           h   -   Index to the handle which needs to be
 *                          processed.
 *
 * Output:          Connection referred by 'h' is served.
 *
 * Side Effects:    None
 *
 * Overview:
 *
 * Note:            None.
 ********************************************************************/
static void HTTPProcess(HTTP_HANDLE h)
{
    BYTE httpData[MAX_HTML_CMD_LEN+1];
    HTTP_COMMAND httpCommand;
    WORD httpLength;
    BOOL lbContinue;
    BYTE *arg[MAX_HTTP_ARGS];
    BYTE argc;
    BYTE i;
    HTTP_INFO* ph;
    ROM char* romString;

    ph = &HCB[h];

    lbContinue = TRUE;
    while( lbContinue )
    {
        lbContinue = FALSE;

        /*
         * If during handling of HTTP socket, it gets disconnected,
         * forget about previous processing and return to idle state.
         */
        if ( !TCPIsConnected(ph->socket) )
        {
            ph->smHTTP = SM_HTTP_IDLE;
            break;
        }


        switch(ph->smHTTP)
        {
        case SM_HTTP_IDLE:
            if ( TCPIsGetReady(ph->socket) )
            {
                lbContinue = TRUE;

                httpLength = 0;
                while( httpLength < MAX_HTML_CMD_LEN &&
                       TCPGet(ph->socket, &httpData[httpLength++]) );
                httpData[httpLength] = '\0';
                TCPDiscard(ph->socket);

                ph->smHTTP = SM_HTTP_NOT_FOUND;
                argc = MAX_HTTP_ARGS;
                httpCommand = HTTPParse(httpData, arg, &argc, &ph->fileType);
                if ( httpCommand == HTTP_GET )
                {
                    /*
                     * If there are any arguments, this must be a remote command.
                     * Execute it and then send the file.
                     * The file name may be modified by command handler.
                     */
                    if ( argc > 1 )
                    {
                        /*
                         * Let main application handle this remote command.
                         */
                        HTTPExecCmd(&arg[0], argc);

                        /*
                         * Command handler must have modified arg[0] which now
                         * points to actual file that will be sent as a result of
                         * this remote command.
                         */

                        /*
                         * Assume that Web author will only use CGI or HTML
                         * file for remote command.
                         */
                        ph->fileType = HTTP_CGI;
                    }

                    ph->file = MPFSOpen(arg[0]);
                    if ( ph->file == MPFS_INVALID )
                    {
                        ph->Variable = HTTP_NOT_FOUND;
                        ph->smHTTP = SM_HTTP_NOT_FOUND;
                    }
                    else if ( ph->file == MPFS_NOT_AVAILABLE )
                    {
                        ph->Variable = HTTP_NOT_AVAILABLE;
                        ph->smHTTP = SM_HTTP_NOT_FOUND;
                    }
                    else
                    {
                        ph->smHTTP = SM_HTTP_HEADER;
                    }
                }
            }
            break;

        case SM_HTTP_NOT_FOUND:
            if ( TCPIsPutReady(ph->socket) )
            {
                romString = HTTPMessages[ph->Variable];

                while( (i = *romString++) )
                    TCPPut(ph->socket, i);

                TCPFlush(ph->socket);
                ph->smHTTP = SM_HTTP_DISCONNECT;
            }
            break;

        case SM_HTTP_HEADER:
            if ( TCPIsPutReady(ph->socket) )
            {
                lbContinue = TRUE;

                for ( i = 0; i < HTTP_OK_STRING_LEN; i++ )
                    TCPPut(ph->socket, HTTP_OK_STRING[i]);

                romString = httpContents[ph->fileType].typeString;
                while( (i = *romString++) )
                    TCPPut(ph->socket, i);

                for ( i = 0; i < HTTP_HEADER_END_STRING_LEN; i++ )
                    TCPPut(ph->socket, HTTP_HEADER_END_STRING[i]);

                if ( ph->fileType == HTTP_DYNAMIC_FILE_TYPE )
                    ph->bProcess = TRUE;
                else
                    ph->bProcess = FALSE;

                ph->smHTTPGet = SM_HTTP_GET_READ;
                ph->smHTTP = SM_HTTP_GET;
            }
            break;


        case SM_HTTP_GET:
            if ( TCPIsGetReady(ph->socket) )
                TCPDiscard(ph->socket);

            if ( SendFile(ph) )
            {
                MPFSClose();
                ph->smHTTP = SM_HTTP_DISCONNECT;

            }
            break;

        case SM_HTTP_DISCONNECT:
            if ( TCPIsConnected(ph->socket) )
            {
                if ( TCPIsPutReady(ph->socket) )
                {
                    TCPDisconnect(ph->socket);

                    /*
                     * Switch to not-handled state.  This FSM has
                     * one common action that checks for disconnect
                     * condition and returns to Idle state.
                     */
                    ph->smHTTP = SM_HTTP_DISCONNECT_WAIT;
                }
            }
            break;

        }
    }
}


/*********************************************************************
 * Function:        static BOOL SendFile(HTTP_INFO* ph)
 *
 * PreCondition:    None
 *
 * Input:           ph      -   A HTTP connection info.
 *
 * Output:          File reference by this connection is served.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None.
 ********************************************************************/
static BOOL SendFile(HTTP_INFO* ph)
{
    BOOL lbTransmit;
    BYTE c;

    MPFSGetBegin(ph->file);

    while( TCPIsPutReady(ph->socket) )
    {
        lbTransmit = FALSE;

        if ( ph->smHTTPGet != SM_HTTP_GET_VAR )
        {
            c = MPFSGet();
            if ( MPFSIsEOF() )
            {
                MPFSGetEnd();
                TCPFlush(ph->socket);
                return TRUE;
            }
        }

        if ( ph->bProcess )
        {
            switch(ph->smHTTPGet)
            {
            case SM_HTTP_GET_READ:
                if ( c == HTTP_VAR_ESC_CHAR )
                    ph->smHTTPGet = SM_HTTP_GET_DLE;
                else
                    lbTransmit = TRUE;
                break;

            case SM_HTTP_GET_DLE:
                if ( c == HTTP_VAR_ESC_CHAR )
                {
                    lbTransmit = TRUE;
                    ph->smHTTPGet = SM_HTTP_GET_READ;
                }
                else
                {
                    ph->Variable = (c - '0') << 4;
                    ph->smHTTPGet = SM_HTTP_GET_HANDLE;
                }
                break;

            case SM_HTTP_GET_HANDLE:
                ph->Variable |= (c - '0');

                ph->smHTTPGet = SM_HTTP_GET_VAR;
                ph->VarRef = HTTP_START_OF_VAR;

                break;

            case SM_HTTP_GET_VAR:
                ph->VarRef = HTTPGetVar(ph->Variable, ph->VarRef, &c);
                lbTransmit = TRUE;
                if ( ph->VarRef == HTTP_END_OF_VAR )
                    ph->smHTTPGet = SM_HTTP_GET_READ;
                break;

            default:
                while(1);
            }

            if ( lbTransmit )
                TCPPut(ph->socket, c);
        }
        else
            TCPPut(ph->socket, c);

    }

    ph->file = MPFSGetEnd();

    // We are not done sending a file yet...
    return FALSE;
}


/*********************************************************************
 * Function:        static HTTP_COMMAND HTTPParse(BYTE *string,
 *                                              BYTE** arg,
 *                                              BYTE* argc,
 *                                              BYTE* type)
 *
 * PreCondition:    None
 *
 * Input:           string      - HTTP Command String
 *                  arg         - List of string pointer to hold
 *                                HTTP arguments.
 *                  argc        - Pointer to hold total number of
 *                                arguments in this command string/
 *                  type        - Pointer to hold type of file
 *                                received.
 *                              Valid values are:
 *                                  HTTP_TXT
 *                                  HTTP_HTML
 *                                  HTTP_GIF
 *                                  HTTP_CGI
 *                                  HTTP_UNKNOWN
 *
 * Output:          HTTP FSM and connections are initialized
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            This function parses URL that may or may not
 *                  contain arguments.
 *                  e.g. "GET HTTP/1.0 thank.htm?name=MCHP&age=12"
 *                      would be returned as below:
 *                          arg[0] => GET
 *                          arg[1] => thank.htm
 *                          arg[2] => name
 *                          arg[3] => MCHP
 *                          arg[4] => 12
 *                          argc = 5
 *
 *                  This parses does not "de-escape" URL string.
 ********************************************************************/
static HTTP_COMMAND HTTPParse(BYTE *string,
                            BYTE** arg,
                            BYTE* argc,
                            BYTE* type)
{
    BYTE i;
    BYTE smParse;
    HTTP_COMMAND cmd;
    BYTE *ext;
    BYTE c;
    ROM char* fileType;

    enum
    {
        SM_PARSE_IDLE,
        SM_PARSE_ARG,
        SM_PARSE_ARG_FORMAT
    };

    smParse = SM_PARSE_IDLE;
    ext = NULL;
    i = 0;

    // Only "GET" is supported for time being.
    if ( !memcmppgm2ram(string, (ROM void*) HTTP_GET_STRING, HTTP_GET_STRING_LEN) )
    {
        string += (HTTP_GET_STRING_LEN + 1);
        cmd = HTTP_GET;
    }
    else
    {
        return HTTP_NOT_SUPPORTED;
    }

    // Skip white spaces.
    while( *string == ' ' )
        string++;

    c = *string;

    while ( c != ' ' &&  c != '\0' && c != '\r' && c != '\n' )

    {
        // Do not accept any more arguments than we haved designed to.
        if ( i >= *argc )
            break;

        switch(smParse)
        {
        case SM_PARSE_IDLE:
            arg[i] = string;
            c = *string;
            if ( c == '/' || c == '\\' )
                smParse = SM_PARSE_ARG;
            break;

        case SM_PARSE_ARG:
            arg[i++] = string;
            smParse = SM_PARSE_ARG_FORMAT;
            /*
             * Do not break.
             * Parameter may be empty.
             */

        case SM_PARSE_ARG_FORMAT:
            c = *string;
            if ( c == '?' || c == '&' )
            {
                *string = '\0';
                smParse = SM_PARSE_ARG;
            }
            else
            {
                // Recover space characters.
                if ( c == '+' )
                    *string = ' ';

                // Remember where file extension starts.
                else if ( c == '.' && i == 1 )
                {
                    ext = ++string;
                }

                else if ( c == '=' )
                {
                    *string = '\0';
                    smParse = SM_PARSE_ARG;
                }

                // Only interested in file name - not a path.
                else if ( c == '/' || c == '\\' )
                    arg[i-1] = string+1;

            }
            break;
        }
        string++;
        c = *string;
    }
    *string = '\0';

    *type = HTTP_UNKNOWN;
    if ( ext != NULL )
    {
        ext = (BYTE*)strupr((char*)ext);

        fileType = httpFiles[0].fileExt;
        for ( c = 0; c < TOTAL_FILE_TYPES; c++ )
        {
            if ( !memcmppgm2ram((void*)ext, (ROM void*)fileType, FILE_EXT_LEN) )
            {
                *type = c;
                break;
            }
            fileType += sizeof(FILE_TYPES);

        }
    }

    if ( i == 0 )
    {
        memcpypgm2ram(arg[0], (ROM void*)HTTP_DEFAULT_FILE_STRING,
                                     HTTP_DEFAULT_FILE_STRING_LEN);
        arg[0][HTTP_DEFAULT_FILE_STRING_LEN] = '\0';
        *type = HTTP_HTML;
        i++;
    }
    *argc = i;

    return cmd;
}





