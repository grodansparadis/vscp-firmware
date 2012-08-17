/********************************************************************
 * FileName:		http.h
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
#ifndef _HTTP_HEADER_FILE
#define _HTTP_HEADER_FILE

#include "generic.h"
#include "bsd_tcp_ip\tcpip.h"
#include "fat16\fat.h"

#define HTTP_PORT                   ((WORD)80)
#define HTTP_GET_FILE_EOF_VAR       0xFFFF

typedef enum
{
    HTTP_TXT,
    HTTP_HTML,
    HTTP_GIF,
    HTTP_CGI,
    HTTP_JPG,
    HTTP_JAVA,
    HTTP_WAV,
    HTTP_JS,
    HTTP_CSS,
    HTTP_PNG,
    HTTP_NUM_FILE_TYPES,
    HTTP_FILE_TYPE_UNKNOWN
}HTTP_FILE_EXT_ENUM;

enum
{
    HTTP_GET_FILE_READ,
    HTTP_GET_FILE_VAR,
    HTTP_GET_FILE_DLE,
    HTTP_GET_FILE_HANDLE
};


enum
{
    HTTP_READ_ERROR
};

typedef enum
{
    HTTP_STATUS_CONTINUE,
    HTTP_STATUS_OK,
    HTTP_STATUS_CREATED,
    HTTP_STATUS_ACCEPTED,
    HTTP_STATUS_NO_CONTENT,
    HTTP_STATUS_MOVED_PERMANENTLY,
    HTTP_STATUS_MOVED_TEMPORILY,
    HTTP_STATUS_NOT_MODIFIED,
    HTTP_STATUS_BAD_REQUEST,
    HTTP_STATUS_UNAUTHORIZED,
    HTTP_STATUS_FORBIDDEN,
    HTTP_STATUS_NOT_FOUND,
    HTTP_STATUS_INTERNAL_SERVER_ERR,
    HTTP_STATUS_NOT_IMPLEMENTED,
    HTTP_STATUS_BAD_GATEWAY,
    HTTP_STATUS_SERVICE_UAVAILABLE,
    HTTP_STATUS_NUMBER
}HTTP_STATUS_CODES;


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
PUBLIC BOOL HTTPInit(void);
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
PUBLIC void HTTPCloseAll(void);
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
PUBLIC BYTE HTTPGetNumberOfConnections(void);
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
PUBLIC const SOCKET *HTTPGetListnerSocket(void);
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
PUBLIC void HTTPServer(void);

#endif
