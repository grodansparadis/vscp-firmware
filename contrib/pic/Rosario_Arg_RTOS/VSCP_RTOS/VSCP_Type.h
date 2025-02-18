/**
 * @brief           VSCP Level I/II type definition file
 * @file            vscp_2.h
 * @author          Ake Hedman, eurosource, <a href="www.vscp.org">VSCP Project</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_vscp
 *
 *
 * @section description Description
 **********************************
 * This module contains the code that implements that 
 *
 *********************************************************************/

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Copyright (C) 1995-2025 Ake Hedman, eurosource, <akhe@eurosource.se>
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

#ifndef VSCP_TYPE_H
#define VSCP_TYPE__H

#define VSCP_TYPE_UNDEFINED                               0

// VSCP Protocol Functionality
#define VSCP_TYPE_PROTOCOL_SEGCTRL_HEARTBEAT              1
#define VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE                2
#define VSCP_TYPE_PROTOCOL_PROBE_ACK                      3
#define VSCP_TYPE_PROTOCOL_SET_NICKNAME                   6
#define VSCP_TYPE_PROTOCOL_NICKNAME_ACCEPTED              7
#define VSCP_TYPE_PROTOCOL_DROP_NICKNAME                  8
#define VSCP_TYPE_PROTOCOL_READ_REGISTER                  9
#define VSCP_TYPE_PROTOCOL_RW_RESPONSE                    10
#define VSCP_TYPE_PROTOCOL_WRITE_REGISTER                 11
#define VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER              12
#define VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER                13
#define VSCP_TYPE_PROTOCOL_NACK_BOOT_LOADER               14
#define VSCP_TYPE_PROTOCOL_START_BLOCK                    15
#define VSCP_TYPE_PROTOCOL_BLOCK_DATA                     16
#define VSCP_TYPE_PROTOCOL_BLOCK_DATA_ACK                 17
#define VSCP_TYPE_PROTOCOL_BLOCK_DATA_NACK                18
#define VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA             19
#define VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_ACK         20
#define VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_NACK        21
#define VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE             22
#define VSCP_TYPE_PROTOCOL_RESET_DEVICE                   23
#define VSCP_TYPE_PROTOCOL_PAGE_READ                      24
#define VSCP_TYPE_PROTOCOL_PAGE_WRITE                     25
#define VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE               26
#define VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_PROBE          27
#define VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_RESPONSE       28
#define VSCP_TYPE_PROTOCOL_INCREMENT_REGISTER             29
#define VSCP_TYPE_PROTOCOL_DECREMENT_REGISTER             30
#define VSCP_TYPE_PROTOCOL_WHO_IS_THERE                   31
#define VSCP_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE          32
#define VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO                33
#define VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE       34
#define VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF               35
#define VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF_RESPONSE      36
#define VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_READ             37
#define VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE            38
#define VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE         39 

// Class 1 (0x01) -- ALARM
#define VSCP_TYPE_ALARM_GENERAL					0x00
#define VSCP_TYPE_ALARM_WARNING					0x01
#define VSCP_TYPE_ALARM_ALARM					0x02


// Class 10 (0x0a) -- MEASUREMENT

#define VSCP_TYPE_MEASUREMENT_TEMPERATURE		0x06
#define VSCP_TYPE_MEASUREMENT_HUMIDITY			0x23
#define VSCP_TYPE_MEASUREMENT_DEWPOINT			0x31

