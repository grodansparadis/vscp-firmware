/*********************************************************************
 *
 *                  Socket API for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        socket.c
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its dsPIC30F and PICmicro® Microcontroller is intended 
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip's dsPIC30F and PICmicro Microcontroller products. 
 * The software is owned by the Company and/or its supplier, and is
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
 * $Id: socket.c,v 1.7 2006/10/24 16:39:08 C12923 Exp $
 *
 ********************************************************************/

#include "bsd_tcp_ip\sockapi.h"
#include "bsd_tcp_ip\socket.h"
#include "bsd_tcp_ip\eudp.h"
#include "eTCP.def"
#include "bsd_tcp_ip\block_mgr.h"
#include "bsd_tcp_ip\gpfunc.h"
#include "string.h"

struct MCSocket  SocketArray[MAX_SOCKET];
WORD gAutoPortNumber = 1024;


/*********************************************************************
 * Function:        void SocketInitialize( SOCKET s )
 *
 * PreCondition:    Must empty the RxMsgQueue before calling this function
 * Input:			s - Socket identifier
 * Output:          None
 * Side Effects:    None
 * Overview:        Initialize the socket structure to a known initial value
 * Note:
 ********************************************************************/
void SocketInitialize( SOCKET s )
{
	struct MCSocket		*pSocket;
	
	pSocket = (struct MCSocket *)&SocketArray[s];
	
	pSocket->SocketType 		= SOCK_DGRAM;
	pSocket->smState 			= SKT_CLOSED;
	
	pSocket->localPort 		= INVALID_TCP_PORT;
	pSocket->localIP			= IP_ADDR_ANY; // = 0

	pSocket->remotePort		= INVALID_TCP_PORT;
	pSocket->remoteIP			= IP_ADDR_ANY; // = 0

	pSocket->ActiveRxSrcIP	= INVALID_TCP_PORT;
    pSocket->ActiveRxSrcPort	= IP_ADDR_ANY;

	pSocket->DataByteIndex 	= 0;  // Rx Data Index
	pSocket->TxDataIndex 		= 0;  // Tx Data Index
	pSocket->TxBfrSize		= TCP_DEFAULT_TX_BFR_SZ;
	pSocket->pRxActivePacket  = NULL;
	pSocket->pTxActivePacket  = NULL;
	pSocket->FirstSend		= TRUE;
	pSocket->TcpNoDelay		= FALSE;

    pSocket->RetryCount		= 0;
    pSocket->startTick		= TickGet();
    pSocket->backlog			= 5;
    pSocket->pParentSocket	= NULL;
    pSocket->SocketID			= s;

	Que_Init( &pSocket->RxMsgQueue );
	Que_Init( &pSocket->CnxnQueue );
	
    pSocket->pTCPListHead		= NULL;
    pSocket->pTCPListTail		= NULL;
    pSocket->AckListCount		= 0;
    pSocket->TCPAppClosed 	= 0;

}

/*********************************************************************
 * Function:        void SocketLayerInit(void)
 *
 * PreCondition:    None
 * Input:			None
 * Output:          None
 * Side Effects:    None
 * Overview:        Initialize the entire socket structure array
 * Note:
 ********************************************************************/
void SocketLayerInit(void)
{
    int s;
    for ( s = 0; s < MAX_SOCKET; s++ )
		SocketInitialize( s );
}

/*********************************************************************
 * Function:        SOCKET socket( int af, int type, int protocol )
 *
 * PreCondition:    None
 * Input:			af - address family - AF_INET
 *					type - socket type SOCK_DGRAM or SOCK_STREAM
 *					protocol - IP protocol IPPROTO_UDP or IPPROTO_TCP
 * Output:          New socket descriptor.
 * Side Effects:    None
 * Overview:        This function creates a new socket
 * Note:
 ********************************************************************/
SOCKET socket( int af, int type, int protocol )
{
    struct MCSocket *socket = SocketArray;
    int s;

	if( af != AF_INET )
		return SOCKET_ERROR;

    for ( s = 0; s < MAX_SOCKET; s++,socket++ )
    {
		if( socket->smState != SKT_CLOSED ) //socket in use
			continue;

		socket->SocketType = type;

		if( type == SOCK_DGRAM && protocol == IPPROTO_UDP )
		{
			socket->smState = SKT_CREATED;
			return s;
		}
		else if( type == SOCK_STREAM && protocol == IPPROTO_TCP )
		{
			socket->smState = SKT_CREATED;
			return s;
		}
		else
			return SOCKET_ERROR;
	}

	return SOCKET_ERROR;
}

