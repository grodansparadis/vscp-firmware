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
#include "vscptest.h"

///////////////////////////////////////////////////////////////////////////////
// doActionCtrlLed
// 

void doActionCtrlLed( unsigned char dmflags, unsigned char arg )
{
	unsigned char i;
	unsigned char val;
uart_puts("action\n");	
	for ( i=0; i<8; i++ ) {
		
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
															REG_SWITCH0_SUBZONE + 
															i ) ) {
				continue;
			}
		}
			
		val = readEEPROM( VSCP_EEPROM_END + REG_SWITCH0_SUBZONE + i );
		
	
		PORTB ^= _BV(i);
									
//		// Should off event be sent?
//		if( val & RELAY_CONTROLBIT_ONEVENT ) {
//			SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );			
//		}
		
		// Should stop event be sent?
//		if( val & RELAY_CONTROLBIT_STARTEVENT ) {
//			SendInformationEvent( i, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_START );
//		}
	}	
}




///////////////////////////////////////////////////////////////////////////////
// doActionHelloWorld
//

void doActionHelloWorld( unsigned char dmflags, unsigned char arg )
{
uart_puts("Hello World!\n");
}

