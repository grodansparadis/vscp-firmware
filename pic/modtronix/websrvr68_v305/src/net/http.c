/*********************************************************************
 *
 *               HTTP Implementation for Modtronix TCP/IP Stack
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
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Microchip Technology, Inc.
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
 * 2005-11-15, David Hosken (DH):
 *      - Fixed MPLAB C18 compiler bug that occured in complex if() statement
 *      - Fixed bug when checking for error during sending of file
 * 2005-12-21, David Hosken (DH):
 *      - Added support for "Modtronix Embedded Debugger" app.
 *      - Added function for determining file type.
 *      - Modified for new multi implementation file system
 *      - Added check to see if HTTP Socket is holding on to an open file if forcefully closed
 * 2002/07/09, Nilesh Rajbharti:    Rev 2.1 (Fixed HTTPParse bug)
 * 2002/05/22, Nilesh Rajbharti:    Rev 2.0 (See version.log for detail)
 * 2002/02/09, Nilesh Rajbharti:    Cleanup
 * 2001/09/12, Nilesh Rajbharti:    Released (Rev. 1.0)
 * 2001/08/14, Nilesh Rajbharti:    Original
********************************************************************/
#define THIS_IS_HTTP_SERVER

#include <string.h>

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\http.h"
#include "net\fsee.h"
#include "net\tcp.h"
#include "debug.h"

#if defined(STACK_USE_HTTP_SERVER)

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xAF, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xAF, msgCode, msgStr)

#if (MAX_HTTP_CONNECTIONS <= 14)
    #define debugPutOffsetMsg(offset, msgCode) debugPut2Bytes(0xA0 + offset, msgCode)
#else
    /* If there are more then 14 HTTP connections, then */
    #define debugPutOffsetMsg(offset, msgCode) \
        if (offset <= 14) {debugPut2Bytes(0xA0 + offset, msgCode);} \
        else {debugPut2Bytes(0xA0, offset); debugPutByte(msgCode);}
#endif

/////////////////////////////////////////////////
//Do some checks
#if (MAC_TX_BUFFER_SIZE <= 130 || MAC_TX_BUFFER_SIZE > 1500 )
#error HTTP : Invalid MAC_TX_BUFFER_SIZE value specified.
#endif


/////////////////////////////////////////////////
// Each dynamic variable within a CGI file should be preceeded with this character.
#define HTTP_VAR_ESC_CHAR       '%'


/////////////////////////////////////////////////
// HTTP File Types. The format must be:
// - zippable file types (first ones with 3 char file types, then ones with 2 char file types)
// - non zippable file types
// - zipped version of the zippable file types in the beginning of the list. Must be same order
//   as "zippable file types" in beginning of list!
#define HTTP_FILETYPE_TXT        (0)
#define HTTP_FILETYPE_HTML       (1)
#define HTTP_FILETYPE_CLA        (2)
#define HTTP_FILETYPE_CSS        (3)
#define HTTP_FILETYPE_JS         (4)
#define HTTP_FILETYPE_GIF        (5)
#define HTTP_FILETYPE_CGI        (6)
#define HTTP_FILETYPE_JPG        (7)
#define HTTP_FILETYPE_JAR        (8)
#define HTTP_FILETYPE_WAV        (9)
#define HTTP_FILETYPE_UNKNOWN    (10)
#define HTTP_FILETYPE_ZTX        (11)   //Zipped (Gzip) Text file
#define HTTP_FILETYPE_ZHT        (12)   //Zipped (Gzip) HTML file
#define HTTP_FILETYPE_ZCL        (13)   //Zipped (Gzip) Java class file
#define HTTP_FILETYPE_ZCS        (14)   //Zipped (Gzip) CSS file
#define HTTP_FILETYPE_ZJS        (15)   //Zipped (Gzip) JavaScript file

//Test if the given type is one of the zippable file types supported by the HTTP server
#define isZippableFiletype(type) (type <= HTTP_FILETYPE_JS)

//Check if the given type is a zipped file type
#define isZippedFiletype(type) (type >= HTTP_FILETYPE_ZTX)

//Check if the given type is zippable AND has a 3 character file extension
#define is3CharZippableFiletype(type) (type <= HTTP_FILETYPE_CSS)

//What is the offset from the normal file extension to the zipped file extension
#define OFFSET_ZIPPED_EXTENSION (HTTP_FILETYPE_ZTX - HTTP_FILETYPE_TXT)

#define FILE_EXT_LEN    (3)
typedef struct _FILE_TYPES
{
    char fileExt[FILE_EXT_LEN+1];
} FILE_TYPES;


