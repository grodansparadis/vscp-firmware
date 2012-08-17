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
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
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
#include "http.h"
#include "debug.h"
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
#define HTTP_DYNAMIC_FILE_TYPE  (HTTP_FILETYPE_CGI)

/*
 * HTTP File Types
 */
#define HTTP_FILETYPE_TXT        (0)
#define HTTP_FILETYPE_HTML       (1)
#define HTTP_FILETYPE_GIF        (2)
#define HTTP_FILETYPE_CGI        (3)
#define HTTP_FILETYPE_JPG        (4)
#define HTTP_FILETYPE_JAVA       (5)
#define HTTP_FILETYPE_WAV        (6)
#define HTTP_FILETYPE_UNKNOWN    (7)


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
    { "TXT" },          // HTTP_FILETYPE_TXT
    { "HTM" },          // HTTP_FILETYPE_HTML
    { "GIF" },          // HTTP_FILETYPE_GIF
    { "CGI" },          // HTTP_FILETYPE_CGI
    { "JPG" },          // HTTP_FILETYPE_JPG
    { "CLA" },          // HTTP_FILETYPE_JAVA
    { "WAV" }           // HTTP_FILETYPE_WAV
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
    { "text/plain" },            // HTTP_FILETYPE_TXT
    { "text/html" },             // HTTP_FILETYPE_HTML
    { "image/gif" },             // HTTP_FILETYPE_GIF
    { "text/html" },             // HTTP_FILETYPE_CGI
    { "image/jpeg" },            // HTTP_FILETYPE_JPG
    { "application/java-vm" },   // HTTP_FILETYPE_JAVA
    { "audio/x-wave" }           // HTTP_FILETYPE_WAV
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
    SM_HTTP_GET_VAR_GRP,
    SM_HTTP_GET_VAR_VALMSB,
    SM_HTTP_GET_VAR_VALLSB,
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
    HTTP_CMD_GET,
    HTTP_CMD_POST,
    HTTP_CMD_NOT_SUPPORTED,
    HTTP_CMD_INVALID
} HTTP_COMMAND;

/*
 * HTTP Connection Info - one for each connection.
 */
