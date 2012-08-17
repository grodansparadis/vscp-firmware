/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Smart Relay Module
 * 	Version: See project header
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2011 Ake Hedman, eurosource
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
#include "../../../../src/vscp/common/vscp_class.h"	
#include "../../../../src/vscp/common/vscp_type.h"	
#include "version.h"
#include "../../paris_relay/project/smartrelay.h"
#include "main.h" 


#if defined(RELEASE)

#pragma config WDT = ON, WDTPS = 128
#pragma config OSC = HSPLL
#if defined(__18F248) || defined(__18F258)
#pragma config PWRT = ON
#pragma config STVR = ON
#pragma config BORV = 27
#endif
#if defined(__18F2480) || defined(__18F2580)
#pragma config BOREN = BOACTIVE
#pragma config STVREN = ON
#pragma config BORV = 3
#endif

#pragma config LVP = ON
#pragma config CPB = ON

#else

#pragma config WDT = OFF
#pragma config OSC = HSPLL
#pragma config PWRT = ON
#if defined(__18F248) || defined(__18F258)
#pragma config STVR = ON
#pragma config BORV = 27
#endif
#if defined(__18F2480) || defined(__18F2580)
#pragma config BOREN = BOACTIVE
#pragma config STVREN = ON
#pragma config BORV = 3
#endif

#pragma config LVP = OFF
#pragma config CPB = OFF

#endif

// Startup code from c018.c
void _startup (void);

// ISR Routines
void isr_low( void );

extern const rom int log_Table[];			// log10 for Hummidity measurement

// The device URL (max 32 characters including null termination)
const rom char vscp_deviceURL[] = "www.eurosource.se/paris_001.xml";

volatile unsigned long measurement_clock;	// Clock for measurments

BYTE seconds;								// counter for seconds
BYTE minutes;								// counter for minutes
BYTE hours;									// Counter for hours

// bit 0 - Relay 0 etc
// Bit set for pulsed relay
unsigned char relay_pulse_flags;

unsigned short relay_pulse_timer[8];
unsigned short relay_protection_timer[8];


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
		}

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
	BYTE ttt;

	// Initialize data
	init_app_ram();

	// Initialize the uP

	// PortA
	// RA0 - Relay 7 - Output
	// RA1 - Relay 6 - Output
	// RA2 - Relay 5 - Output
	// RA3 - Relay 4 - Output
	// RA4 - Relay 3 - Output
	// RA5 - Relay 2 - Output
	// RA6 - OSC
	TRISA = 0b11000000;                        

	// PortB
	// RB0 - INT0   - Input -- 
	// RB1 - INT1   - Input -- 
	// RB2 - CAN TX - output --
	// RB3 - CAN RX - input  -- 
	// RB4 -  		- input  -- 
	// RB5 - 		- input  -- 
	// RB6 - 		- input  -- 
	// RB7 - 		- input  -- 
	TRISB = 0b11111011;
	
	// RC0 - Input  - Init button
	// RC1 - Output - Status LED - Default off
    // RC2 - Input  - 
	// RC3 - Input  - 
	// RC4 - Input  -
	// RC5 - Output	- Relay 7 
	// RC6 - Input  -
	// RC7 - Output - Relay 0
	TRISC = 0b01011101;
	
	PORTCbits.RC1 = 0;	// Status LED off	

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
	PORTCbits.RC7 = 0;
	PORTAbits.RA5 = 0;
	PORTAbits.RA4 = 0;
	PORTAbits.RA3 = 0;
	PORTAbits.RA2 = 0;
	PORTAbits.RA1 = 0;
	PORTAbits.RA0 = 0;
	PORTCbits.RC5 = 0;

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
	
	seconds = 0;
	minutes = 0;
	hours = 0;
	
	relay_pulse_flags = 0;	// No pulse outputs yet
	
	// Clear timers
	for ( i=0; i<8; i++ ) {
		relay_pulse_timer[ i ] = 0;
		relay_protection_timer[ i ] = 0;
	}
}
 

