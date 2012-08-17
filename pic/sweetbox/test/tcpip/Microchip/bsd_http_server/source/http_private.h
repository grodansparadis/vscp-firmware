/********************************************************************
 * FileName:		http_private.h
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
#ifndef _HTTP_PRIVATE_HEADER_FILE
#define _HTTP_PRIVATE_HEADER_FILE

/********************************************************************
 * HTTP Private defines
 ********************************************************************/
#define HTTP_FILE_EXT_LEN           3
#define HTTP_VAR_ESC_CHAR           '%'
#define HTTP_CONTENT_LENGTH         32
#define HTTP_STATUS_STR_LEN         32
#define HTTP_CMD_LEN                10
#define HTTP_GET_FILE_START_VAR     0
#define HTTP_CONNECTION_OPEN        0xFF
#define HTTP_CONNECTION_CLOSED      0x00
/********************************************************************
 * HTTP Private Emunerations
 ********************************************************************/
typedef enum
{
    HTTP_GET_CMD,
    HTTP_POST_CMD,
    HTTP_PUT_CMD,
    HTTP_HEAD_CMD,
    HTTP_OPTIONS_CMD,
    HTTP_PROPFIND_CMD,
    HTTP_NOT_SUPPORTED,
    HTTP_INVALID_CMD
}HTTP_REQ_CMD;

enum
{
    HTTP_GET_SRV_SEARCH,
    HTTP_GET_SRV_NOT_FOUND,
    HTTP_GET_SRV_AUTH_CHK,
    HTTP_GET_SRV_OPEN_FILE,
    HTTP_GET_SRV_FLUSH_HEADER,
    HTTP_GET_SRV_AUTH_REQ,
    HTTP_GET_SRV_HEADER,
    HTTP_GET_SRV_FILE_TX,
    HTTP_GET_SRV_CLEANUP,
};

enum
{
    HTTP_POST_SRV_HEADER,
    HTTP_POST_SRV_CONTINUE,
    HTTP_POST_SRV_BODY,
    HTTP_POST_SRV_CONTENT,
    HTTP_POST_SRV_GET_BODY_PARAM,
    HTTP_POST_SEND_FILE,
    HTTP_POST_SEND_HDR,
    HTTP_POST_SEND_FILE_BODY
};

enum
{
    HTTP_PUT_SRV_HEADER,
    HTTP_PUT_SRV_CONTINUE,
    HTTP_PUT_UPLOAD_FILE,
    HTTP_PUT_SEND_FILE,
    HTTP_PUT_SEND_HDR,
    HTTP_PUT_SEND_FILE_BODY
};

typedef enum
{
    HTTP_SEND_FILE_CONT,
    HTTP_SEND_FILE_CLOSE,
    HTTP_SEND_FILE_ERROR
}HTTP_SEND_FILE_STATUS;

/********************************************************************
 * HTTP Private structures
 ********************************************************************/
typedef struct
{
    BYTE yData[HTTP_VAR_LEN];
}HTTP_GET_PARAM;

typedef HTTP_GET_PARAM *LP_HTTP_GET_PARAM;

typedef struct tagHTTP_CONNECT
{
    SOCKET              sSocket;
    int                 TxSize;
    FSFILE		        *lpFile;
    BYTE                *RxPtr;
    HTTP_GET_PARAM      pGetParams[HTTP_MAX_ARGS];
    BOOL		        fFullResponse;
    BOOL                bProcess;
    DWORD		        dwFilePtr;
    WORD                wVarRef;
    HTTP_STATUS_CODES   HTTPStatus;
    HTTP_REQ_CMD        yCmd;
    BYTE                yFileName[HTTP_FILENAME_LEN];
    BYTE                RxBuf[HTTP_HTML_CMD_LEN + 1];
    BYTE                TxBuf[HTTP_SEND_BUF_LEN + 1];
    BYTE		        yNumArg;
    BYTE                yCmdStage;
    BYTE                yGetFileStage;
    BYTE                yVariable;
    BYTE                yFileType;
    BYTE                open;
    BYTE                pad[2];
}HTTP_CONNECT;

typedef HTTP_CONNECT *LP_HTTP_CONNECT;

typedef struct
{
    char            commandString[HTTP_CMD_LEN];
    BYTE            commandLen;
    HTTP_REQ_CMD    cmd;
}HTTP_CMD_TBL;

typedef HTTP_CMD_TBL *LP_HTTP_CMD_TBL;

typedef struct
{
    BYTE fileExtStr[HTTP_FILE_EXT_LEN + 1];
    BYTE contentStr[HTTP_CONTENT_LENGTH + 1];
}HTTP_FILE_EXT;



typedef struct
{
    BYTE    yStatusString[HTTP_STATUS_STR_LEN];
}HTTP_STATUS_CODE_TBL;

typedef HTTP_STATUS_CODE_TBL *LP_HTTP_STATUS_CODE_TBL;


/********************************************************************
 * HTTP Private methods
 ********************************************************************/
#ifdef _HTTP_PRIVATE_FUNC
PRIVATE BOOL _AddConnection(SOCKET socket);
PRIVATE BOOL _AcceptConnection(void);
PRIVATE LP_HTTP_CONNECT _GetConnection(BYTE idx);
PRIVATE HTTP_REQ_CMD _GetHTTPCmd(BYTE **recv_buf, LP_HTTP_CONNECT connection);
PRIVATE BOOL _ServiceGETRequest(LP_HTTP_CONNECT connection);
PRIVATE BOOL _ServicePOSTRequest(LP_HTTP_CONNECT connection);
PRIVATE HTTP_SEND_FILE_STATUS _SendFile(LP_HTTP_CONNECT connection);
PRIVATE BOOL _ParseFileName(BYTE **instream, BYTE **filename_ptr, BYTE *type, BOOL get);
PRIVATE BOOL _ParseParam(BYTE **instream, BYTE **param_ptr);
PRIVATE BOOL _CloseConnection(LP_HTTP_CONNECT connection);
PRIVATE BYTE *_GetNextLine(BYTE *buf, BYTE *recv_buf, SOCKET socket);
PRIVATE WORD _GetVariableAndValue(BYTE *buf, BYTE *recv_buf, BYTE *var, BYTE *value, WORD size, SOCKET socket);
PRIVATE void _FormatParam(BYTE *param);
PRIVATE BYTE _GetFileType(BYTE *filename);
PRIVATE BOOL _SendReturnFileHeader(HTTP_STATUS_CODES status, LP_HTTP_CONNECT connection);
PRIVATE WORD _CreateHTTPStatusLine(HTTP_STATUS_CODES status, BYTE *buf);
PRIVATE HTTP_STATUS_CODES _ParseHTTPGETHeader(BYTE *filename, BYTE *type, LP_HTTP_GET_PARAM params, BYTE *max_params, BYTE *buf, BYTE *recv_buf, SOCKET socket);
PRIVATE BOOL _SendAuthorizationHeader(LP_HTTP_CONNECT connection);
PRIVATE BOOL _ServicePUTRequest(LP_HTTP_CONNECT connection);
PRIVATE void _NotSupportedResponce(LP_HTTP_CONNECT connection);
PRIVATE void _FlushConnection(LP_HTTP_CONNECT connection, BYTE *buf);
PRIVATE BOOL _FlushHeader(BYTE *buf, BYTE *recv_buf, SOCKET socket);
PRIVATE int _SendData(SOCKET socket, BYTE *buf, int len);
PRIVATE void _FormatLengthASCII(DWORD size, BYTE *buffer);
#endif
#endif

