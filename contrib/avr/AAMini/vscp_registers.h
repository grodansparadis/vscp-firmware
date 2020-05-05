/* This is an modification of the demo_vscp_node_can128, credits to Akhe
 * see original header below this file
 * goal is to implement a small module for home automation including:
 * - 8 input (on/off)		done
 * - 8 output (on/off)		done
 * - IR capable receiver
 * - piezzo buzzer
 * - a number of analog input
 * - a number of analog output	
 * - bootloader support
 *
 * hardware supported:
 * olimex AVR-CAN board
 * custom board, AAmini 0.0, 0.1 will be published as openhardware
 *
 * version 0.0.1
 * 29/08/2010
 *
 * Sven Zwiers
 *
 * more information about VSCP: http://www.vscp.org
 *---------------------------------------------------------------------------
*/




// Register offset for local storage


// This marks the space for the decision matrix start. It
// is just a dummy for EEPROM storage and do not represent the
// actual register position
//page 0

#define REG_DM_START                   0
#define REG_END							(REG_DM_START + (DESCION_MATRIX_ELEMENTS)*16)
#define PAGE0_END						(REG_DM_START + (DESCION_MATRIX_ELEMENTS)*16)

//page 1
// USer registers
#define PAGE1_START						(PAGE0_END+1)
#define REG_ZONE						(PAGE1_START+0)
#define REG_SUBZONE						(PAGE1_START+1)
#define REG_SWITCH1_SUBZONE             (PAGE1_START+2)
#define REG_SWITCH2_SUBZONE             (PAGE1_START+3)
#define REG_SWITCH3_SUBZONE             (PAGE1_START+4)
#define REG_SWITCH4_SUBZONE             (PAGE1_START+5)
#define REG_SWITCH5_SUBZONE             (PAGE1_START+6)
#define REG_SWITCH6_SUBZONE             (PAGE1_START+7)
#define REG_SWITCH7_SUBZONE             (PAGE1_START+8)
#define REG_SWITCH8_SUBZONE             (PAGE1_START+9)
#define REG_OUTPUT1_SUBZONE             (PAGE1_START+10)
#define REG_OUTPUT2_SUBZONE             (PAGE1_START+11)
#define REG_OUTPUT3_SUBZONE             (PAGE1_START+12)
#define REG_OUTPUT4_SUBZONE             (PAGE1_START+13)
#define REG_OUTPUT5_SUBZONE             (PAGE1_START+14)
#define REG_OUTPUT6_SUBZONE             (PAGE1_START+15)
#define REG_OUTPUT7_SUBZONE             (PAGE1_START+16)
#define REG_OUTPUT8_SUBZONE             (PAGE1_START+17)
#define REG_OUTPUT_STATE	            (PAGE1_START+18) //eeprom location is never written to, handled in readAppreg routine
#define REG_OUTPUT_DEFAULT_STATE 	    (PAGE1_START+19)
#define REG_INPUT_DEBOUNCE			    (PAGE1_START+20) //contains the input debounce time (minimum for "short" push) not used anymore (for now)
#define REG_INPUT_START					(PAGE1_START+21) //and when a push is considered a "long" push --- not used anymore
#define REG_FOLLOWZONE					(PAGE1_START+22) //option follow zone/subzone
#define REG_INPUTBEHAVIOUR				(PAGE1_START+23) //defines input as button or state
#define REG_OUTPUT_CHANGE_FILTER		(PAGE1_START+24) //filters event to be send or not on output changes
#define REG_GENERAL_CONFIG				(PAGE1_START+25) //bit 0= enable id blink
#define enable_blink_ID					0x01
#define REG_SW1_SHORT_CLASS				(PAGE1_START+32) //sw1 
#define REG_SW1_SHORT_TYPE				(PAGE1_START+33) 
#define REG_SW1_LONG_CLASS				(PAGE1_START+34) 
#define REG_SW1_LONG_TYPE				(PAGE1_START+35)
#define REG_SW1_LONG_TYPE_RELEASE		(PAGE1_START+36)
#define REG_SW2_SHORT_CLASS				(REG_SW1_SHORT_CLASS+5) //sw2
#define REG_SW2_SHORT_TYPE				(REG_SW1_SHORT_TYPE+5)
#define REG_SW2_LONG_CLASS				(REG_SW1_LONG_CLASS+5)
#define REG_SW2_LONG_TYPE				(REG_SW1_LONG_TYPE+5)
#define REG_SW2_LONG_TYPE_RELEASE		(REG_SW1_LONG_TYPE_RELEASE+5)
#define REG_SW3_SHORT_CLASS				(REG_SW1_SHORT_CLASS+10) //sw3
#define REG_SW3_SHORT_TYPE				(REG_SW1_SHORT_TYPE+10)
#define REG_SW3_LONG_CLASS				(REG_SW1_LONG_CLASS+10)
#define REG_SW3_LONG_TYPE				(REG_SW1_LONG_TYPE+10)
#define REG_SW3_LONG_TYPE_RELEASE		(REG_SW1_LONG_TYPE_RELEASE+10)
#define REG_SW4_SHORT_CLASS				(REG_SW1_SHORT_CLASS+15) //sw4
#define REG_SW4_SHORT_TYPE				(REG_SW1_SHORT_TYPE+15)
#define REG_SW4_LONG_CLASS				(REG_SW1_LONG_CLASS+15)
#define REG_SW4_LONG_TYPE				(REG_SW1_LONG_TYPE+15)
#define REG_SW4_LONG_TYPE_RELEASE		(REG_SW1_LONG_TYPE_RELEASE+15)
#define REG_SW5_SHORT_CLASS				(REG_SW1_SHORT_CLASS+20) //sw5
#define REG_SW5_SHORT_TYPE				(REG_SW1_SHORT_TYPE+20)
#define REG_SW5_LONG_CLASS				(REG_SW1_LONG_CLASS+20)
#define REG_SW5_LONG_TYPE				(REG_SW1_LONG_TYPE+20)
#define REG_SW5_LONG_TYPE_RELEASE		(REG_SW1_LONG_TYPE_RELEASE+20)
#define REG_SW6_SHORT_CLASS				(REG_SW1_SHORT_CLASS+25) //sw6
#define REG_SW6_SHORT_TYPE				(REG_SW1_SHORT_TYPE+25)
#define REG_SW6_LONG_CLASS				(REG_SW1_LONG_CLASS+25)
#define REG_SW6_LONG_TYPE				(REG_SW1_LONG_TYPE+25)
#define REG_SW6_LONG_TYPE_RELEASE		(REG_SW1_LONG_TYPE_RELEASE+25)
#define REG_SW7_SHORT_CLASS				(REG_SW1_SHORT_CLASS+30) //sw7
#define REG_SW7_SHORT_TYPE				(REG_SW1_SHORT_TYPE+30)
#define REG_SW7_LONG_CLASS				(REG_SW1_LONG_CLASS+30)
#define REG_SW7_LONG_TYPE				(REG_SW1_LONG_TYPE+30)
#define REG_SW7_LONG_TYPE_RELEASE		(REG_SW1_LONG_TYPE_RELEASE+30)
#define REG_SW8_SHORT_CLASS				(REG_SW1_SHORT_CLASS+35) //sw8
#define REG_SW8_SHORT_TYPE				(REG_SW1_SHORT_TYPE+35)
#define REG_SW8_LONG_CLASS				(REG_SW1_LONG_CLASS+35)
#define REG_SW8_LONG_TYPE				(REG_SW1_LONG_TYPE+35)
#define REG_SW8_LONG_TYPE_RELEASE		(REG_SW1_LONG_TYPE_RELEASE+35)


#define REG_ZONE_END					(REG_SW8_LONG_TYPE_RELEASE+1)
#define PAGE1_END						(REG_ZONE_END)


// system registers
#define RegisterPagesUsed				2  
















/* original header******************************************************************************
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