typedef struct _HTTP_INFO
{
    TCP_SOCKET socket;  //At initialization, every HTTP connection gets assigned a TCP socket.
    MPFS file;
    SM_HTTP smHTTP;
    BYTE smHTTPGet;
    WORD VarRef;
    union {
        struct
        {
            unsigned int bProcess : 1;
        } bits;
        BYTE val;
    } flags;

    HTTP_VAR httpVar;
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


static HTTP_INFO HCB[MAX_HTTP_CONNECTIONS];


/*
 * Static function prototypes.
 */
static void HTTPProcess(HTTP_HANDLE h);
static BOOL SendFile(HTTP_INFO* ph);
static HTTP_COMMAND HTTPGetCommand(TCP_SOCKET s);
static BYTE HTTPGetRqstRes(TCP_SOCKET s, BYTE* rqstRes, BYTE* type);


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

#if (DEBUG_HTTP > LOG_OFF)
static ROM char DebugMsgSendHdr[] = "\rH:Snd Hdr";
static ROM char DebugMsgSendFile[] = "\rH:Snd F:";
static ROM char DebugMsgSendFileSuccess[] = "\rH:F Snt OK";
static ROM char DebugMsgSendFile2Continue[] = "\rH:F Snd 2be cont";
static ROM char DebugMsgGetCmd[] = "\rH:Get";
static ROM char DebugMsgGet1[] = "\rH:Get ";
static ROM char DebugMsgGet2[] = ", sndg via s ";
static ROM char DebugMsgPostCmd[] = "\rH:Post";
static ROM char DebugMsgUknCmd[] = "\rH:Unknown";
static ROM char DebugMsgEndline[] = "\r";
static ROM char DebugMsgExecGetCmdCalled[] = "\rH:HTTPExecGetCmd Called";
#endif


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
 *  This function is called when we (HTTP Server) receive a message from a HTTP Client. We than
 *  parse the clients request, and send back a reply. This HTTP server currently parses the
 *  following HTTP requests, with the following formats:
 *  - GET: A complete HTTP 1.0 message begins "GET url HTTP/1.0". The addition of the third field
 *         indicates that full headers are being used. The client may then send additional header
 *         fields, one per line, terminating the message with a blank link. The server replies in
 *         a similar vein, first with a series of header lines, then a blank line, then the
 *         document proper. The format is:
 *         > GET / HTTP/1.0
 *         >
 *         < HTTP/1.0 200 OK
 *         < [Additional header fields]: [header field data]
 *         < Content-type: text/html
 *         < Content-length: 1579
 *         <
 *         < HTML document
 *
 *         The first line of a server header includes a response code indicating the success or
 *         failure of the operation. One of the server header fields will be Content-type:, which
 *         specifies a MIME type to describe how the document should be interpreted.
 *  - POST: A complete HTTP 1.0 message begins "POST url HTTP/1.0".  After sending the header and
 *         the blank line, the client transmits the data. The header must have included a
 *         Content-Length: field, which permits the server to determine when all the data has
 *         been received.
 *         > POST ???
 *         >
 *         < [Additional header fields]: [header field data]
 *         < Content-type: text/html
 *         < Content-length: 1579
 *         <
 *         < HTML document
 *
 *         The first line of a server header includes a response code indicating the success or
 *         failure of the operation. One of the server header fields will be Content-type:, which
 *         specifies a MIME type to describe how the document should be interpreted.
 *
 * Note:            None.
 ********************************************************************/
static void HTTPProcess(HTTP_HANDLE h)
{
    BYTE rqstRes[HTTP_MAX_RESOURCE_NAME_LEN + 1];
    HTTP_COMMAND httpCommand;
    BYTE indexRes;
    BOOL lbContinue;
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
            //We are receiving something via HTTP
            if ( TCPIsGetReady(ph->socket) )
            {
                lbContinue = TRUE;

                ph->flags.val = 0;  //Reset all flags
                
                //Read HTTP command from current socket. Is all text on first line till first space
                httpCommand = HTTPGetCommand(ph->socket);

                /////////////////////////////////////////////////
                // GET Command
                if (httpCommand == HTTP_CMD_GET)
                {
                    #if (DEBUG_HTTP >= LOG_INFO)
                    debugPutRomString(DebugMsgGetCmd);
                    #endif

                    /*
                     * Get requested resource. Is all text following command till first space or '?'.
                     * If there is any name-value parameters included in URI, this function will
                     * return true. If there is, this must be a remote command. Execute it and then
                     * send the requested resource (file). The file name may be modified by command
                     * handler. HTTPGetParam is used to get all name-value parameters.
                     */
                    if (HTTPGetRqstRes(ph->socket, rqstRes, &ph->fileType)) {
                        #if (DEBUG_HTTP >= LOG_DEBUG)
                        debugPutRomString(DebugMsgExecGetCmdCalled);
                        #endif

                        /*
                         * Let main application handle this remote command. It can modify the
                         * requested resource to actual file that will be sent as a result of
                         * this remote command.
                         */
                        HTTPExecGetCmd(ph->socket, rqstRes);

                        /*
                         * Assume that Web author will only use CGI or HTML
                         * file for remote command.
                         */
                        ph->fileType = HTTP_FILETYPE_CGI;
                    }

                    TCPDiscard(ph->socket); //We are finished with this socket - discard it

                    ph->smHTTP = SM_HTTP_NOT_FOUND;

                    ph->file = MPFSOpen(rqstRes);   //Open the requested resource
                    if ( ph->file == MPFS_INVALID )
                    {
                        ph->httpVar.val = HTTP_NOT_FOUND;     //Use Variable val for message code
                        ph->smHTTP = SM_HTTP_NOT_FOUND;
                    }
                    else if ( ph->file == MPFS_NOT_AVAILABLE )
                    {
                        ph->httpVar.val = HTTP_NOT_AVAILABLE; //Use Variable variable for message code
                        ph->smHTTP = SM_HTTP_NOT_FOUND;
                    }
                    else
                    {
                        //Initiate sending of a HTTP packet - GET responce
                        ph->smHTTP = SM_HTTP_HEADER;

                        #if (DEBUG_HTTP >= LOG_DEBUG)
                        debugPutRomString(DebugMsgGet1);
                        debugPutString(rqstRes);
                        debugPutRomString(DebugMsgGet2);
                        debugPutByte(ph->socket + '0');
                        #endif
                    }
                }
                /////////////////////////////////////////////////
                // POST Command
                else if (httpCommand == HTTP_CMD_POST)
                {
                    #if (DEBUG_HTTP >= LOG_INFO)
                    debugPutRomString(DebugMsgPostCmd);
                    #endif
                }
                /////////////////////////////////////////////////
                // Unknown Command
                else
                {
                    #if (DEBUG_HTTP >= LOG_WARN)
                    debugPutRomString(DebugMsgUknCmd);
                    #endif
                }
            }
            break;

        case SM_HTTP_NOT_FOUND:
            if ( TCPIsPutReady(ph->socket) )
            {
                romString = HTTPMessages[ph->httpVar.val];    //Use Variable variable for message code

                while( (i = *romString++) )
                    TCPPut(ph->socket, i);

                TCPFlush(ph->socket);
                ph->smHTTP = SM_HTTP_DISCONNECT;
            }
            break;
        //Send HTTP Header. The header consist of header fields, each terminated by a blank line.
        case SM_HTTP_HEADER:
            if ( TCPIsPutReady(ph->socket) )
            {
                lbContinue = TRUE;

                #if (DEBUG_HTTP >= LOG_DEBUG)
                debugPutRomString(DebugMsgSendHdr);
                #endif

                //Send:
                // - Initial responce line = "HTTP/1.0 200 OK"
                // - Content-Type header line = "Content-type: xxxx" with current content type
                for ( i = 0; i < HTTP_OK_STRING_LEN; i++ )
                    TCPPut(ph->socket, HTTP_OK_STRING[i]);

                romString = httpContents[ph->fileType].typeString;
                while( (i = *romString++) )
                    TCPPut(ph->socket, i);

                //Send end of header string = a blank line
                for ( i = 0; i < HTTP_HEADER_END_STRING_LEN; i++ )
                    TCPPut(ph->socket, HTTP_HEADER_END_STRING[i]);

                //Dynimic file or not. CGI file are dynamic, and processes for CGI characters
                if ( ph->fileType == HTTP_DYNAMIC_FILE_TYPE )
                    ph->flags.bits.bProcess = TRUE;
                else
                    ph->flags.bits.bProcess = FALSE;

                //We currently only support get command - set state machine to execute GET command
                ph->smHTTPGet = SM_HTTP_GET_READ;
                ph->smHTTP = SM_HTTP_GET;
            }
            break;
        //Send HTTP GET command's message body - requested files conents
        case SM_HTTP_GET:
            if ( TCPIsGetReady(ph->socket) )
                TCPDiscard(ph->socket);

            #if (DEBUG_HTTP >= LOG_DEBUG)
            debugPutRomString(DebugMsgSendFile);
            #endif

            //Send the requested file. Returns TRUE if finished sending file, or false if only partial
            //file has been send - will continue next time HTTPProcess is entered
            if ( SendFile(ph) )
            {
                MPFSClose();
                ph->smHTTP = SM_HTTP_DISCONNECT;

                #if (DEBUG_HTTP >= LOG_DEBUG)
                debugPutRomString(DebugMsgSendFileSuccess);
                debugPutRomString(DebugMsgEndline);
                #endif
            }
            #if (DEBUG_HTTP >= LOG_DEBUG)
            else {
                debugPutRomString(DebugMsgSendFile2Continue);
            }
            #endif

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
 * Function:        BYTE HTTPGetParam(TCP_SOCKET s, BYTE* param, BYTE* paramLen)
 *
 * PreCondition:    HTTPGetRqstRes() must have already be called.
 *
 * Input:           s   -   TCP Socket from which to read the next name-value parameter (if any)
 *                  param -  Will hold name and value strings when returning
 *                  paramLen - On entry into routine holds lengh of param buffer.
 *                             Will hold Index of value string on return, or 0 if there were none.
 *
 * Output:          1 if there are parameters to follow, else 0
 *
 * Side Effects:    None
 *
 * Overview:        Writes the name and value string to given param buffer. Both strings are
 *                  NULL terminated. On return name ane value string can be accessed as follows:
 *                  - Pointer to Name parameter = &param[0]
 *                  - Pointer to Value parameter = &param[paramLen]
 *
 * Note:            
 ********************************************************************/
BYTE HTTPGetParam(TCP_SOCKET s, BYTE* param, BYTE* paramLen)
{
    BYTE c;
    BYTE index;
    BYTE indexValue;
    BYTE length;

    //- When entering this function, s will point to first character of name field
    //- When leaving this function, s will point to first character after value field (or
    //  & character of value field if more parameters are to follow)

    length = *paramLen;
    index = 0;
    indexValue = 0;

    //Read requested resource name
    while (TCPGet(s, &c))
    {
        //Name field is finished
        if (c == '=') {
            c = '\0';
            *paramLen = index + 1;  //Use paramLen as output parameter
        }
        //Value field is finished
        else if ((c == '&') || (c == ' ') || (c == '\r') || (c == '\n')) {
            *param = '\0';
            break;
        }

        //Recover space characters.
        if (c == '+')
            c = ' ';

        //Write received char to buffer - keep last location free for NULL terminating char
        if ((index + 1) < length)
            *param = c;

        param++;
        index++;
    };

    //Indicate if there are parameters to follow or not
    return (c == '&' ? 1 : 0);
}

#if (DEBUG_HTTP > LOG_OFF)
static ROM char DebugMsgRqstres[] = "\rH:Rqstres=";
#endif

/*********************************************************************
 *
 * Input:           s   -   TCP Socket from which to read
 *                  rqstRes - Buffer to hold name of requested resource, for example "index.htm"
 *                  type - Pointer to hold type of file received.
 *                         Valid values are:
 *                            HTTP_FILETYPE_TXT
 *                            HTTP_FILETYPE_HTML
 *                            HTTP_FILETYPE_GIF
 *                            HTTP_FILETYPE_CGI
 *                            HTTP_FILETYPE_UNKNOWN
 *
 * Output:          0 if requested rousource has no parameters.
 *                  1 if requested rousource has parameters - use HTTPGetParam to get them.
 * Overview:        Parses the http command present on given socket
 *
 * Note:            None.
 ********************************************************************/
static BYTE HTTPGetRqstRes(TCP_SOCKET s, BYTE* rqstRes, BYTE* type)
{
    BYTE c;
    BYTE i;
    BYTE index;
    BYTE indexExt;  //Index of file extension = file type
    ROM char* fileType;

    *type = HTTP_FILETYPE_UNKNOWN;

    //Remove leading white space characters
    while(TCPGet(s, &c)) {
        if (c != ' ') {
            break;
        }
    };

    //Read requested resource name
    index = 0;
    indexExt = 0;
    do {
        //Recover space characters.
        if (c == '+')
            c = ' ';

        //Beginning of requested resource
        if (( c == '/') || (c == '\\')) {
            continue;   //Don't save '/' character
        }
        //File extension part of resource name is about to follow
        else if (c == '.') {
            indexExt = index + 1;
        }
        //Finished reading resource name
        else if ((c == ' ') || (c == '?') || (c == '\r') || (c == '\n')) {
            break;
        }

        rqstRes[index++] = c;
    } while (TCPGet(s, &c));

    //index now points to character after requested resource, c contains same character

    //If only '/' was requested, use default file
    if (index == 0)
    {
        memcpypgm2ram(rqstRes, (ROM void*)HTTP_DEFAULT_FILE_STRING, HTTP_DEFAULT_FILE_STRING_LEN);
        index = HTTP_DEFAULT_FILE_STRING_LEN;
        *type = HTTP_FILETYPE_HTML;
        indexExt = 0;   //Prevent looking for file type below
    }
    
    //Finished reading resource name
    rqstRes[index] = '\0';  //Null terminate resource name

    //Print requested resource
    #if (DEBUG_HTTP >= LOG_DEBUG)
    debugPutRomString(DebugMsgRqstres);
    debugPutString(rqstRes);
    #endif

    //Get filetype
    if (indexExt != 0)
    {
        //Convert file extension to upper case
        strupr((char*)&rqstRes[indexExt]);

        fileType = httpFiles[0].fileExt;    //Get pointer to first file type string
        for ( i = 0; i < TOTAL_FILE_TYPES; i++ )
        {
            if ( !memcmppgm2ram((void*)&rqstRes[indexExt], (ROM void*)fileType, FILE_EXT_LEN) )
            {
                *type = i;  //Store file type in type parameter - used by calling function to get type
                break;
            }
            fileType += sizeof(FILE_TYPES); //Increment to point to next file type string
        }
    }

    //Indicate if there parameters to follow or not
    return (c == '?' ? 1 : 0);
}


/*********************************************************************
 *
 * Input:       s   -   TCP Socket from which to read
 *
 * Output:      The http command found.
 * Overview:    Parses the http command present on given socket
 *
 * Note:        - When entering this function, s will point to first character of command name
 *              - When leaving this function, s will point to first character after the space
 *                trailing this command.
 ********************************************************************/
static HTTP_COMMAND HTTPGetCommand(TCP_SOCKET s)
{
    BYTE buf[5];
    BYTE index;

    //- When entering this function, s will point to first character of command name

    //Read command = all characters till first space
    index = 0;
    while((index < 4) && TCPGet(s, &buf[index]) ) {
        if (buf[index++] == ' ')
            break;
    };
    buf[index] = '\0';

    if ((buf[0] == 'G') && (buf[1] == 'E') && (buf[2] == 'T')) {
        return HTTP_CMD_GET;
    }
    else if ((buf[0] == 'P') && (buf[1] == 'O') && (buf[2] == 'S') && (buf[3] == 'T')) {
        return HTTP_CMD_POST;
    }

    //If no command found, return not supported
    return HTTP_CMD_NOT_SUPPORTED;
}

#if (DEBUG_HTTP > LOG_OFF)
static ROM char DebugMsgGettingVar[] = "\rH:Getting Var";
static ROM char DebugMsgSendingDynFile[] = "\rH:TXing Dynamic file";
#endif

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

    //#if (DEBUG_HTTP >= LOG_DEBUG)
    //#if defined(DEBUG_HTTP)
    //if ( ph->flags.bits.bProcess )
        //debugPutRomString(DebugMsgSendingDynFile);
    //#endif


    while( TCPIsPutReady(ph->socket) )
    {   
        lbTransmit = FALSE;

        //Get next character from file system
        if ( ph->smHTTPGet != SM_HTTP_GET_VAR )
        {
            c = MPFSGet();
            //debugPutByte(c);        //HTTP Debug - print contents of entire file!
            if ( MPFSIsEOF() )
            {
                MPFSGetEnd();
                TCPFlush(ph->socket);
                return TRUE;
            }
        }

        //Is this a Dynamic file type (CGI file). If so, process
        if ( ph->flags.bits.bProcess )
        {
            switch(ph->smHTTPGet)
            {
            case SM_HTTP_GET_READ:
                //If escape character found, get variable
                if ( c == HTTP_VAR_ESC_CHAR )
                    ph->smHTTPGet = SM_HTTP_GET_VAR_GRP;
                else
                    lbTransmit = TRUE;
                break;
            // - If second % char is received, transmit a % char
            // - If not, read in variable group
            case SM_HTTP_GET_VAR_GRP:
                if ( c == HTTP_VAR_ESC_CHAR )
                {
                    lbTransmit = TRUE;
                    ph->smHTTPGet = SM_HTTP_GET_READ;
                }
                else
                {
                    ph->httpVar.group = c;                  //Read MSB of HTTP variable = group
                    ph->smHTTPGet = SM_HTTP_GET_VAR_VALMSB; //Read LSB of variable next
                }
                break;
            //Read MSB of variable value. Value is hex coded
            case SM_HTTP_GET_VAR_VALMSB:
                if (c > 57) 
                c -= 7;
                ph->httpVar.val = (c - '0') << 4;

                ph->smHTTPGet = SM_HTTP_GET_VAR_VALLSB;
                break;
            //Read MSB of variable value. Value is hex coded
            case SM_HTTP_GET_VAR_VALLSB:
                if (c > 57) 
                c -= 7; 
                ph->httpVar.val |= (c - '0');

                ph->smHTTPGet = SM_HTTP_GET_VAR;
                ph->VarRef = HTTP_START_OF_VAR;
                break;
            case SM_HTTP_GET_VAR:
                //debugPutRomString(DebugMsgGettingVar);
                ph->VarRef = HTTPGetVar(ph->httpVar, ph->VarRef, &c);
                lbTransmit = TRUE;
                if ( ph->VarRef == HTTP_END_OF_VAR )
                {
                    //For strings, the trailing NULL character should not be transmitted
                    if (c == '\0')
                        lbTransmit = FALSE;
                        
                    ph->smHTTPGet = SM_HTTP_GET_READ;
                }
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

    //Get address of next byte that has to be read - so we can continue later on.
    ph->file = MPFSGetEnd();

    // We are not done sending a file yet...
    return FALSE;
}






