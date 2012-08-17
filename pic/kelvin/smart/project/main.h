// File:  main.h

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2004-08-25
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2011 Ake Hedman, eurosource
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

#ifndef KELVIN_SMART_MAIN_H
#define KELVIN_SMART_MAIN_H

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
// 100 ns * 156 * 4 * 16 ~ 1 ms
//
#define TIMER2_RELOAD_VALUE		156

//
// Size for array used for temp mean calculations
//
#define TEMPERATURE_ARRAY_SIZE 	0x0A


// EEPROM Storage
#define VSCP_EEPROM_BOOTLOADER_FLAG				0x00	// Reserved for bootloader	 

#define VSCP_EEPROM_NICKNAME					0x01	// Persistant nickname id storage
#define VSCP_EEPROM_SEGMENT_CRC					0x02	// Persistant segment crc storage
#define VSCP_EEPROM_CONTROL					    0x03	// Persistant control byte

#define VSCP_EEPROM_REG_USERID				  	0x04
#define VSCP_EEPROM_REG_USERID1				  	0x05
#define VSCP_EEPROM_REG_USERID2				  	0x06
#define VSCP_EEPROM_REG_USERID3				  	0x07
#define VSCP_EEPROM_REG_USERID4				  	0x08

// The following can be stored in flash or eeprom

#define VSCP_EEPROM_REG_MANUFACTUR_ID0			0x09
#define VSCP_EEPROM_REG_MANUFACTUR_ID1			0x0A
#define VSCP_EEPROM_REG_MANUFACTUR_ID2			0x0B
#define VSCP_EEPROM_REG_MANUFACTUR_ID3			0x0C

#define VSCP_EEPROM_REG_MANUFACTUR_SUBID0		0x0D	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID1		0x0E	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID2		0x0F	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID3		0x10

// The following can be stored in program ROM (recommended) or in EEPROM 

#define VSCP_EEPROM_REG_GUID				    0x11	// Start of GUID MSB	
													    // 		0x11 - 0x20				

#define VSCP_EEPROM_REG_DEVICE_URL				0x21	// Start of Device URL storage
													    // 		0x21 - 0x40

#define VSCP_EEPROM_VSCP_END				    0x41	// marks end of VSCP EEPROM usage
													    //   (next free position)



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

#endif
