/* This is an modification of the demo_vscp_node_can128, credits to Akhe
 * see original header below this file
 * goal is to implement a central module for home automation including:
 * - 16 output (on/off)	with own zone/subzone
 * - a number of analog output	
 * - bootloader support
 *
 * hardware supported:
 * olimex AVR-CAN board
 * custom board, AAmega
 *
 * version 0.0.1
 * 29/06/2011
 *
 * Sven Zwiers
 *
 * more information about VSCP: http://www.vscp.org
 *---------------------------------------------------------------------------
*/


#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_actions.h"
#include "vscp_registers.h"


///////////////////////////////////////////////////////////////////////////////
void doActionToggleOut( unsigned char port, unsigned char dmflags, unsigned char arg )
{
#ifdef PRINT_CAN_EVENTS
uart_puts( "debug doActionToggleOut\n" );
#endif

	unsigned char i;
	
	for ( i=0; i<8; i++ ) 
	{
		// If the relay should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE ) 
		{
			if ( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + i + ((port-1) * 8)) ) 
				{
				continue;
				}
		}

		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) 
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + i + ((port-1) * 8)) ) 
				{
				continue;
				}
		}
			
		if (port == 1) outputport1 ^= _BV(i);
		if (port == 2) outputport2 ^= _BV(portflip(i));
		
	}								
}

///////////////////////////////////////////////////////////////////////////////
// doActionON
void doActionOnOut( unsigned char port, unsigned char dmflags, unsigned char arg )
{
uart_puts( "debug doActionOnOut\n" );
	unsigned char i;
	
	for ( i=0; i<8; i++ ) 
	{
		uart_puts( "debug doActionOnOut i" );
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE ) 
		{
			if ( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + i + ((port-1) * 8) ) ) 
				{
				continue;
				}
		}

		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) 
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + i  + ((port-1) * 8)) ) 
				{
				continue;
				}
		}			
		if (port == 1) outputport1 &= ~ _BV(i);
		if (port == 2) outputport2 &= ~ _BV(portflip(i));

	}	
}



///////////////////////////////////////////////////////////////////////////////
// doActionOFF
void doActionOffOut( unsigned char port, unsigned char dmflags, unsigned char arg )
{
uart_puts( "debug doActionOffOut\n" );
	unsigned char i;
	
	for ( i=0; i<8; i++ ) 
	{
		uart_puts( "debug doActionOffOut i" );
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE ) 
		{
			if ( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + i  + ((port-1) * 8)) ) 
				{
				continue;
				}
		}

		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) 
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + i  + ((port-1) * 8)) ) 
				{
				continue;
				}
		}		
	
		outputport1 |= _BV(i);
		if (port == 1) outputport1 |= _BV(i);
		if (port == 2) outputport2 |= _BV(portflip(i));



	}						
}

///////////////////////////////////////////////////////////////////////////////
void doActionToggleDM( unsigned char dmflags, unsigned char arg )
{
	#ifdef PRINT_DM_EVENTS
	uart_puts( "debug doActionToggleDM\n" );
	#endif

	unsigned char i;
	uint8_t dmToggleflags;
	//unsigned char val;
	
	for ( i=0; i<8; i++ )
	{
		// If the line should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE )
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_SUBZONE  ) )
			{
				continue;
			}
			#ifdef PRINT_DM_EVENTS
			else uart_puts( "ToggleDM subzone match\n" );
			#endif
		}
		
		dmToggleflags = readEEPROM( VSCP_EEPROM_END + REG_DM_START + 1 + ( VSCP_SIZE_STD_DM_ROW * i ) );
		writeEEPROM(( VSCP_EEPROM_END + REG_DM_START + 1 + ( VSCP_SIZE_STD_DM_ROW * i )),dmToggleflags^VSCP_DM_FLAG_ENABLED );		


	}
}

///////////////////////////////////////////////////////////////////////////////
// doActionON DM
void doActionOnDM( unsigned char dmflags, unsigned char arg )
{
	uart_puts( "debug doActionOnDM\n" );
	unsigned char i;
	uint8_t dmToggleflags;
	
	for ( i=0; i<8; i++ )
	{
		uart_puts( "debug doActionOnDM i" );
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE )
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END +
			REG_SUBZONE ) )
			{
				continue;
			}
		}
		
		dmToggleflags = readEEPROM( VSCP_EEPROM_END + REG_DM_START + 1 + ( VSCP_SIZE_STD_DM_ROW * i ) );
		writeEEPROM(( VSCP_EEPROM_END + REG_DM_START + 1 + ( VSCP_SIZE_STD_DM_ROW * i )),dmToggleflags |VSCP_DM_FLAG_ENABLED );
		
		//outputport &= ~ _BV(i);


	}
}



///////////////////////////////////////////////////////////////////////////////
// doActionOFF DM
void doActionOffDM( unsigned char dmflags, unsigned char arg )
{
	uart_puts( "debug doActionOffDM\n" );
	unsigned char i;
	uint8_t dmToggleflags;
	
	for ( i=0; i<8; i++ )
	{
		uart_puts( "debug doActionOffDM i" );
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE )
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + REG_SUBZONE) )
			{
				continue;
			}
		}
		dmToggleflags = readEEPROM( VSCP_EEPROM_END + REG_DM_START + 1 + ( VSCP_SIZE_STD_DM_ROW * i ) );
		writeEEPROM(( VSCP_EEPROM_END + REG_DM_START + 1 + ( VSCP_SIZE_STD_DM_ROW * i )),dmToggleflags & ~(VSCP_DM_FLAG_ENABLED) );
				
		//outputport |= _BV(i);


	}
}




#ifdef PRINT_CAN_EVENTS
///////////////////////////////////////////////////////////////////////////////
// doActionHelloWorld
//
void doActionHelloWorld( unsigned char dmflags, unsigned char arg )
{
uart_puts( "HelloWorld!\n" );
}
#endif



void vscp_outputevent(unsigned int current,unsigned int previous)
{
	unsigned int change=0,i=0,j=1;
	change = current^previous; //only changed bits are left
	#ifdef PRINT_CAN_EVENTS
		uart_puts( "OUTPUT change detected!\n" );
   		char buf[30];
		sprintf(buf, "current/previous: %04x/%04x/", current, previous);
		uart_puts(buf);
	#endif
	for (i=0; i<16; i++)
	{
		if ((change>>i)&0x01)
		{
			if ((j & current) == j)
				SendInformationEventExtended
					(7,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + i ))
					,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + i ))
					,0 , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );
			if (!(j & current)) 
				SendInformationEventExtended
					(7,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + i ))
					,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + i ))
					,0 , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );
		}
		j = j*2;
	}
}

unsigned char portflip(unsigned char old_val)
{
	unsigned char i,reverse =7;

	for (i = 0; i!= old_val; i++)
	{
		reverse--;
	}
    return(reverse); 
}

unsigned char bitflip(unsigned char old_val) 
{ 
   unsigned char new_val = 0; 
   if (old_val & 0x01) new_val |= 0x80; 
   if (old_val & 0x02) new_val |= 0x40; 
   if (old_val & 0x04) new_val |= 0x20; 
   if (old_val & 0x08) new_val |= 0x10; 
   if (old_val & 0x10) new_val |= 0x08; 
   if (old_val & 0x20) new_val |= 0x04; 
   if (old_val & 0x40) new_val |= 0x02; 
   if (old_val & 0x80) new_val |= 0x01; 
   return(new_val); 
} 






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
