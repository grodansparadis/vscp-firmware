/*********************************************************************
 *
 *                  UPD Layer for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        udp.c
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
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
 * $Id: eUDP.c,v 1.2 2006/10/24 16:39:07 C12923 Exp $
 *
 ********************************************************************/
#include "bsd_tcp_ip\etcpcfg.h"
#include "bsd_tcp_ip\eudp.h"
#include "bsd_tcp_ip\sockapi.h"
#include "bsd_tcp_ip\socket.h"
#include "bsd_tcp_ip\block_mgr.h"
#include "bsd_tcp_ip\gpfunc.h"
#include <string.h>

// Header struct to calculate udp checksum
struct tempHdr
{
	DWORD srcIP;
	DWORD DstIP;
	BYTE  filler;
	BYTE  proto;
	WORD  len;
};

#ifdef API_00833_SUPPORT
/**********************************************************************/
/**********************************************************************/
int UDPWriteByte( struct MCSocket* pSock, char dat )
{
	BYTE* pDat;
	WORD udpLen;
    UDP_HEADER* pUDP;
	
	if( pSock->pTxActivePacket == NULL )
	{
		udpLen = pSock->TxBfrSize + sizeof(UDP_HEADER);
		pSock->pTxActivePacket = (NET_PKT*)AllocSocketBuffer( udpLen );
	
		if( pSock->pTxActivePacket == NULL )	//make sure we have good buffer
		{
			return SOCKET_BFR_ALLOC_ERROR;
		}
		pSock->TxDataIndex = 0;
	
		DWORD ip;
		
		if( pSock->remoteIP )
			ip = pSock->remoteIP;
		else
			ip = pSock->ActiveRxSrcIP;
	
		IP_FormatPacket( pSock->pTxActivePacket, ip, IP_PROT_UDP, udpLen );
	}
	
	if( pSock->TxDataIndex < pSock->TxBfrSize )
	{
		pUDP = 	(UDP_HEADER*)pSock->pTxActivePacket->pTransportLayer;
		
		pDat = (BYTE*)pUDP + sizeof(UDP_HEADER);
	
		pDat[pSock->TxDataIndex] = dat;
		pSock->TxDataIndex++;
	}
	else
	{
		return SOCKET_MAX_LEN_ERROR;
	}

	return 0; //success	
}

	
/***************************************************************************/	
/***************************************************************************/	
int UDPXmitActivePkt( struct MCSocket* pSock )
{
    UDP_HEADER* pUDP;
    WORD protoLen;
	DWORD csum;
	struct tempHdr vhdr;
	NET_PKT* sbfr;
		
	if( pSock->pTxActivePacket == NULL )
		return SOCKET_ERROR;

	protoLen = sizeof(UDP_HEADER) + pSock->TxBfrSize;
	
	sbfr = pSock->pTxActivePacket;
	pSock->pTxActivePacket = NULL;
	
	pUDP = 	(UDP_HEADER*)sbfr->pTransportLayer;

	WORD port;
	DWORD ip;
	
	if( pSock->remoteIP )
	{
		ip = pSock->remoteIP;
		port = pSock->remotePort;
	}
	else
	{
		ip = pSock->ActiveRxSrcIP;
		port = pSock->ActiveRxSrcPort;
	}
	
	pUDP->Checksum			= 0;
	pUDP->SourcePort		= swaps(pSock->localPort);
	pUDP->DestinationPort 	= swaps(port);
	pUDP->Length         	= swaps(protoLen);
	
	vhdr.srcIP	= SysIPAddr.Val;
	vhdr.DstIP 	= ip;
	vhdr.filler = 0;
	vhdr.proto 	= IP_PROT_UDP;
	vhdr.len	= pUDP->Length;

	csum = 0;
    checksum16_partial((BYTE*)pUDP, protoLen, &csum);
	checksum16_partial((BYTE*)&vhdr, sizeof(struct tempHdr), &csum );
	pUDP->Checksum	= ~fold_checksum( csum );
	
	IP_TransmitPkt( sbfr );
	return 0; //success
}

#endif

/*********************************************************************
 * Function:        int UDPTransmit( BYTE* buf, int len, WORD SrcPort, 
 *					DWORD remoteIP, WORD remotePort )
 *
 * PreCondition:    None
 * Input:			buf      - data pointer
 *					len      - data length
 *					SrcPort  - Source port number
 *					remoteIP - destination ip address
 *					remotePort - destination port number
 * Output:          returns number of bytes sent, or -1 on error
 * Side Effects:    None
 * Overview:        Packages and transmits a udp data packet
 * Note:
 ********************************************************************/
int UDPTransmit( BYTE* buf, int len, WORD SrcPort, DWORD remoteIP, WORD remotePort )
{
    UDP_HEADER* pUDP;
    WORD temp;
	DWORD csum;
	struct tempHdr vhdr;

	temp = sizeof(UDP_HEADER) + len;
	NET_PKT* sbfr = AllocSocketBuffer( temp );
	if( sbfr == NULL )	//make sure we have good buffer
	{
		return SOCKET_BFR_ALLOC_ERROR;
	}

	IP_FormatPacket( sbfr, remoteIP, IP_PROT_UDP, temp );

	pUDP = 	(UDP_HEADER*)sbfr->pTransportLayer;

    pUDP->Checksum			= 0;
    pUDP->SourcePort		= swaps(SrcPort);
    pUDP->DestinationPort 	= swaps(remotePort);
    pUDP->Length         	= swaps(temp);

    memcpy( ((BYTE*)pUDP) + sizeof(UDP_HEADER), buf, len );
	
	vhdr.srcIP	= SysIPAddr.Val;
	vhdr.DstIP 	= remoteIP;
	vhdr.filler = 0;
	vhdr.proto 	= IP_PROT_UDP;
	vhdr.len	= pUDP->Length;

	csum = 0;
    checksum16_partial((BYTE*)pUDP, temp, &csum);
	checksum16_partial((BYTE*)&vhdr, sizeof(struct tempHdr), &csum );
	pUDP->Checksum	= ~fold_checksum( csum );
	
	IP_TransmitPkt( sbfr );
	return len;
}


