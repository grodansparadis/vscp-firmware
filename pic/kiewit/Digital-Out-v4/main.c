/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Smart Relay Module
 * 	Version: See project header
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2009 Ake Hedman, eurosource
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
#include "../../common/eeprom.h"
#include "../../common/can18xx8.h"
#include "../../common/can18f.h"
#include "../../../common/vscp_firmware.h" 
#include "i:/src/vscp/common/vscp_class.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "i:/src/vscp/common/vscp_type.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "version.h"
#include "smartrelay.h"
#include "main.h" 


#if defined(RELEASE)

#pragma config WDT = ON, WDTPS = 128
#pragma config OSC = HSPLL
#if defined(__18F248) || defined(__18F258)
#pragma config PWRT = ON
#pragma config STVR = ON
#endif
#if defined(__18F2480) || defined(__18F2580)
#pragma config BOREN = BOACTIVE
#pragma config STVREN = ON
#endif
#pragma config BORV = 3
#pragma config LVP = OFF
//#pragma config CPB = ON
#pragma config CPB = OFF

#else

#pragma config WDT = OFF
#pragma config OSC = HSPLL
#pragma config PWRT = ON
#if defined(__18F248) || defined(__18F258)
#pragma config STVR = ON
#endif
#if defined(__18F2480) || defined(__18F2580)
#pragma config BOREN = BOACTIVE
#pragma config STVREN = ON
#endif
#pragma config BORV = 3
#pragma config LVP = OFF
#pragma config CPB = OFF

#endif


// Startup code from c018.c
void _startup (void);

// ISR Routines
void isr_low( void );

extern const rom int log_Table[];			// log10 for Hummidity measurement

// The device URL (max 32 characters including null termination)
// Don't forget to add vscphost in your host file with his corresponding IP-adress
const uint8_t vscp_deviceURL[] = "vscphost/DOV4.xml";

volatile unsigned long measurement_clock;		// Clock for VSCP (Heartbeat every minute)
volatile unsigned long ten_ms_clock;   	    	// Clock for application
volatile unsigned long hundred_ms_clock;   	    // Clock for application
volatile unsigned long one_s_clock;    	    	// Clock for application
volatile unsigned long ten_s_clock;   	        // Clock for application
volatile unsigned long hundred_s_clock;   	    // Clock for application
volatile unsigned long thousand_s_clock;   	    // Clock for application
volatile unsigned long tenthousand_s_clock;   	// Clock for application

unsigned char relay_protection_timer[15];

///////////////////////////////////////////////////////////////////////////////
// Isr() 	- Interrupt Service Routine
//      	- Services Timer0 Overflow
//      	- Services GP3 Pin Change
//////////////////////////////////////////////////////////////////////////////

#ifdef RELOCATE
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
	BYTE temp;
	unsigned short tval0, tval1;
	char *p;	
	
	// Clock
	if ( PIR1bits.TMR2IF ) {	// If a Timer2 Interrupt, Then...
		
		vscp_timer++;
		measurement_clock++;
		ten_ms_clock++;

		// Check for init button
		if ( INIT_BUTTON ) {
			vscp_initbtncnt = 0;
		}
		else {
			// Active
			vscp_initbtncnt++;
		}

		// Status LED02.

		vscp_statuscnt++;
		if ( ( VSCP_LED_BLINK1 == vscp_initledfunc ) && ( vscp_statuscnt > 100 ) ) {

			if ( INIT_LED ) {
				INIT_LED = 0;
			}
			else {
				INIT_LED = 1;
			}	

			vscp_statuscnt = 0;

		}
		else if ( VSCP_LED_ON == vscp_initledfunc ) {
			INIT_LED = 1;	
			vscp_statuscnt = 0;	
		}
		else if ( VSCP_LED_OFF == vscp_initledfunc ) {
			INIT_LED = 0;
			vscp_statuscnt = 0;
		}

		PIR1bits.TMR2IF = 0;     // Clear Timer0 Interrupt Flag

	}

/*	
	// CAN error
	if ( PIR3bits.ERRIF ) {
		
		temp = COMSTAT;
		PIR3 = 0;
		
	}
*/	
	return;
}


