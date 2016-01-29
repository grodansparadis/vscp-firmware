/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *
 *  Copyright (C) 1995-2007 Ake Hedman, eurosource
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

#include "../../../../../common/compiler.h"

#define FIRMWARE_MAJOR_VERSION 			0
#define FIRMWARE_MINOR_VERSION			0
#define FIRMWARE_SUB_MINOR_VERSION		1

// Defines
#define	TRUE			1
#define	FALSE			0

// System oscillator frequency
#define CLOCK_FREQ 		(40000000)
//#define CLOCK_FREQ    (20000000)      // Hz
//#define CLOCK_FREQ    (10000000)      // Hz


// Serial baudrate
#define USART_USE_BRGH_LOW
#define BAUD_RATE		9600


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

#define MAIN_STATE_NONE			0  	// waiting for "AT"
#define MAIN_STATE_ATTENTION	1	// "A" received, waiting for "T"
#define MAIN_STATE_COMMAND		2	// "AT" received

#define DLE		0x10
#define STX		0x02
#define ETX		0x03

#define INCOMING_STATE_NONE		0	// Waiting for <STX>
#define INCOMING_STATE_STX		1	// Reading data
#define INCOMING_STATE_ETX		2	// <ETX> has been received

#define INCOMING_SUBSTATE_NONE	0	// Idle
#define INCOMING_SUBSTATE_DLE	1	// <DLE> received

#define OUTGOING_STATE_NONE		0	// Waiting for CAN packet


void init( void );
void init_app_ram( void );
void init_app_eeprom( void );
unsigned char getMajorVersion( void );
unsigned char getMinorVersion( void );
unsigned char getSubMinorVersion( void );
void serPutByteSubst( BYTE b );
