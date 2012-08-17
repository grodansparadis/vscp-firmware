/********************************************************************
 * FileName:		http.c
 * Dependencies:
 * Processor:		PIC24FJ128
 * Hardware:		N/A
 * Assembler:		N/A
 * Linker:		N/A
 * Company:		Microchip Technology, Inc.
 *
 * Software License Agreement:
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
 *********************************************************************
 * File Description:
 *
 * HTTP Server Appilcation
 *
 * Change History:
 * Name         Date            Changes
 * S Justice    01/09/06        Initial Version
 ********************************************************************/
//#define _HTTP_EXTEND_SET

#define htons       swaps
#define _HTTP_PRIVATE_FUNC
#include <string.h>
#include "http.def"
#include "bsd_http_server\http.h"
#include "http_private.h"
#include "httpEx.h"

/********************************************************************
 * HTTP Private constants
 ********************************************************************/
const BYTE _httpDefaultFileName[]   = HTTP_DEFAULT_FILE_STR;
const BYTE _httpStatusMsgStart[]    = "HTTP/1.1 ";
const BYTE _httpMsgAuthenticate[]   = "WWW-Authenticate: ";
const BYTE _httpMsgAuthorization[]  = "Authorization: ";
const BYTE _httpMsgContentStr[]     = "Content-Length: ";
const BYTE _httpMsgContentType[]    = "Content-Type: ";
const BYTE _httpMsgHdrEnd[]         = "\r\n";
const BYTE _httpMsgExpectCont[]     = "Expect: 100-Continue";
const BYTE _httpMsgAuthMethod[]     = HTTP_AUTHENTICATION_METHOD;

#define HTTP_STATUS_MSG_START       (sizeof(_httpStatusMsgStart) - 1)
#define HTTP_MSG_AUTHENTICATE_LEN   (sizeof(_httpMsgAuthenticate) - 1)
#define HTTP_MSG_CONTENT_LEN        (sizeof(_httpMsgContentStr) - 1)
#define HTTP_MSG_HDR_END_LEN        (sizeof(_httpMsgHdrEnd) - 1)
#define HTTP_MSG_AUTHORIZATION_LEN  (sizeof(_httpMsgAuthorization) - 1)
#define HTTP_MSG_AUTH_METHOD_LEN    (sizeof(_httpMsgAuthMethod) - 1)
#define HTTP_MSG_EXPECT_CONT        (sizeof(_httpMsgExpectCont) - 1)
#define HTTP_MSG_CONTENT_TYPE_LEN   (sizeof(_httpMsgContentType) - 1)

const HTTP_CMD_TBL _httpCmdTbl[] =
{
    {   "GET",      3,  HTTP_GET_CMD        },
    {   "POST",     4,  HTTP_POST_CMD       },
    {   "PUT",      3,  HTTP_PUT_CMD        },
    {   "HEAD",     4,  HTTP_HEAD_CMD       },
    {   "OPTIONS",  7,  HTTP_OPTIONS_CMD    },
    {   "PROPFIND", 8,  HTTP_PROPFIND_CMD   }
};
#define HTTP_CMD_TBL_SIZE   (sizeof(_httpCmdTbl) / sizeof(HTTP_CMD_TBL))

const HTTP_FILE_EXT _httpExtTbl[] =
{
    { "TXT",    "text/plain\r\n"                },
    { "HTM",    "text/html\r\n"                 },
    { "GIF",    "image/gif\r\n"                 },
    { "CGI",    "text/html\r\n"                 },
    { "JPG",    "image/jpeg\r\n"                },
    { "CLA",    "application/java-vm\r\n"       },
    { "WAV",    "audio/xwave\r\n"               },
    { "JS",     "application/x-javascript\r\n"  },
    { "CSS",    "text/css\r\n"                  },
    { "PNG",    "image/png\r\n"                 }
};
#define HTTP_EXT_TBL_SIZE   (sizeof(_httpExtTbl) / sizeof(HTTP_FILE_EXT))

const HTTP_STATUS_CODE_TBL _httpStatusTbl[] =
{
    {   "100 Continue\r\n"              },
    {   "200 OK\r\n"                    },
    {   "201 Created\r\n"               },
    {   "202 Accepted\r\n"              },
    {   "204 No Content\r\n"            },
    {   "301 Moved Permanently\r\n"     },
    {   "302 Moved Temporarily\r\n"     },
    {   "304 Not Modified\r\n"          },
    {   "400 Bad Request\r\n"           },
    {   "401 Unauthorized\r\n"          },
    {   "403 Forbidden\r\n"             },
    {   "404 Not Found\r\n"             },
    {   "500 Internal Server Error\r\n" },
    {   "501 Not Implemented\r\n"       },
    {   "502 Bad Gateway\r\n"           },
    {   "503 Service Unavailable\r\n"   }
};
#define HTTP_STATUS_CODE_TBL_SIZE (sizeof(_httpStatusTbl) / sizeof(HTTP_STATUS_CODE_TBL))
/********************************************************************
 * HTTP Private attributes
 ********************************************************************/
PRIVATE SOCKET          _listenerSocket;
PRIVATE BYTE            _numberOfAccepts;
PRIVATE BYTE            _connectionIdx;
PRIVATE HTTP_CONNECT    _httpConnection[HTTP_MAX_CONNECTIONS];
#ifndef HTTP_SMALL_READ
#define HTTP_READ_SIZE HTTP_SEND_BUF_LEN
PRIVATE BYTE    read_buff[HTTP_READ_SIZE];
#else
#define HTTP_READ_SIZE 64
#endif
/*********************************************************************
 * Function:        BOOL HTTPInit(void)
 *
 * PreCondition:    The TCP is already initialized
 *
 * Input:           none
 *
 * Output:          The HTTP connection is initialized.  If success return true.
 *
 * Side Effects:    None
 *
 * Overview:        Open a socket listening for the clients to connect.
 *
 * Note:            none
 ********************************************************************/
PUBLIC BOOL HTTPInit(void)
{
	SOCKADDR_IN		saServer;
    int             nResult;


    for(nResult = 0; nResult < HTTP_MAX_CONNECTIONS; nResult++)
        _httpConnection[nResult].open = HTTP_CONNECTION_CLOSED;

    _listenerSocket = socket(   AF_INET,
                                SOCK_STREAM,
						        IPPROTO_TCP);

    if(_listenerSocket == INVALID_SOCKET)
        return FALSE;

    #ifdef _BORLAND_BUILD
	saServer.sin_port           = htons(HTTP_PORT);
    #else
	saServer.sin_port           = HTTP_PORT;
    #endif
	saServer.sin_family         = AF_INET;
	saServer.sin_addr.s_addr    = INADDR_ANY;

    nResult = bind( _listenerSocket,
                    (LPSOCKADDR)&saServer,
                    sizeof(struct sockaddr));

    if(nResult == SOCKET_ERROR)
    {
        closesocket(_listenerSocket);
        return FALSE;
    }

    nResult = listen(_listenerSocket, HTTP_MAX_CONNECTIONS);

    if(nResult == SOCKET_ERROR)
    {
        closesocket(_listenerSocket);
        return FALSE;
    }

    _numberOfAccepts    = 0;
    _connectionIdx      = 0;

    return TRUE;
}
/*********************************************************************
 * Function:        void HTTPCloseAll(void)
 *
 * PreCondition:    none
 *
 * Input:           none
 *
 * Output:          Closes the listening socket and any connections
 *
 * Side Effects:    Will not able to handle uncoming client requests
 *
 * Overview:        The user should call this when they would like to
 *                  discontinue client requests
 *
 * Note:            none
 ********************************************************************/
PUBLIC void HTTPCloseAll(void)
{
    BYTE i, num;

    closesocket(_listenerSocket);

    num = _numberOfAccepts;

    for(i = 0; i < num; i++)
        _CloseConnection(&_httpConnection[i]);

    _numberOfAccepts = 0;
}
/*********************************************************************
 * Function:        BYTE HTTPGetNumberOfConnections(void)
 *
 * PreCondition:    none
 *
 * Input:           none
 *
 * Output:          Get the number of HTTP connections
 *
 * Side Effects:    None
 ********************************************************************/
PUBLIC BYTE HTTPGetNumberOfConnections(void)
{
    return _numberOfAccepts;
}    
/*********************************************************************
 * Function:        void HTTPServer(void)
 *
 * PreCondition:    HTTPInit method has been called.  The defines that are
 *                  used in the command, filename, variable, and valua length have
 *                  all been properly defined in http.h.
 *
 * Input:           none
 *
 * Output:          none
 *
 * Side Effects:    none
 *
 * Overview:        Accepts connections from clients and serves their requests
 *
 * Note:            The server will service the GET, POST and PUT commands.
 ********************************************************************/
