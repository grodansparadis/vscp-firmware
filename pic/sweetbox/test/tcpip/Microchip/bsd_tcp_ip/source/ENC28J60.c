/*********************************************************************
 *
 *     MAC Module (Microchip ENC28J60) for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ENC28J60.c
 * Dependencies:    ENC28J60.h
 *					MAC.h
 *					string.h
 *                  StackTsk.h
 *                  gpFunc.h
 *					Delay.h
 * Processor:       PIC24/DAYTONA
 *                  
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
 * $Id: ENC28J60.c,v 1.7 2006/10/24 16:39:06 C12923 Exp $
 *
********************************************************************/
#include <string.h>
#include <p32xxxx.h>
#include <generic.h>
#include <plib.h>
#include "bsd_tcp_ip\etcpcfg.h"
#include "bsd_tcp_ip\gpfunc.h"
#include "bsd_tcp_ip\emac.h"
#include "bsd_tcp_ip\ether.h"
#include "bsd_tcp_ip\ENC28J60.h"
#include "bsd_tcp_ip\block_mgr.h"
#include "bsd_tcp_ip\netpkt.h"
#include "bsd_tcp_ip\pkt_queue.h"
#include "bsd_tcp_ip\tick.h"

/** D E F I N I T I O N S ****************************************************/
// Since the ENC28J60 doesn't support auto-negotiation, full-duplex mode is 
// not compatible with most switches/routers.  If a dedicated network is used 
// where the duplex of the remote node can be manually configured, you may 
// change this configuration.
//#define FULL_DUPLEX
#define HALF_DUPLEX
//#define LEDB_DUPLEX

// Duplex configuration options
typedef enum _DUPLEX {
	HALF = 0, 
	FULL = 1, 
	USE_PHY = 2
} DUPLEX;

/* Pseudo Functions */
#define LOW(a) 		(a & 0xFF)
#define HIGH(a)		((a>>8) & 0xFF)


/* NIC RAM definitions */
#define RAMSIZE	        8192u	
#define ETHER_HDR_LEN	14u
#define MAXFRAMEC	    (1500u + ETHER_HDR_LEN + 4u)
#define TXSIZE          (MAXFRAMEC + 16)
#define RXSIZE          (RAMSIZE - TXSIZE)
 
// According to the B0, B4 and B5 ENC28J60 Errata, the Rx Buffer
// should be placed at the lower address, starting at 0000h to n
// and the Tx buffer at n + 1 to 8191
#define RXSTART     0
#define RXSTOP      (RXSIZE - 1)
#define TXSTART	    (RXSTOP + 1)
#define TXEND	    (RAMSIZE - 1)		//one full frame tx buffer.


/* ENC28J60 Opcodes (to be ORed with a 5 bit address) */
#define	WCR (0x02<<5)			// Write Control Register command
#define BFS (0x04<<5)			// Bit Field Set command
#define	BFC (0x05<<5)			// Bit Field Clear command
#define	RCR (0x00<<5)			// Read Control Register command
#define RBM ((0x01<<5) | 0x1A)	// Read Buffer Memory command
#define	WBM ((0x03<<5) | 0x1A)  // Write Buffer Memory command
#define	SC  ((0x07<<5) | 0x1F)	// System Command does not use an address.  It
								//   requires 0x1F, however.
typedef struct _MCP_PREAMBLE
{
    WORD			NextPacketPointer;
    RXSTATUS		StatusVector;
    BYTE        	DestMACAddr[ETH_ADRLEN];
    BYTE        	SourceMACAddr[ETH_ADRLEN];
    WORD_VAL        Type;
} MCP_PREAMBLE;

typedef struct _MCP_PKT_HDR_
{
    WORD			NextPacketPointer;
    RXSTATUS		StatusVector;
} ENC28J60_PKT_HDR;

#define SetLEDConfig(NewConfig)		WritePHYReg(PHLCON, NewConfig)

/* Prototypes of functions intended for MAC layer use only */

WORD MACGetArray(BYTE *val, WORD len);
void MACPut(BYTE val);
void MACPutArray(BYTE *val, WORD len);
PHYREG ReadPHYReg(BYTE Register);
void WritePHYReg(BYTE Register, WORD Data);

void MACSetDuplex(DUPLEX DuplexState);

static BYTE ReadRAMbyte();
static void BankSel(WORD Register);
static REG ReadETHReg(WORD Address);
static REG ReadMACReg(WORD Address);
static void WriteReg(WORD Address, BYTE Data);
static void BFCReg(WORD Address, BYTE Data);
static void BFSReg(WORD Address, BYTE Data);
static void SendSystemCommand(void);
static BOOL XmitPkt( NET_PKT* sbfr );
BYTE ByteTransfer( BYTE SendByte );

#ifdef MAC_POWER_ON_TEST
static BOOL TestMemory(void);
#endif

/* Internal MAC level variables and flags */
WORD_VAL NextRxAddr;
PktQueue MACTxQueue;	//Transmit Queue

// make sure that we have at least one tick!
WORD    _txCollsionTick;
BOOL    _txCollisionEnable = FALSE;
BYTE    _txCollisionRetry;
//extern void ConfigINT2(unsigned int config);
//extern void ConfigINT4(unsigned int config);

/******************************************************************************
 * Inline Functions
 ******************************************************************************/
 
 
///////////////////////////////////////////////////////////////////////////////
//
//
 
