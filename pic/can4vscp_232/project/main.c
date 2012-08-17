/* ****************************************************************************
 * 	CAN4VSCP - RS232 CAN interface for VSCP
 * 	VSCP - Very Simple Control Protocol
 * 	http://www.vscp.org
 *  Copyright (C) 2000-2010 Ake Hedman, eurosource
 *                          <ake@eurosource.se>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
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
 * ****************************************************************************
 * $Id: main.c 835 2008-02-08 13:58:32Z jaguarondi $
 */

#include <p18cxxx.h>
#include <timers.h>
#include <eeprom.h>
#include <can18xx8.h>
#include <can18f.h>
#include <vscp.h>
//#include <vscp_class.h>
//#include <vscp_type.h>

#include "can4vscp.h"
#include "serint.h"
#include "main.h"

#if defined(RELEASE)

// common
#pragma config WDT = ON, WDTPS = 128	// Watchdog enabled
#pragma config OSC = HSPLL
#pragma config LVP = ON					// Enable low voltage programming
#pragma config CPB = ON					// Code Protection Block 0 enabled

#if defined(__18F248) || defined(__18F258)
#pragma config BOR  = ON
#pragma config PWRT = ON				// Power-up Timer enabled
#pragma config STVR = ON	    		// Stack overflow reset on
#pragma config BORV = 42				// Brown out reset at 2.7V
#endif

#if defined(__18F2480) || defined(__18F2580)
#pragma config BOREN = BOACTIVE
#pragma config STVREN = ON				// Stack overflow reset on
#pragma config BORV = 3					// 
#endif


#else // DEBUG


// common
#pragma config WDT = OFF				// No watchdog timer
#pragma config OSC = HSPLL				// PLL enabed
#pragma config PWRT = ON				// Power-up Timer enabled
#pragma config LVP = OFF				// low voltage programming disabled
#pragma config CPB = OFF				// Code Protection Block 0 disabled

#if defined(__18F248) || defined(__18F258)
#pragma config STVR = ON
#pragma config BORV = 27				// Brown out reset at 2.7V
#endif

#if defined(__18F2480) || defined(__18F2580)
#pragma config BOREN = BOACTIVE			// BOR enabled
#pragma config STVREN = ON				// Stack full/underflow reset
#pragma config BORV = 1					//
#endif



#endif

// ISR Routines
void isr_low( void );

uint16_t measurement_clock;
uint8_t seconds;								// counter for seconds

// Outgoing CAN Frame
struct outgoing_frame {
	uint8_t msg[ 13 ]; // 1 byte flag + 4 bytes id,+ 8 bytes data
	uint8_t size;
};

struct outgoing_frame outfrm;

// Incoming CAN Frame under construction (from RS-232)
//
// Flags
// =====
// Bit 7 - Extended identifier
// Bit 6 - RTR
// Bit 4-5	Package type
//				000 Data packet.
//				001 Error packet.
//				010 Response ACK.
//				011	Response NACK.
//				100 Command.
//				The rest are reserved.
// Bit 0-3	Number of databytes
//

struct incoming_frame {
	uint8_t msg[ 12 ]; // 4 bytes id,+ 8 bytes data
	uint8_t size;
	uint8_t flags;
};


///////////////////////////////////////////////////////////////////////////////
// Isr() 	- Interrupt Service Routine
//			- Services Timer0 Overflow
//			- Services GP3 Pin Change
//////////////////////////////////////////////////////////////////////////////

#if defined(RELOCATE)
#pragma code low_vector = 0x218
#else
#pragma code low_vector = 0x18
#endif

void interrupt_at_low_vector( void ) {
	_asm GOTO isr_low _endasm
}

#pragma code

#pragma interrupt isr_low
void isr_low( void )
{
	uint8_t temp;
	unsigned short tval0, tval1;
	char *p;	
	
	// Clock
	if ( PIR1bits.TMR2IF ) {	// If a Timer2 Interrupt, Then...		
		measurement_clock++;
		PIR1bits.TMR2IF = 0;	// Clear Timer0 Interrupt Flag
	}
	
	// USART Receive
	if ( PIR1_RCIF && PIE1_RCIE ) {
		serRxIsr();
	}

	// USART Transmit
	if ( PIR1_TXIF && PIE1_TXIE) {
		serTxIsr();
	}

	// CAN error
	if ( PIR3bits.ERRIF ) {		
		temp = COMSTAT;
		PIR3 = 0;	
	}
	
	return;
}


