/********************************************************************
 * FileName:		FTP.c
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

#include <string.h>
#include "bsd_ftp_server\ftp.h"
#include "ftp_private.h"
#include "ftpex.h"

/************************************************************************
 * private constants
 ************************************************************************/
const char *_ftpCommandString[] =
{
    "USER",                         // FTP_CMD_USER
    "PASS",                         // FTP_CMD_PASS
    "QUIT",                         // FTP_CMD_QUIT
    "STOR",                         // FTP_CMD_STOR
    "PORT",                         // FTP_CMD_PORT
    "ABOR",                         // FTP_CMD_ABORT
	"SYST",							// FTP_CMD_SYST
	"NOOP",							// FTP_CMD_NOOP
	"PWD",							// FTP_CMD_PWD
	"TYPE",							// FTP_CMD_TYPE
	"RETR"							// FTP_CMD_RETR
};
#define FTP_COMMAND_TABLE_SIZE  ( sizeof(FTPCommandString)/sizeof(FTPCommandString[0]) )

const char *_ftpResponseString[] =
{
    "220 Ready\r\n",							// FTP_RESP_BANNER
    "331 Password required\r\n",				// FTP_RESP_USER_OK
    "230 Logged in\r\n",						// FTP_RESP_PASS_OK
    "221 Bye\r\n",								// FTP_RESP_QUIT_OK
    "500 \r\n",									// FTP_RESP_STOR_OK
    "502 Not implemented\r\n",					// FTP_RESP_UNKNOWN
    "530 Login required\r\n",					// FTP_RESP_LOGIN
    "150 Transferring data...\r\n",				// FTP_RESP_DATA_OPEN
    "125 Done.\r\n",							// FTP_RESP_DATA_READY
    "226 Transfer Complete\r\n",				// FTP_RESP_DATA_CLOSE
    "200 ok\r\n",								// FTP_RESP_OK
	"215 UNIX Type: I\r\n",						// FTP_RESP_SYST
	"257 \"/upfiles\" is current directory\r\n"	// FTP_RESP_DIRECTORY
};

/****************************************************************
 * private variables
 ****************************************************************/
PRIVATE SOCKET			_ftpListenSocket;
PRIVATE SOCKET			_ftpDataSocket;
PRIVATE SOCKET			_ftpConnectionSocket;
PRIVATE SOCKADDR_IN		_ftpSockAddr;
PRIVATE WORD_VAL		_ftpDataPort;
PRIVATE SM_FTP			_ftpStage;
PRIVATE SM_FTP_CMD		_ftpStageCmd;
PRIVATE	FTP_COMMAND		_ftpCmd;
PRIVATE FTP_RESPONSE	_ftpResponce;	
PRIVATE BYTE			_ftpUser[FTP_USER_NAME_LEN];
PRIVATE	BYTE			_ftpStr[MAX_FTP_CMD_STRING_LEN + 2];
PRIVATE	BYTE			_ftpStrLen;
PRIVATE	BYTE			*_ftpArgv[MAX_FTP_ARGS];
PRIVATE BYTE			_ftpNumArg;
PRIVATE TICK			_lastActivity;
PRIVATE FSFILE			*_ftpFile;
PRIVATE FTP_FLAGS		_ftpFlags;
PRIVATE BOOL			_ftpConnection;
PRIVATE BYTE			_ftpTxBuf[FTP_TX_BUF_SIZE];
PRIVATE int				_ftpTxSize;
PRIVATE DWORD			_ftpFilePtr;


/*********************************************************************
 * Function:		PUBLIC BOOL FTPInit(void)       
 *
 * PreCondition:	The TCP/IP is initialized.  
 *
 * Input:           None.
 *
 * Output:          true if socket was successfully creasted. else false
 *
 * Side Effects:    None
 *
 * Overview:        This function will create a socket and bind it to
 *					the FTP port.  It will listen to incoming connection
 *					requests.
 ********************************************************************/
