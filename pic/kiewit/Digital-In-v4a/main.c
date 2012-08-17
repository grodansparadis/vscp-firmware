/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Button Module
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
#include "button.h"
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

// The device URL (max 32 characters including null termination)
// Don't forget to add vscphost into your host file with his corresponding IP-adress
const uint8_t vscp_deviceURL[] = "vscphost/DIV4a.xml";

volatile unsigned long measurement_clock;		// Clock for measurments
volatile unsigned long ten_ms_clock;   	    	// Clock for application/debouncing
volatile unsigned long hundred_ms_clock;   	    // Clock for application
volatile unsigned long one_s_clock;    	    	// Clock for application
volatile unsigned long ten_s_clock;   	        // Clock for application
volatile unsigned long hundred_s_clock;   	    // Clock for application
volatile unsigned long thousand_s_clock;   	    // Clock for application
volatile unsigned long tenthousand_s_clock;   	// Clock for application

/*
BYTE seconds;								// counter for seconds
BYTE minutes;								// counter for minutes
BYTE hours;									// Counter for hours
*/

unsigned char bvalues[15];
unsigned char buttons_state[15] = { BUTTON_STATE_RELEASED,BUTTON_STATE_RELEASED,BUTTON_STATE_RELEASED,
									BUTTON_STATE_RELEASED,BUTTON_STATE_RELEASED,BUTTON_STATE_RELEASED,
									BUTTON_STATE_RELEASED,BUTTON_STATE_RELEASED,BUTTON_STATE_RELEASED,
									BUTTON_STATE_RELEASED,BUTTON_STATE_RELEASED,BUTTON_STATE_RELEASED,
									BUTTON_STATE_RELEASED,BUTTON_STATE_RELEASED,BUTTON_STATE_RELEASED};
unsigned char button_timeout_timer[15];

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
		