/////////////////////////////////////////////////
// Each entry in this structure must be in UPPER case.
// Order of these entries must match with those defined by "HTTP File Types" defines.
// If entry is smaller then 3 characters, terminate with '\0' character
static ROM FILE_TYPES httpFiles[] =
{
    { "TXT" },          // HTTP_FILETYPE_TXT
    { "HTM" },          // HTTP_FILETYPE_HTML
    { "CLA" },          // HTTP_FILETYPE_CLA
    { "CSS" },          // HTTP_FILETYPE_CSS
    { "JS\0" },         // HTTP_FILETYPE_JS
    { "GIF" },          // HTTP_FILETYPE_GIF
    { "CGI" },          // HTTP_FILETYPE_CGI
    { "JPG" },          // HTTP_FILETYPE_JPG
    { "JAR" },          // HTTP_FILETYPE_JAR
    { "WAV" },          // HTTP_FILETYPE_WAV
    { "TXT" },          // HTTP_FILETYPE_UNKNOWN
    { "ZTX" },          // HTTP_FILETYPE_ZTX
    { "ZHT" },          // HTTP_FILETYPE_ZHT
    { "ZCL" },          // HTTP_FILETYPE_ZCL
    { "ZCS" },          // HTTP_FILETYPE_ZCS
    { "ZJS" }           // HTTP_FILETYPE_ZJS
};
#define TOTAL_FILE_TYPES        ( sizeof(httpFiles)/sizeof(httpFiles[0]) )


/////////////////////////////////////////////////
// MIME media types. See http://www.iana.org/assignments/media-types/index.html for list
// Content entry order must match with those "HTTP File Types" define's.
static ROM char * httpContents[] = {
    "text/plain",               /* HTTP_FILETYPE_TXT */
    "text/html",                /* HTTP_FILETYPE_HTML */
    "application/java-vm",      /* HTTP_FILETYPE_CLA */
    "text/css",                 /* HTTP_FILETYPE_CSS */
    "text/javascript",          /* HTTP_FILETYPE_JS */
    "image/gif",                /* HTTP_FILETYPE_GIF */
    "text/html",                /* HTTP_FILETYPE_CGI */
    "image/jpeg",               /* HTTP_FILETYPE_JPG */
    "application/java-archive", /* HTTP_FILETYPE_JAR */
    "audio/x-wave",             /* HTTP_FILETYPE_WAV */
    "text/plain",               /* HTTP_FILETYPE_UNKNOWN */
    "text/plain",               /* HTTP_FILETYPE_ZTX */
    "text/html",                /* HTTP_FILETYPE_ZHT */
    "application/java-vm",      /* HTTP_FILETYPE_ZCS */
    "text/css",                 /* HTTP_FILETYPE_ZCS */
    "text/javascript"           /* HTTP_FILETYPE_ZJS */
};


/////////////////////////////////////////////////
// Supported HTTP Commands
typedef enum _HTTP_COMMAND
{
    HTTP_CMD_GET,
    HTTP_CMD_POST,
    HTTP_CMD_NOT_SUPPORTED,
    HTTP_CMD_INVALID
} HTTP_COMMAND;


typedef BYTE HTTP_HANDLE;


typedef enum
{
    HTTP_NOT_FOUND,
    HTTP_OK,
    HTTP_HEADER_END,
    HTTP_NOT_AVAILABLE
} HTTP_MESSAGES;


/////////////////////////////////////////////////
// Following message order must match with that of HTTP_MESSAGES enum.
static ROM char * HTTPMessages[] =
{
    "HTTP/1.0 404 Not found\r\n\r\nNot found.\r\n",
    "HTTP/1.0 200 OK\r\n\Content-type: ",
    "\r\n\r\n",
    "HTTP/1.0 503 \r\n\r\nService Unavailable\r\n"
};


/////////////////////////////////////////////////
// Standard HTTP messages.
ROM char HTTPMSG_RESPONSE_OK[]              = "HTTP/1.0 200 OK";
ROM char HTTPMSG_CONTENT_TYPE[]             = "Content-type: ";
ROM char HTTPMSG_CONTENT_ENCODING_GZIP[]    = "Content-Encoding: gzip\r\n";
ROM char HTTPMSG_EXPIRES_IMMEDIATELY[]      = "Expires: 0\r\n";
//Indicates to any client that the document will expire in 300 seconds. Can NOT use the cach contents after 60 seconds
ROM char HTTPMSG_CACHECONTROL_MAXAGE300[]    = "Cache-Control: max-age=300, must-revalidate\r\n";
//ROM char HTTP_END_STRING[] ="\r\n";
                        

/////////////////////////////////////////////////
// HTTP Command Strings
ROM BYTE HTTP_GET_STRING[]  = "GET";

/////////////////////////////////////////////////
// Default HTML file.
ROM BYTE HTTP_DEFAULT_FILE_STRING[]  = "INDEX.HTM";

/////////////////////////////////////////////////
// Default Login page
ROM char PAGE_LOGIN[] = "LOGIN.CGI";


/////////////////////////////////////////////////
// Static variables
static HTTP_INFO HCB[MAX_HTTP_CONNECTIONS];
static TICK16 lastActivity;     //Stores the value that the HTTP port was last accessed
HTTP_USER httpUser;


