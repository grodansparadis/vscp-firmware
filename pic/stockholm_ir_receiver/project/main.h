// File:  main.h

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2004-08-25
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

#ifndef MAIN_H
#define MAIN_H

//Defines
#define	TRUE			1
#define	FALSE			0

//
// 8 MHz with PLL => 8 MHz
// 1:4 prescaler => 1 MHz (1 uS cycle )
// 1 ms == 1000 uS 
// 65535 - 1000 = 64535 = 0xfc17
//
// Timer2 use 250 and prescaler 1:4
//
//#define TIMER0_RELOAD_VALUE		0xfc17

//
// 10 MHz with PLL => 40 MHz
// 1:4 prescaler => 1.25 MHz ( 0.800 uS cycle )
// 1 ms == 1000 uS 
// 65535 - 1250 = 64285 = 0xfb1d
//

//#define TIMER0_RELOAD_VALUE		0xfb1d

//
// Timer 2 is used as a 1 ms clock
// 156 is loaded eight time to give ~1250 cycles
// Timer2 use 156 and prescaler 1:4, Postscaler 1:16
// 100 ns * 56 * 4 * 16 ~ 1 ms
//
#define TIMER2_RELOAD_VALUE		156

//
// Size for array used for temp mean calculations
//
#define TEMPERATURE_ARRAY_SIZE 	0x0A

// Function Prototypes

void doWork( void );
void init( void );
void init_app_ram( void );
void init_app_eeprom( void ); 
void doMeasurement( void );
int getCurrentTemp4Sensor0( void );
int getCurrentTemp4Sensor1( void );
int get24HourMean4Sensor0( void );
int get24HourMean4Sensor1( void );
int convertTemperature( int temp, unsigned char unit );
void setEventData( int v, unsigned char unit );
int sendAlarmEvent( unsigned char nAlarm );
int sendErrorEvent( unsigned char nError );
void read_app_register( unsigned char reg );
void write_app_register( unsigned char reg, unsigned char val );

#endif
