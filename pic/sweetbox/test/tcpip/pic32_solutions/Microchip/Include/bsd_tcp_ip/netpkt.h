/*********************************************************************
 *
 *            Packet Buffer Header File
 *
 *********************************************************************
 * FileName:        NetPkt.h
 * Description: 	internal packet buffer definition
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
 * $Id: NetPkt.h,v 1.4 2006/10/24 16:39:06 C12923 Exp $
 *
*/

#ifndef __NET_PKT_H__
#define __NET_PKT_H__

#include "bsd_tcp_ip\etcpcfg.h"
#include "bsd_tcp_ip\tick.h"

struct NetPacket
{
	struct NetPacket *pNext;
	BYTE*	pTransportLayer;
	BYTE*	pNetworkLayer;
	BYTE*	pMacLayer;

	BYTE*	pSrcMAC; 	//valid only for Rx Packets
	BYTE*	pSrcIP;		//valid only for Rx Packets
	BYTE*	pDestIP;	//valid only for Rx Packets
	struct 	NetPacket *pTcpNext;
	struct  MCSocket * pSocket;
	BYTE*	pData;

	DWORD   TCPACKNum;
	TICK	TimeStamp;	//Time when rcvd or queued for xmit
	TICK	ARPTickStart;
	BOOL	TxAutoDeAlloc;
	WORD	PktLen;
	BYTE	PktType;	//valid only for Rx Packets
	
	//real packet data
	//DO NOT ADD FIELDS BELOW THIS LINE	
    DWORD   pPkt[0];
}; 

typedef struct NetPacket NET_PKT;

#endif
