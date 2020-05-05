/* ******************************************************************************
 * VSCP (Very Simple Control Protocol) 
 * http://www.vscp.org
 *
 * Can4VSCP
 * Version see version.h
 * akhe@eurosource.se
 *
 * Copyright (C) 1995-2008 Ake Hedman, eurosource
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


#include <p18cxxx.h>
#include <timers.h>
#include <vscp_compiler.h> 	
#include <vscp_projdefs.h>	
#include <inttypes.h>
#include <eeprom.h>
#include <can18f.h>
#include <vscp.h>
#include <vscp_class.h>
#include <vscp_type.h>
#include "version.h"
#include "serint.h"
#include "main.h" 

/*
#include "../../../../../common/eeprom.h"
#include "../../../../../common/can18f.h"
#include "../../../../../../common/vscp.h" 
#include "../../../../../../common/vscp_class.h"
#include "../../../../../../common/vscp_type.h"
*/




#if defined(RELEASE)

#pragma config WDT = ON, WDTPS = 128
#pragma config OSC = HSPLL
#if defined(__18F248) || defined(__18F258)
#pragma config PWRT = ON
#pragma config STVR = ON
#endif
#if defined(__18F2480) || defined(__18F2580)
#pragma config BOR = BOACTIVE
#pragma config STVREN = ON
#endif
#pragma config BORV = 27
#pragma config LVP = ON
#pragma config CPB = ON

#else

#pragma config WDT = OFF
#pragma config OSC = HSPLL
#pragma config PWRT = ON
#if defined(__18F248) || defined(__18F258)
#pragma config STVR = ON
#endif
#if defined(__18F2480) || defined(__18F2580)
#pragma config BOR = BOACTIVE
#pragma config STVREN = ON
#endif
#pragma config BORV = 27
#pragma config LVP = OFF
#pragma config CPB = OFF

#endif

// The device URL (max 32 characters including null termination)
const rom char vscp_deviceURL[] = "www.eurosource.se/485gateway.xml";

volatile unsigned long timebase;	// Clock for synchronious work

uint8_t seconds;					// counter for seconds
uint8_t minutes;					// counter for minutes
uint8_t hours;						// Counter for hours

// ISR Routines
void isr_low( void );

