/*********************************************************************
 *
 *            Packet Buffer Queue Header File
 *
 *********************************************************************
 * FileName:        Pkt_Queue.h
 * Description: 	internal packet buffer queue definition
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
 * $Id: pkt_queue.h,v 1.3 2006/09/12 18:38:37 C12770 Exp $
 *
*/


#ifndef __PKT_QUEUE_H__
#define __PKT_QUEUE_H__

#include "bsd_tcp_ip\netpkt.h"

typedef struct PktQueue
{
	NET_PKT* pFirst;
	NET_PKT* pLast;
	WORD	PktCount;
}PktQueue, *PktQueuePTR;

//typedef struct PktQueue* ;

void Que_Init   ( PktQueuePTR pQue );
void Que_AddTail( PktQueuePTR pQue, NET_PKT* pPkt );
BOOL Que_GetHead( PktQueuePTR pQue, NET_PKT** pPkt );
BOOL Que_IsEmpty( PktQueuePTR pQue );
BOOL Que_IsQueued( PktQueuePTR pQue, NET_PKT* pPkt );

#endif
