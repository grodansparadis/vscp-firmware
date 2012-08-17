/********************************************************************
 * FileName:		FTP_private.h		
 * Dependencies:
 * Processor:				
 * Hardware:		
 * Assembler:		
 * Linker:		
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
 * FTP Server
 *
 * Change History:
 * Name				Date            Changes
 * Sean Justice		03/10/06		Initial Version
 ********************************************************************/
#ifndef _FTP_PRIVATE_HEADER_FILE
#define _FTP_PRIVATE_HEADER_FILE
/************************************************************************
 * private defines
 ************************************************************************/
#define FTP_COMMAND_PORT                (21)
#define FTP_DATA_PORT                   (20)
#define FTP_TIMEOUT                     (TICK)((TICK)180 * TICKS_PER_SECOND)
#define MAX_FTP_ARGS                    (7)
#define MAX_FTP_CMD_STRING_LEN          (31)
/************************************************************************
 * private enumerations
 ************************************************************************/
typedef enum _SM_FTP_CMD
{
    SM_FTP_CMD_IDLE,
    SM_FTP_CMD_WAIT,
    SM_FTP_CMD_RX_TX,
    SM_FTP_CMD_WAIT_FOR_DISCONNECT
} SM_FTP_CMD;

typedef enum _FTP_COMMAND
{
    FTP_CMD_USER,
    FTP_CMD_PASS,
    FTP_CMD_QUIT,
    FTP_CMD_STOR,
    FTP_CMD_PORT,
    FTP_CMD_ABORT,
	FTP_CMD_SYST,
	FTP_CMD_NOOP,
	FTP_CMD_PWD,
	FTP_CMD_TYPE,
	FTP_CMD_RETR,
    FTP_CMD_UNKNOWN,
    FTP_CMD_NONE,
} FTP_COMMAND;

typedef enum _FTP_RESPONSE
{
    FTP_RESP_BANNER,
    FTP_RESP_USER_OK,
    FTP_RESP_PASS_OK,
    FTP_RESP_QUIT_OK,
    FTP_RESP_STOR_OK,
    FTP_RESP_UNKNOWN,
    FTP_RESP_LOGIN,
    FTP_RESP_DATA_OPEN,
    FTP_RESP_DATA_READY,
    FTP_RESP_DATA_CLOSE,
    FTP_RESP_OK,
	FTP_RESP_SYST,
	FTP_RESP_DIRECTORY,

    FTP_RESP_NONE                       // This must always be the last
                                        // There is no corresponding string.
} FTP_RESPONSE;

/************************************************************************
 * private unions
 ************************************************************************/
typedef union
{
	struct
	{
		BYTE	bUserSupplied:	1;
		BYTE	bLoggedIn:		1;
		BYTE	bSpare:			6;
	}Bits;
	BYTE	Val;
}FTP_FLAGS;

/****************************************************************
 * private functions
 ****************************************************************/
PRIVATE BOOL _AcceptConnection(void);
PRIVATE void _CloseConnection(SOCKET socket);
PRIVATE int _GetTCPRecv(SOCKET socket, BYTE *data, BYTE *data_len);
PRIVATE BYTE _ParseFTPString(BYTE *ftp_str, BYTE **arg, BYTE max_arg);
PRIVATE FTP_COMMAND _GetFTPCommand(BYTE *cmd_str);
PRIVATE BOOL _ExecuteFTPCommand(FTP_COMMAND cmd, BYTE **arg);
PRIVATE BOOL _FTPQuit(void);
PRIVATE BOOL _FTPPutFile(BYTE *filename);
PRIVATE int _SendData(SOCKET socket, BYTE *buf, int len);
PRIVATE BOOL _FTPGetFile(BYTE *filename);
#endif
