/*********************************************************************
 *
 *                  IP module for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        ip.c
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
 * $Id: eIP.c,v 1.2 2006/10/24 16:39:07 C12923 Exp $
 *
 ********************************************************************/

#include "bsd_tcp_ip\etcpcfg.h"
#include "bsd_tcp_ip\eip.h"
#include "bsd_tcp_ip\ether.h"
#include "bsd_tcp_ip\block_mgr.h"
#include "bsd_tcp_ip\route.h"
#include "bsd_tcp_ip\pkt_queue.h"
#include "bsd_tcp_ip\emac.h"
#include "bsd_tcp_ip\earp.h"
#include "bsd_tcp_ip\gpfunc.h"
#include "string.h"


#define IPv4				(4 << 4)

// IHL (Internet Header Length) is # of DWORDs in a header.
// Since, we do not support options, our IP header length will be
// minimum i.e. 20 bytes : IHL = 20 / 4 = 5.
#define IP_IHLEN	            5

static WORD _IP_Identifier = 0;

#define MAX_OPTIONS_LEN     (20)            // As per RFC 791.

#define IP_SERVICE_NW_CTRL  (0x07)
#define IP_SERVICE_IN_CTRL  (0x06)
#define IP_SERVICE_ECP      (0x05)
#define IP_SERVICE_OVR      (0x04)
#define IP_SERVICE_FLASH    (0x03)
#define IP_SERVICE_IMM      (0x02)
#define IP_SERVICE_PRIOR    (0x01)
#define IP_SERVICE_ROUTINE  (0x00)

#define IP_SERVICE_N_DELAY  (0x00)
#define IP_SERCICE_L_DELAY  (0x08)
#define IP_SERVICE_N_THRPT  (0x00)
#define IP_SERVICE_H_THRPT  (0x10)
#define IP_SERVICE_N_RELIB  (0x00)
#define IP_SERVICE_H_RELIB  (0x20)

#define IP_SERVICE          (IP_SERVICE_ROUTINE | IP_SERVICE_N_DELAY)

#define MY_IP_TTL           (100)   /* Time-To-Live in Seconds */

void UDP_RxHandler( NET_PKT *RxPkt );
void TCP_RxHandler( NET_PKT *sbfr );
void ICMP_RxHandler( NET_PKT *RxPkt );

// List of packet pointers that need ARP 
PktQueue PendingARPQue;

/*********************************************************************
 * Function:        void InitIPLayer()
 *
 * PreCondition:    None
 * Input:			None
 * Output:          None
 * Side Effects:    None
 * Overview:        Initialize IP local variables
 * Note:
 ********************************************************************/
void InitIPLayer()
{
	Que_Init( &PendingARPQue );
}

/*********************************************************************
 * Function:        BOOL IP_IsARPqueued( NET_PKT* sbfr )
 *
 * PreCondition:    None
 * Input:			sbfr - Packet pointer
 * Output:          True if packet is queued for ARP, otherwise return False.
 * Side Effects:    None
 * Overview:        This function indicate if the packet is queued for ARP
 * Note:
 ********************************************************************/
BOOL IP_IsARPqueued( NET_PKT* sbfr )
{
	return Que_IsQueued( &PendingARPQue, sbfr );
}

/*********************************************************************
 * Function:        void IPProcess()
 *
 * PreCondition:    None
 * Input:			None
 * Output:          None
 * Side Effects:    None
 * Overview:        Go thru pending ARP Fifo, 
 *					Transmit packets with ARP resolved.
 *					Discard packet if timeout on Arp reply
 * Note:
 ********************************************************************/
