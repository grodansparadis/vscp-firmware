/* This is an modification of the demo_vscp_node_can128, credits to Akhe
 * see original header below this file
 * goal is to implement a small module for home automation including:
 * - 8 input (on/off)
 * - 8 output (on/off)
 * - IR capable receiver
 * - piezzo buzzer
 * - a number of analog input (temperature / light / ...)
 * - bootloader support
 *
 * none of these goals are implemented yet!
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



// * * * Actions * * *
#define ACTION_NOOP							0
#define ACTION_OUTP_TOGGLE1					1
#define ACTION_OUTP_TOGGLE2					2
#define ACTION_OUTP_ON1						3
#define ACTION_OUTP_ON2						4
#define ACTION_OUTP_OFF1					5
#define ACTION_OUTP_OFF2					6
#define ACTION_DM_TOGGLE					7
#define ACTION_DM_ON						8
#define ACTION_DM_OFF						9
//0x0A .. 0x0F spare
#define ACTION_SET_TIMER					0x10
#define ACTION_SET_TIMER2					0x11
#define ACTION_SET_TIMER3					0x12
#define ACTION_SET_TIMER4					0x13
#define ACTION_SET_TIMER5					0x14
#define ACTION_SET_TIMER6					0x15
#define ACTION_SET_TIMER7					0x16
#define ACTION_SET_TIMER8					0x17




//#define ACTION_HELLO_WORLD					2


// Prototypes
void doActionToggleOut(unsigned char port, unsigned char dmflags, unsigned char arg );
void doActionOnOut(unsigned char port, unsigned char dmflags, unsigned char arg );
void doActionOffOut(unsigned char port, unsigned char dmflags, unsigned char arg );
void doActionToggleDM( unsigned char dmflags, unsigned char arg );
void doActionOnDM( unsigned char dmflags, unsigned char arg );
void doActionOffDM( unsigned char dmflags, unsigned char arg );
void doActionSetTimer(unsigned char dmflags, unsigned char arg);
void doActionHelloWorld( unsigned char dmflags, unsigned char arg );
void vscp_outputevent(unsigned int current,unsigned int previous);
unsigned char portflip(unsigned char old_val); //flip over portpin value --> 8 becomes 1, 7 becomes 2 ,...
unsigned char bitflip(unsigned char old_val); // flip over complete byte (lsb becomes msb)






/* original header******************************************************************
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
