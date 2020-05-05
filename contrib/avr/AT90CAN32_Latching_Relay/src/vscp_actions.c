/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2006-04-21
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 2006-2011 Ake Hedman, eurosource
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

#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include "vscp_firmware.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_actions.h"
#include "vscp_registers.h"
#include "latchingrelay.h"

volatile uint8_t relay_pulse_width;  // pull-up timer to change relay state
volatile uint8_t relay_timer_enabled;  // time before relay switch back to the off position
								// if set to 0 then no timer

///////////////////////////////////////////////////////////////////////////////
// doActionAction1
// 


void doActionAction1()
{

RELAY_ON_ON;

relay_pulse_width = 0;

relay_timer_enabled = 1;

vscp_omsg.priority = 0x00;
vscp_omsg.flags = VSCP_VALID_MSG + 4;
vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_ON;

vscp_omsg.data[ 0 ] = 0x00;
vscp_omsg.data[ 1 ] = readEEPROM( REG_ZONE + VSCP_EEPROM_END ); // byte 1 for zone
vscp_omsg.data[ 2 ] = readEEPROM( REG_SUBZONE + VSCP_EEPROM_END ); // byte 2 for subzone

vscp_sendEvent(); // Send data
}




///////////////////////////////////////////////////////////////////////////////
// doActionAction2
//

void doActionAction2()
{

RELAY_OFF_ON;

relay_pulse_width = 0;

vscp_omsg.priority = 0x00;
vscp_omsg.flags = VSCP_VALID_MSG + 4;
vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_OFF;

vscp_omsg.data[ 0 ] = 0x00;
vscp_omsg.data[ 1 ] = readEEPROM( REG_ZONE + VSCP_EEPROM_END ); // byte 1 for zone
vscp_omsg.data[ 2 ] = readEEPROM( REG_SUBZONE + VSCP_EEPROM_END ); // byte 2 for subzone

vscp_sendEvent(); // Send data

}

