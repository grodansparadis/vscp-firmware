/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Kelvin Smart II Module
 * 	Version 0.02 2005-01-31
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2005 Ake Hedman, eurosource
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
#include "../../../common/eeprom.h"
#include "../../../common/can18xx8.h"
#include "../../../common/can18f.h"
#include "../../../common/vscp.h" 
#include "../../../common/vscp_class.h"
#include "../../../common/vscp_type.h"
#include "main.h" 
#include "smartec.h"

//__IDLOC(1);
//__CONFIG(INTIO & WDTDIS & MCLRDIS & BORDIS & UNPROTECT & PWRTEN);

// ISR Routines
void isr_low( void );

extern const rom int log_Table[];			// log10 for Hummidity measurement

// Global Variable Declarations
BYTE nSensor;								// Sensor to do next measurement on

BYTE tarray_idx0;							// idx for next temperature write sensor 0
int tarray0[ TEMPERATURE_ARRAY_SIZE  ];		// Temperature measurement array sensor 0

BYTE tarray_idx1;							// idx for next temperature write sensor 1
int tarray1[ TEMPERATURE_ARRAY_SIZE  ];		// Temperature measurement array sensor 1

int temperature0_array[ 24 ];				// 24-hour period mean value sensor 0
int temperature1_array[ 24 ];				// 24-hour period mean value sensor 1

unsigned char error_cnt_tempsensor0;		// Error counter for temp1 sensor reads			
unsigned char error_cnt_tempsensor1;		// Error counter for temp2 sensor reads 

volatile unsigned long measurement_clock;	// Clock for measurments

BYTE seconds;								// counter for seconds
BYTE minutes;								// counter for minutes
BYTE hours;									// Counter for hours

unsigned char seconds_temperature0;				// timer for temp1 event
unsigned char seconds_temperature1;				// timer for temp2 event

// bit 7 - high temp1 alarm sent
// bit 6 - high temp2 alarm sent
// bit 5 - low temp1 alarm sent
// bit 4 - low temp2 alarm sent
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

