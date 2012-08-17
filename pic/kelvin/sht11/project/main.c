/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Kelvin SHT-11 Module
 * 	Version 0.04 2004-12-24
 * 	akhe@eurosource.se
 *
 * Copyright (C) 1995-2011 Ake Hedman, eurosource
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

// $RCSfile: main.c,v $                                       
// $Date: 2006/03/23 19:57:43 $                                  
// $Author: akhe $                                              
// $Revision: 1.24 $ 

#include <p18cxxx.h>
#include <timers.h>
#include <eeprom.h>
#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include "../../../../common/inttypes.h"
#include "../../../common/eeprom.h"
#include "../../../common/can18xx8.h"
#include "../../../common/can18f.h"
#include "../../../../common/vscp_firmware.h"
#include "i:/src/vscp/common/vscp_class.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "i:/src/vscp/common/vscp_type.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "main.h" 
#include "version.h"
#include "humidity.h"
#include "shtxx.h"

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

// ISR Routines
void isr( void );

extern const rom int log_Table[];		// log10 for Hummidity measurement

// The device URL (max 32 characters including null termination)
const rom char vscp_deviceURL[] = "www.eurosource.se/sht_001.xml";

// Global Variable Declarations
int current_temp;						// Current temperature
int current_humidity;					// Current humidity
int current_dew;						// Current dew point
int mean_temp;							// Running mean for temp
int mean_hum;							// Running mean for humidity
int mean_dew;							// Running mean fro dew-point

unsigned char error_cnt_temp;			// Error counter for temp sensor reads			
unsigned char error_cnt_hum;			// Error counter for hum sensor reads

unsigned long measurement_clock;		// Clock for measurments
unsigned char seconds;					// counter for seconds

unsigned char seconds_temp;				// timer for temp event
unsigned char seconds_hum;				// timer for hum event
unsigned char seconds_dew;				// Timer for dew event

// bit 7 - high temp alarm sent
// bit 6 - low temp alarm sent
// bit 5 - high humidity alarm sent
// bit 4 - low humidity alarm sent
unsigned char alarm_flags;		

// incremented once of each OK reading
// nulled if failure
// if 0xff resests error counts
unsigned char okreadings;

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
	_asm GOTO isr _endasm 
}
#pragma code