PUBLIC BOOL FTPInit(void)
{
	SOCKADDR_IN		saServer;
    int             nResult;

    _ftpListenSocket = socket(  AF_INET,
                                SOCK_STREAM,
						        IPPROTO_TCP);

    if(_ftpListenSocket == INVALID_SOCKET)
        return FALSE;

    #ifdef _BORLAND_BUILD
	saServer.sin_port           = htons(FTP_COMMAND_PORT);
    #else
	saServer.sin_port           = FTP_COMMAND_PORT;
    #endif
    saServer.sin_family         = AF_INET;
	saServer.sin_addr.s_addr    = INADDR_ANY;

    nResult = bind( _ftpListenSocket,
                    (LPSOCKADDR)&saServer,
                    sizeof(struct sockaddr));

    if(nResult == SOCKET_ERROR)
    {
        closesocket(_ftpListenSocket);
		_ftpListenSocket = INVALID_SOCKET;
        return FALSE;
    }

    nResult = listen(_ftpListenSocket, 1);

    if(nResult == SOCKET_ERROR)
    {
        closesocket(_ftpListenSocket);
		_ftpListenSocket = INVALID_SOCKET;
        return FALSE;
    }

	_ftpStage				= SM_FTP_NOT_CONNECTED;
	_ftpStrLen				= 0;
	_ftpFlags.Val			= 0;
	_ftpDataPort.Val		= FTP_DATA_PORT;
	_ftpConnection			= FALSE;
	_ftpConnectionSocket	= INVALID_SOCKET;
	_ftpDataSocket			= INVALID_SOCKET;
	_ftpCmd					= FTP_CMD_NONE;
	_ftpStageCmd			= SM_FTP_CMD_IDLE;
	_ftpTxSize				= 0;
	_ftpFile				= NULL;
	_ftpFilePtr				= 0;
	return TRUE;
	
}
/*********************************************************************
 * Function:        PUBLIC BOOL FTPServer(void)
 *
 * PreCondition:    FTPInit has been called
 *
 * Input:           None.
 *
 * Output:          true if FTP connections are being serviced, else false
 *
 * Side Effects:    None.
 *
 * Overview:        This function handles incoming FTP connections and 
 *					services all incoming commands
 ********************************************************************/
PUBLIC BOOL FTPServer(void)
{
	int result;

	if(_ftpListenSocket == INVALID_SOCKET)
		return FALSE;

	// check to see if we have a connection pending
	if(!_ftpConnection)
		_ftpConnection = _AcceptConnection();

	if(!_ftpConnection)
	{
		_ftpStage				= SM_FTP_NOT_CONNECTED;
		_ftpStrLen				= 0;
		_ftpFlags.Val			= 0;
		_ftpCmd					= FTP_CMD_NONE;
		_ftpStageCmd			= SM_FTP_CMD_IDLE;
		_ftpTxSize				= 0;
		return FALSE;
	}

	
	if(_ftpTxSize)
	{
		_ftpTxSize = _SendData(_ftpConnectionSocket, _ftpTxBuf, _ftpTxSize);

		if(_ftpTxSize == INVALID_SOCKET)
		{
			_CloseConnection(_ftpConnectionSocket);
			return FALSE;
		}

		if(_ftpTxSize)
			return TRUE;
		
	}

	_ftpStrLen = MAX_FTP_CMD_STRING_LEN;
	
	if((result = _GetTCPRecv(_ftpConnectionSocket, _ftpStr, &_ftpStrLen)))
	{
		if(result == INVALID_SOCKET)
		{
			_CloseConnection(_ftpConnectionSocket);
			return FALSE;
		}

		_ftpStrLen--;

		if(_ftpStr[_ftpStrLen] == '\n')
		{
			_ftpStr[_ftpStrLen]		= 0;
			_ftpStrLen				= 0;
			_ftpNumArg				= _ParseFTPString(_ftpStr, _ftpArgv, MAX_FTP_ARGS);
			_ftpCmd					= _GetFTPCommand(_ftpArgv[0]);
		}
	
	}else
	{
		if(_ftpStage != SM_FTP_NOT_CONNECTED)
		{
			TICK	curr_tick;

			curr_tick = TickGet();
			curr_tick = TickGetDiff(curr_tick, _lastActivity);

			if(curr_tick >= FTP_TIMEOUT)
			{
				_lastActivity	= TickGet();
				_ftpCmd			= FTP_CMD_QUIT;
				_ftpStage		= SM_FTP_CONNECTED; 
			}
		}
	}

	switch(_ftpStage)
	{
	case SM_FTP_NOT_CONNECTED:
		_ftpResponce	= FTP_RESP_BANNER;
		_lastActivity	= TickGet();

	case SM_FTP_RESPOND:
		strcpy(_ftpTxBuf, _ftpResponseString[_ftpResponce]);
		_ftpTxSize = strlen(_ftpTxBuf);

		_ftpTxSize		= _SendData(_ftpConnectionSocket, _ftpTxBuf, _ftpTxSize);
		_ftpResponce	= FTP_RESP_NONE;
		_ftpStage		= SM_FTP_CONNECTED;

		if(_ftpTxSize)
			break;

	case SM_FTP_CONNECTED:
		if(_ftpCmd != FTP_CMD_NONE)
		{
			if(_ExecuteFTPCommand(_ftpCmd, _ftpArgv))
			{
				if(_ftpResponce != FTP_RESP_NONE)
					_ftpStage = SM_FTP_RESPOND;

				if(_ftpCmd == FTP_CMD_QUIT)
					_ftpStage = SM_FTP_NOT_CONNECTED;

				_ftpCmd			= FTP_CMD_NONE;
				_ftpStageCmd	= SM_FTP_CMD_IDLE;

			}else
			{
				if(_ftpResponce != FTP_RESP_NONE)
					_ftpStage = SM_FTP_RESPOND;
			}
		}
		break;

    default:
        break;
	}

	return TRUE;
}
/*********************************************************************
 * Function:		PUBLIC BOOL FTPGetUser(BYTE *user)      
 *
 * PreCondition:    None.
 *
 * Input:           user	- pointer to the user buffer
 *
 * Output:          true if there is a FTP connection, else false
 *
 * Side Effects:    None.
 *
 * Overview:        This function fills the user pointer with 
 *					FTP user information.
 ********************************************************************/