//***************************************************************************
// Main() - Main Routine
//***************************************************************************
void main()
{
	unsigned char a;
	unsigned char i;
	unsigned char shortpress;
	unsigned char btransition;
    unsigned char ctrlreg;

	
	init();              	// Initialize Microcontroller	
	
	// Check VSCP persistent storage and
	// restore if needed
	if ( !vscp_check_pstorage() ) {
		
		// Spoiled or not initialized - reinitialize
		init_app_eeprom();	

	}

	vscp_init();			// Initialize the VSCP functionality

	while ( 1 ) {           // Loop Forever	
		
		ClrWdt();			// Feed the dog

		if ( ( vscp_initbtncnt > 250 ) && ( VSCP_STATE_INIT != vscp_node_state ) ) {
			// Init button pressed
			vscp_nickname = VSCP_ADDRESS_FREE;
			writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
			vscp_init();
			shortpress = 0;
		}

	    if ( ( vscp_initbtncnt > 100 ) && ( VSCP_STATE_ACTIVE == vscp_node_state ) ) {
			// Short Init button press -> Send Probe Ack event
			shortpress = 1;
		}
		if ( (shortpress == 1) && (INIT_BUTTON) && (vscp_initbtncnt <= 250)) {
			SendProbeAck(vscp_initbtncnt);
			shortpress = 0;
		}

		// Check for a valid  event
		vscp_imsg.flags = 0;
		vscp_getEvent();

		// Do 10ms jobs 
		if ( ( ten_ms_clock > 10 ) && ( VSCP_STATE_ACTIVE == vscp_node_state ) ) {
			ten_ms_clock = 0;
		    doApplicationTenMilliSecondWork();
		}
		// Do 1s jobs 
		if ( measurement_clock > 1000 ) {
			
			measurement_clock = 0;
			// Do VSCP one second jobs
			vscp_doOneSecondWork();
			
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
					vscp_goActiveState();
				}
				break;

			case VSCP_STATE_INIT:			// Assigning nickname
				vscp_handleProbeState();
				break;

			case VSCP_STATE_PREACTIVE:		// Waiting for host initialisation
				vscp_goActiveState();					
				break;

			case VSCP_STATE_ACTIVE:			// The normal state
				
				if ( vscp_imsg.flags & VSCP_VALID_MSG ) {	// incoming message?
					
					vscp_handleProtocolEvent();
					doDM();		// Feed DM
						
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

	} // while
}

///////////////////////////////////////////////////////////////////////////////
// Init - Initialization Routine
//  

void init()
{
	BYTE msgdata[ 8 ];

	// Initialize data
	init_app_ram();

	// Initialize the uP

	// PortA
	// RA0 - output - Relay 9
	// RA1 - output - Relay 10
	// RA2 - output - Relay 11
	// RA3 - output - Relay 12
	// RA4 - output - Relay 13
	// RA5 - output - Relay 14
	// RA6 - OSC
	// RA7 - OSC
	TRISA = 0b11000000;
	// Configure portA as digital inputs
    ADCON1 = 0b00001111;                        

	// PortB
	// RB0 - INT0   - Input -- Init button
	// RB1 - INT1   - output --  Relay 15
	// RB2 - CAN TX - output --
	// RB3 - CAN RX - input  -- 
	// RB4 -  		- output -- Status LED - Default off
	// RB5 - 		- input  -- 
	// RB6 - 		- input  -- 
	// RB7 - 		- input  -- 
	TRISB = 0b11101001;
	
	// RC0 - Output - Relay 1
	// RC1 - Output - Relay 2
    // RC2 - Output - Relay 3
	// RC3 - Output - Relay 4
	// RC4 - Output - Relay 5
	// RC5 - Output	- Relay 6 
	// RC6 - Output - Relay 7
	// RC7 - Output - Relay 8
	TRISC = 0b00000000;
	
	INIT_LED = 0;	// Status LED off	

	// Sensor 0 timer
	OpenTimer0( TIMER_INT_OFF & 
					T0_16BIT & 
					T0_PS_1_2 &
					T0_SOURCE_INT );		

	// Sensor 1 timer
	OpenTimer1( TIMER_INT_OFF & 
					T1_16BIT_RW &
					T1_SOURCE_INT & 
					T1_PS_1_1 & 
					T1_OSC1EN_OFF & 
					T1_SYNC_EXT_OFF );	

	// Timer 2 is used as a 1 ms clock
	// 156 is loaded eight time to give ~1250 cycles
	PR2 = TIMER2_RELOAD_VALUE; 	// Set reload value
	OpenTimer2( TIMER_INT_ON & 
					T2_PS_1_4 & 
					T2_POST_1_16 );

	vscp18f_init( TRUE );
	
	// Initialize all relays in off pos
	OUT1 = 0;
	OUT2 = 0;
	OUT3 = 0;
	OUT4 = 0;
	OUT6 = 0;
	OUT5 = 0;
	OUT7 = 0;
	OUT8 = 0;
	OUT9 = 0;
	OUT10 = 0;
	OUT11 = 0;
	OUT12 = 0;
	OUT13 = 0;
	OUT14 = 0;
	OUT15 = 0;
	
/* only for testpurpose
    msgdata[ 0 ] = 1;
	msgdata[ 1 ] = 2;
	msgdata[ 2 ] = 3;
	msgdata[ 3 ] = 4;
	msgdata[ 4 ] = 5;
	msgdata[ 5 ] = 6;
	msgdata[ 6 ] = 7;
	msgdata[ 7 ] = 8;
	if ( vscp18f_sendMsg( 0x7f00000,  msgdata, 8, CAN_TX_XTD_FRAME ) ) {
		;
	}
*/

	// Enable peripheral interrupt	
	INTCONbits.PEIE = 1;
	
	// EnableCAN Receive Error Interrupt
	//PIE3bits.IRXIE = 1;

	// Enable global interrupt
	INTCONbits.GIE = 1;		

	return;
}

///////////////////////////////////////////////////////////////////////////////
// init_app_ram
//

void init_app_ram( void )
{
	unsigned char i;
	
	measurement_clock = 0;	// start a new meaurement cycle
	
	// Clear timers
	for ( i=0; i<15; i++ ) {
		relay_protection_timer[ i ] = 0;
	}
}
 

///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom( void )
{
	unsigned char i,j;

	for ( i=0; i<15; i++ ) {
		writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME0 + i, 0 );
		writeEEPROM( VSCP_EEPROM_END + REG_RELAY1_SUBZONE + i, 0 );
		writeEEPROM( VSCP_EEPROM_END + REG_RELAY1_ZONE + i, 0 );
		writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i, RELAY_CONTROLBIT_ONEVENT | 
																RELAY_CONTROLBIT_OFFEVENT |
																RELAY_CONTROLBIT_ENABLED );
	}
	
	writeEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE, 0 );
	
	// * * * Decision Matrix * * *
	// All elements disabled.
	for ( i=0; i<DESCION_MATRIX_ELEMENTS; i++ ) {
		for ( j=0; j<8; j++ ) {
			writeEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + i*8 + j, 0 );
		}
	}
		
}