uint8_t seconds;					// counter for seconds

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
//      	- Services Timer0 Overflow
//      	- Services GP3 Pin Change
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
		
		vscp_timer++;
		timebase++;

		// Check for init button
		if ( PORTCbits.RC0 ) {
			vscp_initbtncnt = 0;
		}
		else {
			// Active
			vscp_initbtncnt++;
		}

		// Status LED
		vscp_statuscnt++;
		if ( ( VSCP_LED_BLINK1 == vscp_initledfunc ) && ( vscp_statuscnt > 100 ) ) {

			if ( PORTCbits.RC1 ) {
				PORTCbits.RC1 = 0;
			}
			else {
				PORTCbits.RC1 = 1;
			}	

			vscp_statuscnt = 0;

		}
		else if ( VSCP_LED_ON == vscp_initledfunc ) {
			PORTCbits.RC1 = 1;	
			vscp_statuscnt = 0;	
		}
		else if ( VSCP_LED_OFF == vscp_initledfunc ) {
			PORTCbits.RC1 = 0;
			vscp_statuscnt = 0;
		}

		PIR1bits.TMR2IF = 0;     // Clear Timer0 Interrupt Flag

	}
	
	// USART Receive
	if ( PIR1_RCIF && PIE1_RCIE ) {
        serRxIsr();
	}

    // USART Transmit
	if ( PIR1_TXIF && PIE1_TXIE)  {
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
	
	init();    			// Initialize Microcontroller
		
	// Check VSCP persistent storage and
	// restore if needed
	if ( !vscp_check_pstorage() ) {
		
		// Spoiled or not initialized - reinitialize
		init_app_eeprom();	

	}

	vscp_init();			// Initialize the VSCP functionality	
	
	while ( 1 ) {		// Loop Forever	
		
		ClrWdt();
		
		if ( ( vscp_initbtncnt > 100 ) && ( VSCP_STATE_INIT != vscp_node_state ) ) {
			// Init button pressed
			vscp_nickname = VSCP_ADDRESS_FREE;
			writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
			vscp_init();
		}
		
		// Check for any valid CAN message
		vscp_imsg.flag = 0;
		vscp_getEvent();

		// do a meaurement if needed
		if ( timebase > 1000 ) {
			
			timebase = 0;
			seconds++;
			
			// Temperature report timers are only updated if in active 
			// state
			if ( VSCP_STATE_ACTIVE == vscp_node_state  ) {
				
			}

			if ( seconds > 59 ) {
			
				seconds = 0;
				minutes++;
				
				if ( minutes > 59 ) {
					minutes = 0;
					hours++;
				}
				
				if ( hours > 23 ) hours = 0;
				
			}				
		}
		
		
		switch ( vscp_node_state ) {

			case VSCP_STATE_STARTUP:			// Cold/warm reset

				// Get nickname from EEPROM
				if ( VSCP_ADDRESS_FREE == vscp_nickname ) {
					// new on segment need a nickname
					vscp_node_state = VSCP_STATE_INIT; 	
				}
				else {
					// been here before - go on
					vscp_node_state = VSCP_STATE_ACTIVE;
					vscp_active();
				}
				break;

			case VSCP_STATE_INIT:			// Assigning nickname
				vscp_probe();
				break;

			case VSCP_STATE_PREACTIVE:		// Waiting for host initialisation
				vscp_rcv_preactive();					
				break;

			case VSCP_STATE_ACTIVE:			// The normal state
				
				if ( vscp_imsg.flag & VSCP_VALID_MSG ) {	// incoming message?
					
					// Yes, incoming message
					if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class ) {
						switch( vscp_imsg.vscp_type ) {

							case VSCP_TYPE_PROTOCOL_SEGCTRL_HEARTBEAT:
								vscp_rcv_heartbeat();
								break;

							case VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE:
								vscp_rcv_new_node_online();
								break;
							
							case VSCP_TYPE_PROTOCOL_SET_NICKNAME:
								vscp_rcv_set_nickname();
								break;
							
							case VSCP_TYPE_PROTOCOL_DROP_NICKNAME:
								vscp_rcv_drop_nickname();
								break;

							case VSCP_TYPE_PROTOCOL_READ_REGISTER:
								if ( ( 2 == ( vscp_imsg.flag & 0x0f ) ) && 
										( vscp_nickname == vscp_imsg.data[ 0 ] ) ) {

									if ( vscp_imsg.data[ 1 ] < 0x80 )  {
										// Read application specific register
										read_app_register( vscp_imsg.data[ 1 ] );
									}
									else {
										// Read VSCP register
										vscp_rcv_readreg();
									}
								}
								break;

							case VSCP_TYPE_PROTOCOL_WRITE_REGISTER:
								if ( ( 3 == ( vscp_imsg.flag & 0x0f ) ) && 
										( vscp_nickname == vscp_imsg.data[ 0 ] ) ) {
									if ( vscp_imsg.data[ 1 ] < 0x80 ) {
										// Write application specific register
										write_app_register( vscp_imsg.data[ 1 ], vscp_imsg.data[ 2 ] );
									}
									else {
										// Read VSCP register
										vscp_rcv_writereg();
									}
								}
								break;
								
							case VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER:
								if ( ( vscp_nickname == vscp_imsg.data[ 0 ] ) &&
										( 1 == vscp_imsg.data[ 1 ] ) &&
										( readEEPROM( VSCP_EEPROM_REG_GUID + 0 ) == vscp_imsg.data[ 2 ] ) &&
										( readEEPROM( VSCP_EEPROM_REG_GUID + 3 ) == vscp_imsg.data[ 3 ] ) &&
										( readEEPROM( VSCP_EEPROM_REG_GUID + 5 ) == vscp_imsg.data[ 4 ] ) &&
										( readEEPROM( VSCP_EEPROM_REG_GUID + 7 ) == vscp_imsg.data[ 5 ] ) &&
										( ( vscp_page_select >> 8 ) == vscp_imsg.data[ 6 ] ) &&
										( ( vscp_page_select & 0xff ) == vscp_imsg.data[ 7 ] ) ) {											
									// OK, We should enter boot loader mode
									// 	First, activate bootloader mode
									writeEEPROM( VSCP_EEPROM_BOOTLOADER_FLAG, VSCP_BOOT_FLAG );
									//_asm goto _startup reset _endasm
									_asm reset _endasm
								}
								break;

							default:
								// Do work load
								break;
						}							
					}
				}
				break;

			case VSCP_STATE_ERROR:			// Everything is *very* *very* bad.
				vscp_error();
				break;

			default:					// Should not be here...
				vscp_node_state = VSCP_STATE_STARTUP;
				break;

		} 

		doWork();
		
		
		
		
		
		
		
		
		
		
		
/*		
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
*/
	

	} // while
}