PUBLIC BOOL FTPGetUser(BYTE *user)
{
	if(!_ftpConnection)
	{
		strcpy(user, "Not Connected");
		return FALSE;
	}

	if(_ftpUser[0] == 0)
	{
		strcpy(user, "No user");
	}else
	{
		strcpy(user, "User:");
		strcat(user, _ftpUser);
	}

	return TRUE;
}
/*********************************************************************
 * Function:            PUBLIC FTP_COMMAND FTPGetCammand(void)      
 *
 * PreCondition:        None.   
 *
 * Input:               None.
 *
 * Output:              Current FTP command
 *
 * Side Effects:        None
 *
 * Overview:            This funciton will return the current 
 *                      FTP command
 *
 ********************************************************************/
PUBLIC SM_FTP FTPGetStage(void)
{
    return _ftpStage;
}   
/*********************************************************************
 * Function:        PRIVATE BOOL _AcceptConnection(void)
 *
 * PreCondition:    None.
 *
 * Input:           None.
 *
 * Output:          true if there is a FTP connection, else false
 *
 * Side Effects:    None.
 *
 * Overview:        This function accepts incoming connections on the 
 *					FTP port.  If there is an incoming connection, the 
 *					socket will be assigned and the service function will
 *					be able to Rx FTP commands.
 ********************************************************************/
PRIVATE BOOL _AcceptConnection(void)
{
	SOCKET		peersocket;
	int			nlen;

	nlen        = sizeof(SOCKADDR_IN);

	peersocket  = accept(   _ftpListenSocket,
					        (LPSOCKADDR)&_ftpSockAddr,
						    &nlen);

	if(peersocket == INVALID_SOCKET)
		return FALSE;

	_ftpConnectionSocket = peersocket;

	_ftpUser[0] = 0;

	return TRUE;
}
/*********************************************************************
 * Function:        PRIVATE void _CloseConnection(SOCKET socket)
 *
 * PreCondition:    None.
 *
 * Input:           socket	- socket to close
 *
 * Output:          None.
 *
 * Side Effects:    None.
 *
 * Overview:        This function closes the socket.  If a file is 
 *					open, this function will close and initialize the
 *					pointer.  Other variables will be initialized as well
 ********************************************************************/