/*********************************************************************
 * Function:        SOCKET socket( int af, int type, int protocol )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 *					level - must be SOL_SOCKET
 *					optname - option to configure.  The possible values are
 *      				SO_SNDBUF    configures the send buffer size to use with 
 *									 send api for tcp sockets.
 *      				TCP_NODELAY  enable or disable Naggles algorithim for the 
 *									 socket.  By default Naggles algorithim is enabled.  
 *									 To turn it off, use a non-zero value for the optval data.
 *					optval - pointer to the option data.
 *					optlen - length of option data
 *
 * Output:          New socket descriptor.
 * Side Effects:    None
 * Overview:        This function creates a new socket
 * Note:
 ********************************************************************/
int setsockopt( SOCKET s, int level, int optname,char * optval, int optlen )
{
	struct MCSocket *pSock;

	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	pSock = &SocketArray[s];
	
	if( level != SOL_SOCKET || optlen < sizeof(int) )
		return SOCKET_ERROR;
		
	switch( optname )
	{
	//////////////////////
	case SO_SNDBUF:
		pSock->TxBfrSize = *(int*)optval;
		return 0;
		
	//////////////////////
	case TCP_NODELAY:
		if( *optval )
			pSock->TcpNoDelay = TRUE;
		else
			pSock->TcpNoDelay = FALSE;
			
		return 0;
	}
	return SOCKET_ERROR;
}


/*********************************************************************
 * Function:        int listen( SOCKET s, int backlog )
 *
 * PreCondition:    None
 * Input:			s - Socket identifier
 *					backlog - maximum number of connection requests that can be queued
 * Output:          returns 0 on success, else return SOCKET_ERROR on error. 
 * Side Effects:    None
 * Overview:        The listen function sets the specified socket in a listen mode.  
 *					Calling the listen function indicates that the application is ready to 
 *					accept connection requests arriving at a socket of type SOCK_STREAM. 
 *					The connection request is queued (if possible) until accepted with an 
 *					accept function. The backlog parameter defines the maximum number of 
 *					pending connections that may be queued.
 * Note:
 ********************************************************************/
int listen( SOCKET s, int backlog )
{
	struct MCSocket *ps;

	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	ps = &SocketArray[s];

	if ( ps->smState != SKT_BOUND || ps->SocketType != SOCK_STREAM )
		return SOCKET_ERROR;

	// put this socket in listen mode
	ps->smState             = TCP_LISTEN;
	ps->backlog				= backlog;
	return 0;
}


/*********************************************************************
 * Function:        SOCKET accept( SOCKET s, struct sockaddr * addr, int * addrlen )
 *
 * PreCondition:    None
 * Input:			s  	    Socket descriptor returned from a previous call to socket.  
 *							must be bound to a local name and in listening mode.
 *					name    pointer to the the sockaddr structure that will receive 
 *							the connecting node IP address and port number.
 *					namelen A value-result parameter and should initially contain 
 *							the amount of space pointed to by name; on return it contains 
 *							the actual length (in bytes) of the name returned. 
 * Output:          If the accept() function succeeds, it returns a non-negative integer 
 *					that is a descriptor for the accepted socket. Otherwise, the value 
 *					SOCKET_ERROR is returned. 
 * Side Effects:    None
 * Overview:        The accept function is used to accept a connection request queued 
 *					for a listening socket.  If a connection request is pending, accept 
 *					removes the request from the queue, and a new socket is created for 
 *					the connection. The original listening socket remains open and 
 *					continues to queue new connection requests.  The socket s must be 
 *					a SOCK_STREAM type socket.
 * Note:
 ********************************************************************/
SOCKET accept( SOCKET s,
    			struct sockaddr * addr,
    			int * addrlen )
{
	struct MCSocket *pListenSock;

	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	pListenSock = &SocketArray[s]; /* Get the pointer to listening socket */

	if ( pListenSock->smState != TCP_LISTEN )
		return SOCKET_ERROR;

	return TCPAccept( pListenSock, addr, addrlen );
}



