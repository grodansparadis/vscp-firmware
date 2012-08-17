/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2004-08-25
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2003 Ake Hedman, eurosource
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

// $RCSfile: humidity.h,v $                                       
// $Date: 2006/03/16 16:43:06 $                                  
// $Author: akhe $                                              
// $Revision: 1.4 $ 

//
// EEPROM locations for persistant storage
//

#include "../../../../common/inttypes.h"

#define EEPROM_REG_ABSMAXTEMP			0x20	// Absolute max temperature
#define EEPROM_REG_ABSMINTEMP			0x21	// Absolute min temperature
#define EEPROM_REG_ALARMTEMPLOW			0x22	// Alarm low temperature
#define EEPROM_REG_ALARMTEMPHIGH		0x23	// Alarm high temperature
 
#define EEPROM_REG_ABSMAXHUMIDITY		0x24	// Absolute max humidity
#define EEPROM_REG_ABSMINHUMIDITY		0x25	// Absolute min humidity
#define EEPROM_REG_ALARMHUMIDITYLOW		0x26	// Alarm low humidity
#define EEPROM_REG_ALARMHUMIDITYHIGH	0x27	// Alarm high humidity

#define EEPROM_REG_TEMP_INTERVAL		0x28	// Interval for temp sends
#define EEPROM_REG_HUMIDITY_INTERVAL	0x29	// Interval for Humidity sends



//
// Alarm bits 
//

// Alarm flags and status bits
#define ALARM_TEMP_HIGH						0x80
#define ALARM_TEMP_LOW						0x40
#define ALARM_HUMIDITY_HIGH					0x20
#define ALARM_HUMIDITY_LOW					0x10

#define ALARM_EVENT_TYPE_GENERAL			0x00
#define ALARM_EVENT_TYPE_TEMP_HIGH			0x01
#define ALARM_EVENT_TYPE_TEMP_LOW			0x02
#define ALARM_EVENT_TYPE_HUMIDITY_HIGH		0x03
#define ALARM_EVENT_TYPE_HUMIDITY_LOW		0x04


//
// Error
//

#define ERROR_GENERAL						0x00
#define ERROR_SENSOR						0x01
#define ERROR_MODULE						0x02

// Storage for module persistent data

#define EEPROM_ABSMAX_TEMP					0x11	// Int - absloute max temperature (R/W)

#define EEPROM_ABSMIN_TEMP					0x13	// Int - absolute min temperature (R/W)

#define EEPROM_ABSMAX_HUMIDITY				0x15	// Int - absolute max humidity (R/W)

#define EEPROM_ABSMIN_HUMIDITY				0x17	// Int - absolute min humidity (R/W)

#define EEPROM_ABSMAX_DEW					0x19	// Int - absolute max dew point (R/W)
	
#define EEPROM_ABSMIN_DEW					0x1B	// Int - absolute min dew point (R/W)

#define EEPROM_TEMP_LOW_ALARMPOINT			0x1D	// Byte - low alarm trigging point (R/W)

#define EEPROM_TEMP_HIGH_ALARMPOINT			0x1E	// Byte - high alarm trigging point (R/W)

#define EEPROM_HUMIDITY_LOW_ALARMPOINT		0x1F	// Byte - low alarm trigging point (R/W)

#define EEPROM_HUMIDITY_HIGH_ALARMPOINT		0x20	// Byte - high alarm trigging point (R/W)

#define EEPROM_INTERVAL_TEMP 				0x21	// Byte - update interval (s) temperature (R/W)

#define EEPROM_INTERVAL_HUMIDITY 			0x22	// Byte - update interval (s) humidity (R/W)

#define EEPROM_INTERVAL_DEW 				0x23	// Byte - update interval (s) dew point (R/W)

#define EEPROM_TEMP_UNIT					0x24	// Byte - Unit for temperature
													// 0x00 - Celsius
													// 0x01 - Farenheit
													// 0x02 - Kelvin

#define EEPROM_TEMP_CALIBRATION				0x25	// Int - calibration constant for temperature
													// default is zero

#define EEPROM_HUM_CALIBRATION				0x27	// Int - calibration constant for humidity
													// default is zero

#define EEPROM_TEMP_HYSTERESIS 				0x29	// Byte - Temp alarm hysteresis (R/W).

#define EEPROM_HUM_HYSTERESIS 				0x2A	// Byte - Hum alarm hysteresis (R/W).

 
#define EEPROM_ZONE							0x2B	// Zone node belongs to
#define EEPROM_SUBZONE						0x2C	// Subzone node belongs to

/*!
	Send Extended ID CAN frame
	@param id CAN extended ID for frame.
	@param size Number of databytes 0-8
	@param pData Pointer to databytes of frame.
	@return TRUE (!=0) on success, FALSE (==0) on failure.
*/
int8_t sendCANFrame( uint32_t id, uint8_t size, uint8_t *pData );

/*!
	Get extended ID CAN frame
	@param pid Pointer to CAN extended ID for frame.
	@param psize Pointer to number of databytes 0-8
	@param pData Pointer to databytes of frame.
	@return TRUE (!=0) on success, FALSE (==0) on failure.
*/
int8_t getCANFrame( uint32_t *pid, uint8_t *psize, uint8_t *pData );


