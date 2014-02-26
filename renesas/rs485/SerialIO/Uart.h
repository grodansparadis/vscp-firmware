/******************************************************************************
*
* ACS SOFTWARE
*
*------------------------------------------------------------------------------
* Copyright(c)2003-2014 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.
*
*	This Information is Proprietary to Ackerman Computer Sciences, Sarasota,
* Florida (ACS) and MAY NOT be copied by any method or incorporated into another
* program without the express written consent of ACS. This Information or any
* portion thereof remains the property of ACS. The Information contained herein
* is believed to be accurate and ACS assumeS no responsibility or liability for
* its use in any way and conveys no license or title under any patent or 
* copyright and makes no representation or warranty that this Information is
* free from patent or copyright infringement.
*------------------------------------------------------------------------------
* Uart.h - ACS R8CM12A UART Interface
*------------------------------------------------------------------------------
* written by:
*	Steven J. Ackerman
*	Consultant
*	ACS
*	6233 E. Sawgrass Rd.
*	Sarasota, FL 34240
*
* date		Comments
* --------------------------
* 10-23-03	First written.
*  3-30-11	Changes for use with R8CM12A instead of z8F.
*  6-07-13	Changes for Serial IO.
*******************************************************************************
*/

#ifndef	_UART_H_
#define	_UART_H_

/* UART support definitions */

/* uncomment one or both of the following */
#define	UART_IMPLEMENT_TX
#define	UART_IMPLEMENT_RX
#define	RS485_MODE_ENABLED

#define	UART_BAUD_RATE		38400UL

/* hardware flow control */
#define	RTS_CTS_INIT()			RTS = 1; pd4_7 = 1
#define	RTS_OFF()				RTS = 1
#define	RTS_ON()				RTS = 0
#define	RTS_IS_ON()				(RTS == 0)

#define	CTS_IS_ON()				(CTS == 0)
#define	CTS_IS_OFF()			(CTS == 1)

/* RS485 transceiver driver control */
#define	RS485_DRIVER_INIT()		RTS = 0; pd4_7 = 1
#define	RS485_DRIVER_ENABLE()	RTS = 1; asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP")
#define	RS485_DRIVER_DISABLE()	RTS = 0; asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP")
#define	RS485_DRIVER_IS_ENABLED()	(RTS == 1)

void UART_Init(BOOLEAN enableTransmitter, BOOLEAN enableReceiver);

#ifdef	UART_IMPLEMENT_RX

#define	MAX_UART_RX_BUFFER	32	/* must be a power of 2 */

BOOLEAN	UART_IsReceivedDataAvailable();
BYTE UART_ReceiveByte();

#endif

#ifdef	UART_IMPLEMENT_TX

#define	MAX_UART_TX_BUFFER	64	/* must be a power of 2 */

BOOLEAN UART_IsSendDataFull();
BOOLEAN UART_IsSendDataEmpty();
BOOLEAN UART_SendQueueRoomFor(BYTE count);
void UART_SendByte(BYTE data);
BOOLEAN UART_QueueSendByte(BYTE data);
void UART_SendQueue(void);
BOOLEAN UART_IsSendDataCompleted();

#endif

#endif	/* _UART_H_ */