// class 20 (0x14) -- INFORMATION
#define VSCP_TYPE_INFORMATION_BUTTON			0x01
#define VSCP_TYPE_INFORMATION_MOUSE				0x02
#define VSCP_TYPE_INFORMATION_ON				0x03
#define VSCP_TYPE_INFORMATION_OFF				0x04
#define VSCP_TYPE_INFORMATION_ALIVE				0x05
#define VSCP_TYPE_INFORMATION_TERMINATING		0x06
#define VSCP_TYPE_INFORMATION_OPENED			0x07
#define VSCP_TYPE_INFORMATION_CLOSED			0x08
#define VSCP_TYPE_INFORMATION_NODE_HEARTBEAT	0x09
#define VSCP_TYPE_INFORMATION_BELOW_LIMIT		0x0A
#define VSCP_TYPE_INFORMATION_ABOVE_LIMIT		0x0B
#define VSCP_TYPE_INFORMATION_PULSE				0x0C
#define VSCP_TYPE_INFORMATION_ERROR				0x0d
#define VSCP_TYPE_INFORMATION_RESUMED			0x0E
#define VSCP_TYPE_INFORMATION_PAUSED			0x0F
#define VSCP_TYPE_INFORMATION_GOOD_MORNING		0x11
#define VSCP_TYPE_INFORMATION_GOOD_DAY			0x12
#define VSCP_TYPE_INFORMATION_GOOD_AFTERNOON	0x13
#define VSCP_TYPE_INFORMATION_GOOD_EVENING		0x14
#define VSCP_TYPE_INFORMATION_GOOD_NIGHT		0x15
#define VSCP_TYPE_INFORMATION_SEE_YOU_SOON		0x16
#define VSCP_TYPE_INFORMATION_GOODBYE			0x17
#define VSCP_TYPE_INFORMATION_STOP				0x18
#define VSCP_TYPE_INFORMATION_START				0x19
#define VSCP_TYPE_INFORMATION_RESET_COMPLETED	0x1A
#define VSCP_TYPE_INFORMATION_INTERRUPTED		0x1B
#define VSCP_TYPE_INFORMATION_PREPARING_TO_SLEEP 0x1C
#define VSCP_TYPE_INFORMATION_WOKEN_UP			0x1D
#define VSCP_TYPE_INFORMATION_DUSK				0x1E
#define VSCP_TYPE_INFORMATION_DAWN				0x1F
#define VSCP_TYPE_INFORMATION_ACTIVE			0x20
#define VSCP_TYPE_INFORMATION_INACTIVE			0x21
#define VSCP_TYPE_INFORMATION_BUSY				0x22
#define VSCP_TYPE_INFORMATION_IDLE				0x23
#define VSCP_TYPE_INFORMATION_STREAM_DATA		0x24
#define VSCP_TYPE_INFORMATION_TOKEN_ACTIVITY	0x25
#define VSCP_TYPE_INFORMATION_LEVEL_CHANGED		0x28
#define VSCP_TYPE_INFORMATION_WARNING			0x29
#define VSCP_TYPE_INFORMATION_STATE				0x2A

// class 102 (0x66) -- DISPLAY
#define VSCP_TYPE_DIPLAY_CLEAR_DISPLAY						0x01
#define VSCP_TYPE_DIPLAY_POSITION_CURSOR					0x02
#define VSCP_TYPE_DIPLAY_WRITE_DISPLAY						0x03
#define VSCP_TYPE_DIPLAY_WRITE_DISPLAY_BUFFER			0x04
#define VSCP_TYPE_DIPLAY_SHOW_DISPLAY_BUFFER			0x05
#define VSCP_TYPE_DIPLAY_SET_DISPLAY_BUFFER_PARAM	0x06
#define VSCP_TYPE_DIPLAY_SHOW_TEXT								0x20
#define VSCP_TYPE_DIPLAY_SHOW_LED									0x30
#define VSCP_TYPE_DIPLAY_SHOW_LED_COLOR						0x31



// class 110 (0x6E) -- IR Remote
#define VSCP_TYPE_REMOTE_RC5								0x01
#define VSCP_TYPE_REMOTE_SONY								0x02
#define VSCP_TYPE_REMOTE_LIRC								0x20
#define VSCP_TYPE_REMOTE_VSCP								0x30
#define VSCP_TYPE_REMOTE_MAPITO							0x40

///////////////////////////////////////////////////////////////////////////////
//						        Level II
///////////////////////////////////////////////////////////////////////////////

// Level II Protocol functionality
#define VSCP2_TYPE_SEGCTRL_READREGISTERII		0x01
#define VSCP2_TYPE_SEGCTRL_WRITEREGISTERII		0x02
#define VSCP2_TYPE_SEGCTRL_READWRITERESPONSEII	0x03
#define VSCP2_TYPE_SEGCTRL_CONFIGREADREQUEST	0x04
#define VSCP2_TYPE_SEGCTRL_CONFIGRADRESPONSE	0x05
#define VSCP2_TYPE_SEGCTRL_CONFIGCHANGED		0x06
#define VSCP2_TYPE_SEGCTRL_CONFIGUPDATEREQUEST	0x07
#define VSCP2_TYPE_SEGCTRL_CONFIGUPDATERESPONSE	0x08
#define VSCP2_TYPE_SEGCTRL_CONFIGPARAMREQUEST	0x09
#define VSCP2_TYPE_SEGCTRL_CONFIGPARAMRESPONSE	0x0A

#endif
