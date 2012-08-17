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

#ifndef SMARTRELAY_H
#define SMARTRELAY_H

// * * * Relay states * * *
#define RELAY_STATE_INACTIVE				0
#define RELAY_STATE_ACTIVE					1

// * * * Relay control bits * * *
#define RELAY_CONTROLBIT_TIMER0				0x01 	// Type of Timer bit 0	
#define RELAY_CONTROLBIT_TIMER1				0x02 	// Type of Timer bit 1	
#define RELAY_CONTROLBIT_TIMER2				0x04 	// Type of Timer bit 2	
#define RELAY_CONTROLBIT_ALARM				0x08 	// Send alarm on protection time activation
#define RELAY_CONTROLBIT_PROTECTION			0x10 	// Enable protection timer
#define RELAY_CONTROLBIT_ONEVENT			0x20 	// Send ON event
#define RELAY_CONTROLBIT_OFFEVENT			0x40 	// Send OFF event
#define RELAY_CONTROLBIT_ENABLED			0x80 	// Relay enabled

// * * *  Relay Registers * * *
#define REG_RELAY_STATE0_7					2
#define REG_RELAY_STATE8_14					3

#define REG_RELAY_CONTROL0					4
#define REG_RELAY_CONTROL1					5
#define REG_RELAY_CONTROL2					6
#define REG_RELAY_CONTROL3					7
#define REG_RELAY_CONTROL4					8
#define REG_RELAY_CONTROL5					9
#define REG_RELAY_CONTROL6					10
#define REG_RELAY_CONTROL7					11
#define REG_RELAY_CONTROL8					12
#define REG_RELAY_CONTROL9					13
#define REG_RELAY_CONTROL10					14
#define REG_RELAY_CONTROL11					15
#define REG_RELAY_CONTROL12					16
#define REG_RELAY_CONTROL13					17
#define REG_RELAY_CONTROL14					18

#define REG_RELAY_PROTECTION_TIME0			19
#define REG_RELAY_PROTECTION_TIME1			20
#define REG_RELAY_PROTECTION_TIME2			21
#define REG_RELAY_PROTECTION_TIME3			22
#define REG_RELAY_PROTECTION_TIME4			23
#define REG_RELAY_PROTECTION_TIME5			24
#define REG_RELAY_PROTECTION_TIME6			25
#define REG_RELAY_PROTECTION_TIME7			26
#define REG_RELAY_PROTECTION_TIME8			27
#define REG_RELAY_PROTECTION_TIME9			28
#define REG_RELAY_PROTECTION_TIME10			29
#define REG_RELAY_PROTECTION_TIME11			30
#define REG_RELAY_PROTECTION_TIME12			31
#define REG_RELAY_PROTECTION_TIME13			32
#define REG_RELAY_PROTECTION_TIME14			33

#define REG_RELAY1_SUBZONE			    34
#define REG_RELAY2_SUBZONE			    35
#define REG_RELAY3_SUBZONE			    36
#define REG_RELAY4_SUBZONE			    37
#define REG_RELAY5_SUBZONE			    38
#define REG_RELAY6_SUBZONE			    39
#define REG_RELAY7_SUBZONE			    40
#define REG_RELAY8_SUBZONE			    41
#define REG_RELAY9_SUBZONE			    42
#define REG_RELAY10_SUBZONE			    43
#define REG_RELAY11_SUBZONE			    44
#define REG_RELAY12_SUBZONE			    45
#define REG_RELAY13_SUBZONE			    46
#define REG_RELAY14_SUBZONE			    47
#define REG_RELAY15_SUBZONE			    48	

#define REG_RELAY1_ZONE   				49
#define REG_RELAY2_ZONE 				50
#define REG_RELAY3_ZONE 				51
#define REG_RELAY4_ZONE 				52
#define REG_RELAY5_ZONE 				53
#define REG_RELAY6_ZONE	 				54
#define REG_RELAY7_ZONE					55	
#define REG_RELAY8_ZONE					56
#define REG_RELAY9_ZONE 				57
#define REG_RELAY10_ZONE				58
#define REG_RELAY11_ZONE				59
#define REG_RELAY12_ZONE				60
#define REG_RELAY13_ZONE				61
#define REG_RELAY14_ZONE				62
#define REG_RELAY15_ZONE				63

// * * * Persistent storage
#define EEPROM_ZONE						0x00	// Zone node belongs to
#define EEPROM_SUBZONE					0x01	// Subzone node belongs to

#define EEPROM_LAST_POS						0x60    // Value of last used EEPROM pos. Used for checking reg read/write range

// --------------------------------------------------------------------------------

#define REG_DESCION_MATRIX					72	// Start of matrix
#define DESCION_MATRIX_ELEMENTS				7

// * * * Actions * * *
#define ACTION_NOOP							0
#define ACTION_ON_1							1
#define ACTION_ON_2							2
#define ACTION_OFF_1						3
#define ACTION_OFF_2						4
// 5 reserved
#define ACTION_TOGGLE_1						6
#define ACTION_TOGGLE_2						7
// 8-9 reserved
#define ACTION_STATUS_1						10
#define ACTION_STATUS_2						11
// 12-13 reserved
#define ACTION_DISABLE_1					14
#define ACTION_DISABLE_2					15

// * * * I/O Definitions * * *
#define OUT1								PORTCbits.RC0
#define OUT2								PORTCbits.RC1
#define OUT3								PORTCbits.RC2
#define OUT4								PORTCbits.RC3
#define OUT5								PORTCbits.RC4
#define OUT6								PORTCbits.RC5
#define OUT7								PORTCbits.RC6
#define OUT8								PORTCbits.RC7
#define OUT1_8								PORTC

#define OUT9								PORTAbits.RA0
#define OUT10								PORTAbits.RA1
#define OUT11								PORTAbits.RA2
#define OUT12								PORTAbits.RA3
#define OUT13								PORTAbits.RA4
#define OUT14								PORTAbits.RA5
#define OUT9_14								PORTA

#define OUT15								PORTBbits.RB1

#define INIT_BUTTON							PORTBbits.RB0
#define INIT_LED							PORTBbits.RB4

#endif