/*
		if ( seconds > 59 ) {
			
	        seconds = 0;
			minutes++;
				
			if ( minutes > 59 ) {
			    minutes = 0;
				hours++;
			}
				
			if ( hours > 23 ) hours = 0;
				
		}
*/

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


		if ( ten_ms_clock > 10 ) {

            bvalues[0] = BUTTON1;
            bvalues[1] = BUTTON2;
            bvalues[2] = BUTTON3;
            bvalues[3] = BUTTON4;
            bvalues[4] = BUTTON5;
            bvalues[5] = BUTTON6;
            bvalues[6] = BUTTON7;
            bvalues[7] = BUTTON8;
            bvalues[8] = BUTTON9;
            bvalues[9] = BUTTON10;
            bvalues[10] = BUTTON11;
            bvalues[11] = BUTTON12;
            bvalues[12] = BUTTON13;
            bvalues[13] = BUTTON14;
            bvalues[14] = BUTTON15;

            for (i=0; i < 15; i++) {
		        // Get control register for this button
				ctrlreg = readEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL1 + i );
				if ( ctrlreg & BUTTON_CONTROLBIT_INVERT ) {
					if ( bvalues[i] ) { bvalues[i] = 0; }
                    else { bvalues[i] = 1; }
				}

				//execute debounce algorithm
				btransition = ButtonTransition( bvalues[i], &buttons_state[i] );

				//Do not delete this line. Otherwise BUTTON15 will not work. Strange and i don't know why.			
				if ( i == 0 ) { i = 0; }
				//

				if ( btransition && ((buttons_state[i] & BUTTON_MASK_STATE) == BUTTON_STATE_PUSHED) && 
						(ctrlreg & BUTTON_CONTROLBIT_SLAVE) && (ctrlreg & BUTTON_CONTROLBIT_PRESS) ) {
			    	SendEvent( 0x01, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, 0x00, i, 0x00, 0x00 );
				}
				if ( btransition && ((buttons_state[i] & BUTTON_MASK_STATE) == BUTTON_STATE_RELEASED) &&
						(ctrlreg & BUTTON_CONTROLBIT_SLAVE) && (ctrlreg & BUTTON_CONTROLBIT_RELEASE) ) {
		    		SendEvent( 0x00, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, 0x00, i, 0x00, 0x00 );
				}
				if ( btransition && ((buttons_state[i] & BUTTON_MASK_STATE) == BUTTON_STATE_PUSHED) &&
						!(ctrlreg & BUTTON_CONTROLBIT_SLAVE) && !(ctrlreg & BUTTON_CONTROLBIT_FLANK) ) {
		    		SendEvent( 0x02, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, 0x00, i, 0x00, 0x00 );
				}
				if ( btransition && ((buttons_state[i] & BUTTON_MASK_STATE) == BUTTON_STATE_RELEASED) &&
						!(ctrlreg & BUTTON_CONTROLBIT_SLAVE) && (ctrlreg & BUTTON_CONTROLBIT_FLANK) ) {
		    		SendEvent( 0x02, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, 0x00, i, 0x00, 0x00 );
				}
				//end debounce


				if ((buttons_state[i] & BUTTON_MASK_STATE) == BUTTON_STATE_PUSHED) {
					switch (ctrlreg & 0b01110000) {
						case 0x10: 
				    		doApplicationWork(i);
							break;
						case 0x20: 
							if ( hundred_ms_clock > 10 ) {
								hundred_ms_clock = 0;
					    		doApplicationWork(i);
							}
							else {
								hundred_ms_clock++;
							}
							break;
						case 0x30:
							if ( one_s_clock > 100 ) {
								one_s_clock = 0;
					    		doApplicationWork(i);
							}
							else {
								one_s_clock++;
							}
							break;
						case 0x40:
							if ( ten_s_clock > 1000 ) {
								ten_s_clock = 0;
					    		doApplicationWork(i);
							}
							else {
								ten_s_clock++;
							}
							break;
						case 0x50:
							if ( hundred_s_clock > 10000 ) {
								hundred_s_clock = 0;
					    		doApplicationWork(i);
							}
							else {
								hundred_s_clock++;
							}
							break;
						case 0x60:
							if ( thousand_s_clock > 100000 ) {
								thousand_s_clock = 0;
					    		doApplicationWork(i);
							}
							else {
								thousand_s_clock++;
							}
							break;
						case 0x70:
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
				if ((buttons_state[i] & BUTTON_MASK_STATE) == BUTTON_STATE_RELEASED) {
	    			button_timeout_timer[ i ] = readEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT1 + i ); 
				}
			}	
			ten_ms_clock = 0;
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
	// RA0 - Input
	// RA1 - Input
	// RA2 - Input
	// RA3 - Input
	// RA4 - Input
	// RA5 - Input
	// RA6 - OSC
	// RA7 - OSC
	TRISA = 0b11111111;
	// Configure portA as digital inputs

    ADCON1 = 0b00001111;                        

	// PortB
	// RB0 - INT0   - Input -- Init button
	// RB1 - INT1   - Input -- 
	// RB2 - CAN TX - output --
	// RB3 - CAN RX - input  -- 
	// RB4 -  		- output -- Status LED - Default off
	// RB5 - 		- input  -- 
	// RB6 - 		- input  -- 
	// RB7 - 		- input  -- 
	TRISB = 0b11101011;
	
	// RC0 - Input
	// RC1 - Input
    // RC2 - Input
	// RC3 - Input
	// RC4 - Input
	// RC5 - Input 
	// RC6 - Input
	// RC7 - Input
	TRISC = 0b11111111;
	
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
	
/*
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
	
/*
	seconds = 0;
	minutes = 0;
	hours = 0;
*/
	for ( i=0; i<15; i++ ) {
		button_timeout_timer[ i ] = 0;
	}
	
}
 

///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom( void )
{
	unsigned char i,j;
	
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL1, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT);
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL2, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL3, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL4, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL5, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL6, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL7, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL8, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL9, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL10, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL11, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL12, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL13, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL14, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_CONTROL15, BUTTON_CONTROLBIT_PRESS | BUTTON_CONTROLBIT_RELEASE | BUTTON_CONTROLBIT_SLAVE | BUTTON_CONTROLBIT_INVERT );

	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON1_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON2_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON3_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON4_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON5_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON6_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON7_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON8_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON9_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON10_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON11_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON12_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON13_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON14_SUBZONE, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON15_SUBZONE, 0 );

	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT1, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT2, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT3, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT4, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT5, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT6, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT7, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT8, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT9, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT10, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT11, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT12, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT13, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT14, 0 );
	writeEEPROM( VSCP_EEPROM_END + REG_BUTTON_TIMEOUT15, 0 );

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

}   

///////////////////////////////////////////////////////////////////////////////
// doApplicationWork
//

