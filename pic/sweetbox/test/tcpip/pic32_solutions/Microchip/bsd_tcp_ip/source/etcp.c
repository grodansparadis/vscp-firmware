/*********************************************************************
 *
 *                  TCP Layer for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        tcp.c
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
 * $Id: eTCP.c,v 1.2 2006/10/24 16:39:07 C12923 Exp $
 *
 ********************************************************************/

#include "eTCP.def"
#include "bsd_tcp_ip\etcpcfg.h"
#include "bsd_tcp_ip\netpkt.h"
#include "bsd_tcp_ip\sockapi.h"
#include "bsd_tcp_ip\socket.h"
#include "bsd_tcp_ip\gpfunc.h"
#include "bsd_tcp_ip\block_mgr.h"
#include "string.h"
#include "bsd_tcp_ip\ether.h"

/*
 * TCP Header def. as per rfc 793.
 */
#pragma push
#pragma pack(1)
typedef struct _TCP_HEADER
{
    WORD    SourcePort;
    WORD    DestPort;
    BYTE    SeqNum[4];
    BYTE    AckNum[4];
    BYTE    HdrLen;
    BYTE    Flags;
    WORD    Window;
    WORD    Checksum;
    WORD    UrgentPointer;

} TCP_HEADER;
#pragma pop

/*
 * TCP Options as defined by rfc 793
 */
#define TCP_OPTIONS_END_OF_LIST     0x00
#define TCP_OPTIONS_NO_OP           0x01
#define TCP_OPTIONS_MAX_SEG_SIZE    0x02

typedef struct _TCP_OPTIONS
{
    BYTE        Kind;
    BYTE        Length;
    WORD_VAL    MaxSegSize;
   
} TCP_OPTIONS;

/*
 * Pseudo header as defined by rfc 793.
 */
typedef struct _PSEUDO_HEADER
{
    DWORD 	SourceAddress;
    DWORD 	DestAddress;
    BYTE 	Zero;
    BYTE 	Protocol;
    WORD 	TCPLength;
} PSEUDO_HEADER;

static DWORD ISS;

/*
 * Function prototypes
 */
static void SwapTCPHeader(TCP_HEADER* header);
void HandleTCPSegment( SOCKET s, NET_PKT* pNetPkt, WORD len );
int TCPXmitPkt(DWORD remoteIP, WORD localPort, WORD remotePort, DWORD tseq, DWORD tack,
		BYTE flags, NET_PKT* sbfr, WORD DataLen, struct MCSocket* pSock );

/*********************************************************************
 * Function:        void TCPAdd2AckList( struct MCSocket* pSock, NET_PKT* pktPtr )
 *
 * PreCondition:    None
 * Input:			pSock - Socket structure pointer
 *					pktPtr - Pointer to Packet.
 * Output:          None
 * Side Effects:    None
 * Overview:        This function adds the packet pointer to the TCP Ack list
 * Note:
 ********************************************************************/
void TCPAdd2AckList( struct MCSocket* pSock, NET_PKT* pktPtr )
{
	if( pSock->pTCPListTail == NULL )
	{
		pSock->pTCPListHead = pktPtr;
		pSock->pTCPListTail = pktPtr;
	}
	else
	{
		pSock->pTCPListTail->pTcpNext = pktPtr;
		pSock->pTCPListTail = pktPtr;
	}

	pSock->AckListCount++;
	pktPtr->pTcpNext = NULL;
}

/*********************************************************************
 * Function:        NET_PKT* DeQueAckList( struct MCSocket* pSock )
 *
 * PreCondition:    None
 * Input:			pSock - Socket structure pointer
 * Output:          Pointer to Packet or NULL
 * Side Effects:    None
 * Overview:        This function removes a packet pointer from the TCP Acl List
 * Note:
 ********************************************************************/
NET_PKT* DeQueAckList( struct MCSocket* pSock )
{
	NET_PKT* pNode;

	if( pSock->pTCPListHead == NULL )
		return NULL;

	pNode = pSock->pTCPListHead;

	pSock->pTCPListHead = pNode->pTcpNext;
	pNode->pTcpNext = NULL;

	if( pSock->pTCPListTail == pNode )
	{
		pSock->pTCPListHead = NULL;
		pSock->pTCPListTail = NULL;
	}

	pSock->AckListCount--;
	return pNode;	
}
/*********************************************************************
 * DeallocateAckList
 *********************************************************************/
NET_PKT *DeallocateAckList(struct MCSocket* pSock, NET_PKT *good_bye_pkt)
{
    NET_PKT *prev;
    NET_PKT *curr;

    prev    = pSock->pTCPListHead;
    curr    = pSock->pTCPListHead;

    while((curr) && (curr != good_bye_pkt))
    {
        prev = curr;
        curr = curr->pTcpNext;
    }

    if(!curr)
        return NULL;

    if(curr == prev)
    {
        pSock->pTCPListHead = curr->pTcpNext;
        prev = pSock->pTCPListHead;

    }else
    {
        prev->pTcpNext = curr->pTcpNext;    
    }

    DeAllocateBuffer(curr);

    return prev;
}
/*********************************************************************
 * Function:        void TCPInit()
 *
 * PreCondition:    None
 * Input:			None
 * Output:          None
 * Side Effects:    None
 * Overview:        Initialize TCP local variables
 * Note:
 ********************************************************************/
void eTCPInit()
{
	ISS = 1000000; //Initial Sequence number
}