static inline void OpenSPI(unsigned int config1, unsigned int config2)
{
#if defined (TCP_USE_SPI_1)
    OpenSPI1(config1, config2);
#else
    OpenSPI2(config1, config2);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//

static inline void CloseSPI(void)
{
#if defined (TCP_USE_SPI_1)
    CloseSPI1();
#else
    CloseSPI2();
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//

static inline void putcSPI(unsigned int data_out)
{
#if defined (TCP_USE_SPI_1)
    putcSPI1(data_out);
#else
    putcSPI2(data_out);
#endif

}

///////////////////////////////////////////////////////////////////////////////
//
//

static inline unsigned int getcSPI(void)
{
#if defined (TCP_USE_SPI_1)
    return getcSPI1();
#else
    return getcSPI2();
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//

static inline  __attribute__((always_inline)) unsigned int ChangeSPIBRG(unsigned int brg)
{
#if defined (MEDIASD_IF_SPI1)
    SpiChnSetBrg( 1, brg );
#else
    SpiChnSetBrg( 2, brg );
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//

static inline void SPISelectEthernet(void)
{
    mSetSPILatch();

    mSetCSOn();
}

///////////////////////////////////////////////////////////////////////////////
//
//

static inline void SPIUnselectEthernet(void)
{
    mSetSPILatch();
    mSetCSOff();
}

///////////////////////////////////////////////////////////////////////////////
//
//

static inline void SetReset(void)
{
    mSetResetOn();

}

///////////////////////////////////////////////////////////////////////////////
//
//

static inline void ClearReset(void)
{
    mSetResetOff();

}

/******************************************************************************
 * Function:        BYTE GetETHReg(WORD Address)
 *
 * PreCondition:    None
 *
 * Input:           Internal address of ENC
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This functions reads the internal ethernet register
 *
 * Note:            None
 *****************************************************************************/
static BYTE GetETHReg(WORD Address)
{
BYTE temp;

	// Select the chip and send the Read Control Register opcode/address
	SPISelectEthernet();
	ByteTransfer( RCR | (Address&0xFF) );
	temp = ByteTransfer( 0 );

	SPIUnselectEthernet();

	return temp;
} // end ReadETHReg

/******************************************************************************
 * Function:        void MACInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACInit sets up the PIC's SPI module and all the 
 *					registers in the ENC28J60 so that normal operation can 
 *					begin.
 *
 * Note:            None
 *****************************************************************************/
void MACInit(void)
{
	BYTE i;
	
	// Init transmit holding queue
	Que_Init( &MACTxQueue );

	/*
	 * Set up the SPI module on the PIC for communications with the ENC28J60
	 */

	//Configure port pins
	mSetUpCSDirection();
	mSetUpResetDirection();

    mSetUpINTDirection();
	mSetUpWOLDirection();

	SPISelectEthernet();
    SetReset();
    SleepMS(20);
    SPIUnselectEthernet();
    ClearReset();

   
    // SPI Clk = Fcore (no prescale)
    // IDLE state is low level
    // SDO Change on falling edge of clock
    // Input sampled at middle of data output time
    OpenSPI(SPI_OPEN_CFG_1, SPI_OPEN_CFG_2);

	// Wait for CLKRDY to become set.
	// Bit 3 in ESTAT is an unimplemented bit.  If it reads out as '1' that
	// means the part is in RESET or otherwise our SPI pin is being driven
	// incorrectly.  Make sure it is working before proceeding.
	do
	{
        i = GetETHReg(ESTAT);
	} while ( ( i & 0x08 ) || ( ~i & ESTAT_CLKRDY ) );


#ifdef MAC_POWER_ON_TEST
	// Do the memory test and enter a while always trap if a hardware error 
	// occured.  The LEDA and LEDB pins will be configured to blink 
	// periodically in an abnormal manner to indicate to the user that the 
	// error occured.
	if ( !TestMemory() )
	{
		SetLEDConfig( 0x0AA2 );		// Set LEDs to blink periodically
		while( 1 );			
	}
#endif

	// RESET the entire ENC28J60, clearing all registers
	SendSystemCommand();	
    SleepMS( 40 );
	
	/*
	 * Start up in Bank 0 and configure the receive buffer boundary pointers 
	 * and the buffer write protect pointer (receive buffer read pointer)
	 */
	NextRxAddr.Val = RXSTART;
        
	WriteReg( ERXSTL, LOW(RXSTART ) );
	WriteReg( ERXSTH, HIGH(RXSTART ) );

	WriteReg( ERXRDPTL, LOW(RXSTART ) );	// Write low byte first
	WriteReg( ERXRDPTH, HIGH(RXSTART ) );	// Write high byte last

#if RXSTOP != 0x1FFF	// The RESET default ERXND is 0x1FFF
	WriteReg( ERXNDL, LOW( RXSTOP ) );
	WriteReg( ERXNDH, HIGH( RXSTOP ) );
#endif
#if TXSTART != 0		// The RESET default ETXST is 0
	WriteReg( ETXSTL, LOW( TXSTART ) );
	WriteReg( ETXSTH, HIGH( TXSTART ) );
#endif

	/*
	 * Enter Bank 1 and configure Receive Filters 
	 * (No need to reconfigure - Unicast OR Broadcast with CRC checking is 
	 * acceptable)
	 */
	 //BankSel(ERXFCON);
	 //WriteReg(ERXFCON, ERXFCON_CRCEN);

	/*
	 * Enter Bank 2 and configure the MAC
	 */
	BankSel( MACON1 );

	// Before touching the MAC and PHY, make certain that 50us has passed 
	// since the call to SendSystemCommand() and that the OST has expired
	//Delay10us(30);           //RRoj: changed from 4 to 30

	// Enable the receive portion of the MAC
	WriteReg( MACON1, MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN );
	
	// Bring MAC and subsystem components out of RESET
	WriteReg(MACON2, 0x00);
	
	// Pad packets to 60 bytes, add CRC, and check Type/Length field.
	WriteReg(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
	
	// Set non-back-to-back inter-packet gap to 9.6us.  The back-to-back 
	// inter-packet gap (MABBIPG) is set by MACSetDuplex() which is called 
	// later.
	WriteReg(MAIPGL, 0x12);
	WriteReg(MAIPGH, 0x0C);

	// Set the maximum packet size which the controller will accept
	WriteReg(MAMXFLL, LOW(MAXFRAMEC));	
	WriteReg(MAMXFLH, HIGH(MAXFRAMEC));

	// TODO: Remove.  This fix is required for revision A0 beta silicon only.  
	// The reserved register must be 0x00 for future silicon versions, which is 
	// the reset default.  If 0x01 is written to 0x215 in future silicon, the 
	// PHY registers will not be readable/writable.  This code checks EREVID 
	// and writes the appropriate value.
	BankSel(EREVID);	// Enter Bank 3
	if( ReadETHReg(EREVID).Val == 0 )
	{
		BankSel(0x200);
		WriteReg(0x15, 0x01);
	}

	/*
     * Enter Bank 3 and initialize physical MAC address registers
	 */
	BankSel(MAADR0);
    WriteReg(MAADR0, SysMACAddr.v[5]);
    WriteReg(MAADR1, SysMACAddr.v[4]);
    WriteReg(MAADR2, SysMACAddr.v[3]);
    WriteReg(MAADR3, SysMACAddr.v[2]);
    WriteReg(MAADR4, SysMACAddr.v[1]);
    WriteReg(MAADR5, SysMACAddr.v[0]);

	// Disable half duplex loopback in PHY.  Bank bits changed to Bank 2 as a 
	// side effect.
	WritePHYReg(PHCON2, PHCON2_HDLDIS);           //RRoj:

	// Configure LEDA to display LINK status, LEDB to display TX/RX activity
	SetLEDConfig(0x0472);

	//Enable Tx Interrupt bits. (used only for status display)
	BFCReg( EIR, EIR_TXIF );
	BFSReg( EIE, EIE_TXIE | EIE_INTIE );

	// Use the external LEDB polarity to determine weather full or half duplex 
	// communication mode should be set.  
#if defined(FULL_DUPLEX)
	MACSetDuplex(FULL);		// Function exits with Bank 2 selected
#elif defined(HALF_DUPLEX)
	MACSetDuplex(HALF);		// Function exits with Bank 2 selected
#else
	MACSetDuplex(USE_PHY);	// Function exits with Bank 2 selected
#endif

	// Enable packet reception
	BFSReg(ECON1, ECON1_RXEN);

    ChangeSPIBRG(TCP_IP_SPI_BRG);

    SleepMS(40);

}//end MACInit


void DecodeStatusVector( WORD LastPktLen )
{
	WORD offset;
	struct 
	{
		WORD TxBytecount;
		struct 
		{
			int TransmitCollisionCount : 4;
			int TransmitCRCError:1;
			int TransmitLengthCheckError:1;
			int TransmitLenghtOutOfRange:1;
			int TransmitDone:1;
			int TransmitMulticast:1;
			int TransmitBroadcast:1;
			int TransmitPacketDefer:1;
			int TransmitExcessiveDefer:1;
			int TransmitExcessiveCollision:1;
			int TransmitLateCollision:1;
			int TransmitGiant:1;
			int TransmitUnderrun:1;
		};
		
		int TotalBytesTransmitted;
		
		struct
		{
			int TransmitControlFrame:1;
			int TransmitPauseControlFrame:1;
			int BackpressureApplied:1;
			int Zeros:11;
		};
	}VectorBfr;
			
		

	
	while( ReadETHReg(ECON1).ECON1bits.TXRTS )
	{}

	offset = TXSTART + LastPktLen + 1;
	
	BankSel(ERDPTL);
	WriteReg(ERDPTL, LOW(offset));
	WriteReg(ERDPTH, HIGH(offset));

	MACGetArray( (BYTE*)&VectorBfr, 8 );	
	
	offset++;
	
	offset++;
	
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
void MACTxProcess(void)
{
    NET_PKT* pPktBfr;
	
    // Silicon Errata Work Around B rev
	// This is a silicon errata workaround
	if(ReadETHReg(EIR).EIRbits.TXERIF)
	{
		BFSReg(ECON1, ECON1_TXRST);
		BFCReg(ECON1, ECON1_TXRST);
    	BFCReg(EIR, EIR_TXERIF);
	}
    
    // if the transmission has occured, clear the flags and counters
    if(!ReadETHReg(ECON1).ECON1bits.TXRTS)
    {
        _txCollisionEnable = FALSE; 
        _txCollisionRetry = 0;
    }

	// this is a software work-around 
    // if a collision occurs, then the tx
    // state machine will not properly be reset
    // we will check to see if the transmited 
    // packet has been in the buffer for to long of a 
    // time
    if((ReadETHReg(ECON1).ECON1bits.TXRTS) && (_txCollisionEnable))
    {
        TICK collision_ticks;
        TICK tock;
        
        tock = TickGet();
        
        collision_ticks = TickGetDiff(tock, _txCollsionTick);

        if(collision_ticks > TX_COLLSION_TIMEOUT)
        {
	        // clear the TXRTS to abort the tx attempt
            BFCReg(ECON1, ECON1_TXRTS);
            asm volatile("nop");
            // set the TXRST to reset the tx logic and force a tx state machine in a known state
            BFSReg(ECON1, ECON1_TXRST);
            asm volatile("nop");
	        // clear the TXRST
            BFCReg(ECON1, ECON1_TXRST);
            asm volatile("nop");

            if(_txCollisionRetry < 10)           // try ten times
            {
	            BFCReg( EIR, EIR_TXIF ); //Clear Tx Interrupt Flag
                // set the TXRTS to try and re-transmit the packet
                BFSReg(ECON1, ECON1_TXRTS);
                asm volatile("nop");
                _txCollsionTick = TickGet();
                _txCollisionEnable = TRUE;
                _txCollisionRetry++;
                return;
            }else
            {
                _txCollisionEnable = FALSE;
                _txCollisionRetry = 0;
            }
        }
    }
	// End Silicon Errata Work Around B rev

    if( !MACTxQueue.PktCount )
		return;		//nothing in the queue, so just return.

	// we have one or more packet in Tx Queue
	// Make sure link is up. If not discard the Tx Queue

	MACIsLinked();	//unlatch so we get the true result in the next call

	if( !MACIsLinked() )
	{
		//link is down. empty the queue
		while( Que_GetHead( &MACTxQueue, &pPktBfr ) )
		{
			DeAllocateBuffer( pPktBfr );
		}

		return;
	}

    // Link is up and Tx Queue not empty - send one pkt out if chip ready

	// Make sure the transmitter is ready
	if( !ReadETHReg(ECON1).ECON1bits.TXRTS )
	{
		if( Que_GetHead( &MACTxQueue, &pPktBfr ) )
		{
			XmitPkt( pPktBfr );

			if( pPktBfr->TxAutoDeAlloc )
				DeAllocateBuffer( pPktBfr );
		}
	}
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
NET_PKT* MACGetRxPacket(void)
{
	NET_PKT *pNetPkt;
    ENC28J60_PKT_HDR PktPreamble;
    BYTE pkt_cnt;
    WORD_VAL next_location;


	BankSel(EPKTCNT);
	pkt_cnt = ReadETHReg(EPKTCNT).Val;
	BankSel(ERDPTL);
	if(!pkt_cnt)
        return NULL;
  
	// Set the SPI read pointer to the beginning of the next unprocessed packet
	BankSel(ERDPTL);
	WriteReg(ERDPTL, LSB(NextRxAddr));
	WriteReg(ERDPTH, MSB(NextRxAddr));

	// Obtain the MAC header from Ethernet buffer
	MACGetArray((BYTE*)&PktPreamble, sizeof(PktPreamble));
	
	// Testing B0 silicon.
	// TODO: Remove this code.  In revision A0 beta silicon, a bug exists 
	// which can cause the receive buffer to become corrupt when moving the 
	// ERXRDPT value.  They do not update as a WORD, but rather as two 
	// separate BYTEs.  In future silicon revisions, receive buffer corruption
	// should never be possible.
    {
        BOOL    bad_pkt = FALSE;

        // check the next pointer bounds
        bad_pkt = (PktPreamble.NextPacketPointer > RXSTOP || (PktPreamble.NextPacketPointer & 0x0001));
        bad_pkt |= (PktPreamble.StatusVector.bits.Status & (RXSTATUS_ZERO | RXSTATUS_CRC_ERR));
        bad_pkt |= (PktPreamble.StatusVector.bits.ByteCount > MAX_ETHERNET_FRAME_SZ);
        bad_pkt |= (!(PktPreamble.StatusVector.bits.Status & RXSTATUS_RX_OK));

//	    if(PktPreamble.NextPacketPointer > RXSTOP || 
//	       PktPreamble.StatusVector.bits.ByteCount > MAX_ETHERNET_FRAME_SZ)
	    if(bad_pkt)
	    {
		    // Reset the part.  We've gotten corrupt.
		    MACInit();
		    return NULL;
	    }
    }
	// Make sure we have buffer for this packet
	pNetPkt = (NET_PKT*)AllocPacketBuffer( PktPreamble.StatusVector.bits.ByteCount );
	if( pNetPkt )
	{
		//save packet size
		pNetPkt->PktLen 	= PktPreamble.StatusVector.bits.ByteCount;
	
		//off-load this packet from NIC 28J60
		MACGetArray( (BYTE *)pNetPkt->pPkt, pNetPkt->PktLen );
	}
	else
	{
		// no room to store the packet. leave it on the chip
		//return NULL;
		pNetPkt = NULL; // remove packet from the controller and discard
	}

	// Save the location where the hardware will write the next packet to
	// Decrement the next packet pointer before writing it into 
	// the ERXRDPT registers.  This is a silicon errata workaround.
	// RX buffer wrapping must be taken into account if the 
	// NextPacketLocation is precisely RXSTART.
    NextRxAddr.Val = PktPreamble.NextPacketPointer;

    next_location.Val = PktPreamble.NextPacketPointer - 1;
	
    if((next_location.Val > RXSTOP))
	{
		next_location.Val = RXSTOP;
	}
	
        
	// Decrement the RX packet counter register, EPKTCNT
	BFSReg(ECON2, ECON2_PKTDEC);

	// Move the receive read pointer to unwrite-protect the memory used by the 
	// last packet.  The writing order is important: set the low byte first, 
	// high byte last.
	BankSel(ERXRDPTL);
	WriteReg(ERXRDPTL, LSB(next_location));
	WriteReg(ERXRDPTH, MSB(next_location));

	return pNetPkt;
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
BOOL MACTransmit( NET_PKT* sbfr )
{	
	// TODO: implement push flag handling here
	
	if( MACTxQueue.PktCount )
	{
		Que_AddTail( &MACTxQueue, sbfr );
		return FALSE;
	}
	else if( !XmitPkt( sbfr ) )
	{
		Que_AddTail( &MACTxQueue, sbfr );
		return FALSE;
	}
	else
	{
		if( sbfr->TxAutoDeAlloc )
			DeAllocateBuffer( sbfr );

		return TRUE;
	}
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
static BOOL XmitPkt( NET_PKT* sbfr )
{
	WORD_VAL dataLen;

	// Make sure the transmitter is ready
	if( ReadETHReg(ECON1).ECON1bits.TXRTS )
	{
		//transmitter busy
		return FALSE;
	}

	BFCReg( EIR, EIR_TXIF ); //Clear Tx Interrupt Flag

	// Set the SPI write pointer to the beginning of the transmit buffer
	BankSel(EWRPTL);
	WriteReg(EWRPTL, LOW(TXSTART));
	WriteReg(EWRPTH, HIGH(TXSTART));

	// Set the TXND pointer to correspond to the packet size given
    // much sure that we place one byte for the ethernet control byte
    dataLen.Val = (sbfr->PktLen + TXSTART + 1);
	WriteReg(ETXNDL, dataLen.v[0]);
	WriteReg(ETXNDH, dataLen.v[1]);

	// Set the per-packet control byte and write the Ethernet destination 
	// address
	MACPut(0x00);	// Use default control configuration

	//write packet to ethernet controller memory
    MACPutArray( (BYTE *)sbfr->pPkt, sbfr->PktLen );

	// Silicon Errata Work Around B rev
	// reset everything for the work around
    _txCollsionTick     = TickGet();
    _txCollisionEnable  = TRUE;
    _txCollisionRetry   = 0;
	// End Silicon Errata Work Around B rev 

    //Enable transmission of this packet
	BFSReg(ECON1, ECON1_TXRTS);
		
	return TRUE;
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
BOOL MACIsLinked(void)
{
	// LLSTAT is a latching low link status bit.  Therefore, if the link 
	// goes down and comes back up before a higher level stack program calls
	// MACIsLinked(), MACIsLinked() will still return FALSE.  The next 
	// call to MACIsLinked() will return TRUE (unless the link goes down 
	// again).
	return ReadPHYReg(PHSTAT1).PHSTAT1bits.LLSTAT;
}


/////////////////////////////////////////////////
/////////////////////////////////////////////////
BOOL MACIsTxReady(void)
{
    return !ReadETHReg(ECON1).ECON1bits.TXRTS;
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
WORD MACGetFreeRxSize(void)
{
	WORD_VAL ReadPT, WritePT;

	// Read the Ethernet hardware buffer write pointer.  Because packets can be 
	// received at any time, it can change between reading the low and high 
	// bytes.  A loop is necessary to make certain a proper low/high byte pair
	// is read.
	BankSel(EPKTCNT);
	do {
		// Save EPKTCNT in a temporary location
		ReadPT.v[0] = ReadETHReg(EPKTCNT).Val;
	
		BankSel(ERXRDPTL);
		WritePT.v[0] = ReadETHReg(ERXWRPTL).Val;
		WritePT.v[1] = ReadETHReg(ERXWRPTH).Val;
	
		BankSel(EPKTCNT);
	} while(ReadETHReg(EPKTCNT).Val != ReadPT.v[0]);
	
	// Determine where the write protection pointer is
	BankSel(ERXRDPTL);
	ReadPT.v[0] = ReadETHReg(ERXRDPTL).Val;
	ReadPT.v[1] = ReadETHReg(ERXRDPTH).Val;
	
	// Calculate the difference between the pointers, taking care to account 
	// for buffer wrapping conditions
	if ( WritePT.Val > ReadPT.Val )
		return (RXSTOP - RXSTART) - (WritePT.Val - ReadPT.Val);
	else if ( WritePT.Val == ReadPT.Val )
		return RXSIZE - 1;
	else
		return ReadPT.Val - WritePT.Val - 1;
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
#if defined(MCHP_MAC)
// MACCalcRxChecksum() and MACCalcTxChecksum() use the DMA module to calculate
// checksums.  These two functions have been tested.
WORD MACCalcRxChecksum(WORD offset, WORD len)
{
	WORD_VAL temp;

	BankSel(ERXRDPTL);
	temp.v[0] = ReadETHReg(ERXRDPTL).Val;
	temp.v[1] = ReadETHReg(ERXRDPTH).Val;

	temp.Val += offset;
	temp.Val += sizeof(MCP_PREAMBLE);
	if ( temp.Val > RXSTOP )		// Adjust value if a wrap is needed
		temp.Val -= RXSIZE;

	WriteReg(EDMASTL, temp.v[0]);
	WriteReg(EDMASTH, temp.v[1]);

	temp.Val += len-1;
	if ( temp.Val > RXSTOP )		// Adjust value if a wrap is needed
		temp.Val -= RXSIZE;

	WriteReg(EDMANDL, temp.v[0]);
	WriteReg(EDMANDH, temp.v[1]);
	
	BFSReg(ECON1, ECON1_DMAST | ECON1_CSUMEN);
	while(ReadETHReg(ECON1).ECON1bits.DMAST);

	// Swap endianness and return
	temp.v[1] = ReadETHReg(EDMACSL).Val;
	temp.v[0] = ReadETHReg(EDMACSH).Val;
	return temp.Val;
}


/////////////////////////////////////////////////
/////////////////////////////////////////////////
WORD MACCalcTxChecksum(WORD offset, WORD len)
{
	WORD_VAL temp;

	BankSel(EDMASTL);

	temp.Val = TXSTART + ETHER_HDR_LEN	//sizeof(ETHER_HEADER)
				+ offset + 1;	// +1 needed to account for per packet control byte
	WriteReg(EDMASTL, temp.v[0]);
	WriteReg(EDMASTH, temp.v[1]);
	
	temp.Val += len-1;
	WriteReg(EDMANDL, temp.v[0]);
	WriteReg(EDMANDH, temp.v[1]);
	
	BFSReg(ECON1, ECON1_DMAST | ECON1_CSUMEN);
	while(ReadETHReg(ECON1).ECON1bits.DMAST);

	temp.v[1] = ReadETHReg(EDMACSL).Val;	// Byte order swapped
	temp.v[0] = ReadETHReg(EDMACSH).Val;	// Byte order swapped
	return temp.Val;
}


/******************************************************************************
 * Function:        WORD CalcIPBufferChecksum(WORD len)
 *
 * PreCondition:    TCPInit() is already called     AND
 *                  MAC buffer pointer set to starting of buffer
 *
 * Input:           len     - Total number of bytes to calculate
 *                          checksum for.
 *
 * Output:          16-bit checksum as defined by rfc 793.
 *
 * Side Effects:    None
 *
 * Overview:        This function performs a checksum calculation in the MAC
 *                  buffer itself.  The ENC28J60 has a hardware DMA module 
 *					which can calculate the checksum faster than software, so 
 *					this function replaces the CaclIPBufferChecksum() function 
 *					defined in the helpers.c file.  Through the use of 
 *					preprocessor defines, this replacement is automatic.
 *
 * Note:            
 *****************************************************************************/
WORD CalcIPBufferChecksum(WORD len)
{
	WORD_VAL temp;

	// Take care of special cases which the DMA cannot be used for
	if(len == 0u)
		return 0xFFFF;
	else if(len == 1u)
		return ~(((WORD)ReadRAMbyte())<<8);
		

	// Set the DMA starting address to the SPI read pointer value
	BankSel(ERDPTL);
	temp.v[0] = ReadETHReg(ERDPTL).Val;
	temp.v[1] = ReadETHReg(ERDPTH).Val;
	WriteReg(EDMASTL, temp.v[0]);
	WriteReg(EDMASTH, temp.v[1]);
	
	// Calculate the DMA ending address given the starting address and len 
	// parameter.  The DMA will follow the receive buffer wrapping boundary.
	temp.Val += len-1;
	if(temp.Val > RXSTOP)
		temp.Val -= RXSIZE;
	
	// Write the DMA end address
	WriteReg(EDMANDL, temp.v[0]);
	WriteReg(EDMANDH, temp.v[1]);
	
	// Begin the DMA checksum calculation and wait until it is finished
	BFSReg(ECON1, ECON1_DMAST | ECON1_CSUMEN);
	while(ReadETHReg(ECON1).ECON1bits.DMAST);

	// Return the resulting good stuff
	temp.v[0] = ReadETHReg(EDMACSL).Val;
	temp.v[1] = ReadETHReg(EDMACSH).Val;
	return temp.Val;
}
#endif	// End of MCHP_MAC specific code


/******************************************************************************
 * Function:        void MACCopyRxToTx(WORD RxOffset, WORD TxOffset, WORD len)
 *
 * PreCondition:    None
 *
 * Input:           RxOffset: Offset in the RX buffer (0=first byte of 
 * 							  destination MAC address) to copy from.
 *					TxOffset: Offset in the TX buffer (0=first byte of
 *							  destination MAC address) to copy to.
 *					len:	  Number of bytes to copy
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        If the TX logic is transmitting a packet (ECON1.TXRTS is 
 *					set), the hardware will wait until it is finished.  Then, 
 *					the DMA module will copy the data from the receive buffer 
 *					to the transmit buffer.
 *
 * Note:            None
 *****************************************************************************/
// TODO: Remove this line if your application needs to use this 
// function.  This code has not been tested.
#if 0 
void MACCopyRxToTx(WORD RxOffset, WORD TxOffset, WORD len)
{
	WORD_VAL temp;

	BankSel(ERXRDPTL);
	temp.v[0] = ReadETHReg(ERXRDPTL).Val;
	temp.v[1] = ReadETHReg(ERXRDPTH).Val;

	temp.Val += RxOffset;
	temp.Val += sizeof(MCP_PREAMBLE);
	if ( temp.Val > RXSTOP )		// Adjust value if a wrap is needed
		temp.Val -= RXSIZE;

	WriteReg(EDMASTL, temp.v[0]);
	WriteReg(EDMASTH, temp.v[1]);

	temp.Val += len-1;
	if ( temp.Val > RXSTOP )		// Adjust value if a wrap is needed
		temp.Val -= RXSIZE;

	WriteReg(EDMANDL, temp.v[0]);
	WriteReg(EDMANDH, temp.v[1]);
	
	TxOffset += TXSTART+1;
	WriteReg(EDMADSTL, ((WORD_VAL*)&TxOffset)->v[0]);
	WriteReg(EDMADSTH, ((WORD_VAL*)&TxOffset)->v[1]);
	
	// Do the DMA Copy.  The DMA module will wait for TXRTS to become clear 
	// before starting the copy.
	BFCReg(ECON1, ECON1_CSUMEN);
	BFSReg(ECON1, ECON1_DMAST);
	while(ReadETHReg(ECON1).ECON1bits.DMAST);
}
#endif


#if defined(MAC_FILTER_BROADCASTS)
// NOTE: This code has not been tested.  See StackTsk.h's explanation
// of MAC_FILTER_BROADCASTS.
/******************************************************************************
 * Function:        void MACSetPMFilter(BYTE *Pattern, 
 *										BYTE *PatternMask, 
 *										WORD PatternOffset)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					MACIsTxReady() must return TRUE
 *
 * Input:           *Pattern: Pointer to an intial pattern to compare against
 *					*PatternMask: Pointer to an 8 byte pattern mask which 
 *								  defines which bytes of the pattern are 
 *								  important.  At least one bit must be set.
 *					PatternOffset: Offset from the beginning of the Ethernet 
 *								   frame (1st byte of destination address), to
 *								   begin comparing with the given pattern.
 *
 * Output:          None
 *
 * Side Effects:    Contents of the TX buffer space are overwritten
 *
 * Overview:        MACSetPMFilter sets the hardware receive filters for: 
 *					CRC AND (Unicast OR Pattern Match).  As a result, only a 
 *					subset of the broadcast packets which are normally 
 *					received will be received.
 *
 * Note:            None
 *****************************************************************************/
void MACSetPMFilter(BYTE *Pattern, 
					BYTE *PatternMask, 
					WORD PatternOffset)
{
	WORD_VAL i;
	BYTE *MaskPtr;
	BYTE UnmaskedPatternLen;
	
	// Set the SPI write pointer and DMA startting address to the beginning of 
	// the transmit buffer
	BankSel(EWRPTL);
	WriteReg(EWRPTL, LOW(TXSTART));
	WriteReg(EWRPTH, HIGH(TXSTART));
	WriteReg(EDMASTL, LOW(TXSTART));
	WriteReg(EDMASTH, HIGH(TXSTART));

	// Fill the transmit buffer with the pattern to match against.  Only the 
	// bytes which have a mask bit of 1 are written into the buffer and will
	// subsequently be used for checksum computation.  
	MaskPtr = PatternMask;
	for(i.Val = 0x0100; i.v[0] < 64; i.v[0]++)
	{
		if( *MaskPtr & i.v[1] )
		{
			MACPut(*Pattern);
			UnmaskedPatternLen++;
		}
		Pattern++;
		
		i.v[1] <<= 1;
		if( i.v[1] == 0u )
		{
			i.v[1] = 0x01;
			MaskPtr++;
		}
	}

	// Calculate and set the DMA end address
	i.Val = TXSTART + (WORD)UnmaskedPatternLen - 1;
	WriteReg(EDMANDL, i.v[0]);
	WriteReg(EDMANDH, i.v[1]);

	// Calculate the checksum on the given pattern using the DMA module
	BFSReg(ECON1, ECON1_DMAST | ECON1_CSUMEN);
	while(ReadETHReg(ECON1).ECON1bits.DMAST);

	// Make certain that the PM filter isn't enabled while it is 
	// being reconfigured.
	BankSel(ERXFCON);
	WriteReg(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);

	// Get the calculated DMA checksum and store it in the PM 
	// checksum registers
	i.v[0] == ReadETHReg(EDMACSL).Val;
	i.v[1] == ReadETHReg(EDMACSH).Val;
	WriteReg(EPMCSL, i.v[0]);
	WriteReg(EPMCSH, i.v[0]);

	// Set the Pattern Match offset and 8 byte mask
	WriteReg(EPMOL, ((WORD_VAL*)&PatternOffset)->v[0]);
	WriteReg(EPMOH, ((WORD_VAL*)&PatternOffset)->v[1]);
	for(i.Val = EPMM0; i.Val <= EPMM7 ; i.Val++)
	{
		WriteReg(i.Val, *PatternMask++);
	}

	// Begin using the new Pattern Match filter instead of the 
	// broadcast filter
	WriteReg(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN);
}//end MACSetPMFilter


/******************************************************************************
 * Function:        void MACDisablePMFilter(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACDisablePMFilter disables the Pattern Match receive 
 *					filter (if enabled) and returns to the default filter 
 *					configuration of: CRC AND (Unicast OR Broadcast).
 *
 * Note:            None
 *****************************************************************************/
void MACDisablePMFilter(void)
{
	BankSel(ERXFCON);
	WriteReg(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);
	return;
}//end MACDisablePMFilter
#endif // end of MAC_FILTER_BROADCASTS specific code

//////////////////////////////////////////////
//////////////////////////////////////////////
BYTE ByteTransfer( BYTE SendByte )
{
    unsigned int rx_data;

    putcSPI((unsigned int) SendByte);

    rx_data = getcSPI();
        
    return((BYTE)(rx_data & 0xFF));
}

/******************************************************************************
 * Function:        BYTE ReadRAMbyte()
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					ERDPT must point to the place to read from.
 *
 * Input:           None
 *
 * Output:          Byte read from the ENC28J60's RAM
 *
 * Side Effects:    None
 *
 * Overview:        ReadRAMbyte returns the byte pointed to by ERDPT and 
 *					increments ERDPT so ReadRAMbyte() can be called again.  The 
 *					increment will follow the receive buffer wrapping boundary.
 *
 * Note:            None
 *****************************************************************************/
static BYTE ReadRAMbyte()
{
BYTE temp;

	SPISelectEthernet();

	ByteTransfer( RBM ); 	  // Send the Read Buffer Memory opcode.
	temp = ByteTransfer( 0 ); // Send a dummy byte to receive the register contents.

	SPIUnselectEthernet();

	return temp;
}//end ReadRAMbyte


/******************************************************************************
 * Function:        WORD MACGetArray(BYTE *val, WORD len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					ERDPT must point to the place to read from.
 *
 * Input:           *val: Pointer to storage location
 *					len:  Number of bytes to read from the data buffer.
 *
 * Output:          Byte(s) of data read from the data buffer.
 *
 * Side Effects:    None
 *
 * Overview:        Burst reads several sequential bytes from the data buffer 
 *					and places them into local memory.  With hardware support, 
 *					it performs much faster than multiple ReadRAMbyte() calls.
 *					ERDPT is incremented after each byte, following the same 
 *					rules as ReadRAMbyte().
 *
 * Note:            None
 *****************************************************************************/
WORD MACGetArray(BYTE *val, WORD len)
{
	// Start the burst operation
	SPISelectEthernet();

	ByteTransfer( RBM ); 	  // Send the Read Buffer Memory opcode.

	// Read the data
	while(len)
	{
		*val = ByteTransfer( 0 ); // Send a dummy byte to receive the register contents.;
		len--;
		val++;
	};

	// Terminate the burst operation
	SPIUnselectEthernet();

	return 0;
}//end MACGetArray


/******************************************************************************
 * Function:        void MACPut(BYTE val)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					EWRPT must point to the location to begin writing.
 *
 * Input:           Byte to write into the ENC28J60 buffer memory
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPut outputs the Write Buffer Memory opcode/constant 
 *					(8 bits) and data to write (8 bits) over the SPI.  
 *					EWRPT is incremented after the write.
 *
 * Note:            None
 *****************************************************************************/
void MACPut(BYTE val)
{
	SPISelectEthernet();

	ByteTransfer( WBM );	// Send the Write Buffer Memory opcode.
	ByteTransfer( val ); 	// Send the byte to be writen.

	SPIUnselectEthernet();

}//end MACPut


/******************************************************************************
 * Function:        void MACPutArray(BYTE *val, WORD len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					EWRPT must point to the location to begin writing.
 *
 * Input:           *val: Pointer to source of bytes to copy.
 *					len:  Number of bytes to write to the data buffer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPutArray writes several sequential bytes to the 
 *					ENC28J60 RAM.  It performs faster than multiple MACPut()
 *					calls.  EWRPT is incremented by len.
 *
 * Note:            None
 *****************************************************************************/
void MACPutArray(BYTE *val, WORD len)
{
	// Select the chip and send the proper opcode
	SPISelectEthernet();

	ByteTransfer( WBM ); 	  // Send the Write Buffer Memory opcode.

	// Send the data
	while(len)
	{
		ByteTransfer( *val ); // Send the byte to be writen.
		val++;				
		len--;				
	};

	// Terminate the burst operation
	SPIUnselectEthernet();
}//end MACPutArray


/******************************************************************************
 * Function:        SendSystemCommand
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        SendSystemCommand sends the Soft Reset System Command to 
 *					the Ethernet controller.  It resets all register contents.
 *
 * Note:            MAC, MII, and PHY registers should not be accessed for at
 *					least 50us after calling this function.
 *****************************************************************************/
static void SendSystemCommand(void)
{
	SPISelectEthernet();
	ByteTransfer( SC );
	SPIUnselectEthernet();
}//end SendSystemCommand


/******************************************************************************
 * Function:        REG ReadETHReg(BYTE Address)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					Bank select bits must be set corresponding to the register 
 * 					to read from.
 *
 * Input:           5 bit address of the ETH control register to read from.
 *					  The top 3 bits must be 0.
 *
 * Output:          Byte read from the Ethernet controller's ETH register.
 *
 * Side Effects:    None
 *
 * Overview:        ReadETHReg sends the 8 bit RCR opcode/Address byte over 
 *					the SPI and then retrives the register contents in the 
 *					next 8 SPI clocks.
 *
 * Note:            This routine cannot be used to access MAC/MII or PHY 
 *					registers.  Use ReadMACReg() or ReadPHYReg() for that 
 *					purpose.  
 *****************************************************************************/
static REG ReadETHReg(WORD Address)
{
REG temp;

	// Select the chip and send the Read Control Register opcode/address
	SPISelectEthernet();
	ByteTransfer( RCR | (Address&0xFF) );
	temp.Val = ByteTransfer( 0 );

	SPIUnselectEthernet();

	return temp;
}//end ReadETHReg


/******************************************************************************
 * Function:        REG ReadMACReg(BYTE Address)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					Bank select bits must be set corresponding to the register 
 * 					to read from.
 *
 * Input:           5 bit address of the MAC or MII register to read from.
 *					  The top 3 bits must be 0.
 *
 * Output:          Byte read from the Ethernet controller's MAC/MII register.
 *
 * Side Effects:    None
 *
 * Overview:        ReadMACReg sends the 8 bit RCR opcode/Address byte as well 
 *					as a dummy byte over the SPI and then retrives the 
 *					register contents in the last 8 SPI clocks.
 *
 * Note:            This routine cannot be used to access ETH or PHY 
 *					registers.  Use ReadETHReg() or ReadPHYReg() for that 
 *					purpose.  
 *****************************************************************************/
static REG ReadMACReg(WORD Address)
{
REG temp;

	SPISelectEthernet();
	ByteTransfer( RCR | (Address&0xFF) );	// Send the Read Control Register opcode and 
									// address.
	ByteTransfer( 0 );				// Send a dummy byte 
	temp.Val = ByteTransfer( 0 );		// Send another dummy byte to receive the register

	SPIUnselectEthernet();
	return temp;

}//end ReadMACReg


/******************************************************************************
 * Function:        ReadPHYReg
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           Address of the PHY register to read from.
 *
 * Output:          16 bits of data read from the PHY register.
 *
 * Side Effects:    Alters bank bits to point to Bank 2
 *
 * Overview:        ReadPHYReg performs an MII read operation.  While in 
 *					progress, it simply polls the MII BUSY bit wasting time.
 *
 * Note:            None
 *****************************************************************************/
PHYREG ReadPHYReg(BYTE Register)
{
	PHYREG Result;

	// Set the right address and start the register read operation
	BankSel(MIREGADR);
	WriteReg(MIREGADR, Register);
	WriteReg(MICMD, MICMD_MIIRD);	

	// Loop to wait until the PHY register has been read through the MII
	BankSel(MISTAT);
	while(ReadMACReg(MISTAT).MISTATbits.BUSY);

	// Stop reading
	BankSel(MIREGADR);
	WriteReg(MICMD, 0x00);	
	
	// Obtain results and return
	Result.VAL.v[0] = ReadMACReg(MIRDL).Val;
	Result.VAL.v[1] = ReadMACReg(MIRDH).Val;
	return Result;
}//end ReadPHYReg


/******************************************************************************
 * Function:        void WriteReg(BYTE Address, BYTE Data)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					Bank select bits must be set corresponding to the register 
 *					to modify.
 *
 * Input:           5 bit address of the ETH, MAC, or MII register to modify.  
 *					  The top 3 bits must be 0.  
 *					Byte to be written into the register.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        WriteReg sends the 8 bit WCR opcode/Address byte over the 
 *					SPI and then sends the data to write in the next 8 SPI 
 *					clocks.
 *
 * Note:            This routine is almost identical to the BFCReg() and 
 *					BFSReg() functions.  It is seperate to maximize speed.  
 *					Unlike the ReadETHReg/ReadMACReg functions, WriteReg() 
 *					can write to any ETH or MAC register.  Writing to PHY 
 *					registers must be accomplished with WritePHYReg().
 *****************************************************************************/
static void WriteReg(WORD Address, BYTE Data)
{
	SPISelectEthernet();
	ByteTransfer( WCR | (Address&0xFF) );	// Send the Write Control Register opcode and 
									// address.
	ByteTransfer( Data );			// Send the data byte 
	SPIUnselectEthernet();
}//end WriteReg


/******************************************************************************
 * Function:        void BFCReg(BYTE Address, BYTE Data)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					Bank select bits must be set corresponding to the register 
 *					  to modify.
 *
 * Input:           5 bit address of the register to modify.  The top 3 bits 
 *					  must be 0.  
 *					Byte to be used with the Bit Field Clear operation.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BFCReg sends the 8 bit BFC opcode/Address byte over the 
 *					SPI and then sends the data in the next 8 SPI clocks.
 *
 * Note:            This routine is almost identical to the WriteReg() and 
 *					BFSReg() functions.  It is separate to maximize speed.  
 *					BFCReg() must only be used on ETH registers.
 *****************************************************************************/
static void BFCReg(WORD Address, BYTE Data)
{
	SPISelectEthernet();
	ByteTransfer( BFC | (Address&0xFF) );	// Send the opcode and address.
	ByteTransfer( Data );			// Send the data byte 
	SPIUnselectEthernet();
}//end BFCReg


/******************************************************************************
 * Function:        void BFSReg(BYTE Address, BYTE Data)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					Bank select bits must be set corresponding to the register 
 *					to modify.
 *
 * Input:           5 bit address of the register to modify.  The top 3 bits 
 *					  must be 0.  
 *					Byte to be used with the Bit Field Set operation.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BFSReg sends the 8 bit BFC opcode/Address byte over the 
 *					SPI and then sends the data in the next 8 SPI clocks.
 *
 * Note:            This routine is almost identical to the WriteReg() and 
 *					BFCReg() functions.  It is separate to maximize speed.
 *					BFSReg() must only be used on ETH registers.
 *****************************************************************************/
static void BFSReg(WORD Address, BYTE Data)
{
	SPISelectEthernet();
	ByteTransfer( BFS | (Address&0xFF) );	// Send the opcode and address.
	ByteTransfer( Data );			// Send the data byte 
	SPIUnselectEthernet();
}//end BFSReg


/******************************************************************************
 * Function:        WritePHYReg
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           Address of the PHY register to write to.
 *					16 bits of data to write to PHY register.
 *
 * Output:          None
 *
 * Side Effects:    Alters bank bits to point to Bank 3
 *
 * Overview:        WritePHYReg performs an MII write operation.  While in 
 *					progress, it simply polls the MII BUSY bit wasting time.
 *
 * Note:            None
 *****************************************************************************/
void WritePHYReg(BYTE Register, WORD Data)
{
	// Write the register address
	BankSel(MIREGADR);
	WriteReg(MIREGADR, Register);
	
	// Write the data
	// Order is important: write low byte first, high byte last
	WriteReg(MIWRL, ((WORD_VAL*)&Data)->v[0]);	
	WriteReg(MIWRH, ((WORD_VAL*)&Data)->v[1]);

	// Wait until the PHY register has been written
	BankSel(MISTAT);
	while(ReadMACReg(MISTAT).MISTATbits.BUSY);
}//end WritePHYReg


/******************************************************************************
 * Function:        BankSel
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           Register address with the high byte containing the 2 bank 
 *					  select 2 bits.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BankSel takes the high byte of a register address and 
 *					changes the bank select bits in ETHCON1 to match.
 *
 * Note:            None
 *****************************************************************************/
static void BankSel(WORD Register) 
{
	BFCReg(ECON1, ECON1_BSEL1 | ECON1_BSEL0);
	BFSReg(ECON1, ((WORD_VAL*)&Register)->v[1]);
}//end BankSel


/******************************************************************************
 * Function:        static BOOL TestMemory(void) 
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          TRUE if the memory tests have passed 
 *					FALSE if the BIST has detected a hardware fault
 *
 * Side Effects:    Alters the state of numerous control registers and all 
 *					RAM bytes.
 *
 * Overview:        The internal BIST and DMA modules are used to fill the 
 *					entire dual port memory and calculate a checksum of the 
 *					data stored within.  Address and Random fill modes are 
 *					used.
 *
 * Note:            For the Random Fill mode, the random number generator is
 *					seeded by the contents of the TMR0L PIC SFR.  If the timer
 *					is running, additional confidence that the memory is 
 *					working can be obtained by calling TestMemory multiple 
 *					times.
 *****************************************************************************/
#if defined(MAC_POWER_ON_TEST)
static BOOL TestMemory(void) 
{
	#define RANDOM_FILL		0b0000
	#define ADDRESS_FILL	0b0100
	#define PATTERN_SHIFT	0b1000
	#define RANDOM_RACE		0b1100
	
	WORD_VAL DMAChecksum, BISTChecksum;
	
	
	// Select Bank 0 and disable anything that could have been in progress
	WriteReg(ECON1, 0x00);
	
	// Set up necessary pointers for the DMA to calculate over the entire 
	// memory
	WriteReg(EDMASTL, 0x00);
	WriteReg(EDMASTH, 0x00);
	WriteReg(EDMANDL, LOW(RAMSIZE-1u));
	WriteReg(EDMANDH, HIGH(RAMSIZE-1u));
	WriteReg(ERXNDL, LOW(RAMSIZE-1u));
	WriteReg(ERXNDH, HIGH(RAMSIZE-1u));

	// Enable Test Mode and do an Address Fill
	BankSel(EBSTCON);
	WriteReg(EBSTCON, EBSTCON_TME | 
						 EBSTCON_BISTST | 
						 ADDRESS_FILL);
	
	
	// TODO: Remove these two lines
	// Wait for the BIST to complete and disable test mode before 
	// starting any DMA operations.  These two lines are required on revision 
	// A0 beta silicon.  They are unecessary and slighly undesireable on 
	// production silicon.
	while(ReadETHReg(EBSTCON).EBSTCONbits.BISTST);
	BFCReg(EBSTCON, EBSTCON_TME);


	// Begin reading the memory and calculating a checksum over it
	// Block until the checksum is generated
	BFSReg(ECON1, ECON1_DMAST | ECON1_CSUMEN);
	BankSel(EDMACSL);
	while(ReadETHReg(ECON1).ECON1bits.DMAST);

	// Obtain the resulting DMA checksum and the expected BIST checksum
	DMAChecksum.v[0] = ReadETHReg(EDMACSL).Val;
	DMAChecksum.v[1] = ReadETHReg(EDMACSH).Val;
	BankSel(EBSTCSL);
	BISTChecksum.v[0] = ReadETHReg(EBSTCSL).Val;
	BISTChecksum.v[1] = ReadETHReg(EBSTCSH).Val;
	BFCReg(EBSTCON, EBSTCON_TME);
	
	// Compare the results
	// 0xF807 should always be generated in Address fill mode
	if( (DMAChecksum.Val != BISTChecksum.Val) || (DMAChecksum.Val != 0xF807) )
		return FALSE;
	
	// Seed the random number generator and begin another Random Fill test 
	// with the DMA and BIST memory access ports swapped.
	WriteReg(EBSTSD, TMR0L);
	WriteReg(EBSTCON, EBSTCON_TME | 
					  EBSTCON_PSEL | 
					  EBSTCON_BISTST | 
					  RANDOM_FILL);
						 
						 
	// TODO: Remove these two lines
	// Wait for the BIST to complete and disable test mode before 
	// starting any DMA operations.  These two lines are required on revision 
	// A0 beta silicon.  They are unecessary and slighly undesireable on 
	// production silicon.
	while(ReadETHReg(EBSTCON).EBSTCONbits.BISTST);
	BFCReg(EBSTCON, EBSTCON_TME);
	
	
	// Begin reading the memory and calculating a checksum over it
	// Block until the checksum is generated
	BFSReg(ECON1, ECON1_DMAST | ECON1_CSUMEN);
	BankSel(EDMACSL);
	while(ReadETHReg(ECON1).ECON1bits.DMAST);

	// Obtain the resulting DMA checksum and the expected BIST checksum
	DMAChecksum.v[0] = ReadETHReg(EDMACSL).Val;
	DMAChecksum.v[1] = ReadETHReg(EDMACSH).Val;
	BankSel(EBSTCSL);
	BISTChecksum.v[0] = ReadETHReg(EBSTCSL).Val;
	BISTChecksum.v[1] = ReadETHReg(EBSTCSH).Val;
	
	return (DMAChecksum.Val == BISTChecksum.Val);
}//end TestMemory
#endif


/******************************************************************************
 * Function:        void MACSetDuplex(DUPLEX DuplexState) 
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           Member of DUPLEX enum:
 *						FULL: Set full duplex mode
 *						HALF: Set half duplex mode
 *						USE_PHY: Set the MAC to match the PHYDPLXMODE bit in 
 *								 PHYCON.  This is controlled by LEDB on RESET.
 *
 * Output:          None
 *
 * Side Effects:    Changes bank bits to Bank 2.
 *
 * Overview:        Disables RX, TX logic, sets MAC up for full duplex 
 *					operation, sets PHY up for full duplex operation, and 
 *					reenables RX logic.  The back-to-back inter-packet gap 
 *					register (MACBBIPG) is updated to maintain a 9.6us gap.
 *
 * Note:            If a packet is being transmitted or received while this 
 *					function is called, it will be aborted.
 *****************************************************************************/
void MACSetDuplex(DUPLEX DuplexState)
{
	REG Register;
	PHYREG PhyReg;
	
	// Disable receive logic and abort any packets currently being transmitted
	BFCReg(ECON1, ECON1_TXRTS | ECON1_RXEN);
	
	// Set the PHY to the proper duplex mode
	PhyReg = ReadPHYReg(PHCON1);
	if(DuplexState == USE_PHY)
	{
		DuplexState = PhyReg.PHCON1bits.PDPXMD;
	}
	else
	{
		PhyReg.PHCON1bits.PDPXMD = DuplexState;
		WritePHYReg(PHCON1, PhyReg.Val);
	}

	// Set the MAC to the proper duplex mode
	BankSel(MACON3);
	Register = ReadMACReg(MACON3);
	Register.MACON3bits.FULDPX = DuplexState;
	WriteReg(MACON3, Register.Val);

	// Set the back-to-back inter-packet gap time to IEEE specified 
	// requirements.  The meaning of the MABBIPG value changes with the duplex
	// state, so it must be updated in this function.
	// In full duplex, 0x15 represents 9.6us; 0x12 is 9.6us in half duplex
	WriteReg(MABBIPG, DuplexState ? 0x15 : 0x12);	
	
	// Reenable receive logic
	BFSReg(ECON1, ECON1_RXEN);
}//end MACSetDuplex


/******************************************************************************
 * Function:        void MACPowerDown(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPowerDown puts the ENC28J60 in low power sleep mode. In
 *					sleep mode, no packets can be transmitted or received.  
 *					All MAC and PHY registers should not be accessed.
 *
 * Note:            If a packet is being transmitted while this function is 
 * 					called, this function will block until it is it complete.
 *					If anything is being received, it will be completed.
 *****************************************************************************/
void MACPowerDown(void)
{
	// Disable packet reception
	BFCReg(ECON1, ECON1_RXEN);

	// Make sure any last packet which was in-progress when RXEN was cleared 
	// is completed
	while(ReadETHReg(ESTAT).ESTATbits.RXBUSY);

	// If a packet is being transmitted, wait for it to finish
	while(ReadETHReg(ECON1).ECON1bits.TXRTS);
	
	// Enter sleep mode
	BFSReg(ECON2, ECON2_PWRSV);
}//end MACPowerDown


/******************************************************************************
 * Function:        void MACPowerUp(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPowerUp returns the ENC28J60 back to normal operation
 *					after a previous call to MACPowerDown().  Calling this 
 *					function when already powered up will have no effect.
 *
 * Note:            If a link partner is present, it will take 10s of 
 *					milliseconds before a new link will be established after
 *					waking up.  While not linked, packets which are 
 *					transmitted will most likely be lost.  MACIsLinked() can 
 *					be called to determine if a link is established.
 *****************************************************************************/
void MACPowerUp(void)
{	
	// Leave power down mode
	BFCReg(ECON2, ECON2_PWRSV);

	// Wait for the 300us Oscillator Startup Timer (OST) to time out.  This 
	// delay is required for the PHY module to return to an operational state.
	while(!ReadETHReg(ESTAT).ESTATbits.CLKRDY);
	
	
	// TODO: Remove.  On revision A0 silicon, setting the ECON1.RXEN bit causes 
	// the ERXWRPT registers to be reset, which will invalidate all the receive 
	// buffer contents.  This behavior is not how the data sheet claims it 
	// operates, and will be fixed on all future silicon revisions.
	BankSel(EREVID);
	if(ReadETHReg(EREVID).Val == 0)
	{
		while(ReadETHReg(EIR).EIRbits.PKTIF)
			BFSReg(ECON2, ECON2_PKTDEC);
	}
	
	
	// Enable packet reception
	BFSReg(ECON1, ECON1_RXEN);
}//end MACPowerUp


/******************************************************************************
 * Function:        void SetCLKOUT(BYTE NewConfig)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           NewConfig - 0x00: CLKOUT disabled (pin driven low)
 *								0x01: Divide by 1 (25 MHz)
 *								0x02: Divide by 2 (12.5 MHz)
 *								0x03: Divide by 3 (8.333333 MHz)
 *								0x04: Divide by 4 (6.25 MHz, POR default)
 *								0x05: Divide by 8 (3.125 MHz)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Writes the value of NewConfig into the ECOCON register.  
 *					The CLKOUT pin will beginning outputting the new frequency
 *					immediately.
 *
 * Note:            
 *****************************************************************************/
void SetCLKOUT(BYTE NewConfig)
{	
	BankSel(ECOCON);
	WriteReg(ECOCON, NewConfig);
}//end SetCLKOUT


/******************************************************************************
 * Function:        BYTE GetCLKOUT(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          BYTE - 0x00: CLKOUT disabled (pin driven low)
 *						   0x01: Divide by 1 (25 MHz)
 *						   0x02: Divide by 2 (12.5 MHz)
 *						   0x03: Divide by 3 (8.333333 MHz)
 *						   0x04: Divide by 4 (6.25 MHz, POR default)
 *						   0x05: Divide by 8 (3.125 MHz)
 *						   0x06: Reserved
 *						   0x07: Reserved
 *
 * Side Effects:    None
 *
 * Overview:        Returns the current value of the ECOCON register.
 *
 * Note:            None
 *****************************************************************************/
BYTE GetCLKOUT(void)
{	
	BankSel(ECOCON);
	return ReadETHReg(ECOCON).Val;
}//end GetCLKOUT