///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom( void )
{
	unsigned char i,j;
	
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0, RELAY_CONTROLBIT_ONEVENT | 
										RELAY_CONTROLBIT_OFFEVENT |
										RELAY_CONTROLBIT_ENABLED );

	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL1, RELAY_CONTROLBIT_ONEVENT | 
										RELAY_CONTROLBIT_OFFEVENT |
										RELAY_CONTROLBIT_ENABLED );

	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL2, RELAY_CONTROLBIT_ONEVENT | 
										RELAY_CONTROLBIT_OFFEVENT |
										RELAY_CONTROLBIT_ENABLED );

	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL3, RELAY_CONTROLBIT_ONEVENT | 
										RELAY_CONTROLBIT_OFFEVENT |
										RELAY_CONTROLBIT_ENABLED );

	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL4, RELAY_CONTROLBIT_ONEVENT | 
										RELAY_CONTROLBIT_OFFEVENT |
										RELAY_CONTROLBIT_ENABLED );
										
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL5, RELAY_CONTROLBIT_ONEVENT | 
										RELAY_CONTROLBIT_OFFEVENT |
										RELAY_CONTROLBIT_ENABLED );

	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL6, RELAY_CONTROLBIT_ONEVENT | 
										RELAY_CONTROLBIT_OFFEVENT |
										RELAY_CONTROLBIT_ENABLED );

	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL7, RELAY_CONTROLBIT_ONEVENT | 
										RELAY_CONTROLBIT_OFFEVENT |
										RELAY_CONTROLBIT_ENABLED );
	
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB0, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB0, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB1, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB1, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB2, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB2, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB3, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB3, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB4, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB4, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB5, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB5, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB6, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB6, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB7, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB7, 0 );
	
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB0, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB0, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB1, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB1, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB2, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB2, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB3, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB3, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB4, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB4, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB5, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB5, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB6, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB6, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB7, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB7, 0 );
	
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
    uint8_t i;
    uint8_t ctrlreg;	// Current control register
	BOOL bOn;
    
    for ( i=0; i<8; i++ ) {
					
        // Get control register for this relay
		ctrlreg = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i );
					
		// If not enabled check next
		if ( !( ctrlreg & RELAY_CONTROLBIT_ENABLED ) ) continue;
										
	    if ( relay_protection_timer[ i ] ) {
						
		    relay_protection_timer[ i ]--;
						
			// Check if its time to act on protection time
			if ( !relay_protection_timer[ i ] && 
				( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i ) & RELAY_CONTROLBIT_PROTECTION ) ) {
	
			    // Yes - its time to protect this relay
				doActionOff( 0, ( 1 << i ) );
							
				// Should alarm be sent?
				if ( ctrlreg & RELAY_CONTROLBIT_ALARM ) {
				    SendInformationEvent( i, VSCP_CLASS1_ALARM, VSCP_TYPE_ALARM_ALARM );
				}
							
			}	
		}
					
		// Handle pulsed relays
		if( relay_pulse_flags & ( 1 << i ) ) {
						
		if ( relay_pulse_timer[ i ] ) {
							
		    relay_pulse_timer[ i ]--;
							
			// If zero its time for state change
			if ( !relay_pulse_timer[ i ] ) {
								
			    switch ( i ) {
									
				    case 0:
					    if ( PORTCbits.RC7 ) {
						    PORTCbits.RC7 = 0;
							bOn=FALSE;
						}
						else {
							PORTCbits.RC7 = 1;
							bOn=TRUE;
						}
						break;
										
					case 1:
					    if ( PORTAbits.RA5 ) { 
						    PORTAbits.RA5 = 0;
							bOn=FALSE;
						}
						else {
						    PORTAbits.RA5 = 1;
						    bOn=TRUE;
						}
						break;
									
					case 2:
					    if ( PORTAbits.RA4 ) {
						    PORTAbits.RA4 = 0;
							bOn=FALSE;
						}
						else {
							PORTAbits.RA4 = 1;
							bOn=TRUE;
						}
						break;
									
					case 3:
					    if ( PORTAbits.RA3 ) {
						    PORTAbits.RA3 = 0;
							bOn=FALSE;
						}
						else {
						    PORTAbits.RA3 = 1;
							bOn=TRUE;
						}
						break;
									
					case 4:
					    if ( PORTAbits.RA2 ) {
						    PORTAbits.RA2 = 0;
							bOn=FALSE;
						}
						else {
							PORTAbits.RA2 = 1;
							bOn=TRUE;
						}
						break;
									
					case 5:
						if ( PORTAbits.RA1 ) {
							PORTAbits.RA1 = 0;
							bOn=FALSE;
						}
						else {
							PORTAbits.RA1 = 1;
							bOn=TRUE;
						}
						break;				
									
					case 6:
					    if ( PORTAbits.RA0 ) {
							PORTAbits.RA0 = 0;
							bOn=FALSE;
						}
						else {
							PORTAbits.RA0 = 1;
							bOn=TRUE;
						}
						break;
									
					case 7:
						if ( PORTCbits.RC5 ) {
						    PORTCbits.RC5 = 0;
							bOn=FALSE;
						}
						else {
						    PORTCbits.RC5 = 1;
							bOn=TRUE;
						}
						break;
					}
								
					// Reload timer
					relay_pulse_timer[ i ] = 
					    readEEPROM( VSCP_EEPROM_END + 
					        REG_RELAY_PULSE_TIME_MSB0 + 2 * i ) * 
					        256 +
							readEEPROM( VSCP_EEPROM_END + 
							REG_RELAY_PULSE_TIME_LSB0 + 2 * i );
							
				    if ( bOn ) {	
			
				        if ( ctrlreg & RELAY_CONTROLBIT_ONEVENT ) {
						    SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );
						}
			
					    if ( ctrlreg & RELAY_CONTROLBIT_STARTEVENT ) {
					        SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_START );
						}
			
					}
					else {
		
					    if ( ctrlreg & RELAY_CONTROLBIT_OFFEVENT ) {
						    SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );
						}
		
						if ( ctrlreg & RELAY_CONTROLBIT_STOPEVENT ) {
						    SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_STOP );
						}
					}
															
				} // State change
							
			} // Something to count down	
																	
		} // Pulse bit
					
	} // for all relays
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
// vscp_writeAppReg
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
			case 0x02:
				rv = PORTCbits.RC7;
				break;
			
			// Relay 1 Control
			case 0x03:
				rv = PORTAbits.RA5;
				break;
			
			// Relay 2 Control
			case 0x04:
				rv = PORTAbits.RA4;
				break;
			
			// Relay 3 Control
			case 0x05:
				rv = PORTAbits.RA3;
				break;
			
			// Relay 4 Control
			case 0x06:
				rv = PORTAbits.RA2;
				break;
			
			// Relay 5 Control
			case 0x07:
				rv = PORTAbits.RA1;
				break;
			
			// Relay 6 Control
			case 0x08:
				rv = PORTAbits.RA0;
				break;
			
			// Relay 7 Control
			case 0x09:
				rv = PORTCbits.RC5;
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
	uint8_t i;
	int tmpval;
	uint8_t checksum;
	BOOL bInfoEvent = FALSE;
	BOOL bOn = FALSE;

	rv = ~val; // error return

	// Relay Status write?
	if ( ( reg >= 2 ) && ( reg < 10 )  ) {

		if ( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + reg - 2 ) & RELAY_CONTROLBIT_ENABLED ) {

			switch ( reg ) {
		
				// Relay 0 Control
				case REG_RELAY_STATE0:
					bInfoEvent = TRUE;
					if ( 1 == val ) {
						PORTCbits.RC7 = 1;
						rv = 1;
						bOn = TRUE;
						relay_pulse_flags &= 0b11111110;
						
						if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 ) & RELAY_CONTROLBIT_PROTECTION ) {
							relay_protection_timer[ 0 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB0 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB0 );
						}
					
					}
					else if ( 0 == val ) {
						PORTCbits.RC7 = 0;
						rv = 0;
						bOn = FALSE;
						relay_pulse_flags &= 0b11111110;
					}
					else if ( 2 == val ) {
						PORTCbits.RC7 = 0;			// Start out at a known state
						rv = 2;
						bOn = FALSE;
						
						relay_pulse_flags |= 0x01;	// Enable pulse output	
						relay_pulse_timer[ 0 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB0 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB0 );					
					}
					break;
			
				// Relay 1 Control
				case REG_RELAY_STATE1:
					bInfoEvent = TRUE;
					if ( 1 == val ) {
						PORTAbits.RA5 = 1;
						rv = 1;
						bOn = TRUE;
						relay_pulse_flags &= 0b11111101;
						
						if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL1 ) & RELAY_CONTROLBIT_PROTECTION ) {
							relay_protection_timer[ 1 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB1 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB1 );
						}

					}
					else if ( 0 == val ) {
						PORTAbits.RA5 = 0;
						rv = 0;
						bOn = FALSE;
						relay_pulse_flags &= 0b11111101;
					}
					else if ( 2 == val ) {
						PORTAbits.RA5 = 0;			// Start out at a known state
						rv = 2;
						bOn = FALSE;
						relay_pulse_flags |= 0x02;	// Enable pulse output	
						
						relay_pulse_timer[ 1 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB1 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB1 );		
					}
					break;
			
				// Relay 2 Control
				case REG_RELAY_STATE2:
					bInfoEvent = TRUE;
					if ( 1 == val ) {
						PORTAbits.RA4 = 1;
						rv = 1;
						bOn = TRUE;
						relay_pulse_flags &= 0b11111011;
						
						if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL2 ) & RELAY_CONTROLBIT_PROTECTION ) {
							relay_protection_timer[ 2 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB2 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB2 );
						}
					
					}
					else if ( 0 == val ) {
						PORTAbits.RA4 = 0;
						rv = 0;
						bOn = FALSE;
						relay_pulse_flags &= 0b11111011;
						
					}
					else if ( 2 == val ) {
						PORTAbits.RA4 = 0;			// Start out at a known state
						rv = 2;
						bOn = FALSE;
						relay_pulse_flags |= 0x04;	// Enable pulse output	
						
						relay_pulse_timer[ 2 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB2 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB2 );		
					}
					break;
			
				// Relay 3 Control
				case REG_RELAY_STATE3:
					bInfoEvent = TRUE;
					if ( 1 == val ) {
						PORTAbits.RA3 = 1;
						rv = 1;
						bOn = TRUE;
						relay_pulse_flags &= 0b11110111;
						
						if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL3 ) & RELAY_CONTROLBIT_PROTECTION ) {
							relay_protection_timer[ 3 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB3 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB3 );
						}
					
					}
					else if ( 0 == val ) {
						PORTAbits.RA3 = 0;
						rv = 0;
						bOn = FALSE;
						relay_pulse_flags &= 0b11110111;
						
				
					}
					else if ( 2 == val ) {
						PORTAbits.RA3 = 0;			// Start out at a known state
						rv = 2;
						bOn = FALSE;
						relay_pulse_flags |= 0x08;	// Enable pulse output
						
						relay_pulse_timer[ 3 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB3 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB3 );			
					}
					break;
			
				// Relay 4 Control
				case REG_RELAY_STATE4:
					bInfoEvent = TRUE;
					if ( 1 == val ) {
						PORTAbits.RA2 = 1;
						rv = 1;
						bOn = TRUE;
						relay_pulse_flags &= 0b11101111;
						
						if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL4 ) & RELAY_CONTROLBIT_PROTECTION ) {
							relay_protection_timer[ 4 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB4 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB4 );
						}
					
					}
					else if ( 0 == val ) {
						PORTAbits.RA2 = 0;
						rv = 0;
						bOn = FALSE;
						relay_pulse_flags &= 0b11101111;
						
					}
					else if ( 2 == val ) {
						PORTAbits.RA2 = 0;			// Start out at a known state
						rv = 2;
						bOn = FALSE;
						relay_pulse_flags |= 0x10;	// Enable pulse output	
						
						relay_pulse_timer[ 4 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB4 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB4 );					
					}
					break;
			
				// Relay 5 Control
				case REG_RELAY_STATE5:
					bInfoEvent = TRUE;
					if ( 1 == val ) {
						PORTAbits.RA1 = 1;
						rv = 1;
						bOn = TRUE;
						relay_pulse_flags &= 0b11011111;
						
						if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL5 ) & RELAY_CONTROLBIT_PROTECTION ) {
							relay_protection_timer[ 5 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB5 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB5 );
						}
					
					}
					else if ( 0 == val ) {
						PORTAbits.RA1 = 0;
						rv = 0;
						bOn = FALSE;
						relay_pulse_flags &= 0b11011111;
					}
					else if ( 2 == val ) {
						PORTAbits.RA1 = 0;			// Start out at a known state
						rv = 2;
						bOn = FALSE;
						relay_pulse_flags |= 0x20;	// Enable pulse output	
						
						relay_pulse_timer[ 5 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB5 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB5 );		
					}
					break;
			
				// Relay 6 Control
				case REG_RELAY_STATE6:
					bInfoEvent = TRUE;
					if ( 1 == val ) {
						PORTAbits.RA0 = 1;
						rv = 1;
						bOn = TRUE;
						relay_pulse_flags &= 0b10111111;
						
						if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL6 ) & RELAY_CONTROLBIT_PROTECTION ) {
							relay_protection_timer[ 6 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB6 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB6 );
						}

					}
					else if ( 0 == val ) {
						PORTAbits.RA0 = 0;
						rv = 0;
						bOn = FALSE;
						relay_pulse_flags &= 0b10111111;
		
					}
					else if ( 2 == val ) {
						PORTAbits.RA0 = 0;			// Start out at a known state
						rv = 2;
						bOn = FALSE;
						relay_pulse_flags |= 0x40;	// Enable pulse output	
						
						relay_pulse_timer[ 6 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB6 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB6 );		
					}
					break;
			
				// Relay 7 Control
				case REG_RELAY_STATE7:
					bInfoEvent = TRUE;
					if ( 1 == val ) {
						PORTCbits.RC5 = 1;
						rv = 1;
						bOn = TRUE;
						relay_pulse_flags &= 0b01111111;
						
						if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL7 ) & RELAY_CONTROLBIT_PROTECTION ) {
							relay_protection_timer[ 7 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB7 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB7 );
						}

					}
					else if ( 0 == val ) {
						PORTCbits.RC5 = 0;
						rv = 0;
						bOn = FALSE;
						relay_pulse_flags &= 0b01111111;	// Disable pulse output
						

					}
					else if ( 2 == val ) {
						
						PORTCbits.RC5 = 0;			// Start out at a known state
						rv = 2;
						bOn = FALSE;
						relay_pulse_flags |= 0x80;	// Enable pulse output	
						
						relay_pulse_timer[ 7 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB7 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB7 );		
					}
					break;		
			}
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
	
	// Sned information Event 
	// if enabled in the registers
	if ( bInfoEvent ) {
		
		unsigned char val;		
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + reg - 2 );
		
		if ( bOn ){	
			
			if ( val & RELAY_CONTROLBIT_ONEVENT ) {
				SendInformationEvent( reg - 2, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );
			}
			
			if( val & RELAY_CONTROLBIT_STARTEVENT ) {
				SendInformationEvent( reg - 2, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_START );
			}
			
		}
		else {
		
			if( val & RELAY_CONTROLBIT_OFFEVENT ) {
				SendInformationEvent( reg - 2, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );
			}
		
			if( val & RELAY_CONTROLBIT_STOPEVENT ) {
				SendInformationEvent( reg - 2, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_STOP );
			}
		}
	}
	
	return rv;
	
}