/*********************************************************************
 * Function:        void TCPInitiateCnxn( struct MCSocket* pSock )
 *
 * PreCondition:    Must be called by the socket layer
 * Input:			pSock - pointer to socket structure
 * Output:          None
 * Side Effects:    None
 * Overview:        Transmits SYN message to initiate connection establish process
 * Note:
 ********************************************************************/
void TCPInitiateCnxn( struct MCSocket* pSock )
{
	if( pSock->smState >= TCP_SYN_SENT )
		return;
	
    // assign an initial sequence number
    pSock->TcpSEQ = ++ISS;
    pSock->TcpACK = 0;
    
    pSock->RetryCount	= 0;
    pSock->startTick	= TickGet();

	// Initiate connection request - Send SYN message.
    TCPXmitPkt(pSock->remoteIP,
            pSock->localPort,
            pSock->remotePort,
            pSock->TcpSEQ++, //SYN takes 1 seq number
            pSock->TcpACK,
            SYN, NULL, 0, NULL );

    pSock->smState = TCP_SYN_SENT;
}

/*********************************************************************
 * Function:        WORD CalcTCP_Checksum( DWORD SrcIP, DWORD DsetIP, BYTE* PktBfr, WORD BfrLen )
 *
 * PreCondition:    None
 * Input:			SrcIP - local IP address
 *					DestIP - destination IP address
 *					PktBfr - data buffer
 *					BfrLen - data length
 * Output:          checksum for the packet
 * Side Effects:    None
 * Overview:        calculates the TCP checksum
 * Note:
 ********************************************************************/
WORD CalcTCP_Checksum( DWORD SrcIP, DWORD DestIP, BYTE* PktBfr, WORD BfrLen )
{
    PSEUDO_HEADER   pseudoHeader;
    DWORD checksum;
	
	//create the tcp pseudo header
    pseudoHeader.SourceAddress      = SrcIP;
    pseudoHeader.DestAddress		= DestIP;
    pseudoHeader.Zero               = 0x0;
    pseudoHeader.Protocol           = IP_PROT_TCP;
    pseudoHeader.TCPLength          = swaps(BfrLen);

	checksum = 0;
    checksum16_partial((BYTE*)PktBfr, BfrLen, &checksum);
	checksum16_partial((BYTE*)&pseudoHeader, sizeof(pseudoHeader), &checksum );
	return fold_checksum( checksum );
}	

/*********************************************************************
 * Function:        int TCPXmitPkt(DWORD remoteIP,
 *					WORD localPort,	WORD remotePort,
 *					DWORD tseq,	DWORD tack, BYTE flags,	BYTE* bfr,
 *					WORD len, struct MCSocket* pSock )
 *
 * PreCondition:    None
 * Input:			remoteIP 	- destination IP address
 *					localPort 	- local port number
 *					tseq 		- transmit sequence number
 *					tack 		- transmit ack number
 *					flags 		- transmit flags
 *					sbfr 		- packet pointer
 *					DataLen		- App data length
 *					pSock 		- pointer to socket structure
 *
 * Output:          if successful, number of bytes sent, else 
 *					error code (negative number)
 * Side Effects:    None
 * Overview:        Package and transmit a TCP packet
 * Note:
 ********************************************************************/
int TCPXmitPkt(DWORD remoteIP,
					WORD localPort,
					WORD remotePort,
					DWORD tseq,
					DWORD tack,
					BYTE flags,
					NET_PKT	*sbfr,
					WORD	DataLen,
					struct MCSocket* pSock )
{
    TCP_HEADER*      pHdr;
    TCP_OPTIONS     options;
	WORD			TcpLen;
	BYTE 			*pTcpData;
	BYTE			hLen = sizeof(TCP_HEADER);

	
	TcpLen = DataLen + sizeof(TCP_HEADER);
    if ( flags & SYN )
		TcpLen = sizeof(TCP_HEADER) + sizeof(options);

	if( sbfr == NULL )
	{
		sbfr = (NET_PKT*)AllocSocketBuffer( TcpLen );
	
		if( sbfr == NULL )	//make sure we have good buffer
		{
			return SOCKET_BFR_ALLOC_ERROR;
		}	
	}

	IP_FormatPacket( sbfr, remoteIP, IP_PROT_TCP, TcpLen );
	
	pHdr = 	(TCP_HEADER*)sbfr->pTransportLayer;

    pHdr->SourcePort    = localPort;
    pHdr->DestPort      = remotePort;
    
    _dwordToArray(tseq, pHdr->SeqNum);
    _dwordToArray(tack, pHdr->AckNum);

    pHdr->Flags			= flags;
    pHdr->Window 		= 500;	/*?????????*/

    pHdr->Checksum      = 0;
    pHdr->UrgentPointer = 0;

    SwapTCPHeader(pHdr);
	
	pTcpData = sbfr->pTransportLayer+sizeof(TCP_HEADER);

    if( flags & SYN )
    {
        options.Kind = TCP_OPTIONS_MAX_SEG_SIZE;
        options.Length = 0x04;

        // Load MSS in already swapped order.
        options.MaxSegSize.v[0]  = 0x05;
        options.MaxSegSize.v[1]  = 0xB4;
        
        //copy options to packet
        memcpy( pTcpData, (BYTE*)&options, sizeof(options));
        hLen += sizeof(options);
    }

	pHdr->HdrLen = (hLen << 2);
    pHdr->Checksum = ~CalcTCP_Checksum( SysIPAddr.Val, remoteIP, sbfr->pTransportLayer, TcpLen );
	