/////////////////////////////////////////////////
// Static function prototypes.
static void HTTPProcess(HTTP_HANDLE h);
static HTTP_COMMAND HTTPGetCommand(TCP_SOCKET s);
BYTE HTTPGetRqstRes(TCP_SOCKET s, BYTE* rqstRes);
BYTE HTTPGetRqstResFiletype(BYTE* rqstRes);
static BOOL sendFile(HTTP_INFO* ph);
static void sendLineEnd(HTTP_HANDLE h);
static void sendRomStr(HTTP_HANDLE h, ROM char* s);


/**
 * Set all HTTP connections to Listening state. Initialize FSM for each connection.
 * This function is called only onece during lifetime of the application. <br>
 * Important!! This function modifies HTTP FSM and connections are initialized.
 *
 * @preCondition TCP must already be initialized.
 */
void HTTPInit(void)
{
    BYTE i;
    
    // TEST TEST
    //Future releases of this code will NOT use this method anymore. Will use HTTP Authentication!
    httpUser.user = USER_GUEST;    //Initialze with GUEST
    lastActivity = TickGet16bit();

    //Initialize all HTTP connections:
    // - Allocate each one a TCP sockets to listen on HTTP port
    // - Set file to FILE_INVALID. This indicates that this HTTP connection does NOT currently have an open file
    for ( i = 0; i <  MAX_HTTP_CONNECTIONS; i++ )
    {
        HCB[i].socket = TCPListen(HTTP_PORT);
        HCB[i].smHTTP = SM_HTTP_IDLE;
        HCB[i].flags.val = 0;   //Clear all flags
    }
}


/**
 * Itterate through all connections and let it handle its connection. If a connection
 * is not finished, do not process next connections. This must be done, all
 * connections use some static variables that are common.
 * This function acts as a task (similar to one in RTOS). This function performs its task in
 * co-operative manner. Main application must call this function repeatdly to ensure all open
 * or new connections are served on time.
 *
 * @preCondition    HTTPInit() must already be called.
 */
void HTTPServer(void)
{
    BYTE conn;

    // TEST TEST
    //If no activity for 2 minutes, log out
    //Future versions of this code will check the HTTP header for HTTP Authorization, and NOT
    //use this method anymore!
    if (TickGetDiff16bit(lastActivity) >= ((TICK16)TICKS_PER_SECOND * (TICK16)120) ) {
        httpUser.user = USER_GUEST;
    }

    //Process each connection
    for ( conn = 0;  conn < MAX_HTTP_CONNECTIONS; conn++ ) {
        HTTPProcess(conn);
    }
}


/**
 * Gets the file type of the given filename.
 * @param[in] rqstRes   Buffer containing name of requested resource, for example "index.htm".
 *                      The name is all uppercase characters!
 * @return              Returns the file type. Valid values are HTTP_FILETYPE_XXX values
 *                      Examples are: <br>
 *                      HTTP_FILETYPE_TXT       <br>
 *                      HTTP_FILETYPE_HTML      <br>
 *                      HTTP_FILETYPE_GIF       <br>
 *                      HTTP_FILETYPE_CGI       <br>
 *                      HTTP_FILETYPE_UNKNOWN
 */
BYTE HTTPGetRqstResFiletype(BYTE* rqstRes) {
    BYTE *fileExt;      //Pointer to file extension
    BYTE i;
    
    fileExt = rqstRes;

    //Get index of file extension
    while (*fileExt++ != '.') {
        //If end of string reached, it means there is no file extension
        if (*fileExt == '\0')
            return HTTP_FILETYPE_UNKNOWN;
    }
    
    //Get filetype
    for ( i = 0; i < TOTAL_FILE_TYPES; i++ )
    {
        //Compare two strings
        if (strcmppgm2ram((char *)fileExt, (ROM char *)httpFiles[i].fileExt) == 0) {
            return i;   //Found! Return file type
        }
    }

    
    return HTTP_FILETYPE_UNKNOWN;
}

/**
 * Parses the http command present on given socket
 * @param[in] s         TCP Socket from which to read
 * @param[out] rqstRes  Buffer to hold name of requested resource, for example "index.htm".
 *                      The resource name is converted to all uppercase characters!
 *
 * @return      0 if requested rousource has no parameters.
 *              1 if requested rousource has parameters - use HTTPGetParam to get them.
 */
BYTE HTTPGetRqstRes(TCP_SOCKET s, BYTE* rqstRes)
{
    BYTE c;
    BYTE index;

    //Remove leading white space characters
    while(TCPGet(s, &c)) {
        if (c != ' ') {
            break;
        }
    };

    //Read requested resource name
    index = 0;
    do {
        //Recover space characters.
        if (c == '+')
            c = ' ';

        //Beginning of requested resource
        if (( c == '/') || (c == '\\')) {
            continue;   //Don't save '/' character
        }
        //Finished reading resource name
        else if ((c == ' ') || (c == '?') || (c == '\r') || (c == '\n')) {
            break;
        }

        //Don't overwrite buffer
        if (index < (HTTP_MAX_RESOURCE_NAME_LEN)) {
            //Convert lowercase letter to uppercase
            if((c >= 'a') && (c <= 'z')) {
                c -= ('a' - 'A');   //Convert to upper case
            }
            rqstRes[index++] = c;
        }
    } while (TCPGet(s, &c));

    //index now points to character after requested resource, c contains same character

    //Finished reading resource name
    rqstRes[index] = '\0';  //Null terminate resource name

    //If only '/' was requested, use default file
    if (index == 0)
    {
        strcpypgm2ram((char *)rqstRes, (ROM char *)HTTP_DEFAULT_FILE_STRING);    //Copy string, including NULL
    }

    //Indicate if there parameters to follow or not
    return (c == '?' ? 1 : 0);
}


