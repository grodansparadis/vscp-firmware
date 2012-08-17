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

#ifndef BUTTON_H
#define BUTTON_H

// * * * Button states (for debounce)* * *
#define BUTTON_STATE_RELEASED				0
#define BUTTON_STATE_TRANS1  				1
#define BUTTON_STATE_TRANS2  				2
#define BUTTON_STATE_TRANS3  				3
#define BUTTON_STATE_TRANS4  				4
#define BUTTON_STATE_PUSHED  				5

#define BUTTON_MASK_STATE					0x0F
#define BUTTON_MASK_CLICK					0xF0

// * * * Button control bits * * *
#define BUTTON_CONTROLBIT_PRESS				0x01 	// Send press event	
#define BUTTON_CONTROLBIT_RELEASE			0x02 	// Send release event
#define BUTTON_CONTROLBIT_SLAVE				0x04 	// Slave mode bit (1=follow button)
#define BUTTON_CONTROLBIT_FLANK				0x08 	// Send event on which flank (0=up 1=down)
#define BUTTON_CONTROLBIT_TIMER0			0x10 	// Type of Timer bit 0	
#define BUTTON_CONTROLBIT_TIMER1			0x20 	// Type of Timer bit 1	
#define BUTTON_CONTROLBIT_TIMER2			0x40 	// Type of Timer bit 2	
#define BUTTON_CONTROLBIT_INVERT			0x80 	// Invert the input-signal


// * * *  Button Registers * * *

#define REG_BUTTON1_SUBZONE					2
#define REG_BUTTON2_SUBZONE					3
#define REG_BUTTON3_SUBZONE					4
#define REG_BUTTON4_SUBZONE					5
#define REG_BUTTON5_SUBZONE					6
#define REG_BUTTON6_SUBZONE					7
#define REG_BUTTON7_SUBZONE					8
#define REG_BUTTON8_SUBZONE					9
#define REG_BUTTON9_SUBZONE					10
#define REG_BUTTON10_SUBZONE				11
#define REG_BUTTON11_SUBZONE				12
#define REG_BUTTON12_SUBZONE				13
#define REG_BUTTON13_SUBZONE				14
#define REG_BUTTON14_SUBZONE				15
#define REG_BUTTON15_SUBZONE				16

#define REG_BUTTON1_ZONE					17
#define REG_BUTTON2_ZONE					18
#define REG_BUTTON3_ZONE					19
#define REG_BUTTON4_ZONE					20
#define REG_BUTTON5_ZONE					21
#define REG_BUTTON6_ZONE					22
#define REG_BUTTON7_ZONE					23
#define REG_BUTTON8_ZONE					24
#define REG_BUTTON9_ZONE					25
#define REG_BUTTON10_ZONE			 		26
#define REG_BUTTON11_ZONE					27
#define REG_BUTTON12_ZONE					28
#define REG_BUTTON13_ZONE					29
#define REG_BUTTON14_ZONE					30
#define REG_BUTTON15_ZONE					31

#define REG_BUTTON_CONTROL1					32
#define REG_BUTTON_CONTROL2					33
#define REG_BUTTON_CONTROL3					34
#define REG_BUTTON_CONTROL4					35
#define REG_BUTTON_CONTROL5					36
#define REG_BUTTON_CONTROL6					37
#define REG_BUTTON_CONTROL7					38
#define REG_BUTTON_CONTROL8					39
#define REG_BUTTON_CONTROL9					40
#define REG_BUTTON_CONTROL10				41
#define REG_BUTTON_CONTROL11				42
#define REG_BUTTON_CONTROL12				43
#define REG_BUTTON_CONTROL13				44
#define REG_BUTTON_CONTROL14				45
#define REG_BUTTON_CONTROL15				46

#define REG_BUTTON_TIMEOUT1					47
#define REG_BUTTON_TIMEOUT2					48
#define REG_BUTTON_TIMEOUT3					49
#define REG_BUTTON_TIMEOUT4					50
#define REG_BUTTON_TIMEOUT5					51
#define REG_BUTTON_TIMEOUT6					52
#define REG_BUTTON_TIMEOUT7					53
#define REG_BUTTON_TIMEOUT8					54
#define REG_BUTTON_TIMEOUT9					55
#define REG_BUTTON_TIMEOUT10				56
#define REG_BUTTON_TIMEOUT11				57
#define REG_BUTTON_TIMEOUT12				58
#define REG_BUTTON_TIMEOUT13				59
#define REG_BUTTON_TIMEOUT14				60
#define REG_BUTTON_TIMEOUT15				61

#define REG_BUTTON_STATUS1_7				62  // Vitual register, only used to read the button input status
#define REG_BUTTON_STATUS9_15				63  // Vitual register, only used to read the button input status

// * * * Persistent storage
#define EEPROM_ZONE							0x00	// Zone node belongs to
#define EEPROM_SUBZONE						0x01	// Subzone node belongs to

#define EEPROM_LAST_POS						0x60    // Value of last used EEPROM pos. Used for checking reg read/write range

// --------------------------------------------------------------------------------

#define REG_DESCION_MATRIX					72	// Start of matrix
#define DESCION_MATRIX_ELEMENTS				7

// * * * Actions * * *
#define ACTION_NOOP							0
// 1-9 reserved
#define ACTION_STATUS						10
// 11-15 reserved
#define ACTION_DISABLE						16

// * * * I/O Definitions * * *
#define BUTTON1								PORTCbits.RC0
#define BUTTON2								PORTCbits.RC1
#define BUTTON3								PORTCbits.RC2
#define BUTTON4								PORTCbits.RC3
#define BUTTON5								PORTCbits.RC4
#define BUTTON6								PORTCbits.RC5
#define BUTTON7								PORTCbits.RC6
//#define BUTTON8								PORTCbits.RC7 // not connected
#define BUTTON1_7							PORTC

#define BUTTON9								PORTAbits.RA0
#define BUTTON10							PORTAbits.RA1
#define BUTTON11							PORTAbits.RA2
#define BUTTON12							PORTAbits.RA3
#define BUTTON13							PORTAbits.RA4
#define BUTTON14							PORTAbits.RA5
#define BUTTON9_14							PORTA

#define BUTTON15							PORTBbits.RB1

#define INIT_BUTTON							PORTBbits.RB0
#define INIT_LED							PORTBbits.RB4

#endif