/*********************************************************************
 * Function:        int bind( SOCKET s, const struct sockaddr * name, int namelen )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 *					name - pointer to the the sockaddr structure containing the 
 *							local address of the socket.
 *					namelen - length of the sockaddr structure.
 * Output:          If bind is successful, a value of 0 is returned. A return value 
 *					of SOCKET_ERROR indicates an error.
 * Side Effects:    None
 * Overview:        The bind function assigns a name to an unnamed socket.  The name 
 *					represents the local address of the communication endpoint. 
 *					For sockets of type SOCK_STREAM, the name of the remote endpoint 
 *					is assigned when a connect or accept function is executed.
 * Note:
 ********************************************************************/
int bind( SOCKET s, const struct sockaddr * name, int namelen )
{
    struct MCSocket *socket;
	struct sockaddr_in *local_addr;
	int lPort;
		
	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	socket = &SocketArray[s];

	if( socket->smState != SKT_CREATED ) //only work with recently created socket
		return SOCKET_ERROR;

	if( namelen < sizeof(struct sockaddr_in) )
		return SOCKET_ERROR;

	local_addr = (struct sockaddr_in *)name;

	lPort = local_addr->sin_port;
	if( lPort == 0 ) //pick a port
	{
		lPort = gAutoPortNumber++;
		if( gAutoPortNumber == 5000 ) //then reset the port numbers
			gAutoPortNumber = 1024;
	}

	socket->localPort 	= lPort;
	socket->localIP 	= local_addr->sin_addr.S_un.S_addr;
	socket->smState 	= SKT_BOUND;
	return 0; //success
}

/*********************************************************************
 * Function:        int connect( SOCKET s, struct sockaddr * name, int namelen )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 *					name - pointer to the the sockaddr structure containing the 
 *							local address of the socket.
 *					namelen - length of the sockaddr structure.
 * Output:          If the connect() function succeeds, it returns 0. Otherwise, 
 *					the value SOCKET_ERROR is returned to indicate an error condition.
 *					For stream based socket, if the connection is not established yet, 
 *					connect returns SOCKET_CNXN_IN_PROGRESS.
 * Side Effects:    None
 * Overview:        The connect function assigns the address of the peer communications
 *					endpoint.  For stream sockets,  connection is established between 
 *					the endpoints. For datagram sockets, an address filter is established 
 *					between the endpoints until changed with another connect() function.
 * Note:
 ********************************************************************/
int connect( SOCKET s, struct sockaddr * name, int namelen )
{
	struct MCSocket *pSock;
	struct sockaddr_in *addr;
	DWORD remoteIP;
	WORD remotePort;

	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	pSock = &SocketArray[s];

	if( pSock->smState < SKT_BOUND ) //only work with sockets bound to a local ip & port
		return SOCKET_ERROR;

	if( namelen < sizeof(struct sockaddr_in) )
		return SOCKET_ERROR;

	addr = (struct sockaddr_in *)name;

	remotePort 	= addr->sin_port;		
	remoteIP 	= addr->sin_addr.S_un.S_addr;
	
	if( remoteIP == 0 || remotePort == 0 ) 
		return SOCKET_ERROR;
		
	if( pSock->SocketType == SOCK_STREAM )
	{
 		if( pSock->smState >= TCP_EST )
 			return 0; // already established
		
		if( pSock->smState == TCP_SYN_SENT )
			return SOCKET_CNXN_IN_PROGRESS;
		
		if( pSock->smState == SKT_BOUND )
		{
		    pSock->remotePort = remotePort;
			pSock->remoteIP = remoteIP;		
			TCPInitiateCnxn(pSock);
			return SOCKET_CNXN_IN_PROGRESS;
		}
		else
			return SOCKET_ERROR;
	}
	else
	{	
		// UDP: remote port is used as a filter only.  no need to call
		// 'connect' if recieving from a number of host
	    pSock->remotePort = remotePort;
	    pSock->remoteIP = remoteIP;
		
		return 0; //success
	}
}


/*********************************************************************
 * Function:        int MPSocketTxReady( SOCKET s )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 * Output:          0 = success
 *					SOCKET_ERROR = invalid socket.
 *					SOCKET_TX_NOT_READY = TCP flow control Tx not ready
 * Side Effects:    None
 * Overview:        For Stream sockets, this fucntion indicate if TCP flow 
 *					control mechanism will allow more packets to be sent 
 *					when send api is called. For Datagram sockets, this 
 *					function is of no value.
 * Note:
 ********************************************************************/
