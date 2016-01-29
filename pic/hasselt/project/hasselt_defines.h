/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2009-11-08
 * 	kurt.sidekick@yahoo.com
 *
 *  Copyright (C) 2009 Kurt Herremans
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

#ifndef HASSELT
#define HASSELT

// * * * I/O Definitions * * *
#define BUTTON1								PORTAbits.RA5
#define BUTTON2								PORTAbits.RA4
#define BUTTON3								PORTAbits.RA3
#define BUTTON4								PORTAbits.RA2
#define BUTTON5								PORTAbits.RA1
#define BUTTON6								PORTAbits.RA0

#define DEBOUNCE_CHECKS 5 // # checks before a switch is debounced

#define BUTTON_RELEASED						0x04
#define BUTTON_PRESSED						0x01
#define BUTTON_HOLD							0x03
#define BUTTON_HOLD_REPEAT					0x05
#define BUTTON_KEY							0x02


#define INIT_BUTTON							PORTBbits.RB0
#define INIT_LED							PORTBbits.RB4

#define OUT1								PORTCbits.RC3
#define OUT2								PORTCbits.RC1
#define OUT3								PORTCbits.RC0
#define OUT4								PORTCbits.RC6
#define OUT5								PORTCbits.RC5
#define OUT6								PORTCbits.RC4
#define OUT_PWM								PORTCbits.RC2

#define ONEWIRE								PORTCbits.RC7

// * * *  Registers * * *
#define EEPROM_ZONE							0x00	// Zone node belongs to
#define EEPROM_SUBZONE						0x01	// Subzone node belongs to

#define REG_RELAY_STATE0					2
#define REG_RELAY_STATE1					3
#define REG_RELAY_STATE2					4
#define REG_RELAY_STATE3					5
#define REG_RELAY_STATE4					6
#define REG_RELAY_STATE5					7
#define REG_RELAY_STATE6					8
#define REG_RELAY_STATE7					9

#define REG_BUTTON_SUBZONE0					10
#define REG_BUTTON_SUBZONE1					11
#define REG_BUTTON_SUBZONE2					12
#define REG_BUTTON_SUBZONE3					13
#define REG_BUTTON_SUBZONE4					14
#define REG_BUTTON_SUBZONE5					15

#define REG_RELAY1_SUBZONE					50	
#define REG_RELAY2_SUBZONE					51
#define REG_RELAY3_SUBZONE 					52
#define REG_RELAY4_SUBZONE 					53
#define REG_RELAY5_SUBZONE 					54
#define REG_RELAY6_SUBZONE 					55
#define REG_RELAY7_SUBZONE 					56
#define REG_RELAY8_SUBZONE 					57


#define EEPROM_LAST_POS						0x60    // Last used EEPROM position
// --------------------------------------------------------------------------------

#define REG_DESCION_MATRIX					72	// Start of matrix
#define DESCION_MATRIX_ELEMENTS				7

// * * * Actions * * *
#define ACTION_NOOP							0
#define ACTION_ON							1
#define ACTION_OFF							2
#define ACTION_TOGGLE						4

#endif