///////////////////////////////////////////////////////////////////////////////
// Send Decsion Matrix Information
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
// SendInformationEvent
//

void SendInformationEvent( unsigned char idx, unsigned char eventClass, unsigned char eventTypeId ) 
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.class = eventClass;
	vscp_omsg.type = eventTypeId;

	vscp_omsg.data[ 0 ] = idx;	// Register
	vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE );
	vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_RELAY1_SUBZONE + idx );

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
				
			if ( !( ( class_filter ^ vscp_imsg.class ) & class_mask ) &&
				 	!( ( type_filter ^ vscp_imsg.type ) & type_mask )) {
					 						
				// OK Trigger this action
				switch ( readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTION  ) ) {
					
					case ACTION_ON:			// Enable relays in arg. bitarry
						doActionOn( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;
						
					case ACTION_OFF: 		// Disable relays in arg. bitarry
						doActionOff(dmflags,  readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );	
						break;
						
					case ACTION_PULSE:		// Pulse relays in arg. bitarry, zone, subzone
						doActionPulse( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;
						
					case ACTION_STATUS:		// Send status for all relays
						doActionStatus( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;
						
					case ACTION_DISABLE:	// Disable realys in bitarray
						doActionDisable( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;

					case ACTION_TOGGLE:	// Toggle relay(s)
						doActionToggle( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
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
	unsigned char val;
	
	for ( i=0; i<8; i++ ) {
		
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
															REG_RELAY1_SUBZONE + 
															i ) ) {
				continue;
			}
		}
			
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i );
		
		// Do nothing if disabled
		if ( !( val & RELAY_CONTROLBIT_ENABLED ) ) continue;
		
		switch ( i ) {
								
			case 0:
				PORTCbits.RC7 = 1;
				relay_pulse_flags &= 0b11111110;
				
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 0 ] = 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB0 ) * 256 + 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB0 );
				}
				break;
									
			case 1:
				PORTAbits.RA5 = 1;
				relay_pulse_flags &= 0b11111101;
				
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL1 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 1 ] = 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB1 ) * 256 + 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB1 );
				}
				break;
									
			case 2:
				PORTAbits.RA4 = 1;
				relay_pulse_flags &= 0b11111011;
				
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL2 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 2 ] = 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB2 ) * 256 + 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB2 );
				}
				break;
									
			case 3:
				PORTAbits.RA3 = 1;
				relay_pulse_flags &= 0b11110111;
				
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL3 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 3 ] = 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB3 ) * 256 + 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB3 );
				}
				break;
									
			case 4:
				PORTAbits.RA2 = 1;
				relay_pulse_flags &= 0b11101111;
				
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL4 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 4 ] = 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB4 ) * 256 + 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB4 );
				}
				break;
									
			case 5:
				PORTAbits.RA1 = 1;
				relay_pulse_flags &= 0b11011111;
				
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL5 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 5 ] = 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB5 ) * 256 + 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB5 );
				}
				break;				
									
			case 6:
				PORTAbits.RA0 = 1;
				relay_pulse_flags &= 0b10111111;
				
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL6 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 6 ] = 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB6 ) * 256 + 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB6 );
				}
				break;
									
			case 7:
				PORTCbits.RC5 = 1;
				relay_pulse_flags &= 0b01111111;
						
				if( readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL7 ) & RELAY_CONTROLBIT_PROTECTION ) {
					relay_protection_timer[ 7 ] = 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_MSB7 ) * 256 + 
								readEEPROM( VSCP_EEPROM_END + REG_RELAY_PROTECTION_TIME_LSB7 );
				}
				break;		
		}
									
		// Should off event be sent?
		if( val & RELAY_CONTROLBIT_ONEVENT ) {
			SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );			
		}
		
		// Should stop event be sent?
		if( val & RELAY_CONTROLBIT_STARTEVENT ) {
			SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_START );
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////
// doActionOff
// 