int MPSocketTxReady( SOCKET s )
{
	struct MCSocket *pSock;

	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	pSock = &SocketArray[s];

	if( pSock->SocketType == SOCK_DGRAM )
		return 0;
	else
		return TCPIsTxReady( pSock );	
}

/*********************************************************************
 * Function:        int MPSocketConnected( SOCKET s )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 * Output:          0 = success
 *					SOCKET_ERROR = invalid socket.
 *					SOCKET_CNXN_IN_PROGRESS = TCP flow control Tx not ready
 * Side Effects:    None
 * Overview:        For stream type socket, the function returns 0 if the connection 
 *					is established.  If connection establish process is not yet 
 *					complete the function returns SOCKET_CNXN_IN_PROGRESS.  
 *					The MpSocketConnected allows the application to check for connection 
 *					status before assembling the transmit data.
 * Note:
 ********************************************************************/
int MPSocketConnected( SOCKET s )
{
	struct MCSocket *pSock;

	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	pSock = &SocketArray[s];

	if( pSock->smState == TCP_EST )
		return 0; //success
	else if( pSock->smState == TCP_SYN_SENT )
		return SOCKET_CNXN_IN_PROGRESS;
	else
		return SOCKET_ERROR;
}

#if defined(API_00833_SUPPORT)

///////////////////////////////////////
int FlushTCP( SOCKET s )
{
	struct MCSocket *pSock;

	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	pSock = &SocketArray[s];

	FlushTxBuffer( pSock, PSH|ACK );
	return 0;
}

/*********************************************************************
 * Function:        int MPUDPRxReady( SOCKET s )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 * Output:          Returns TRUE if Rx Data is available.
 *					
 * Side Effects:    None
 * Overview:        This function checks for Revceive data availability on a 
 *					datagram socket.
 *					
 * Note:
 ********************************************************************/
BOOL MPUDPRxReady( SOCKET s )
{
    struct MCSocket *pSock;
    
	if( s >= MAX_SOCKET )
		return FALSE;

	pSock = &SocketArray[s];

	if( pSock->SocketType != SOCK_DGRAM )
		return FALSE;

	return UDPSetRxActivePacket(pSock);
}

BOOL MPTCPRxReady( SOCKET s )
{
    struct MCSocket *pSock;
    
	if( s >= MAX_SOCKET )
		return FALSE;

	pSock = &SocketArray[s];

	if( pSock->SocketType != SOCK_STREAM )
		return FALSE;

	return (TCPIsRxReady(pSock) == 0);
}

/*********************************************************************
 * Function:        int MPUDPDiscardRxPkt( SOCKET s )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 * Output:          None
 *					
 * Side Effects:    None
 * Overview:        This function deallocate the active Rx packet. 
 *					
 * Note:
 ********************************************************************/
void MPUDPDiscardRxPkt( SOCKET s )
{
    struct MCSocket *pSock;
    
	if( s >= MAX_SOCKET )
		return;

	pSock = &SocketArray[s];

	if( pSock->SocketType != SOCK_DGRAM )
		return;

	return UDPDiscardRxActivePacket(pSock);
}
int MPDiscardTCPActiveRX( SOCKET s )
{
    struct MCSocket *pSock;
    
	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	pSock = &SocketArray[s];

	if( pSock->SocketType != SOCK_STREAM )
		return SOCKET_ERROR;

	return TCPDiscardActiveRX( pSock );
}

#endif

/*********************************************************************
 * Function:        int send( SOCKET s, const char* buf, int len, int flags )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 * 					buf - application data buffer containing data to transmit
 *					len - length of data in bytes
 *					flags - message flags. Currently this field is not supported and must be 0.
 * Output:          On success, send returns number of bytes sent.  In case of error, one of 
 * 					the following values is returned:
 *
 *					SOCKET_BFR_ALLOC_ERROR
 *					No memory is available to allocate packet buffer.
 *					
 *					SOCKET_ERROR
 *					General error code. check format of address srtucture and also make 
 *					sure socket descriptor is correct. 
 *					
 *					SOCKET_TX_NOT_READY
 *					The TCP transmit functionality is temporarily disabled because of the
 *					TCP flow control mechanism.
 *					
 *					SOCKET_MAX_LEN_ERROR
 *					The maximum length of the data buffer must be less than the MTU value 
 *					which for ethernet is 1500 bytes.
 *
 * Side Effects:    None
 * Overview:        The send function is used to send outgoing data on an already connected 
 *					socket. This function is normally used to send a reliable, ordered stream 
 *					of data bytes on a socket of type SOCK_STREAM, but can also be used to send 
 *					datagrams on a socket of type SOCK_DGRAM.
 * Note:
 ********************************************************************/
