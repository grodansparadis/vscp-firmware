/*********************************************************************
 *
 *                  ether layer functions for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        ether.c
 * Processor:       PIC24/DAYTONA
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
 * $Id: ether.c,v 1.5 2006/10/24 16:39:07 C12923 Exp $
 *
 ********************************************************************/
#include "bsd_tcp_ip\netpkt.h"
#include "bsd_tcp_ip\ether.h"
#include "bsd_tcp_ip\etcpcfg.h"
#include <string.h>
#include "bsd_tcp_ip\gpfunc.h"

BYTE broadcastMAC[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/*********************************************************************
 * Function:        WORD GetEtherFrameInfo( NET_PKT* sbfr )
 *
 * PreCondition:    None
 * Input:			sbfr - Pointer to New Packet received
 * Output:          None
 * Side Effects:    None
 * Overview:        This function updates packet information in the packet buffer
 * Note:
 ********************************************************************/
WORD GetEtherFrameInfo( NET_PKT* sbfr )
{
    ETHER_HEADER *e_hdr;

	sbfr->PktLen 	-= sizeof(ETHER_HEADER);
	sbfr->pNetworkLayer	=  (BYTE *)sbfr->pPkt;
	sbfr->pNetworkLayer += sizeof(ETHER_HEADER);

    e_hdr 			= (ETHER_HEADER *)sbfr->pMacLayer;

    if( e_hdr->destMAC[0] & 1 )
    {
        if( memcmp( e_hdr->destMAC, broadcastMAC, ETH_ADRLEN ) == 0 )
            sbfr->PktType = PKT_BROADCAST;
        else
            sbfr->PktType = PKT_MULTICAST;
    }
    else if( memcmp( e_hdr->destMAC, SysMACAddr.v, ETH_ADRLEN ) == 0 )
        sbfr->PktType = PKT_LOCAL;
	else
        sbfr->PktType = PKT_NON_LOCAL;

	//extract the src MAC address
	sbfr->pSrcMAC = e_hdr->srcMAC;

    return( swaps( e_hdr->Type ) );
}

/*********************************************************************
 * Function:        void FormatEtherFrame( NET_PKT* sbfr, WORD proto, BYTE* pSrcMAC )
 *
 * PreCondition:    None
 * Input:			sbfr - Pointer to Packet
 *					proto - protocol to use in the ether frame
 *					pSrcMAC - pointer to MAC address of local node
 * Output:          None
 * Side Effects:    None
 * Overview:        This function updates the packet's MAC layer
 * Note:
 ********************************************************************/
void FormatEtherFrame( NET_PKT* sbfr, WORD proto, BYTE* pSrcMAC )
{
    ETHER_HEADER *hdr;

	//setup next up layer raw pointer
	sbfr->pNetworkLayer	= (BYTE *)sbfr->pPkt;
    sbfr->pNetworkLayer += sizeof(ETHER_HEADER);

	hdr = (ETHER_HEADER *)sbfr->pMacLayer; //MacLayer pointer already setup by Alloc Buffer
	hdr->Type = swaps( proto );
	sbfr->PktLen = sizeof( ETHER_HEADER );
	memcpy( hdr->srcMAC, pSrcMAC, ETH_ADRLEN );
}

/*********************************************************************
 * Function:        void SetEtherDestMAC( NET_PKT* sbfr, BYTE* pDestMAC )
 *
 * PreCondition:    None
 * Input:			sbfr - Pointer to Packet
 *					pDestMAC - pointer to MAC address of remote node
 * Output:          None
 * Side Effects:    None
 * Overview:        This function updates the remote MAC address in the packet
 * Note:
 ********************************************************************/
void SetEtherDestMAC( NET_PKT* sbfr, BYTE* pDestMAC )
{
    ETHER_HEADER *hdr;
	hdr = (ETHER_HEADER *)sbfr->pMacLayer; //MacLayer pointer already setup by Alloc Buffer
	memcpy( hdr->destMAC, pDestMAC, ETH_ADRLEN );
}	
