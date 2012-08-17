/*********************************************************************
 *
 *                  CAN MCP2515 Library file
 *
 *********************************************************************
 * FileName:        canlib2515.c
 * Dependencies:	
 *
 * Processor:       PIC32
 *
 * Complier:        MPLAB C32 1.0+
 *                  MPLAB IDE 8.0+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC32/PIC24 Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC32/PIC24 Microcontroller products.
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
 *
 * $Id: canlib2515.c 275 2008-12-28 22:29:23Z akhe $
 *
 ********************************************************************/
#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include <inttypes.h>                       // Include path set to .../firmware/common/

#include <plib.h>
#include "canlib2515.h"
#include "REGS2515.h"

#include "FreeRTOS.h"   // FreeRTOS
#include "task.h"   

static uint8_t ReadStatus2515(int Channel);
static uint8_t CAN2515ByteRead(int Channel, unsigned char addr);
static void CAN2515ByteWrite(int Channel, unsigned char addr, unsigned char value );
static uint8_t XferSPI( int Channel, uint8_t dat );


/*********************************************************************
 * Function:        void CAN2515InitSPI(int Channel, UINT32 pClk, UINT32 SPIClk )
 *
 * PreCondition:    None
 *
 * Input:			Channel: SPI channel number, 1 based
 *					pClk: Peripheral clock speed in hertz
 *					SPIClk: Desired SPI clock speed for MCP2515
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		This function configures the spi port for the MCP2515 CAN chip
 *
 * Note:            None.
 ********************************************************************/
void CAN2515InitSPI(int Channel, uint32_t pClk, uint32_t SPIClk )
{
	uint32_t dvdr;
	
	//calculate SPI clocl divider
	dvdr = pClk/SPIClk;
	
	if( pClk % SPIClk )
		dvdr++;
		
	SpiChnOpen( Channel, 
	                SPI_CON_MSTEN | 
	                        SPI_CON_MODE8 |
						    SPI_CON_CKE |	
						    SPI_CON_ON, 
					dvdr );
}

/*********************************************************************
 * Function:        void CANReset(int Channel)
 *
 * PreCondition:    SPI port configured
 *
 * Input:			Channel: SPI channel number, 1 based
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Send a software reset commmand over spi port to MCP2515 chip
 *
 * Note:            None.
 ********************************************************************/
void CANReset(int Channel)
{
	mSetCanCSOn( Channel );
	XferSPI( Channel, CAN_RESET );
	mSetCanCSOff( Channel );
}

/*********************************************************************
 * Function:        void CAN2515SetRXB0Filters(int Channel, 
 *								UINT16 Mask0, UINT16* pFlt0_1 )
 *
 * PreCondition:    SPI port configured
 *
 * Input:			Channel: SPI channel number, 1 based
 *					Mask0:	 define identifier 11 bits that are must match.
 *					pFlt0_1: Pointer to array of UINT16, 2 words, 
 *							 ie. Filter 0 and Filer 1.  Lower 11 bits are siginicant
 *							 1 = Rcvd Idenifier bit must match filter bit
 *							 0 = Don't care Rcvd identifier bit.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Initialize RXB0 mask and 2 filters. See MCP2515 datasheet for more information.
 *
 * Note:            None.
 ********************************************************************/ 
void CAN2515SetRXB0Filters(int Channel, uint16_t Mask0, uint16_t* pFlt0_1 )
{
	CAN2515ByteWrite(Channel, RXM0SIDH, Mask0 >> 3);
	CAN2515ByteWrite(Channel, RXM0SIDL, Mask0 << 5);

	// Set two filters associated with RXB0	
	
	CAN2515ByteWrite(Channel, RXF0SIDH, *pFlt0_1 >> 3);
	CAN2515ByteWrite(Channel, RXF0SIDL, *pFlt0_1 << 5);
	
	pFlt0_1++;
	CAN2515ByteWrite(Channel, RXF1SIDH, *pFlt0_1 >> 3);
	CAN2515ByteWrite(Channel, RXF1SIDL, *pFlt0_1 << 5);
}


/*********************************************************************
 * Function:        void CAN2515SetRXB0Filters(int Channel, 
 *								UINT16 Mask0, UINT16* pFlt0_1 )
 *
 * PreCondition:    SPI port configured
 *
 * Input:			Channel: SPI channel number, 1 based
 *					Mask0:	 define identifier 11 bits that are must match.
 *					pFlt2_5: Pointer to array of UINT16, 4 words, 
 *							 ie. Filter 2 thru Filer 5.  Lower 11 bits are siginicant.
 *							 1 = Rcvd Idenifier bit must match filter bit
 *							 0 = Don't care Rcvd identifier bit.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Initialize RXB1 mask and 4 filters. See MCP2515 datasheet for more information.
 *
 * Note:            None.
 ********************************************************************/ 
