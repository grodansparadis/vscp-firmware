// File:  main.h

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2004-09-18
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

#ifndef RFID_H
#define RFID_H

// * * * Version * * *
// 2005-09-27
//#define FIRMWARE_MAJOR_VERSION				0
//#define FIRMWARE_MINOR_VERSION				0
//#define FIRMWARE_SUB_MINOR_VERSION 			1
// moved to version.h AKHE

// eeprom registers
#define REG_ZONE						    0
#define REG_SUBZONE                         1
#define REG_SUBZONE_LED_RED					2	
#define REG_SUBZONE_LED_GREEN				3
#define REG_SUBZONE_KEY 					4
#define REG_RED_LED_ON_TIME					5
#define REG_GREEN_LED_ON_TIME				6
#define REG_KEY_ON_TIME                     7
#define REG_DEV_CONFIG                      8

// ram registers
#define REG_PWM_VALUE                       9
#define REG_PWM_ADC                         10

// CARD data
#define REG_EMDATA0                        11
#define REG_EMDATA1                        12
#define REG_EMDATA2                        13
#define REG_EMDATA3                        14
#define REG_EMDATA4                        15
#define REG_EMDATA5                        16

#define REG_DESCION_MATRIX					72	// Start of matrix
#define DESCION_MATRIX_ELEMENTS				7

// MASKS
#define MASK_GREEN_LED                      1
#define MASK_BEEP                           2

// * * * Actions * * *
#define ACTION_NOOP							0
#define ACTION_ON							1
#define ACTION_OFF							2
#define ACTION_PULSE						3
// 4-9 reserved
#define ACTION_STATUS						10
// 11-15 reserved
#define ACTION_DISABLE						16


// EEPROM storage 
#define EEPROM_ZONE							0x66	// Zone node belongs to
#define EEPROM_SUBZONE						0x67	// Subzone node belongs to
#define EEPROM_SUBZONE_LED_RED				0x68
#define EEPROM_SUBZONE_LED_GREEN			0x69
#define EEPROM_SUBZONE_KEY			        0x6A
#define EEPROM_ONTIME_LED_RED			    0x6B
#define EEPROM_ONTIME_LED_GREEN			    0x6C
#define EEPROM_ONTIME_KEY			        0x6D
#define EEPROM_CONFIG			            0x6E

#endif