PRIVATE void _CloseConnection(SOCKET socket)
{
	closesocket(socket);

	if(_ftpFile)
		FSfclose(_ftpFile);

	_ftpFile				= NULL;
	_ftpStage				= SM_FTP_NOT_CONNECTED;
	_ftpStrLen				= 0;
	_ftpFlags.Val			= 0;
	_ftpCmd					= FTP_CMD_NONE;
	_ftpStageCmd			= SM_FTP_CMD_IDLE;
	_ftpConnection			= FALSE;
	_ftpTxSize				= 0;
	_ftpDataPort.Val		= FTP_DATA_PORT;

}
/*********************************************************************
 * Function:		PRIVATE int _GetTCPRecv(	SOCKET socket, 
 *												BYTE *data, 
 *												BYTE *data_len)        
 *
 * PreCondition:    None
 *
 * Input:           socket		- socket handle to get pending data from
 *					data		- pointer to the buffer to place the data
 *					data_len	- pointer to the size of the data gotten, it
 *									will also pass the max size of the data
 *
 * Output:          The result of the recv funciton
 *
 * Side Effects:    None
 *
 * Overview:        This function gets pending data from a socket.
 ********************************************************************/
PRIVATE int _GetTCPRecv(SOCKET socket, BYTE *data, BYTE *data_len)
{
	int			result;




	result = recv(	socket,
					data,
					*data_len,
					0);

#ifdef _WINDOWS
	if(result == 0)
		return INVALID_SOCKET;
#endif

	if(result > 0)
		*data_len = (BYTE)result;

	return result;
	
}
/*********************************************************************
 * Function:        PRIVATE BYTE _ParseFTPString(	BYTE *ftp_str, 
 *													BYTE **arg, 
 *													BYTE max_arg)
 *
 * PreCondition:    None
 *
 * Input:           ftp_str	- pointer to an FTP command string
 *					arg		- pointer to a list of FTP command arguments
 *					max_arg - maximun number of arguments allowed
 *
 * Output:          number of agruments in the command string
 *
 * Side Effects:    None
 *
 * Overview:        This function parses the FTP command string into 
 *					a list of arguments.
 *
 * Note:			EXAMPLE:
 *					Rx FTP Cmd:	PORT,10,10,33,203,4,84
 *					Parsed Cmd:
 *						arg[0] = PORT
 *						arg[1] = 10
 *						arg[2] = 10
 *						arg[3] = 33
 *						arg[4] = 203
 *						arg[5] = 4
 *						arg[6] = 84
 ********************************************************************/
PRIVATE BYTE _ParseFTPString(BYTE *ftp_str, BYTE **arg, BYTE max_arg)
{
	BYTE	num;
	BYTE	alpha;
	BOOL	space;

	while(*ftp_str == ' ')
		ftp_str++;

	space	= FALSE;
	arg[0]	= ftp_str;
	num		= 1;

	while((alpha = *ftp_str) && (num < max_arg))
	{
		if(!space)
		{
			if(alpha == ' ' || alpha == ',')
			{
				*ftp_str	= 0;
				space		= TRUE;
			}else
			{
				if(alpha == '\r' || alpha == '\n')
					*ftp_str = 0;
			}
		
		}else
		{
			if(alpha != ' ')
			{
				arg[num++] = ftp_str;
				space = FALSE;
			}
		}

		ftp_str++;
	}

	return num;
}
/*********************************************************************
 * Function:        PRIVATE FTP_COMMAND _GetFTPCommand(BYTE *cmd_str)
 *
 * PreCondition:    None
 *
 * Input:           cmd_str - the parsed FTP command
 *
 * Output:          FTP_COMMAND enum, if none return FTP_CMD_UNKNOWN
 *					See ftp_private for the FTP_COMMAND enum
 *
 * Side Effects:    None.
 *
 * Overview:        This function takes the command string and returns 
 *					the enumeration that will be used to identify and 
 *					processes it.
 *
 * Note:			EXAMPLE
 *					Command String:     "USER"
 *					Return Value:		FTP_CMD_USER
 ********************************************************************/
PRIVATE FTP_COMMAND _GetFTPCommand(BYTE *cmd_str)
{
	FTP_COMMAND	cmd;

	for(cmd = FTP_CMD_USER; cmd < FTP_CMD_UNKNOWN; cmd++)
	{
		BYTE size;

		size = strlen(_ftpCommandString[cmd]);

		if(!strncmp(cmd_str, _ftpCommandString[cmd], size))
			break;
	}

	return cmd;
}
/*********************************************************************
 * Function:		PRIVATE BOOL _ExecuteFTPCommand(	FTP_COMMAND cmd, 
 *														BYTE **arg)
 *
 * PreCondition:    None.
 *
 * Input:           cmd - FTP_COMMAND (see ftp_private.h for FTP_COMMAND enum)
 *					arg - pointer to the list of arguments
 *
 * Output:          if command serviced true, else false
 *
 * Side Effects:    None.
 *
 * Overview:        This funciton services FTP commands and provides the 
 *					responces
 *
 * Note:			If the FTP command is unknown, the responce will be
 *					"502 Not implemented\r\n"
 ********************************************************************/