void doActionOff( unsigned char dmflags, unsigned char arg )
{
	unsigned char i;
	unsigned char val;
	
	for ( i=0; i<8; i++ ) {
		
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
															REG_RELAY1_SUBZONE + 
															i ) ) {
				continue;
			}
		}
		
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i );
		
		// Do nothing if disabled
		if ( !( val & RELAY_CONTROLBIT_ENABLED ) ) continue;
		
		switch ( i ) {
								
			case 0:
				PORTCbits.RC7 = 0;
				relay_pulse_flags &= 0b11111110;	// Disable pulse output
				break;
									
			case 1:
				PORTAbits.RA5 = 0;
				relay_pulse_flags &= 0b11111101;	// Disable pulse output
				break;
									
			case 2:
				PORTAbits.RA4 = 0;
				relay_pulse_flags &= 0b11111011;	// Disable pulse output
				break;
									
			case 3:
				PORTAbits.RA3 = 0;
				relay_pulse_flags &= 0b11110111;	// Disable pulse output
				break;
									
			case 4:
				PORTAbits.RA2 = 0;
				relay_pulse_flags &= 0b11101111;	// Disable pulse output
				break;
									
			case 5:
				PORTAbits.RA1 = 0;
				relay_pulse_flags &= 0b11011111;	// Disable pulse output
				break;				
									
			case 6:
				PORTAbits.RA0 = 0;
				relay_pulse_flags &= 0b10111111;	// Disable pulse output
				break;
									
			case 7:
				PORTCbits.RC5 = 0;
				relay_pulse_flags &= 0b01111111;	// Disable pulse output
				break;		
		}
									
		// Should off event be sent?
		if( val & RELAY_CONTROLBIT_OFFEVENT ) {
			SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );			
		}
		
		// Should stop event be sent?
		if( val & RELAY_CONTROLBIT_STOPEVENT ) {
			SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_STOP );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// doActionPulse
