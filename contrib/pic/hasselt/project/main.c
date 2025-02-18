/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Smart Relay Module
 * 	Version: See project header
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2025 Ake Hedman, eurosource
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
 *
 * This software is part of the VSCP node Hasselt
 * 
 * ******************************************************************************
*/

#include <p18cxxx.h>
#include <timers.h>
#include "../../common/eeprom.h"
#include "../../common/can18xx8.h"
#include "../../common/can18f.h"
#include "../../common/vscp_firmware.h"
#include "../../src/vscp/common/vscp_class.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "../../src/vscp/common/vscp_type.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "version.h"
#include "hasselt_defines.h"
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
#pragma config CPB = ON

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

#pragma config PBADEN = OFF //does the trick for Hasselt

// Startup code from c018.c
void _startup (void);

// ISR Routines
void isr_low( void );

// The device URL (max 32 characters including null termination)
const const uint8_t vscp_deviceURL[] = "192.168.1.250:8080/hass_011.xml"; //changed for MPLAB X

volatile unsigned long measurement_clock;	// Clock for measurments
unsigned short clock_10ms;					// Clock for debouncing
BYTE seconds;								// counter for seconds
BYTE minutes;								// counter for minutes
BYTE hours;									// Counter for hours

unsigned short button_state[6];
unsigned short button_hold_timer[6];

static uint8_t BufferButton1 = 0;
static uint8_t BufferButton2 = 0;
static uint8_t BufferButton3 = 0;
static uint8_t BufferButton4 = 0;
static uint8_t BufferButton5 = 0;
static uint8_t BufferButton6 = 0;


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
		clock_10ms++;

		// Check for init button
		if ( INIT_BUTTON ) {
			vscp_initbtncnt = 0;
		}
		else {
			// Active
			vscp_initbtncnt++;
		}

		// Status LED
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
	unsigned int i;
	unsigned char keyval;
	BOOL bOn = FALSE;
	unsigned char shortpress;
	
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

// REMOVED INIT BUTTON DETECT TO TEST
//		if ( ( vscp_initbtncnt > 1024 ) && ( VSCP_STATE_INIT != vscp_node_state ) ) {
//			// Init button pressed
//			vscp_nickname = VSCP_ADDRESS_FREE;
//			writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
//			vscp_init();
//			shortpress = 0;
//		}
//
//	    if ( ( vscp_initbtncnt > 512 ) && ( VSCP_STATE_ACTIVE == vscp_node_state ) ) {
//			// Short Init button press -> Send Probe Ack event
//			shortpress = 1;
//		}
//		if ( (shortpress == 1) && (INIT_BUTTON) && (vscp_initbtncnt <= 1024)) {
//			SendProbeAck(vscp_initbtncnt);
//			shortpress = 0;
//		}

		// Check for a valid  event
		vscp_imsg.flags = 0;
		vscp_getEvent();

		// do a meaurement if needed
		if ( measurement_clock > 1000 ) {
			
			measurement_clock = 0;
			//measurement_seconds++;
			
			// Do VSCP one second jobs 
			vscp_doOneSecondWork();

			// Temperature report timers are only updated if in active 
			// state guid_reset
			if ( VSCP_STATE_ACTIVE == vscp_node_state  ) {
					
				// Do VSCP one second jobs 
			    doApplicationOneSecondWork();	
			}
							
			// If we have errors in sensor readings
			// send out error event
			//if ( error_cnt_tempsensor0 || error_cnt_tempsensor1 ) {
			//	sendErrorEvent( ERROR_SENSOR );
			//}
							
		}	
		
		//execute input debounce every 10 ms
		if ( clock_10ms > 10) {
			for ( i=0 ; i<6; i++) {
				switch (i) {
					case 0:
						keyval = DebounceButton1();
						break;
					case 1:
						keyval = DebounceButton2();
						break;
					case 2:
						keyval = DebounceButton3();
						break;
					case 3:
						keyval = DebounceButton4();
						break;
					case 4:
						keyval = DebounceButton5();
						break;
					case 5:
						keyval = DebounceButton6();
						break;
				}
			
				switch (button_state[i]) { 
					case BUTTON_RELEASED:
						if (keyval == BUTTON_PRESSED) {
							button_state[i] = BUTTON_PRESSED;
							SendInformationEvent( i , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, BUTTON_PRESSED );		
						}
						if (keyval == BUTTON_RELEASED) {
							button_state[i] = BUTTON_RELEASED;
							button_hold_timer[i] = 0;
						}
						break;
						
					case BUTTON_PRESSED:
						if (keyval == BUTTON_RELEASED) {
							button_state[i] = BUTTON_RELEASED;
							SendInformationEvent( i , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, BUTTON_KEY );
						}
						if (keyval == BUTTON_HOLD) {
							button_state[i] = BUTTON_HOLD;
						}
						break;
					case BUTTON_HOLD:
						if (keyval == BUTTON_RELEASED) {
							button_state[i] = BUTTON_RELEASED;
							SendInformationEvent( i , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, BUTTON_KEY );
						}
						if (keyval == BUTTON_HOLD) {
							button_hold_timer[i] += 1;
							if ( button_hold_timer[i] > 50 ) {
									button_state[i] = BUTTON_HOLD_REPEAT;
									SendInformationEvent( i , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, BUTTON_HOLD );
									button_hold_timer[i] = 0;
								}		
								else button_state[i] = BUTTON_HOLD;
						}
						break;
					case BUTTON_HOLD_REPEAT:
						if (keyval == BUTTON_RELEASED) {
							button_state[i] = BUTTON_RELEASED;
							SendInformationEvent( i , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, BUTTON_RELEASED );
							button_hold_timer[i] = 0;
						}
						if (keyval == BUTTON_HOLD) {
							button_hold_timer[i] += 1;
							if ( button_hold_timer[i] > 5 ) {
									SendInformationEvent( i , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, BUTTON_HOLD );
									button_hold_timer[i] = 0;
								};		
							button_state[i] = BUTTON_HOLD_REPEAT;
						}
						break;
					}
			} //end if i
			clock_10ms = 0;
		} //end 10ms

		