PUBLIC void HTTPServer(void)
{

    _AcceptConnection();            /* check to see if we have a new connection */

    if(_connectionIdx >= _numberOfAccepts)
        _connectionIdx = 0;


    if(_numberOfAccepts)
    {
        LP_HTTP_CONNECT connection;
        int             result;

        /*
         * Set up the pointers
         */
        
        connection  = _GetConnection(_connectionIdx);

        if(!connection)
        {
            _connectionIdx++;
            return;
        }

        if(connection->open == HTTP_CONNECTION_CLOSED)
        {
	        _connectionIdx++;
	  	}
	  	
        #ifdef _BORLAND_BUILD
        {
            BOOL    delay = TRUE;

            setsockopt( connection->sSocket,
                        IPPROTO_TCP,
                        TCP_NODELAY,
                        (char *)&delay,
                        sizeof(BOOL));
        }
        #endif

        /*
         * Any pending data to send
         */

		if(connection->TxSize > 0)
        {
			connection->TxSize = _SendData( connection->sSocket,
                                            connection->TxBuf,
                                            connection->TxSize);


			if(connection->TxSize > 0)
            {
                _connectionIdx++;
                return;
            }
        }

		if(connection->TxSize == SOCKET_ERROR)
		{
			_CloseConnection(connection);
			return;
		}

        if(connection->yCmd == HTTP_INVALID_CMD)
		{
			if(connection->bProcess)
			{
				_CloseConnection(connection);
              return;
			}
		}
		/*
         * service any pending commands
         */
        switch(connection->yCmd)
        {
            case HTTP_GET_CMD:
				if(_ServiceGETRequest(connection))
				{
                    connection->yCmd = HTTP_INVALID_CMD;
					_connectionIdx++;
                    connection->bProcess = TRUE;
				    return;
                }
                break;

            case HTTP_POST_CMD:
                if(_ServicePOSTRequest(connection))
				{
                    connection->yCmd = HTTP_INVALID_CMD;
					_connectionIdx++;
                    connection->bProcess = TRUE;
				    return;
                }
                break;

            case HTTP_PUT_CMD:
                if(_ServicePUTRequest(connection))
				{
                    connection->yCmd = HTTP_INVALID_CMD;
					_connectionIdx++;
                    connection->bProcess = TRUE;
				    return;
                }
                break;

            case HTTP_OPTIONS_CMD:
            case HTTP_PROPFIND_CMD:
            case HTTP_HEAD_CMD:
                
                if(!_FlushHeader(connection->RxPtr, connection->RxBuf, connection->sSocket))
                    break;

                _CreateHTTPStatusLine(  HTTP_STATUS_OK,
                                        connection->TxBuf);

                strcat(connection->TxBuf, _httpMsgHdrEnd);

                connection->TxSize  = strlen(connection->TxBuf);
                connection->yCmd    = HTTP_INVALID_CMD;
                connection->bProcess = TRUE;

                connection->TxSize = _SendData(connection->sSocket, connection->TxBuf, connection->TxSize);

                if(connection->TxSize)
                {
                    _connectionIdx++;
                    return;
                }
                break;


            case HTTP_NOT_SUPPORTED:
                _NotSupportedResponce(connection);
                _FlushConnection(connection, connection->RxBuf);
                connection->yCmd = HTTP_INVALID_CMD;
                break;

			default:
                connection->yCmd = HTTP_INVALID_CMD;
				break;
        }


        /*
         * Check to see if we have anything else left to do
         */

        if(connection->yCmd == HTTP_INVALID_CMD)
        {
            if(strlen(connection->RxBuf) < 3)
            {
                result      = recv(connection->sSocket, connection->RxBuf, HTTP_HTML_CMD_LEN, 0);


                #ifdef _BORLAND_BUILD
                if(result == 0)
                {
                    _CloseConnection(connection);
                    return;
                }
                #endif

                if(result == SOCKET_ERROR)
                {
                    _CloseConnection(connection);
                    return;
                }

                connection->RxPtr   = connection->RxBuf;
            }else
            {
                result = 1;
                connection->RxPtr   = connection->RxBuf;
                connection->RxPtr   += 2;                   // skip over the \r\n
            }

            if(result)
            {
                connection->yCmd    = _GetHTTPCmd((BYTE **)&connection->RxPtr, connection);
                
                if(connection->yCmd != HTTP_INVALID_CMD)
                    connection->bProcess = FALSE;

                return;
            }
        }else
        {
            connection->RxPtr = connection->RxBuf;
        }

        _connectionIdx++;
    }

}
/*********************************************************************
 * Function:        const SOCKET *HTTPGetListenerSocket(void)
 *
 * PreCondition:    HTTPInit has been called
 *
 * Input:           none.
 *
 * Output:          A pointer to the listener socket.
 *
 * Side Effects:    none
 *
 * Overview:        Will return an unmodifiable pointer.
 *
 * Note:
 ********************************************************************/
PUBLIC const SOCKET *HTTPGetListnerSocket(void)
{
    return ((const SOCKET *)&_listenerSocket);
}
/*********************************************************************
 * Function:        static BOOL _AcceptConnection(void)
 *
 * PreCondition:    User must properly define the HTTP_MAX_CONNECTIONS in
 *                  http.h.
 *
 * Input:           none
 *
 * Output:          Returns true if a new connection has been accepted
 *
 * Side Effects:    Create memory on the heap
 *
 * Overview:        Accepts an incoming request for a client to
 *                  connect.
 *
 * Note:            None
 ********************************************************************/
PRIVATE BOOL _AcceptConnection(void)
{
	SOCKADDR_IN	sockaddr;
	SOCKET		peersocket;
	BOOL		bresult;
	int			nlen;

    if(_numberOfAccepts >= HTTP_MAX_CONNECTIONS)
        return FALSE;


    #ifdef _BORLAND_BUILD
    {
        fd_set  readfds;
        int     status;
        struct  timeval to = {0,0};

        FD_ZERO(&readfds);
        FD_SET(_listenerSocket, &readfds);

        status = select(0, &readfds, NULL, NULL, &to);

        if((status == SOCKET_ERROR) || (!status))
            return FALSE;
    }
    #endif
	nlen        = sizeof(SOCKADDR_IN);

	peersocket  = accept(   _listenerSocket,
					        (LPSOCKADDR)&sockaddr,
						    &nlen);

	if (peersocket == SOCKET_ERROR)
	{
        return FALSE;
	}

    bresult = _AddConnection(peersocket);

    if(bresult)
    {
        _numberOfAccepts++;
    }

    return bresult;
}
/*********************************************************************
 * Function:        static HTTP_REQ_CMD _GetHTTPCmd(    BYTE *buf,
 *                                                      LP_HTTP_CONNECT connection)
 *
 * PreCondition:    none
 *
 * Input:           buf         - the Rx data from the client
 *                  connection  - the client data
 *
 * Output:          The command that the client is requesting.
 *
 * Side Effects:    none
 *
 * Overview:        After Rx a command from the client, this method is
 *                  called to parse and return the request.
 *
 * Note:
 ********************************************************************/
PRIVATE HTTP_REQ_CMD _GetHTTPCmd(BYTE **recv_buf, LP_HTTP_CONNECT connection)
{

    BYTE            i;
    HTTP_REQ_CMD    cmd;
    BYTE            *buf;
    LP_HTTP_CMD_TBL tbl;

    buf = *recv_buf;
    cmd = HTTP_NOT_SUPPORTED;
    tbl = (LP_HTTP_CMD_TBL)_httpCmdTbl;

    for(i = 0; i < HTTP_CMD_TBL_SIZE; i++)
    {
        if(!strncmp(buf, tbl->commandString, tbl->commandLen))
            break;

        tbl++;
    }

    if(i >= HTTP_CMD_TBL_SIZE)
        return cmd;

    cmd = tbl->cmd;

    buf += (tbl->commandLen + 1);

    while(*buf == ' ')
        buf++;

    *recv_buf = buf;

    switch(cmd)
    {
        case HTTP_GET_CMD:
            connection->yCmdStage = HTTP_GET_SRV_SEARCH;
            break;

        case HTTP_POST_CMD:
            connection->yCmdStage = HTTP_POST_SRV_HEADER;
            break;

        case HTTP_PUT_CMD:
            connection->yCmdStage = HTTP_PUT_SRV_HEADER;
            break;

		default:
            connection->yCmdStage = HTTP_GET_SRV_SEARCH;
            break;
	
    }
    return cmd;

}
/*********************************************************************
 * Function:        static void _ServiceGETRequest( LP_HTTP_CONNECT connection)
 *
 * PreCondition:    The user should define HTTP_MAX_ARGS to be number of data
 *                  to be handled plus the filename.
 *
 * Input:           connection  - client data
 *
 * Output:          none.
 *
 * Side Effects:    none.
 *
 * Overview:        Service the HTTP GET request from the client.  If the request is
 *                  valid, then the server will send the request/default file to the
 *                  client.  If the client has sent any data with it, the server
 *                  will process too.
 *
 * Note:            Example:
 *                  GET filename.htm?var[0]=value[0]&....var[n]=value[n]
 *
 *                  Parsed data:
 *                  arg[0] = filename.htm
 *                  arg[1] = var[0]
 *                  arg[2] = value[0]
 *                  .
 *                  .
 *                  arg[2n + 1] = var[n]
 *                  arg[2n + 2] = value[n]
 *
 ********************************************************************/
