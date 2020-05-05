/* This is an modification of the demo_vscp_node_can128, credits to Akhe
 * see original header below this file
 * goal is to implement a module for home automation including:
 * - 16 output (on/off)	
 * - a number of analog output	
 * - bootloader support
 *
 * hardware supported:
 * custom board, AAmega 0.0
 *
 * version 0.0.1
 * 29/08/2011
 *
 * Sven Zwiers
 *
 * more information about VSCP: http://www.vscp.org
 *---------------------------------------------------------------------------
*/
volatile uint16_t io_timer;			// 1 ms timer counter
volatile unsigned int VSCP_USER_TIMER[9] = {0} ;				//actual timer
volatile unsigned int VSCP_USER_TIMER_PRESCALER[9] = {1};	//prescaler. increases user_timer when 0
volatile unsigned int VSCP_USER_SHUTTER_ACTUAL[5] = {0} ;				//actual position
volatile unsigned int VSCP_USER_SHUTTER_WANTED[5] = {0} ;				//wanted position
volatile unsigned char latest_direction=0;	
	

// # of elements in decision matrix
#define DESCION_MATRIX_ELEMENTS             11

// Start register position for decision matrix
//#define REG_DESCION_MATRIX                  112		// should equal the MDF!


// EEPROM Storage
#define VSCP_EEPROM_BOOTLOADER_FLAG         0x00	// Reserved for bootloader	 

#define VSCP_EEPROM_NICKNAME                0x01	// Persistant nickname id storage
#define VSCP_EEPROM_SEGMENT_CRC             0x02	// Persistant segment crc storage
#define VSCP_EEPROM_CONTROL                 0x03	// Persistant control byte

#define VSCP_EEPROM_REG_USERID              0x04
#define VSCP_EEPROM_REG_USERID1             0x05
#define VSCP_EEPROM_REG_USERID2             0x06
#define VSCP_EEPROM_REG_USERID3             0x07
#define VSCP_EEPROM_REG_USERID4             0x08

// The following can be stored in flash or eeprom

#define VSCP_EEPROM_REG_MANUFACTUR_ID0      0x09
#define VSCP_EEPROM_REG_MANUFACTUR_ID1      0x0A
#define VSCP_EEPROM_REG_MANUFACTUR_ID2      0x0B
#define VSCP_EEPROM_REG_MANUFACTUR_ID3      0x0C

#define VSCP_EEPROM_REG_MANUFACTUR_SUBID0   0x0D	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID1   0x0E	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID2   0x0F	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID3   0x10

#define VSCP_EEPROM_REGISTER	            0x11	//define the start of memory where registers 0 through 0x7F are stored

//#define VSCP_EEPROM_REG_SUBZONE             0x12

// The following can be stored in program ROM (recommended) or in EEPROM 

#define VSCP_EEPROM_REG_GUID                0x11    // Start of GUID MSB	
                                                    // 		0x11 - 0x20		not used		

#define VSCP_EEPROM_REG_DEVICE_URL          0x21	// Start of Device URL storage
                                                    // 		0x21 - 0x40		not used

//#define VSCP_EEPROM_END                   	VSCP_EEPROM_REGISTER + REG_END	// marks end of VSCP EEPROM usage












// Prototypes
void SendInformationEvent( uint8_t idx, uint8_t eventClass, uint8_t eventTypeId );;
int readEEPROM( uint16_t addr );
int writeEEPROM( uint16_t addr, uint8_t data );

void SendInformationEventExtended
(uint8_t priority, uint8_t zone, uint8_t subzone, uint8_t idx,
uint8_t eventClass, uint8_t eventTypeId );

void SendInformationEventExtendedData
(uint8_t priority, uint8_t zone, uint8_t subzone, uint8_t idx, uint8_t data, uint8_t eventClass, uint8_t eventTypeId );

unsigned char CheckBit (unsigned char x, unsigned char i);

// debug routines
void sendCharHex (char data);
void sendChar (char data);



/* original header****************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2006-04-21
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 2006-2009 Ake Hedman, eurosource
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
