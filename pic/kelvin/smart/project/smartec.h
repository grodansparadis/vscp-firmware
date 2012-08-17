/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: smartec.h
 *
 *  Copyright (C) 1995-2007 Ake Hedman, eurosource, <akhe@eurosource.se>
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

#ifndef KELVIN_SMART_H
#define KELVIN_SMART_H

// # loops to measure
#define DUMP_MEASUREMENT_COUNT	0x20000


//
// EEPROM locations for persistant storage
//

#define EEPROM_REG_ABSMAXTEMP1			0x40	// Absolute max temperature 1
#define EEPROM_REG_ABSMAXTEMP2			0x41	// Absolute max temperature 2
#define EEPROM_REG_ABSMINTEMP1			0x42	// Absolute min temperature 1
#define EEPROM_REG_ABSMINTEMP2			0x43	// Absolute min temperature 2
#define EEPROM_REG_ALARMTEMP1LOW		0x44	// Alarm low temperature 1
#define EEPROM_REG_ALARMTEMP2LOW		0x45	// Alarm low temperature 2
#define EEPROM_REG_ALARMTEMP1HIGH		0x46	// Alarm high temperature 1
#define EEPROM_REG_ALARMTEMP2HIGH		0x47	// Alarm high temperature 2
 
#define EEPROM_REG_TEMP0_INTERVAL		0x48	// Interval for temp 1 sends
#define EEPROM_REG_TEMP2_INTERVAL		0x49	// Interval for temp 2 sends 

//
// Alarm bits 
//

// Alarm flags and status bits
#define ALARM_TEMP1_HIGH					0x80
#define ALARM_TEMP2_HIGH					0x40
#define ALARM_TEMP1_LOW						0x20
#define ALARM_TEMP2_LOW						0x10

#define ALARM_EVENT_TYPE_GENERAL			0x00
#define ALARM_EVENT_TYPE_TEMP_HIGH			0x01
#define ALARM_EVENT_TYPE_TEMP_LOW			0x02


//
// Error codes
//

#define ERROR_GENERAL						0x00
#define ERROR_SENSOR						0x01
#define ERROR_MODULE						0x02

// Storage for module persistent data

#define EEPROM_ABSMAX_TEMP0					0x51	// Int - absloute max temperature (R/W)

#define EEPROM_ABSMAX_TEMP1					0x53	// Int - absloute max temperature (R/W)

#define EEPROM_ABSMIN_TEMP0					0x55	// Int - absolute min temperature (R/W)

#define EEPROM_ABSMIN_TEMP1					0x57	// Int - absolute min temperature (R/W)

#define EEPROM_TEMP0_LOW_ALARMPOINT			0x59	// Byte - low alarm trigging point (R/W)

#define EEPROM_TEMP1_LOW_ALARMPOINT			0x5A	// Byte - low alarm trigging point (R/W)

#define EEPROM_TEMP0_HIGH_ALARMPOINT		0x5B	// Byte - high alarm trigging point (R/W)

#define EEPROM_TEMP1_HIGH_ALARMPOINT		0x5C	// Byte - high alarm trigging point (R/W)

#define EEPROM_INTERVAL_TEMP0 				0x5D	// Byte - update interval (s) temperature (R/W)

#define EEPROM_INTERVAL_TEMP1 				0x5E	// Byte - update interval (s) temperature (R/W)

#define EEPROM_TEMP_UNIT					0x5F	// Byte - Unit for temperature
													// 0x00 - Celsius
													// 0x01 - Farenheit
													// 0x02 - Kelvin

#define EEPROM_TEMP0_CALIBRATION			0x60	// Int - calibration constant for temperature
													// default is zero

#define EEPROM_TEMP1_CALIBRATION			0x62	// Int - calibration constant for temperature
													// default is zero

#define EEPROM_TEMP0_HYSTERESIS 			0x64	// Byte - Temp alarm hysteresis (R/W).

#define EEPROM_TEMP1_HYSTERESIS 			0x65	// Byte - Temp alarm hysteresis (R/W).

#define EEPROM_ZONE							0x66	// Zone node belongs to
#define EEPROM_SUBZONE						0x67	// Subzone node belongs to



#define TEMP_UNIT_KELVIN 		0
#define TEMP_UNIT_CELSIUS		1
#define TEMP_UNIT_FAHRENHEIT	2

//
// Prototypes
//
long Celsius2Fahrenheit( long tc );
long Fahrenheit2Celsius( long tf );
long Celsius2Kelvin( long tc );
long Kelvin2Celsius( long tf ); 

#endif
