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
#include "config.h"
#include "../../common/can_at90can128.h"
#include "../../common/uart.h"

///////////////////////////////////////////////////////////////////////////////
// LED_On
//
// This seems to produce the right amount of delay for the LED to be
// seen
//

void Delay()
{
	unsigned char a, b; //, c;

	for (a = 1; a; a++)
		for (b = 1; b; b++)
			//for (c = 1; c; c++)
				;
}

///////////////////////////////////////////////////////////////////////////////
// LED_On
//

void LED_On( int i )
{
	PORTB = ~BIT( i );	/* low output to turn LED on */
	Delay();
}



///////////////////////////////////////////////////////////////////////////////
// main
//

int main( void )
{	
    CANMsg msg;
    int rv;
    
	DDRB = 0xFF;	/* output */
	PORTB = 0xFF;	/* all off */
    
    // Initialize UART
    UCSRA = 0;
	UCSRC = MSK_UART_8BIT;	// 8N1
    Uart_set_baudrate( 9600 );
     
    UCSRB = MSK_UART_ENABLE_TX | MSK_UART_ENABLE_RX;
    
    // Init can
    if ( ERROR_OK != can_Open( CAN_BITRATE_500K, 0, 0, 0 ) ) {
    	uart_puts("Failed to pne channel!!\n");
	}
	
	uart_puts( "VSCP AT90CAN128 Test" );

  	while ( 1 ) {
       
        msg.id = 0xffffff;
        msg.flags = CAN_IDFLAG_EXTENDED;
        msg.len = 3;
        msg.byte[ 0 ] = 11;
        msg.byte[ 1 ] = 22;
        msg.byte[ 2 ] = 33;
       
        if ( ERROR_OK == ( rv = can_SendFrame( &msg ) ) ) {
        	uart_putchar( 'S' );
        }
        else if ( ERROR_BUFFER_FULL == rv ) {
        	uart_putchar( 'F' );
        }
        else {
        	uart_putchar( '?' );
        }
       
        // Check for received frame
        if ( ERROR_OK == can_readFrame( &msg ) ) {
        	msg.id++;
			can_SendFrame( &msg );
        }
        
	}
}

