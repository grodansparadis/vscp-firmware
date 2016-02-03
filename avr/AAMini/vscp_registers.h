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

// USer registers
#define REG_ZONE						0
#define REG_SUBZONE						1
#define REG_SWITCH1_SUBZONE             2
#define REG_SWITCH2_SUBZONE             3
#define REG_SWITCH3_SUBZONE             4
#define REG_SWITCH4_SUBZONE             5
#define REG_SWITCH5_SUBZONE             6
#define REG_SWITCH6_SUBZONE             7
#define REG_SWITCH7_SUBZONE             8
#define REG_SWITCH8_SUBZONE             9
#define REG_OUTPUT1_SUBZONE             10
#define REG_OUTPUT2_SUBZONE             11
#define REG_OUTPUT3_SUBZONE             12
#define REG_OUTPUT4_SUBZONE             13
#define REG_OUTPUT5_SUBZONE             14
#define REG_OUTPUT6_SUBZONE             15
#define REG_OUTPUT7_SUBZONE             16
#define REG_OUTPUT8_SUBZONE             17
#define REG_OUTPUT_STATE	            18 //never written to, handled in readAppreg routine
#define REG_OUTPUT_DEFAULT_STATE 	    19
#define REG_INPUT_DEBOUNCE			    20 //contains the input debounce time (minimum for "short" push)
#define REG_INPUT_START					21 //and when a push is considered a "long" push
#define REG_FOLLOWZONE					22 //option follow zone/subzone
#define REG_ZONE_END					23
//#define ADRR_DM_START        	       0x70

// This marks the space for the decision matrix start. It
// is just a dummy for EEPROM storage and do not represent the
// actual register position
#define REG_DM_START                   30
#define REG_END							(REG_DM_START + (DESCION_MATRIX_ELEMENTS)*16)

// system registers
#define RegisterPagesUsed				1  //future expansion of DM would be page 2
















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
