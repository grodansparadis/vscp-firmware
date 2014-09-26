/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Kelvin Smart II Module
 * 	Version information in version.h
 * 	akhe@eurosource.se
 *
 * Copyright (C) 1995-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
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

#ifdef __ICCPIC18__
#else
#include <p18cxxx.h>
#endif
#include <timers.h>
#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include "../../../../common/inttypes.h"
#include "../../../common/eeprom.h"
#include "../../../common/can18xx8.h"
#include "../../../common/can18f.h"
#include "../../../../common/vscp_firmware.h"
#include "i:/src/vscp/common/vscp_class.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "i:/src/vscp/common/vscp_type.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "version.h"
#include "main.h" 
#include "smartec.h"

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


// Startup code from c018.c
void _startup (void);

// ISR Routines
void isr_low( void );

// externals from vscp.c
extern uint8_t vscp_second;
extern uint8_t vscp_minute;
extern uint8_t vscp_hour;

// The device URL (max 32 characters including null termination)
const rom uint8_t scp_deviceURL[] = "www.eurosource.se/smart_001.xml";

// Global Variable Declarations
uint8_t nSensor;							// Sensor to do next measurement on

uint8_t tarray_idx0;						// idx for next temperature write sensor 0
int tarray0[ TEMPERATURE_ARRAY_SIZE  ];		// Temperature measurement array sensor 0

uint8_t tarray_idx1;						// idx for next temperature write sensor 1
int tarray1[ TEMPERATURE_ARRAY_SIZE  ];		// Temperature measurement array sensor 1

int temperature0_array[ 24 ];				// 24-hour period mean value sensor 0
int temperature1_array[ 24 ];				// 24-hour period mean value sensor 1

uint16_t    current_temp0;                  // Current temperature sensor 1
uint16_t    current_temp1;                  // Current Temperature sensor 2

uint8_t error_cnt_tempsensor0;				// Error counter for temp1 sensor reads			
uint8_t error_cnt_tempsensor1;				// Error counter for temp2 sensor reads 

volatile uint32_t measurement_clock;		// Clock for measurments

uint8_t seconds_temperature0;				// timer for temp1 event
uint8_t seconds_temperature1;				// timer for temp2 event

// bit 7 - high temp1 alarm sent
// bit 6 - high temp2 alarm sent
// bit 5 - low temp1 alarm sent
// bit 4 - low temp2 alarm sent
uint8_t alarm_flags;		

// incremented once of each OK reading
// nulled if failure
// if 0xff resests error counts
uint8_t okreadings;

