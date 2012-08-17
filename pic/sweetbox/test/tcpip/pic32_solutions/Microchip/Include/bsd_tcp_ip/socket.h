/*********************************************************************
 *
 *                  Socket layer for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        Socket.h
 * Description: 	Socket Layer header
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
 * $Id: socket.h,v 1.5 2006/10/24 16:39:08 C12923 Exp $
 *
*/



#ifndef __MCHP_SOCKET_API__
#define __MCHP_SOCKET_API__

#include "bsd_tcp_ip\etcpcfg.h"
#include "bsd_tcp_ip\emac.h"
#include "bsd_tcp_ip\ether.h"
#include "bsd_tcp_ip\pkt_queue.h"
#include "bsd_tcp_ip\eip.h"

/*
 * TCP States as defined by rfc793
 */
typedef enum _TCP_STATE
{
    SKT_CLOSED,		// 0
	SKT_CREATED,	// 1
	SKT_BOUND,		// 2
    TCP_LISTEN,		// 3
    TCP_SYN_RCVD,	// 4
    TCP_SYN_SENT,	// 5
    TCP_EST,		// 6
    TCP_CLOSE_WAIT, // 7
    TCP_LAST_ACK,	// 8
    TCP_FIN_WAIT_1,	// 9
    TCP_RESET_RCVD,	// 10
    TCP_CLOSE,		// 11
    TCP_INVALID,	// 12
} TCP_STATE;

/*
 * TCP Flags as defined by rfc793
 */
#define FIN     0x01
#define SYN     0x02
#define RST     0x04
#define PSH     0x08
#define ACK     0x10
#define URG     0x20
#define ACK_SENT     0x80	//Internal to this stack

struct MCSocket
{
	int			    SocketType;
	NET_PKT         *pRxActivePacket;
	NET_PKT         *pTxActivePacket;
	NET_PKT		    *pTCPListHead;
	NET_PKT		    *pTCPListTail;
	struct MCSocket *pParentSocket;
	PktQueue	    CnxnQueue;
	PktQueue	    RxMsgQueue;
	TCP_STATE	    smState;

	//for cnxn less, the remote address acts as a filter on the rcv datagrams.
	//discard packets that do not match the remote address.
	DWORD		remoteIP;
    WORD		remotePort;
    
	DWORD		RemIP; //used for the legacy api
	DWORD		ActiveRxSrcIP;
    WORD		ActiveRxSrcPort;
	
	//local address must be bind after socket is created
	DWORD		localIP;	//bind
    WORD		localPort;	//bind

	//Incoming packet FIFO

	// TCP Specific fields
	int			DataByteIndex;
	int			TxDataIndex;
	int			TxBfrSize;	
	BOOL		FirstSend;
	BOOL		TcpNoDelay;

	//PktQueue	AckPktList; 	//TCP List for pkt retransmission
	int			AckListCount;
	
	int			backlog;
	int			SocketID;
    DWORD 		TcpSEQ;  //our sequence number
    DWORD 		TcpACK; //peer sequence number

    BYTE 		RetryCount;
    TICK 		startTick;
    
	TICK		TCPAppCloseTimer;
	BOOL		TCPAppClosed;
	TICK		NagglesTimer;
    
};

#define INVALID_TCP_PORT        (0L)

extern struct MCSocket  SocketArray[MAX_SOCKET];
SOCKET FindTargetSocket( int SockType, DWORD SrcIP, DWORD DestIP, WORD DestPort );
void SocketInitialize( SOCKET s );
void SocketLayerInit();

#endif

