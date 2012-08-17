/*********************************************************************
 *
 *                  Packet Queue module for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        pkt_queue.c
 * Processor:       DAYTONA/PIC24F
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement:
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended 
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip's PICmicro Microcontroller products. 
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
 * $Id: pkt_queue.c,v 1.3 2006/09/12 18:38:37 C12770 Exp $
 *
 ********************************************************************/

#include "bsd_tcp_ip\pkt_queue.h"
#include <string.h>

/*********************************************************************
 * Function:        void Que_Init( PktQueuePTR pQue )
 *
 * PreCondition:    None
 * Input:			pQue - pointer to queue
 * Output:          None
 * Side Effects:    None
 * Overview:        Initialize IP local variables
 * Note:
 ********************************************************************/
void Que_Init( PktQueuePTR pQue )
{
	pQue->pFirst = NULL;
	pQue->pLast = NULL;
	pQue->PktCount = 0;
}

BOOL Que_IsQueued( PktQueuePTR pQue, NET_PKT* pPkt )
{
	NET_PKT* pNode;

	pNode = pQue->pFirst;
	
	while( pNode )
	{
		if( pNode == pPkt )
			return TRUE;
		
		pNode = pNode->pNext;
	}
	
	return FALSE;
}

void Que_AddTail( PktQueuePTR pQue, NET_PKT* pPkt )
{
	if( pQue->pLast == NULL )
	{
		pQue->pFirst = pPkt;
		pQue->pLast = pPkt;
	}
	else
	{
		pQue->pLast->pNext = pPkt;
		pQue->pLast = pPkt;
	}

	pQue->PktCount++;
	pPkt->pNext = NULL;
}


BOOL Que_GetHead( PktQueuePTR pQue, NET_PKT** pPkt )
{
	NET_PKT* pNode;

	if( pQue->pFirst == NULL )
		return FALSE;

	pNode = pQue->pFirst;
	*pPkt = pNode;

	pQue->pFirst = pNode->pNext;
	pNode->pNext = NULL;

	if( pQue->pLast == pNode )
	{
		pQue->pFirst = NULL;
		pQue->pLast = NULL;
	}

	pQue->PktCount--;
	return TRUE;
}
BOOL Que_IsEmpty( PktQueuePTR pQue )
{
	return( pQue->pFirst == pQue->pLast );
}