///////////////////////////////////////////////////////////////////////////////
// doApplicationTenMilliSecondWork
//
void doApplicationTenMilliSecondWork( void )
{
    uint8_t i;
    uint8_t ctrlreg;	// Current control register

    for ( i=0; i<15; i++ ) {
	    // Get type of timer from the control register for this relay 
		ctrlreg = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i );
		// If not enabled check next
		if ( !( ctrlreg & RELAY_CONTROLBIT_ENABLED ) ) continue;

		switch (ctrlreg & 0b00000111) {
			case 1: 
				doApplicationWork(i);
				break;
			case 2: 
				if ( hundred_ms_clock > 10 ) {
					hundred_ms_clock = 0;
					doApplicationWork(i);
				}
				else {
					hundred_ms_clock++;
				}
				break;
			case 3:
				if ( one_s_clock > 100 ) {
					one_s_clock = 0;
					doApplicationWork(i);
				}
				else {
					one_s_clock++;
				}
				break;
			case 4:
				if ( ten_s_clock > 1000 ) {
					ten_s_clock = 0;
					doApplicationWork(i);
				}
				else {
					ten_s_clock++;
				}
				break;
			case 5:
				if ( hundred_s_clock > 10000 ) {
					hundred_s_clock = 0;
					doApplicationWork(i);
				}
				else {
					hundred_s_clock++;
				}
				break;
			case 6:
				if ( thousand_s_clock > 100000 ) {
					thousand_s_clock = 0;
					doApplicationWork(i);
				}
				else {
					thousand_s_clock++;
				}
				break;
			case 7:
				if ( tenthousand_s_clock > 1000000 ) {
					tenthousand_s_clock = 0;
					doApplicationWork(i);
				}
				else {
					tenthousand_s_clock++;
				}
				break;
			default:
				break;
		}						
	}
}

/*
///////////////////////////////////////////////////////////////////////////////
// doApplicationOneSecondWork
//

void doApplicationOneSecondWork( void )
{
	doApplicationWork();
}
*/

///////////////////////////////////////////////////////////////////////////////
// doApplicationWork
//

void doApplicationWork( unsigned char i )
{
    uint8_t ctrlreg;	// Current control register
	BOOL bOn;
    
       // Get control register for this relay
	ctrlreg = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i );
				
	// If not enabled check next
	if ( !( ctrlreg & RELAY_CONTROLBIT_ENABLED ) ) return;
									
    if ( relay_protection_timer[ i ] ) {
					
	    relay_protection_timer[ i ]--;
					
		// Check if its time to act on protection time
		if ( !relay_protection_timer[ i ] && 
			( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i ) & RELAY_CONTROLBIT_PROTECTION ) ) {

		    // Yes - its time to protect this relay
			if (i < 8) { doActionOff( 0, ( 1 << i ), 0 ); }
			else { doActionOff( 0, ( 1 << (i-8) ), 8 ); }
						
			// Should alarm be sent?
			if ( ctrlreg & RELAY_CONTROLBIT_ALARM ) {
			    SendRelayInformationEvent( i, VSCP_CLASS1_ALARM, VSCP_TYPE_ALARM_ALARM );
			}
						
		}	
	}
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
// Get GUID from EEPROM
//