PRIVATE BOOL _ServiceGETRequest(LP_HTTP_CONNECT connection)
{
    BYTE                *buff;
    BYTE                *recv_buf;

    buff        = connection->RxPtr;
    recv_buf    = connection->RxBuf;

    switch(connection->yCmdStage)
	{
	    case HTTP_GET_SRV_SEARCH:
	        {

                connection->yNumArg = HTTP_MAX_ARGS - 1;

                _ParseHTTPGETHeader(    connection->yFileName,
                                        &connection->yFileType,
                                        connection->pGetParams,
                                        &connection->yNumArg,
                                        buff,
                                        recv_buf,
                                        connection->sSocket);




                if(HTTPAuthorizationRequired(connection->yFileName))
                    connection->yCmdStage = HTTP_GET_SRV_AUTH_CHK;
                else
                    connection->yCmdStage = HTTP_GET_SRV_OPEN_FILE;
            }
            break;

        case HTTP_GET_SRV_AUTH_CHK:
            connection->yCmdStage = HTTP_GET_SRV_AUTH_CHK;
            /*
             * Search for the autherization field
             */
            while(buff && *buff != '\0' && (buff[0] != '\r' && buff[1] != '\n'))
            {
                buff = _GetNextLine(buff, recv_buf, connection->sSocket);

                if(!buff)
                    return FALSE;

                /*
                 * found a match
                 */
                if(!strncmp(buff, _httpMsgAuthorization, HTTP_MSG_AUTHORIZATION_LEN))
                {
                    BYTE    *encrypt_type;
                    BYTE    *encrypt_text;

                    buff += HTTP_MSG_AUTHORIZATION_LEN;

                    encrypt_type = buff;

                    while(*buff != ' ')
                        buff++;

                    *buff = '\0';
                    buff++;

                    encrypt_text = buff;

                    while(*buff != ' ' && *buff != '\r')
                        buff++;

                    *buff = '\0';

                    if(HTTPChkAuthorization(encrypt_type, encrypt_text))
                    {
                        connection->yCmdStage = HTTP_GET_SRV_OPEN_FILE;
                        return FALSE;
                    }
                }
            }
            connection->yCmdStage = HTTP_GET_SRV_AUTH_REQ;
            break;

        case HTTP_GET_SRV_OPEN_FILE:
            #ifndef _HTTP_EXTEND_SET
	        if(connection->yNumArg > 1)
	        {
                BYTE    *arg[HTTP_MAX_ARGS];
                BYTE    i;

                arg[0] = connection->yFileName;

                for(i = 1; i < connection->yNumArg; i++)
                    arg[i] = connection->pGetParams[i - 1].yData;

                /*
                 * callback to the main process for this
	             */
                HTTPExecCmd(arg, connection->yNumArg);

    	        connection->yFileType = HTTP_CGI;
            }
            #endif
            connection->yCmdStage = HTTP_GET_SRV_NOT_FOUND;

            if(connection->yFileType != HTTP_FILE_TYPE_UNKNOWN)
            {

    	        if(connection->lpFile)
	                FSfclose(connection->lpFile);

                connection->lpFile      = FSfopen(connection->yFileName, "rb");
	            connection->dwFilePtr   = 0;

	            if(connection->lpFile)
    	            connection->yCmdStage = HTTP_GET_SRV_HEADER;
                else    	            
    	            connection->yCmdStage = HTTP_GET_SRV_NOT_FOUND;
            }
            break;

        case HTTP_GET_SRV_AUTH_REQ:
            if(connection->lpFile)
	            FSfclose(connection->lpFile);

            connection->lpFile      = FSfopen(HTTP_UNATHORIZED_FILE, "rb");
	        connection->dwFilePtr   = 0;

            connection->yFileType   = _GetFileType(HTTP_UNATHORIZED_FILE);

            _SendAuthorizationHeader(connection);
            connection->yCmdStage   = HTTP_GET_SRV_FILE_TX;
            break;

        case HTTP_GET_SRV_NOT_FOUND:
            _CreateHTTPStatusLine(  HTTP_STATUS_NOT_FOUND,
                                    connection->TxBuf);

            strcat(connection->TxBuf, _httpMsgHdrEnd);
            connection->TxSize      = strlen(connection->TxBuf);
            connection->yCmdStage   = HTTP_GET_SRV_CLEANUP;

            connection->TxSize = _SendData(  connection->sSocket, connection->TxBuf, connection->TxSize);
            break;

        case HTTP_GET_SRV_HEADER:
            _SendReturnFileHeader(HTTP_STATUS_OK, connection);
            connection->yCmdStage   = HTTP_GET_SRV_FILE_TX;
            break;

        case HTTP_GET_SRV_FILE_TX:
            if(_SendFile(connection) != HTTP_SEND_FILE_CONT)
            {
                connection->yCmdStage   = HTTP_GET_SRV_CLEANUP;
            }
            break;

        case HTTP_GET_SRV_CLEANUP:
            return _FlushHeader(buff, recv_buf, connection->sSocket);

        default:
            break;

    }

    // testing 1 2 3
    return FALSE;
}
/*********************************************************************
 * Function:        static BOOL _ServicePOSTRequest(    LP_HTTP_CONNECT connection)
 *
 * PreCondition:    none
 *
 * Input:           connection  -  the connection param for the data
 *
 * Output:          Return TRUE if done, FALSE if more
 *
 * Side Effects:
 *
 * Overview:        Services the POST cmd by the client
 *
 * Note:            The user will have all of the upper level methods to handle
 *                  the callbacks with the param and values that are received
 *                  Example POST method format
 *
 *                  POST /login.html HTTP/1.1
 *                  Host: www.mysite.com
 *                  User-Agent: Moszilla/4.0
 *                  Content-Length: 27
 *                  Content-Type: application/x-www-form-urlencoded
 *
 *                  userif=joe&password=guessme
 ********************************************************************/