//***************************************************************************
// Main() - Main Routine
//***************************************************************************
void main()
{
	uint8_t c;
	uint8_t i;
	uint8_t msgflags;	// RS-232 frame flags
	
	struct incoming_frame infrm;

	uint8_t state_incoming;
	uint8_t substate_incoming;
	
	// Incoming CAN data
	uint32_t ican_id;
	uint8_t ican_data[ 8 ];
	uint8_t ican_dlc;
	uint8_t ican_flags;
	
	unsigned long msgid;
	
	state_incoming = INCOMING_STATE_NONE;
	substate_incoming = INCOMING_SUBSTATE_NONE;
	
	init();				// Initialize Microcontroller	
	
	while ( 1 ) {		// Loop Forever	
		
		ClrWdt();
		
		// Incoming CAN message
		if ( vscp18f_readMsg( &ican_id, ican_data, &ican_dlc, &ican_flags ) ) {
			
			// Send message on RS-323
			serPutByte( DLE );
			serPutByte( STX );

			msgflags = ican_dlc;
			
			if ( ican_flags & CAN_RX_XTD_FRAME ) {
				msgflags |= 0x80;	// This is an extended frame
			}
				
			if ( ican_flags & CAN_RX_RTR_FRAME ) {
				msgflags |= 0x40; // This is a RTR frame
			}
			
			serPutByte( msgflags );
			
			// Send id
			c = ( ican_id >> 24 ) & 0xff;
			serPutByteSubst( c );
			c = ( ican_id >> 16 ) & 0xff;
			serPutByteSubst( c );
			c = ( ican_id >> 8 ) & 0xff;
			serPutByteSubst( c );
			c = ican_id & 0xff;
			serPutByteSubst( c );
			
			// Send data
			for ( i=0; i < ican_dlc; i++ ) {
				serPutByteSubst( ican_data[ i ] );	
			}
			
			serPutByte( DLE );
			serPutByte( ETX );
		}
		
		// If Receive RS-232 data available
		if ( serIsGetReady() ) {
			
			c = serGetByte();
			
			switch ( state_incoming ) {
			
				case INCOMING_STATE_NONE:
				
					if ( INCOMING_SUBSTATE_NONE == substate_incoming &&
							DLE == c ) {
						substate_incoming = INCOMING_SUBSTATE_DLE;		
					}
					else if ( INCOMING_SUBSTATE_DLE == substate_incoming &&
							STX == c ) {
						state_incoming = INCOMING_STATE_STX;		
						substate_incoming = INCOMING_SUBSTATE_NONE;
						outfrm.size = 0;
					}
					else {
						state_incoming = INCOMING_STATE_NONE;		
						substate_incoming = INCOMING_SUBSTATE_NONE;	
					}
					break;
				
					
				case INCOMING_STATE_STX:
				
					if ( INCOMING_SUBSTATE_NONE == substate_incoming &&
								DLE == c ) {
						substate_incoming = INCOMING_SUBSTATE_DLE;		
					}
					else if ( INCOMING_SUBSTATE_DLE == substate_incoming &&
								STX == c ) {
									
						// This is strange as a DEL STX is not expected here
						// We try to sync up again...
						state_incoming = INCOMING_STATE_STX;		
						substate_incoming = INCOMING_SUBSTATE_NONE;
						outfrm.size = 0;
					}
					else if ( INCOMING_SUBSTATE_DLE == substate_incoming &&
								ETX == c ) {
						
						// We have a packet to send out on the bus
									
						state_incoming = INCOMING_STATE_NONE;
						substate_incoming = INCOMING_SUBSTATE_NONE;
						
						msgflags = 0;
						
						// Check for extended identifier
						if ( outfrm.msg[ 0 ] & 0x80 ) {
							msgflags |= 0x08;
						}
						
						// Check for RTR
						if ( outfrm.msg[ 0 ] & 0x40 ) {
							msgflags |= 0x40;
						}
						
						// We must at least got the id == four bytes
						if ( outfrm.size >= 4 ) {
							msgid = ((unsigned long)outfrm.msg[ 1 ] << 24 ) +
										((unsigned long)outfrm.msg[ 2 ] << 16 ) +
										((unsigned long)outfrm.msg[ 3 ] << 8 ) +
										(unsigned long)outfrm.msg[ 4 ];			
							vscp18f_sendMsg( msgid,
												outfrm.msg + 5,
												( outfrm.msg[ 0 ] & 0x0f ),
												msgflags );	
						}
					}
					// We come here if data is received
					// but also for DLE DEL
					else {		
						substate_incoming = INCOMING_SUBSTATE_NONE;	
						if ( outfrm.size < 13 ) {
							outfrm.msg[ outfrm.size++ ] = c;
						}
						else {
							// This packet has wrong format as it have
							// to many databytes - start over!
							state_incoming = INCOMING_STATE_STX;		
							substate_incoming = INCOMING_SUBSTATE_NONE;	
						}
					}
					break;	
					
				case INCOMING_STATE_ETX:
					break;		
			}
		}

		// do a meaurement if needed
		if ( measurement_clock > 1000 ) {
			
			measurement_clock = 0;
			seconds++;
				
		}

	} // while
}