#ifdef GUID_IN_EEPROM
unsigned char getGuidFromEEprom( unsigned char idx )
{
	readEEPROM( EEPROM_REG_GUIID0 + idx );		
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//

#ifdef MANUFACTURER_INFO_IN_EEPROM
unsigned char getManufacturerIdFromEEprom( unsigned char idx )
{
	readEEPROM( EEPROM_REG_MANUFACTUR_ID0 + idx );	
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

unsigned char getBootLoaderAlgorithm( void ) 
{
	return VSCP_BOOTLOADER_PIC1;	
}

///////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//

unsigned char getBufferSize( void ) 
{
	return 8;	// Standard CAN frame
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_readNicknamePermanent
//

uint8_t vscp_readNicknamePermanent( void )
{
	return readEEPROM( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_writeNicknamePermanent
//

void vscp_writeNicknamePermanent( uint8_t nickname )
{
	writeEEPROM( VSCP_EEPROM_NICKNAME, nickname );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getZone
//

uint8_t vscp_getZone( void )
{
	return readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getSubzone
//

uint8_t vscp_getSubzone( void )
{
	return readEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE );
}

///////////////////////////////////////////////////////////////////////////////
// doWork
//
// The actual work is done here.
//

void doWork( void )
{
	unsigned char tmp;

	if ( VSCP_STATE_ACTIVE == vscp_node_state ) {	
			
	}	
}

///////////////////////////////////////////////////////////////////////////////
// vscp_readAppReg
//

uint8_t vscp_readAppReg( uint8_t reg )
{
    uint8_t rv;

	rv = 0x00; // default read

	if ( reg == REG_RELAY_STATE0_7 ) {
		rv = OUT1_8;
	}
	else if ( reg == REG_RELAY_STATE8_14 ) {
		rv = (OUT9_14 & 0b00111111) | (OUT15 << 6);
	}
	else if ( reg <= EEPROM_LAST_POS ) {
		rv = readEEPROM( VSCP_EEPROM_END + reg );
	}
	else if ( reg >= REG_DESCION_MATRIX ) {
		rv = readEEPROM( VSCP_EEPROM_END + reg );
	}
	
	return rv;

}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeAppReg
//

uint8_t vscp_writeAppReg( uint8_t reg, uint8_t val )
{
    uint8_t rv;
	uint8_t i;
	uint8_t tmpval;
	uint8_t newval;
	uint8_t mask;

	rv = ~val; // error return

	// Relay Status write?
	if ( reg == REG_RELAY_STATE0_7 ) {
		rv = OUT1_8;
		tmpval = rv ^ val; 
		for (i=0; i < 8; i++) {
			mask = (1 << i);
			if (tmpval & mask) {
				if ( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i ) & RELAY_CONTROLBIT_ENABLED ) {
					switch (i) {
						case 0: 
							if ( val & mask ) { OUT1 = 1; }
							else OUT1 = 0; 
							break;
						case 1: 
							if ( val & mask ) { OUT2 = 1; }
							else OUT2 = 0; 
							break;
						case 2: 
							if ( val & mask ) { OUT3 = 1; }
							else OUT3 = 0; 
							break;
						case 3: 
							if ( val & mask ) { OUT4 = 1; }
							else OUT4 = 0; 
							break;
						case 4: 
							if ( val & mask ) { OUT5 = 1; }
							else OUT5 = 0; 
							break;
						case 5: 
							if ( val & mask ) { OUT6 = 1; }
							else OUT6 = 0; 
							break;
						case 6: 
							if ( val & mask ) { OUT7 = 1; }
							else OUT7 = 0; 
							break;
						case 7: 
							if ( val & mask ) { OUT8 = 1; }
							else OUT8 = 0; 
							break;
					}
					if ( val & mask ) {
						if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i ) & RELAY_CONTROLBIT_PROTECTION ) {
							relay_protection_timer[ i ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME0 + i ); 
						}
//						if ( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i) & RELAY_CONTROLBIT_ONEVENT ) {
//							SendRelayInformationEvent( i , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );
//						}	
					}
					else {
						relay_protection_timer[ i ] = 0;
//						if ( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i) & RELAY_CONTROLBIT_OFFEVENT ) {
//							SendRelayInformationEvent( i , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );
//						}
					}
				}
			}
		}
		rv = OUT1_8;
	}
	else if ( reg == REG_RELAY_STATE8_14 ) {
		rv = (OUT9_14 & 0b00111111) | (OUT15 << 6);
		tmpval = rv ^ (val & 0b01111111);
		for (i=0; i < 7; i++) {
			mask = (1 << i);
			if (tmpval & mask) {
				if ( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + 8 + i ) & RELAY_CONTROLBIT_ENABLED ) {
					switch (i) {
						case 0: 
							if ( val & mask ) { OUT9 = 1; }
							else OUT9 = 0; 
							break;
						case 1: 
							if ( val & mask ) { OUT10 = 1; }
							else OUT10 = 0; 
							break;
						case 2: 
							if ( val & mask ) { OUT11 = 1; }
							else OUT11= 0; 
							break;
						case 3: 
							if ( val & mask ) { OUT12 = 1; }
							else OUT12 = 0; 
							break;
						case 4: 
							if ( val & mask ) { OUT13 = 1; }
							else OUT13 = 0; 
							break;
						case 5: 
							if ( val & mask ) { OUT14 = 1; }
							else OUT14 = 0; 
							break;
						case 6: 
							if ( val & mask ) { OUT15 = 1; }
							else OUT15 = 0; 
							break;
					}
					if ( val & mask ) {
						if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + 8 + i ) & RELAY_CONTROLBIT_PROTECTION ) {
							relay_protection_timer[  8 + i ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME0 + 8 + i ); 
						}
//						if ( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + 8 + i) & RELAY_CONTROLBIT_ONEVENT ) {
//							SendRelayInformationEvent( 8 + i , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );
//						}	
					}
					else {
						relay_protection_timer[ 8 + i ] = 0;
//						if ( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + 8 + i) & RELAY_CONTROLBIT_OFFEVENT ) {
//							SendRelayInformationEvent( 8 + i , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );
//						}
					}
				}
			}
		}
		rv = (OUT9_14 & 0b00111111) | (OUT15 << 6);
	}

	else if ( reg <= EEPROM_LAST_POS ) {
		writeEEPROM( VSCP_EEPROM_END + reg, val );
		rv = readEEPROM( VSCP_EEPROM_END + reg );
	}
	else if ( reg >= REG_DESCION_MATRIX ) {
		writeEEPROM( VSCP_EEPROM_END + reg, val );
		rv = readEEPROM( VSCP_EEPROM_END + reg );
	}
	
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
// Send Decision Matrix Information
//

void sendDMatrixInfo( void )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;

	vscp_omsg.data[ 0 ] = DESCION_MATRIX_ELEMENTS;
	vscp_omsg.data[ 1 ] = REG_DESCION_MATRIX;

	vscp_sendEvent();	// Send data		
}


///////////////////////////////////////////////////////////////////////////////
// SendRelayInformationEvent
//

void SendRelayInformationEvent( unsigned char idx, unsigned char eventClass, unsigned char eventTypeId ) 
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.class = eventClass;
	vscp_omsg.type = eventTypeId;

	vscp_omsg.data[ 0 ] = idx;	// Register
	vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_ZONE + idx);
    if ( vscp_omsg.data[ 1 ] == 0x00 ) {
		vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE );
    }
    vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_SUBZONE + idx );
    if ( vscp_omsg.data[ 2 ] == 0x00 ) {
		vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE );
    }

	vscp_sendEvent();	// Send data
}

