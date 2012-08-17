/********************************************************************
 * FileName:		httpEx.c
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

#ifndef _HTTPEX_HEADER_FILE
#define _HTTPEX_HEADER_FILE

typedef struct
{
    BYTE filename[24];
}HTTP_EX_FILENAME;

void HTTPExecCmd(BYTE **arg, BYTE num_arg);
WORD HTTPGetVar(BYTE var, WORD ref, BYTE *data);
HTTP_STATUS_CODES HTTPSendVar(BYTE *filename, BYTE *var, BYTE *value, BOOL end);
BOOL HTTPAuthorizationRequired(BYTE *filename);
BOOL HTTPChkAuthorization(BYTE *type, BYTE *text);
BOOL HTTPPUTUpload(BYTE *filename);
BOOL HTTPPUTSendFile(BYTE *filename, HTTP_STATUS_CODES status);
BYTE _GetNumberChar(DWORD number, BYTE position);
#endif