	if( pSock )
	{
		//save this packet for retransmission
		TCPAdd2AckList( pSock, sbfr );
		sbfr->TimeStamp = TickGet();
		sbfr->TxAutoDeAlloc = FALSE; // we will deAlloc this pkt when ACK is received
        sbfr->TCPACKNum = tseq + (DWORD)DataLen;
	}

	IP_TransmitPkt( sbfr );
		
	return DataLen;
}

/*********************************************************************
 * Function:        void TCP_RxHandler( NET_PKT *RxPkt )
 *
 * PreCondition:    None
 * Input:			RxPkt - Receive Packet buffer
 * Output:          None
 * Side Effects:    None
 * Overview:        Process new TCP packet received
 * Note:
 ********************************************************************/
void TCP_RxHandler( NET_PKT *RxPkt )
{
	SOCKET socket;
    TCP_HEADER      *pTCPHeader;
    WORD        	checksum;
    BYTE            optionsSize;
	WORD			len;
    DWORD           src_ip, dest_ip;

    src_ip  = _arrayToDword(RxPkt->pSrcIP);
    dest_ip = _arrayToDword(RxPkt->pDestIP);
    

	checksum = CalcTCP_Checksum( src_ip, SysIPAddr.Val, RxPkt->pTransportLayer, RxPkt->PktLen );

    if ( checksum != 0xFFFF )
    {
		DeAllocateBuffer(RxPkt);
        return;
    }
	
    // Retrieve TCP header.
	pTCPHeader = (TCP_HEADER*)RxPkt->pTransportLayer;
    SwapTCPHeader( pTCPHeader );

    // Skip over options and retrieve all data bytes.
    optionsSize = pTCPHeader->HdrLen >> 4;
    optionsSize <<= 2;
    optionsSize -= sizeof(TCP_HEADER);
    len = RxPkt->PktLen - optionsSize - sizeof(TCP_HEADER);

    // Find matching socket.
	if( (socket = FindTargetSocket(SOCK_STREAM, src_ip, dest_ip, pTCPHeader->DestPort)) != INVALID_SOCK )
	{
		HandleTCPSegment( socket, RxPkt, len );
	}
	else
	{
		//no matching socket found - discard
   		DeAllocateBuffer(RxPkt);
	}
}

/*********************************************************************
 * Function:        int TCPIsTxReady( struct MCSocket* pSock )
 *
 * PreCondition:    None
 * Input:			pSock - Socket structure pointer
 * Output:          return 0 if success, else -1
 * Side Effects:    None
 * Overview:        if the last transmit packet is not yet acked, the app 
 *					is not allowed to transmit more data.
 * Note:
 ********************************************************************/
int TCPIsTxReady( struct MCSocket* pSock )
{
	if( pSock->smState != TCP_EST )
		return SOCKET_ERROR;
	
	if( pSock->AckListCount >= MAX_TCP_TX_PKT_BFR ) // previous packet not yet acked
		return SOCKET_TX_NOT_READY;
	else
		return 0; //success
}

#if defined(API_00833_SUPPORT)

int TCPIsRxReady( struct MCSocket* pSock )
{
	if( pSock->smState != TCP_EST )
		return SOCKET_ERROR;
	
	if( pSock->pRxActivePacket )
	{
		int AvailLen = pSock->pRxActivePacket->PktLen - pSock->DataByteIndex;
		if( AvailLen > 0 )
			return 0; //success	
	}
	
	if( pSock->RxMsgQueue.PktCount ) 
		return 0; //success	
		
	return SOCKET_NOT_READY;
}
	
int TCPDiscardActiveRX( struct MCSocket* pSock )
{
	if( pSock->smState != TCP_EST )
		return SOCKET_ERROR;
	
	if( pSock->pRxActivePacket )
	{
		DeAllocateBuffer( pSock->pRxActivePacket );
		pSock->pRxActivePacket = NULL;
		return 0;
	}
	else
		return SOCKET_NOT_READY;
}
#endif
	
/*********************************************************************
 * Function:        int TCPTransmitData( struct MCSocket* pSock, BYTE* buf, int len )

 *
 * PreCondition:    None
 * Input:			pSock - Socket structure pointer
 *					buf - data buffer
 *					len - data length
 * Output:          returns number of bytes sent, negative number for error
 * Side Effects:    None
 * Overview:        This function transmits TCP data and returns number of bytes sent
 * Note:
 ********************************************************************/