///////////////////////////////////////////////////////////////////////////////
// doWork
//

void doWork( void )
{
	
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

	if ( vscp18f_sendMsg( 0x123,  msgdata, 4, CAN_TX_XTD_FRAME ) ) {
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
	timebase = 0;	// start a new meaurement cycle	
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


///////////////////////////////////////////////////////////////////////////////
// read_app_register
//

void read_app_register( unsigned char reg )
{
	int tmpval;
	unsigned char val, checksum;

	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = reg;
	vscp_omsg.data[ 1 ] = 0x00; // default read

	switch ( reg ) {

/*
		// MSB of current temperature 1
		case 0x02:
			tmpval = convertTemperature( getCurrentTemp4Sensor0(), unit );
			vscp_omsg.data[ 1 ] =  ( ( tmpval & 0xff00 ) >> 8 );			
			break;

		// LSB of current temperature 1
		case 0x03:
			tmpval = convertTemperature( getCurrentTemp4Sensor0(), unit );
			vscp_omsg.data[ 1 ] =  ( tmpval & 0x00ff );
			break;

		// MSB of current temperature 2
		case 0x04:
			tmpval = convertTemperature( getCurrentTemp4Sensor1(), unit );
			vscp_omsg.data[ 1 ] =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of current temperature 2
		case 0x05:
			tmpval = convertTemperature( getCurrentTemp4Sensor1(), unit );
			vscp_omsg.data[ 1 ] =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut high temp
		case 0x06:
			tmpval = readEEPROM( EEPROM_ABSMAX_TEMP0 ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP0 + 1 );
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut high temp
		case 0x07:
			tmpval = readEEPROM( EEPROM_ABSMAX_TEMP0 ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP0 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( tmpval & 0x00ff );
			break;
	
		// MSB of absolut high temp
		case 0x08:
			tmpval = readEEPROM( EEPROM_ABSMAX_TEMP1 ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP1 + 1 );
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut high temp
		case 0x09:
			tmpval = readEEPROM( EEPROM_ABSMAX_TEMP1 ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP1 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut low temp
		case 0x0A:
			tmpval = readEEPROM( EEPROM_ABSMIN_TEMP0 ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP0 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut low temp
		case 0x0B:
			tmpval = readEEPROM( EEPROM_ABSMIN_TEMP0 ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP0 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut low temp
		case 0x0C:
			tmpval = readEEPROM( EEPROM_ABSMIN_TEMP1 ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP1 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut low temp
		case 0x0D:
			tmpval = readEEPROM( EEPROM_ABSMIN_TEMP1 ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP1 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( tmpval & 0x00ff );
			break;
 
		// MSB of running mean temperature
		case 0x0E:
			tmpval = get24HourMean4Sensor0();
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of running mean temperature
		case 0x0F:
			tmpval = get24HourMean4Sensor0();
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( tmpval & 0x00ff );
			break;

		// MSB of running mean temperature
		case 0x10:
			tmpval = get24HourMean4Sensor1();
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of running mean temperature
		case 0x11:
			tmpval = get24HourMean4Sensor1();
			tmpval = convertTemperature( tmpval, unit );
			vscp_omsg.data[ 1 ] =  ( tmpval & 0x00ff );
			break;

		// Temperature 1 1ow alarm point
		case 0x12:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP0_LOW_ALARMPOINT );
			break;

		// Temperature 2 1ow alarm point
		case 0x13:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP1_LOW_ALARMPOINT );
			break;

		// Temperature 1 high alarm point
		case 0x14:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP0_HIGH_ALARMPOINT );
			break;

		// Temperature 2 high alarm point
		case 0x15:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP1_HIGH_ALARMPOINT );
			break;

		// Temperature 1 sensor error counter
		case 0x16:
			vscp_omsg.data[ 1 ] = error_cnt_tempsensor0;
			break;

		// Temperature 2 sensor error counter
		case 0x17:
			vscp_omsg.data[ 1 ] = error_cnt_tempsensor1;
			break;

		// Temperature 1 event interval
		case 0x18:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_INTERVAL_TEMP0 );
			break;

		// Temperature 2 event interval
		case 0x19:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_INTERVAL_TEMP1 );
			break;

		// Temperature format
		case 0x1A:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP_UNIT );
			break;

		// MSB of temperature 1 calibration 
		case 0x1B:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP0_CALIBRATION );
			break;

		// LSB of temperature 1 calibration 
		case 0x1C:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP0_CALIBRATION + 1 );
			break;

		// MSB of temperature 2 calibration 
		case 0x1D:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP1_CALIBRATION );
			break;

		// LSB of temperature 2 calibration 
		case 0x1E:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP1_CALIBRATION + 1 );
			break;

		// Hysteresis for temperature 1 alarm
		case 0x1F:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP0_HYSTERESIS );
			break;

		// Hysteresis for temperature 2 alarm
		case 0x20:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP1_HYSTERESIS );
			break;
			
		// Zone
		case 0x21:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_ZONE );
			break;
			
		// SubZone
		case 0x22:
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_SUBZONE );
			break;		
*/	 
		default:
			vscp_omsg.data[ 1 ] = 0;	
			break;
	}

	// Send data
	vscp_sendEvent();

}

///////////////////////////////////////////////////////////////////////////////
// write_app_register
//

void write_app_register( unsigned char reg, unsigned char val )
{
	uint8_t i;
	int tmpval;
	unsigned char checksum;

	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = reg;
	vscp_omsg.data[ 1 ] = ~val; // error return

	switch ( reg ) {

/*
		// MSB of absolut high temp1
		case 0x08:
			writeEEPROM( ( EEPROM_ABSMAX_TEMP0 + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_TEMP0, 0xff );
			vscp_omsg.data[ 1 ] = 0xff;
			break;

		// LSB of absolut high temp1
		case 0x09:
			writeEEPROM( ( EEPROM_ABSMAX_TEMP0 + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_TEMP0, 0xff );
			vscp_omsg.data[ 1 ] = 0xff;
			break;

		// MSB of absolut high temp2
		case 0x0A:
			writeEEPROM( ( EEPROM_ABSMAX_TEMP1 + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_TEMP1, 0xff );
			vscp_omsg.data[ 1 ] = 0xff;
			break;

		// LSB of absolut high temp2
		case 0x0B:
			writeEEPROM( ( EEPROM_ABSMAX_TEMP1 + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_TEMP1, 0xff );
			vscp_omsg.data[ 1 ] = 0xff;
			break;

		// MSB of absolut low temp1
		case 0x0C:
			writeEEPROM( ( EEPROM_ABSMIN_TEMP0 + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_TEMP0, 0xff );
			vscp_omsg.data[ 1 ] = 0x7f;
			break;

		// LSB of absolut low temp1
		case 0x0D:
			writeEEPROM( ( EEPROM_ABSMIN_TEMP0 + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_TEMP0, 0xff );
			vscp_omsg.data[ 1 ] = 0xff;
			break;

		// MSB of absolut low temp2
		case 0x0E:
			writeEEPROM( ( EEPROM_ABSMIN_TEMP1 + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_TEMP1, 0xff );
			vscp_omsg.data[ 1 ] = 0x7f;
			break;

		// LSB of absolut low temp2
		case 0x0F:
			writeEEPROM( ( EEPROM_ABSMIN_TEMP1 + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_TEMP1, 0xff );
			vscp_omsg.data[ 1 ] = 0xff;
			break;

		// MSB of running mean temperature 1
		case 0x10:
			for ( i=0; i<24; i++ ) {
				temperature0_array[ i ] = 0;	
			}
			vscp_omsg.data[ 1 ] = 0x00;
			break;

		// LSB of running mean temperature 1
		case 0x11:
			for ( i=0; i<24; i++ ) {
				temperature0_array[ i ] = 0;	
			}
			vscp_omsg.data[ 1 ] = 0x00;
			break;

		// MSB of running mean temperature 2
		case 0x12:
			for ( i=0; i<24; i++ ) {
				temperature1_array[ i ] = 0;	
			}
			vscp_omsg.data[ 1 ] = 0x00;
			break;

		// LSB of running mean temperature 2
		case 0x13:
			for ( i=0; i<24; i++ ) {
				temperature1_array[ i ] = 0;	
			}
			vscp_omsg.data[ 1 ] = 0x00;
			break;

		// Temperature 1 low alarm point
		case 0x14:
			writeEEPROM( EEPROM_TEMP0_LOW_ALARMPOINT, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP0_LOW_ALARMPOINT );
			break;

		// Temperature 2 low alarm point
		case 0x15:
			writeEEPROM( EEPROM_TEMP1_LOW_ALARMPOINT, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP1_LOW_ALARMPOINT );
			break;

		// Temperature 1 high alarm point
		case 0x16:
			writeEEPROM( EEPROM_TEMP0_HIGH_ALARMPOINT, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP0_HIGH_ALARMPOINT );
			break;

		// Temperature 2 high alarm point
		case 0x17:
			writeEEPROM( EEPROM_TEMP1_HIGH_ALARMPOINT, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP1_HIGH_ALARMPOINT );
			break; 

		// Temperature 1 sensor error counter
		case 0x18:
			error_cnt_tempsensor0 = 0;
			vscp_omsg.data[ 1 ] = 0x00;
			break;

		// Temperature 2 sensor error counter
		case 0x19:
			error_cnt_tempsensor1 = 0;
			vscp_omsg.data[ 1 ] = 0x00;
			break;

		// Temperature 1 event interval
		case 0x1A:
			writeEEPROM( EEPROM_INTERVAL_TEMP0, val );		
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_INTERVAL_TEMP0 );
			break;

		// Temperature 2 event interval
		case 0x1B:
			writeEEPROM( EEPROM_INTERVAL_TEMP1, val );		
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_INTERVAL_TEMP1 );
			break;

		// Temperature format
		case 0x1C:
			writeEEPROM( EEPROM_TEMP_UNIT, val );
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP_UNIT );
			break;

		// MSB of temperature 1 calibration 
		case 0x1D:
			writeEEPROM( EEPROM_TEMP0_CALIBRATION, val );
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP0_CALIBRATION );
			break;

		// LSB of temperature 1 calibration 
		case 0x1E:
			writeEEPROM( EEPROM_TEMP0_CALIBRATION + 1, val );
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP0_CALIBRATION + 1 );
			break;

		// MSB of temperature 2 calibration 
		case 0x1F:
			writeEEPROM( EEPROM_TEMP1_CALIBRATION, val );
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP1_CALIBRATION );
			break;

		// LSB of temperature 2 calibration 
		case 0x20:
			writeEEPROM( EEPROM_TEMP1_CALIBRATION + 1, val );
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP1_CALIBRATION + 1 );
			break;

		// Hysteresis for temperature 1 alarm
		case 0x21:
			writeEEPROM( EEPROM_TEMP0_HYSTERESIS, val );
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP0_HYSTERESIS );
			break;

		// Hysteresis for temperature 2 alarm
		case 0x22:
			writeEEPROM( EEPROM_TEMP1_HYSTERESIS, val );
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_TEMP1_HYSTERESIS );
			break;
			
		// Zone
		case 0x23:
			writeEEPROM( EEPROM_ZONE, val );
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_ZONE );
			break;
			
		// SubZone
		case 0x24:
			writeEEPROM( EEPROM_SUBZONE, val );
			vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_SUBZONE );
			break;	
*/
		default:
			vscp_omsg.data[ 1 ] = ~val; // error return	
			break;
	}

	// Send data
	vscp_sendEvent();	
}



///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Get Major version number for this hardware module
//

unsigned char vscp_getMajorVersion()
{
	return FIRMWARE_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Minor version number for this hardware module
//

unsigned char vscp_getMinorVersion()
{
	return FIRMWARE_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Subminor version number for this hardware module
//

unsigned char vscp_getSubMinorVersion()
{
	return FIRMWARE_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP_GUID
//
// Get GUID from EEPROM
//

uint8_t vscp_getGUID( uint8_t idx )
{
	return readEEPROM( VSCP_EEPROM_REG_GUID + idx );		
}


///////////////////////////////////////////////////////////////////////////////
// getDeviceURL
//
// Get device URL from EEPROM
//

uint8_t vscp_getMDF_URL( uint8_t idx )
{
	return vscp_deviceURL[ idx ];
}

///////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getUserID( uint8_t idx )
{
	return readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx );	
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPUserID
//

void vscp_setUserID( uint8_t idx, uint8_t data )
{
	writeEEPROM( idx + VSCP_EEPROM_REG_USERID, data );
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getManufacturerId( uint8_t idx )
{
	return readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx );	
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

void vscp_setManufacturerId( uint8_t idx, uint8_t data )
{
	writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx, data );	
}

///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm( void ) 
{
	return VSCP_BOOTLOADER_PIC1;	
}

///////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//

uint8_t vscp_getBufferSize( void ) 
{
	return 8;	// Standard CAN frame
}

	
///////////////////////////////////////////////////////////////////////////////
//  getNickname
//

uint8_t vscp_getNickname( void )
{
	return readEEPROM( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
//  setNickname
//

void vscp_setNickname( uint8_t nickname )
{
	writeEEPROM( VSCP_EEPROM_NICKNAME, nickname );
}

///////////////////////////////////////////////////////////////////////////////
//  getSegmentCRC
//

uint8_t vscp_getSegmentCRC( void )
{
	return readEEPROM( VSCP_EEPROM_SEGMENT_CRC );
}

///////////////////////////////////////////////////////////////////////////////
//  setSegmentCRC
//

void vscp_setSegmentCRC( uint8_t crc )
{
	writeEEPROM( VSCP_EEPROM_SEGMENT_CRC, crc );
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPControlByte
//

void vscp_setControlByte( uint8_t ctrl )
{
	writeEEPROM( VSCP_EEPROM_CONTROL, ctrl );
}


///////////////////////////////////////////////////////////////////////////////
//  getVSCPControlByte
//

uint8_t vscp_getControlByte( void )
{
	return readEEPROM( VSCP_EEPROM_CONTROL );
}



///////////////////////////////////////////////////////////////////////////////
// sendCANFrame
//

int8_t sendCANFrame( uint32_t id, uint8_t dlc, uint8_t *pdata )
{
	if ( !vscp18f_sendMsg( id, 
							pdata , 
							dlc, 
							CAN_TX_XTD_FRAME  ) ) {
		
		// Failed to send message
		return FALSE;

	}

	vscp_omsg.flags = 0;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// getCANFrame
//

int8_t getCANFrame( uint32_t *pid, uint8_t *pdlc, uint8_t *pdata )
{
	uint8_t flags;

	// Dont read in new message if there already is a message
	// in the input buffer
	if ( vscp_imsg.flag & VSCP_VALID_MSG ) return FALSE;

	if ( vscp18f_readMsg( pid, pdata, pdlc, &flags ) ) {

		// RTR not interesting
		if ( flags & CAN_RX_RTR_FRAME ) return FALSE;

		// Must be extended frame
		if ( !( flags & CAN_RX_XTD_FRAME ) ) return FALSE;
		
		return TRUE;
	}	
	
	return FALSE;
}



