/* ******************************************************************************
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


// Module Version
#define THIS_MODULE_MAJOR_VERSION		0
#define THIS_MODULE_MINOR_VERSION		0
#define THIS_MODULE_SUBMINOR_VERSION	1

// # of elements in decision matrix
#define DESCION_MATRIX_ELEMENTS			10

// Start register position for decision matrix
#define REG_DESCION_MATRIX              0x80-(DESCION_MATRIX_ELEMENTS * 8)

// EEPROM Storage, dont use Byte 0

#define VSCP_EEPROM_BOOTLOADER_FLAG       0x01	// Reserved for bootloader

#define VSCP_EEPROM_NICKNAME              0x02	// Persistant nickname id storage
#define VSCP_EEPROM_SEGMENT_CRC           0x03	// Persistant segment crc storage
#define VSCP_EEPROM_CONTROL               0x04	// Persistant control byte

#define VSCP_EEPROM_REG_USERID            0x05
#define VSCP_EEPROM_REG_USERID1           0x06
#define VSCP_EEPROM_REG_USERID2           0x07
#define VSCP_EEPROM_REG_USERID3           0x08
#define VSCP_EEPROM_REG_USERID4           0x09

// The following can be stored in flash or eeprom

#define VSCP_EEPROM_REG_MANUFACTUR_ID0    0x0A
#define VSCP_EEPROM_REG_MANUFACTUR_ID1    0x0B
#define VSCP_EEPROM_REG_MANUFACTUR_ID2    0x0C
#define VSCP_EEPROM_REG_MANUFACTUR_ID3    0x0D

#define VSCP_EEPROM_REG_MANUFACTUR_SUBID0 0x0E	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID1 0x0F	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID2 0x10	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID3 0x11

#define VSCP_EEPROM_REG_ZONE              0x12

#define VSCP_EEPROM_REG_SUBZONE           0x13

// The following can be stored in program ROM (recommended) or in EEPROM 

#define VSCP_EEPROM_REG_GUID              0x14	// Start of GUID MSB	
                                                // 		0x11 - 0x20				

#define VSCP_EEPROM_REG_DEVICE_URL        0x24	// Start of Device URL storage
                                                // 		0x21 - 0x40

#define VSCP_EEPROM_END                   0x44	// marks end of VSCP EEPROM usage
                                                //   (next free position)

// Prototypes
void SendInformationEvent( uint8_t idx, uint8_t eventClass, uint8_t eventTypeId );

void init_app_eeprom( void );
void doDM( void );

int readEEPROM( uint8_t addr );
int writeEEPROM( uint8_t addr, uint8_t data );

void array_write(uint8_t* array, uint16_t pos,  uint16_t bytes, void* data);
void array_read (uint8_t* array, uint16_t pos,  uint16_t bytes, void* data);

//stefans new routines
void LCD_Backlight( unsigned char level);
// defines for the LCD backlight driver
#define LCD_BL_OMASK 128
#define LCD_BL_PORT PORTB
#define LCD_BL_DDR DDRB
#define LCD_BL_UMASK 127
#define LCD_BL_BIT 7