//		if ( clock_10ms > 10) {
//			if (DebounceButton1()) SendInformationEvent( 0, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON );
//			if (DebounceButton2()) SendInformationEvent( 1, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON );
//			if (DebounceButton3()) SendInformationEvent( 2, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON );
//			if (DebounceButton4()) SendInformationEvent( 3, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON );
//			if (DebounceButton5()) SendInformationEvent( 4, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON );
//			if (DebounceButton6()) SendInformationEvent( 5, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON );
//			clock_10ms = 0;
//		}	//end debounce	

		if ( seconds > 59 ) {
			
	        seconds = 0;
			minutes++;
				
			if ( minutes > 59 ) {
			    minutes = 0;
				hours++;
			}
				
			if ( hours > 23 ) hours = 0;
				
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
					doDM();		
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
	// RA0 - Button 6 - Input
	// RA1 - Button 5 - Input
	// RA2 - Button 4 - Input
	// RA3 - Button 3 - Input
	// RA4 - Button 2 - Input
	// RA5 - Button 1 - Input
	// RA6 - OSC
	TRISA = 0b11111111;    
	ADCON1= 0b00001111;  //Configure all analog as digital inputs        

	// PortB
	// RB0 - Init Button - Input 
	// RB1 - INT1   	 - Output 
	// RB2 - CAN TX 	 - output 
	// RB3 - CAN RX 	 - input   
	// RB4 - Status LED  - Output
	// RB5 - 		- input  -- 
	// RB6 - 		- input  -- 
	// RB7 - 		- input  -- 
	TRISB = 0b00001001;

	INIT_LED = 0; // Status LED off
	
	// RC0 - Output - Q3
	// RC1 - Output - Q2
	// RC2 - Output - PWM
	// RC3 - Output - Q1
	// RC4 - Output - Q6
	// RC5 - Output - Q5
	// RC6 - Output - Q4
	// RC7 - Output - 1Wire	
	TRISC = 0b00000000;
	
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
	ONEWIRE = 0;
	OUT4 = 0;
	OUT5 = 0;
	OUT6 = 0;
	OUT1 = 0;
	OUT_PWM = 0;
	OUT2 = 0;
	OUT3 = 0;

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
	unsigned int i;
	
	measurement_clock = 0;	// start a new meaurement cycle
	
	for ( i = 0; i<6 ; i++) {
		button_hold_timer[i] = 0;
		button_state[i] = BUTTON_RELEASED;
	}
	
	seconds = 0;
	minutes = 0;
	hours = 0;
	
}
 

///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom( void )
{
	unsigned char i,j;
		
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY1_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY2_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY3_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY4_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY5_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY6_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY7_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY8_SUBZONE, 0 );
	
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
// doApplicationOneSecondWork
//