PRIVATE BOOL _ExecuteFTPCommand(FTP_COMMAND cmd, BYTE **arg)
{
	switch(cmd)
	{
	case FTP_CMD_USER:
		_ftpFlags.Bits.bUserSupplied	= TRUE;
		_ftpFlags.Bits.bLoggedIn		= FALSE;
		_ftpResponce					= FTP_RESP_USER_OK;
		strncpy(_ftpUser, arg[1], FTP_USER_NAME_LEN);
		break;

	case FTP_CMD_PASS:
		if(!_ftpFlags.Bits.bUserSupplied)
		{
			_ftpResponce = FTP_RESP_LOGIN;
		}else
		{
			if(FTPVerify(_ftpUser, arg[1]))
			{
				_ftpResponce				= FTP_RESP_PASS_OK;
				_ftpFlags.Bits.bLoggedIn	= TRUE;
			}else
			{
				_ftpResponce				= FTP_RESP_LOGIN;
			}
		}
		break;

	case FTP_CMD_PORT:
		_ftpDataPort.v[1]	= (BYTE)atoi(arg[5]);
		_ftpDataPort.v[0]	= (BYTE)atoi(arg[6]);
		_ftpResponce		= FTP_RESP_OK;
		break;

	case FTP_CMD_ABORT:
		if(_ftpDataSocket != INVALID_SOCKET)
			closesocket(_ftpDataSocket);

		if(_ftpFile)
			FSfclose(_ftpFile);
		
		_ftpFile = NULL;

		_ftpResponce	= FTP_RESP_OK;
		break;

	case FTP_CMD_QUIT:
		return _FTPQuit();
		
	case FTP_CMD_STOR:
		return _FTPPutFile(arg[1]);

	case FTP_CMD_RETR:
		return _FTPGetFile(arg[1]);

	case FTP_CMD_NOOP:
		_ftpResponce = FTP_RESP_OK;
		break;

	case FTP_CMD_PWD:
		_ftpResponce = FTP_RESP_DIRECTORY;
		break;

	case FTP_CMD_SYST:
	case FTP_CMD_TYPE:
		_ftpResponce = FTP_RESP_SYST;
		break;

	default:
		_ftpResponce = FTP_RESP_UNKNOWN;
		break;

	}

	return TRUE;
}
/*********************************************************************
 * Function:        PRIVATE BOOL _FTPQuit(void)
 *
 * PreCondition:	None   
 *
 * Input:           None
 *
 * Output:          Always false.
 *
 * Side Effects:    None
 *
 * Overview:        This function properly closes all open sockets
 *					and files.  It will also re-initialize all parameters
 ********************************************************************/
PRIVATE BOOL _FTPQuit(void)
{
	switch(_ftpStageCmd)
	{
	default:
	case SM_FTP_CMD_IDLE:
	case SM_FTP_CMD_WAIT:
	case SM_FTP_CMD_RX_TX:
		if(_ftpDataSocket != INVALID_SOCKET)
			closesocket(_ftpDataSocket);

		if(_ftpFile)
			FSfclose(_ftpFile);

		_ftpResponce			= FTP_RESP_QUIT_OK;
		_ftpDataSocket			= INVALID_SOCKET;
		_ftpFile				= NULL;
		_ftpStageCmd			= SM_FTP_CMD_WAIT_FOR_DISCONNECT;
		break;

	case SM_FTP_CMD_WAIT_FOR_DISCONNECT:
		if(_ftpConnectionSocket != INVALID_SOCKET)
			_CloseConnection(_ftpConnectionSocket);
		break;
	}

	return FALSE;
}
/*********************************************************************
 * Function:		PRIVATE BOOL _FTPPutFile(BYTE *filename)     
 *
 * PreCondition:    None.
 *
 * Input:           filename - pointer to the ASCII file name to 
 *								put on the server
 *
 * Output:          true if competed, else false
 *
 * Side Effects:    None.
 *
 * Overview:        This function will open a socket on the user supplied
 *					port and connect to it.  It will then create a file to store
 *					all of the information coming accross the "data" port.
 ********************************************************************/
