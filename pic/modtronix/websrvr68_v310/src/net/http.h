/**
 * @brief           HTTP Module for Modtronix TCP/IP Stack
 * @file            http.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    none
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_httpsrvr
 *
 * @section description Description
 **********************************
 * Contains the HTTP Server code.
 * <br>For a detailed description, see the @ref mod_tcpip_httpsrvr section of this document - in 
 * [Modules] [HTTP Server].
 *
 *
 * @section http_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //--------------------   Configuration --------------------
 //*********************************************************************

 //Define the port used for the HTTP server, default is 80
 #define DEFAULT_HTTPSRVR_PORT (80ul)

 //Configured HTTP Server port
 #define HTTPSRVR_PORT     (80ul)

 //Define as 1 to parse (replace %xnn tags) HTML files, or 0 not to parse them 
 #define HTTP_PARSE_FILETYPE_HTML 0ul

 //Define as 1 to parse (replace %xnn tags) JavaScript files, or 0 not to parse them 
 #define HTTP_PARSE_FILETYPE_JS 0ul

 //Define as 1 if Authentication required for all files that start with 'X' character
 #define HTTP_AUTH_REQ_FOR_X_FILES 1ul

 //Define as 1 if Authentication required for all
 #define HTTP_AUTH_REQ_FOR_ALL_FILES 0ul

 //Define as 1 if Authentication required for all pages with GET Methods
 #define HTTP_AUTH_REQ_FOR_GET 0ul

 //Define as 1 if Authentication required for all Dynamic files
 #define HTTP_AUTH_REQ_FOR_DYN 0ul

 //Define as 1 if Authentication required for all CGI files
 #define HTTP_AUTH_REQ_FOR_CGI 0ul

 //Define as 1 if Authentication required for all Secure Tags
 #define HTTP_AUTH_REQ_FOR_SECTAG 1ul

 //Included this define if the user application will process HTTP headers. It it does,
 //the HTTPProcessHdr() callback function must be implemented by the user
 #define HTTP_USER_PROCESSES_HEADERS

 @endcode
 *********************************************************************/

 /*********************************************************************
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
 **********************************************************************
 * File History
 *
 * 2001-8-14, Nilesh Rajbharti:
 *    - Original (Rev. 1.0)
 * 2002-2-9, Nilesh Rajbharti:
 *    - Cleanup
 * 2002-5-22, Nilesh Rajbharti:
 *    - Rev 2.0 (See version.log for detail)
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/


/////////////////////////////////////////////////
// HTTP Server - documentation module
/**
@defgroup mod_tcpip_httpsrvr HTTP Server

The HTTP Server included with the @appname is implemented as a cooperative task
that co-exists with the Modtronix TCP/IP Stack and the user’s main application.
The Server itself is implemented in the source file "HTTP.c", with a user application
implementing two callback functions.

The HTTP Server provided here does not implement all HTTP functionality. it is a minimal
server targeted for embedded system. The user can easily add new functionality as required.

The HTTP Server incorporates these main features:
- Supports multiple HTTP connections
- Contains a simple file system
- Supports Web pages located in either internal program memory or external serial EEPROM
- Includes a @ref page_update_webpages "PC-based program" to create a File System Image from a given directory
- Supports the HTTP method @b GET (other methods can be easily added)
- Supports a modified Common Gateway Interface (CGI) to invoke predefined functions from within
  the remote browser. See @ref mod_tcpip_httpsrvr_cgi for details.
- Supports dynamic web page content generation - see @ref mod_tcpip_httpsrvr_dynamic for details.

In order to integrate the HTTP Server into a user application, do the following (has already been done
for the @appname):
- Uncomment STACK_USE_HTTP_SERVER in the <i>projdefs.h</i> header file, this will enable HTTP
  Server related code. See @ref mod_conf_projdefs "Project Configuration" for details.
- Set the desired MAX_HTTP_CONNECTIONS value in the projdefs.h header file. See @ref mod_conf_projdefs
  "Project Configuration" for details.
- Include the files http.c in the project.
- Depending on what File System you want to use, include the required fsxx.c file in the project.
  The default File System is the Modtronix EEPROM File System, and requires the fsee.c file.
- Implement the HTTPGetVar() and HTTPExecGetCmd() callback functions in the user application.


It will also be necessary to generate any Web pages in advance and convert them into a compatible format for
storage. For details on generating Web pages, see @ref page_update_webpages.

@section mod_tcpip_httpsrvr_conf Configuration
For details on configuring this module, see @ref http_conf section of http.h file.
*/


