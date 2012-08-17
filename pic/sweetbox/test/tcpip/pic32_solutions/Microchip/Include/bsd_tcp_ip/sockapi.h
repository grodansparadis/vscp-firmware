/*********************************************************************
 *
 *            Socket API Header File
 *
 *********************************************************************
 * FileName:        SockAPI.h
 * Description: 	Socket API for eTCP Stack
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
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
 * $Id: sockAPI.h,v 1.4 2006/09/28 22:10:27 C12770 Exp $
 *
*/

#ifndef _SOCKET_API_HEADER_FILE
#define _SOCKET_API_HEADER_FILE

#include "bsd_tcp_ip\etcpcfg.h"

typedef int SOCKET;

#if defined(API_00833_SUPPORT)

typedef struct _NODE_INFO
{
    MAC_ADDR    MACAddr;
    IP_ADDR     IPAddr;
} NODE_INFO;


int MPReadUDPByte( SOCKET s, char* buf, int flags );
void MPSetUDPRemoteAddress( SOCKET s, int remPort, DWORD remIP );
int MPWriteUDPByte( SOCKET s, char dat );
int MPXmitUDPActivePkt( SOCKET s );

int MPDiscardTCPActiveRX( SOCKET s );
BOOL MPTCPRxReady( SOCKET s );

SOCKET FindTCPSocket( int Port, int State );
SOCKET FindListenSocket( int port );
int FlushTCP( SOCKET s );

#endif

#define MSG_PEEK        0x2             /* peek at incoming message */

// Level number for (get/set)sockopt() to apply to socket itself.
#define SOL_SOCKET      0xffff          /* options for socket level */

#define SO_SNDBUF       0x1001          /* send buffer size */
#define TCP_NODELAY     0x0001

#define AF_INET         2   // Internet Address Family - UDP, TCP, etc.

#define SOCK_STREAM		100
#define SOCK_DGRAM		110
 
#define IPPROTO_TCP		6               // tcp
#define IPPROTO_UDP		17              // user datagram protocol

#define IP_ADDR_ANY				0
#define IP_ADDR_BROADCAST 		0xffffffffu

#define INADDR_ANY              0x00000000
#define INADDR_LOOPBACK         0x7f000001
#define INADDR_BROADCAST        0xffffffff
#define INADDR_NONE             0xffffffff

#define ADDR_ANY                INADDR_ANY

#define INVALID_SOCK			(-1)
#define INVALID_SOCKET			(-1)
#define SOCKET_ERROR			(-1)
#define SOCKET_BFR_ALLOC_ERROR	(-2)
#define SOCKET_HOST_NO_ROUTE	(-3)
#define SOCKET_MAX_LEN_ERROR	(-4)
#define SOCKET_NOT_READY		(-5)
#define SOCKET_TX_NOT_READY		(-5) //for backward compatibility
#define SOCKET_CNXN_IN_PROGRESS	(-6)

struct in_addr {
	union {
	        struct { BYTE s_b1,s_b2,s_b3,s_b4; } S_un_b;
	        struct { WORD s_w1,s_w2; } S_un_w;
	        DWORD S_addr;
	} S_un;
	
#define s_addr  S_un.S_addr
                                /* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2
                                /* host on imp */
#define s_net   S_un.S_un_b.s_b1
                                /* network */
#define s_imp   S_un.S_un_w.s_w2
                                /* imp */
#define s_impno S_un.S_un_b.s_b4
                                /* imp # */
#define s_lh    S_un.S_un_b.s_b3
                                /* logical host */	
	
};

struct sockaddr {
        unsigned short	sa_family;              /* address family */
        char    sa_data[14];            /* up to 14 bytes of direct address */
};

struct sockaddr_in {
	short   sin_family;
	WORD	sin_port;
	struct  in_addr sin_addr;
	char    sin_zero[8];
};

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct sockaddr_in *LPSOCKADDR_IN;

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *PSOCKADDR;
typedef struct sockaddr *LPSOCKADDR;

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
SOCKET socket( int af, int type, int protocol );

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
int bind( SOCKET s, const struct sockaddr * name, int namelen );

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
int listen( SOCKET s, int backlog );

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
SOCKET accept( SOCKET s, struct sockaddr * addr, int * addrlen );

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
int connect( SOCKET s, struct sockaddr * name, int namelen );

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
int sendto( SOCKET s, const char * buf, int len, int flags, const struct sockaddr * to, int tolen );

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
int send( SOCKET s, const char* buf, int len, int flags );

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
int recvfrom( SOCKET s, char * buf, int len, int flags, struct sockaddr * from, int * fromlen );

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
int recv( SOCKET s, char * buf, int len, int flags );

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
int setsockopt( SOCKET s, int level, int optname,char * optval, int optlen );

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
int closesocket( SOCKET s );

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
int MPSocketConnected( SOCKET s );

#ifdef API_00833_SUPPORT

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
BOOL MPUDPRxReady( SOCKET s );

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
void MPUDPDiscardRxPkt( SOCKET s );

#endif

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
int MPSocketTxReady( SOCKET s );

/*********************************************************************
 * Function:        void InitStackMgr(void)
 *
 * PreCondition:    None
 * Input:			None
 * Output:          None
 * Side Effects:    None
 * Overview:        Initialize internal stack variables
 * Note:
 ********************************************************************/
void InitStackMgr();

/*********************************************************************
 * Function:        void StackMgrProcess(void)
 *
 * PreCondition:    None
 * Input:			None
 * Output:          None
 * Side Effects:    None
 * Overview:        Executes state machine and processes of stack
 * Note:
 ********************************************************************/
