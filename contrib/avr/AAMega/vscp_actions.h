/* This is an modification of the demo_vscp_node_can128, credits to Akhe
 * see original header below this file
 * more information about VSCP: http://www.vscp.org
 *---------------------------------------------------------------------------
*/



// * * * Actions * * *
#define ACTION_NOOP							0
#define ACTION_OUTP_TOGGLE1					1
#define ACTION_OUTP_TOGGLE2					2
#define ACTION_OUTP_TOGGLE3					3
#define ACTION_OUTP_TOGGLE4					4
#define ACTION_OUTP_ON1						5
#define ACTION_OUTP_ON2						6
#define ACTION_OUTP_ON3						7
#define ACTION_OUTP_ON4						8
#define ACTION_OUTP_OFF1					9
#define ACTION_OUTP_OFF2					10
#define ACTION_OUTP_OFF3					11
#define ACTION_OUTP_OFF4					12
#define ACTION_OUTP_TOGGLEALL				13
#define ACTION_OUTP_ONALL					14
#define ACTION_OUTP_OFFALL					15

#define ACTION_DM_TOGGLE					0x10
#define ACTION_DM_ON						0x11
#define ACTION_DM_OFF						0x12
#define ACTION_SET_TIMER1					0x20
#define ACTION_SET_TIMER2					0x21
#define ACTION_SET_TIMER3					0x22
#define ACTION_SET_TIMER4					0x23
#define ACTION_SET_TIMER5					0x24
#define ACTION_SET_TIMER6					0x25
#define ACTION_SET_TIMER7					0x26
#define ACTION_SET_TIMER8					0x27

#define ACTION_SHUTTER_MOVE					0x30
#define ACTION_SHUTTER_MOVE1				0x31
#define ACTION_SHUTTER_MOVE2				0x32
#define ACTION_SHUTTER_MOVE3				0x33
#define ACTION_SHUTTER_MOVE4				0x34
#define ACTION_SHUTTER_PRESET				0x35
#define ACTION_SHUTTER_1BUTTON				0x36
#define ACTION_SHUTTER_UP					0x37
#define ACTION_SHUTTER_DOWN					0x38





//shutter 1button states
#define latest_unknown	0
#define latest_down		1
#define latest_up		2
#define latest_down_stopped	3
#define latest_up_stopped	4

//shutter moving states
#define shutter_unknown		0
#define shutter_ini			1
#define shutter_notmoving	2
#define shutter_moving_up	3
#define shutter_moving_down	4
#define shutter_forcestop	5

//#define ACTION_HELLO_WORLD					2


// Prototypes
void doActionToggleOut(unsigned char port, unsigned char dmflags, unsigned char arg );
void doActionOnOut(unsigned char port, unsigned char dmflags, unsigned char arg );
void doActionOffOut(unsigned char port, unsigned char dmflags, unsigned char arg );
void doActionToggleDM( unsigned char dmflags, unsigned char arg );
void doActionOnDM( unsigned char dmflags, unsigned char arg );
void doActionOffDM( unsigned char dmflags, unsigned char arg );
void doActionSetTimer(unsigned char select_timer, unsigned char dmflags, unsigned char arg);
void doActionShutterMove( unsigned char dmflags, unsigned char arg );
void doActionShutterMoveUp( unsigned char dmflags);
void doActionShutterMoveDown( unsigned char dmflags);
void doActionShutterPreset( unsigned char dmflags, unsigned char arg );
void doActionFixedShutterMove( unsigned char select_shutter,unsigned char dmflags, unsigned char arg );
void doActionShutter1BUTTON( unsigned char dmflags, unsigned char arg );
void doFixedActions();


void doActionHelloWorld( unsigned char dmflags, unsigned char arg );
void vscp_outputevent(unsigned char,unsigned int current,unsigned int previous);
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