// 

void doActionPulse( unsigned char dmflags, unsigned char arg )
{
	unsigned char i;
	unsigned char val;
	
	for ( i=0; i<8; i++ ) {
		
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
															REG_RELAY1_SUBZONE + 
															i ) ) {
				continue;
			}
		}
		
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i );
		
		// Do nothing if disabled
		if ( !( val & RELAY_CONTROLBIT_ENABLED ) ) continue;
		
		switch ( i ) {
								
			case 0:
				PORTCbits.RC7 = 0;			// Start out at a known state
				relay_pulse_flags |= 0x01;	// Enable pulse output	
				relay_pulse_timer[ 0 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB0 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB0 );
				break;
									
			case 1:
				PORTAbits.RA5 = 0;			// Start out at a known state
				relay_pulse_flags |= 0x02;	// Enable pulse output	
				relay_pulse_timer[ 1 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB1 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB1 );
				break;
									
			case 2:
				PORTAbits.RA4 = 0;			// Start out at a known state
				relay_pulse_flags |= 0x04;	// Enable pulse output	
				relay_pulse_timer[ 2 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB2 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB2 );
				break;
									
			case 3:
				PORTAbits.RA3 = 0;			// Start out at a known state
				relay_pulse_flags |= 0x08;	// Enable pulse output	
				relay_pulse_timer[ 3 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB3 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB3 );
				break;
									
			case 4:
				PORTAbits.RA2 = 0;			// Start out at a known state
				relay_pulse_flags |= 0x10;	// Enable pulse output	
				relay_pulse_timer[ 4 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB4 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB4 );
				break;
									
			case 5:
				PORTAbits.RA1 = 0;			// Start out at a known state
				relay_pulse_flags |= 0x20;	// Enable pulse output	
				relay_pulse_timer[ 5 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB5 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB5 );
				break;				
									
			case 6:
				PORTAbits.RA0 = 0;			// Start out at a known state
				relay_pulse_flags |= 0x40;	// Enable pulse output	
				relay_pulse_timer[ 6 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB6 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB6 );
				break;
									
			case 7:
				PORTCbits.RC5 = 0;			// Start out at a known state
				relay_pulse_flags |= 0x80;	// Enable pulse output	
				relay_pulse_timer[ 7 ] = 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_MSB7 ) * 256 + 
									readEEPROM( VSCP_EEPROM_END + REG_RELAY_PULSE_TIME_LSB7 );
				break;		
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////
// doActionStatus
// 

