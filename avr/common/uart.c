/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2006-03-20
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 2006-2011 Ake Hedman, eurosource
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

#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include <avr/io.h>
#include "uart.h"
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// uart_putchar
//

void uart_putchar( char ch )
{
  while( !( UCSRA & MSK_UART_DRE ) );
  UDR = ch;
}

///////////////////////////////////////////////////////////////////////////////
// uart_putc
//

void uart_putc( char ch )
{
  while( !( UCSRA & MSK_UART_DRE ) );
  UDR = ch;
}
    
///////////////////////////////////////////////////////////////////////////////
// uart_puts
//

void uart_puts( char * sz )
{
 	register char ch;

	//for (;ch=*sz++; ) {
	while ( ( ch = *sz++ ) ) {
		if ( ch == '\n' ) uart_putchar('\r');
  		uart_putchar( ch );
    }

	uart_putchar('\r');
	uart_putchar('\n');
	
}

///////////////////////////////////////////////////////////////////////////////
// uart_puts_P
//

void uart_puts_P( char * sz )
{
 	register char ch;

	//for (;ch=*sz++; ) {
	while ( ( ch = *sz++ ) ) {
		if ( ch == '\n' ) uart_putchar('\r');
  		uart_putchar( ch );
    }
	
}

/*************************************************************************
Function: uart_put_int()
Purpose:  transmit integer as ASCII to UART
Input:    integer value
Returns:  none
**************************************************************************/
void uart_put_int( const int val )
{
	char buffer[10];
	uart_puts( itoa( val, buffer, 10 ) );
} /* uart_puti */

/*************************************************************************
Function: uart_puthex_byte()
Purpose:  transmit upper and lower nibble as ASCII-hex to UART
Input:    byte value
Returns:  none
**************************************************************************/
void uart_puthex_byte( const unsigned char  b )
{
	uart_puthex_nibble( b >> 4 );
	uart_puthex_nibble( b );
} /* uart_puthex_byte */

/*************************************************************************
Function: uart_puthex_nibble()
Purpose:  transmit lower nibble as ASCII-hex to UART
Input:    byte value
Returns:  none
**************************************************************************/
void uart_puthex_nibble(const unsigned char b)
{
	unsigned char  c = b & 0x0f;
	if ( c > 9 ) { 
		c += 'A'-10; 
	}
	else {
		c += '0';
	}
	uart_putc(c);
} /* uart_puthex_nibble */


/*************************************************************************
Function: uart_sendChar( char data )
Purpose:  Send a character
Input:    byte value
Returns:  none
**************************************************************************/
void uart_sendChar ( char data )
{
    int i = 0;
    // to send data with the usart put the data in the usart data register
    UDR = data;
    
    // check to see if the global interrupts are enabled
    if (SREG & 0x80)
    {
        // wait until the byte is sent or we count out
        while ( !(UCSRA&0x40) && (i<10000) )
        {
             __asm__ ("nop");
            i++;
        }
    }
    else
    // wait until the byte is sent
    while ( !(UCSRA&0x40) );
    
    // clear the TXCflag
    UCSRA=(UCSRA|0x40);
}

/*************************************************************************
Function: uart_sendCharHex( char data )
Purpose:  Output HEX value
Input:    byte value
Returns:  none
**************************************************************************/
void uart_sendCharHex (char data)
{
    // sends out a byte in its HEX-notation
    unsigned char dataC = 0;
    dataC = data; //backup
    
    data = (data >> 4);
    data = (data & 0x0F);
    if ((data >= 0) && (data <= 9)) uart_sendChar(data+0x30);
    else {
        uart_sendChar(data+0x37);
    }    

    data = (dataC & 0x0F); //start from the backup
    if ((data >= 0) && (data <= 9)) uart_sendChar(data+0x30);
    else {
        uart_sendChar(data+0x37);
    }        
    
    uart_sendChar (' ');

    
}