///////////////////////////////////////////////////////////////////////////////
// Init - Initialization Routine
//

void init()
{
	uint8_t msgdata[ 8 ];
	uint8_t ttt;

	// Initialize data
	init_app_ram();

	// Initialize the uP

	// Set all as inputs
	TRISA = 0xFF;

	// PortB
	// RB0 - INT0 - Input -- Sensor 0
	// RB1 - INT1 - Input -- Sensor 1
	// RB2 CAN TX - output
	// RB3 CAN RX - input
	TRISB = 0xFB;

	// RC0 - Input  - Init button
	// RC1 - Output - Status LED - Default off
	// RC2 - Input  -
	// RC3 - Input  -
	// RC5 - Input	-
	TRISC = 0xFD;
	PORTCbits.RC1 = 0;	// Status LED off	

	// Timer 2 is used as a 1 ms clock
	// 156 is loaded eight time to give ~1250 cycles
	PR2 = TIMER2_RELOAD_VALUE; 	// Set reload value
	OpenTimer2( TIMER_INT_ON &
					T2_PS_1_4 &
					T2_POST_1_16 );

	vscp18f_init( FALSE );

/*
	msgdata[ 0 ] = 1;
	msgdata[ 1 ] = 2;
	msgdata[ 2 ] = 3;
	msgdata[ 3 ] = 4;

	if ( vscp18f_sendMsg( 0x123, msgdata, 4, CAN_TX_XTD_FRAME ) ) {
		;
	}
*/	

	// Init serial port
	serInit();
	serEnable();

	// Enable peripheral interrupt	
	INTCONbits.PEIE = 1;
	
	// EnableCAN Receive Error Interrupt
	PIE3bits.IRXIE = 1;

	// Enable global interrupt
	INTCONbits.GIE = 1;		

	return;
}

///////////////////////////////////////////////////////////////////////////////
// init_app_ram
//

void init_app_ram( void )
{
	measurement_clock = 0;	// start a new meaurement cycle	
	seconds = 0;	
}


///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom( void )
{
	//writeEEPROM( EEPROM_ABSMAX_TEMP0, 0xFF );
	
}


///////////////////////////////////////////////////////////////////////////////
// Get Major version number for this hardware module
//

unsigned char getMajorVersion()
{
	return FIRMWARE_MAJOR_VERSION;
}


///////////////////////////////////////////////////////////////////////////////
// Get Minor version number for this hardware module
//

unsigned char getMinorVersion()
{
	return FIRMWARE_MINOR_VERSION;
}


///////////////////////////////////////////////////////////////////////////////
// Get Subminor version number for this hardware module
//

unsigned char getSubMinorVersion()
{
	return FIRMWARE_SUB_MINOR_VERSION;
}


///////////////////////////////////////////////////////////////////////////////
// serPutByteSubst
//

void serPutByteSubst( uint8_t b )
{
	if ( DLE == b ) {
		serPutByte( DLE );
		serPutByte( DLE );	
	}
	else {
		serPutByte( b );	
	}
}