///////////////////////////////////////////////////////////////////////////////
// Send Probe Ack
//

void SendProbeAck( unsigned char buttontime )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG+1;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_PROBE_ACK;
	vscp_omsg.data[ 0] = buttontime;

	vscp_sendEvent();	// Send data		
}

///////////////////////////////////////////////////////////////////////////////
// Do decision Matrix handling
// 
// The routine expects vscp_imsg to contain a vaild incoming event
//

void doDM( void )
{
	unsigned char i;
	unsigned char dmflags;
	unsigned short class_filter;
	unsigned short class_mask;
	unsigned char type_filter;
	unsigned char type_mask;
	
	// Don't deal with the control functionality
	if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.class ) return;
	
	for ( i=0; i<DESCION_MATRIX_ELEMENTS; i++ ) {
		
		// Get DM flags for this row
		dmflags = readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + 1 + ( 8 * i ) );
		
		// Is the DM row enabled?
		if ( dmflags & VSCP_DM_FLAG_ENABLED ) {
			
			// Should the originating id be checked and if so is it the same?
			if ( ( dmflags & VSCP_DM_FLAG_CHECK_OADDR ) &&  
                (  vscp_imsg.oaddr != readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) ) ) ) {  
                continue;
			}
			
			class_filter = ( dmflags & VSCP_DM_FLAG_CLASS_FILTER )*256 + 
									readEEPROM( VSCP_EEPROM_END + 
													REG_DESCION_MATRIX + 
													( 8 * i ) + 
													VSCP_DM_POS_CLASSFILTER  );
			class_mask = ( dmflags & VSCP_DM_FLAG_CLASS_MASK )*256 + 
									readEEPROM( VSCP_EEPROM_END + 
													REG_DESCION_MATRIX + 
													( 8 * i ) +
													 VSCP_DM_POS_CLASSMASK  );
			type_filter = readEEPROM( VSCP_EEPROM_END + 
										REG_DESCION_MATRIX + 
										( 8 * i ) + 
										VSCP_DM_POS_TYPEFILTER );
			type_mask = readEEPROM( VSCP_EEPROM_END + 
										REG_DESCION_MATRIX + 
										( 8 * i ) + 
										VSCP_DM_POS_TYPEMASK  );
				
			if ( !( ( class_filter ^ vscp_imsg.class ) & class_mask ) &&
				 	!( ( type_filter ^ vscp_imsg.type ) & type_mask )) {
					 						
				// OK Trigger this action
				switch ( readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTION  ) ) {
					
					case ACTION_ON_1:			// Enable relays in arg. bitarry
						doActionOn( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ), 0 );
						break;
						
					case ACTION_ON_2:			// Enable relays in arg. bitarry
						doActionOn( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ), 8 );
						break;
						
					case ACTION_OFF_1: 		// Disable relays in arg. bitarry
						doActionOff(dmflags,  readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ), 0 );	
						break;
						
					case ACTION_OFF_2: 		// Disable relays in arg. bitarry
						doActionOff(dmflags,  readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ), 8 );	
						break;
						
					case ACTION_STATUS_1:		// Send status for relays 1-8
						doActionStatus( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ), 0 );
						break;
						
					case ACTION_STATUS_2:		// Send status for all relays 9-15
						doActionStatus( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ), 8 );
						break;
						
					case ACTION_DISABLE_1:	// Disable realys in bitarray
						doActionDisable( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ), 0 );
						break;

					case ACTION_DISABLE_2:	// Disable realys in bitarray
						doActionDisable( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ), 8 );
						break;

					case ACTION_TOGGLE_1:	// Toggle relays in bitarray
						doActionToggle( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ), 0 );
						break;
						
					case ACTION_TOGGLE_2:	// Toggle relays in bitarray
						doActionToggle( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ), 8 );
						break;
						
				} // case	
			} // Filter/mask
		} // Row enabled
	} // for each row	
}

///////////////////////////////////////////////////////////////////////////////
// doActionOn
// 

void doActionOn( unsigned char dmflags, unsigned char arg, unsigned char relayoffset )
{
	unsigned char i;
	unsigned char val;
	unsigned char offset;
	
	for ( i=0; i<8; i++ ) {
		
		// If the relay should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		offset = i + relayoffset;

		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  ) {
			if ( 255 != vscp_imsg.data[ 1 ] ) {
				val = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_ZONE + offset );
				if ( val == 0x00 ) val = readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE );
				if ( vscp_imsg.data[ 1 ] != val ) {
					continue;
				}
			}	
		}				
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			val = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_SUBZONE + offset );
			if ( val == 0x00 ) val = readEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE );
			if ( vscp_imsg.data[ 2 ] != val ) {
				continue;
			}
		}
			
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + offset );
		
		// Do nothing if disabled
		if ( !( val & RELAY_CONTROLBIT_ENABLED ) ) continue;
		
		switch ( offset ) {
			case 0:
				OUT1 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 0 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME0 );
				}
				break;
			case 1:
				OUT2 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL1 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 1 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME1 );
				}
				break;
			case 2:
				OUT3 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL2 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME2 );
				}
				break;
			case 3:
				OUT4 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL3 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 3 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME3 );
				}
				break;
			case 4:
				OUT5 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL4 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 4 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME4 );
				}
				break;
			case 5:
				OUT6 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL5 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 5 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME5 );
				}
				break;				
			case 6:
				OUT7 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL6 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 6 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME6 );
				}
				break;				
			case 7:
				OUT8 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL7 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 7 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME7 );
				}
				break;				
			case 8:
				OUT9 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL8 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 8 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME8 );
				}
				break;				
			case 9:
				OUT10 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL9 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 9 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME9 );
				}
				break;				
			case 10:
				OUT11 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL10 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 10 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME10 );
				}
				break;				
			case 11:
				OUT12 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL11 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 11 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME11 );
				}
				break;				
			case 12:
				OUT13 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL12 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 12 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME12 );
				}
				break;				
			case 13:
				OUT14 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL13 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 13 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME13 );
				}
				break;				
			case 14:
				OUT15 = 1;
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL14 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 14 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME14 );
				}
				break;				
		}
									
		// Should on event be sent?
		if( val & RELAY_CONTROLBIT_ONEVENT ) {
			SendRelayInformationEvent( offset , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );			
		}
		
	}	
}