/*********************************************************************
 * Function:        int UDPExtractData( NET_PKT *RxPkt, BYTE* bfr, int len )
 *
 * PreCondition:    None
 * Input:			RxPkt - Raw UDP Packet
 *					bfr - data buffer to receive data
 *					len - buffer length
 * Output:          returns number of bytes copied to app buffer, or -1 on error
 * Side Effects:    None
 * Overview:        this function taked the raw data packet and 
 *					extracts the data portion and copies it to the application
 *					buffer
 * Note:
 ********************************************************************/
int UDPExtractData( NET_PKT *RxPkt, BYTE* bfr, int len )
{
	UDP_HEADER* pUDP;
	BYTE *pData;
	WORD DataLen;

	pUDP = (UDP_HEADER*)RxPkt->pTransportLayer;

	pData = RxPkt->pTransportLayer + sizeof(UDP_HEADER);

	DataLen = pUDP->Length;
	
	if( len < DataLen ) //bfr length is not enough for the entire packet
		DataLen = len;

	memcpy( bfr, pData, DataLen );
	return DataLen;
}

#if defined(API_00833_SUPPORT)

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
BOOL UDPSetRxActivePacket( struct MCSocket* pSock )
{
	NET_PKT *pSbuf;
	UDP_HEADER* pUDP;

	if( pSock->pRxActivePacket )
    {
        return TRUE;

    }else if( pSock->RxMsgQueue.PktCount )
	{
        DWORD   src_ip;

		//retreive the Datagram from Rx Fifo
		Que_GetHead( &pSock->RxMsgQueue, &pSbuf );
		
        pSock->pRxActivePacket 	= pSbuf;
		pSock->DataByteIndex 	= 0;
		

        src_ip                  = _arrayToDword(pSbuf->pSrcIP);
        pSock->ActiveRxSrcIP	= src_ip;

		pSbuf->pData = pSbuf->pTransportLayer + sizeof(UDP_HEADER);
		
		pUDP = (UDP_HEADER*)pSbuf->pTransportLayer;		
		pSock->ActiveRxSrcPort	= pUDP->SourcePort;
		pSbuf->PktLen = pUDP->Length;
		
		return TRUE;
	}
	else // No data available
	{
		return FALSE; //just tell app there is no data
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void UDPDiscardRxActivePacket( struct MCSocket* pSock )
{
	if( pSock->pRxActivePacket )
	{
		DeAllocateBuffer( pSock->pRxActivePacket );
		pSock->pRxActivePacket = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// This function allows the incomming packet to buffer.  the user can ask for 
// any amount of data (upto max available in packet).  When all data is consumed, 
// the packet will be deallocated.
int UDPGetBfrData( struct MCSocket* pSock, BYTE* buf, int len )
{
	NET_PKT *pSbuf;
	int AvailLen, BytesRead = 0;

	if( pSock->pRxActivePacket == NULL )
		return 0;
		
	pSbuf = pSock->pRxActivePacket;

	AvailLen = pSbuf->PktLen - pSock->DataByteIndex;

	if( AvailLen > 0 )
	{
		if( len > AvailLen )
			len = AvailLen;
			
		memcpy( buf, &pSbuf->pData[pSock->DataByteIndex], len );
		pSock->DataByteIndex += len;
		BytesRead = len;
	}
	else
	{
		DeAllocateBuffer( pSock->pRxActivePacket );
		pSock->pRxActivePacket = NULL;
	}

	return BytesRead;
}
#endif
/*********************************************************************
 * Function:        void UDP_RxHandler( NET_PKT *RxPkt )
 *
 * PreCondition:    None
 * Input:			RxPkt - Raw UDP Packet received
 * Output:          None
 * Side Effects:    None
 * Overview:        Process new udp message received
 * Note:
 ********************************************************************/
void UDP_RxHandler( NET_PKT *RxPkt )
{
	UDP_HEADER* pUDP;
	SOCKET sk;
	struct MCSocket* pSock = NULL;
    DWORD   src_ip, dest_ip;


	pUDP = (UDP_HEADER*)RxPkt->pTransportLayer;

    pUDP->SourcePort        = swaps(pUDP->SourcePort);
    pUDP->DestinationPort   = swaps(pUDP->DestinationPort);
    pUDP->Length            = swaps(pUDP->Length) - sizeof(UDP_HEADER);;
    pUDP->Checksum          = swaps(pUDP->Checksum);

	//TODO - Discard packet if checksum fails


    src_ip  = _arrayToDword(RxPkt->pSrcIP);
    dest_ip = _arrayToDword(RxPkt->pDestIP);

	if( (sk = FindTargetSocket(SOCK_DGRAM, src_ip, dest_ip, pUDP->DestinationPort)) != INVALID_SOCK )
	{
		pSock = &SocketArray[sk];
		Que_AddTail( &pSock->RxMsgQueue, RxPkt ); //Add packet to socket incoming msg queue
	}
	else
	{
		//no matching socket found - discard
   		DeAllocateBuffer(RxPkt);
	}
}