int send( SOCKET s, const char* buf, int len, int flags )
{
    struct MCSocket *socket;

	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	socket = &SocketArray[s];

	if( socket->SocketType == SOCK_DGRAM )
	{
		if( socket->smState < SKT_BOUND ) //must create and bind before send
			return SOCKET_ERROR;

		return UDPTransmit( (char* )buf, len, socket->localPort, socket->remoteIP, socket->remotePort );
	}
	else
	{
		return TCPTransmitData( socket, (char* )buf, len );
	}	
}

/*********************************************************************
 * Function:        int sendto( SOCKET s, const char * buf, int len, int flags, const struct sockaddr * to, int tolen )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 * 					buf - application data buffer containing data to transmit
 *					len - length of data in bytes
 *					flags - message flags. Currently this field is not supported and must be 0.
 *					to - pointer to the the sockaddr structure containing the destination address.
 *					tolen - length of the sockaddr structure.
 * Output:          On success, sendto returns number of bytes sent.  In case of error, one of 
 * 					the following values is returned:
 *
 *					SOCKET_BFR_ALLOC_ERROR
 *					No memory is available to allocate packet buffer.
 *					
 *					SOCKET_ERROR
 *					General error code. check format of address srtucture and also make 
 *					sure socket descriptor is correct. 
 *					
 *					SOCKET_MAX_LEN_ERROR
 *					The maximum length of the data buffer must be less than the MTU value 
 *					which for ethernet is 1500 bytes.
 *
 * Side Effects:    None
 * Overview:        The sendto function is used to send outgoing data on a socket 
 *					of type datagram.  The destination address is given by to and 
 *					tolen.  If no memory block is available to create the datagram, 
 *					the function returns an error code.
 * Note:
 ********************************************************************/
int sendto( SOCKET s, const char * buf, int len, int flags, const struct sockaddr * to, int tolen )
{
    struct MCSocket *socket;
	struct sockaddr_in *addr;
	DWORD remoteIP;
	WORD remotePort;

	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	socket = &SocketArray[s];

	if( tolen < sizeof(struct sockaddr_in) )
		return SOCKET_ERROR;

	addr = (struct sockaddr_in *)to;

	remotePort 	= addr->sin_port;		
	remoteIP 	= addr->sin_addr.S_un.S_addr;

	if( socket->SocketType == SOCK_DGRAM )
	{
		if( socket->smState < SKT_BOUND ) //must create and bind before send
			return SOCKET_ERROR;

		return UDPTransmit( (char*)buf, len, socket->localPort, remoteIP, remotePort );
	}
	else
	{
		return SOCKET_ERROR;
	}
}

/*********************************************************************
 * Function:        int recv( SOCKET s, char * buf, int len, int flags )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 *					buf - application data receive buffer
 *					len - buffer length in bytes
 *					flags - message flags. Currently this field supports only the 
 						MSG_PEEK flag.
 * Output:          If recv is successful, the number of bytes copied to application 
 *					buffer buf is returned. A value of zero indicates no data available.  
 *					A return value of SOCKET_ERROR (-1) indicates an error condition.
 * Side Effects:    None
 * Overview:        The recv() function is used to receive incoming data that has been 
 *					queued for a socket. This function can be used with both datagram 
 *					and stream type sockets.  If the available data is too large to fit 
 *					in the supplied application buffer buf, excess bytes are discarded 
 *					in case of SOCK_DGRAM type sockets.  For SOCK_STREAM types, the data 
 *					is buffered internally so the application can retreive all data by 
 *					multiple calls of recv.  If MSG_PEEK flag is specified, no data is 
 *					deleted from the socket receive queue. 
 * Note:
 ********************************************************************/
int recv( SOCKET s, char * buf, int len, int flags )
{
	return recvfrom( s, buf, len, flags, NULL, NULL );
}

#if defined(API_00833_SUPPORT)

