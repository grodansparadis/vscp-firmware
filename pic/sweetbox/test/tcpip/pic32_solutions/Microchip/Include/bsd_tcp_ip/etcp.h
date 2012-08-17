/*********************************************************************
 *
 *            TCP Header File
 *
 *********************************************************************
 * FileName:        eTCP.h
 * Description: 	TCP layer header file
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
 * $Id: eTCP.h,v 1.1 2006/09/28 22:10:29 C12770 Exp $
 *
*/

#include "bsd_tcp_ip\sockapi.h"

void TCP_RxHandler( NET_PKT *RxPkt );
void TCPInitiateCnxn( struct MCSocket* pSock );

int TCPExtractData( struct MCSocket* pSock, BYTE* buf, int len );
int TCPPeekData( struct MCSocket* pSock, BYTE* buf, int len );

int TCPTransmitData( struct MCSocket* pSock, BYTE* buf, int len );
void TCPCloseSocket( struct MCSocket *pSock );
int TCPAccept( struct MCSocket *pListenSock,
				struct sockaddr * addr,
				int * addrlen );

int TCPIsTxReady( struct MCSocket* pSock );

#if defined(API_00833_SUPPORT)

BOOL FlushTxBuffer( struct MCSocket *pSocket, BYTE TxFlags );
int TCPIsRxReady( struct MCSocket* pSock );
int TCPDiscardActiveRX( struct MCSocket* pSock );

#endif