void doActionStatus( unsigned char dmflags, unsigned char arg )
{
	unsigned char i;
	unsigned char val;
	BOOL bOn = FALSE;
	
	for ( i=0; i<8; i++ ) {
		
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
															REG_RELAY1_SUBZONE + 
															i ) ) {
				continue;
			}
		}
		
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i );
		
		switch ( i ) {
								
			case 0:
				bOn = (BOOL)PORTCbits.RC7;
				break;
									
			case 1:
				bOn = (BOOL)PORTAbits.RA5;
				break;
									
			case 2:
				bOn = (BOOL)PORTAbits.RA4;
				break;
									
			case 3:
				bOn = (BOOL)PORTAbits.RA3;
				break;
									
			case 4:
				bOn = (BOOL)PORTAbits.RA2;
				break;
									
			case 5:
				bOn = (BOOL)PORTAbits.RA1;
				break;				
									
			case 6:
				bOn = (BOOL)PORTAbits.RA0;
				break;
									
			case 7:
				bOn = (BOOL)PORTCbits.RC5;
				break;		
		}
		
		if ( bOn ) {
		
			// Should off event be sent?
			if( val & RELAY_CONTROLBIT_OFFEVENT ) {
				SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );			
			}
		
			// Should stop event be sent?
			if( val & RELAY_CONTROLBIT_STOPEVENT ) {
				SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_STOP );
			}
		
		}
		else {
										
			// Should off event be sent?
			if( val & RELAY_CONTROLBIT_OFFEVENT ) {
				SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );			
			}
		
			// Should stop event be sent?
			if( val & RELAY_CONTROLBIT_STOPEVENT ) {
				SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_STOP );
			}
			
		}
		
	}	
}