/**
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
 *  - POST: We can receive two types of POST messages.
 *         - The one type contains all form data after a blank like. Before the blank line the 
 *           "Content-type:" and "Content-length:" headers will give the type and length of the data to
 *           follow after the blank line.
 *         - The other type contains multiple blocks of form data. It is indicated by a
 *           "Content-Type: multipart/form-data;" header field followed by a blank line. All subsequent blank
 *           lines will be followed by data for a single control found on the HTTP form.
 *           Each block of form data is given after a blank line that follows a "Content-Disposition:" header.
 *     - POST: A complete HTTP 1.0 message begins "POST url HTTP/1.0".  After sending the header, multiple
 *         header fields and the blank line, the client transmits the data. The header must have included a
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
 *     - POST Multipart: A complete HTTP 1.0 message begins "POST url HTTP/1.0".  After sending the header and
 *         multiple header fields, a "Content-Type: multipart/form-data;" header field followed by a blank
 *         line will indicate that multiple data blocks are to follow. Each of these data blocks contains some
 *         header fields, a blanks line and the data.
 *         > POST ???
 *         >
 *         < [Additional header fields]: [header field data]
 *         < Content-Type: multipart/form-data; boundary=AaB03x
 *         < 
 *         < --AaB03x
 *         < Content-Disposition: form-data; name="myControl"
 *         < 
 *         < Contents of myControl form control
 *         < --AaB03x
 *         < Content-Disposition: form-data; name="fsimage"; filename="default.img"
 *         < Content-Type: application/octet-stream
 *         < 
 *         < ... contents of fsimage file control = contents of default.img file ...
 *         < --AaB03x--
 *
 * @preCondition    HTTPInit() called.
 *
 * @param h         Index to the handle which needs to be processed
 *
 * @return          Connection referred by 'h' is served.
 */