///////////////////////////////////////////////////////////////////////////////
// doActionOff
// 

void doActionOff( unsigned char dmflags, unsigned char arg, unsigned char relayoffset )
{
	unsigned char i;
	unsigned char val;
    unsigned char offset;
	
	for ( i=0; i<8; i++ ) {
		
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		offset = i + relayoffset;

		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  ) {
			if ( 255 != vscp_imsg.data[ 1 ] ) {
				val = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_ZONE + offset );
				if ( val == 0x00 ) val = readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE );
				if ( vscp_imsg.data[ 1 ] != val ) {
					continue;
				}
			}	
		}				
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			if ( 255 != vscp_imsg.data[ 2 ] ) {
				val = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_SUBZONE + offset );
				if ( val == 0x00 ) val = readEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE );
				if ( vscp_imsg.data[ 2 ] != val ) {
					continue;
				}
			}
		}
		
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + offset );
		
		// Do nothing if disabled
		if ( !( val & RELAY_CONTROLBIT_ENABLED ) ) continue;
		
		switch ( offset ) {
			case 0:
				OUT1 = 0;
				relay_protection_timer[ 0 ] = 0;
				break;
			case 1:
				OUT2 = 0;
				relay_protection_timer[ 1 ] = 0;
				break;
			case 2:
				OUT3 = 0;
				relay_protection_timer[ 2 ] = 0;
				break;
			case 3:
				OUT4 = 0;
				relay_protection_timer[ 3 ] = 0;
				break;
			case 4:
				OUT5 = 0;
				relay_protection_timer[ 4 ] = 0;
				break;
			case 5:
				OUT6 = 0;
				relay_protection_timer[ 5 ] = 0;
				break;				
			case 6:
				OUT7 = 0;
				relay_protection_timer[ 6 ] = 0;
				break;				
			case 7:
				OUT8 = 0;
				relay_protection_timer[ 7 ] = 0;
				break;				
			case 8:
				OUT9 = 0;
				relay_protection_timer[ 8 ] = 0;
				break;				
			case 9:
				OUT10 = 0;
				relay_protection_timer[ 9 ] = 0;
				break;				
			case 10:
				OUT11 = 0;
				relay_protection_timer[ 10 ] = 0;
				break;				
			case 11:
				OUT12 = 0;
				relay_protection_timer[ 11 ] = 0;
				break;				
			case 12:
				OUT13 = 0;
				relay_protection_timer[ 12 ] = 0;
				break;				
			case 13:
				OUT14 = 0;
				relay_protection_timer[ 13 ] = 0;
				break;				
			case 14:
				OUT15 = 0;
				relay_protection_timer[ 14 ] = 0;
				break;				
		}
									
		// Should off event be sent?
		if( val & RELAY_CONTROLBIT_OFFEVENT ) {
			SendRelayInformationEvent( offset , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );			
		}
		
	}
}

///////////////////////////////////////////////////////////////////////////////
// doActionStatus
// 

void doActionStatus( unsigned char dmflags, unsigned char arg, unsigned char relayoffset )
{
	unsigned char i;
	unsigned char val;
    unsigned char offset;
	BOOL bOn = FALSE;
	
	for ( i=0; i<8; i++ ) {
		
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
        offset = i + relayoffset;

		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  ) {
			if ( 255 != vscp_imsg.data[ 1 ] ) {
				val = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_ZONE + offset );
				if ( val == 0x00 ) val = readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE );
				if ( vscp_imsg.data[ 1 ] != val ) {
					continue;
				}
			}	
		}				
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			val = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_SUBZONE + offset );
			if ( val == 0x00 ) val = readEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE );
			if ( vscp_imsg.data[ 2 ] != val ) {
				continue;
			}
		}
		
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + offset );
		
		switch ( offset ) {
			case 0:
				bOn = (BOOL)OUT1;
				break;
									
			case 1:
				bOn = (BOOL)OUT2;
				break;
									
			case 2:
				bOn = (BOOL)OUT3;
				break;
									
			case 3:
				bOn = (BOOL)OUT4;
				break;
									
			case 4:
				bOn = (BOOL)OUT5;
				break;
									
			case 5:
				bOn = (BOOL)OUT6;
				break;				
									
			case 6:
				bOn = (BOOL)OUT7;
				break;				
									
			case 7:
				bOn = (BOOL)OUT8;
				break;				
									
			case 8:
				bOn = (BOOL)OUT9;
				break;				
									
			case 9:
				bOn = (BOOL)OUT10;
				break;				
									
			case 10:
				bOn = (BOOL)OUT11;
				break;				
									
			case 11:
				bOn = (BOOL)OUT12;
				break;				
									
			case 12:
				bOn = (BOOL)OUT13;
				break;				
									
			case 13:
				bOn = (BOOL)OUT14;
				break;				
									
			case 14:
				bOn = (BOOL)OUT15;
				break;				
		}
		
		if ( bOn ) {
		
			// Should on event be sent?
			if( val & RELAY_CONTROLBIT_ONEVENT ) {
				SendRelayInformationEvent( offset , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );			
			}
		
		}
		else {
										
			// Should off event be sent?
			if( val & RELAY_CONTROLBIT_OFFEVENT ) {
				SendRelayInformationEvent( offset , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );			
			}
		
		}
		
	}	
}