/********************************************************************/
/********************************************************************/
int MPReadUDPByte( SOCKET s, char* buf, int flags )
{
    struct MCSocket *socket;
	int BytesRead = 0;
	
	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	socket = &SocketArray[s];

	if( socket->SocketType == SOCK_DGRAM )
	{
		if( socket->smState < SKT_BOUND ) //must call bind before recv
			return SOCKET_ERROR;

		BytesRead = UDPGetBfrData( socket, buf, 1 );
	}
	else	//TCP
	{
		return 0;	
	}
	
	return BytesRead;
}	


/********************************************************************/
/********************************************************************/
void MPSetUDPRemoteAddress( SOCKET s, int remPort, DWORD remIP )
{
    struct MCSocket *pSock;
    
	if( s >= MAX_SOCKET )
		return;

	pSock = &SocketArray[s];

	if( pSock->SocketType != SOCK_DGRAM )
		return;

	pSock->remotePort		= remPort;
	pSock->remoteIP			= remIP;
}

/***********************************************************/
/***********************************************************/
int MPWriteUDPByte( SOCKET s, char dat )
{
    struct MCSocket *pSock;
	
	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	pSock = &SocketArray[s];

	if( pSock->SocketType == SOCK_DGRAM )
	{
		if( pSock->smState < SKT_BOUND ) //must call bind before recv
			return SOCKET_ERROR;

		return UDPWriteByte( pSock, dat );
	}
	else	//TCP
	{
		return SOCKET_ERROR;	
	}
}

/***********************************************************/
/***********************************************************/
int MPXmitUDPActivePkt( SOCKET s )
{
    struct MCSocket *pSock;
	
	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	pSock = &SocketArray[s];

	if( pSock->SocketType == SOCK_DGRAM )
	{
		if( pSock->smState < SKT_BOUND ) //must call bind before recv
			return SOCKET_ERROR;

		return UDPXmitActivePkt( pSock );
	}
	else	//TCP
	{
		return SOCKET_ERROR;	
	}
}

#endif

/*********************************************************************
 * Function:        int recvfrom( SOCKET s, char * buf, int len, int flags, struct sockaddr * from, int * fromlen )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 *					buf - application data receive buffer
 *					len - buffer length in bytes
 *					flags - message flags. Currently this field supports only the 
 *						MSG_PEEK flag.
 *					from - pointer to the the sockaddr structure that will be filled 
 *						in with the destination address.
 *					fromlen - size of buffer pointed by from.
 * Output:          If recvfrom is successful, the number of bytes copied to application 
 *					buffer buf is returned. A value of zero indicates no data available.  
 *					A return value of SOCKET_ERROR (-1) indicates an error condition.
 * Side Effects:    None
 * Overview:        The recvfrom() function is used to receive incoming data that has been 
 *					queued for a socket. This function can be used with both datagram 
 *					and stream type sockets.  If the available data is too large to fit 
 *					in the supplied application buffer buf, excess bytes are discarded 
 *					in case of SOCK_DGRAM type sockets.  For SOCK_STREAM types, the data 
 *					is buffered internally so the application can retreive all data by 
 *					multiple calls of recvfrom.  If MSG_PEEK flag is specified, no data is 
 *					deleted from the socket receive queue. 
 * Note:
 ********************************************************************/
int recvfrom( SOCKET s, char * buf, int len, int flags, struct sockaddr * from, int * fromlen )
{
    struct MCSocket *socket;
	NET_PKT* rcvPkt;
	int temp, BytesRead = 0;
	UDP_HEADER* pUDP;
	struct sockaddr_in *rem_addr;
	
	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	socket = &SocketArray[s];

	if( socket->SocketType == SOCK_DGRAM )
	{
		if( socket->smState < SKT_BOUND ) //must call bind before recv
			return SOCKET_ERROR;

		if( socket->RxMsgQueue.PktCount )
		{
			if( flags & MSG_PEEK )
				rcvPkt = socket->RxMsgQueue.pFirst;
			else
			{
				//retreive the Datagram from Rx Fifo
				Que_GetHead( &socket->RxMsgQueue, &rcvPkt );
			}
			
			//extract data
			BytesRead = UDPExtractData( rcvPkt, buf, len );
					
			pUDP = (UDP_HEADER*)rcvPkt->pTransportLayer;

			if( from && fromlen && *fromlen >= sizeof(struct sockaddr_in) )
			{
                DWORD   src_ip;

                src_ip = _arrayToDword(rcvPkt->pSrcIP);

				 rem_addr = (struct sockaddr_in *)from;
				 *fromlen = sizeof(struct sockaddr_in);
				 
				 rem_addr->sin_family 			 = AF_INET;
				 rem_addr->sin_port				 = pUDP->SourcePort;
				 rem_addr->sin_addr.S_un.S_addr  = src_ip;
		
				 for( temp = 0; temp < 8; temp++ )
				 	rem_addr->sin_zero[temp] = 0;
			}

			if( flags & MSG_PEEK )
				BytesRead = pUDP->Length;
			else
			{
				//Release packet memory
				DeAllocateBuffer( rcvPkt );
			}
		}
	}
	else	//TCP
	{
		if( flags & MSG_PEEK )
			BytesRead = TCPPeekData( socket, buf, len );
		else
			BytesRead = TCPExtractData( socket, buf, len );

		if( from && fromlen && *fromlen >= sizeof(struct sockaddr_in) )
		{
			 rem_addr = (struct sockaddr_in *)from;
			 *fromlen = sizeof(struct sockaddr_in);
			 
			 rem_addr->sin_family 			 = AF_INET;
			 rem_addr->sin_port				 = socket->remotePort;
			 rem_addr->sin_addr.S_un.S_addr  = socket->remoteIP;
	
			 for( temp = 0; temp < 8; temp++ )
			 	rem_addr->sin_zero[temp] = 0;
		}
	}

	return BytesRead;
}