void doApplicationOneSecondWork( void )
{
	//do nothing so far for Hasselt
	//No jobs to do every 1s for Hasselt
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
	int tmpval;
	unsigned char val, checksum;

	rv = 0x00; // default read

	if ( ( reg >= 2 ) && ( reg < 10 )  ) {
			
		switch ( reg ) {
		
			// Relay 0 Control
			case REG_RELAY_STATE0:
				rv = OUT1;
				break;
			
			// Relay 1 Control
			case REG_RELAY_STATE1:
				rv = OUT2;
				break;
			
			// Relay 2 Control
			case REG_RELAY_STATE2:
				rv = OUT3;
				break;
			
			// Relay 3 Control
			case REG_RELAY_STATE3:
				rv = OUT4;
				break;
			
			// Relay 4 Control
			case REG_RELAY_STATE4:
				rv = OUT5;
				break;
			
			// Relay 5 Control
			case REG_RELAY_STATE5:
				rv = OUT6;
				break;
			
			// Relay 6 Control
			case REG_RELAY_STATE6:
				rv = OUT_PWM;
				break;
			
			// Relay 7 Control
			case REG_RELAY_STATE7:
				rv = ONEWIRE;
				break;		
		}
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

	rv = ~val; // error return

	// Relay Status write?
	if ( ( reg >= 2 ) && ( reg < 10 )  ) {

		switch ( reg ) {
		
			// Relay 0 Control
			case REG_RELAY_STATE0:
				if ( 1 == val ) {
						OUT1 = 1;
						rv = 1;
				}
				break;
			
			// Relay 1 Control
			case REG_RELAY_STATE1:
				if ( 1 == val ) {
						OUT2 = 1;
						rv = 1;
					}
				break;
			
			// Relay 2 Control
			case REG_RELAY_STATE2:
				if ( 1 == val ) {
						OUT3 = 1;
						rv = 1;
					}
				break;
			
			// Relay 3 Control
			case REG_RELAY_STATE3:
				if ( 1 == val ) {
						OUT4 = 1;
						rv = 1;
					}
				break;
			
			// Relay 4 Control
			case REG_RELAY_STATE4:
				if ( 1 == val ) {
						OUT5 = 1;
						rv = 1;
					}
				break;
			
			// Relay 5 Control
			case REG_RELAY_STATE5:
				if ( 1 == val ) {
						OUT6 = 1;
						rv = 1;
					}
				break;
			
			// Relay 6 Control
			case REG_RELAY_STATE6:
				if ( 1 == val ) {
						OUT_PWM = 1;
						rv = 1;
					}
				break;
			
			// Relay 7 Control
			case REG_RELAY_STATE7:
				if ( 1 == val ) {
						ONEWIRE = 1;
						rv = 1;
					}
				break;		
		}

	}
	else if ( reg <= EEPROM_LAST_POS ) {
		writeEEPROM( VSCP_EEPROM_END + reg, val );
		rv = readEEPROM( VSCP_EEPROM_END + reg );
	}
	else if ( reg >= REG_DESCION_MATRIX ) {
		writeEEPROM( VSCP_EEPROM_END + reg, val );
		rv = readEEPROM( VSCP_EEPROM_END + reg );
	}
	
	// Send response
	vscp_sendEvent();	
	
	return rv;
	
}

///////////////////////////////////////////////////////////////////////////////
// Send Decsion Matrix Information
//

void sendDMatrixInfo( void )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;

	vscp_omsg.data[ 0 ] = DESCION_MATRIX_ELEMENTS;
	vscp_omsg.data[ 1 ] = REG_DESCION_MATRIX;

	vscp_sendEvent();	// Send data		
}


///////////////////////////////////////////////////////////////////////////////
// SendInformationEvent
//

void SendInformationEvent( unsigned char idx, unsigned char eventClass, unsigned char eventTypeId, unsigned char keyAction ) 
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.vscp_class = eventClass;
	vscp_omsg.vscp_type = eventTypeId;

	vscp_omsg.data[ 0 ] = keyAction;
	vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE );
	vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_BUTTON_SUBZONE0 + idx );

	vscp_sendEvent();	// Send data
}

///////////////////////////////////////////////////////////////////////////////
// Send Probe Ack
//

void SendProbeAck( unsigned char buttontime )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG+1;
	vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_PROBE_ACK;
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
	if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class ) return;

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

			// Check if zone should match and if so if it match
			if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  ) {
				if ( 255 != vscp_imsg.data[ 1 ] ) {
					if ( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE  ) ) {
						continue;
					}
				}	
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
				
			if ( !( ( class_filter ^ vscp_imsg.vscp_class ) & class_mask ) &&
				 	!( ( type_filter ^ vscp_imsg.vscp_type ) & type_mask )) {
					 						
				// OK Trigger this action
				switch ( readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTION  ) ) {
					
					case ACTION_ON:			// Enable relays in arg. bitarry
						doActionOn( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;
						
					case ACTION_OFF: 		// Disable relays in arg. bitarry
						doActionOff(dmflags,  readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );	
						break;

				} // case	
			} // Filter/mask
		} // Row enabled
	} // for each row	
}