///////////////////////////////////////////////////////////////////////////////
// doActionDisable
// 

void doActionDisable( unsigned char dmflags, unsigned char arg, unsigned char relayoffset )
{
	unsigned char i;
	unsigned char val;
    unsigned char offset;
	BOOL bOn = FALSE;
	
	for ( i=0; i<8; i++ ) {
		
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		offset = i + relayoffset;

		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  ) {
			if ( 255 != vscp_imsg.data[ 1 ] ) {
				val = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_ZONE + offset );
				if ( val == 0x00 ) val = readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE );
				if ( vscp_imsg.data[ 1 ] != val ) {
					continue;
				}
			}	
		}				
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			val = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_SUBZONE + offset );
			if ( val == 0x00 ) val = readEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE );
			if ( vscp_imsg.data[ 2 ] != val ) {
				continue;
			}
		}
		
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + offset );
		writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + offset, val & ~RELAY_CONTROLBIT_ENABLED  );
	}
		
}


///////////////////////////////////////////////////////////////////////////////
// doActionToggle
// 

void doActionToggle( unsigned char dmflags, unsigned char arg, unsigned char relayoffset )
{
	unsigned char i;
	unsigned char val;
    unsigned char offset;
	BOOL bOn = FALSE;
	
	for ( i=0; i<8; i++ ) {
		
		// If the relay should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		offset = i + relayoffset;

		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  ) {
			if ( 255 != vscp_imsg.data[ 1 ] ) {
				val = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_ZONE + offset );
				if ( val == 0x00 ) val = readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE );
				if ( vscp_imsg.data[ 1 ] != val ) {
					continue;
				}
			}	
		}				
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			val = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_SUBZONE + offset );
			if ( val == 0x00 ) val = readEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE );
			if ( vscp_imsg.data[ 2 ] != val ) {
				continue;
			}
		}
		
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + offset );
		
		switch ( offset ) {
								
			case 0:
				bOn = (BOOL)OUT1;
				if (bOn) { 
					OUT1 = 0;
					relay_protection_timer[ 0 ] = 0;
    			}
				else {
					OUT1 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 0 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME0 );
					}
				}            
				break;

			case 1:
				bOn = (BOOL)OUT2;
				if (bOn) { 
					OUT2 = 0;
					relay_protection_timer[ 1 ] = 0;
    			}
				else {
					OUT2 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL1 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 1 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME1 );
					}
				}            
				break;
									
			case 2:
				bOn = (BOOL)OUT3;
				if (bOn) { 
					OUT3 = 0;
					relay_protection_timer[ 2 ] = 0;
    			}
				else {
					OUT3 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL2 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME2 );
					}
				}            
				break;
									
			case 3:
				bOn = (BOOL)OUT4;
				if (bOn) { 
					OUT4 = 0;
					relay_protection_timer[ 3 ] = 0;
    			}
				else {
					OUT4 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL3 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 3 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME3 );
					}
				}            
				break;
									
			case 4:
				bOn = (BOOL)OUT5;
				if (bOn) { 
					OUT5 = 0;
					relay_protection_timer[ 4 ] = 0;
    			}
				else {
					OUT5 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL4 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 4 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME4 );
					}
				}            
				break;
									
			case 5:
				bOn = (BOOL)OUT6;
				if (bOn) { 
					OUT6 = 0;
					relay_protection_timer[ 5 ] = 0;
    			}
				else {
					OUT6 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL5 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 5 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME5 );
					}
				}            
				break;
									
			case 6:
				bOn = (BOOL)OUT7;
				if (bOn) { 
					OUT7 = 0;
					relay_protection_timer[ 6 ] = 0;
    			}
				else {
					OUT7 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL6 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 6 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME6 );
					}
				}            
				break;
									
			case 7:
				bOn = (BOOL)OUT8;
				if (bOn) { 
					OUT8 = 0;
					relay_protection_timer[ 7 ] = 0;
    			}
				else {
					OUT8 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL7 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 7 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME7 );
					}
				}            
				break;
									
			case 8:
				bOn = (BOOL)OUT9;
				if (bOn) { 
					OUT9 = 0;
					relay_protection_timer[ 8 ] = 0;
    			}
				else {
					OUT9 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL8 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 8 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME8 );
					}
				}            
				break;
									
			case 9:
				bOn = (BOOL)OUT10;
				if (bOn) { 
					OUT10 = 0;
					relay_protection_timer[ 9 ] = 0;
    			}
				else {
					OUT10 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL9 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 9 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME9 );
					}
				}            
				break;
									
			case 10:
				bOn = (BOOL)OUT11;
				if (bOn) { 
					OUT11 = 0;
					relay_protection_timer[ 10 ] = 0;
    			}
				else {
					OUT11 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL10 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 10 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME10 );
					}
				}            
				break;
									
			case 11:
				bOn = (BOOL)OUT12;
				if (bOn) { 
					OUT12 = 0;
					relay_protection_timer[ 11 ] = 0;
    			}
				else {
					OUT12 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL11 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 11 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME11 );
					}
				}            
				break;
									
			case 12:
				bOn = (BOOL)OUT13;
				if (bOn) { 
					OUT13 = 0;
					relay_protection_timer[ 12 ] = 0;
    			}
				else {
					OUT13 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL12 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 12 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME12 );
					}
				}            
				break;
									
			case 13:
				bOn = (BOOL)OUT14;
				if (bOn) { 
					OUT14 = 0;
					relay_protection_timer[ 13 ] = 0;
    			}
				else {
					OUT14 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL13 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 13 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME13 );
					}
				}            
				break;
									
			case 14:
				bOn = (BOOL)OUT15;
				if (bOn) { 
					OUT15 = 0;
					relay_protection_timer[ 14 ] = 0;
    			}
				else {
					OUT15 = 1;
					if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL14 ) & RELAY_CONTROLBIT_PROTECTION ) {
						relay_protection_timer[ 14 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME14 );
					}
				}            
				break;
			
			default:
				break;						
		}
		
		if ( bOn ) {
		
			// Should off event be sent?
			if( val & RELAY_CONTROLBIT_OFFEVENT ) {
				SendRelayInformationEvent( offset , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );			
			}
		
		}
		else {
										
			// Should on event be sent?
			if( val & RELAY_CONTROLBIT_ONEVENT ) {
				SendRelayInformationEvent( offset , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );			
			}
		
		}
	}	
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
	return readEEPROM( VSCP_EEPROM_REG_USERID + idx );	
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
//  vscp_getEmbeddedMdfInfo
//

