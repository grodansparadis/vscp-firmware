/*
	FreeRTOS V4.0.0 - Copyright (C) 2003-2006 Richard Barry.

	This file is part of the FreeRTOS distribution.

	FreeRTOS is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS, without being obliged to provide
	the source code for any proprietary components.  See the licensing section 
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

	***************************************************************************
	See http://www.FreeRTOS.org for documentation, latest information, license 
	and contact details.  Please ensure to read the configuration and relevant 
	port sections of the online documentation.
	***************************************************************************
*/

// Changes for the VSCP project: 2006-04-20, Ake Hedman <akhe@eurosource.se>

#include <stdlib.h>
#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "freertos_serial.h"

#define serBAUD_DIV_CONSTANT			( ( unsigned portLONG ) 16 )

// Constants for writing to UCSRB. 
#define serRX_INT_ENABLE				( ( unsigned portCHAR ) 0x80 )
#define serRX_ENABLE					( ( unsigned portCHAR ) 0x10 )
#define serTX_ENABLE					( ( unsigned portCHAR ) 0x08 )
#define serTX_INT_ENABLE				( ( unsigned portCHAR ) 0x20 )

// Constants for writing to UCSRC. 
#define serUCSRC_SELECT					( ( unsigned portCHAR ) 0x80 )
#define serEIGHT_DATA_BITS				( ( unsigned portCHAR ) 0x06 )

static xQueueHandle rxQueue; 
static xQueueHandle txQueue; 

///////////////////////////////////////////////////////////////////////////////
// MACRO - serialInterruptOn
//

#define serialInterruptOn()									\
{															\
	unsigned portCHAR ucByte;								\
															\
	ucByte = UCSRB;											\
	ucByte |= serTX_INT_ENABLE;								\
	UCSRB = ucByte;											\
}	



///////////////////////////////////////////////////////////////////////////////
// MACRO - serialInterruptOff
//

#define serialInterruptOff()								\
{															\
	unsigned portCHAR ucInByte;							\
															\
	ucInByte = UCSRB;										\
	ucInByte &= ~serTX_INT_ENABLE;							\
	UCSRB = ucInByte;										\
}


///////////////////////////////////////////////////////////////////////////////
// initSerialPort
//

void initSerialPort( unsigned portLONG ulWantedBaud, 
						unsigned char rxQueueLength,
						unsigned char txQueueLength )
{
	unsigned char ulBaudRateCounter;
	unsigned char ucByte;

	portENTER_CRITICAL();
	{
		// Create the queues used by the com test task. 
		rxQueue = xQueueCreate( rxQueueLength, ( unsigned char ) sizeof( unsigned char ) );
		txQueue = xQueueCreate( txQueueLength, ( unsigned char ) sizeof( unsigned char ) );

		// Calculate the baud rate register value from the equation in the
		// data sheet. 
		ulBaudRateCounter = ( configCPU_CLOCK_HZ / ( serBAUD_DIV_CONSTANT * ulWantedBaud ) ) - ( unsigned portLONG ) 1;

		/* Set the baud rate. */	
		ucByte = ( unsigned char ) ( ulBaudRateCounter & ( unsigned long ) 0xff );	
		UBRRL = ucByte;

		ulBaudRateCounter >>= ( unsigned long ) 8;
		ucByte = ( unsigned portCHAR ) ( ulBaudRateCounter & ( unsigned long ) 0xff );	
		
		UBRRH = ucByte;

		// Enable the Rx interrupt.  The Tx interrupt will get enabled
		// later. Also enable the Rx and Tx.
		
		UCSRB = ( serRX_INT_ENABLE | serRX_ENABLE | serTX_ENABLE );	

		// Set the data bits to 8.		
		UCSRC = ( serUCSRC_SELECT | serEIGHT_DATA_BITS );
	
	}
	
	portEXIT_CRITICAL();
	
}

///////////////////////////////////////////////////////////////////////////////
// serialGetChar
//

signed portBASE_TYPE serialGetChar( signed portCHAR *pcRxedChar, 
										portTickType xBlockTime )
{
	// Get the next character from the buffer.  Return false if no characters
	// are available, or arrive before xBlockTime expires.
	if( xQueueReceive( rxQueue, pcRxedChar, xBlockTime ) ) {
		return pdTRUE;
	}
	else {
		return pdFALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
// serialPutChar
//

signed portBASE_TYPE serialPutChar( signed portCHAR cOutChar, 
										portTickType xBlockTime )
{
	// Return false if after the block time there is no room on the Tx queue. 
	if ( xQueueSend( txQueue, &cOutChar, xBlockTime ) != pdPASS ) {
		return pdFAIL;
	}
	
	serialInterruptOn();

	return pdPASS;
}

///////////////////////////////////////////////////////////////////////////////
// serialClose
//

void serialClose( void )
{
	unsigned char ucByte;

	// Turn off the interrupts.  We may also want to delete the queues and/or
	// re-install the original ISR. 

	portENTER_CRITICAL();
	{
		serialInterruptOff();		
		ucByte = UCSRB;
	
		ucByte &= ~serRX_INT_ENABLE;
	
		UCSRB = ucByte;
	
	}
	portEXIT_CRITICAL();
}

///////////////////////////////////////////////////////////////////////////////
// SIGNAL( SIG_UART_RECV )
//

#ifdef USE_UART1
ISR(USART0_RX_vect)
#endif
#ifdef USE_UART2
ISR(USART1_RX_vect)
#endif
{
	signed portCHAR cChar;

	// Get the character and post it on the queue of Rxed characters.
	// If the post causes a task to wake force a context switch as the woken task
	// may have a higher priority than the task we have interrupted. 
	
	cChar = UDR;

	if( xQueueSendFromISR( rxQueue, &cChar, pdFALSE ) ) {
		taskYIELD();
	}
}

///////////////////////////////////////////////////////////////////////////////
// SIGNAL( SIG_UART_DATA )
//

#ifdef USE_UART1
ISR(USART0_UDRE_vect)
#endif
#ifdef USE_UART2
ISR(USART1_UDRE_vect)
#endif
{
	signed portCHAR cChar, cTaskWoken;
	
	if ( xQueueReceiveFromISR( txQueue, &cChar, &cTaskWoken ) == pdTRUE ) {
		// Send the next character queued for Tx. 	
		UDR = cChar;
	}
	else {
		// Queue empty, nothing to send. 
		serialInterruptOff();
	}
}