PRIVATE BOOL _ServicePOSTRequest(LP_HTTP_CONNECT connection)
{
    BYTE                    var_buf[HTTP_VAR_LEN + 1];
    BYTE                    value_buf[HTTP_VALUE_LEN + 1];
    BYTE                    *temp_ptr;
    BYTE                    *buf;
    BYTE                    *recv_buf;

    buf         = connection->RxPtr;
    recv_buf    = connection->RxBuf;

    switch(connection->yCmdStage)
    {
        case HTTP_POST_SRV_HEADER:
            /*
             * Get the file name for the data
             */
            _ParseFileName(&buf, &temp_ptr, &connection->yFileType, FALSE);

            strcpy(connection->yFileName, temp_ptr);

            /*
             * Get the content length
             */
            connection->dwFilePtr = 0;

            connection->yCmdStage = HTTP_POST_SRV_CONTENT;
            break;

        case HTTP_POST_SRV_CONTENT:

            while(buf[0] != '\r' && buf[1] != '\n')
            {
                buf = _GetNextLine(buf, recv_buf, connection->sSocket);

                if(!buf)
                    return FALSE;           // come back to check again

                if(!strncmp(buf, _httpMsgContentStr, HTTP_MSG_CONTENT_LEN))
                {
                    BYTE    temp = 0;

                    buf += HTTP_MSG_CONTENT_LEN;

                    while(*buf != '\r')
                    {
                        value_buf[temp++] = *buf++;
                    }

                    value_buf[temp]         = '\0';
                    /* doubling as size */
                    connection->dwFilePtr   = (DWORD)atoi(value_buf);

                    buf = _GetNextLine(buf, recv_buf, connection->sSocket);

                    if(!buf)
                        return FALSE;           // come back to check again
                }

                if(!strncmp(buf, _httpMsgExpectCont, HTTP_MSG_EXPECT_CONT))
                    connection->yCmdStage = HTTP_POST_SRV_CONTINUE;

            }
            /*
             * advance to the body
             */
            buf = _GetNextLine(buf, recv_buf, connection->sSocket);

            if(connection->yCmdStage == HTTP_POST_SRV_CONTENT)
                connection->yCmdStage = HTTP_POST_SRV_BODY;
            break;

        case HTTP_POST_SRV_CONTINUE:
            _CreateHTTPStatusLine(HTTP_STATUS_CONTINUE, connection->TxBuf);
            strcat(connection->TxBuf, _httpMsgHdrEnd);
            connection->TxSize      = strlen(connection->TxBuf);
            connection->yCmdStage   = HTTP_POST_SRV_BODY;

            connection->TxSize = _SendData(connection->sSocket, connection->TxBuf, connection->TxSize);
            break;

        case HTTP_POST_SRV_BODY:
            connection->HTTPStatus  =  HTTP_STATUS_OK;

            {
                BOOL    end = FALSE;

                connection->dwFilePtr = _GetVariableAndValue(   buf,
                                                                recv_buf,
                                                                var_buf,
                                                                value_buf,
                                                                connection->dwFilePtr,
                                                                connection->sSocket);


                if(!connection->dwFilePtr)
                {
                    end = TRUE;
                }

                connection->HTTPStatus = HTTPSendVar(   connection->yFileName,
                                                        var_buf,
                                                        value_buf,
                                                        end);




            }

            if(!connection->dwFilePtr || connection->HTTPStatus >= HTTP_STATUS_MOVED_PERMANENTLY)
                connection->yCmdStage = HTTP_POST_SEND_FILE;
            else
                connection->yCmdStage = HTTP_POST_SRV_GET_BODY_PARAM;
            break;

        case HTTP_POST_SRV_GET_BODY_PARAM:
            {
                DWORD    buffer_size;

                buffer_size             = (DWORD)strlen(recv_buf);
                connection->yCmdStage   = HTTP_POST_SRV_BODY;

                if(buffer_size < connection->dwFilePtr)
                {
                    int result;

                    buf         = &recv_buf[buffer_size];
                    buffer_size = HTTP_HTML_CMD_LEN - buffer_size;

                    #ifdef _BORLAND_BUILD
                    {
                        fd_set  readfds;
                        int     status;
                        struct  timeval to = {0,0};

                        FD_ZERO(&readfds);
                        FD_SET(connection->sSocket, &readfds);

                        status = select(0, &readfds, NULL, NULL, &to);

                        if((status == SOCKET_ERROR) || (!status))
                        {
                            connection->yCmdStage = HTTP_POST_SRV_GET_BODY_PARAM;
                            return FALSE;
                        }
                    }
                    #endif

                    result = recv(  connection->sSocket,
                                    buf,
                                    buffer_size,
                                    0);


                    if(!result)
                        connection->yCmdStage = HTTP_POST_SRV_GET_BODY_PARAM;
                }
            }
            break;
        case HTTP_POST_SEND_FILE:
        /*
         * Send a file if needed
         */

            connection->yFileType = _GetFileType(connection->yFileName);

            if(connection->yFileType == HTTP_FILE_TYPE_UNKNOWN)
                return TRUE; /* ERROR */


            if(connection->lpFile)
	            FSfclose(connection->lpFile);

            connection->lpFile      = FSfopen(connection->yFileName, "rb");
	        connection->dwFilePtr   = 0;

	        if(connection->lpFile)
            {
                connection->yCmdStage = HTTP_POST_SEND_HDR;
            }else
            {
                _CreateHTTPStatusLine(connection->HTTPStatus, connection->TxBuf);
                strcat(connection->TxBuf, _httpMsgHdrEnd);

                connection->TxSize = strlen(connection->TxBuf);
                connection->TxSize = _SendData(     connection->sSocket,
  	                                                connection->TxBuf,
    	                                            connection->TxSize);
                return TRUE;    /* OK */
            }
            break;

        case HTTP_POST_SEND_HDR:
            _SendReturnFileHeader(connection->HTTPStatus, connection);

            connection->yCmdStage   = HTTP_POST_SEND_FILE_BODY;
	        break;

        case HTTP_POST_SEND_FILE_BODY:
            if(_SendFile(connection) == HTTP_SEND_FILE_CONT)
                break;
                
            return TRUE;

    }

    return FALSE;   /* More work to be done */
}
/*********************************************************************
 * Function:        static void _ServicePUTRequest( LP_HTTP_CONNECT connection)
 *
 * PreCondition:    none.
 *
 * Input:           connection  -  the connection param for the data
 *
 * Output:          Return TRUE if done, FALSE if more
 *
 * Side Effects:    none.
 *
 * Overview:        Services the PUT cmd by the client
 *
 * Note:            Example:
 *                  PUT [filename] HTTP/1.x
 *                  X-FB-Mode: UploadPic
 *                  X-FB-Auth: crp:0123456789abcdef:0123456789abcdef
 *                  X-FB-Uploadpic.Meta.Filename: somefile.jpeg
 *                  Content-Length: 134332
 *
 *                  <ImageData>
 ********************************************************************/
PRIVATE BOOL _ServicePUTRequest(LP_HTTP_CONNECT connection)
{
    BYTE    *temp_ptr;
    BYTE    *buf;
    BYTE    *recv_buf;

    buf         = connection->RxPtr;
    recv_buf    = connection->RxBuf;

    switch(connection->yCmdStage)
    {
        case HTTP_PUT_SRV_HEADER:
            /*
             * Get the file name for the data
             */
            _ParseFileName(&buf, &temp_ptr, &connection->yFileType, FALSE);

            strcpy(connection->yFileName, temp_ptr);

            if(HTTPPUTUpload(connection->yFileName))
            {
                _CreateHTTPStatusLine(HTTP_STATUS_FORBIDDEN, connection->TxBuf);
                strcat(connection->TxBuf, _httpMsgHdrEnd);

                connection->TxSize = strlen(connection->TxBuf);
                connection->TxSize = _SendData( connection->sSocket,
                                                connection->TxBuf,
                                                connection->TxSize);
                return TRUE;
            }

            connection->yCmdStage = HTTP_PUT_UPLOAD_FILE;
            connection->dwFilePtr = 0;

            while(buf[0] != '\r' && buf[1] != '\n')
            {
                buf = _GetNextLine(buf, recv_buf, connection->sSocket);

                if(!buf)
                    return TRUE;

                if(!strncmp(buf, _httpMsgContentStr, HTTP_MSG_CONTENT_LEN))
                {
                    BYTE    size_buf[24];
                    BYTE    temp = 0;

                    buf += HTTP_MSG_CONTENT_LEN;

                    while(*buf != '\r')
                    {
                        size_buf[temp++] = *buf++;
                    }

                    size_buf[temp]          = '\0';
                    connection->dwFilePtr   = (UINT)atoi(size_buf);

                    buf = _GetNextLine(buf, recv_buf, connection->sSocket);
                }

                if(!strncmp(buf, _httpMsgExpectCont, HTTP_MSG_EXPECT_CONT))
                    connection->yCmdStage = HTTP_PUT_SRV_CONTINUE;

            }

            /*
             * advance to the body
             */
            buf = _GetNextLine(buf, recv_buf, connection->sSocket);
            break;
        case HTTP_PUT_SRV_CONTINUE:
            _CreateHTTPStatusLine(HTTP_STATUS_CONTINUE, connection->TxBuf);
            strcat(connection->TxBuf, _httpMsgHdrEnd);

            connection->TxSize      = strlen(connection->TxBuf);
            connection->TxSize      = _SendData(    connection->sSocket,
                                                    connection->TxBuf,
                                                    connection->TxSize);

            connection->yCmdStage   = HTTP_PUT_UPLOAD_FILE;
            break;

        case HTTP_PUT_UPLOAD_FILE:
            connection->HTTPStatus = HTTP_STATUS_CREATED;

            /*
             * File data
             */
            connection->lpFile = FSfopen(connection->yFileName, "wb");

            if(!connection->lpFile)
                connection->HTTPStatus = HTTP_STATUS_INTERNAL_SERVER_ERR;

            while(connection->dwFilePtr &&
                    (connection->HTTPStatus < HTTP_STATUS_MOVED_PERMANENTLY))
            {
                WORD    buf_size;
                int     result;

                if(connection->dwFilePtr < HTTP_HTML_CMD_LEN)
                    buf_size = connection->dwFilePtr;
                else
                    buf_size = HTTP_HTML_CMD_LEN;

                result = recv(  connection->sSocket,
                                recv_buf,
                                buf_size,
                                0);


                if((result == SOCKET_ERROR) || (!result))
                    connection->HTTPStatus = HTTP_STATUS_INTERNAL_SERVER_ERR;

                connection->dwFilePtr -= (DWORD)buf_size;

                if(connection->HTTPStatus != HTTP_STATUS_INTERNAL_SERVER_ERR)
                {
                    FSfwrite(recv_buf, buf_size, 1, connection->lpFile);
                }
            }

            if(connection->lpFile)
            {
                FSfclose(connection->lpFile);
                connection->lpFile = NULL;
            }
            connection->yCmdStage = HTTP_PUT_SEND_FILE;
            break;

        case HTTP_PUT_SEND_FILE:
            /*
             * Send a file if needed
             */
            if(HTTPPUTSendFile(connection->yFileName, connection->HTTPStatus))
            {
                connection->yFileType = _GetFileType(connection->yFileName);

                if(connection->yFileType == HTTP_FILE_TYPE_UNKNOWN)
                    return TRUE;


                if(connection->lpFile)
	                FSfclose(connection->lpFile);

                connection->lpFile      = FSfopen(connection->yFileName, "rb");
	            connection->dwFilePtr   = 0;

	            if(connection->lpFile)
                {
                    connection->yCmdStage = HTTP_PUT_SEND_HDR;
                }else
                {
                    _CreateHTTPStatusLine(connection->HTTPStatus, connection->TxBuf);
                    strcat(connection->TxBuf, _httpMsgHdrEnd);

                    connection->TxSize = strlen(connection->TxBuf);
                    connection->TxSize = _SendData(     connection->sSocket,
  	                                                    connection->TxBuf,
    	                                                connection->TxSize);
                    return TRUE;    /* OK */
                }
            }
            break;

            case HTTP_PUT_SEND_HDR:
                _SendReturnFileHeader(connection->HTTPStatus, connection);

                connection->yCmdStage   = HTTP_PUT_SEND_FILE_BODY;
	            break;

            case HTTP_PUT_SEND_FILE_BODY:
                if(_SendFile(connection) == HTTP_SEND_FILE_CONT)
                    break;

                return TRUE;

    }

    return FALSE;   /* More work to be done */
}
/*********************************************************************
 * Function:        static void _NotSupportedResponce(SOCKET socket)
 *
 * PreCondition:    none
 *
 * Input:           socket  - the handle to the TCP/IP connection
 *
 * Output:          none
 *
 * Side Effects:    none
 *
 * Overview:        If the HTTP command is not supported the server will
 *                  respond with the "Not Found" status message.
 *
 * Note:
 ********************************************************************/