PRIVATE BOOL _FTPPutFile(BYTE *filename)
{
	int result;

	switch(_ftpStageCmd)
	{
	case SM_FTP_CMD_IDLE:
		if(!_ftpFlags.Bits.bLoggedIn)
		{
			_ftpResponce	= FTP_RESP_LOGIN;
			return TRUE;
		}else
		{
			SOCKADDR_IN		saServer;

			_ftpDataSocket =	socket(  AF_INET,
										SOCK_STREAM,
										IPPROTO_TCP);

			if(_ftpDataSocket == INVALID_SOCKET)
				break;

			saServer.sin_port           = _ftpDataPort.Val;
			saServer.sin_family         = AF_INET;
			saServer.sin_addr.s_addr    = INADDR_ANY;


			result = bind(	_ftpDataSocket,
							(LPSOCKADDR)&saServer,
							sizeof(SOCKADDR));

			if(result == SOCKET_ERROR)
			{
				closesocket(_ftpDataSocket);
				_ftpDataSocket = INVALID_SOCKET;
				return TRUE;
			}

			_ftpSockAddr.sin_port		= _ftpDataPort.Val;

			result = connect(	_ftpDataSocket,
								(LPSOCKADDR)&_ftpSockAddr,
								sizeof(SOCKADDR));

			if(result == SOCKET_ERROR)
			{
				closesocket(_ftpDataSocket);
				_ftpDataSocket = INVALID_SOCKET;
				return TRUE;
			}

			_ftpResponce	= FTP_RESP_DATA_OPEN;

			if(_ftpFile)
				FSfclose(_ftpFile);

			_ftpFile = FSfopen(filename, "wb");

			if(!_ftpFile)
			{
				_ftpResponce = FTP_RESP_UNKNOWN;
				closesocket(_ftpDataSocket);
				_ftpDataSocket = INVALID_SOCKET;
				return TRUE;
			}else
			{
				_ftpStageCmd = SM_FTP_CMD_WAIT;
			}
		}
		break;


	case SM_FTP_CMD_WAIT:
			result = connect(	_ftpDataSocket,
								(LPSOCKADDR)&_ftpSockAddr,
								sizeof(SOCKADDR));

			switch(result)
			{
			case SOCKET_ERROR:
				closesocket(_ftpDataSocket);
				FSfclose(_ftpFile);
				
				_ftpDataSocket 	= INVALID_SOCKET;
				_ftpFile 		= NULL;
				return TRUE;
			
			case SOCKET_CNXN_IN_PROGRESS:
				break;

			case 0:
				_ftpStageCmd = SM_FTP_CMD_RX_TX;
				break;
			}
			break;

	case SM_FTP_CMD_RX_TX:
		{
			BYTE	buf[64];
			BYTE	size;

			while(1)
			{
				size = 64;

				result = _GetTCPRecv(	_ftpDataSocket,
										buf,
										&size);

			
				if(!result)
					break;

				if(result == SOCKET_ERROR)
				{
					closesocket(_ftpDataSocket);
					FSfclose(_ftpFile);

					_ftpDataSocket	= INVALID_SOCKET;
					_ftpFile		= NULL;
					_ftpResponce	= FTP_RESP_DATA_CLOSE;
					return TRUE;
				}

				FSfwrite(buf, 1, size, _ftpFile);
			}
			
		}
		break;

	default:
		break;
	}
	return FALSE;
}
/*********************************************************************
 * Function:		PRIVATE BOOL _FTPGetFile(BYTE *filename)     
 *
 * PreCondition:    None.
 *
 * Input:           filename - pointer to the ASCII file name to 
 *								get for the server
 *
 * Output:          true if competed, else false
 *
 * Side Effects:    None.
 *
 * Overview:        This function will open a socket on the user supplied
 *					port and connect to it.  It will then open the file and 
 *					pass the information in it to the client.
 ********************************************************************/
