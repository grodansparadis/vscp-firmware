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

#ifndef FREERTOS_SERIAL_COMMS_H
#define FREERTOS_SERIAL_COMMS_H

#define MSK_UART_5BIT           0x00 /* UCSRnC register */
#define MSK_UART_6BIT           0x02
#define MSK_UART_7BIT           0x04
#define MSK_UART_8BIT           0x06
#define MSK_UART_9BIT           0x06

#define MSK_UART_RX_DONE        0x80 /* UCSRnA register */
#define MSK_UART_TX_COMPLET     0x40
#define MSK_UART_DRE            0x20

#define MSK_UART_ENABLE_RX      0x10 /* UCSRnB register */
#define MSK_UART_ENABLE_TX      0x08 
#define MSK_UART_TX_BIT9        0x01
#define MSK_UART_RX_BIT9        0x02

#ifdef USE_UART1

#ifndef UCSRC
    #define UCSRC   (UCSR0C)
#endif    
#ifndef UCSRB
    #define UCSRB   (UCSR0B)
#endif    
#ifndef UCSRA
    #define UCSRA   (UCSR0A)
#endif
#ifndef UDR    
    #define UDR     (UDR0)
#endif
#ifndef UBRRL    
    #define UBRRL   (UBRR0L)  
#endif       
#ifndef UBRRH                 
    #define UBRRH   (UBRR0H)
#endif   
     
#endif
  

#ifdef USE_UART2

#ifndef UCSRC
    #define UCSRC    (UCSR1C)
#endif     
#ifndef UCSRB   
    #define UCSRB    (UCSR1B)
#endif     
#ifndef UCSRA   
    #define UCSRA    (UCSR1A)
#endif     
#ifndef UDR   
    #define UDR      (UDR1)
#endif     
#ifndef UBRRL   
    #define UBRRL    (UBRR1L)
#endif     
#ifndef UBRRH                      
    #define UBRRH    (UBRR1H)
#endif    

#endif

#define Uart_set_baudrate(bdr)  ( UBRRH = (unsigned char)((((unsigned long)configCPU_CLOCK_HZ)/((unsigned long)bdr*16)-1)>>8),\
                                  UBRRL = (unsigned char)(((unsigned long)configCPU_CLOCK_HZ )/((unsigned long)bdr*16)-1)    )


typedef void * xComPortHandle;

void initSerialPort( unsigned portLONG ulWantedBaud, unsigned char rxQueueLength, unsigned char txQueueLength );
signed portBASE_TYPE serialGetChar( signed portCHAR *pcRxedChar, portTickType xBlockTime );
signed portBASE_TYPE serialPutChar( signed portCHAR cOutChar, portTickType xBlockTime );
void serialClose( void );

#endif