void CAN2515SetRXB1Filters(int Channel, uint16_t Mask1, uint16_t* pFlt2_5 )
{
	CAN2515ByteWrite(Channel, RXM1SIDH, Mask1 >> 3);
	CAN2515ByteWrite(Channel, RXM1SIDL, Mask1 << 5);

	// Set Four filters associated with RXB1	
	
	CAN2515ByteWrite(Channel, RXF2SIDH, *pFlt2_5 >> 3);
	CAN2515ByteWrite(Channel, RXF2SIDL, *pFlt2_5 << 5);
	
	pFlt2_5++;
	CAN2515ByteWrite(Channel, RXF3SIDH, *pFlt2_5 >> 3);
	CAN2515ByteWrite(Channel, RXF3SIDL, *pFlt2_5 << 5);
	
	pFlt2_5++;
	CAN2515ByteWrite(Channel, RXF4SIDH, *pFlt2_5 >> 3);
	CAN2515ByteWrite(Channel, RXF4SIDL, *pFlt2_5 << 5);

	pFlt2_5++;
	CAN2515ByteWrite(Channel, RXF5SIDH, *pFlt2_5 >> 3);
	CAN2515ByteWrite(Channel, RXF5SIDL, *pFlt2_5 << 5);
}



/*********************************************************************
 * Function:        void CANEnable(int Channel, int BusSpeed )
 *
 * PreCondition:    SPI port configured, CANReset is called, and RXB 
 *					Filters intitialized.
 *
 * Input:			Channel: SPI channel number, 1 based
 *					BusSpeed: Bus Speed code: CAN_500kbps
 *							CAN_250kbps, or CAN_125kbps
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Sets the CAN bus speed and turns on the CAN controller.
 *
 * Note:            None.
 ********************************************************************/
void CANEnable( int Channel, int BusSpeed )
{
	//Set CNF1
	CAN2515ByteWrite(Channel, CNF1,BusSpeed);
	
	//Set CNF2
	CAN2515ByteWrite(Channel, CNF2,0x80 | PHSEG1_3TQ | PRSEG_1TQ);
	
	//Set CNF3
	CAN2515ByteWrite(Channel, CNF3, PHSEG2_3TQ);
	
	//Interrupt on RXB0 - CANINTE
	CAN2515ByteWrite(Channel, CANINTE,0x01); // Interrupts are on
	
	//Set NORMAL mode
	CAN2515ByteWrite(Channel, CANCTRL,REQOP_NORMAL | CLKOUT_ENABLED);

	CAN2515ByteRead(Channel, CANSTAT); //dummy read to give 2515 time to switch to normal mode
	
	if( (CAN2515ByteRead(Channel, CANSTAT) & 0xE0) != OPMODE_NORMAL )
		CAN2515ByteWrite(Channel, CANCTRL,REQOP_NORMAL | CLKOUT_ENABLED);
}



/*********************************************************************
 * Function:        BOOL CANTxReady( int Channel )
 *
 * PreCondition:    CAN initialized
 *
 * Input:			Channel: SPI channel number, 1 based
 *
 * Output:          Return true if the CAN controller Transmit buffer is available
 *
 * Side Effects:    None
 *
 * Overview:		Application can check is Tx buffer is available before 
 *					calling CANSendMSg.
 *
 * Note:            None.
 ********************************************************************/
inline BOOL CANTxReady( int Channel )
{
	return (ReadStatus2515(Channel)&0x04) == 0;
}	


/*********************************************************************
 * Function:        BOOL CANRxReady( int Channel )
 *
 * PreCondition:    CAN initialized
 *
 * Input:			Channel: SPI channel number, 1 based
 *
 * Output:          Return true if the CAN controller Receive buffer is Full
 *
 * Side Effects:    None
 *
 * Overview:		Application can check is Rx buffer is full before 
 *					calling CANGetMSg.
 *
 * Note:            None.
 ********************************************************************/
inline BOOL CANRxReady( int Channel )
{
	return (ReadStatus2515(Channel)&0x3) != 0;
}	


/*********************************************************************
 * Function:        BOOL CANSendMsg( int Channel, UINT16 Identifier, 
 *											UINT8* Msg, UINT8 MsgSize )
 *
 * PreCondition:    CAN initialized
 *
 * Input:			Channel: SPI channel number, 1 based
 *					Identifier: 11bit data for identifier
 *					Msg: Data bytes, 8 bytes max
 *					MsgSize: number of data bytes
 *
 * Output:          Return true if the message was successfuly transferred
 *					to the CAN controller Tx buffer.
 *
 * Side Effects:    None
 *
 * Overview:		Application call this function to send a message to the CAN bus
 *
 * Note:            None.
 ********************************************************************/