PRIVATE BOOL _FTPGetFile(BYTE *filename)
{
	int result;

	switch(_ftpStageCmd)
	{
	case SM_FTP_CMD_IDLE:
		if(!_ftpFlags.Bits.bLoggedIn)
		{
			_ftpResponce	= FTP_RESP_LOGIN;
			return TRUE;
		}else
		{
			SOCKADDR_IN		saServer;

			_ftpDataSocket =	socket(  AF_INET,
										SOCK_STREAM,
										IPPROTO_TCP);

			if(_ftpDataSocket == INVALID_SOCKET)
				break;

			saServer.sin_port           = _ftpDataPort.Val;
			saServer.sin_family         = AF_INET;
			saServer.sin_addr.s_addr    = INADDR_ANY;


			result = bind(	_ftpDataSocket,
							(LPSOCKADDR)&saServer,
							sizeof(SOCKADDR));

			if(result == SOCKET_ERROR)
			{
				closesocket(_ftpDataSocket);
				_ftpDataSocket = INVALID_SOCKET;
				return TRUE;
			}

			_ftpSockAddr.sin_port		= _ftpDataPort.Val;

			result = connect(	_ftpDataSocket,
								(LPSOCKADDR)&_ftpSockAddr,
								sizeof(SOCKADDR));

			if(result == SOCKET_ERROR)
			{
				closesocket(_ftpDataSocket);
				_ftpDataSocket = INVALID_SOCKET;
				return TRUE;
			}

			_ftpResponce	= FTP_RESP_DATA_OPEN;

			if(_ftpFile)
				FSfclose(_ftpFile);

			_ftpFile		= FSfopen(filename, "rb");
			_ftpFilePtr		= 0;

			if(!_ftpFile)
			{
				_ftpResponce = FTP_RESP_UNKNOWN;
				closesocket(_ftpDataSocket);
				_ftpDataSocket = INVALID_SOCKET;
				return TRUE;
			}else
			{
				_ftpStageCmd = SM_FTP_CMD_WAIT;
			}
		}
		break;


	case SM_FTP_CMD_WAIT:
			result = connect(	_ftpDataSocket,
								(LPSOCKADDR)&_ftpSockAddr,
								sizeof(SOCKADDR));

			switch(result)
			{
			case SOCKET_ERROR:
				closesocket(_ftpDataSocket);
				FSfclose(_ftpFile);
				
				_ftpDataSocket 	= INVALID_SOCKET;
				_ftpFile 		= NULL;
				return TRUE;
			
			case SOCKET_CNXN_IN_PROGRESS:
				break;

			case 0:
				_ftpStageCmd = SM_FTP_CMD_RX_TX;
				break;
			}
			break;

	case SM_FTP_CMD_RX_TX:
		{
			BYTE	buf[64];
			BYTE	size;


            if(FSfseek(_ftpFile, _ftpFilePtr, SEEK_SET))
			{	
				closesocket(_ftpDataSocket);
				FSfclose(_ftpFile);

				_ftpDataSocket	= INVALID_SOCKET;
				_ftpFile		= NULL;
				_ftpResponce	= FTP_RESP_UNKNOWN;
				return TRUE;
			}
			result = FSfread(buf, 1, 64, _ftpFile);

			if(!result)
			{
				closesocket(_ftpDataSocket);
				FSfclose(_ftpFile);

				_ftpDataSocket	= INVALID_SOCKET;
				_ftpFile		= NULL;
				_ftpResponce	= FTP_RESP_DATA_CLOSE;
				return TRUE;

			}
			
			size = (BYTE)result;

			result = send(	_ftpDataSocket,
							buf,
							size,
							0);

			if(result == SOCKET_ERROR)
			{
				closesocket(_ftpDataSocket);
				FSfclose(_ftpFile);

				_ftpDataSocket	= INVALID_SOCKET;
				_ftpFile		= NULL;
				_ftpResponce	= FTP_RESP_UNKNOWN;
				return TRUE;

			}
			
			if(result > 0)
				_ftpFilePtr += (DWORD)result;
			
		}
		break;
	default:
		break;
	}
	return FALSE;

}
/*********************************************************************
 * Function:		PRIVATE int _SendData(	SOCKET socket, 
 *											BYTE *buf, 
 *											int len)
 *
 * PreCondition:	None    
 *
 * Input:           socket	- socket handle 
 *					buf		- buffer of data to send
 *					len		- size of data in bytes to transfer
 *
 * Output:          result of the send function
 *
 * Side Effects:    none
 *
 * Overview:        This funciton sends data on the TCP/IP socket. 
 *					If the data is not sent, it will return the remaining
 *					amount that needs to be sent.
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