static void HTTPProcess(HTTP_HANDLE h)
{
    BYTE rqstRes[HTTP_MAX_RESOURCE_NAME_LEN + 1];

    HTTP_COMMAND httpCommand;
    BOOL lbContinue;
    BYTE i; //Temp variable
    HTTP_INFO* ph;
    ROM char* romString;

    ph = &HCB[h];

    lbContinue = TRUE;
    while( lbContinue )
    {
        lbContinue = FALSE;

        //If during handling of HTTP socket, it gets disconnected,
        //forget about previous processing and return to idle state.
        if ( !TCPIsConnected(ph->socket) )
        {
            //Check if this HTTP socket still has an open file
            if ( ph->flags.bits.bOwnsFile ) {
                ph->flags.bits.bOwnsFile = FALSE;
                fileClose(ph->file);
            }
            
            //Indicate that no user is logged in
            ph->flags.bits.bUserLoggedIn = FALSE;

            //Return to idle state
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
                lastActivity = TickGet16bit();

                ph->flags.val = 0;  //Reset all flags
                
                //Read HTTP command from current socket. Is all text on first line till first space
                httpCommand = HTTPGetCommand(ph->socket);
                
                // TEST TEST
                //Determine current user that is logged in. Future versions of this code will check
                //the HTTP header here for HTTP Authorization code! The httpUser structure will NOT
                //be used in future releases of this code!
                ph->flags.bits.bUserLoggedIn = FALSE;
                if (httpUser.ipAdr.Val == TCPGetSocketInfo(ph->socket)->remote.IPAddr.Val) {
                    if (httpUser.user != USER_GUEST) {
                        ph->flags.bits.bUserLoggedIn = TRUE;
                    }
                    ph->flags.bits.bUserSuper = (httpUser.user == USER_SUPER);
                }

                /////////////////////////////////////////////////
                // Received a GET Command
                if (httpCommand == HTTP_CMD_GET)
                {
                    //At this stage ph->socket will point to first character after the space trailing the
                    //GET command in the HTTP Header
                    
                    BYTE parametersToFollow;
                    /*
                     * Get NULL terminated requested resource in "rqstRes" variable
                     * The requested resource is all text following command till first space or '?'.
                     * If there is any name-value parameters included in URI, this function will
                     * return true. If there is, this must be a remote command. Execute it and then
                     * send the requested resource (file). The file name may be modified by command
                     * handler. HTTPGetParam is used to get all name-value parameters.
                     * The returned requested resouce (in rqstRes) is all uppercase!
                     */
                    parametersToFollow = HTTPGetRqstRes(ph->socket, rqstRes);

                    #if (DEBUG_HTTP >= LOG_DEBUG)
                    debugPutOffsetMsg(h, 19);   //HTTPGetRqstRes() got %s
                    debugPutString(rqstRes);
                    #endif

                    //If filename begins with 'X' character, we need to be logged in to access this page!
                    if (rqstRes[0] == 'X') {
                        if (ph->flags.bits.bUserLoggedIn == FALSE) {
                            memcpypgm2ram((void*)rqstRes, (ROM void*)PAGE_LOGIN, sizeof(PAGE_LOGIN));
                        }
                    }

                    //Get filetype of requested resource
                    ph->var.get.fileType = HTTPGetRqstResFiletype(rqstRes);
                    
                    //The URL contains name-value parameters!
                    if (parametersToFollow) {
                        #if (DEBUG_HTTP >= LOG_DEBUG)
                        debugPutOffsetMsg(h, 10);   //Calling HTTPExecGetCmd() for GET parameters
                        #endif

                        /*
                         * Let main application handle this remote command. It can modify the
                         * requested resource to actual file that will be sent as a result of
                         * this remote command.
                         */
                        HTTPExecGetCmd(ph, rqstRes);
                    }
                    
                    #if (DEBUG_HTTP >= LOG_INFO)
                    debugPutOffsetMsg(h, 01);   //Received GET command for %s
                    debugPutString(rqstRes);
                    #endif
                    
                    #if (DEBUG_HTTP >= LOG_DEBUG)
                    debugPutOffsetMsg(h, 11);   //Filetype of requested resource is %d
                    debugPutByte(ph->var.get.fileType);
                    #endif

                    TCPDiscard(ph->socket); //We are finished with this socket - discard it

                    ph->smHTTP = SM_HTTP_NOT_FOUND;

                    //Try and open the requested resource. If not found, check if it is a zippable file. If it
                    //is, rename it to it's zipped filename, and try open that. For example, if "file.htm" was
                    //not found, try and open "file.zht" 
                    ph->file = fileOpen(rqstRes, 0);
                    if ( !fileIsValidHandle(ph->file)) {
                        //If this is a zipped file type, we do the following:
                        if (isZippableFiletype(ph->var.get.fileType))
                        {
                            i = strlen((char *)rqstRes);
                            
                            //Zippable file's extension is 3 characters long
                            if (is3CharZippableFiletype(ph->var.get.fileType)) {
                                i -= 3; //Get index of first character of file extension
                            }
                            //Zippable file's extension is 2 characters long
                            else {
                                i -= 2; //Get index of first character of file extension
                            }
                        
                            //Update fileType to zipped version. For example, if type was HTM, it is now ZHT
                            ph->var.get.fileType = ph->var.get.fileType + OFFSET_ZIPPED_EXTENSION;
                            
                            //Rename the file extension to what it will be for the zipped file. For example, if
                            //this file is *.htm, rename to *.zht
                            strcpypgm2ram((char *)&rqstRes[i], (ROM char*)httpFiles[ph->var.get.fileType].fileExt);
                                
                            #if (DEBUG_HTTP >= LOG_DEBUG)
                            debugPutOffsetMsg(h, 15);   //Zipped file name = %s
                            debugPutString(rqstRes);
                            #endif
                        
                            //Try and open zippable version of this file
                            ph->file = fileOpen(rqstRes, 0);
                        }
                    }
                    
                    if ( ph->file == FILE_INVALID )
                    {
                        ph->var.gen.msgCode = HTTP_NOT_FOUND;   //Message code
                        ph->smHTTP = SM_HTTP_NOT_FOUND;
                    }
                    else if ( ph->file == FSYS_NOT_AVAILABLE )
                    {
                        ph->var.gen.msgCode = HTTP_NOT_AVAILABLE; //Message code
                        ph->smHTTP = SM_HTTP_NOT_FOUND;
                    }
                    else
                    {
                        ph->flags.bits.bOwnsFile = TRUE;    //Indicate that this HTTP Connection has an open file.
                        
                        //Initiate sending of a HTTP packet - GET responce
                        ph->smHTTP = SM_HTTP_GET_TX_HDR;

                        #if (DEBUG_HTTP >= LOG_INFO)
                        debugPutOffsetMsg(h, 02);   //Found requested file
                        #endif
                    }
                }
                /////////////////////////////////////////////////
                // Received a POST Command
                else if (httpCommand == HTTP_CMD_POST)
                {
                    //At this stage ph->socket will point to first character after the space trailing the
                    //POST command in the HTTP Header

                    //We will be receiving the POST header next
                    ph->smHTTP = SM_HTTP_POST_RX_HDR;

                    #if (DEBUG_HTTP >= LOG_INFO)
                    debugPutOffsetMsg(h, 03);   //Received POST command
                    #endif
                }
                /////////////////////////////////////////////////
                // Unknown Command
                else
                {
                    #if (DEBUG_HTTP >= LOG_WARN)
                    debugPutOffsetMsg(h, 12);   //Received unknown HTTP command
                    #endif
                }
            }
            break;
            
        //Send HTTP Header responce for GET message received from client. The header consist of header fields, each terminated by a blank line.
        case SM_HTTP_GET_TX_HDR:
            if ( TCPIsPutReady(ph->socket) )
            {
                lbContinue = TRUE;

                #if (DEBUG_HTTP >= LOG_INFO)
                debugPutOffsetMsg(h, 13);   //Sending GET Header
                #endif

                /////////////////////////////////////////////////
                //Send HTTP Responce message
                
                //Send HTTP Status header line = "HTTP/1.0 200 OK"
                sendRomStr(h, HTTPMSG_RESPONSE_OK);
                sendLineEnd(h);     //Terminate with "CRLF" characters
                
                //Send "Content-Type: " header line = "Content-type: xxxx" with current content type
                sendRomStr(h, HTTPMSG_CONTENT_TYPE);
                sendRomStr(h, httpContents[ph->var.get.fileType]);   //Send current content type string
                sendLineEnd(h);     //Terminate with "CRLF" characters
                
                //Send "Content-Encoding: gzip" if the file type is a compressed HTTP file
                if (isZippedFiletype(ph->var.get.fileType))
                {
                    sendRomStr(h, HTTPMSG_CONTENT_ENCODING_GZIP);
                    //sendLineEnd(h);     //"CRLF" characters is included in HTTPMSG_CONTENT_ENCODING_GZIP string
                }
                
                //Send the "Expires: 0\r\n" header line. Prevents anything from being cached by client
                //sendRomStr(h, HTTPMSG_EXPIRES_IMMEDIATELY);
                
                //Set cach of files to:
                // - JavaScript file to be reloaded after 10 minutes
                // - All other files are reloaded after 5 minutes
                if ((ph->var.get.fileType == HTTP_FILETYPE_JS) || (ph->var.get.fileType == HTTP_FILETYPE_ZJS)) {
                    sendRomStr(h, (ROM char*)"Cache-Control: max-age=600, must-revalidate\r\n");
                }
                else {
                    sendRomStr(h, (ROM char*)"Cache-Control: max-age=300, must-revalidate\r\n");
                }
                
                //Send end of header string = a blank line
                sendLineEnd(h);     //Terminate with "CRLF" characters


                /////////////////////////////////////////////////
                //Dynamic file or not. Check if the current file type is a dynamic file, and if it
                //is, set bit so it will be parsed. All %nnn tags in a dynamic file are replaced by CGI server.
                if (   (ph->var.get.fileType == HTTP_FILETYPE_CGI)
                #if defined(HTTP_PARSE_FILETYPE_HTML)
                    || (ph->var.get.fileType == HTTP_FILETYPE_HTML)
                #endif
                #if defined(HTTP_PARSE_FILETYPE_JS)
                    || (ph->var.get.fileType == HTTP_FILETYPE_JS)
                #endif
                )
                {
                    ph->flags.bits.bProcess = TRUE;
                }
                else {
                    ph->flags.bits.bProcess = FALSE;
                }

                //We currently only support get command - set state machine to execute GET command
                ph->smHTTPSub = SM_HTTP_GET_READ;
                ph->smHTTP = SM_HTTP_GET_TX_BODY;
            }
            break;
        //Send HTTP body responce for GET message received from client = requested files conents
        case SM_HTTP_GET_TX_BODY:
            if ( TCPIsGetReady(ph->socket) )
                TCPDiscard(ph->socket);

            #if (DEBUG_HTTP >= LOG_INFO)
            debugPutOffsetMsg(h, 14);   //Sending GET command body
            #endif

            //Send the requested file. Returns TRUE if finished sending file, or false if only partial
            //file has been send - will continue next time HTTPProcess is entered
            if ( sendFile(ph) )
            {
                ph->flags.bits.bOwnsFile = FALSE;    //Indicate that this HTTP Connection does NOT own a file.
                
                fileClose(ph->file);
                ph->smHTTP = SM_HTTP_DISCONNECT;

                #if (DEBUG_HTTP >= LOG_INFO)
                debugPutOffsetMsg(h, 05);   //Successfully sent file
                #endif
            }
            #if (DEBUG_HTTP >= LOG_INFO)
            else {
                debugPutOffsetMsg(h, 06);   //Sending of file to be continued
            }
            #endif
            break;

        //Parse the POST header received from client
        case SM_HTTP_POST_RX_HDR:
            //At this stage ph->socket will point to first character after the space trailing the
            //POST command in the HTTP Header
            
            //The POST header will consist out of multiple lines of header 

            #if (DEBUG_HTTP >= LOG_INFO)
            debugPutOffsetMsg(h, 18);   //Receiving POST Header
            #endif
            
            // TODO!

            //Check "Content-Type" header field, it can be:
            // - "text/plain" Plain text file is to follow
            // - "image/gif" GIF file is to follow
            // - "application/octet-stream" Undefined binary file is to follow 

            //Check "Content-Disposition" header field gives info about the form data to follow, examples are:
            // - "Content-Disposition: form-data; name="myinput"
            //    This example will send the data entered in the following input control <INPUT type="text" name="myinput">
            //
            // - "Content-Disposition: form-data; name="fsimage"; filename="default.img"
            //   "Content-Type: application/octet-stream"
            //   This example will send the file data of input control <INPUT type="file" name="fsimage">

            //After receiving whole header, we now get ready to receive body
            //ph->smHTTP = SM_HTTP_POST_RX_HDR;

            break;
            
        //Parse the POST body received from client
        case SM_HTTP_POST_RX_BODY:

            #if (DEBUG_HTTP >= LOG_INFO)
            debugPutOffsetMsg(h, 19);   //Receiving POST Body
            #endif
            
            // TODO!

            break;

        case SM_HTTP_NOT_FOUND:
            if ( TCPIsPutReady(ph->socket) )
            {
                romString = HTTPMessages[ph->var.gen.msgCode];    //Message code

                while( (i = *romString++) )
                    TCPPut(ph->socket, i);

                TCPFlush(ph->socket);
                ph->smHTTP = SM_HTTP_DISCONNECT;
            }
            break;

        case SM_HTTP_DISCONNECT:
            if ( TCPIsConnected(ph->socket) )
            {
                //Check if the given socket is ready for transmission. If not, we will return here later again and do
                //the same check!
                if ( TCPIsPutReady(ph->socket) )
                {
                    #if (DEBUG_HTTP >= LOG_INFO)
                    debugPutOffsetMsg(h, 07);   //HTTP Disconnected
                    #endif

                    TCPDisconnect(ph->socket);

                    //Switch to not-handled state.  This FSM has one common action that checks for
                    //disconnect condition and returns to Idle state.
                    ph->smHTTP = SM_HTTP_DISCONNECT_WAIT;
                }
            }
            
            //Indicate that no user is logged in
            ph->flags.bits.bUserLoggedIn = FALSE;

            break;
            
        //Do nothing. This FSM has one common action that checks for disconnect condition and returns to Idle state.
        case SM_HTTP_DISCONNECT_WAIT:
            break;
        }
    }
}