/////////////////////////////////////////////////
// HTTP Server File Types - documentation module
/**
@defgroup mod_tcpip_httpsrvr_filetype File Types
@ingroup mod_tcpip_httpsrvr
The Modtronix HTTP server supports the following file types:

@li <b>CGI:</b> These files will be parsed by the CGI server. The CGI server will look for \%nxx tags
in the code, and replace them with default values or values defined by the user.

@li <b>CSS:</b> Style Sheet

@li <b>GIF:</b> Image file

@li <b>HTM:</b> HTML web page

@li <b>JAR:</b> JAR file

@li <b>JPG:</b> Image file

@li <b>JS:</b> JavaScript code file

@li <b>TXT:</b> Text file

@li <b>WAV:</b> Wave file

Any other file type can however also be used, but the HTTP Server will set the "Content-Type:" header
in the HTTP message to "text/plain". For most HTTP Clients (WEB Browsers) this is no problem, seeing
that the browser will figure out the file type by it's file extension. For example, if you add a PNG image to
your web pages, it will be sent as a "text/plain" file to the requesting HTTP Client, but on receiving
it the client will see it is a PNG image by it's file extension!

*/


/////////////////////////////////////////////////
// HTTP Compression - documentation module
/**
@defgroup mod_tcpip_httpsrvr_compress HTTP Compression
@ingroup mod_tcpip_httpsrvr
The Modtronix TCP/IP stack supports Gzip HTTP compression. All common browsers support
HTTP compression. By using this method, text, HTML and JavaScript files can be compressed.
This dramatically reduces the amount of space required to store a Web page in the EEPROM or
FLASH. Compressed files must be generated with the @ref tools_mxbootloader
application, and have the "Compress" check box enabled for that file. This will create a File System Image
with a flag set indicating to the Web Server that this is a compressed file.
Currenly the following files can be comressed:

@li <b>HTML files</b>, must have a <b>htm</b> file extension.

@li <b>HTML CSS files</b>, must have a <b>css</b> file extension.

@li <b>Text files</b>, must have a <b>txt</b> file extension.

@li <b>JavaScript files,</b> just have a <b>js</b> file extension.

It is important to note that NO tag (request command) replacement will be done on the contents of compressed files. This
means that any \%nxx tags contained in compressed files will not be parsed and replaced by the CGI server.
This is because the file is stored in compressed format in the EEPROM or FLASH, and can not be
parsed for % characters!
*/


/////////////////////////////////////////////////
// HTTP Authentication - documentation module
/**
@defgroup mod_tcpip_httpsrvr_pw HTTP Authentication
@ingroup mod_tcpip_httpsrvr
The Modtronix TCP/IP stack supports password protected web pages. Currently any web page that starts
with a 'X' character will be password protected. For example, to only display a page to a user that
has logged in you could name it "XPAGE1.HTM". When a user requests this page via a browser, the login
page will be displayed. After loggin in, the requested page will be displayed. The user is automatically
logged off if not active for 2 minutes.
The default @b username and @b password for the @appname is:<br>
Username = "admin"<br>
Password = "pw"

To change this, click on the "Change the username and password" link on the login page. The username
and password can have a maximum of 8 characters each.
*/


/////////////////////////////////////////////////
// HTTP Caching - documentation module
/**
@defgroup mod_tcpip_httpsrvr_cach HTTP Caching
@ingroup mod_tcpip_httpsrvr
The Modtronix HTTP server sets JavaScript files to be cached for 10 minutes and
all other files to be cached for 5 minutes. This is done with the "Cache-Control: max-age=..."
HTTP headers. This has the effect that JavaScript files are only downloaded every 10 minutes, and
all other files every 5 minutes. This can be changed by modifying the "http.c" file.

If a web page file should not be cached, it should have to following
line in it's header:
@code
<meta http-equiv="Pragma" content="no-cache">
@endcode
*/


