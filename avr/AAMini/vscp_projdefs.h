/* This is an modification of the demo_vscp_node_can128, credits to Akhe
 * see original header below this file
 * goal is to implement a small module for home automation including:
 * - 8 input (on/off)		done
 * - 8 output (on/off)		done
 * - IR capable receiver
 * - piezzo buzzer
 * - a number of analog input
 * - a number of analog output	
 * - bootloader support
 *
 * hardware supported:
 * olimex AVR-CAN board
 * custom board, AAmini 0.0, 0.1 will be published as openhardware
 *
 * version 0.0.1
 * 29/08/2010
 *
 * Sven Zwiers
 *
 * more information about VSCP: http://www.vscp.org
 *---------------------------------------------------------------------------
*/
#ifndef CAN128_TEST_PROJDEFS_H
#define CAN128_TEST_PROJDEFS_H

#include <avr/io.h>





//
// Undefine one of the following suitable for the
// platform you want to compile for 
//defined in makefile
//#define ATMEL_STK501
//#define OLIMEX_AT90CAN128
//#define BOARD AAMINI00


// System clock in kHz
//defined in makefile
//#define FOSC                16000  // Available value: 22118 16000 12000 11059 custom 
//#define FOSC                14300  // Available value: 22118 16000 12000 11059 custom 

// UART channel to use 
#define USE_UART1 
//#define USE_UART2

// Baudrate
#define BAUDRAT             9600 
 

#ifndef FALSE
#define FALSE               0
#ifndef TRUE
#define TRUE                !FALSE
#endif
#endif

#ifndef ON
#define ON                  !FALSE
#endif

#ifndef OFF
#define OFF                 FALSE
#endif

#ifndef BIT
#define BIT(n)              (1 << (n))
#endif   

#ifndef BOOL
#define BOOL                int
#endif


// VSCP Defines
// ============
//
// Described in vscp.h

// If GUID should be in EEPROM define the following
//#define GUID_IN_EEPROM

// If manufacturing D and GUID should bw possible to write when page_select = 0xffff
// undef the following
#define ENABLE_WRITE_2PROTECTED_LOCATIONS

// Disable initial nickname discovery by undefining the following
//#define DISABLE_AUTOMATIC_NICKNAME_DISCOVERY

#endif



/* original header****************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Copyright (C) 1995-2009 Ake Hedman, eurosource, <akhe@eurosource.se>
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