PRIVATE void _NotSupportedResponce(LP_HTTP_CONNECT connection)
{
    _CreateHTTPStatusLine(HTTP_STATUS_NOT_FOUND, connection->TxBuf);
    strcat(connection->TxBuf, _httpMsgHdrEnd);

    connection->TxSize = strlen(connection->TxBuf);
    connection->TxSize = _SendData( connection->sSocket,
                                    connection->TxBuf,
                                    connection->TxSize);

}
/*********************************************************************
 * Function:        static void _SendHTTPStatusLine(BYTE status,
 *                                                  SOCKET socket)
 * PreCondition:    none
 *
 * Input:           status      - one of the enumeration status codes
 *                                (HTTP_STATUS_CODES)
 *                  socket      -  TCP/IP socket handle to send the data
 *
 * Output:          none.
 *
 * Side Effects:    none.
 *
 * Overview:        This function will send out a HTTP 1.1 status line.
 *
 * Note:            Status Line -
 *                  HTTP-Version SP Status Code SP Reason-Phrase CRLF
 ********************************************************************/
PRIVATE WORD _CreateHTTPStatusLine(HTTP_STATUS_CODES status, BYTE *buf)
{
    if(status >= HTTP_STATUS_NUMBER)
        return 0;


    strcpy(buf, _httpStatusMsgStart);
    strcat(buf, _httpStatusTbl[status].yStatusString);

    return (strlen(buf));


}
/*********************************************************************
 * Function:        static void _SendReturnFileHeader(LP_HTTP_CONNECT connection)
 *
 * PreCondition:    none
 *
 * Input:           connection      - The connection data
 *
 * Output:          none
 *
 * Side Effects:    none
 *
 * Overview:        The method will send out a status line, content type and
 *                  content length HTTP header
 *
 * Note:            Example:
 *                  HTTP/1.0 200 OK\r\n
 *                  Content-Type: text/html\r\n
 *                  Content-Length: xxx\r\n
 *                  \r\n
 ********************************************************************/
PRIVATE BOOL _SendReturnFileHeader(HTTP_STATUS_CODES status, LP_HTTP_CONNECT connection)
{
    BYTE            len_buf[12];
    long            file_size;

    if(connection->yFileType == HTTP_DYNAMIC_FILE_TYPE)
        connection->bProcess = TRUE;
    else
        connection->bProcess = FALSE;

    /*
     * header start
     */
    _CreateHTTPStatusLine(status, connection->TxBuf);
    /*
     * content type
     */
    strcat(connection->TxBuf, _httpMsgContentType);
    /*
     * file type
     */
    strcat(connection->TxBuf, _httpExtTbl[connection->yFileType].contentStr);
    
	if(!connection->bProcess)
	{
		/*
	     * content length
    	 */
	    strcat(connection->TxBuf, _httpMsgContentStr);

    	FSfseek(connection->lpFile, 0, SEEK_END);

	    file_size = FSftell(connection->lpFile);

        _FormatLengthASCII(file_size, len_buf);
        strcat(len_buf, "\r\n");
	    strcat(connection->TxBuf, len_buf);
	}
    /*
     * Header end
     */
    strcat(connection->TxBuf, _httpMsgHdrEnd);

    connection->TxSize = strlen(connection->TxBuf);


    connection->TxSize = _SendData(     connection->sSocket,
                                        connection->TxBuf,
                                        connection->TxSize);



    connection->dwFilePtr       = 0;
    connection->yGetFileStage   = HTTP_GET_FILE_READ;

    if(connection->TxSize)
        return FALSE;

    return TRUE;
}
/*********************************************************************
 * Function:        static void _SendAuthorizationHeader(LP_HTTP_CONNECT connection)
 *
 * PreCondition:    none
 *
 * Input:           connection      - The connection data
 *
 * Output:          none
 *
 * Side Effects:    none
 *
 * Overview:        Sends the client a header indicatinge that authroizatino is
 *                  required to view the file
 *
 * Note:            Example:
 *                  HTTP/1.0 401 Unauthorized\r\n
 *                  Content-Type: text/html\r\n
 *                  Content-Length: xxx\r\n
 *                  \r\n
 ********************************************************************/
PRIVATE BOOL _SendAuthorizationHeader(LP_HTTP_CONNECT connection)
{
    const BYTE      *ptr;
    BYTE            len_buf[12];
    long            file_size;

    /*
     * header start
     */
    _CreateHTTPStatusLine(HTTP_STATUS_UNAUTHORIZED, connection->TxBuf);

    /*
     * content type
     */
    strcat(connection->TxBuf, _httpMsgContentType);
    /*
     * file type
     */
    ptr = _httpExtTbl[connection->yFileType].contentStr;

    strcat(connection->TxBuf, ptr);

    /*
     * content length
     */
    strcat(connection->TxBuf, _httpMsgContentStr);

    FSfseek(connection->lpFile, 0, SEEK_END);

    file_size = FSftell(connection->lpFile);

    _FormatLengthASCII(file_size, len_buf);
    strcat(len_buf, "\r\n");

    strcat(connection->TxBuf, len_buf);

    strcat(connection->TxBuf, _httpMsgAuthenticate);
    strcat(connection->TxBuf, _httpMsgAuthMethod);
    /*
     * Header end
     */
    strcat(connection->TxBuf, _httpMsgHdrEnd);

    connection->TxSize = strlen(connection->TxBuf);

    connection->TxSize = _SendData(     connection->sSocket,
                                        connection->TxBuf,
                                        connection->TxSize);

    if(connection->yFileType == HTTP_DYNAMIC_FILE_TYPE)
        connection->bProcess = TRUE;
    else
        connection->bProcess = FALSE;

    connection->dwFilePtr       = 0;
    connection->yGetFileStage   = HTTP_GET_FILE_READ;

    if(connection->TxSize)
        return FALSE;

    return TRUE;

}
/*********************************************************************
 * Function:        static HTTP_SEND_FILE_STATUS _SendFile(LP_HTTP_CONNECT connection)
  *
 * PreCondition:    none.
 *
 * Input:           connection  - Client connection data
 *
 * Output:          status of the stage of the file being sent
 *
 * Side Effects:    none.
 *
 * Overview:        Sends a file to the client
 *
 * Note:
 ********************************************************************/