/////////////////////////////////////////////////
// CGI Documentation
 /**
@defgroup mod_tcpip_httpsrvr_cgi CGI
@ingroup mod_tcpip_httpsrvr
@section Introduction
CGI commands are accepted by the Web Server via the HTTP GET command. All CGI commands
follow the HTTP GET syntax of <i>name=value</i>. For example, to set PIC port pin
A2, we can send the following HTTP command to the target board:<br>
http://10.1.0.1/?a2=1
<br>In this example the <i>name</i> part is "a2" and the <i>value</i> part is '1'.

The <i>name</i> and <i>value</i> strings are only allowed to use the following characters:
- Upper and lower case letters
- Decimal digits
- The following Unreserved Characters: -  _ . ! ~ * ' ( )

@section mod_tcpip_httpsrvr_cgi_getcmd GET Command
A remote client invokes a function by the HTML GET method with more than one parameter.
Refer to RFC1866 (the HTML 2.0 language specification) for more information. When a remote browser
executes a GET method with more than one parameter, the HTTP server parses it and calls the main
application with the actual method code and its parameter. In order to handle this method, the main
application must implement a callback function with an appropriate code.

The Modtronix HTTP Server does not perform "URL decoding". This means that if any of the form field
text contains certain special non-alphanumeric characters (such as <, >, #, %, etc.), the actual
parameter value would contain "%xx" ("xx" being the two-digit hexadecimal value of the ASCII character)
instead of the actual character. For example, an entry of "<Name>" would return "%3CName%3C".

@if httpExecGetImplemented
The HTTPExecGetCmd() function is a callback function from the HTTP Server. When the HTTP Server receives
a GET method with more than one parameter, it calls this function. This function has been implemented
by the @appname in the "httpexec.c" file! All commands implemented by the @appname are listed below.
@else
The HTTPExecGetCmd() function is a callback function from the HTTP Server. When the HTTP Server receives
a GET method with more than one parameter, it calls this function. This function is not implemented
in the stack, and has to be implemented by the user application.
@endif This function must repetively call the HTTPGetParam() function until all name-value parameters sent
with the GET command have been obtained. It must then decode the name-value parameter and take
appropriate actions. Such actions may include supplying a new Web page name to be returned and/or
performing an I/O task.

@section mod_tcpip_httpsrvr_cgi_defined Defined Commands
For a list of all commands, see @ref mod_cmd_defined
*/


/////////////////////////////////////////////////
// Dynamic Web Pages Documentation
/**
@defgroup mod_tcpip_httpsrvr_dynamic Dynamic Web Pages
@ingroup mod_tcpip_httpsrvr
@section Introduction
The HTTP Server can dynamically alter pages and substitute real-time information, such as input/output
status. The web server will parse CGI files for special tags, and replace them with user defined
strings. By default, only "*.cgi" files are parsed for these special tags. It is however possible configuring
other files to be parsed for tags too.
<p>
To incorporate this real-time information, the corresponding Dynamic file (*.cgi by default) must contain a
text string with the format: "%nxx". Where the '\%' character serves as a control code, 'n'
represents the variable group and "xx" represents a two-digit variable value ( in uper case hex format).
<br>The <i>variable value</i> has a range of 00-FF (Which translates to 0-255 decimal), and must use <b>upper case</b>
characters!
<br>The <i>variable group</i> ('n' character) can be any alpha numeric character ('0-9', 'a-z', 'A-Z'),
giving a total of 10+26+26 = 62 groups. Each group can have 256 variable values. This gives a total of
15872 possible variables.</p>
<p>

@if httpGetVarImplemented
When the HTTP Server encounters this text string, it removes the '\%' character and calls the HTTPGetVar()
function. This function has been implemented by the @appname in the "httpexec.c" file! All tags implemented
by the @appname are listed below.
@else
When the HTTP Server encounters this text string, it removes the '\%' character and calls the HTTPGetVar()
function. This function is not implemented in the stack, and has to be implemented by the user application.
@endif The HTTPGetVar() function will define strings that will replace tags found in the Dynamic file.
If the page requires '\%' as a display character, it should be preceded by another '\%' character.
For example, to display "23%" in a page, put "23%%".</p>

@section mod_tcpip_httpsrvr_dynamic_defined Defined Tags (Tags)
For a list of all tags, see @ref mod_tags_defined

*********************************************************************/