void IPProcess(void)
{	
	NET_PKT* sbfr;
	BYTE* pDestMAC;
	IP_HEADER* IPH;
	TICK diffTicks;
	NET_PKT* tempPtr = NULL;
	BYTE  brdcstMACadr[ETH_ADRLEN] = {0xff,0xff,0xff,0xff,0xff,0xff};
		
	//iterate thru the packets that are waiting for ARP reply
	while( Que_GetHead( &PendingARPQue, &sbfr ) )
	{
		DWORD   dest_addr;

        if( tempPtr == sbfr ) //we have checked all
		{ 
			Que_AddTail( &PendingARPQue, sbfr );
			break;
		}
			
		if( tempPtr == NULL )
			tempPtr = sbfr;
			
		IPH = (IP_HEADER*)sbfr->pNetworkLayer;
		
		dest_addr = _arrayToDword(IPH->DestAddr);

        pDestMAC = QueryHostRoute( dest_addr );
	
		if( pDestMAC )
		{
			SetEtherDestMAC( sbfr, pDestMAC );
			MACTransmit( sbfr );
		}
		else if (dest_addr == 0xffffffff)
		{
			SetEtherDestMAC( sbfr, brdcstMACadr);
			MACTransmit( sbfr );
		}
		else
		{
			//if not timed out, add this packet back to the arp wait queue.
			
	        TICK tock;

            tock = TickGet();

            diffTicks = TickGetDiff(tock, sbfr->ARPTickStart);
	
	        // If timeout has not occured, do not do anything.
	        if ( diffTicks <= ARP_TIMEOUT_TICKS )
	        {
		        Que_AddTail( &PendingARPQue, sbfr );
	            continue;
	        }
	        else
	        {
		        if( sbfr->TxAutoDeAlloc )
		        	DeAllocateBuffer( sbfr );
		    }
		}
	}
}

/*********************************************************************
 * Function:        int IP_TransmitPkt( NET_PKT* sbfr )
 *
 * PreCondition:    None
 * Input:			sbfr - packet buffer
 * Output:          IP_TX_SUCCESS - packet transmision complete
 *					IP_TX_QUEUED  - pakcet queued in the transmit queue
 * 					IP_TX_ARPQUEUED - packet is queued for ARP resolution
 *
 * Side Effects:    None
 * Overview:        transmit an IP packet by calling MAC layer transmit function
 *					if MAC address of remote node is not available, packet
 *					is put into the PendingARPqueue for address resolution
 * Note:
 ********************************************************************/
int IP_TransmitPkt( NET_PKT* sbfr )
{
	BYTE* pDestMAC;
	IP_HEADER* IPH;
    DWORD   dest_addr;

	//Race condition?????
	//if tcp retransmits and hostroute is now available, 
	//but the ip_process has not run to process the PendingARP que yet
	//tcp may dealloc the packet, while the PendingARP que still
	//hold the pointer.

	IPH = (IP_HEADER*)sbfr->pNetworkLayer;

    dest_addr = _arrayToDword(IPH->DestAddr);

    // Get MAC for the dest
	pDestMAC = QueryHostRoute( dest_addr );

	if( pDestMAC )
	{
		SetEtherDestMAC( sbfr, pDestMAC );

		if( MACTransmit( sbfr ) )
			return IP_TX_SUCCESS;
		else
			return IP_TX_QUEUED;
	}

	// Add this packet to our pending arp reply list
	if( !Que_IsQueued( &PendingARPQue, sbfr ) )
	{
		InitiateARP( dest_addr );
		sbfr->ARPTickStart = TickGet();
		Que_AddTail( &PendingARPQue, sbfr );
	}

	return IP_TX_ARPQUEUED;

}

/*********************************************************************
 * Function:        BOOL IP_FormatPacket( NET_PKT* sbfr, DWORD DestIP, BYTE protocol, WORD len )
 *
 * PreCondition:    None
 * Input:			sbfr - packet buffer
 *					DestIP - destination IP address
 *					protocol - IP protocol
 *					len - data length
 * Output:          None
 *
 * Side Effects:    None
 * Overview:        Formats the IP layer of the packet
 * Note:
 ********************************************************************/
