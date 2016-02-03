/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	Version 0.01 2004-04-06
 * 	akhe@eurosource.se
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

// Read VSCP_FIRMWARE_MAJOR_VERSION register
// D:\dev\can\can\src\canal\delivery>cancmd --level=2 --class=0 --type=9 --data="0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0x00,0x00,0xa3,0x04,0x00,0x90,0x01,0x94"

// Write VSCP_FIRMWARE_MAJOR_VERSION register
// D:\dev\can\can\src\canal\delivery>cancmd --level=2 --class=0 --type=0x0b --data="0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0x00,0x00,0xa3,0x04,0x00,0x90,0x01,0x92,0x12"

#include <crc.h>
#include <vscp_2.h>

// EEPROM Storage
#define VSCP_EEPROM_RESERVED					0x00	// Reserved for bootloader	 

#define VSCP_EEPROM_CONTROL						0x01	// Persistant control byte

#define VSCP_EEPROM_REG_USERID					0x02
#define VSCP_EEPROM_REG_USERID1					0x03
#define VSCP_EEPROM_REG_USERID2					0x04
#define VSCP_EEPROM_REG_USERID3					0x05
#define VSCP_EEPROM_REG_USERID4					0x06

// The following can be stored in flash or eeprom

#define VSCP_EEPROM_REG_MANUFACTUR_ID0			0x07
#define VSCP_EEPROM_REG_MANUFACTUR_ID1			0x08
#define VSCP_EEPROM_REG_MANUFACTUR_ID2			0x09
#define VSCP_EEPROM_REG_MANUFACTUR_ID3			0x0a

#define VSCP_EEPROM_REG_MANUFACTUR_SUBID0		0x0b	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID1		0x0c	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID2		0x0d	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID		0x0e

#define VSCP_EEPROM_VSCP_END					0x0f	// marks end of VSCP EEPROM usage

#define VSCP_REGISTER_PORTA_STATE				0x40
#define VSCP_REGISTER_PORTB_STATE				0x41
#define VSCP_REGISTER_PORTC_STATE				0x42
#define VSCP_REGISTER_PORTF_STATE				0x43
#define VSCP_REGISTER_PORTG_STATE				0x44

#define VSCP_ADC0_MSB							0x45
#define VSCP_ADC0_LSB							0x46
#define VSCP_ADC1_MSB							0x47
#define VSCP_ADC1_LSB							0x48
#define VSCP_ADC2_MSB							0x49
#define VSCP_ADC2_LSB							0x4A
#define VSCP_ADC3_MSB							0x4B
#define VSCP_ADC3_LSB							0x4C
#define VSCP_ADC4_MSB							0x4D
#define VSCP_ADC4_LSB							0x4E
#define VSCP_ADC5_MSB							0x4F
#define VSCP_ADC5_LSB							0x50
#define VSCP_ADC6_MSB							0x51
#define VSCP_ADC6_LSB							0x52
#define VSCP_ADC7_MSB							0x53
#define VSCP_ADC7_LSB							0x54
#define VSCP_ADC8_MSB							0x55
#define VSCP_ADC8_LSB							0x56
#define VSCP_ADC9_MSB							0x57
#define VSCP_ADC9_LSB							0x58
#define VSCP_ADC10_MSB							0x59
#define VSCP_ADC10_LSB							0x5A
#define VSCP_ADC11_MSB							0x5B
#define VSCP_ADC11_LSB							0x5C

#define VSCP_PWM_MSB							0x5D
#define VSCP_PWM_LSB							0x5E

#define VSCP_ADC_CONTROL						0x5F

#define VSCP_PORTA_CHANGE_EVENT					0x60
#define VSCP_PORTB_CHANGE_EVENT					0x61
#define VSCP_PORTC_CHANGE_EVENT					0x62
#define VSCP_PORTF_CHANGE_EVENT					0x63
#define VSCP_PORTG_CHANGE_EVENT					0x64

#define VSCP_PORTA_STATUS_EVENT					0x65
#define VSCP_PORTB_STATUS_EVENT					0x66
#define VSCP_PORTC_STATUS_EVENT					0x67
#define VSCP_PORTF_STATUS_EVENT					0x68
#define VSCP_PORTG_STATUS_EVENT					0x69

#define VSCP_ADC_MEASUREMENT_EVENT_MSB			0x6A
#define VSCP_ADC_MEASUREMENT_EVENT_LSB			0x6B

#define VSCP_STATUS_EVENT_INTERVAL_MSB			0x6C
#define VSCP_STATUS_EVENT_INTERVAL_LSB			0x6D

#define VSCP_MEASUREMENT_EVENT_INTERVAL_MSB		0x6E
#define VSCP_MEASUREMENT_EVENT_INTERVAL_LSB		0x6F

#define VSCP_CLASS_ON_EVENT_MSB					0x70
#define VSCP_CLASS_ON_EVENT_LSB					0x71 

#define VSCP_TYPE_ON_EVENT_LSB					0x72

// 0x73 reserved 

#define VSCP_CLASS_OFF_EVENT_MSB				0x74
#define VSCP_CLASS_OFF_EVENT_LSB				0x75 

#define VSCP_TYPE_OFF_EVENT_LSB					0x76

// 0x77 reserved 

#define VSCP_MODULE_ZONE						0x78
#define VSCP_MODULE_SUBZONE						0x79

 
#define VSCP_EVENT_ZONE							0x7A
#define VSCP_EVENT_SUBZONE_BASE					0x7B  




// Prototypes
void vscp2_Task( void );
_u8 getVSCP2MajorVersion( void );
_u8 getVSCP2MinorVersion( void );
_u8 getVSCP2SubMinorVersion( void );
int vscp2_udpinit( void );
int vscp2_sendMsg( PVSCPMSG pmsg );
int vscp2_receiveMsg( PVSCPMSG pmsg );