// Counter for seconds between measurements
uint8_t measurement_seconds;

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
	uint16_t tval0, tval1;
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
	
	// Time 0 - RB0
	if ( 1 ) {
	
	}	
	
	// Time 1 - RB1
	if ( 1 ) {
	
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
			measurement_seconds++;
			
			// Do VSCP one second jobs 
			vscp_doOneSecondWork();
			
			// Temperature report timers are only updated if in active 
			// state guid_reset
			if ( VSCP_STATE_ACTIVE == vscp_node_state  ) {
				
				seconds_temperature0++;
				seconds_temperature1++;
					
			}
							
			// If we have errors in sensor readings
			// send out error event
			if ( error_cnt_tempsensor0 || error_cnt_tempsensor1 ) {
				sendErrorEvent( ERROR_SENSOR );
			}
							
		}
		
		// Do a measurement every half minute
		if ( measurement_seconds > 30 ) {
            measurement_seconds = 0;
            doMeasurement();
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
	uint8_t msgdata[ 8 ];
	uint8_t ttt;

	// Initialize data
	init_app_ram();

	// Initialize the uP
	
	ADCON1 = 0x0f;	// All digital inputs - Needed by 18F2580

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

	// Sensor 0 timer
	OpenTimer0( TIMER_INT_OFF & 
					T0_16BIT & 
					T0_PS_1_1 &
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

	if ( vscp18f_sendMsg( 0x123,  msgdata, 4, CAN_TX_PRIORITY_0 & CAN_TX_XTD_FRAME & CAN_TX_NO_RTR_FRAME ) ) {
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
    uint8_t i;
    
	measurement_clock = 0;	// start a new meaurement cycle
		
	seconds_temperature0 = 0;
	seconds_temperature1 = 0;

	alarm_flags = 0;		// no alarms yet

	okreadings = 0;				
	
	error_cnt_tempsensor0 = 0;		// no errors in temp readings
	error_cnt_tempsensor1 = 0;		// no errors in temp readings

	nSensor = 0;					// start with sensor 0
	
	tarray_idx0 = 0;				// Index into temperature 0 array
	tarray_idx1 = 0;				// Index into temperature 1 array
	
	measurement_seconds = 0xff;     // Do an initial measurement
	
	for ( i = 0; i<TEMPERATURE_ARRAY_SIZE; i++ ) {
		tarray0[ i ] = 0;
		tarray1[ i ] = 0;
	}
	
	current_temp0 = current_temp1 = 0xffff; // Init temperaturesensors
}
 

///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom( void )
{
 	// Abs. max temperature set to -327 degrees
	writeEEPROM( EEPROM_ABSMAX_TEMP0, 0xFF );
	writeEEPROM( EEPROM_ABSMAX_TEMP0 + 1, 0xFF );

	writeEEPROM( EEPROM_ABSMAX_TEMP1, 0xFF );
	writeEEPROM( EEPROM_ABSMAX_TEMP1 + 1, 0xFF );

	// Abs. min temperature set to +327 degrees
	writeEEPROM( EEPROM_ABSMIN_TEMP0, 0x7F );
	writeEEPROM( EEPROM_ABSMIN_TEMP0 + 1, 0xFF );

	writeEEPROM( EEPROM_ABSMIN_TEMP1, 0x7F );
	writeEEPROM( EEPROM_ABSMIN_TEMP1 + 1, 0xFF );

	// Low temp alarm disables
	writeEEPROM( EEPROM_TEMP0_LOW_ALARMPOINT, 0xFF );
	writeEEPROM( EEPROM_TEMP1_LOW_ALARMPOINT, 0xFF );

	// High temp alarm disabled
	writeEEPROM( EEPROM_TEMP0_HIGH_ALARMPOINT, 0xFF );
	writeEEPROM( EEPROM_TEMP1_HIGH_ALARMPOINT, 0xFF );

	// Send temp event once every 30 seconds
	writeEEPROM( EEPROM_INTERVAL_TEMP0, 0x1E );
	writeEEPROM( EEPROM_INTERVAL_TEMP1, 0x1E );

	// Unit is Celsius
	writeEEPROM( EEPROM_TEMP_UNIT, TEMP_UNIT_CELSIUS );

	// Temp calibration value is zero
	writeEEPROM( EEPROM_TEMP0_CALIBRATION, 0x00 );
	writeEEPROM( EEPROM_TEMP0_CALIBRATION + 1, 0x00 );

	writeEEPROM( EEPROM_TEMP1_CALIBRATION, 0x00 );
	writeEEPROM( EEPROM_TEMP1_CALIBRATION + 1, 0x00 );
	
	// Temp hysteresis = 5 
	writeEEPROM( EEPROM_TEMP0_HYSTERESIS, 0x05 );
	writeEEPROM( EEPROM_TEMP1_HYSTERESIS, 0x05 );
	
	// Zone/Subzne
	writeEEPROM( EEPROM_ZONE, 0 );
	writeEEPROM( EEPROM_SUBZONE, 0 );
	
	// Default GUID (if not programmed)
	writeEEPROM( VSCP_EEPROM_REG_GUID, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 1, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 2, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 3, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 4, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 5, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 6, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 7, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 8, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 9, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 10, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 11, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 12, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 13, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 14, 0 );
	writeEEPROM( VSCP_EEPROM_REG_GUID + 15, 0 );
	
}

///////////////////////////////////////////////////////////////////////////////
// doWork
//
// The actual work is done here.
//

void doWork( void )
{
	uint8_t tmp;

	if ( VSCP_STATE_ACTIVE == vscp_node_state ) {	
		
		// Time for temperature report sensor 1
		tmp = readEEPROM( EEPROM_INTERVAL_TEMP0 );
		if ( tmp && ( seconds_temperature0 > tmp ) ) {
			
			vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
			vscp_omsg.flags = VSCP_VALID_MSG + 4;
			vscp_omsg.vscp_class = VSCP_CLASS1_MEASUREMENT;
			vscp_omsg.vscp_type = VSCP_TYPE_MEASUREMENT_TEMPERATURE;
			vscp_omsg.data[ 0 ] = 0x80 + ( readEEPROM( EEPROM_TEMP_UNIT ) << 3 );
			vscp_omsg.data[ 1 ] = 0x02; // Exponent
			
			setEventData( getCurrentTemp4Sensor0() + readEEPROM( EEPROM_TEMP0_CALIBRATION ), 
			                readEEPROM( EEPROM_TEMP_UNIT ) );

			// Send data
			if ( vscp_sendEvent() ) {
				seconds_temperature0 = 0;	
			}
		}

		// Time for temperature report sensor 2
		tmp = readEEPROM( EEPROM_INTERVAL_TEMP1 );
		if ( tmp && ( seconds_temperature1 > tmp ) ) {
			
			vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
			vscp_omsg.flags = VSCP_VALID_MSG + 4;
			vscp_omsg.vscp_class = VSCP_CLASS1_MEASUREMENT;
			vscp_omsg.vscp_type = VSCP_TYPE_MEASUREMENT_TEMPERATURE;
			vscp_omsg.data[ 0 ] = 0x80 + ( readEEPROM( EEPROM_TEMP_UNIT ) << 3 ) + 1;
			vscp_omsg.data[ 1 ] = 0x02; // Exponent
			
			setEventData( getCurrentTemp4Sensor1() + readEEPROM( EEPROM_TEMP0_CALIBRATION ), 
			                    readEEPROM( EEPROM_TEMP_UNIT ) );

			// Send data
			if ( vscp_sendEvent() ) {
				seconds_temperature1 = 0;	
			}
		}
		
	}	
}

///////////////////////////////////////////////////////////////////////////////
// setEventData
//

void setEventData( int v, uint8_t unit )
{
	int newval;
	
	if (TEMP_UNIT_KELVIN == unit ) {
		// Convert to Kelvin
		newval = Celsius2Kelvin( v );	
	}
	else if ( TEMP_UNIT_FAHRENHEIT == unit ) {
		// Convert to Fahrenheit
		newval = Celsius2Fahrenheit( v );								
	}
	else {
		// Defaults to Celsius
		newval = v;
	}

	if ( newval < 0 ) {
		vscp_omsg.data[ 1 ] |= 0x80;			// if negative set sign bit
		newval = -1 * newval;		
	}
	
	vscp_omsg.data[ 2 ] = ( ( newval & 0xff00) >> 8 );
	vscp_omsg.data[ 3 ] = ( newval & 0xff);
	
}



///////////////////////////////////////////////////////////////////////////////
// doMeasurement
//
// The following formula is used
// Temperature = ( 65536 * d.c. - 20922 ) / 309
//

void doMeasurement( void ) 
{
/*	
	uint32_t temp;
	unsigned int high;
	unsigned int low;
	unsigned int ttt;
	
	if ( 0 == nSensor ) {
		
		nSensor = 1;	// next is sensor 1
		
		INTCONbits.GIE = 0;
		
		WriteTimer0( 0 );
			
		// Wait until low
		while ( PORTBbits.RB0 ) {
			ClrWdt();
			if ( ReadTimer0() > 0x8000 ) return;
		}
	
		// Measure low cycle
		WriteTimer0( 0 );	
		
		// Wait until high
		while ( !PORTBbits.RB0 ) {
			ClrWdt();
			if ( ReadTimer0() > 0x8000 ) return;
		}
		
		// Save low value
		low = ReadTimer0();
		
		WriteTimer0( 0 );
		
		// Wait until low
		while ( PORTBbits.RB0 ) {
			ClrWdt();
			if ( ReadTimer0() > 0x8000 ) return;
		}
		
		// Save high value
		high = ReadTimer0();
		
		INTCONbits.GIE = 1;	
		
		temp = 6553600L / ( high + low );
		temp = temp * (uint32_t)high;
		temp = ( temp - 2092200L ) / 309L;
		
		ttt = 1;
		
		tarray0[ tarray_idx0++  ] = temp;
		if ( tarray_idx0 > TEMPERATURE_ARRAY_SIZE ) tarray_idx0 = 0;
		temperature0_array[ hours ] = ( temperature0_array[ hours ] + getCurrentTemp4Sensor0() ) / 2;
		
		
	}
	else {
		
		nSensor = 0;	// next is sensor 1
		
		WriteTimer0( 0 );
		
		// Wait until low
		while ( PORTBbits.RB1 ) {
			ClrWdt();
			if ( ReadTimer0() > 0x8000 ) return;
		}
	
		// Measure low cycle
		WriteTimer0( 0 );	
		
		// Wait until high
		while ( !PORTBbits.RB1 ) {
			ClrWdt();
			if ( ReadTimer0() > 0x8000 ) return;
		}
		
		// Save low value
		low = ReadTimer0();
		
		WriteTimer0( 0 );
		
		// Wait until low
		while ( PORTBbits.RB1 ) {
			ClrWdt();
			if ( ReadTimer0() > 0x8000 ) return;
		}
		
		// Save high value
		high = ReadTimer0();
	
	}
*/	
			
	uint32_t cnt;
	volatile uint32_t high;
	volatile uint32_t low;
	
	cnt = DUMP_MEASUREMENT_COUNT;
	high = 0;
	low = 0;
	
	if ( 0 == nSensor ) {
				while ( cnt-- ) {
			ClrWdt();
			if ( PORTBbits.RB0 ) {
				high++;
			}
			else {
				low++;
			}
		}
 
 		if ( ( DUMP_MEASUREMENT_COUNT == high ) || ( DUMP_MEASUREMENT_COUNT == low ) ) { 	
			// No sensor or faulty sensor
			tarray0[ tarray_idx0++  ] = 0x8000;
			if ( tarray_idx0 > TEMPERATURE_ARRAY_SIZE ) tarray_idx0 = 0;
			temperature0_array[ vscp_hour ] = 0x8000;
		
		}
		else {
    		// Store temperature
			tarray0[ tarray_idx0++  ] = 100 * ( high/2 - 20922 ) / 309 ;
			if ( tarray_idx0 > TEMPERATURE_ARRAY_SIZE ) tarray_idx0 = 0;
			temperature0_array[ vscp_hour ] = 
					( temperature0_array[ vscp_hour ] + getCurrentTemp4Sensor0() ) / 2;
		
			current_temp0 = 100 * ( high/2 - 20922 ) / 309 ;
			cnt = 1;
		}
	}
	else {
		
		// Sensor 1
		while ( cnt-- ) {
			ClrWdt();
			if ( PORTBbits.RB1 ) {
				high++;
			}
			else {
				low++;
			}
		}
	
		if ( ( DUMP_MEASUREMENT_COUNT == high ) || ( DUMP_MEASUREMENT_COUNT == low ) ) { 	
			// No sensor or faulty sensor
			tarray1[ tarray_idx1++  ] = 0x8000;
			if ( tarray_idx1 > TEMPERATURE_ARRAY_SIZE ) tarray_idx1 = 0;
			temperature1_array[ vscp_hour ] = 0x8000;
		
		}
		else {
			tarray1[ tarray_idx1++  ] = 100 * ( high/2 - 20922 ) / 309 ;
			if ( tarray_idx1 > TEMPERATURE_ARRAY_SIZE ) tarray_idx1 = 0;
			temperature1_array[ vscp_hour ] = ( temperature1_array[ vscp_hour ] + getCurrentTemp4Sensor1() ) / 2;
		
			current_temp1	= 100 * ( high/2 - 20922 ) / 309 ;
			cnt = 1;
		}
	}
	
	if ( nSensor ) {
		nSensor = 0;	// next is sensor 0
	}
	else {
		nSensor = 1;	// next is sensor 1
	}
				
}


///////////////////////////////////////////////////////////////////////////////
// getCurrentTemp4Sensor0
//
// returns the mean of the allocated measurement values
//

int getCurrentTemp4Sensor0( void )
{
	uint8_t i;
	long temp;
	
	temp = 0;
	for ( i=0; i<TEMPERATURE_ARRAY_SIZE; i++ ) {
		temp += tarray0[ i ];
	}
	
	//return ( (int)( temp / TEMPERATURE_ARRAY_SIZE ) );
	return current_temp0;
}

///////////////////////////////////////////////////////////////////////////////
// getCurrentTemp4Sensor1
//
// returns the mean of the allocated measurement values
//

int getCurrentTemp4Sensor1( void )
{
	uint8_t i;
	long temp;
	
	temp = 0;
	for ( i=0; i<TEMPERATURE_ARRAY_SIZE; i++ ) {
		temp += tarray1[ i ];
	}
	
	//return ( (int)( temp / TEMPERATURE_ARRAY_SIZE ) );
	return current_temp1;
}


///////////////////////////////////////////////////////////////////////////////
// get24HourMean4Sensor0
//
// returns the mean of the allocated 24 hour measurement values
//

int get24HourMean4Sensor0( void )
{
	uint8_t i;
	long temp;
	
	temp = 0;
	for ( i=0; i<24; i++ ) {
		temp += temperature0_array[ i ];
	}
	
	return ( (int)( temp / 24 ) );
}

///////////////////////////////////////////////////////////////////////////////
// get24HourMean4Sensor1
//
// returns the mean of the allocated 24 hour measurement values
//

int get24HourMean4Sensor1( void )
{
	uint8_t i;
	long temp;
	
	temp = 0;
	for ( i=0; i<24; i++ ) {
		temp += temperature1_array[ i ];
	}
	
	return ( (int)( temp / 24 ) );
}

///////////////////////////////////////////////////////////////////////////////
// sendAlarmEvent
//

int sendAlarmEvent( uint8_t nAlarm )
{
	vscp_omsg.priority = VSCP_PRIORITY_HIGH;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.vscp_class = VSCP_CLASS1_ALARM;
	vscp_omsg.vscp_type = VSCP_TYPE_ALARM_ALARM;
	vscp_omsg.data[ 0 ] = nAlarm;
	vscp_omsg.data[ 1 ] = vscp_alarmstatus;

	// Send data
	if ( !vscp_sendEvent() ) {
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// sendErrorEvent
//

int sendErrorEvent( uint8_t nError )
{
	vscp_omsg.priority = VSCP_PRIORITY_HIGH;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
	vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_ERROR;
	vscp_omsg.data[ 0 ] = nError;
	vscp_omsg.data[ 1 ] = readEEPROM( EEPROM_ZONE ); 	// Zone
	vscp_omsg.data[ 2 ] = readEEPROM( EEPROM_SUBZONE ); // Suz Zone
	
	// Send data
	if ( !vscp_sendEvent() ) {
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// convertTemperature
//

int convertTemperature( int temp, uint8_t unit )
{
	int newval;

	if (TEMP_UNIT_KELVIN == unit  ) {
		// Convert to Kelvin
		newval = Celsius2Kelvin( temp );	
	}
	else if ( TEMP_UNIT_FAHRENHEIT == unit ) {
		// Convert to Fahrenheit
		newval = Celsius2Fahrenheit( temp );								
	}
	else {
		// Defaults to Celsius
		newval = temp;
	}

	return newval;
}

///////////////////////////////////////////////////////////////////////////////
// read_app_register
//

uint8_t vscp_readAppReg( uint8_t reg )
{
	int tmpval;
	uint8_t val, checksum;
	uint8_t unit = readEEPROM( EEPROM_TEMP_UNIT );
	uint8_t rv;

	switch ( reg ) {
		
		// Zone
		case 0x00:
			rv =  readEEPROM( EEPROM_ZONE );			
			break;

		// Subzone
		case 0x01:
			rv =  readEEPROM( EEPROM_SUBZONE );
			break;		

		// MSB of current temperature 1
		case 0x02:
			tmpval = convertTemperature( getCurrentTemp4Sensor0(), unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );			
			break;

		// LSB of current temperature 1
		case 0x03:
			tmpval = convertTemperature( getCurrentTemp4Sensor0(), unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of current temperature 2
		case 0x04:
			tmpval = convertTemperature( getCurrentTemp4Sensor1(), unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of current temperature 2
		case 0x05:
			tmpval = convertTemperature( getCurrentTemp4Sensor1(), unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut high temp
		case 0x06:
			tmpval = readEEPROM( EEPROM_ABSMAX_TEMP0 ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP0 + 1 );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut high temp
		case 0x07:
			tmpval = readEEPROM( EEPROM_ABSMAX_TEMP0 ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP0 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;
	
		// MSB of absolut high temp
		case 0x08:
			tmpval = readEEPROM( EEPROM_ABSMAX_TEMP1 ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP1 + 1 );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut high temp
		case 0x09:
			tmpval = readEEPROM( EEPROM_ABSMAX_TEMP1 ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP1 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut low temp
		case 0x0A:
			tmpval = readEEPROM( EEPROM_ABSMIN_TEMP0 ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP0 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut low temp
		case 0x0B:
			tmpval = readEEPROM( EEPROM_ABSMIN_TEMP0 ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP0 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut low temp
		case 0x0C:
			tmpval = readEEPROM( EEPROM_ABSMIN_TEMP1 ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP1 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut low temp
		case 0x0D:
			tmpval = readEEPROM( EEPROM_ABSMIN_TEMP1 ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP1 + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;
 
		// MSB of running mean temperature
		case 0x0E:
			tmpval = get24HourMean4Sensor0();
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of running mean temperature
		case 0x0F:
			tmpval = get24HourMean4Sensor0();
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of running mean temperature
		case 0x10:
			tmpval = get24HourMean4Sensor1();
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of running mean temperature
		case 0x11:
			tmpval = get24HourMean4Sensor1();
			tmpval = convertTemperature( tmpval, unit );
			rv = ( tmpval & 0x00ff );
			break;

		// Temperature 1 1ow alarm point
		case 0x12:
			rv = readEEPROM( EEPROM_TEMP0_LOW_ALARMPOINT );
			break;

		// Temperature 2 1ow alarm point
		case 0x13:
			rv = readEEPROM( EEPROM_TEMP1_LOW_ALARMPOINT );
			break;

		// Temperature 1 high alarm point
		case 0x14:
			rv = readEEPROM( EEPROM_TEMP0_HIGH_ALARMPOINT );
			break;

		// Temperature 2 high alarm point
		case 0x15:
			rv = readEEPROM( EEPROM_TEMP1_HIGH_ALARMPOINT );
			break;

		// Temperature 1 sensor error counter
		case 0x16:
			rv = error_cnt_tempsensor0;
			break;

		// Temperature 2 sensor error counter
		case 0x17:
			rv = error_cnt_tempsensor1;
			break;

		// Temperature 1 event interval
		case 0x19:
			rv = readEEPROM( EEPROM_INTERVAL_TEMP0 );
			break;

		// Temperature 2 event interval
		case 0x1A:
			rv = readEEPROM( EEPROM_INTERVAL_TEMP1 );
			break;

		// Temperature format
		case 0x1B:
			rv = readEEPROM( EEPROM_TEMP_UNIT );
			break;

		// MSB of temperature 1 calibration 
		case 0x1C:
			rv = readEEPROM( EEPROM_TEMP0_CALIBRATION );
			break;

		// LSB of temperature 1 calibration 
		case 0x1D:
			rv = readEEPROM( EEPROM_TEMP0_CALIBRATION + 1 );
			break;

		// MSB of temperature 2 calibration 
		case 0x1E:
			rv = readEEPROM( EEPROM_TEMP1_CALIBRATION );
			break;

		// LSB of temperature 2 calibration 
		case 0x1F:
			rv = readEEPROM( EEPROM_TEMP1_CALIBRATION + 1 );
			break;

		// Hysteresis for temperature 1 alarm
		case 0x20:
			rv = readEEPROM( EEPROM_TEMP0_HYSTERESIS );
			break;

		// Hysteresis for temperature 2 alarm
		case 0x21:
			rv = readEEPROM( EEPROM_TEMP1_HYSTERESIS );
			break;		
	 
		default:
			rv = 0;	
			break;
	}

	return rv;
}

///////////////////////////////////////////////////////////////////////////////
// write_app_register
//

uint8_t vscp_writeAppReg( uint8_t reg, uint8_t val )
{
	uint8_t i;
	int tmpval;
	uint8_t checksum;
	uint8_t unit = readEEPROM( EEPROM_TEMP_UNIT );
	uint8_t rv;

	switch ( reg ) {

		// Zone
		case 0x00:
			writeEEPROM( EEPROM_ZONE, val );
			rv = readEEPROM( EEPROM_ZONE );
			break;
			
		// SubZone
		case 0x01:
			writeEEPROM( EEPROM_SUBZONE, val );
			rv = readEEPROM( EEPROM_SUBZONE );
			break;
			
		// MSB of absolut high temp1
		case 0x06:
			writeEEPROM( ( EEPROM_ABSMAX_TEMP0 + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_TEMP0, 0xff );
			rv = 0xff;
			break;

		// LSB of absolut high temp1
		case 0x07:
			writeEEPROM( ( EEPROM_ABSMAX_TEMP0 + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_TEMP0, 0xff );
			rv = 0xff;
			break;

		// MSB of absolut high temp2
		case 0x08:
			writeEEPROM( ( EEPROM_ABSMAX_TEMP1 + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_TEMP1, 0xff );
			rv = 0xff;
			break;

		// LSB of absolut high temp2
		case 0x09:
			writeEEPROM( ( EEPROM_ABSMAX_TEMP1 + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_TEMP1, 0xff );
			rv = 0xff;
			break;

		// MSB of absolut low temp1
		case 0x0A:
			writeEEPROM( ( EEPROM_ABSMIN_TEMP0 + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_TEMP0, 0xff );
			rv = 0x7f;
			break;

		// LSB of absolut low temp1
		case 0x0B:
			writeEEPROM( ( EEPROM_ABSMIN_TEMP0 + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_TEMP0, 0xff );
			rv = 0xff;
			break;

		// MSB of absolut low temp2
		case 0x0C:
			writeEEPROM( ( EEPROM_ABSMIN_TEMP1 + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_TEMP1, 0xff );
			rv = 0x7f;
			break;

		// LSB of absolut low temp2
		case 0x0D:
			writeEEPROM( ( EEPROM_ABSMIN_TEMP1 + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_TEMP1, 0xff );
			rv = 0xff;
			break;

		// MSB of running mean temperature 1
		case 0x0e:
			for ( i=0; i<24; i++ ) {
				temperature0_array[ i ] = 0;	
			}
			rv = 0x00;
			break;

		// LSB of running mean temperature 1
		case 0x0f:
			for ( i=0; i<24; i++ ) {
				temperature0_array[ i ] = 0;	
			}
			rv = 0x00;
			break;

		// MSB of running mean temperature 2
		case 0x10:
			for ( i=0; i<24; i++ ) {
				temperature1_array[ i ] = 0;	
			}
			rv = 0x00;
			break;

		// LSB of running mean temperature 2
		case 0x11:
			for ( i=0; i<24; i++ ) {
				temperature1_array[ i ] = 0;	
			}
			rv = 0x00;
			break;

		// Temperature 1 low alarm point
		case 0x12:
			writeEEPROM( EEPROM_TEMP0_LOW_ALARMPOINT, val ); 
			rv = readEEPROM( EEPROM_TEMP0_LOW_ALARMPOINT );
			break;

		// Temperature 2 low alarm point
		case 0x13:
			writeEEPROM( EEPROM_TEMP1_LOW_ALARMPOINT, val ); 
			rv = readEEPROM( EEPROM_TEMP1_LOW_ALARMPOINT );
			break;

		// Temperature 1 high alarm point
		case 0x14:
			writeEEPROM( EEPROM_TEMP0_HIGH_ALARMPOINT, val ); 
			rv = readEEPROM( EEPROM_TEMP0_HIGH_ALARMPOINT );
			break;

		// Temperature 2 high alarm point
		case 0x15:
			writeEEPROM( EEPROM_TEMP1_HIGH_ALARMPOINT, val ); 
			rv = readEEPROM( EEPROM_TEMP1_HIGH_ALARMPOINT );
			break; 

		// Temperature 1 sensor error counter
		case 0x16:
			error_cnt_tempsensor0 = 0;
			rv = 0x00;
			break;

		// Temperature 2 sensor error counter
		case 0x17:
			error_cnt_tempsensor1 = 0;
			rv = 0x00;
			break;

		// Temperature 1 event interval
		case 0x19:
			writeEEPROM( EEPROM_INTERVAL_TEMP0, val );		
			rv = readEEPROM( EEPROM_INTERVAL_TEMP0 );
			break;

		// Temperature 2 event interval
		case 0x1A:
			writeEEPROM( EEPROM_INTERVAL_TEMP1, val );		
			rv = readEEPROM( EEPROM_INTERVAL_TEMP1 );
			break;

		// Temperature format
		case 0x1B:
			writeEEPROM( EEPROM_TEMP_UNIT, val );
			rv = readEEPROM( EEPROM_TEMP_UNIT );
			break;

		// MSB of temperature 1 calibration 
		case 0x1C:
			writeEEPROM( EEPROM_TEMP0_CALIBRATION, val );
			rv = readEEPROM( EEPROM_TEMP0_CALIBRATION );
			break;

		// LSB of temperature 1 calibration 
		case 0x1D:
			writeEEPROM( EEPROM_TEMP0_CALIBRATION + 1, val );
			rv = readEEPROM( EEPROM_TEMP0_CALIBRATION + 1 );
			break;

		// MSB of temperature 2 calibration 
		case 0x1E:
			writeEEPROM( EEPROM_TEMP1_CALIBRATION, val );
			rv = readEEPROM( EEPROM_TEMP1_CALIBRATION );
			break;

		// LSB of temperature 2 calibration 
		case 0x1F:
			writeEEPROM( EEPROM_TEMP1_CALIBRATION + 1, val );
			rv = readEEPROM( EEPROM_TEMP1_CALIBRATION + 1 );
			break;

		// Hysteresis for temperature 1 alarm
		case 0x20:
			writeEEPROM( EEPROM_TEMP0_HYSTERESIS, val );
			rv = readEEPROM( EEPROM_TEMP0_HYSTERESIS );
			break;

		// Hysteresis for temperature 2 alarm
		case 0x21:
			writeEEPROM( EEPROM_TEMP1_HYSTERESIS, val );
			rv = readEEPROM( EEPROM_TEMP1_HYSTERESIS );
			break;
			
		default:
			rv = ~val; // error return	
			break;
	}

	return rv;	
}



///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Get Major version number for this hardware module
//

uint8_t vscp_getMajorVersion()
{
	return FIRMWARE_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Minor version number for this hardware module
//

uint8_t vscp_getMinorVersion()
{
	return FIRMWARE_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Subminor version number for this hardware module
//

uint8_t vscp_getSubMinorVersion()
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
	writeEEPROM( VSCP_EEPROM_REG_USERID + idx, data );
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
// setVSCPManufacturerId
// 
// Set Manufacturer id and subid from EEPROM
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
	return VSCP_SIZE_STD_DM_ROW;	// Standard CAN frame
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
//  vscp_getMatrixInfo
//

void vscp_getMatrixInfo( char *pData )
{
	uint8_t i;
	
	// We don't implement a DM for this module
	// So we just set the data to zero
	for ( i = 0; i < 8; i++ ) {
		pData[ i ] = 0;
	}	

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

///////////////////////////////////////////////////////////////////////////////
// vscp_getRegisterPagesUsed
//

uint8_t vscp_getRegisterPagesUsed( void )
{
	return 1;
}

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