/*********************************************************************
 * Function:        int closesocket( SOCKET s )
 *
 * PreCondition:    None
 * Input:			s - Socket descriptor returned from a previous call to socket
 * Output:          If closesocket is successful, a value of 0 is returned. 
 *					A return value of SOCKET_ERROR (-1) indicates an error.
 * Side Effects:    None
 * Overview:        The closesocket function closes an existing socket.  
 * 					This function releases the socket descriptor s.  
 *					Further references to s fails with SOCKET_ERROR code.  
 *					Any data buffered at the socket is discarded.  If the 
 *					socket s is no longer needed, closesocket() must be 
 *					called in order to release all resources associated 
 *					with s.
 * Note:
 ********************************************************************/
int closesocket( SOCKET s )
{
    struct MCSocket *socket;
	NET_PKT* rcvPkt;

	if( s >= MAX_SOCKET )
		return SOCKET_ERROR;

	socket = &SocketArray[s];

	if( socket->SocketType == SOCK_DGRAM )
	{
		if( socket->smState != SKT_CLOSED ) //this is actually in use
		{
			//empty rcv fifo
			while( Que_GetHead( &socket->RxMsgQueue, &rcvPkt ) )
				DeAllocateBuffer( rcvPkt );

			if( socket->pRxActivePacket )
				DeAllocateBuffer( socket->pRxActivePacket );

			if( socket->pTxActivePacket )
				DeAllocateBuffer( socket->pTxActivePacket );

			SocketInitialize(s); //initialize the socket memory
			return 0; //success
		}
		else
			return SOCKET_ERROR;
	}
	else //tcp sockets
	{
		TCPCloseSocket(socket);
		return 0; //success
	}	
}

/*********************************************************************
 * Function:        SOCKET FindTargetSocket( int SockType, DWORD SrcIP, 
 *					DWORD DestIP, WORD DestPort )
 *
 * PreCondition:    None
 * Input:		 	SockType - Datagram
 *					SrcIP - Source IP address
 *					DestIP	- Destination IP address
 *					DestPort - Detination Port
 * Output:          
 * Side Effects:    None
 * Overview:        Match the incomming packet with socket.
 * Note:
 ********************************************************************/
SOCKET FindTargetSocket( int SockType, DWORD SrcIP, DWORD DestIP, WORD DestPort )
{
    int s;
    struct MCSocket *socket = SocketArray;

    for ( s = 0; s < MAX_SOCKET; s++,socket++ )
    {
		if( socket->SocketType != SockType )
			continue;

		if( socket->smState < SKT_BOUND ) //socket not initialized
			continue;

        if( DestPort == socket->localPort && 	//Port must match
			( DestIP == socket->localIP || 		//exact match with IP
			  socket->localIP == IP_ADDR_ANY || //socket is accepeting all IP addresses (for multi-homed devices)
			  DestIP == IP_ADDR_BROADCAST ) )	//DestIP is a broadcast address
		{
			if( socket->remoteIP == IP_ADDR_ANY || ( socket->remoteIP && socket->remoteIP == SrcIP ) )
				return s;
		}
	}

	return -1;
}