PRIVATE HTTP_SEND_FILE_STATUS _SendFile(LP_HTTP_CONNECT connection)
{
    int     send_index;
    BYTE    *buf;
#ifdef HTTP_SMALL_READ
    BYTE    read_buff[HTTP_READ_SIZE];
#endif
    int     read_idx;
    int     read_size;

    send_index  = 0;
    read_idx    = 0;
    read_size   = -1;
    buf         = connection->TxBuf;

    if(FSfseek(connection->lpFile, connection->dwFilePtr, SEEK_SET))
    {
        FSfclose(connection->lpFile);
        connection->lpFile = NULL;
        return HTTP_SEND_FILE_CLOSE;
    }
    
    // decide if we are able to send out raw data
    if(!connection->bProcess)
    {
        while(1)
        {

            read_size = HTTP_SEND_BUF_LEN;

            if(read_size > HTTP_READ_SIZE)
                read_size = HTTP_READ_SIZE;

            read_size   = FSfread(buf, 1, read_size, connection->lpFile);

            if(!read_size)
            {
                FSfclose(connection->lpFile);
                connection->lpFile = NULL;
                return HTTP_SEND_FILE_CLOSE;
            }


            connection->TxSize = _SendData(connection->sSocket, connection->TxBuf, read_size);
            connection->dwFilePtr   += (DWORD)read_size;

            if(connection->TxSize)
                return HTTP_SEND_FILE_CONT;

        }
       
    }else
    {
        while(1)
        {
            BOOL tx;

            tx = FALSE;

            if((connection->yGetFileStage != HTTP_GET_FILE_VAR)
                && (read_idx >= read_size))
            {

                read_size = HTTP_SEND_BUF_LEN - send_index;

                if(read_size > HTTP_READ_SIZE)
                    read_size = HTTP_READ_SIZE;

                read_size   = FSfread(&read_buff, 1, read_size, connection->lpFile);
                read_idx    = 0;

                if(!read_size)
                {
                    if(send_index)
                        connection->TxSize = _SendData(connection->sSocket, connection->TxBuf, send_index);

                    FSfclose(connection->lpFile);
                    connection->lpFile = NULL;

                    return HTTP_SEND_FILE_CLOSE;
                }

            }


            switch(connection->yGetFileStage)
            {
                case HTTP_GET_FILE_READ:
                    if(read_buff[read_idx] == HTTP_VAR_ESC_CHAR)
                        connection->yGetFileStage = HTTP_GET_FILE_DLE;
                    else
                        tx = TRUE;
                    break;

                case HTTP_GET_FILE_DLE:
                    if(read_buff[read_idx] == HTTP_VAR_ESC_CHAR)
                    {
                        tx = TRUE;
                        connection->yGetFileStage = HTTP_GET_FILE_READ;
                    }else
                    {
                        connection->yVariable = (read_buff[read_idx] - '0') << 4;
                        connection->yGetFileStage = HTTP_GET_FILE_HANDLE;
                    }
                    break;

                case HTTP_GET_FILE_HANDLE:
                    connection->yVariable |= (read_buff[read_idx] - '0');
                    connection->yGetFileStage = HTTP_GET_FILE_VAR;
                    connection->wVarRef = HTTP_GET_FILE_START_VAR;
                    break;

                case HTTP_GET_FILE_VAR:
                    connection->wVarRef = HTTPGetVar(   connection->yVariable,
                                                        connection->wVarRef,
                                                        &read_buff[read_idx]);
                    tx = TRUE;
                    if(connection->wVarRef == HTTP_GET_FILE_EOF_VAR)
                        connection->yGetFileStage = HTTP_GET_FILE_READ;
                    break;


            }

            if(tx)
                buf[send_index++] = read_buff[read_idx];


            if(connection->yGetFileStage != HTTP_GET_FILE_VAR)
            {
                read_idx++;
                connection->dwFilePtr++;
            }

            if(send_index >= HTTP_SEND_BUF_LEN)
            {
                connection->TxSize = _SendData(connection->sSocket, connection->TxBuf, send_index);

                if(connection->TxSize)
                    return HTTP_SEND_FILE_CONT;

                send_index = 0;
            }

        }

    }

 }
/*********************************************************************
 * Function:        static BOOL _GetFileName(   BYTE **instream,
 *                                              BYTE **filename,
 *                                              BYTE *type
 *                                              BOOL get)
 *
 * PreCondition:    none
 *
 * Input:           instream    - a pointer to a pointer of the data
 *                  filename    - a pointer to the "holder" of the filename
 *                  type        - a pointer to the "holder" of the type
 *                  get         - if it is a GET command
 *
 * Output:          none
 *
 * Side Effects:
 *
 * Overview:        This function parses out the filename that is requested
 *                  if no filename is give a default is loaded.  The function will
 *                  return a true if data is present
 *
 * Note:
 ********************************************************************/
PRIVATE BOOL _ParseFileName(BYTE **instream, BYTE **filename_ptr, BYTE *type, BOOL get)
{
    BYTE    stage;
    BYTE    apha;
    BOOL    result = FALSE;
    BYTE    *buf;
    BYTE    *filename;

    enum
    {
        GET_FILENAME_SEARCH,
        GET_FILENAME_FORMAT
    };

    buf         = *instream;
    stage       = GET_FILENAME_SEARCH;
    apha        = *buf;
    filename    = NULL;

    while(apha != ' ' && apha != '\0' && apha != '\r' && apha != '\n'
            && apha != '&' && apha != '=')
    {

        if((apha == '?') && (get))
        {
            result = TRUE;
            break;
        }

        switch(stage)
        {
            case GET_FILENAME_SEARCH:
                filename    = buf;
                apha        = *buf;
                stage       = GET_FILENAME_FORMAT;

            case GET_FILENAME_FORMAT:
                apha = *buf;

                switch(apha)
                {
                    case '+':
                        *buf = ' ';
                        break;

                    /*
                     * Only interested in the file name - not the path
                     */
                    case '/':
                    case '\\':
                        stage = GET_FILENAME_SEARCH;
                        break;


                }
                break;

        }

        buf++;
        apha = *buf;
    }



    *type = _GetFileType(filename);

    if(*type != HTTP_FILE_TYPE_UNKNOWN)
    {
        *buf = '\0';
        buf++;
    }else
    {
        filename    = (BYTE *)_httpDefaultFileName;
        *type       = _GetFileType(filename);
    }


    *instream       = buf;
    *filename_ptr   = filename;

    return result;
}
/*********************************************************************
 * Function:        static BYTE _GetFileType(BYTE *filename)
 *
 * PreCondition:    none
 *
 * Input:           filename - The file name
 *
 * Output:          The file type of the filename.
 *
 * Side Effects:    none.
 *
 * Overview:        The file name contains an extension that is used
 *                  to fill the content-type header field of the HTTP.
 *                  This function gets the index in the table to be able
 *                  to get the proper string to fill that header field with.
 *
 * Note:
 ********************************************************************/
PRIVATE BYTE _GetFileType(BYTE *filename)
{
    BYTE    ext[9];
    BYTE    i;


    while((*filename != '\0') && (*filename != ' '))
    {
        if(*filename == '.')
        {
            filename++;
            strncpy(ext, filename, 8);
			ext[8] = 0;
            break;
        }

        filename++;
    }

    if((*filename != '\0') && (*filename != ' '))
    {

        strupr((char *)ext);

        for(i = 0; i < HTTP_NUM_FILE_TYPES; i++)
        {
            if(!strncmp(ext, _httpExtTbl[i].fileExtStr, HTTP_FILE_EXT_LEN))
                return i;
        }

    }

    return HTTP_FILE_TYPE_UNKNOWN;
}
/*********************************************************************
 * Function:        static BOOL _ParseParam(    BYTE **instream,
 *                                              BYTE **param_ptr)
 *
 * PreCondition:    none.
 *
 * Input:           instream    -  a pointer to the input data
 *                  param_ptr   -  a pointer to the param string
 *
 * Output:          param_ptr will be pointing to a param that is located
 *                  in the instream and the intream pointer is advanced to the
 *                  next param
 *
 * Side Effects:    instream and param_ptr are modified.
 *
 * Overview:        The data that is in the instream  is parsed so that
 *                  the param_ptr points to a parameter that will be passed to
 *                  a higher level method.
 *
 * Note:            Example:
 *                  Input
 *                  instream:   name=Sean&job=MicroChip
 *                  param_ptr:  NULL
 *
 *                  Output:
 *                  instream:   Sean&job=MicroChip
 *                  param_ptr:  name
 ********************************************************************/