#pragma interrupt isr
void isr( void )
{
	// Check timer
	if ( INTCONbits.TMR0IF ) {	// If A Timer0 Interrupt, Then

		// Reload value for 1 ms reolution
		WriteTimer0( TIMER0_RELOAD_VALUE );
		vscp_timer++;
		sht_measure_timer++;
		measurement_clock++;
		INTCONbits.TMR0IF = 0;     // Clear Timer0 Interrupt Flag
	}

	// Check for init button
	if ( !( PORTC & 0x01 ) ) {
		// Active
		vscp_initbtncnt++;	
	}
	else {
		vscp_initbtncnt = 0;
	}

	// Status LED
	vscp_statuscnt++;
	if ( ( VSCP_LED_BLINK1 == vscp_initledfunc ) && ( vscp_statuscnt > 100 ) ) {
		if ( PORTC & 0x02 ) {
			PORTC &= ~0x02;
		}
		else {
			PORTC |= 0x02;
		}	
		vscp_statuscnt = 0;
	}
	else if ( VSCP_LED_ON == vscp_initledfunc ) {
		PORTC |= 0x02;	
		vscp_statuscnt = 0;	
	}
	else if ( VSCP_LED_OFF == vscp_initledfunc ) {
		PORTC &= ~0x02;
		vscp_statuscnt = 0;
	}
	
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
			doMeasurement();
			seconds++;
			
			// Temperature/hum/dew report timers are only updated if in active 
			// state
			if ( VSCP_STATE_ACTIVE == vscp_node_state ) {
				seconds_temp++;
				seconds_hum++;
				seconds_dew++;
			}

			if ( seconds > 60 ) {
			
				seconds = 0;
				
				// If we have errors in sensor readings
				// send out error event
				if ( error_cnt_temp || error_cnt_hum ) {
					sendErrorEvent( ERROR_SENSOR );
				}
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
// doWork
//
// The actual work is done here.
//

void doWork( void )
{
	unsigned char tmp;

	if ( VSCP_STATE_ACTIVE == vscp_node_state ) {	
		
		// Time for temperature report
		tmp = readEEPROM( EEPROM_INTERVAL_TEMP );
		if ( tmp && ( seconds_temp > tmp ) ) {
			
			vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
			vscp_omsg.flags = VSCP_VALID_MSG + 4;
			vscp_omsg.class = VSCP_CLASS1_MEASUREMENT;
			vscp_omsg.type = VSCP_TYPE_MEASUREMENT_TEMPERATURE;
			vscp_omsg.data[ 0 ] = 0x80 + ( readEEPROM( EEPROM_TEMP_UNIT ) << 3 );
			vscp_omsg.data[ 1 ] = 0x02; // Exponent
			
			setEventData( current_temp, readEEPROM( EEPROM_TEMP_UNIT ) );

			// Send data
			if ( vscp_sendEvent() ) {
				seconds_temp = 0;	
			}

		}

		// Time for humidity report
		tmp = readEEPROM( EEPROM_INTERVAL_HUMIDITY );
		if ( tmp && ( seconds_hum > tmp )	) {
			
			vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
			vscp_omsg.flags = VSCP_VALID_MSG + 4;
			vscp_omsg.class = VSCP_CLASS1_MEASUREMENT;
			vscp_omsg.type = VSCP_TYPE_MEASUREMENT_HUMIDITY;
			vscp_omsg.data[ 0 ] = 0x80;
			vscp_omsg.data[ 1 ] = 0x02; // Exponent
			vscp_omsg.data[ 2 ] = ( ( current_humidity  & 0xFF00 ) >> 8 );
			vscp_omsg.data[ 3 ] = ( current_humidity  & 0xFF );

			// Send data
			if ( vscp_sendEvent() ) {
				seconds_hum = 0;;
			}
		
		}

		// Time for dew point report
		tmp = readEEPROM( EEPROM_INTERVAL_DEW  );
		if ( tmp && ( seconds_dew > tmp )	) {
			
			vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
			vscp_omsg.flags = VSCP_VALID_MSG + 4;
			vscp_omsg.class = VSCP_CLASS1_MEASUREMENT;
			vscp_omsg.type = VSCP_TYPE_MEASUREMENT_DEWPOINT;
			vscp_omsg.data[ 0 ] = 0x80 + ( readEEPROM( EEPROM_TEMP_UNIT ) << 3 );
			vscp_omsg.data[ 1 ] = 0x02; // Exponent
			
			setEventData( current_dew, readEEPROM( EEPROM_TEMP_UNIT ) );

			// Send data
			if ( vscp_sendEvent() ) {
				seconds_dew = 0;	
			}

		}		
	}	
}

///////////////////////////////////////////////////////////////////////////////
// setEventData
//

void setEventData( int v, unsigned char unit )
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
// Init - Initialization Routine
//  

void init()
{
	uint8_t msgdata[ 8 ];

	// Initialize data
	init_app_ram();

	// Initialize the uP
	
	ADCON1 = 0x0f;	// All digital inputs - Needed by 18F2580

	// Set all as inputs
	TRISA = 0xFF;                        

	// PortB
	// RB3 CAN RX - input
    // RB2 CAN TX - output
	TRISB = 0xFB;

	// RC0 - Input  - Init button
	// RC1 - Output - Status LED - Default off
    // RC2 - Input  - 
	// RC3 - Output - SHTxx clock
	// RC5 - I/O	- SHTxx data
	TRISC = 0xF5;
	PORTC = 0x00;		
	
	OpenTimer0( TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_8 );
	WriteTimer0( TIMER0_RELOAD_VALUE );
	
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
	
	seconds_temp = 0;
	seconds_hum = 0;
	seconds_dew = 0;

	alarm_flags = 0;		// no alarms yet

	okreadings = 0;
	
	current_temp = -1;		// no temp read yet			
	current_humidity = -1;	// no humidity read yet 				
	current_dew = -1;		// no dew point yet					
	mean_temp = 0;			// no mean temp yet
	mean_hum = 0;			// no mean humidity yet
	mean_dew = 0;			// no mean dew point yet				
	error_cnt_temp = 0;		// no errors in temp readings	
	error_cnt_hum = 0;		// no errors in hum readings
	
}

///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom( void )
{
 	// Abs. max temperature set to -327 degrees
	writeEEPROM( EEPROM_ABSMAX_TEMP, 0xFF );
	writeEEPROM( EEPROM_ABSMAX_TEMP + 1, 0xFF );

	// Abs. min temperature set to +327 degrees
	writeEEPROM( EEPROM_ABSMIN_TEMP, 0x7F );
	writeEEPROM( EEPROM_ABSMIN_TEMP + 1, 0xFF );

	// Abs. max hummidity set to -327 %
	writeEEPROM( EEPROM_ABSMAX_HUMIDITY, 0xFF );
	writeEEPROM( EEPROM_ABSMAX_HUMIDITY + 1, 0xFF );

	// Abs. min hummidity set to +327 %
	writeEEPROM( EEPROM_ABSMIN_HUMIDITY, 0x7F );
	writeEEPROM( EEPROM_ABSMIN_HUMIDITY + 1, 0xFF );

	// Abs. max temperature set to -327 degrees
	writeEEPROM( EEPROM_ABSMAX_DEW, 0xFF );
	writeEEPROM( EEPROM_ABSMAX_DEW + 1, 0xFF );

	// Abs. min temperature set to +327 degrees
	writeEEPROM( EEPROM_ABSMIN_DEW, 0x7F );
	writeEEPROM( EEPROM_ABSMIN_DEW + 1, 0xFF );

	// Low temp alarm disables
	writeEEPROM( EEPROM_TEMP_LOW_ALARMPOINT, 0xFF );

	// High temp alarm disabled
	writeEEPROM( EEPROM_TEMP_HIGH_ALARMPOINT, 0xFF );

	// Low humidity alarm disabled
	writeEEPROM( EEPROM_HUMIDITY_LOW_ALARMPOINT, 0xFF );

	// High humidity alarm disabled
	writeEEPROM( EEPROM_HUMIDITY_HIGH_ALARMPOINT, 0xFF );

	// Send temp event once every 30 seconds
	writeEEPROM( EEPROM_INTERVAL_TEMP, 0x1E );

	// Send humidity event once every 30 seconds
	writeEEPROM( EEPROM_INTERVAL_HUMIDITY, 0x1E );

	// Send dew point event once every 30 seconds
	writeEEPROM( EEPROM_INTERVAL_DEW, 0x1E );

	// Unit is Celsius
	writeEEPROM( EEPROM_TEMP_UNIT, TEMP_UNIT_CELSIUS );

	// Temp calibration value is zero
	writeEEPROM( EEPROM_TEMP_CALIBRATION, 0x00 );
	writeEEPROM( EEPROM_TEMP_CALIBRATION + 1, 0x00 );

	// Humidity calibration value is zero
	writeEEPROM( EEPROM_HUM_CALIBRATION, 0x00 );
	writeEEPROM( EEPROM_HUM_CALIBRATION + 1, 0x00 );
	
	// Temp hysteresis = 5 
	writeEEPROM( EEPROM_TEMP_HYSTERESIS, 0x05 );

	// Humidity hysteresis = 5
	writeEEPROM( EEPROM_HUM_HYSTERESIS, 0x05 );
	
	// Zone/Subzne
	writeEEPROM( EEPROM_ZONE, 0 );
	writeEEPROM( EEPROM_SUBZONE, 0 );
}

///////////////////////////////////////////////////////////////////////////////
// doMeasurement
//

void doMeasurement( void ) 
{
	unsigned char i;
	unsigned char value[2];
	unsigned char checksum;
	unsigned char save,crc;
	int bHum;
	unsigned long lval_hum, lval_temp;
	long m;
	long humkorr;

	// Mark readings as valid	
	bHum = TRUE;

	if ( sht_measure( value, &checksum, SHT_MODE_HUMI ) ) {
		sht_connectionreset();
	}
	
	crc = 0x00;
	crc = sht_calc_crc( crc, MEASURE_HUMI );
	crc = sht_calc_crc( crc, value[ 0 ] );
	crc = sht_calc_crc( crc, value[ 1 ] );
		
	save = crc;
	crc = 0;
	for ( i = 0x80; i > 0; i /= 2 ) { 		// Reverse bitorder
		if ( save & i ) {
			crc |= 0x80; 
		}	
		if ( i != 1 ) crc = crc >> 1;
	}
		
	if ( crc == checksum ) {
		
		okreadings++;
		lval_hum = value[ 0 ] * 256 + value[ 1 ];
		current_humidity = ( 405000L * lval_hum - 28L * ( lval_hum * lval_hum ) - 40000000L ) / 100000 ;	
		// Calibrate
		current_humidity += readEEPROM( EEPROM_HUM_CALIBRATION ) * 256 + readEEPROM( EEPROM_HUM_CALIBRATION );
	}
	else {
	
		// Count humidity read error
		bHum = FALSE;
		okreadings = 0;
		error_cnt_hum++;
	}

	if ( sht_measure( value, &checksum, SHT_MODE_TEMP ) ) {
		sht_connectionreset();
	}
		
	crc = 0x00;
	crc = sht_calc_crc( crc, MEASURE_TEMP );
	crc = sht_calc_crc( crc, value[ 0 ] );
	crc = sht_calc_crc( crc, value[ 1 ] );
		
	save = crc;
	crc = 0;
	for ( i = 0x80; i > 0; i /= 2 ) { 		// Reverse bitorder
		if ( save & i ) {
			crc |= 0x80; 
		}	
		if ( i != 1 ) crc = crc >> 1;
	}

	if ( crc == checksum ) {

		// * * * T E M P E R A T U R E * * *

		okreadings++;

		lval_temp = value[ 0 ] * 256 + value[ 1 ];
		current_temp = lval_temp - 4000L;
		// Calibrate
		current_temp += readEEPROM( EEPROM_TEMP_CALIBRATION ) * 256 + readEEPROM( EEPROM_TEMP_CALIBRATION + 1 );
		mean_temp = ( mean_temp + current_temp ) / 2;

		if ( ( readEEPROM( EEPROM_ABSMAX_TEMP ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP + 1 ) ) < current_temp ) {
			writeEEPROM( EEPROM_ABSMAX_TEMP, ( ( current_temp & 0xff00 ) >> 8 ) );
			writeEEPROM( EEPROM_ABSMAX_TEMP, ( current_temp & 0x00ff ) );
		}

		if ( ( readEEPROM( EEPROM_ABSMIN_TEMP ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP + 1 ) ) > current_temp ) {
			writeEEPROM( EEPROM_ABSMIN_TEMP, ( ( current_temp & 0xff00 ) >> 8 ) );
			writeEEPROM( EEPROM_ABSMIN_TEMP, ( current_temp & 0x00ff ) );
		}

		// Check for low temp alarm
		if ( current_temp < ( readEEPROM( EEPROM_TEMP_LOW_ALARMPOINT ) * 256  ) ) {
			// Low temp alarm	
			if ( !( alarm_flags & ALARM_TEMP_LOW ) ) {
				alarm_flags |= ALARM_TEMP_LOW;
				// Send temp low alarm	
				if ( sendAlarmEvent( ALARM_EVENT_TYPE_TEMP_LOW ) ) {
					vscp_alarmstatus |= ALARM_TEMP_LOW;
				}
			}
		}

		// Check for high temp alarm
		if ( current_temp > ( readEEPROM( EEPROM_TEMP_HIGH_ALARMPOINT ) * 256  ) ) {
			// High temp alarm		
			if ( !( alarm_flags & ALARM_TEMP_HIGH ) ) {
				alarm_flags |= ALARM_TEMP_HIGH;
				// Send temp high alarm				
				if ( sendAlarmEvent( ALARM_EVENT_TYPE_TEMP_HIGH ) ) {
					vscp_alarmstatus |= ALARM_TEMP_HIGH;
				}
			}
		}

		// Reset low alarm bit if over alarmpoint + hysteresis
		if ( current_temp > ( ( readEEPROM( EEPROM_TEMP_LOW_ALARMPOINT ) + 
										readEEPROM( EEPROM_TEMP_HYSTERESIS ) ) * 256  ) ) {
			alarm_flags &= ~ALARM_TEMP_LOW;	
		}

		// Reset high alarm bit if under alarmpoint - hysteresis
		if ( current_temp > ( ( readEEPROM( EEPROM_TEMP_LOW_ALARMPOINT ) - 
										readEEPROM( EEPROM_TEMP_HYSTERESIS ) ) * 256  ) ) {
			alarm_flags &= ~ALARM_TEMP_LOW;	
		}

		humkorr = ( ( current_temp - 2500 ) * ( 1000 + 8 * lval_hum ) ) / 100000000L; 		
		if ( bHum ) {
			
			// * * * H U M I D I T Y * * *
			current_humidity += humkorr;	
			mean_hum = ( mean_hum + current_humidity ) / 2;
		
			if ( ( readEEPROM( EEPROM_ABSMAX_HUMIDITY ) * 256 + readEEPROM( EEPROM_ABSMAX_HUMIDITY + 1 ) ) < current_humidity ) {
				writeEEPROM( EEPROM_ABSMAX_HUMIDITY, ( ( current_humidity & 0xff00 ) >> 8 ) );
				writeEEPROM( EEPROM_ABSMAX_HUMIDITY, ( current_humidity & 0x00ff ) );	
			}

			if ( ( readEEPROM( EEPROM_ABSMIN_HUMIDITY ) * 256 + readEEPROM( EEPROM_ABSMIN_HUMIDITY + 1 ) ) > current_humidity ) {
				writeEEPROM( EEPROM_ABSMIN_HUMIDITY, ( ( current_humidity & 0xff00 ) >> 8 ) );
				writeEEPROM( EEPROM_ABSMIN_HUMIDITY, ( current_humidity & 0x00ff ) );
			}

			// Check for low humidity alarm
			if ( current_humidity < ( readEEPROM( EEPROM_HUMIDITY_LOW_ALARMPOINT ) * 256  ) ) {
				// Low humidity alarm	
				if ( !( alarm_flags & ALARM_HUMIDITY_LOW ) ) {
					alarm_flags |= ALARM_HUMIDITY_LOW;
					// Send humidity low alarm	
					if ( sendAlarmEvent( ALARM_EVENT_TYPE_HUMIDITY_LOW ) ) {
						vscp_alarmstatus |= ALARM_HUMIDITY_LOW;
					}
				}
			}

			// Check for high humidity alarm
			if ( current_humidity > ( readEEPROM( EEPROM_HUMIDITY_HIGH_ALARMPOINT ) * 256  ) ) {
				// High humidity alarm		
				if ( !( alarm_flags & ALARM_HUMIDITY_HIGH ) ) {
					alarm_flags |= ALARM_HUMIDITY_HIGH;
					// Send humidity high alarm
					if ( sendAlarmEvent( ALARM_EVENT_TYPE_HUMIDITY_HIGH ) ) {
						vscp_alarmstatus |= ALARM_HUMIDITY_HIGH;	
					}
				}
			}

			// Reset low alarm bit if over alarmpoint + hysteresis
			if ( current_humidity > ( ( readEEPROM( EEPROM_HUMIDITY_LOW_ALARMPOINT ) + 
											readEEPROM( EEPROM_HUM_HYSTERESIS ) ) * 256  ) ) {
				alarm_flags &= ~ALARM_HUMIDITY_LOW;	
			}

			// Reset high alarm bit if under alarmpoint - hysteresis
			if ( current_humidity > ( ( readEEPROM( EEPROM_HUMIDITY_LOW_ALARMPOINT ) - 
											readEEPROM( EEPROM_HUM_HYSTERESIS ) ) * 256  ) ) {
				alarm_flags &= ~ALARM_HUMIDITY_LOW;	
			}

		}

		// * * * D E W * * *
		m =  661L + ( ( 7500L * current_temp ) / ( 23730L + current_temp )) + log_Table[ ( ( current_humidity + 50 )/100 ) ];
		current_dew = (23730*(661-m)/(m-8161));
		mean_dew = ( mean_dew + current_dew ) / 2;
	
		if ( ( readEEPROM( EEPROM_ABSMAX_DEW ) * 256 + readEEPROM( EEPROM_ABSMAX_DEW + 1 ) ) < current_dew ) {
			writeEEPROM( EEPROM_ABSMAX_DEW, ( ( current_dew & 0xff00 ) >> 8 ) );
			writeEEPROM( EEPROM_ABSMAX_DEW, ( current_dew & 0x00ff ) );
		}

		if ( ( readEEPROM( EEPROM_ABSMIN_DEW ) * 256 + readEEPROM( EEPROM_ABSMIN_DEW + 1 ) ) > current_dew ) {
			writeEEPROM( EEPROM_ABSMIN_DEW, ( ( current_dew & 0xff00 ) >> 8 ) );
			writeEEPROM( EEPROM_ABSMIN_DEW, ( current_dew & 0x00ff ) );
		}
	}
	else {
		// Count temperature read error
		okreadings = 0;
		error_cnt_temp++;
	}

	// Reset error counts if we have
	// enough good reads.
	if ( 0xff == okreadings ) {		
		error_cnt_temp = 0;	
		error_cnt_hum = 0;
	}
		
}

///////////////////////////////////////////////////////////////////////////////
// sendAlarmEvent
//

int sendAlarmEvent( unsigned char nAlarm )
{
	vscp_omsg.priority = VSCP_PRIORITY_HIGH;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_ALARM;
	vscp_omsg.type = VSCP_TYPE_ALARM_ALARM;
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

int sendErrorEvent( unsigned char nError )
{
	vscp_omsg.priority = VSCP_PRIORITY_HIGH;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.class = VSCP_CLASS2_LEVEL1_INFORMATION;
	vscp_omsg.type = VSCP_TYPE_INFORMATION_ERROR;
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

int convertTemperature( int temp, unsigned char unit )
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
// vscp_readAppReg
//

uint8_t vscp_readAppReg( unsigned char reg )
{
	uint8_t rv;
	int tmpval;
	uint8_t val, checksum;
	uint8_t unit = readEEPROM( EEPROM_TEMP_UNIT );

	switch ( reg ) {
    	
    	// Zone
		case 0x00:
			rv =  readEEPROM( EEPROM_ZONE );			
			break;

		// Subzone
		case 0x01:
			rv =  readEEPROM( EEPROM_SUBZONE );
			break;

		// MSB of current temperature
		case 0x02:
			tmpval = convertTemperature( current_temp, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );			
			break;

		// LSB of current temperature
		case 0x03:
			tmpval = convertTemperature( current_temp, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of current humidity
		case 0x04:
			rv =  ( ( current_humidity ) & 0xff00 >> 8 );
			break;

		// LSB of current humidity
		case 0x05:
			rv =  ( current_humidity & 0x00ff );
			break;

		// MSB of current dew point
		case 0x06:
			tmpval = convertTemperature( current_dew, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of current dew point
		case 0x07:
			tmpval = convertTemperature( current_dew, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut high temp
		case 0x08:
			tmpval = readEEPROM( EEPROM_ABSMAX_TEMP ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP + 1 );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut high temp
		case 0x09:
			tmpval = readEEPROM( EEPROM_ABSMAX_TEMP ) * 256 + readEEPROM( EEPROM_ABSMAX_TEMP + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut low temp
		case 0x0A:
			tmpval = readEEPROM( EEPROM_ABSMIN_TEMP ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut low temp
		case 0x0B:
			tmpval = readEEPROM( EEPROM_ABSMIN_TEMP ) * 256 + readEEPROM( EEPROM_ABSMIN_TEMP + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut high humidity
		case 0x0C:
			tmpval = readEEPROM( EEPROM_ABSMAX_HUMIDITY ) * 256 + readEEPROM( EEPROM_ABSMAX_HUMIDITY + 1  );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut high humidity
		case 0x0D:
			tmpval = readEEPROM( EEPROM_ABSMAX_HUMIDITY ) * 256 + readEEPROM( EEPROM_ABSMAX_HUMIDITY + 1  );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut low humidity
		case 0x0E:
			tmpval = readEEPROM( EEPROM_ABSMIN_HUMIDITY ) * 256 + readEEPROM( EEPROM_ABSMIN_HUMIDITY + 1  );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut low humidity
		case 0x0F:
			tmpval = readEEPROM( EEPROM_ABSMIN_HUMIDITY ) * 256 + readEEPROM( EEPROM_ABSMIN_HUMIDITY + 1  );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut high dew point
		case 0x10:
			tmpval = readEEPROM( EEPROM_ABSMAX_DEW ) * 256 + readEEPROM( EEPROM_ABSMAX_DEW  + 1 );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut high dew point
		case 0x11:
			tmpval = readEEPROM( EEPROM_ABSMAX_DEW ) * 256 + readEEPROM( EEPROM_ABSMAX_DEW + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of absolut low dew point
		case 0x12:
			tmpval = readEEPROM( EEPROM_ABSMIN_DEW ) * 256 + readEEPROM( EEPROM_ABSMIN_DEW  + 1 );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of absolut low dew point
		case 0x13:
			tmpval = readEEPROM( EEPROM_ABSMIN_DEW ) * 256 + readEEPROM( EEPROM_ABSMIN_DEW + 1  );
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of running mean temperature
		case 0x14:
			tmpval = mean_temp;
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of running mean temperature
		case 0x15:
			tmpval = mean_temp;
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of running mean of humidity
		case 0x16:
			tmpval = mean_hum;
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of running mean of humidity
		case 0x17:
			tmpval = mean_hum;
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// MSB of running mean od dew point
		case 0x18:
			tmpval = mean_dew;
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( ( tmpval & 0xff00 ) >> 8 );
			break;

		// LSB of running mean of dew point
		case 0x19:
			tmpval = mean_dew;
			tmpval = convertTemperature( tmpval, unit );
			rv =  ( tmpval & 0x00ff );
			break;

		// Temperature low alarm point
		case 0x1A:
			rv = readEEPROM( EEPROM_TEMP_LOW_ALARMPOINT );
			break;

		// Temperature high alarm point
		case 0x1B:
			rv = readEEPROM( EEPROM_TEMP_HIGH_ALARMPOINT );
			break;

		// Humidity low alarm point
		case 0x1C:
			rv = readEEPROM( EEPROM_HUMIDITY_LOW_ALARMPOINT );
			break;

		// Humidity high alarm point
		case 0x1D:
			rv = readEEPROM( EEPROM_HUMIDITY_HIGH_ALARMPOINT );
			break;

		// Temperature sensor error counter
		case 0x1E:
			rv = error_cnt_temp;
			break;

		// Humidity sensor error counter
		case 0x1F:
			rv = error_cnt_hum;
			break;

		// Sensor status register
		case 0x20:
			sht_read_statusreg( &val, &checksum );
			rv = val;
			break;

		// Temperature event interval
		case 0x21:
			rv = readEEPROM( EEPROM_INTERVAL_TEMP );
			break;

		// Humidity event interval
		case 0x22:
			rv = readEEPROM( EEPROM_INTERVAL_HUMIDITY );
			break;

		// Dew Point event interval
		case 0x23:
			rv = readEEPROM( EEPROM_INTERVAL_DEW );
			break;

		// Temperature format
		case 0x24:
			rv = readEEPROM( EEPROM_TEMP_UNIT );
			break;

		// MSB of temperature calibration 
		case 0x25:
			rv = readEEPROM( EEPROM_TEMP_CALIBRATION );
			break;

		// LSB of temperature calibration 
		case 0x26:
			rv = readEEPROM( EEPROM_TEMP_CALIBRATION + 1 );
			break;

		// MSB of humidity calibration 
		case 0x27:
			rv = readEEPROM( EEPROM_HUM_CALIBRATION );
			break;

		// LSB of humidity calibration 
		case 0x28:
			rv = readEEPROM( EEPROM_HUM_CALIBRATION + 1 );
			break;

		// Hysteresis for temperature alarm
		case 0x29:
			rv = readEEPROM( EEPROM_TEMP_HYSTERESIS );
			break;

		// Hysteresis for humidity alarm
		case 0x2A:
			rv = readEEPROM( EEPROM_HUM_HYSTERESIS );
			break;

		default:
			rv = 0;	
			break;
	}
	
	return rv;

}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeAppReg
//

uint8_t vscp_writeAppReg( unsigned char reg, unsigned char val )
{
    uint8_t rv;
	int tmpval;
	uint8_t checksum;
	uint8_t unit = readEEPROM( EEPROM_TEMP_UNIT );

	rv = ~val; // error return

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
			
		// MSB of absolut high temp
		case 0x08:
			writeEEPROM( ( EEPROM_ABSMAX_TEMP + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_TEMP, 0xff );
			rv = 0xff;
			break;

		// LSB of absolut high temp
		case 0x09:
			writeEEPROM( ( EEPROM_ABSMAX_TEMP + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_TEMP, 0xff );
			rv = 0xff;
			break;

		// MSB of absolut low temp
		case 0x0A:
			writeEEPROM( ( EEPROM_ABSMIN_TEMP + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_TEMP, 0xff );
			rv = 0x7f;
			break;

		// LSB of absolut low temp
		case 0x0B:
			writeEEPROM( ( EEPROM_ABSMIN_TEMP + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_TEMP, 0xff );
			rv = 0xff;
			break;

		// MSB of absolut high humidity
		case 0x0C:
			writeEEPROM( ( EEPROM_ABSMAX_HUMIDITY + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_HUMIDITY, 0xff );
			rv = 0xff;
			break;

		// LSB of absolut high humidity
		case 0x0D:
			writeEEPROM( ( EEPROM_ABSMAX_HUMIDITY + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_HUMIDITY, 0xff );
			rv = 0xff;
			break;

		// MSB of absolut low humidity
		case 0x0E:
			writeEEPROM( ( EEPROM_ABSMIN_HUMIDITY + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_HUMIDITY, 0xff );
			rv = 0x7f;
			break;

		// LSB of absolut low humidity
		case 0x0F:
			writeEEPROM( ( EEPROM_ABSMIN_HUMIDITY + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_HUMIDITY, 0xff );
			rv = 0xff;
			break;

		// MSB of absolut high dew point
		case 0x10:
			writeEEPROM( ( EEPROM_ABSMAX_DEW + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_DEW, 0xff );
			rv = 0xff;
			break;

		// LSB of absolut high dew point
		case 0x11:
			writeEEPROM( ( EEPROM_ABSMAX_DEW + 1 ), 0xff );
			writeEEPROM( EEPROM_ABSMAX_DEW, 0xff );
			rv = 0xff;
			break;

		// MSB of absolut low dew point
		case 0x12:
			writeEEPROM( ( EEPROM_ABSMIN_DEW + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_DEW, 0xff );
			rv = 0x7f;
			break;

		// LSB of absolut low dew point
		case 0x13:
			writeEEPROM( ( EEPROM_ABSMIN_DEW + 1 ), 0x7f );
			writeEEPROM( EEPROM_ABSMIN_DEW, 0xff );
			rv = 0xff;
			break;

		// MSB of running mean temperature
		case 0x14:
			mean_temp = 0;
			rv = 0x00;
			break;

		// LSB of running mean temperature
		case 0x15:
			mean_temp = 0;
			rv = 0x00;
			break;

		// MSB of running mean of humidity
		case 0x16:			
			mean_hum = 0;
			rv = 0x00;
			break;

		// LSB of running mean of humidity
		case 0x17:			
			mean_hum = 0;
			rv = 0x00;
			break;

		// MSB of running mean od dew point
		case 0x18:
			mean_dew = 0;
			rv = 0x00;
			break;

		// LSB of running mean of dew point
		case 0x19:
			mean_dew = 0;
			rv = 0x00;
			break;

		// Temperature low alarm point
		case 0x1A:
			writeEEPROM( EEPROM_TEMP_LOW_ALARMPOINT, val ); 
			rv = readEEPROM( EEPROM_TEMP_LOW_ALARMPOINT );
			break;

		// Temperature high alarm point
		case 0x1B:
			writeEEPROM( EEPROM_TEMP_HIGH_ALARMPOINT, val ); 
			rv = readEEPROM( EEPROM_TEMP_HIGH_ALARMPOINT );
			break;

		// Humidity low alarm point
		case 0x1C:
			writeEEPROM( EEPROM_HUMIDITY_LOW_ALARMPOINT, val ); 
			rv = readEEPROM( EEPROM_HUMIDITY_LOW_ALARMPOINT );
			break;

		// Humidity high alarm point
		case 0x1D:
			writeEEPROM( EEPROM_HUMIDITY_HIGH_ALARMPOINT, val ); 
			rv = readEEPROM( EEPROM_HUMIDITY_HIGH_ALARMPOINT );
			break;

		// Temperature sensor error counter
		case 0x1E:
			error_cnt_temp = 0;
			rv = 0x00;
			break;

		// Humidity sensor error counter
		case 0x1F:
			error_cnt_hum = 0;
			rv = 0x00;
			break;

		// Sensor status register
		case 0x20:
			val &= ( val & 4 );	// Only heater bit
			sht_write_statusreg( &val );
			sht_read_statusreg( &val, &checksum );
			rv = val;
			break;

		// Temperature event interval
		case 0x21:
			writeEEPROM( EEPROM_INTERVAL_TEMP, val );		
			rv = readEEPROM( EEPROM_INTERVAL_TEMP );
			break;

		// Humidity event interval
		case 0x22:
			writeEEPROM( EEPROM_INTERVAL_HUMIDITY, val );
			rv = readEEPROM( EEPROM_INTERVAL_HUMIDITY );
			break;

		// Dew Point event interval
		case 0x23:
			writeEEPROM( EEPROM_INTERVAL_DEW, val );
			rv = readEEPROM( EEPROM_INTERVAL_DEW );
			break;

		// Temperature format
		case 0x24:
			writeEEPROM( EEPROM_TEMP_UNIT, val );
			rv = readEEPROM( EEPROM_TEMP_UNIT );
			break;

		// MSB of temperature calibration 
		case 0x25:
			writeEEPROM( EEPROM_TEMP_CALIBRATION, val );
			rv = readEEPROM( EEPROM_TEMP_CALIBRATION );
			break;

		// LSB of temperature calibration 
		case 0x26:
			writeEEPROM( EEPROM_TEMP_CALIBRATION + 1, val );
			rv = readEEPROM( EEPROM_TEMP_CALIBRATION + 1 );
			break;

		// MSB of humidity calibration 
		case 0x27:
			writeEEPROM( EEPROM_HUM_CALIBRATION, val );
			rv = readEEPROM( EEPROM_HUM_CALIBRATION );
			break;

		// LSB of humidity calibration 
		case 0x28:
			writeEEPROM( EEPROM_HUM_CALIBRATION + 1, val );
			rv = readEEPROM( EEPROM_HUM_CALIBRATION + 1 );
			break;

		// Hysteresis for temperature alarm
		case 0x29:
			writeEEPROM( EEPROM_TEMP_HYSTERESIS, val );
			rv = readEEPROM( EEPROM_TEMP_HYSTERESIS );
			break;

		// Hysteresis for humidity alarm
		case 0x2A:
			writeEEPROM( EEPROM_HUM_HYSTERESIS, val );
			rv = readEEPROM( EEPROM_HUM_HYSTERESIS );
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
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

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
//  getNickname
//

uint8_t vscp_readNicknamePermanent( void )
{
	return readEEPROM( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
//  setNickname
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