BOOL IP_FormatPacket( NET_PKT* sbfr, DWORD DestIP, BYTE protocol, WORD len )
{
	IP_HEADER* header;
	
	FormatEtherFrame( sbfr, ETH_PROTO_IP, SysMACAddr.v );
	
	sbfr->PktLen 	+= sizeof(IP_HEADER) + len;
	sbfr->pTransportLayer	= sbfr->pNetworkLayer + sizeof(IP_HEADER);

	header			= (IP_HEADER*)sbfr->pNetworkLayer;

    header->IHL_Version	     = (IP_IHLEN | IPv4);
    header->TypeOfService    = IP_SERVICE;
    header->TotalLength      = sizeof(IP_HEADER) + len;
    header->Identification   = ++_IP_Identifier;
    header->FragmentInfo     = 0;
    header->TimeToLive       = MY_IP_TTL;
    header->Protocol         = protocol;
    header->HeaderChecksum   = 0;
    
    _dwordToArray(SysIPAddr.Val, header->SourceAddr);
    _dwordToArray(DestIP, header->DestAddr);

    header->TotalLength      = swaps(header->TotalLength);
    header->Identification   = swaps(header->Identification);

    header->HeaderChecksum   = ~checksum16((BYTE*)header, sizeof(IP_HEADER));

	return TRUE;
}

/*********************************************************************
 * Function:        void IP_DeliverLocalPkt( NET_PKT *sbfr )
 *
 * PreCondition:    None
 * Input:			sbfr - packet buffer
 * Output:          None
 * Side Effects:    None
 * Overview:        Delivers the packet to the UDP, TCP or ICMP handler
 * Note:
 ********************************************************************/
void IP_DeliverLocalPkt( NET_PKT *sbfr )
{
    IP_HEADER *pIP;

	switch( sbfr->PktType )
	{
	case PKT_LOCAL:
	case PKT_BROADCAST:
	case PKT_MULTICAST:
		break;

	case PKT_NON_LOCAL:	
	default:
   		DeAllocateBuffer(sbfr);
		return;
		break;
	}

	sbfr->PktLen 	-= sizeof(IP_HEADER);
    sbfr->pTransportLayer = sbfr->pNetworkLayer + sizeof(IP_HEADER);

	pIP 			= (IP_HEADER*)sbfr->pNetworkLayer;

	sbfr->pSrcIP  	= pIP->SourceAddr;
	sbfr->pDestIP  	= pIP->DestAddr;

    if( pIP->Protocol == IP_PROT_ICMP )
		ICMP_RxHandler( sbfr );
	else if( pIP->Protocol == IP_PROT_UDP )
		UDP_RxHandler( sbfr );
	else if( pIP->Protocol == IP_PROT_TCP )
		TCP_RxHandler( sbfr );
	else
	{
   		DeAllocateBuffer(sbfr);
	}
}

/*********************************************************************
 * Function:        void IP_RxHandler( NET_PKT *sbfr )
 *
 * PreCondition:    None
 * Input:			sbfr - packet buffer
 * Output:          None
 * Side Effects:    None
 * Overview:        Process new IP packet received by local NIC
 * Note:
 ********************************************************************/
void IP_RxHandler( NET_PKT *sbfr, BYTE * srcMAC )
{
    IP_HEADER *ipHdr;
    WORD len, optionsLen;
	BYTE ihl;
	
	// If this packet is not for us then discard it.
	// we may get these packets only in promiscous mode
    if( sbfr->PktType == PKT_NON_LOCAL )
    {
		goto IP_DropPkt;
	}

	ipHdr = (IP_HEADER*)sbfr->pNetworkLayer;

    ihl = ipHdr->IHL_Version & 0x0F;
    
    if( (ihl < IP_IHLEN) || ((ipHdr->IHL_Version & 0xF0) != IPv4) )
    {
		goto IP_DropPkt;
	}

    // Calculate options length in this header, if there is any.
    // IHL is in terms of numbers of 32-bit DWORDs; i.e. actual
    // length is 4 times IHL.
   
   
    optionsLen = (ihl * 4) - sizeof(IP_HEADER);

    if( optionsLen > MAX_OPTIONS_LEN )
    {
		goto IP_DropPkt;
	}
        
    if( checksum16( (BYTE*)ipHdr, ihl * 4 ) != 0xFFFF )  // must be FFFF
    {
		goto IP_DropPkt;
    }

    len = swaps( ipHdr->TotalLength );
        
    if( (sbfr->PktLen < len) || (len < (ihl * 4) ) )
    {
		goto IP_DropPkt;
    }

	sbfr->PktLen = len; //adjust pktlen to not exceed totallength
	IP_DeliverLocalPkt( sbfr );
	return;

//////////////////////////////////
IP_DropPkt:
   	DeAllocateBuffer(sbfr);
   	return;

}