///////////////////////////////////////////////////////////////////////////////
// doActionOn
// 

void doActionOn( unsigned char dmflags, unsigned char arg )
{
	unsigned char i;
	
	for ( i=0; i<6; i++ ) { 
		
		// If the output should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
															REG_RELAY1_SUBZONE + 
															i ) ) {
				continue;
			}
		}
			
		switch ( i ) {
				
			case 0:
				OUT1 = 1;
				break;

			case 1:
				OUT2 = 1;
				break;
				
			case 2:
				OUT3 = 1;
				break;
				
			case 3:
				OUT4 = 1;
				break;
				
			case 4:
				OUT5 = 1;
				break;
				
			case 5:
				OUT6 = 1;
				break;
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////
// doActionOff
// 

void doActionOff( unsigned char dmflags, unsigned char arg )
{
	unsigned char i;
	//	unsigned char val;
	
	for ( i=0; i<8; i++ ) {
		
		// If the output should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
															REG_RELAY1_SUBZONE + 
															i ) ) {
				continue;
			}
		}
			
		switch ( i ) {
								
			case 0:
				OUT1 = 0;
				break;
									
			case 1:
				OUT2 = 0;
				break;
									
			case 2:
				OUT3 = 0;
				break;
									
			case 3:
				OUT4 = 0;
				break;
									
			case 4:
				OUT5 = 0;
				break;
									
			case 5:
				OUT6 = 0;
				break;				
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
	vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

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
	return readEEPROM( EEPROM_ZONE );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getSubzone
//

uint8_t vscp_getSubzone( void )
{
	return readEEPROM( EEPROM_SUBZONE );
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
	
	for ( i = 0; i < 8; i++ ) {
		vscp_omsg.data[ i ] = 0;
	}	
	vscp_omsg.data[ 0 ] = DESCION_MATRIX_ELEMENTS; //[KURT] added for test 
	vscp_omsg.data[ 1 ] = REG_DESCION_MATRIX; //[KURT] added for test
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


///////////////////////////////////////////////////////////////////////////////
// DebounceSwitch
//

unsigned char DebounceButton1(void)
{
BufferButton1=(BufferButton1<<1) | BUTTON1 | 0xe0;
if(BufferButton1==0xf0)return BUTTON_PRESSED;
if(BufferButton1==0xe0)return BUTTON_HOLD;
if(BufferButton1==0xe1)return BUTTON_RELEASED;
return FALSE;
}

unsigned char DebounceButton2(void)
{
BufferButton2=(BufferButton2<<1) | BUTTON2 | 0xe0;
if(BufferButton2==0xf0)return BUTTON_PRESSED;
if(BufferButton2==0xe0)return BUTTON_HOLD;
if(BufferButton2==0xe1)return BUTTON_RELEASED;
return FALSE;
}

unsigned char DebounceButton3(void)
{
BufferButton3=(BufferButton3<<1) | BUTTON3 | 0xe0;
if(BufferButton3==0xf0)return BUTTON_PRESSED;
if(BufferButton3==0xe0)return BUTTON_HOLD;
if(BufferButton3==0xe1)return BUTTON_RELEASED;
return FALSE;
}

unsigned char DebounceButton4(void)
{
BufferButton4=(BufferButton4<<1) | BUTTON4 | 0xe0;
if(BufferButton4==0xf0)return BUTTON_PRESSED;
if(BufferButton4==0xe0)return BUTTON_HOLD;
if(BufferButton4==0xe1)return BUTTON_RELEASED;
return FALSE;
}

unsigned char DebounceButton5(void)
{
BufferButton5=(BufferButton5<<1) | BUTTON5 | 0xe0;
if(BufferButton5==0xf0)return BUTTON_PRESSED;
if(BufferButton5==0xe0)return BUTTON_HOLD;
if(BufferButton5==0xe1)return BUTTON_RELEASED;
return FALSE;
}

unsigned char DebounceButton6(void)
{
BufferButton6=(BufferButton6<<1) | BUTTON6 | 0xe0;
if(BufferButton6==0xf0)return BUTTON_PRESSED;
if(BufferButton6==0xe0)return BUTTON_HOLD;
if(BufferButton6==0xe1)return BUTTON_RELEASED;
return FALSE;
}