/*********************************************************************
 *
 *                  ARP module for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        eARP.c
 * Processor:       PIC24
 * Complier:        MPLAB C30
 *
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement:
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
 * $Id: eARP.c,v 1.2 2006/10/24 16:39:07 C12923 Exp $
 *
 ********************************************************************/


#include "bsd_tcp_ip\block_mgr.h"
#include "bsd_tcp_ip\ether.h"
#include "bsd_tcp_ip\route.h"
#include "bsd_tcp_ip\etcp.h"
#include "bsd_tcp_ip\emac.h"
#include "bsd_tcp_ip\gpfunc.h"
#include <string.h>
#include "bsd_tcp_ip\earp.h"


/*
 * ETHERNET packet type as defined by IEEE 802.3
 */
#define HW_ETHERNET             (0x0001)

typedef struct _ARP_PKT
{
    WORD        HardwareType;
    WORD        Protocol;
    BYTE        MACAddrLen;
    BYTE        ProtocolLen;
    WORD        Operation;
    BYTE    	SenderMACAddr[ETH_ADRLEN];
    BYTE        SenderIPAddr[4];
    BYTE    	TargetMACAddr[ETH_ADRLEN];
    BYTE        TargetIPAddr[4];
} ARP_PKT;


extern BYTE broadcastMAC[];
/*
 function prototypes
*/
void SwapARPPacket(ARP_PKT *p);


/*********************************************************************
 * Function:        void ARP_SendPkt( WORD OpCode, DWORD SenderIP, BYTE* pSenderMAC, 
 *						   	   DWORD TargetIP, BYTE* pTargetMAC )
 *
 * PreCondition:    None
 * Input:           OpCode		- Request or Responce Op-Code. see arp.h
 * 					SenderIP	- local IP Address
 * 					pSenderMAC	- Pointer to local MAC address string.
 * 					TargetIP	- Remote node IP Address
 * 					pTargetMAC	- Pointer to remote MAC address string.
 
 * Output:          None
 * Side Effects:    None
 * Overview:        This function formats and transmits an ARP packet.
 * Note:
 ********************************************************************/
void ARP_SendPkt( WORD OpCode, DWORD SenderIP, BYTE* pSenderMAC, 
						   	   DWORD TargetIP, BYTE* pTargetMAC )
{
	ARP_PKT* pArp;

	NET_PKT* sbfr = AllocPacketBuffer( sizeof(ARP_PKT) + sizeof(ETHER_HEADER) );

	if( sbfr == NULL )	//make sure we have good buffer
	{
		return;
	}

	FormatEtherFrame( sbfr, ETH_PROTO_ARP, pSenderMAC );
	SetEtherDestMAC( sbfr, pTargetMAC );

	// Fillin the ARP data
	pArp 				= (ARP_PKT*)sbfr->pNetworkLayer; //pointer setup by MAC_format...
	pArp->HardwareType	= swaps(HW_ETHERNET);			//w
	pArp->Protocol 		= swaps(ETH_PROTO_IP);			//w
	pArp->MACAddrLen	= ETH_ADRLEN;
	pArp->ProtocolLen	= sizeof(DWORD);
	pArp->Operation		= swaps(OpCode);				//w

	memcpy( pArp->SenderMACAddr, pSenderMAC, ETH_ADRLEN );
	_dwordToArray(SenderIP, pArp->SenderIPAddr);

	memcpy( pArp->TargetMACAddr, pTargetMAC, ETH_ADRLEN );
	_dwordToArray(TargetIP, pArp->TargetIPAddr);

	sbfr->PktLen 	+= sizeof(ARP_PKT);

	MACTransmit( sbfr );

}

/*********************************************************************
 * Function:        void InitiateARP( DWORD TargetIP )
 *
 * PreCondition:    AppConfig must be properly initialized
 * Input:			TargetIP - Remote node IP Address
 * Output:          None
 * Side Effects:    None
 * Overview:        This function broadcasts an ARP request packet.
 * Note:
 ********************************************************************/
void InitiateARP( DWORD TargetIP )
{
	//if the target is on the same network, then use targetIP, else use gateway ip
	TargetIP = ((SysIPAddr.Val ^ TargetIP) & SysIPMask.Val) ? SysIPGateway.Val : TargetIP;
	ARP_SendPkt( ARP_OPERATION_REQ, SysIPAddr.Val, SysMACAddr.v, TargetIP, broadcastMAC );
}

/*********************************************************************
 * Function:        void ARP_RxHandler( NET_PKT *sbfr )
 *
 * PreCondition:    None
 * Input:			sbfr - Pointer to New ARP Packet received
 * Output:          None
 * Side Effects:    None
 * Overview:        This function process ARP packets received by the local NIC
 * Note:
 ********************************************************************/
void ARP_RxHandler( NET_PKT *sbfr )
{
	ARP_PKT * pARP;
	DWORD   ip_addr;

	pARP = (ARP_PKT *)sbfr->pNetworkLayer;

    SwapARPPacket( pARP );

    if( pARP->HardwareType 	!= HW_ETHERNET ||
        pARP->MACAddrLen 	!= ETH_ADRLEN  ||
        pARP->ProtocolLen 	!= sizeof(DWORD) ||
		pARP->Protocol 		!= ETH_PROTO_IP ||
		sbfr->PktType 		== PKT_NON_LOCAL ||
		sbfr->PktType 		== PKT_LOOPBACK )
	{
		goto ARP_DropPkt;
	}

    ip_addr = _arrayToDword(pARP->TargetIPAddr);

	if( ip_addr == SysIPAddr.Val ) // dest to our IP address
	{

        //if( pARP->Operation == ARP_OPERATION_REQ )
		// Generate the ARP reply.
		ip_addr = _arrayToDword(pARP->SenderIPAddr);

        AddHostRoute( ip_addr, pARP->SenderMACAddr, TRUE );
		ARP_SendPkt( ARP_OPERATION_RESP, SysIPAddr.Val, SysMACAddr.v, ip_addr, pARP->SenderMACAddr );
	}

	DeAllocateBuffer(sbfr);
	return;

/////////////////////////////
ARP_DropPkt:
   	DeAllocateBuffer(sbfr);
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
void SwapARPPacket(ARP_PKT *p)
{
    p->HardwareType     = swaps(p->HardwareType);
    p->Protocol         = swaps(p->Protocol);
    p->Operation        = swaps(p->Operation);
}