int TCPTransmitData( struct MCSocket* pSock, BYTE* buf, int len )
{
	int temp, lp = pSock->localPort;
	BYTE *pTcpData;
	int BfrSize, ByteXfer, bfrAvail;
	
	if( (temp = TCPIsTxReady( pSock )) < 0 )
		return temp;

	if( len > (MAX_ETHERNET_FRAME_SZ - (sizeof(ETHER_HEADER) + sizeof(IP_HEADER)) - 10) )
		return SOCKET_MAX_LEN_ERROR;

	if( len == 0 )
		return 0;

	if( pSock->pParentSocket ) //if we are part of listening socket
		lp = pSock->pParentSocket->localPort;

	if( (pSock->TcpNoDelay || pSock->FirstSend) && (pSock->pTxActivePacket == NULL) )
		BfrSize = len;
	else
		BfrSize = pSock->TxBfrSize;

	if( pSock->pTxActivePacket == NULL )
	{
		WORD TcpLen = BfrSize + sizeof(TCP_HEADER);
	
		pSock->NagglesTimer = TickGet();
		pSock->pTxActivePacket = (NET_PKT*)AllocSocketBuffer( TcpLen );
	
		if( pSock->pTxActivePacket == NULL )	//make sure we have good buffer
		{
			return SOCKET_BFR_ALLOC_ERROR;
		}
		pSock->TxDataIndex = 0;
	
		IP_FormatPacket( pSock->pTxActivePacket, pSock->remoteIP, IP_PROT_TCP, TcpLen );
	}
		
	pTcpData = pSock->TxDataIndex + pSock->pTxActivePacket->pTransportLayer + sizeof(TCP_HEADER);
	
	bfrAvail = BfrSize - pSock->TxDataIndex;
	
	if( bfrAvail < len )
		ByteXfer = bfrAvail;
	else
		ByteXfer = len;

	//copy user data to packet buffer
	memcpy( pTcpData, buf, ByteXfer );
	pSock->TxDataIndex += ByteXfer;
	
	if( pSock->TcpNoDelay || pSock->TxDataIndex == pSock->TxBfrSize || pSock->FirstSend )
	{
		TCPXmitPkt(pSock->remoteIP,
	               	lp,
	               	pSock->remotePort,
	               	pSock->TcpSEQ,
	               	pSock->TcpACK,
					PSH|ACK, pSock->pTxActivePacket, BfrSize, pSock);
					
		pSock->pTxActivePacket = NULL;
		pSock->FirstSend = FALSE;

		pSock->TcpSEQ += BfrSize; //for next segment use
	}


	return ByteXfer;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
int TCPPeekData( struct MCSocket* pSock, BYTE* buf, int len )
{
	NET_PKT *pSbuf;
	int AvailLen = 0;

	// socket must be in or past established state
	if( pSock->smState < TCP_EST )
		return SOCKET_ERROR;

	if( pSock->pRxActivePacket == NULL )
	{
		if( pSock->RxMsgQueue.PktCount ) 
		{
			//retreive the Datagram from Rx Fifo
			Que_GetHead( &pSock->RxMsgQueue, &pSbuf );
			pSock->pRxActivePacket = pSbuf;
			pSock->DataByteIndex = 0;
		}
		else // No data available
		{
			if( pSock->smState > TCP_EST ) //not in established state - somebody is closing
				return SOCKET_ERROR; //Inform the app that the connection is being closed
			else
				return 0; //just tell app there is no data
		}
	}
	else
		pSbuf = pSock->pRxActivePacket;

	AvailLen = pSbuf->PktLen - pSock->DataByteIndex;

	if( len > AvailLen ) //app is asking for more than what we have in the packet
		len = AvailLen;

	memcpy( buf, &pSbuf->pData[pSock->DataByteIndex], len );

	return AvailLen;
}


/*********************************************************************
 * Function:        int TCPExtractData( struct MCSocket* pSock, BYTE* buf, int len )

 *
 * PreCondition:    None
 * Input:			pSock - Socket structure pointer
 *					buf - data buffer
 *					len - max data length
 * Output:          returns number of bytes transferred to buf, negative number for error
 * Side Effects:    None
 * Overview:        This function extract data from TCP packet received
 * Note:			Number of bytes recevied can be less than len.
 ********************************************************************/
int TCPExtractData( struct MCSocket* pSock, BYTE* buf, int len )
{
	NET_PKT *pSbuf;
	int AvailLen, BytesRead = 0;

	// socket must be in or past established state
	if( pSock->smState < TCP_EST )
		return SOCKET_ERROR;

	if( pSock->pRxActivePacket == NULL )
	{
		if( pSock->RxMsgQueue.PktCount ) 
		{
			//retreive the Datagram from Rx Fifo
			Que_GetHead( &pSock->RxMsgQueue, &pSbuf );
			pSock->pRxActivePacket = pSbuf;
			pSock->DataByteIndex = 0;
		}
		else // No data available
		{
			if( pSock->smState > TCP_EST ) //not in established state - somebody is closing
				return SOCKET_ERROR; //Inform the app that the connection is being closed
			else
				return 0; //just tell app there is no data
		}
	}
	else
		pSbuf = pSock->pRxActivePacket;

	AvailLen = pSbuf->PktLen - pSock->DataByteIndex;

	if( len >= AvailLen ) //app is asking for more than or equal to what we have in the active packet
	{
		len -= AvailLen;
		memcpy( buf, &pSbuf->pData[pSock->DataByteIndex], AvailLen );
		//all data of packet is consumed - deAllocate
		DeAllocateBuffer( pSock->pRxActivePacket );
		pSock->pRxActivePacket = NULL;
		BytesRead += AvailLen;
		if( len )
		{
			//try to get more data (if we have more packets lined up)
			AvailLen = TCPExtractData( pSock, buf+AvailLen, len );

			if( AvailLen > 0 ) //good data
				BytesRead += AvailLen;
		}
	}
	else
	{
		memcpy( buf, &pSbuf->pData[pSock->DataByteIndex], len );
		pSock->DataByteIndex += len;
		BytesRead = len;
	}

	return BytesRead;
}

/*********************************************************************
 * Function:        void ProcessSYN( struct MCSocket* pListenSock, NET_PKT* pNetPkt )
 * PreCondition:    None
 * Input:			pListenSock - Socket structure pointer
 *					pNetPkt - packet buffer
 * Output:          None
 * Side Effects:    None
 * Overview:        This function replys and acks the SYN received from remote
 * Note:			
 ********************************************************************/
void ProcessSYN( struct MCSocket* pListenSock, NET_PKT* pNetPkt )
{
	// recevied syn on a listening socket	
	struct MCSocket *pNewSock;
	SOCKET sd;
	TCP_HEADER *pTCP;
    DWORD   src_ip, seq_num;

	pTCP = (TCP_HEADER*)pNetPkt->pTransportLayer;

	//create a new socket for this connection
	sd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( sd == SOCKET_ERROR )
		return;

    src_ip = _arrayToDword(pNetPkt->pSrcIP);

	//bind to a unique local port
	bind( sd, IP_ADDR_ANY, 0 );

	pNewSock = &SocketArray[sd];

	pNewSock->remoteIP	= src_ip;
	pNewSock->remotePort = pTCP->SourcePort;
	
	pNewSock->smState	= TCP_SYN_RCVD;

    seq_num = _arrayToDword(pTCP->SeqNum);
   
	pNewSock->TcpSEQ	= ++ISS; 		//our own start seq#
	pNewSock->TcpACK	= seq_num + 1, //client seq + 1
	
	pNewSock->pParentSocket = pListenSock;

	pNewSock->startTick  = TickGet();
	pNewSock->RetryCount = 0;
	
	TCPXmitPkt(pNewSock->remoteIP,
               pListenSock->localPort,
               pNewSock->remotePort,
               pNewSock->TcpSEQ++, //take takes up 1 seq#
               pNewSock->TcpACK,
               SYN | ACK, NULL, 0, NULL);
               

}

/*********************************************************************
 * Function:        void PurgeQueues( struct MCSocket *pSock )
 * PreCondition:    None
 * Input:			pSock - Socket structure pointer
 * Output:          None
 * Side Effects:    None
 * Overview:        DeAllocate all packets in socket queues and buffers
 * Note:			
 ********************************************************************/
void PurgeQueues( struct MCSocket *pSock )
{
	NET_PKT *pktPtr;
	NET_PKT *oldPtr;

	while( Que_GetHead( &pSock->RxMsgQueue, &pktPtr ) )
		DeAllocateBuffer( pktPtr );

	while( Que_GetHead( &pSock->CnxnQueue, &pktPtr ) )
		DeAllocateBuffer( pktPtr );

	if( pSock->pRxActivePacket )
	{
		DeAllocateBuffer( pSock->pRxActivePacket );
		pSock->pRxActivePacket = NULL;
	}

	if( pSock->pTxActivePacket )
	{
		DeAllocateBuffer( pSock->pTxActivePacket );
		pSock->pTxActivePacket = NULL;
	}

	pktPtr = pSock->pTCPListHead;
	oldPtr = pSock->pTCPListHead;

	while( pktPtr )
	{

        if( IP_IsARPqueued( pktPtr ) )
        {
            pktPtr->TxAutoDeAlloc = TRUE;  //let someone else deallocate buffer
        }else
        {
            pktPtr = DeallocateAckList(pSock, pktPtr);

            if(!pktPtr)
                break;
        }

        pktPtr = pktPtr->pTcpNext;    
	}
	
	pSock->AckListCount = 0;

}

/*********************************************************************
 * Function:        void FlushTxBuffer( struct MCSocket *pSock )
 * PreCondition:    None
 * Input:			pSock - Socket structure pointer
 *					TxFlags - TCP Tx Flags
 * Output:          returns true if a packet was transmited, else false
 * Side Effects:    None
 * Overview:        Transmits any buffered data in Naggle Tx Buffer
 * Note:			
 ********************************************************************/
BOOL FlushTxBuffer( struct MCSocket *pSocket, BYTE TxFlags )
{
	int lp = pSocket->localPort;

	if( pSocket->pParentSocket ) //if we are part of listening socket
		lp = pSocket->pParentSocket->localPort;
	
	if( pSocket->pTxActivePacket )
	{
		TCPXmitPkt(pSocket->remoteIP,
	               	lp,
	               	pSocket->remotePort,
	               	pSocket->TcpSEQ,
	               	pSocket->TcpACK,
					TxFlags, pSocket->pTxActivePacket, pSocket->TxDataIndex , pSocket);

		pSocket->pTxActivePacket = NULL;
		pSocket->TcpSEQ += pSocket->TxDataIndex; //for next segment use
		pSocket->TxDataIndex = 0;
		
		if( TxFlags & FIN )
			pSocket->TcpSEQ++;
			
		return TRUE;
	}
	
	return FALSE;
}

/*********************************************************************
 * Function:        void CleanupTCPSocket( struct MCSocket *pSock )
 * PreCondition:    None
 * Input:			pSock - Socket structure pointer
 * Output:          None
 * Side Effects:    None
 * Overview:        DeAllocate all memory pointers and init socket structure
 * Note:			
 ********************************************************************/
void CleanupTCPSocket( struct MCSocket *pSock )
{
	PurgeQueues( pSock );
	SocketInitialize(pSock->SocketID); //initialize the socket memory
}

/*********************************************************************
 * Function:        void TCPCloseSocket( struct MCSocket *pSock )
 * PreCondition:    None
 * Input:			pSock - Socket structure pointer
 * Output:          None
 * Side Effects:    None
 * Overview:        This function sends FIN message to remote node
 *					Called by Socket Layer (initiated by App closesocket)
 * Note:			
 ********************************************************************/
void TCPCloseSocket( struct MCSocket *pSock )
{
	BYTE TxFlags = 0;
		
	if( pSock->smState == TCP_CLOSE_WAIT ) //If we are in passive close state, reply FIN to the client
	{
		TxFlags = FIN | ACK;
		pSock->smState = TCP_LAST_ACK;  //wait for ack and then remove socket
	}
	else if( pSock->smState == TCP_EST ) //Active close
	{
		pSock->smState = TCP_FIN_WAIT_1;  //wait for FIN|ACK and then remove socket
		
		if( !FlushTxBuffer( pSock, FIN|PSH|ACK ) )
			TxFlags = FIN|ACK;
		
		PurgeQueues( pSock );
	}
	else if( pSock->smState == TCP_RESET_RCVD ) //force close by client, OK by the app
	{
		CleanupTCPSocket( pSock );
		return;
	}
	else if( pSock->smState < TCP_EST )
	{
		CleanupTCPSocket( pSock );
		return;
	}

	if( TxFlags )
	{
		int lp = pSock->localPort;

		if( pSock->pParentSocket ) //if we are part of listening socket
			lp = pSock->pParentSocket->localPort;
			
		TCPXmitPkt(pSock->remoteIP,
	               	lp,
	               	pSock->remotePort,
	               	pSock->TcpSEQ,
	               	pSock->TcpACK,
					TxFlags, NULL, 0, NULL);

		pSock->TcpSEQ++;  //FIN takes 1 byte
	}
	
	// App has initiated close on this socket.  start timer.  
	// if timer expires we will delete the socket.
	// normally socket will be deleted before timer expires.
	pSock->TCPAppCloseTimer = TickGet();
	pSock->TCPAppClosed = TRUE;
}

/*********************************************************************
 * Function:        void HandleTCPSegment( SOCKET s, NET_PKT* pNetPkt, WORD len )
 * PreCondition:    None
 * Input:			s - socket index
 *					pNetPkt - Packet buffer
 *					len - length of TCP data
 * Output:          None
 * Side Effects:    None
 * Overview:        This function process a new TCP message
 *					received from remote node
 * Note:			
 ********************************************************************/
void HandleTCPSegment( SOCKET s, NET_PKT* pNetPkt, WORD len )
{
	TCP_HEADER *pTCP;
	struct MCSocket *pSocket, *pSockTemp;
	BOOL DeAllocBfr = FALSE;
	BYTE TxFlags = 0;
	char tempStr[10];
	NET_PKT* pktPtr;
	BYTE DeAllocCount = 0;
	TCP_HEADER *pTxHdr;
	WORD lp;
	DWORD   seq_num, ack_num;

	pTCP = (TCP_HEADER*)pNetPkt->pTransportLayer;
	pSocket = &SocketArray[s];
	pNetPkt->pSocket = pSocket;
	
	//Reset retry count as we just heard from the peer
    pSocket->RetryCount	= 0;
    pSocket->startTick	= TickGet();

	switch( pSocket->smState )
	{
	///////////////////////
	case TCP_LISTEN:
	
        if ( pTCP->Flags & SYN ) //Connection Request
        {
	        if( pSocket->CnxnQueue.PktCount < pSocket->backlog ) //Room in the backlog queue
				ProcessSYN( pSocket, pNetPkt );
            else
                lp = 0xFFFF;

            DeAllocBfr = TRUE;
			break;
		}

		if( pTCP->Flags & RST )
		{
			s++;
			s--;
		}

		
		//this is Data/ack packet to the master listening port.  
		//find the matching child port for this client
	    pSockTemp = SocketArray;
	
	    for ( s = 0; s < MAX_SOCKET; s++,pSockTemp++ )
	    {
            DWORD   src_ip;

            src_ip = _arrayToDword(pNetPkt->pSrcIP);
			
            if( pSockTemp->SocketType == SOCK_STREAM &&
				pSockTemp->smState >= TCP_SYN_RCVD && 
				pSockTemp->remoteIP == src_ip &&
				pSockTemp->remotePort == pTCP->SourcePort )
			{
				return HandleTCPSegment( s, pNetPkt, len );
			}
		}
		
		DeAllocBfr = TRUE;
		break;

	///////////////////////
	case TCP_SYN_RCVD:
	
		// SYN RCV state is set by the listening socket (parent socket). 
		// we will have a pointer to the parent socket.
		if( !(pTCP->Flags & ACK) )
		{
			DeAllocBfr = TRUE;
			break;
		}			
		
		ack_num = _arrayToDword(pTCP->AckNum);

        if( pSocket->TcpSEQ != ack_num )
		{
			DeAllocBfr = TRUE;
			break;
		}
				
		//good ack recevied - go to TCP_EST state
		seq_num = _arrayToDword(pTCP->SeqNum);

        pSocket->TcpACK     = seq_num + len;
		pSocket->smState    = TCP_EST;

		pNetPkt->pSocket    = pSocket;
		pNetPkt->PktLen     = len;
		pNetPkt->pData      = pNetPkt->pTransportLayer + (pTCP->HdrLen >> 2);
		
		// save this packet in the master listener backlog queue
		pSockTemp = pSocket->pParentSocket; 
		Que_AddTail( &pSockTemp->CnxnQueue, pNetPkt );


		if( len )
		{
			TCPXmitPkt(pSocket->remoteIP,
	              pSocket->pParentSocket->localPort,
	              pSocket->remotePort,
	              pSocket->TcpSEQ,
	              pSocket->TcpACK,
	              ACK, NULL, 0, NULL);
		}
			
		break;

	///////////////////////
	// Embedded Client Initiated connection
	case TCP_SYN_SENT:
		
        if ( pTCP->Flags & SYN ) //Connection Request reply from remote client
        {
	        NET_PKT* tempPkt;
	        
			
		    seq_num = _arrayToDword(pTCP->SeqNum);

			pSocket->TcpACK	= seq_num + 1, //client seq + 1
			
			TCPXmitPkt(pSocket->remoteIP,
	              pSocket->localPort,
	              pSocket->remotePort,
	              pSocket->TcpSEQ,
	              pSocket->TcpACK,
	              ACK, NULL, 0, NULL);
	              
	        pSocket->smState = TCP_EST;
	        
			while( (tempPkt = DeQueAckList( pSocket )) )
				DeAllocateBuffer( tempPkt );	        
		}
			
		DeAllocBfr = TRUE;
	
	break;
	
	///////////////////////
	case TCP_EST:
	case TCP_FIN_WAIT_1:  //we initiated FIN - Active
	case TCP_CLOSE_WAIT: //client initiated FIN - Passive
	case TCP_LAST_ACK: //waiting for ack on our passive FIN reply
		

		itoa( pTCP->Flags, tempStr );
		
		if( pTCP->Flags & RST )
		{
			if( pSocket->smState == TCP_EST || pSocket->smState == TCP_CLOSE_WAIT )
			{
				if( pSocket->smState == TCP_EST )
					pSocket->smState = TCP_RESET_RCVD;
					
				PurgeQueues( pSocket );
			}
			else
				CleanupTCPSocket( pSocket );

			DeAllocBfr = TRUE;
			break;		
		}

		if( pTCP->Flags & ACK )
		{
			if( pSocket->smState == TCP_LAST_ACK ) // passive close. rcvd ACK for our FIN reply
			{
				// client ACK our FIN reply - goto closed state
				// App has already authorized removal
				CleanupTCPSocket( pSocket );
				DeAllocBfr = TRUE;
				break;
			}
			
			if( pSocket->AckListCount ) //we have a tx packet that needs ack
			{
				DWORD localSeq;
				//dealloc packets that are acked by this packet
				//allows to send more packets
				
				pktPtr = pSocket->pTCPListHead;
				DeAllocCount = 0;
				
				while( pktPtr )
				{
                    
		            ack_num = _arrayToDword(pTCP->AckNum);

                    if( ack_num >= pktPtr->TCPACKNum )
					{
						DeAllocCount++;
					}
					else
					{
						break;
					}
					pktPtr = pktPtr->pTcpNext;
				}

                while( DeAllocCount )
				{
					if( (pktPtr = DeQueAckList( pSocket )) )
						DeAllocateBuffer( pktPtr );	
						
					DeAllocCount--;
				}
			}
		}

		if( pTCP->Flags & FIN ) //finish
		{
			if( pSocket->smState == TCP_FIN_WAIT_1 ) //Acitve Close - rcvd FIN reply
			{
		
		        seq_num = _arrayToDword(pTCP->SeqNum);

				pSocket->TcpACK = seq_num + len + 1; //calculate the ack			

				if( pSocket->pParentSocket ) //if we are part of listening socket
					lp = pSocket->pParentSocket->localPort;
				else
					lp = pSocket->localPort;
									
				TCPXmitPkt(pSocket->remoteIP,
		              lp,
		              pSocket->remotePort,
		              pSocket->TcpSEQ,
		              pSocket->TcpACK,
		              ACK, NULL, 0, NULL);				
				
				CleanupTCPSocket( pSocket ); //deallocate all memory for this socket 
				
				DeAllocBfr = TRUE;
				
				break;
				
			}
			else if( pSocket->smState == TCP_EST ) //start passive close
			{
				pSocket->smState = TCP_CLOSE_WAIT;  //let our app send FIN reply
				
				TxFlags |= ACK;
					
				//PurgeQueues( pSocket );
			}
		}
			
		if( len > 0 )
		{
			// Make sure packet is in sequence.
		    seq_num = _arrayToDword(pTCP->SeqNum);

			if( pSocket->TcpACK == seq_num )
			{
				if(( pSocket->smState == TCP_EST ) || ( pTCP->Flags & FIN ))
				{
					pNetPkt->PktLen = len;
					pNetPkt->pData = pNetPkt->pTransportLayer + (pTCP->HdrLen >> 2);
		
					// Packet Contains data - save it in our incomming queue
					Que_AddTail( &pSocket->RxMsgQueue, pNetPkt );
					TxFlags |= ACK;
				}
				else
				{
					TxFlags |= RST|ACK;
				}

				//calculate the ack
				pSocket->TcpACK = seq_num + len;
			}
			else
				DeAllocBfr = TRUE;
		}
		else
			DeAllocBfr = TRUE;

		// If we are buffering Tx data, then transmit now.
		if( pSocket->pTxActivePacket && (pTCP->Flags & ACK) )
		{
			FlushTxBuffer( pSocket, PSH|ACK );
			TxFlags = 0;
		}

		if( TxFlags )
		{
			if( pSocket->pParentSocket ) //if we are part of listening socket
				lp = pSocket->pParentSocket->localPort;
			else
				lp = pSocket->localPort;			
			
			TCPXmitPkt(pSocket->remoteIP,
	              lp,
	              pSocket->remotePort,
	              pSocket->TcpSEQ,
	              pSocket->TcpACK,
	              TxFlags, NULL, 0, NULL);
	    }
		
		break;

	///////////////////////
	case TCP_RESET_RCVD:
		DeAllocBfr = TRUE;
		break;

	///////////////////////
	default:
		DeAllocBfr = TRUE;
		break;
	}

	if( DeAllocBfr )
		DeAllocateBuffer(pNetPkt);
}

/*********************************************************************
 * Function:        void TCPTimer(void)
 *
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Retransmits last message that timedout on ACK
 * Note:            None
 ********************************************************************/
void TCPTimer(void)
{
	struct MCSocket *pSocket;
    TICK diffTicks;
    TICK tick;
    SOCKET s;

    BYTE flags;

    flags = 0x00;
     
    pSocket = SocketArray;

    tick = TickGet();

    for ( s = 0; s < MAX_SOCKET; s++,pSocket++ )
    {
		// If app has closed the socket, we will not hold on to the socket for ever.
		if( pSocket->TCPAppClosed )
		{
			diffTicks = TickGetDiff( tick, pSocket->TCPAppCloseTimer );
			if( diffTicks > TCP_WAIT_SOCKET_DEL )
			{
				CleanupTCPSocket( pSocket );
				continue;
			}
		}

		// If we are buffering a tx packet then transmit it if naggle timer expires
		if( pSocket->pTxActivePacket )
		{
			diffTicks = TickGetDiff( tick, pSocket->NagglesTimer );
			if( diffTicks > NAGGLES_TX_BFR_TIMEOUT )
			{
				FlushTxBuffer( pSocket, PSH|ACK );
			}
		}

		diffTicks = TickGetDiff( tick, pSocket->startTick );
		if( diffTicks <= TCP_RETRY_TIMEOUT_VAL )
			continue;

		//below is timedout state

    	pSocket->startTick = tick;  //reset timer

	    if( pSocket->smState == TCP_SYN_RCVD ) //someone tried to establish cnxn, but did not complete
	    {
			CleanupTCPSocket( pSocket );
			continue;
		}

		// we are trying to connect to remote server.  timedout on SYN
	    if( pSocket->smState == TCP_SYN_SENT )
	    {
			// Initiate another connection request
		    TCPXmitPkt(pSocket->remoteIP,
		            pSocket->localPort,
		            pSocket->remotePort,
		            pSocket->TcpSEQ++, //SYN takes 1 seq number
		            pSocket->TcpACK,
		            SYN, NULL, 0, NULL );
			continue;
		}

		// we need to retransmit packets that did not get ack by remote node	    	    
		if( pSocket->AckListCount )
		{
	      	NET_PKT *pktPtr;
			
			if( pSocket->RetryCount <= MAX_RETRY_COUNTS )
	      	{
	      		pSocket->RetryCount++;			      		
		    	
				pktPtr = pSocket->pTCPListHead;
				
				while( pktPtr )
				{
		    		IP_TransmitPkt( pktPtr );
					pktPtr = pktPtr->pTcpNext;
				}
		    }
		    else // Retry count expired - deAlloc all packets
		   	{
				pktPtr = pSocket->pTCPListHead;
				
				while( pktPtr )
				{
					if( IP_IsARPqueued( pktPtr ) )
						pktPtr->TxAutoDeAlloc = TRUE;  //let someone else deallocate buffer
					else
						DeAllocateBuffer( pktPtr );
		    	
					pktPtr = pktPtr->pTcpNext;
				}
				
				pSocket->AckListCount = 0;
			}
		}
	}
}
/*********************************************************************
 * Function:        int TCPAccept( struct MCSocket *pListenSock,
 * 					struct sockaddr * addr,
 *					int * addrlen )
 *
 * PreCondition:    None
 * Input:           pListenSock - Pointer to socket
 *					addr - address pointer to receive peer address
 *					len - length of address structure
 * Output:          If no connection request is pending, return SOCKET_ERROR,
 *					otherwise, returns a socket descriptor.
 * Side Effects:    None
 *
 * Overview:        This function returns the socket id for a pending connection.
 * Note:            None
 ********************************************************************/
int TCPAccept( struct MCSocket *pListenSock,
				struct sockaddr * addr,
				int * addrlen )
{
	NET_PKT *pSbfr;
	struct MCSocket *pSock;
	struct sockaddr_in *rem_addr;
	int temp;
	
	if( !Que_GetHead( &pListenSock->CnxnQueue, &pSbfr ) )
		return SOCKET_ERROR;
	
	
	//Get the socket pointer
	pSock = pSbfr->pSocket;

	if( addr && addrlen && *addrlen >= sizeof(struct sockaddr_in) )
	{
		 rem_addr = (struct sockaddr_in *)addr;
		 *addrlen = sizeof(struct sockaddr_in);
		 
		 rem_addr->sin_family 		= AF_INET;
		 rem_addr->sin_port			= pSock->remotePort;
		 rem_addr->sin_addr.S_un.S_addr  = pSock->remoteIP;

		 for( temp = 0; temp < 8; temp++ )
		 	rem_addr->sin_zero[temp] = 0;
	}

	//if data in this packet, then make it active packet.	
	if( pSbfr->PktLen )
		pSock->pRxActivePacket = pSbfr;
	else
		DeAllocateBuffer(pSbfr);

	return pSock->SocketID;
}

/*********************************************************************
 * Function:        static void SwapTCPHeader(TCP_HEADER* header)
 *
 * PreCondition:    None
 * Input:           header - TCP Header pointer
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        this function convert data from network byte 
 * 					order to machine byte order and vice versa
 * Note:            None
 ********************************************************************/
static void SwapTCPHeader(TCP_HEADER* header)
{
    DWORD   seq_num, ack_num;

    header->SourcePort      = swaps(header->SourcePort);
    header->DestPort        = swaps(header->DestPort);

	seq_num = _arrayToDword(header->SeqNum);
	swapl(seq_num, &seq_num);
    _dwordToArray(seq_num, header->SeqNum);

	ack_num = _arrayToDword(header->AckNum);
	swapl(ack_num, &ack_num);
    _dwordToArray(ack_num, header->AckNum);

    header->Window          = swaps(header->Window);
    header->UrgentPointer   = swaps(header->UrgentPointer);
}