#ifndef HTTP_H
#define HTTP_H

#include "net\ip.h"         //Requered for TCP_SOCKET definition
#include "net\tcp.h"        //Requered for TCP_SOCKET definition
#include "net\fsee.h"       //Requered for TCP_SOCKET definition
#include "net\security.h"   //Security related code


/////////////////////////////////////////////////
//Default defines - they can be overridden in projdefs.h file

#define HTTP_START_OF_VAR       (0x0000ul)
#define HTTP_END_OF_VAR         (0xFFFFul)


/////////////////////////////////////////////////
// HTTP FSM states for each connection.
typedef enum _SM_HTTP
{
    SM_HTTP_IDLE,
    SM_HTTP_GET_TX_HDR,         /* We are busy transmitting the header of our responce to client's GET message */
    SM_HTTP_GET_TX_BODY,        /* We are busy transmitting the body of our responce to a client's GET message */
    SM_HTTP_POST_RX_HDR,        /* We are busy receiving a POST header */
    SM_HTTP_POST_RX_BODY,       /* We are busy receiving a POST message body */
    SM_HTTP_NOT_FOUND,
    SM_HTTP_AUTHENTICATION,     /* Authentication required */
    SM_HTTP_DISCONNECT,
    SM_HTTP_DISCONNECT_WAIT
} SM_HTTP;


/////////////////////////////////////////////////
// HTTP GET FSM states
typedef enum _SM_HTTP_GET
{
    SM_HTTP_GET_READ,
    SM_HTTP_GET_VAR_GRP,
    SM_HTTP_GET_VAR_VALMSB,
    SM_HTTP_GET_VAR_VALLSB,
    SM_HTTP_GET_VAR
} SM_HTTP_GET;


/////////////////////////////////////////////////
// HTTP GET FSM states
typedef enum _SM_HTTP_POST
{
    SM_HTTP_POST_IMAGE
} SM_HTTP_POST;


/////////////////////////////////////////////////
// HTTP Connection Info - one for each connection.
typedef struct _HTTP_INFO
{
    TCP_SOCKET socket;  //At initialization, every HTTP connection gets assigned a TCP socket.
    FILE file;
    SM_HTTP smHTTP;     //State of main HTTP Server state machine
    BYTE smHTTPSub;     //State of any sub state machines

    //The following union contains variables used by different parts of the HTTP state maschine
    union {
        //Variables for general HTTP state maschine
        struct
        {
            BYTE        msgCode;
        } gen;

        //Variables for HTTP GET command
        struct
        {
            WORD        varRef;
            BYTE        tagVal;     /* Value part of tag variable found on dynamic page */
            BYTE        tagGroup;   /* Group part of tag variable found on dynamic page */
            BYTE        fileType;   /* Identifies the file type, is a HTTP_FILETYPE_XXX constant */
        } get;

        //Variables for HTTP POST command
        struct
        {
            WORD        temp;
        } post;
    } var;

    union {
        struct
        {
            unsigned int bProcess : 1;      /* Indicates if this is a Dynamic File */
            unsigned int bOwnsFile : 1;     /* Indicate if this HTTP Connection currently has a file open. If true, file will contain file's FILE handle */
            unsigned int bUserLoggedIn : 1; /* Indicates that the Admin or Super user is currently logged in, see bUserSuper to see which one it is! */
            unsigned int bUserSuper : 1;    /* Indicates that the Admin user is currently logged in */
            unsigned int bLoginReq : 1;     /* Indicates that the user is required to log in for this page to be displayed */
        } bits;
        BYTE val;
    } flags;

} HTTP_INFO;


/**
 * Maximum length of a requested resource via HTTP = 8 + 3.
 */
