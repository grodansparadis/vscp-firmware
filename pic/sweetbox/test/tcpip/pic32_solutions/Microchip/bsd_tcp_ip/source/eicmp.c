/*********************************************************************
 *
 *                  ICMP Module for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        ICMP.C
 * Processor:       PIC24/DAYTONA
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
 *
 * $Id: eICMP.c,v 1.2 2006/10/24 16:39:07 C12923 Exp $
 *
 ********************************************************************/
 
#include "bsd_tcp_ip\block_mgr.h"
#include "bsd_tcp_ip\eip.h"
#include "bsd_tcp_ip\emac.h"
#include "string.h"
#include "bsd_tcp_ip\gpfunc.h"

typedef enum _ICMP_CODE
{
    ICMP_ECHO_REPLY = 0,
    ICMP_ECHO_REQUEST = 8
} ICMP_MSG_TYPE, ICMP_REQUESTS;

#pragma push 
#pragma pack(1)
typedef struct _ICMP_HEADER
{
    BYTE    Type;
    BYTE    Code;
    WORD    Checksum;
    WORD    Identifier;
    WORD    SequenceNumber;
    DWORD   Data[0];
} ICMP_HEADER;
#pragma pop

/*
* Function Prototypes
*/
static void ICMP_Send(	ICMP_MSG_TYPE mType,
						DWORD DestIP,
             			BYTE *icmpData,
             			BYTE extraLen,
             			WORD id,
             			WORD seq);

/*********************************************************************
 * Function:        void ICMP_RxHandler( NET_PKT *sbfr )
 *
 * PreCondition:    None
 * Input:			sbfr - Pointer to New ICMP Packet received
 * Output:          None
 * Side Effects:    None
 * Overview:        This function process new ICMP packet
 * Note:
 ********************************************************************/
void ICMP_RxHandler( NET_PKT *sbfr )
{
	ICMP_HEADER *packet;

	packet = (ICMP_HEADER *)sbfr->pTransportLayer;

	if( checksum16( sbfr->pTransportLayer, sbfr->PktLen ) != 0xFFFF ) // must be -1
	{
		DeAllocateBuffer(sbfr);
		return;
	}	

	if( packet->Type == ICMP_ECHO_REQUEST )
	{

        DWORD   src_ip;

        src_ip = _arrayToDword(sbfr->pSrcIP);

        ICMP_Send(	ICMP_ECHO_REPLY, src_ip,
					(sbfr->pTransportLayer + sizeof(ICMP_HEADER)),
		        	sbfr->PktLen - sizeof(ICMP_HEADER), 
					packet->Identifier, packet->SequenceNumber );
	}

	DeAllocateBuffer(sbfr);
}

/*********************************************************************
 * Function:        static void ICMP_Send(	ICMP_MSG_TYPE mType,
 *						DWORD DestIP,
 *            			BYTE *icmpData,
 *            			BYTE extraLen,
 *            			WORD id,
 *            			WORD seq)
 *
 * PreCondition:    None
 * Input:			mType - icmp message type
 *					DestIP - destination ip address
 					icmpData - pointer to icmp extra data recevied
 *					extraLen - data length of extra data
 *					id - icmp identifier
 * 					seq - icmp sequence
 * Output:          None
 * Side Effects:    None
 * Overview:        This function packes and send icmp (ping) message
 * Note:
 ********************************************************************/
static void ICMP_Send(	ICMP_MSG_TYPE mType,
						DWORD DestIP,
             			BYTE *icmpData,
             			BYTE extraLen,
             			WORD id,
             			WORD seq)
{

    ICMP_HEADER* pICMP;
    WORD ICMPLen;

	NET_PKT* sbfr = AllocSocketBuffer( sizeof(ICMP_HEADER) + extraLen );
	if( sbfr == NULL )	//make sure we have good buffer
	{
		return;
	}

	ICMPLen = sizeof(ICMP_HEADER) + extraLen;

	if( !IP_FormatPacket( sbfr, DestIP, IP_PROT_ICMP, ICMPLen ) )
	{
		DeAllocateBuffer( sbfr );
		return;
	}

	pICMP = 	(ICMP_HEADER*)sbfr->pTransportLayer;

    pICMP->Code             = 0;
    pICMP->Type             = mType;
    pICMP->Checksum         = 0;
    pICMP->Identifier       = id;
    pICMP->SequenceNumber   = seq;

    memcpy( pICMP->Data, icmpData, extraLen);

    pICMP->Checksum        = ~checksum16((BYTE*)pICMP,
                                    ICMPLen);
	IP_TransmitPkt( sbfr );
	
}