///////////////////////////////////////////////////////////////////////////////
// doActionDisable
// 

void doActionDisable( unsigned char dmflags, unsigned char arg )
{
	unsigned char i;
	unsigned char val;
	BOOL bOn = FALSE;
	
	for ( i=0; i<8; i++ ) {
		
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
															REG_RELAY1_SUBZONE + 
															i ) ) {
				continue;
			}
		}
		
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i );
		writeEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i, val & ~RELAY_CONTROLBIT_ENABLED  );
	}
		
}

///////////////////////////////////////////////////////////////////////////////
// doActionToggle
// 

void doActionToggle( unsigned char dmflags, unsigned char arg )
{
	unsigned char i;
	unsigned char val;
	BOOL bOn = FALSE;
	
	for ( i=0; i<8; i++ ) {
		
		// If the relay should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
															REG_RELAY1_SUBZONE + 
															i ) ) {
				continue;
			}
		}
		
		val = readEEPROM( VSCP_EEPROM_END + REG_RELAY_CONTROL0 + i );
		
		switch ( i ) {
								
			case 0:
				if ((BOOL)PORTCbits.RC7){
					PORTCbits.RC7 = 0;
					bOn = FALSE;
				}
				else  {
					PORTCbits.RC7 = 1;
					bOn = TRUE;
				} 
				break;
									
			case 1:
				if ((BOOL)PORTAbits.RA5){
					PORTAbits.RA5 = 0;
					bOn = FALSE;
				}
				else  {
					PORTAbits.RA5 = 1;
					bOn = TRUE;
				} 
				break;
									
			case 2:
				if ((BOOL)PORTAbits.RA4){
					PORTAbits.RA4 = 0;
					bOn = FALSE;
				}
				else  {
					PORTAbits.RA4 = 1;
					bOn = TRUE;
				} 
				break;
									
			case 3:
				if ((BOOL)PORTAbits.RA3){
					PORTAbits.RA3 = 0;
					bOn = FALSE;
				}
				else  {
					PORTAbits.RA3 = 1;
					bOn = TRUE;
				} 
				break;
									
			case 4:
				if ((BOOL)PORTAbits.RA2){
					PORTAbits.RA2 = 0;
					bOn = FALSE;
				}
				else  {
					PORTAbits.RA2 = 1;
					bOn = TRUE;
				} 
				break;
									
			case 5:
				if ((BOOL)PORTAbits.RA1){
					PORTAbits.RA1 = 0;
					bOn = FALSE;
				}
				else  {
					PORTAbits.RA1 = 1;
					bOn = TRUE;
				} 
				break;				
									
			case 6:
				if ((BOOL)PORTAbits.RA0){
					PORTAbits.RA0 = 0;
					bOn = FALSE;
				}
				else  {
					PORTAbits.RA0 = 1;
					bOn = TRUE;
				} 
				break;
									
			case 7:
				if ((BOOL)PORTCbits.RC5){
					PORTCbits.RC5 = 0;
					bOn = FALSE;
				}
				else  {
					PORTCbits.RC5 = 1;
					bOn = TRUE;
				} 
				break;		
		}
		
		if ( bOn ) {
		
			// Should on event be sent?
			if( val & RELAY_CONTROLBIT_ONEVENT ) {
				SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );			
			}
		
			// Should start event be sent?
			if( val & RELAY_CONTROLBIT_STARTEVENT ) {
				SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_START );
			}
		
		}
		else {
										
			// Should off event be sent?
			if( val & RELAY_CONTROLBIT_OFFEVENT ) {
				SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );			
			}
		
			// Should stop event be sent?
			if( val & RELAY_CONTROLBIT_STOPEVENT ) {
				SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_STOP );
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
