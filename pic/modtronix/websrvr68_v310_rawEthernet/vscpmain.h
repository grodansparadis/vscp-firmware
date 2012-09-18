/* ******************************************************************************
 * VSCP task for the rawEthernet reference project
 *
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Copyright (C) 1995-2012 Ake Hedman, Grodans Paradis
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

#ifndef VSCP_VSCPTASK_H
#define VSCP_VSCPTASK_H

// Read VSCP_FIRMWARE_MAJOR_VERSION register
// D:\dev\can\can\src\canal\delivery>cancmd --level=2 --class=0 --type=9 --data="0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0x00,0x00,0xa3,0x04,0x00,0x90,0x01,0x94"

// Write VSCP_FIRMWARE_MAJOR_VERSION register
// D:\dev\can\can\src\canal\delivery>cancmd --level=2 --class=0 --type=0x0b --data="0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0x00,0x00,0xa3,0x04,0x00,0x90,0x01,0x92,0x12"

#include <crc.h>

// Version
#define FIRMWARE_MAJOR_VERSION                          0x00
#define FIRMWARE_MINOR_VERSION                          0x00
#define FIRMWARE_SUB_MINOR_VERSION                      0x02

#define VSCP_REG_MODULE_ZONE				0x00
#define VSCP_REG_MODULE_SUBZONE				0x01
#define VSCP_REG_MODULE_CONTROL				0x03
#define VSCP_REG_EVENT_INTERVAL				0x04
#define VSCP_REG_INPUT_STATUS				0x05
#define VSCP_REG_OUTPUT_STATUS				0x06
#define VSCP_REG_ADCTRL_EVENT_PERIODCTRL		0x07
#define VSCP_REG_ADCTRL_ALARM_LOW			0x08
#define VSCP_REG_ADCTRL_ALARM_HIGH			0x09
#define VSCP_REG_OUTPUT_CONTROL0			0x0A
#define VSCP_REG_OUTPUT_CONTROL1			0x0B
#define VSCP_REG_OUTPUT_CONTROL2			0x0C
#define VSCP_REG_OUTPUT_CONTROL3			0x0D
#define VSCP_REG_OUTPUT_CONTROL4			0x0E
#define VSCP_REG_OUTPUT_CONTROL5			0x0F
#define VSCP_REG_OUTPUT_CONTROL6			0x10
#define VSCP_REG_OUTPUT_CONTROL7			0x11

#define VSCP_REG_INPUT_CONTROL0				0x12
#define VSCP_REG_INPUT_CONTROL1				0x13
#define VSCP_REG_INPUT_CONTROL2				0x14
#define VSCP_REG_INPUT_CONTROL3				0x15
#define VSCP_REG_INPUT_CONTROL4				0x16
#define VSCP_REG_INPUT_CONTROL5				0x17
#define VSCP_REG_INPUT_CONTROL6				0x18
#define VSCP_REG_INPUT_CONTROL7				0x19

#define VSCP_REG_OUTPUT_PROTECTION_TIME0_MSB		0x1A
#define VSCP_REG_OUTPUT_PROTECTION_TIME0_LSB		0x1B
#define VSCP_REG_OUTPUT_PROTECTION_TIME1_MSB		0x1C
#define VSCP_REG_OUTPUT_PROTECTION_TIME1_LSB		0x1D
#define VSCP_REG_OUTPUT_PROTECTION_TIME2_MSB		0x1E
#define VSCP_REG_OUTPUT_PROTECTION_TIME2_LSB		0x1F
#define VSCP_REG_OUTPUT_PROTECTION_TIME3_MSB		0x20
#define VSCP_REG_OUTPUT_PROTECTION_TIME3_LSB		0x21
#define VSCP_REG_OUTPUT_PROTECTION_TIME4_MSB		0x22
#define VSCP_REG_OUTPUT_PROTECTION_TIME4_LSB		0x23
#define VSCP_REG_OUTPUT_PROTECTION_TIME5_MSB		0x24
#define VSCP_REG_OUTPUT_PROTECTION_TIME5_LSB		0x25
#define VSCP_REG_OUTPUT_PROTECTION_TIME6_MSB		0x26
#define VSCP_REG_OUTPUT_PROTECTION_TIME6_LSB		0x27
#define VSCP_REG_OUTPUT_PROTECTION_TIME7_MSB		0x28
#define VSCP_REG_OUTPUT_PROTECTION_TIME7_LSB		0x29

#define VSCP_REG_ANALOG0_LSB                            0x2A
#define VSCP_REG_ANALOG0_MSB                            0x2B
#define VSCP_REG_ANALOG1_LSB				0x2C
#define VSCP_REG_ANALOG1_MSB				0x2D
#define VSCP_REG_ANALOG2_LSB				0x2E
#define VSCP_REG_ANALOG2_MSB				0x2F
#define VSCP_REG_ANALOG3_LSB				0x30
#define VSCP_REG_ANALOG3_MSB				0x31
#define VSCP_REG_ANALOG4_LSB				0x32
#define VSCP_REG_ANALOG4_MSB				0x33
#define VSCP_REG_ANALOG5_LSB				0x34
#define VSCP_REG_ANALOG5_MSB				0x35
#define VSCP_REG_ANALOG6_LSB				0x36
#define VSCP_REG_ANALOG6_MSB				0x37
#define VSCP_REG_ANALOG7_LSB				0x38
#define VSCP_REG_ANALOG7_MSB				0x39

#define VSCP_REG_PWM_MSB							0x42
#define VSCP_REG_PWM_LSB							0x43

#define VSCP_REG_SERIAL_BAUDRATE			0x44
#define VSCP_REG_SERIAL_CONTROL				0x46

#define VSCP_REG_ANALOG_MIN0_MSB			0x47
#define VSCP_REG_ANALOG_MIN0_LSB			0x48
#define VSCP_REG_ANALOG_MIN1_MSB			0x49
#define VSCP_REG_ANALOG_MIN1_LSB			0x4A
#define VSCP_REG_ANALOG_MIN2_MSB			0x4B
#define VSCP_REG_ANALOG_MIN2_LSB			0x4C
#define VSCP_REG_ANALOG_MIN3_MSB			0x4D
#define VSCP_REG_ANALOG_MIN3_LSB			0x4E
#define VSCP_REG_ANALOG_MIN4_MSB			0x4F
#define VSCP_REG_ANALOG_MIN4_LSB			0x50
#define VSCP_REG_ANALOG_MIN5_MSB			0x51
#define VSCP_REG_ANALOG_MIN5_LSB			0x52
#define VSCP_REG_ANALOG_MIN6_MSB			0x53
#define VSCP_REG_ANALOG_MIN6_LSB			0x54
#define VSCP_REG_ANALOG_MIN7_MSB			0x55
#define VSCP_REG_ANALOG_MIN7_LSB			0x56

#define VSCP_REG_ANALOG_MAX0_MSB			0x57
#define VSCP_REG_ANALOG_MAX0_LSB			0x58
#define VSCP_REG_ANALOG_MAX1_MSB			0x59
#define VSCP_REG_ANALOG_MAX1_LSB			0x5A
#define VSCP_REG_ANALOG_MAX2_MSB			0x5B
#define VSCP_REG_ANALOG_MAX2_LSB			0x5C
#define VSCP_REG_ANALOG_MAX3_MSB			0x5D
#define VSCP_REG_ANALOG_MAX3_LSB			0x5E
#define VSCP_REG_ANALOG_MAX4_MSB			0x5F
#define VSCP_REG_ANALOG_MAX4_LSB			0x60
#define VSCP_REG_ANALOG_MAX5_MSB			0x61
#define VSCP_REG_ANALOG_MAX5_LSB			0x62
#define VSCP_REG_ANALOG_MAX6_MSB			0x63
#define VSCP_REG_ANALOG_MAX6_LSB			0x64
#define VSCP_REG_ANALOG_MAX7_MSB			0x65
#define VSCP_REG_ANALOG_MAX7_LSB			0x66

#define VSCP_REG_ANALOG_HYSTERESIS			0x67

#define VSCP_REG_CAN_CONTROL				0x68

#define VSCP_REG_CAN_CLASS_MASK_MSB			0x69
#define VSCP_REG_CAN_CLASS_MASK_LSB			0x6A
#define VSCP_REG_CAN_TYPE_MASK_MSB			0x6B
#define VSCP_REG_CAN_TYPE_MASK_LSB			0x6C

#define VSCP_REG_CAN_CLASS_FILTER_MSB                   0x6D
#define VSCP_REG_CAN_CLASS_FILTER_LSB			0x6E
#define VSCP_REG_CAN_TYPE_FILTER_MSB			0x6F
#define VSCP_REG_CAN_TYPE_FILTER_LSB			0x70 

// The following 15 register positions is used for the CAN
// interface GUID.
#define VSCP_REG_CAN_GUID							0x71


// Control register bits
#define PWM_PERSISTENT_BIT							0x10
#define ENABLE_INPUT_CHANGE							0x20
#define ENABLE_PERIODIC_OUTPUT_EVENTS			0x40
#define ENABLE_PERIODIC_INPUT_EVENTS			0x80

// Serial contrl register bits
#define SERIAL_CRTL_CONFIRM_ENABLE			0x01
#define SERIAL_CRTL_STREAM_OUTPUT_ENABLE		0x10
#define SERIAL_CRTL_STREAM_INPUT_ENABLE			0x20
#define SERIAL_CRTL_OPEN_PORT				0x40
#define SERIAL_CRTL_PERSISTENT_OPEN_PORT		0x80

//Baudrates are in appcfg.h
#define SERIAL_SPEED_DEFAULT				BAUD_57600				

// CAN speed
#define CAN_SPEED_10K								0
#define CAN_SPEED_20K								1
#define CAN_SPEED_50K								2
#define CAN_SPEED_100K								3
#define CAN_SPEED_125K								4
#define CAN_SPEED_250K								5
#define CAN_SPEED_500K								6
#define CAN_SPEED_800K								7
#define CAN_SPEED_1000K								8

#define CAN_SPEED_DEFAULT							CAN_SPEED_500K

// CAN control register bits
#define CAN_CRTL_OPEN_PORT							0x40
#define CAN_CRTL_PERSISTENT_OPEN_PORT			0x80

// Base address for DM matrix in external EEPROM
#define VSCP_DM_MATRIX_BASE							0x200   // base address in internal EEPROM
#define DM_ENTERIES									16		// Number of DM enteries

// Action codes
#define VSCP_ACTION_NOOP							0
#define VSCP_ACTION_OUTPUTS_ACTIVATE			1
#define VSCP_ACTION_OUTPUTS_DEACTIVATE			2
#define VSCP_ACTION_SERIAL_SEND_ARGUMENT		3
#define VSCP_ACTION_SERIAL_SEND_ROM			4
#define VSCP_ACTION_SERIAL_OPEN				5
#define VSCP_ACTION_SERIAL_CLOSE			6
#define VSCP_ACTION_CAN_OPEN				7
#define VSCP_ACTION_CAN_CLOSE				8
#define VSCP_ACTION_CAN_SEND				9
#define VSCP_ACTION_OUTPUT_SEND_STATUS			10
#define VSCP_ACTION_INPUT_SEND_STATUS			11
#define VSCP_ACTION_ANALOG_SEND_STATUS			12
#define VSCP_ACTION_INPUT_SEND_STATUS_IO		13
#define VSCP_ACTION_ANALOG_SEND_STATUS_IO		14

// Prototypes
void vscp_main_task( void );

// Send VSCP heartbeat to show we are alive
void sendHeartBeat(void );

// send High End Server Probe
void sendHighEndServerProbe(void );

// Send enabled periodic output events
void sendPeriodicOutputEvents( void );

// Send enabled periodic input events
void sendPeriodicInputEvents( void );

// Send enabled perodic A/D events.
void sendPeriodicADEvents( uint8_t ch );

// Get state of all input channels
uint8_t getInputState( void );

// Get Current PWM value
uint16_t getPWMValue( void );

// Set PWM value
void setPWMValue( uint16_t value );

// Do Action
void doAction( uint16_t action, PVSCPEVENT pmsg );	

#endif