//#pragma code low_vector = 0x218
#pragma code low_vector = 0x18

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
	init();                             // Initialize Microcontroller	
	
	// Check VSCP persistent storage and
	// restore if needed
	if ( !vscp_check_pstorage() ) {
		
		// Spoiled or not initialized - reinitialize
		init_app_eeprom();	

	}

	vscp_init();						// Initialize the VSCP functionality

	while ( 1 ) {                     	// Loop Forever	
		
		ClrWdt();

		if ( ( vscp_initbtncnt > 100 ) && ( STATE_INIT != vscp_node_state ) ) {
			// Init button pressed
			vscp_nickname = VSCP_ADDRESS_FREE;
			writeEEPROM( EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
			vscp_init();
		}

		// Check for any valid CAN message
		vscp_imsg.flag = 0;
		vscp_receiveMsg();

		// do a meaurement if needed
		if ( measurement_clock > 1000 ) {
			
			measurement_clock = 0;
			doMeasurement();
			seconds++;
			
			// Temperature report timers are only updated if in active 
			// state
			if ( STATE_ACTIVE == vscp_node_state  ) {
				seconds_temperature0++;
				seconds_temperature1++;
			}

			if ( seconds > 59 ) {
			
				seconds = 0;
				minutes++;
				
				if ( minutes > 59 ) {
					minutes = 0;
					hours++;
				}
				
				if ( hours > 23 ) hours = 0;
				
				// If we have errors in sensor readings
				// send out error event
				if ( error_cnt_tempsensor0 || error_cnt_tempsensor1 ) {
					sendErrorEvent( ERROR_SENSOR );
				}
			}				
		}

		switch ( vscp_node_state ) {

			case STATE_STARTUP:			// Cold/warm reset

				// Get nickname from EEPROM
				if ( VSCP_ADDRESS_FREE == vscp_nickname ) {
					// new on segment need a nickname
					vscp_node_state = STATE_INIT; 	
				}
				else {
					// been here before - go on
					vscp_node_state = STATE_ACTIVE;
					vscp_active();
				}
				break;

			case STATE_INIT:			// Assigning nickname
				vscp_probe();
				break;

			case STATE_PREACTIVE:		// Waiting for host initialisation
				vscp_rcv_preactive();					
				break;

			case STATE_ACTIVE:			// The normal state
				
				if ( vscp_imsg.flag & VSCP_VALID_MSG ) {	// incoming message?
					
					// Yes, incoming message
					if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.class ) {
						switch( vscp_imsg.type ) {

							case VSCP_TYPE_SEGCTRL_HEARTBEAT:
								vscp_rcv_heartbeat();
								break;

							case VSCP_TYPE_NEW_NODE_ONLINE:
								vscp_rcv_new_node_online();
								break;
							
							case VSCP_TYPE_SET_NICKNAME:
								vscp_rcv_set_nickname();
								break;
							
							case VSCP_TYPE_DROP_NICKNAME:
								vscp_rcv_drop_nickname();
								break;

							case VSCP_TYPE_READ_REGISTER:
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

							case VSCP_TYPE_WRITE_REGISTER:
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

							default:
								// Do work load
								break;
						}							
					}
				}
				break;

			case STATE_ERROR:			// Everything is *very* *very* bad.
				vscp_error();
				break;

			default:					// Should not be here...
				vscp_node_state = STATE_STARTUP;
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
	measurement_clock = 0;	// start a new meaurement cycle
	
	seconds = 0;
	minutes = 0;
	hours = 0;
	
	seconds_temperature0 = 0;
	seconds_temperature1 = 0;

	alarm_flags = 0;		// no alarms yet

	okreadings = 0;				
	
	error_cnt_tempsensor0 = 0;		// no errors in temp readings
	error_cnt_tempsensor1 = 0;		// no errors in temp readings

	nSensor = 0;					// start with sensor 0
	
	tarray_idx0 = 0;				// Index into temperature 0 array
	tarray_idx1 = 0;				// Index into temperature 1 array
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
}

///////////////////////////////////////////////////////////////////////////////
// doWork
//
// The actual work is done here.
//

void doWork( void )
{
	unsigned char tmp;

	if ( STATE_ACTIVE == vscp_node_state ) {	
		
		// Time for temperature report sensor 1
		tmp = readEEPROM( EEPROM_INTERVAL_TEMP0 );
		if ( tmp && ( seconds_temperature0 > tmp ) ) {
			
			vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
			vscp_omsg.flag = VSCP_VALID_MSG + 4;
			vscp_omsg.class = VSCP_CLASS1_MEASUREMENT;
			vscp_omsg.type = VSCP_TYPE_MEASUREMENT_TEMPERATURE;
			vscp_omsg.data[ 0 ] = 0x80 + ( readEEPROM( EEPROM_TEMP_UNIT ) << 3 );
			vscp_omsg.data[ 1 ] = 0x02; // Exponent
			
			setEventData( getCurrentTemp4Sensor0() + readEEPROM( EEPROM_TEMP0_CALIBRATION ), readEEPROM( EEPROM_TEMP_UNIT ) );

			// Send data
			if ( vscp_sendMsg() ) {
				seconds_temperature0 = 0;	
			}
		}

		// Time for temperature report sensor 2
		tmp = readEEPROM( EEPROM_INTERVAL_TEMP1 );
		if ( tmp && ( seconds_temperature1 > tmp ) ) {
			
			vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
			vscp_omsg.flag = VSCP_VALID_MSG + 4;
			vscp_omsg.class = VSCP_CLASS1_MEASUREMENT;
			vscp_omsg.type = VSCP_TYPE_MEASUREMENT_TEMPERATURE;
			vscp_omsg.data[ 0 ] = 0x80 + ( readEEPROM( EEPROM_TEMP_UNIT ) << 3 ) + 1;
			vscp_omsg.data[ 1 ] = 0x02; // Exponent
			
			setEventData( getCurrentTemp4Sensor1() + readEEPROM( EEPROM_TEMP0_CALIBRATION ), readEEPROM( EEPROM_TEMP_UNIT ) );

			// Send data
			if ( vscp_sendMsg() ) {
				seconds_temperature1 = 0;	
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
// doMeasurement
//
// The following formula is used
// Temperature = ( 65536 * d.c. - 20922 ) / 309
//

void doMeasurement( void ) 
{		
	unsigned long cnt;
	volatile unsigned long high;
	volatile unsigned long low;
	
	cnt = 0x20000;
	high = 0;
	low = 0;
	
	if ( 0 == nSensor ) {
		
		// Sensor 0
		nSensor = 1;	// nest is sensor 1
		
		while ( cnt-- ) {
			if ( PORTBbits.RB0 ) {
				high++;
			}
			else {
				low++;
			}
		}
 
		tarray0[ tarray_idx0++  ] = 100 * ( high/2 - 20922 ) / 309 ;
		if ( tarray_idx0 > TEMPERATURE_ARRAY_SIZE ) tarray_idx0 = 0;
		temperature0_array[ hours ] = ( temperature0_array[ hours ] + getCurrentTemp4Sensor0() ) / 2;
		
		cnt	= 100 * ( high/2 - 20922 ) / 309 ;
		cnt = 1;
	}
	else {
		
		// Sensor 1
		while ( cnt-- ) {
			if ( PORTBbits.RB1 ) {
				high++;
			}
			else {
				low++;
			}
		}
		
		nSensor = 0;	// nest is sensor 0
		tarray1[ tarray_idx1++  ] = 100 * ( high/2 - 20922 ) / 309 ;
		if ( tarray_idx1 > TEMPERATURE_ARRAY_SIZE ) tarray_idx1 = 0;
		temperature1_array[ hours ] = ( temperature1_array[ hours ] + getCurrentTemp4Sensor1() ) / 2;
		
		cnt	= 100 * ( high/2 - 20922 ) / 309 ;
		cnt = 1;
	}
				
}


///////////////////////////////////////////////////////////////////////////////
// getCurrentTemp4Sensor0
//
// returns the mean of the allocated measurement values
//

int getCurrentTemp4Sensor0( void )
{
	BYTE i;
	long temp;
	
	temp = 0;
	for ( i=0; i<TEMPERATURE_ARRAY_SIZE; i++ ) {
		temp += tarray0[ i ];
	}
	
	return ( (int)( temp / TEMPERATURE_ARRAY_SIZE ) );
}

///////////////////////////////////////////////////////////////////////////////
// getCurrentTemp4Sensor1
//
// returns the mean of the allocated measurement values
//

int getCurrentTemp4Sensor1( void )
{
	BYTE i;
	long temp;
	
	temp = 0;
	for ( i=0; i<TEMPERATURE_ARRAY_SIZE; i++ ) {
		temp += tarray1[ i ];
	}
	
	return ( (int)( temp / TEMPERATURE_ARRAY_SIZE ) );
}

///////////////////////////////////////////////////////////////////////////////
// get24HourMean4Sensor0
//
// returns the mean of the allocated 24 hour measurement values
//

int get24HourMean4Sensor0( void )
{
	BYTE i;
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
	BYTE i;
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

int sendAlarmEvent( unsigned char nAlarm )
{
	vscp_omsg.priority = VSCP_PRIORITY_HIGH;
	vscp_omsg.flag = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_ALARM;
	vscp_omsg.type = VSCP_TYPE_ALARM_ALARM;
	vscp_omsg.data[ 0 ] = nAlarm;
	vscp_omsg.data[ 1 ] = vscp_alarmstatus;

	// Send data
	if ( !vscp_sendMsg() ) {
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
	vscp_omsg.flag = VSCP_VALID_MSG + 1;
	vscp_omsg.class = VSCP_CLASS1_INFORMATION;
	vscp_omsg.type = VSCP_TYPE_EVENT_ERROR;
	vscp_omsg.data[ 0 ] = nError;

	// Send data
	if ( !vscp_sendMsg() ) {
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
// read_app_register
//

void read_app_register( unsigned char reg )
{
	int tmpval;
	unsigned char val, checksum;
	unsigned char unit = readEEPROM( EEPROM_TEMP_UNIT );

	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flag = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = reg;
	vscp_omsg.data[ 1 ] = 0x00; // default read

	switch ( reg ) {

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
	 
		default:
			vscp_omsg.data[ 1 ] = 0;	
			break;
	}

	// Send data
	vscp_sendMsg();

}

///////////////////////////////////////////////////////////////////////////////
// write_app_register
//

void write_app_register( unsigned char reg, unsigned char val )
{
	BYTE i;
	int tmpval;
	unsigned char checksum;
	unsigned char unit = readEEPROM( EEPROM_TEMP_UNIT );

	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flag = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = reg;
	vscp_omsg.data[ 1 ] = ~val; // error return

	switch ( reg ) {

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

		default:
			vscp_omsg.data[ 1 ] = ~val; // error return	
			break;
	}

	// Send data
	vscp_sendMsg();	
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