void StackMgrProcess();
/*********************************************************************
 * Function:        TCPIPSetDefaultAddr       
 *
 * PreCondition:    Call before InitStackMgr   
 *
 * Input:           None          
 *
 * Output:          None
 *
 * Side Effects:    Sets the IP address, IP mask, IP Gateway and
 *                  MAC address to the default values defined in
 *                  etcp.def file.
 ********************************************************************/
void TCPIPSetDefaultAddr(void);
/*********************************************************************
 * Function:            TCPIPSetIPAddr      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               addr1 - addr4 Ip address in bytes
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP address
 * 
 * Note:                Set the IP address to 10.10.33.201
 *                      TCPIPSetIPAddr( 10,
 *                                      10,
 *                                      33,
 *                                      201);
 ********************************************************************/
void TCPIPSetIPAddr(BYTE addr1, BYTE addr2, BYTE addr3, BYTE addr4);
/*********************************************************************
 * Function:            TCPIPSetIPAddrStr      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               ip_addr - Ip address in ASCII format
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP address
 * 
 * Note:                Set the IP address to 10.10.33.201
 *                      TCPIPSetIPAddrStr( "10.10.33.201");
 ********************************************************************/
void TCPIPSetIPAddrStr(BYTE *ip_addr);
/*********************************************************************
 * Function:            TCPIPGetIPAddr     
 *
 * PreCondition:        None
 *
 * Input:               None
 *
 * Output:              IP address as a 32 bit value
 *
 * Side Effects:        None
 * 
 * Note:                DWORD_VAL curr_ip =   TCPIPGetIPAddr();         
 ********************************************************************/
DWORD TCPIPGetIPAddr(void);
/*********************************************************************
 * Function:            TCPIPSetIPMask      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               addr1 - addr4 Ip mask in bytes
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP mask
 * 
 * Note:                Set the IP mask to 255.255.255.201
 *                      TCPIPSetIPMask( 255,
 *                                      255,
 *                                      255,
 *                                      201);
 ********************************************************************/
void TCPIPSetIPMask(BYTE addr1, BYTE addr2, BYTE addr3, BYTE addr4);
/*********************************************************************
 * Function:            TCPIPSetIPMaskStr      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               ip_addr - Ip mask in ASCII format
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP mask
 * 
 * Note:                Set the IP mask to 255.255.255.201
 *                      TCPIPSetIPMaskStr( "255.255.255.201");
 ********************************************************************/
void TCPIPSetIPMaskStr(BYTE *ip_addr);
/*********************************************************************
 * Function:            TCPIPGetIPMask     
 *
 * PreCondition:        None
 *
 * Input:               None
 *
 * Output:              IP mask as a 32 bit value
 *
 * Side Effects:        None
 * 
 * Note:                DWORD_VAL curr_ip =   TCPIPGetIPMask();         
 ********************************************************************/
DWORD TCPIPGetIPMask(void);
/*********************************************************************
 * Function:            TCPIPSetIPGateway      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               addr1 - addr4 Ip gateway in bytes
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP gateway
 * 
 * Note:                Set the IP gateway to 10.10.33.201
 *                      TCPIPSetIPGateway(  10,
 *                                          10,
 *                                          33,
 *                                          201);
 ********************************************************************/
void TCPIPSetIPGateway(BYTE addr1, BYTE addr2, BYTE addr3, BYTE addr4);
/*********************************************************************
 * Function:            TCPIPSetIPGatwayStr      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               ip_addr - Ip gateway in ASCII format
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP gateway
 * 
 * Note:                Set the IP gateway to 10.10.33.201
 *                      TCPIPSetIPGatwayStr( "10.10.33.201");
 ********************************************************************/
void TCPIPSetIPGatwayStr(BYTE *ip_addr);
/*********************************************************************
 * Function:            TCPIPGetIPGateway     
 *
 * PreCondition:        None
 *
 * Input:               None
 *
 * Output:              IP gateway as a 32 bit value
 *
 * Side Effects:        None
 * 
 * Note:                DWORD_VAL curr_ip =   TCPIPGetIPGateway();         
 ********************************************************************/
DWORD TCPIPGetIPGateway(void);
/*********************************************************************
 * Function:            TCPIPSetMACAddr      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               addr1 - addr6 MAC address in bytes
 *
 * Output:              None
 *
 * Side Effects:        Sets the MAC address
 * 
 * Note:                Set the MAC address to 00-04-a3-00-00-02
 *                      TCPIPSetMACAddr(    0x00,
 *                                          0x04,
 *                                          0xa3,
 *                                          0x00,
 *                                          0x00,
 *                                          0x02);
 ********************************************************************/
void TCPIPSetMACAddr(BYTE addr1, BYTE addr2, BYTE addr3, BYTE addr4, BYTE addr5, BYTE addr6);
/*********************************************************************
 * Function:            TCPIPSetMACAddrStr      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               mac_addr - MAC address in ASCII format
 *
 * Output:              None
 *
 * Side Effects:        Sets the MAC address
 * 
 * Note:                Set the MAC address to 00-04-a3-00-00-02
 *                      TCPIPSetMACAddrStr( "00-04-a3-00-00-02");
 ********************************************************************/
void TCPIPSetMACAddrStr(BYTE *mac_addr);
/*********************************************************************
 * Function:            TCPIPGetMACAddr     
 *
 * PreCondition:        None
 *
 * Input:               MAC address as a 6 byte array value
 *
 * Output:              None
 *
 * Side Effects:        None
 * 
 * Note:                MAC_ADDR mac_addr;
 *                      TCPIPGetMACAddr(&mac_addr);         
 ********************************************************************/
void TCPIPGetMACAddr(MAC_ADDR *mac_addr);
#endif