void doApplicationWork( unsigned char i )
{
	if ( button_timeout_timer[ i ] ) {
	    button_timeout_timer[ i ]--;
		// Check if its time to send an event
		if ( !button_timeout_timer[ i ] ) {
    		SendEvent( 0x00, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_GOOD_NIGHT, 0x00, i, 0x00, 0x00 );
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
	int tmpval;
	unsigned char val, checksum;

	rv = 0x00; // default read

	if ( reg <= EEPROM_LAST_POS ) {
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

	if ( reg <= EEPROM_LAST_POS ) {
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
// SendEvent
//

void SendEvent( unsigned char code, unsigned char eventClass, unsigned char eventTypeId, unsigned char msb1, unsigned char lsb1, unsigned char msb2, unsigned char lsb2 ) 
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 7;
	vscp_omsg.class = eventClass;
	vscp_omsg.type = eventTypeId;

	vscp_omsg.data[ 0 ] = code;
	vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + EEPROM_ZONE );
	vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_BUTTON1_SUBZONE + lsb1 );
    if ( vscp_omsg.data[ 2 ] == 0x00 ) {
		vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + EEPROM_SUBZONE );
    }
	vscp_omsg.data[ 3 ] = msb1;
	vscp_omsg.data[ 4 ] = lsb1;
	vscp_omsg.data[ 5 ] = msb2;
	vscp_omsg.data[ 6 ] = lsb2;

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
					
						
				} // case	
			} // Filter/mask
		} // Row enabled
	} // for each row	
}


///////////////////////////////////////////////////////////////////////////////
// Debounce switch and return if a transition occured (ON -> OFF or OFF -> ON)

uint8_t ButtonTransition(uint8_t button, uint8_t *button_state)
{
	uint8_t buttontransition = FALSE;

	if ( button ) {
		switch ( *button_state & BUTTON_MASK_STATE ) {
			case BUTTON_STATE_RELEASED:
				*button_state = ( *button_state & BUTTON_MASK_CLICK ) | BUTTON_STATE_TRANS1;
				break;
			case BUTTON_STATE_TRANS1:
				*button_state = ( *button_state & BUTTON_MASK_CLICK ) | BUTTON_STATE_TRANS2;
				break;
			case BUTTON_STATE_TRANS2:
				*button_state = ( *button_state & BUTTON_MASK_CLICK ) | BUTTON_STATE_TRANS3;
				break;
			case BUTTON_STATE_TRANS3:
				*button_state = ( *button_state & BUTTON_MASK_CLICK ) | BUTTON_STATE_TRANS4;
				break;
			case BUTTON_STATE_TRANS4:
				*button_state = ( *button_state & BUTTON_MASK_CLICK ) | BUTTON_STATE_PUSHED;
	            if (!(*button_state & BUTTON_MASK_CLICK)) {
					buttontransition = TRUE;
				    *button_state = BUTTON_MASK_CLICK | BUTTON_STATE_PUSHED;
				}
				break;
		}
	}
	else {
		switch ( *button_state & BUTTON_MASK_STATE ) {
			case BUTTON_STATE_PUSHED:
				*button_state = ( *button_state & BUTTON_MASK_CLICK ) | BUTTON_STATE_TRANS4;
				break;
			case BUTTON_STATE_TRANS4:
				*button_state = ( *button_state & BUTTON_MASK_CLICK ) | BUTTON_STATE_TRANS3;
				break;
			case BUTTON_STATE_TRANS3:
				*button_state = ( *button_state & BUTTON_MASK_CLICK ) | BUTTON_STATE_TRANS2;
				break;
			case BUTTON_STATE_TRANS2:
				*button_state = ( *button_state & BUTTON_MASK_CLICK ) | BUTTON_STATE_TRANS1;
				break;
			case BUTTON_STATE_TRANS1:
				*button_state = ( *button_state & BUTTON_MASK_CLICK ) | BUTTON_STATE_RELEASED;
	            if ((*button_state & BUTTON_MASK_CLICK) != 0) {
					buttontransition = TRUE;
				    *button_state = ~BUTTON_MASK_CLICK & BUTTON_STATE_RELEASED;
				}
				break;
		}
	}
	return buttontransition;
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

	// The rest set to zero no paging
	for ( i = 2; i < 8; i++ ) {
		vscp_omsg.data[ i ] = 0;
	}	

}

///////////////////////////////////////////////////////////////////////////////
//
//

uint8_t vscp_getRegisterPagesUsed( void )
{
	return 1;	// One page used
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