PRIVATE BOOL _ParseParam(BYTE **instream, BYTE **param_ptr)
{
    BYTE    apha;
    BYTE    *buf;
    BOOL    result = FALSE;
    BYTE    *param;
    enum
    {
        PARSE_PARAM_SEARCH,
        PARSE_PARAM_ARG,
        PARSE_PARAM_ARG_FORMAT
    };

    buf     = *instream;
    apha    = *buf;
    param   = buf;

    if((apha == ' ') || (apha == '\0'))
        return result;
        
    while(apha != '\0' && apha != '\r' && apha != '\n' && apha != '?')
    {
        if(apha == '&' || apha == '=' || apha == ' ') 
        {
            result = TRUE;
            break;
        }
        buf++;
        apha    = *buf;
    }

    *buf = '\0';
    buf++;

    *param_ptr  = param;
    *instream   = buf;

    return result;
}
/*********************************************************************
 * Function:        static HTTP_STATUS_CODES _ParseHTTPGETHeader(   BYTE *filename,
 *                                                                  BYTE *type,
 *                                                                  BYTE *params,
 *                                                                  BYTE *max_params,
 *                                                                  BYTE *buf,
 *                                                                  BYTE *recv_buf,
 *                                                                  SOCKET socket)
 *
 * PreCondition:    none
 *
 * Input:           filename    - pointer to where the filename will be
 *                  type        - pointer to the file type
 *                  params      - pointer to the pasted parameters
 *                  max_params  - max number of parameters
 *                  buf         - pointer to the Rx data
 *                  recv_buf    - buffer to put the Rx data
 *                  socket      - handle of the client connection
 *
 * Output:          Status of the HTTP request.
 *
 * Side Effects:    none
 *
 * Overview:        Takes the HTTP GET request and parses the header
 *
 * Note:            GET filename?args[0]=args[1]&....args[num_args-2]=args[num_args-1]
 *                  <other header information>
 *                  Authorization: method username:password
 ********************************************************************/
PRIVATE HTTP_STATUS_CODES _ParseHTTPGETHeader(BYTE *filename, BYTE *type, LP_HTTP_GET_PARAM params, BYTE *max_params, BYTE *buf, BYTE *recv_buf, SOCKET socket)
{
    BYTE                num_params;
    HTTP_STATUS_CODES   status = HTTP_STATUS_OK;
    BYTE                *temp_ptr;

    num_params = 0;

    /*
     * Get the filname and any params
     */
    if(_ParseFileName(&buf, &temp_ptr, type, TRUE))
    {

        strcpy(filename, temp_ptr);

        #ifndef _HTTP_EXTEND_SET
        while(_ParseParam(&buf, &temp_ptr) && (num_params < *max_params))
        {
             strcpy(params[num_params].yData, temp_ptr);
             num_params++;
        }
        #else
        {
            BOOL    ext_present = FALSE;

            num_params = 0;

            while(_ParseParam(&buf, &temp_ptr))
            {

                 ext_present = TRUE;

                 strcpy(params[num_params].yData, temp_ptr);
                 num_params++;

                 if(num_params == 2)
                 {
                    HTTPSendVar(filename, params[0].yData, params[1].yData, FALSE);
                    num_params = 0;
                 }
            }

            if(ext_present)
                HTTPSendVar(filename, params[0].yData, params[1].yData, TRUE);
        }
        #endif

    }else
    {
        strcpy(filename, temp_ptr);
    }

    *max_params = num_params + 1;

    return status;
}
/*********************************************************************
 * Function:        static BYTE *_GetNextLine(  BYTE *buf,
 *                                              BYTE *recv_buf,
 *                                              SOCKET socket)
 *
 * PreCondition:    none
 *
 * Input:           buf         - a pointer the current location in the buffer
 *                  recv_buf    - a pointer to the Rx buffer for TCP/IP
 *                  socket      - a handle to the TCP/IP connection
 *
 * Output:          A pointer to the new line
 *
 * Side Effects:    none.
 *
 * Overview:        This method will return a pointer to the next line, if present
 *                  from the HTTP header
 *
 * Note:            Example.
 *
 *                  Input: "Content-Type: text/plain\r\nContent-Length:24\r\n\r\n"
 *                  Output: "Content-Length:24\r\n\r\n"
 ********************************************************************/
PRIVATE BYTE *_GetNextLine(BYTE *buf, BYTE *recv_buf, SOCKET socket)
{
    BOOL    found = FALSE;

    while(!found)
    {

        while(*buf != '\r' && *buf != '\0')
            buf++;

        if(*buf == '\0')
        {
            int result;

            memset(recv_buf, 0, HTTP_HTML_CMD_LEN);     // clear the buffer

            result = recv(socket, recv_buf, HTTP_HTML_CMD_LEN, 0);

            if((!result) || (result == SOCKET_ERROR))
                return NULL;

            buf = recv_buf;

        }else
        {
            if(buf[1] == '\0')
            {
                int result;

                memset(recv_buf, 0, HTTP_HTML_CMD_LEN);     // clear the buffer

                result = recv(socket, recv_buf, HTTP_HTML_CMD_LEN, 0);

                if(!result || result == SOCKET_ERROR)
                    return NULL;

                buf = recv_buf;

            }else
            {
                buf++;
            }

            if(*buf == '\n')
            {
                BYTE    size;

                buf++;
                strcpy(recv_buf, buf);

                size    = strlen(recv_buf);
                buf     = &recv_buf[size];
                size    = HTTP_HTML_CMD_LEN - size;

                if(size)
                {
                    int result;

                    #ifdef _BORLAND_BUILD
                    {
                        fd_set  readfds;
                        int     status;
                        struct  timeval to = {0,0};

                        FD_ZERO(&readfds);
                        FD_SET(socket, &readfds);

                        status = select(0, &readfds, NULL, NULL, &to);

                        if((status == SOCKET_ERROR) || (!status))
                            return recv_buf;
                    }
                    #endif

                    memset(buf, 0, size);     // clear the buffer

                    result = recv(socket, buf, (int)size, 0);

                    if(!result || result == SOCKET_ERROR)
                        return NULL;
                    
                    buf[result] = '\0';

                }
                found = TRUE;
            }
        }
    }


    return recv_buf;
}
/*********************************************************************
 * Function:        static WORD _GetVariableAndValue(   BYTE *buf,
 *                                                      BYTE *recv_buf,
 *                                                      BYTE *var,
 *                                                      BYTE *value,
 *                                                      WORD size,
 *                                                      SOCKET socket)
 *
 * PreCondition:    User should have the defines HTTP_VAR_LEN and HTTP_VALUE_LEN
 *                  correctly defined to handle the incoming data
 *
 * Input:           buf         - pointer to the current location of data
 *                  recv_buf    - pointer to the buffer for the TCP/IP data
 *                  var         - pointer to the buffer for the variable name
 *                  value       - pointer to the buffer for the value data
 *                  size        - size of the remaining bytes in the data packet
 *                  socket      - handle to the TCP/IP connection
 *
 * Output:          The new remaining size of the data packet.
 *
 * Side Effects:    none.
 *
 * Overview:        This function takes the data that is passed in the POST method
 *                  and breaks it into variable and values to present the the higher level
 *                  methods.  Any special characters, begining with '%', will be formated
 *                  before passing.
 *
 * Note:            Example
 *                  Input: var1=value1&var2=value2&......var(n)=value(n)
 *                  var1 and value1 will be passed to a higher level method to
 *                  evaluate.
 *                  New string will look like:
 *                  var2=value2&......var(n)=value(n)
 ********************************************************************/
PRIVATE WORD _GetVariableAndValue(BYTE *buf, BYTE *recv_buf, BYTE *var, BYTE *value, WORD size, SOCKET socket)
{
    WORD    idx;
    WORD    temp;

    if(!size)
        return size;

    temp = size;

    idx = 0;
    while(*buf != '=' && *buf != '\0' && idx < HTTP_VAR_LEN)
    {
        var[idx] = *buf;
        idx++;
        buf++;
        size--;

    }

    if(*buf == '\0' || !size || idx >= HTTP_VAR_LEN)
        return 0;

    var[idx] = '\0';
    _FormatParam(var);

    buf++;
    size--;
    idx = 0;

    while(*buf != '&' && *buf != '\0' && size && idx < HTTP_VALUE_LEN)
    {
        value[idx] = *buf;
        idx++;
        buf++;
        size--;
    }

    if(idx >= HTTP_VALUE_LEN)
        return 0;

    value[idx] = '\0';
    _FormatParam(value);

    if((*buf != '\0') && (size))
        buf++;

    strcpy(recv_buf, buf);

    if(size)
        size--;

    return size;
}
/*********************************************************************
 * Function:            static void _FormatParam(BYTE *param)
 *
 * PreCondition:        none
 *
 * Input:               param - a pointer the data that needs to be formated
 *
 * Output:              The passes param will be formated without the special
 *                      characters
 *
 * Side Effects:        none
 *
 * Overview:            This method takes a parameter that is passed in a
 *                      HTTP method and formats, taking out the special characters
 *                      and replacing them with the appropriate ones.
 *
 * Note:                Example
 *                      Input: %34Sean%39s+method%34
 *                      Output: "Sean's method"
 ********************************************************************/