void vscp_getEmbeddedMdfInfo( void )
{
	// No embedded DM so we respond with info about that
	
	vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = 0;
	vscp_omsg.data[ 1 ] = 0;
	vscp_omsg.data[ 2 ] = 0;	
	
	// send the message
	vscp_sendEvent();
}

/*
///////////////////////////////////////////////////////////////////////////////
// vscp_getZone
//

uint8_t vscp_getZone( void )
{
	return readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getSubzone
//

uint8_t vscp_getSubzone( void )
{
	return readEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE );
}
*/

///////////////////////////////////////////////////////////////////////////////
// vscp_goBootloaderMode
//

void vscp_goBootloaderMode( void )
{	 											
	// OK, We should enter boot loader mode
	// 	First, activate bootloader mode
	writeEEPROM( VSCP_EEPROM_BOOTLOADER_FLAG, VSCP_BOOT_FLAG );
					
	//_asm goto _startup reset _endasm
	_asm reset _endasm
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getMatrixInfo
//

void vscp_getMatrixInfo( char *pData )
{
	uint8_t i;

	vscp_omsg.data[ 0 ] = 7;	// Matrix is seven rows
	vscp_omsg.data[ 1 ] = 72;	// Matrix start offset

	// The resr set to zero no paging
	for ( i = 2; i < 8; i++ ) {
		vscp_omsg.data[ i ] = 0;
	}	

}

///////////////////////////////////////////////////////////////////////////////
//
//

uint8_t vscp_getRegisterPagesUsed( void )
{
	return 1;	// One pae used
}

///////////////////////////////////////////////////////////////////////////////
// sendVSCPFrame
//

int8_t sendVSCPFrame( uint16_t vscpclass, 
						uint8_t vscptype, 
						uint8_t nodeid,
						uint8_t priority,
						uint8_t size, 
						uint8_t *pData )
{
	uint32_t id = ( (uint32_t)priority << 26 ) |
						( (uint32_t)vscpclass << 16 ) |
						( (uint32_t)vscptype << 8) |
						nodeid;		// nodeaddress (our address)
	
	if ( !sendCANFrame( id, size, pData ) ) {
		// Failed to send message
		vscp_errorcnt++;
		return FALSE;
	}
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// getVSCPFrame
//

int8_t getVSCPFrame( uint16_t *pvscpclass, 
						uint8_t *pvscptype, 
						uint8_t *pNodeId, 
						uint8_t *pPriority, 
						uint8_t *pSize, 
						uint8_t *pData )
{
	uint32_t id;
	
	if ( !getCANFrame( &id, pSize, pData ) ) {
		return FALSE;
	}

	*pNodeId = id & 0x0ff;
	*pvscptype = ( id >> 8 ) & 0xff;
	*pvscpclass = ( id >> 16 ) & 0x1ff;
    *pPriority = (uint16_t)( 0x07 & ( id >> 26 ) );
    
	return TRUE;
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
	if ( vscp_imsg.flags & VSCP_VALID_MSG ) return FALSE;

	if ( vscp18f_readMsg( pid, pdata, pdlc, &flags ) ) {

		// RTR not interesting
		if ( flags & CAN_RX_RTR_FRAME ) return FALSE;

		// Must be extended frame
		if ( !( flags & CAN_RX_XTD_FRAME ) ) return FALSE;
		
		return TRUE;
	}	
	
	return FALSE;
}