/**
 * Writes the name and value string to given param buffer. Both strings are
 * NULL terminated. On return name ane value string can be accessed as follows: <br>
 * - Pointer to Name parameter = &param[0] <br>
 * - Pointer to Value parameter = &param[paramLen]
 * @preCondition    HTTPGetRqstRes() must have already be called.
 * @param s         TCP Socket from which to read the next name-value parameter (if any)
 * @param param     Will hold name and value strings when returning
 * @param paramLen  On entry into routine holds lengh of param buffer. Will hold Index
 *                  of value string on return, or 0 if there were none.
 * @return 1 if there are parameters to follow, else 0
 */
BYTE HTTPGetParam(TCP_SOCKET s, BYTE* param, BYTE* paramLen)
{
    BYTE c;
    BYTE index;
    BYTE lastIndex;     //Index (offset) of last byte in given param buffer
    BYTE moreParamsToFollow;
    #if (DEBUG_HTTP >= LOG_DEBUG)
    BYTE* bufOutput;
    bufOutput = param;
    #endif

    //- When entering this function, s will point to first character of name field
    //- When leaving this function, s will point to first character after value field (or
    //  & character of value field if more parameters are to follow)
    
    lastIndex = (*paramLen) - 1;
    index = 0;
    *paramLen = 0;  //Indicates that there is no value field
    moreParamsToFollow = 0;

    //Null terminate last location of given buffer
    param[lastIndex] = '\0';

    //Read requested resource name
    while ((moreParamsToFollow==0) && TCPGet(s, &c))
    {
        //Name field is finished
        if (c == '=') {
            c = '\0';

            //Is there still space in the given buffer for the value field?
            if (index < lastIndex) {
                *paramLen = index + 1;  //Use paramLen as output parameter
            }
        }
        //Value field is finished, and there ARE more parameters to follow
        else if (c == '&') {
            moreParamsToFollow = 2;     //We return (moreParamsToFollow - 1) = return 1 = more params to follow
            c = '\0';
        }
        //Value field is finished, and there are NO more parameters to follow
        else if ((c == ' ') || (c == '\r') || (c == '\n')) {
            moreParamsToFollow = 1;     //We return (moreParamsToFollow - 1) = return 0 = NO more params to follow
            c = '\0';
        }
        //Recover space characters.
        else if (c == '+') {
            c = ' ';
        }

        //Write received char to buffer - keep last location free for NULL terminating char
        if (index < lastIndex) {
            *param = c;
        }
        #if (DEBUG_HTTP >= LOG_WARN)
        else {
           debugPutMsg(16);   //Name-Value pair of GET command had to be truncated. Larger then buffer!
        }
        #endif

        param++;
        index++;
    };
    
    //moreParamsToFollow will be 0 if no value field was found and TCPGet() returned 0. In this case, assume
    //there are no more parameters to follow
    if (moreParamsToFollow == 0) return 0;
    
    #if (DEBUG_HTTP >= LOG_DEBUG)
    debugPutMsg(17);     //@mxd:17:Read HTTP command, name=%s, value=%s
    debugPutString(bufOutput);
    debugPutString(&bufOutput[*paramLen]);
    #endif
    
    //Indicate if there are parameters to follow or not
    return (moreParamsToFollow - 1);
}