PRIVATE void _FormatParam(BYTE *param)
{
    BYTE    apha;
    BYTE    *buf;
    BYTE    non_apha;

    buf             = param;
    apha            = *param;
    non_apha        = 0;

    while(apha != '\0')
    {
        if(apha != '%' && !non_apha)
        {
            if(apha != '+')
                *buf = apha;
            else
                *buf = ' ';
                                
            buf++;
        }else
        {
            switch(non_apha)
            {
                case 0:
                    non_apha++;
                    break;

                case 1:
                    *buf = ((apha & 0x0F) << 4);
                    non_apha++;
                    break;

                case 2:
                    *buf |= (apha & 0x0F);
                    buf++;
                    non_apha = 0;
                    break;
            }
        }

        param++;
        apha = *param;
    }

    *buf = '\0';
}
/*********************************************************************
 * Function:        BOOL _AddConnection(    SOCKET socket,
 *                                          LPSOCKADDR lpsockaddr,
 *					                        int naddrlen)
 *
 * PreCondition:    that a socket has been accepted
 *
 * Input:           socket      - hoandle to the TCP/IP connection
 *                  lpsockaddr  - a pointer to the socket address
 *                  naddrlen    - length of the socket address
 *
 * Output:          true if success
 *
 * Side Effects:    memory on the heap
 *
 * Overview:        Creates a connection structure that is sotred in a list
 *
 * Note:
 ********************************************************************/
PRIVATE BOOL _AddConnection(SOCKET socket)
{
    LP_HTTP_CONNECT lpthis;
    BYTE num;

    for(num = 0; num < HTTP_MAX_CONNECTIONS; num++)
        if(_httpConnection[num].open == HTTP_CONNECTION_CLOSED)
            break;

    if(num >= HTTP_MAX_CONNECTIONS)
        return FALSE;

    lpthis = &_httpConnection[num];

    /*
     * Init everything
	 */
	lpthis->sSocket         = socket;
    lpthis->lpFile          = NULL;
    lpthis->fFullResponse   = FALSE;
    lpthis->dwFilePtr       = 0;
    lpthis->yFileType       = HTTP_FILE_TYPE_UNKNOWN;
    lpthis->bProcess        = FALSE;
    lpthis->yCmd            = HTTP_INVALID_CMD;
    lpthis->TxSize          = 0;
    lpthis->yNumArg         = 0;
    lpthis->open            = HTTP_CONNECTION_OPEN;

    memset(lpthis->TxBuf, 0, (HTTP_SEND_BUF_LEN + 1));
    memset(lpthis->RxBuf, 0, (HTTP_HTML_CMD_LEN + 1));

    return TRUE;
}
/*********************************************************************
 * Function:        static BOOL _CloseConnection(LP_HTTP_CONNECT connection)
 *
 * PreCondition:    none
 *
 * Input:           connection - a pointer the HTTP_CONNECT sturture
 *
 * Output:          TRUE if success
 *
 * Side Effects:    none
 *
 * Overview:        The method will close the socket connection along with
 *                  a file if open.  The memory used for the structure and any pointers
 *                  that may have allocated memory on the heap will be freed.
 *
 * Note:
 ********************************************************************/
PRIVATE BOOL _CloseConnection(LP_HTTP_CONNECT connection)
{
    if(!connection)
        return FALSE;

    closesocket(connection->sSocket);

    if(connection->lpFile)
        FSfclose(connection->lpFile);

    connection->open    = HTTP_CONNECTION_CLOSED;

    _numberOfAccepts--;

    return TRUE;
}
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
PRIVATE LP_HTTP_CONNECT _GetConnection(BYTE idx)
{
    BYTE i, j;

    if(idx >= HTTP_MAX_CONNECTIONS)
        return NULL;

    for(i = 0, j = 0; i < HTTP_MAX_CONNECTIONS; i++)
    {
        if(_httpConnection[i].open == HTTP_CONNECTION_OPEN)
        {
            if(j == idx)
                return &_httpConnection[i];

            j++;
        }
    }

    return NULL;
}
/*********************************************************************
 * Function:        static void _FlushConnection(   LP_HTTP_CONNECT connection,
 *                                                  BYTE *buf)
 *
 * PreCondition:    none
 *
 * Input:           connection  - TCP/IP data structure
 *                  buf         - receive buffer for teh TCP/IP stack
 *
 * Output:          TCP/IP connection is closed
 *
 * Side Effects:    none
 *
 * Overview:        This flushes the buffer and closes the connection
 *
 * Note:
 ********************************************************************/
PRIVATE void _FlushConnection(LP_HTTP_CONNECT connection, BYTE *buf)
{
    int result;

    #ifdef _BORLAND_BUILD
    {
        fd_set  readfds;
        int     status;
        struct  timeval to = {0,0};

        FD_ZERO(&readfds);
        FD_SET(connection->sSocket, &readfds);

        status = select(0, &readfds, NULL, NULL, &to);

        if((status == SOCKET_ERROR) || (!status))
            return;
    }
    #endif

    result = recv(connection->sSocket, buf, HTTP_HTML_CMD_LEN, 0);

    while(result && result != SOCKET_ERROR)
    {
        #ifdef _BORLAND_BUILD
        {
            fd_set  readfds;
            int     status;
            struct  timeval to = {0,0};

            FD_ZERO(&readfds);
            FD_SET(connection->sSocket, &readfds);

            status = select(0, &readfds, NULL, NULL, &to);

            if((status == SOCKET_ERROR) || (!status))
                return;
        }
        #endif

        result = recv(connection->sSocket, buf, HTTP_HTML_CMD_LEN, 0);
    }

    #ifndef _BORLAND_BUILD
    if(!result)
        return;
    #endif

    _CloseConnection(connection);
}
/*********************************************************************
 * Function:        static void _FlushHeader(   BYTE *buf,
 *                                              BYTE *recv_buf,
 *                                              SOCKET socket)
 *
 * PreCondition:    none
 *
 * Input:           buf         - pointer to the current location of the receive
 *                                buffer
 *                  recv_buf    - pointer to the receve buffer
 *                  socket      - TCL/IP socket handle
 *
 * Output:          Header is flushed from the packet
 *
 * Side Effects:    none
 *
 * Overview:        Gets the HTTP header from the TCP/IP stack
 *
 * Note:           
 ********************************************************************/
PRIVATE BOOL _FlushHeader(BYTE *buf, BYTE *recv_buf, SOCKET socket)
{
    while(buf[0] != '\r' && buf[1] != '\n' && buf)
    {
        buf = _GetNextLine(buf, recv_buf, socket);

        if(!buf)
            return FALSE;

        if(*buf == '\0')
            return FALSE;
    }

    return TRUE;
}
/*********************************************************************
 * Function:        static int _SendData(   SOCKET socket,
 *                                          const BYTE *buf,
 *                                          int len)
 *
 * PreCondition:    none
 *
 * Input:           socket      - TCP/IP handle
 *                  buf         - buffer of bytes to transmit
 *                  len         -  number of bytes to send
 *
 * Output:          Number of bytes sent
 *
 * Side Effects:    none
 *
 * Overview:        This function is called to send out TCP/IP packet
 *
 * Note:
 ********************************************************************/
PRIVATE int _SendData(SOCKET socket, BYTE *buf, int len)
{
    int result;
	int i, size;


    result = send(  socket,
                    buf,
                    len,
                    0);

    if(result == SOCKET_ERROR)
		return result;

	if(result <= 0)					// does not include SOCKET ERROR
		return len;					// re-xmit the packet

	
	size = len - result;			// bytes left in the buffer
	
	if(size)
	{
		for(i = 0; i < size; i++)		// move the buffer 
			buf[i] = buf[result + i];	
	}

	return size;
}
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
PRIVATE void _FormatLengthASCII(DWORD size, BYTE *buffer)
{
    DWORD   remainder;
    UINT    i, k;

    if(!size)
    {
        buffer[0] = '0';
        buffer[1] = 0;
        return;
    }

    // Example size = 25348957
    i = 0;

    while(size)
    {
        remainder = size % 10;
        remainder |= 0x30;

        buffer[i++] = (BYTE)remainder;

        size /= 10;
    }

    // Example buffer = 75984352

    // reverse everything
    for(k = 0; k < (i >> 1); k++)
    {
        BYTE temp;

        temp = buffer[k];
        buffer[k] = buffer[(i - 1) - k];
        buffer[(i - 1) - k] = temp;
    }

    // Example buffer = 25348957

    buffer[i] = 0;
    // Example buffer = "25348957"
}
