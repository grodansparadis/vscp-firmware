/*********************************************************************
 *
 *                  CAN Library header for MCP2515
 *
 *********************************************************************
 * FileName:        canlib2515.h
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
 * $Id: $
 *
 ********************************************************************/
#include "GenericTypedefs.h"
#include "canlib2515.def"



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
void CAN2515InitSPI(int Channel, UINT32 pClk, UINT32 SPIClk );


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
void CANReset(int Channel);


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
void CAN2515SetRXB0Filters(int Channel, UINT16 Mask0, UINT16* pFlt0_1 );


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
void CAN2515SetRXB1Filters(int Channel, UINT16 Mask1, UINT16* pFlt2_5 );


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
void CANEnable( int Channel, int BusSpeed );


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
inline BOOL CANTxReady( int Channel );


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
inline BOOL CANRxReady( int Channel );


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
BOOL CANSendMsg( int Channel, UINT16 Identifier, UINT8* Msg, UINT8 MsgSize );


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
BOOL CANGetMsg( int Channel, UINT16* pIdentifier, UINT8* Msg, UINT8* pMsgSize );