/**
 * Parses the http command present on given socket
 * Note:    - When entering this function, s will point to first character of command name <br>
 *          - When leaving this function, s will point to first character after the space
 *            trailing this command.
 *
 * @param s     TCP Socket from which to read
 *
 * @return      The http command found.
 */
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

/**
 * Sends a file from the filesystem.
 * @param ph A HTTP connection info.
 * @return TRUE if finished sending file, FALSE if file has partially been sent.
 */
static BOOL sendFile(HTTP_INFO* ph)
{
    BOOL lbTransmit;
    BYTE c;

    while( TCPIsPutReady(ph->socket) )
    {
        lbTransmit = FALSE;

        //Get next character from file system
        if ( ph->smHTTPSub != SM_HTTP_GET_VAR )
        {
            //If file has reached end, finish transmission
            if ( fileIsEOF(ph->file) ) {
                #if (DEBUG_HTTP >= LOG_INFO)
                debugPutMsg(8);   //Reached EOF
                #endif

                //fileGetByteEnd();
                TCPFlush(ph->socket);
                return TRUE;
            }

            c = fileGetByte(ph->file);

            //If error, finish transmisstion
            if (fileHasError(ph->file)) {
                #if (DEBUG_HTTP >= LOG_ERROR)
                debugPutMsg(9);   //Error while reading file
                #endif
                TCPFlush(ph->socket);
                return TRUE;
            }
        }

        //Is this a Dynamic file type (CGI file). If so, process
        if ( ph->flags.bits.bProcess )
        {
            switch(ph->smHTTPSub)
            {
            case SM_HTTP_GET_READ:
                //If escape character found, get variable
                if ( c == HTTP_VAR_ESC_CHAR )
                    ph->smHTTPSub = SM_HTTP_GET_VAR_GRP;
                else
                    lbTransmit = TRUE;
                break;
            // - If second % char is received, transmit a % char
            // - If not, read in variable group
            case SM_HTTP_GET_VAR_GRP:
                if ( c == HTTP_VAR_ESC_CHAR )
                {
                    lbTransmit = TRUE;
                    ph->smHTTPSub = SM_HTTP_GET_READ;
                }
                else
                {
                    ph->var.get.tagGroup = c;                   //Read MSB of HTTP variable = group
                    ph->smHTTPSub = SM_HTTP_GET_VAR_VALMSB;     //Read LSB of variable next
                }
                break;
            //Read MSB of variable value. Value is hex coded
            case SM_HTTP_GET_VAR_VALMSB:
                //Lowercase character
                if (c >= 'a')
                    c -= 39;
                //Uppercase character
                else if (c >= 'A')
                    c -= 7;
                ph->var.get.tagVal = (c - '0') << 4;

                ph->smHTTPSub = SM_HTTP_GET_VAR_VALLSB;
                break;
            //Read MSB of variable value. Value is hex coded
            case SM_HTTP_GET_VAR_VALLSB:
                //Lowercase character
                if (c >= 'a')
                    c -= 39;
                //Uppercase character
                else if (c >= 'A')
                    c -= 7;
                ph->var.get.tagVal |= (c - '0');

                ph->smHTTPSub = SM_HTTP_GET_VAR;
                ph->var.get.varRef = HTTP_START_OF_VAR;
                break;
            case SM_HTTP_GET_VAR:
                ph->var.get.varRef = HTTPGetVar(ph, &c);
                lbTransmit = TRUE;
                if ( ph->var.get.varRef == HTTP_END_OF_VAR )
                {
                    //For strings, the trailing NULL character should not be transmitted
                    if (c == '\0')
                        lbTransmit = FALSE;
                        
                    ph->smHTTPSub = SM_HTTP_GET_READ;
                }
                break;
            default:
                break;
            }

            if ( lbTransmit )
                TCPPut(ph->socket, c);
        }
        else
            TCPPut(ph->socket, c);
            
        FAST_USER_PROCESS();
    }

    // Release file. The file still remains open, and we can continue later on.
    fileRelease(ph->file);

    // We are not done sending a file yet...
    return FALSE;
}

/**
 * Sends a "\r\r" end of line string to TCP
 * @param ph A HTTP connection info.
 */
static void sendLineEnd(HTTP_HANDLE h)
{
    HTTP_INFO* ph;
    ph = &HCB[h];

    TCPPut(ph->socket, '\r');
    TCPPut(ph->socket, '\n');
}

/**
 * Send the given string via TCP. String has to be NULL terminated!
 */
static void sendRomStr(HTTP_HANDLE h, ROM char* s) {
    HTTP_INFO* ph;
    ph = &HCB[h];

    while (*s != '\0') {
        TCPPut(ph->socket, *s++);
    }
}

#endif //#if defined(STACK_USE_HTTP_SERVER)
