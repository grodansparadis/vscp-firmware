/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2006-03-20
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 2006-2007 Ake Hedman, eurosource
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
*/


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

#define Uart_set_baudrate(bdr)  ( UBRRH = (unsigned char)((((unsigned long)FOSC*1000L)/((unsigned long)bdr*16)-1)>>8),\
                                  UBRRL = (unsigned char)(((unsigned long)FOSC*1000 )/((unsigned long)bdr*16)-1)    )


// Prototypes
void uart_putchar( char ch );
void uart_puts( char * sz );