#define HTTP_MAX_RESOURCE_NAME_LEN (12)


/**
 * Set all HTTP connections to Listening state. Initialize FSM for each connection.
 * This function is called only one during lifetime of the application.
 * Modifies HTTP FSM and connections are initialized
 *
 * @preCondition TCP must already be initialized.
 */
void HTTPInit(void);


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
void HTTPServer(void);

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
BYTE HTTPGetParam(TCP_SOCKET s, BYTE* param, BYTE* paramLen);

#if defined(THIS_IS_HTTP_SERVER)
    /*
     * Main application must implement these callback functions
     * to complete Http.c implementation.
     */

/**
 * This is a callback function from the HTTPServer() to the main application. This function
 * must be implemented by the user somewhere in the code. Whenever a variable substitution
 * is required (the parsed web pages contains a "\%nxx" tag) on any Dynamic pages, HTTPServer
 * calls this function. This function is responsible for replacing this variable ("\%nxx" tag")
 * with a user defined string. For example, the user might want to replace any "\%a00" tag found
 * on a web page with the value of channel 0 of the analog input. When the "\%a00" is found on
 * a web page, the HTTPServer will call this function, and the user can then return a string
 * representing the value of channel 0 of the analog input.
 *
 * <p>The given HTTP_INFO structure contains a 8-bit <i>variable reference</i> (httpInfo->VarRef), a
 * 8-bit <i>variable group</i> (httpInfo->var.get.tagGroup) and a 16-bit <i>variable value</i>
 * (httpInfo->var.get.tagVal).
 * <br>The <i>variable reference</i> indicates whether this is a first call or not.
 * <br>The <i>variable group</i> and <i>variable value</i> gives the group and value of the tag
 * found on the web page.
 *
 * <p>Applications should return one character at a time as a variable value. If there are no more
 * characters, or the requeseted variable is not found, this function must set <i>val</i> to NULL.
 * Since this function only allows one character to be returned at a time as part of the variable
 * value, HTTPServer() calls this function multiple times until the user application indicates that
 * there are no more values left for this variable. On begining, HTTPGetVar() is called with
 * the <i>variable reference</i> (httpInfo->var.get.varRef) = HTTP_START_OF_VAR to indicate that this is a
 * first call. Applications should use this reference to start the variable value extraction and return
 * updated reference. If there are no more values left for this variable the application should return
 * HTTP_END_OF_VAR. If there are any bytes left to be sent, the application should return a value
 * other than HTTP_START_OF_VAR or HTTP_END_OF_VAR.
 *
 * @preCondition    HTTPInit() must already be called.
 * @param httpInfo  HTTP_INFO structure of HTTP connection requesting this Variable.
 * @param val       Buffer for value storage.
 * @return          Variable reference as required by application.
 */
extern WORD HTTPGetVar(HTTP_INFO* httpInfo, BYTE* val);

/**
 * This function is a "callback" from HTTPServer task. Whenever a remote node performs interactive
 * GET task on page that was served, HTTPServer calls this functions. Use HTTPGetParam()
 * to get all name-value parameters.
 *
 * @param httpInfo  HTTP_INFO structure of current HTTP connection
 * @param rqstRes   Name of the Requested resource - GET command's action. All characters are in uppercase!
 */
extern void HTTPExecGetCmd(HTTP_INFO* httpInfo, BYTE* rqstRes);

/**
 * This function is a "callback" from HTTPServer task. For each HTTP Header found in the HTTP Request
 * message from the client, this function is called. The application has the chance to parse the
 * received headers.
 *
 * @param httpInfo  HTTP_INFO structure of current HTTP connection
 * @param hdr       Buffer containing NULL terminated header to handle
 * @param rqstRes   Name of the Requested resource - GET command's action. All characters are in uppercase!
 */
extern void HTTPProcessHdr(HTTP_INFO* httpInfo, BYTE* hdr, BYTE* rqstRes);
#endif


/**
 * Returns the current user logged in.
 *
 * @return The current user logged in. Is a USER_XX constant
 */
BYTE HTTPGetCurrentUser(HTTP_INFO* httpInfo);


#endif
