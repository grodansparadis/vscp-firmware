/*********************************************************************
 *
 *                  Memory Block Manager for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        block_mgr.c
 * Processor:       PIC24
 * Complier:        MPLAB C30
 *
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
 * $Id: block_mgr.c,v 1.6 2006/10/24 16:39:07 C12923 Exp $
 *
 ********************************************************************/

#include "bsd_tcp_ip\block_mgr.h"
#include "bsd_tcp_ip\ether.h"
#include "bsd_tcp_ip\eip.h"
#include "string.h"


BYTE BlockPool[ MGR_MAX_BLOCKS ][ MGR_BLOCK_SIZE ];
BYTE BlockStatus[ MGR_MAX_BLOCKS ];
WORD BlockCounter[MGR_MAX_BLOCKS];
WORD _blockCounter = 0;
   
#ifdef ENABLE_HEAP_PKT_ALLOCATION
NET_PKT* HeapPktList[ MAX_HEAP_PKTS ];
#endif

#define BLOCK_OPEN			0xFF
#define BLOCK_UNDEFINED		0xFF

NET_PKT* AllocBuffer( WORD BfrSize );
/////////////////////////////////////////////
void InitBufferManager()
{
BYTE temp;

	for( temp = 0; temp < MGR_MAX_BLOCKS; temp++ )
    {
		BlockStatus[temp] = BLOCK_OPEN;
        BlockCounter[temp] = 0;
    }

#ifdef ENABLE_HEAP_PKT_ALLOCATION
	for( temp = 0; temp < MAX_HEAP_PKTS; temp++ )
		HeapPktList[temp] = NULL;
#endif
}

/////////////////////////////////////////////
NET_PKT* AllocSocketBuffer( WORD DataLen )
{
	return AllocPacketBuffer( DataLen + sizeof(ETHER_HEADER) + sizeof(IP_HEADER) );
}

/////////////////////////////////////////////
NET_PKT* AllocPacketBuffer( WORD BfrSize )
{
	NET_PKT* pSbfr;

	pSbfr = AllocBuffer( BfrSize );

	if( pSbfr == NULL )
		return NULL;

	pSbfr->pNext 		= NULL;
	pSbfr->pMacLayer 	= (BYTE *)pSbfr->pPkt;
	pSbfr->PktLen		= 0;
	pSbfr->pSrcMAC 		= NULL;
	pSbfr->pSrcIP		= NULL;
	pSbfr->pDestIP		= NULL;
	pSbfr->TimeStamp	= TickGet();
	pSbfr->TxAutoDeAlloc = TRUE;
	pSbfr->pTcpNext		= NULL;
	pSbfr->pSocket		= NULL;

	return pSbfr;
}

/////////////////////////////////////////////
NET_PKT* AllocBuffer( WORD BfrSize )
{
    BYTE idx, bcnt, startBlock;
    BYTE nBlocks;

	if(!BfrSize)
		return NULL;

	//find out how many blocks we will need
	BfrSize     += sizeof(NET_PKT);
	nBlocks     = BfrSize / MGR_BLOCK_SIZE;

    if( BfrSize % MGR_BLOCK_SIZE )
		nBlocks++;

    if(nBlocks >= MGR_MAX_BLOCKS)
        return NULL;

	
	startBlock  = BLOCK_UNDEFINED;
	bcnt        = nBlocks;

	for( idx = 0; idx < MGR_MAX_BLOCKS; idx++ )
	{
		if( startBlock == BLOCK_UNDEFINED && BlockStatus[idx] == BLOCK_OPEN )
			startBlock = idx; //remember the first open block

		if( startBlock != BLOCK_UNDEFINED && BlockStatus[idx] == BLOCK_OPEN )
			bcnt--;

		if(!bcnt)	//we have found the required contigous area
		{
			BYTE mem_idx;
            BYTE stop_idx;

            //mark the blocks with the start block ID
			//to identify them in de-allocate
			_blockCounter++;
            stop_idx = startBlock + nBlocks;

            for( mem_idx = startBlock; mem_idx < stop_idx; mem_idx++ )
            {
				BlockStatus[mem_idx] = startBlock;	
                BlockCounter[mem_idx] = _blockCounter;
            }

		    return (NET_PKT*)BlockPool[startBlock];
		}
		else if( startBlock != BLOCK_UNDEFINED && BlockStatus[idx] != BLOCK_OPEN )
		{
			//non-contigous, reset control variables
			bcnt = nBlocks;
			startBlock = BLOCK_UNDEFINED;
		}
	}

			
#ifndef ENABLE_HEAP_PKT_ALLOCATION
	return NULL;		                    //no static buffer available
#else
	//use malloc to reserve mem for this packet
	
	for( temp = 0; temp < MAX_HEAP_PKTS; temp++ )
	{
		if( HeapPktList[temp] == NULL )
		{
			HeapPktList[temp] = (NET_PKT*)malloc( BfrSize );
			break;
		}
	}

	if( temp == MAX_HEAP_PKTS )  //HeapPktlist overflow
		return NULL;	
	else
	{
		return HeapPktList[temp];
	}
#endif

}

///////////////////////////////////////////
void DeAllocateBuffer( NET_PKT* pBfr )
{
BYTE bcnt, startBlock;
BYTE nBlocks = 0;

	for( startBlock = 0; startBlock < MGR_MAX_BLOCKS; startBlock++ )	
	{
		//Find the address in the pool
		if( pBfr == (NET_PKT*)BlockPool[startBlock] )
		{
			for( bcnt = startBlock; bcnt < MGR_MAX_BLOCKS; bcnt++ )
			{	
				if( BlockStatus[ bcnt ] == startBlock )
				{
					BlockStatus[ bcnt ] = BLOCK_OPEN;
					nBlocks++; //only for debug
				}
				else
					return; //done unmarking
			}
		}
	}

#ifdef ENABLE_HEAP_PKT_ALLOCATION
	// not a static allocated packet.  
	// try heap pkt list
	for( temp = 0; temp < MAX_HEAP_PKTS; temp++ )
	{
		if( HeapPktList[temp] == pBfr )
		{
			free( pBfr );
			HeapPktList[temp] = NULL;
		}
	}
#endif

}
			