BOOL CANSendMsg( int Channel, uint16_t Identifier, uint8_t* Msg, uint8_t MsgSize )
{
	int WaitCntr = 0;
	
	// wait for TXB0 to get ready.  If not ready within XMIT_TIMEOUT ms,then return false
	while( CAN2515ByteRead( Channel, TXB0CTRL ) & 0x08 ) //TXREQ == 1
	{
        vTaskDelay( 1 );

		if( WaitCntr++ >= XMIT_TIMEOUT )
			return FALSE;
	}

	CAN2515ByteWrite(Channel, TXB0SIDH, (Identifier >> 3) & 0xFF );    //Set TXB0 SIDH
	CAN2515ByteWrite(Channel, TXB0SIDL, (Identifier << 5) & 0xE0 );    //Set TXB0 SIDL
	
	if( MsgSize > 8 )
		MsgSize = 8;
	
	CAN2515ByteWrite(Channel, TXB0DLC, MsgSize);  //Set DLC
	
	int temp;
	for( temp = 0; temp < MsgSize; temp++ )
		CAN2515ByteWrite( Channel, TXB0D0+temp, Msg[temp] );
	
	CAN2515ByteWrite( Channel, TXB0CTRL, 0x08 ); //Start Transmission.
	
	return TRUE;
}	


/*********************************************************************
 * Function:        BOOL CANGetMsg( int Channel, UINT16 *pIdentifier, 
 *											UINT8* Msg, UINT8 *pMsgSize )
 *
 * PreCondition:    CAN initialized
 *
 * Input:			Channel: SPI channel number, 1 based
 *					Identifier: 11bit data for identifier
 *					Msg: Data bytes, 8 bytes max
 *					MsgSize: number of data bytes
 *
 * Output:          Return true if a message is received
 *
 * Side Effects:    None
 *
 * Overview:		Application call this function to read the CAN message
 *					received by the CAN controller
 *
 * Note:            None.
 ********************************************************************/
BOOL CANGetMsg( int Channel, uint16_t* pIdentifier, uint8_t* Msg, uint8_t* pMsgSize )
{
	int temp;
	uint8_t loc, S1, S2;

	temp = ReadStatus2515(Channel);
	
	if( (temp & 3) == 0 )	
		return FALSE;

	//enable CAN chip
	mSetCanCSOn(Channel);
				
	loc = ( temp & 1 )?0x90:0x94;	//select start location based on the buffer available (RXB 0 or 1)
	XferSPI( Channel, loc );
	
	S1 = XferSPI( Channel, 0 );	//RXBnSIDH
	S2 = XferSPI( Channel, 0 );	//RXBnSIDL
		
	*pIdentifier = S1<<3 | S2>>5; //format the 11 bit identifier
	
	XferSPI( Channel, 0 );	//EID8 - ignore
	XferSPI( Channel, 0 );	//EID0 - ignore
	
	*pMsgSize = XferSPI( Channel, 0 ) & 0x0F; //Data Length
	
	if(*pMsgSize>8)
		*pMsgSize = 8;
		
	for( temp = 0; temp < *pMsgSize; temp++ )
		Msg[temp] = XferSPI( Channel, 0 );
	
	//deselect CAN chip
	mSetCanCSOff(Channel);
		
	
	return TRUE;
	
}

//////////// Private library function ///////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
//Read Status from 2515
static uint8_t ReadStatus2515(int Channel)
{
	mSetCanCSOn(Channel);

	SpiChnPutC( Channel, CAN_RD_STATUS );

	while( !SpiChnDataRdy(Channel) ) //wait for data to xmit
		asm( "Nop;" );

	SpiChnGetC(Channel); //clear Rx Rdy flag

	SpiChnPutC( Channel, 0 );
	while( !SpiChnDataRdy(Channel) )
		asm( "Nop;" );

	uint8_t dat = SpiChnGetC(Channel);

	mSetCanCSOff(Channel);
	
	return dat;
}


///////////////////////////////////////////////
///////////////////////////////////////////////
static uint8_t XferSPI( int Channel, uint8_t dat )
{
	SpiChnPutC( Channel, dat );
	while( !SpiChnDataRdy(Channel) )
		asm( "Nop;" );

	return SpiChnGetC(Channel); 
}

///////////////////////////////////////////////
///////////////////////////////////////////////
static void CAN2515ByteWrite(int Channel, unsigned char addr, unsigned char value )
{
	mSetCanCSOn(Channel);  
	XferSPI(Channel, CAN_WRITE);
	XferSPI(Channel, addr);
	XferSPI(Channel, value);
	mSetCanCSOff(Channel);
}


///////////////////////////////////////////////
///////////////////////////////////////////////
static uint8_t CAN2515ByteRead(int Channel, unsigned char addr)
{

unsigned char tempdata;

	mSetCanCSOn(Channel);
	XferSPI(Channel, CAN_READ);
	XferSPI(Channel, addr);
	tempdata = XferSPI(Channel,0);
	mSetCanCSOff(Channel);

	return tempdata;
}

