/* This is an modification of the demo_vscp_node_can128, credits to Akhe
 * goal is to implement a small module for home automation including:
 * - 8 input (on/off)		done
 * - 8 output (on/off)		done
 * - IR capable receiver
 * - piezzo buzzer
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


#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_actions.h"
#include "vscp_registers.h"

///////////////////////////////////////////////////////////////////////////////
void doActionToggleOut( unsigned char dmflags, unsigned char arg )
{
#ifdef PRINT_DM_EVENTS
uart_puts( "debug doActionToggleOut\n" );
#endif

	unsigned char i;
	//unsigned char val;
	
	for ( i=0; i<8; i++ ) 
	{
		// If the relay should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) 
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
											REG_OUTPUT1_SUBZONE + i ) ) 
				{
				continue;
				}
		}
			
		//val = readEEPROM( VSCP_EEPROM_END + REG_OUTPUT1_SUBZONE + i );
		
		// Do nothing if disabled
		//if ( !( val & RELAY_CONTROLBIT_ENABLED ) ) continue;
			outputport ^= _BV(i);


	}								
}

///////////////////////////////////////////////////////////////////////////////
// doActionON
void doActionOnOut( unsigned char dmflags, unsigned char arg )
{
uart_puts( "debug doActionOnOut\n" );
	unsigned char i;
	//unsigned char val;
	
	for ( i=0; i<8; i++ ) 
	{
		uart_puts( "debug doActionOnOut i" );
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) 
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
											REG_OUTPUT1_SUBZONE + i ) ) 
			{
			continue;
			}
		}
			
		//val = readEEPROM( VSCP_EEPROM_END + REG_OUTPUT1_SUBZONE + i );
		
		outputport &= ~ _BV(i);


	}	
}



///////////////////////////////////////////////////////////////////////////////
// doActionOFF
void doActionOffOut( unsigned char dmflags, unsigned char arg )
{
uart_puts( "debug doActionOffOut\n" );
	unsigned char i;
	//unsigned char val;
	
	for ( i=0; i<8; i++ ) 
	{
		uart_puts( "debug doActionOffOut i" );
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) 
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_END + 
												REG_OUTPUT1_SUBZONE + i ) ) 
				{
				continue;
				}
		}
			
		//val = readEEPROM( VSCP_EEPROM_END + REG_OUTPUT1_SUBZONE + i );
		
		outputport |= _BV(i);


	}						
}




#ifdef PRINT_DM_EVENTS
///////////////////////////////////////////////////////////////////////////////
// doActionHelloWorld
//
void doActionHelloWorld( unsigned char dmflags, unsigned char arg )
{
uart_puts( "HelloWorld!\n" );
}
#endif



void vscp_outputevent(unsigned char current,unsigned char previous)
{
unsigned char change=0,i=0,j=1;

change = current^previous; //only changed bits are left!
#ifdef PRINT_IO_EVENTS
uart_puts( "OUTPUT change detected!\n" );
#endif

for (i=0; i<8; i++)
	{
		if ((change>>i)&0x01)
		{
			if ((j & current) == j)
				{
				SendInformationEventExtended(VSCP_PRIORITY_MEDIUM, 
							readEEPROM( VSCP_EEPROM_REGISTER + REG_ZONE ), 
							readEEPROM( VSCP_EEPROM_REGISTER + + REG_OUTPUT1_SUBZONE + i),
							0x00, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF);
				}
			if (!(j & current)) 
				{
				SendInformationEventExtended(VSCP_PRIORITY_MEDIUM, 
							readEEPROM( VSCP_EEPROM_REGISTER + REG_ZONE ), 
							readEEPROM( VSCP_EEPROM_REGISTER + + REG_OUTPUT1_SUBZONE + i),
							0x00, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON);
				
				}
		}
	j = j*2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// doFollow
//
void doFollow()
//
// The routine expects vscp_imsg to contain a valid incoming event
//
{
	#ifdef PRINT_DM_EVENTS
	uart_puts( "debug  doFollow\n" );
    #endif
	
	// Don't deal with the control functionality
	//if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class ) return;

	uint8_t i=0;

	if ( VSCP_CLASS1_INFORMATION != vscp_imsg.vscp_class ) return; //only information events are processed
	
	if ( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_END + REG_ZONE  ) ) 
			{
				#ifdef PRINT_DM_EVENTS
				uart_puts( "zone mismatch\n" );
                #endif
            	return;
			}
    
	if ( VSCP_TYPE_INFORMATION_ON == vscp_imsg.vscp_type) 
	{
		#ifdef PRINT_DM_EVENTS
		uart_puts( "doFollow informationON\n" );
    	#endif	

		//read Follow eeprom register into ram
		unsigned char followflag = readEEPROM( VSCP_EEPROM_END + REG_FOLLOWZONE ) ;

		for (i=0 ; i<8 ; i++)
		{
			if (!(followflag & (1 << i))) return; //only check when follow bit is set
			
            // Check if subzone match or is empty
            if (readEEPROM( VSCP_EEPROM_REGISTER + + REG_OUTPUT1_SUBZONE + i  ) == 0x00) 
				{
				uart_puts( "subzone 0x00\n" );
				}
			else if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + + REG_OUTPUT1_SUBZONE + i  ) ) 
				{
				#ifdef PRINT_DM_EVENTS
				uart_puts( "subzone mismatch\n" );
				#endif
                continue;
                }	
            #ifdef PRINT_DM_EVENTS
			uart_puts( "subzone match\n" );
			#endif

			outputport &= ~ _BV(i);
			
		}
	}

	if ( VSCP_TYPE_INFORMATION_OFF == vscp_imsg.vscp_type ) 
	{
		#ifdef PRINT_DM_EVENTS
		uart_puts( "doFollow informationOFF\n" );
    	#endif	
	
		//read Follow eeprom register into ram
		unsigned char followflag = readEEPROM( VSCP_EEPROM_END + REG_FOLLOWZONE ) ;
	
		for (i=0 ; i<8 ; i++)
		{
			if (!(followflag & (1 << i))) return; //only check when follow bit is set
			
            // Check if subzone match or is empty
            if (readEEPROM( VSCP_EEPROM_REGISTER + + REG_OUTPUT1_SUBZONE + i  ) == 0x00) 
				{
				uart_puts( "subzone 0x00\n" );
				}
			else if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + + REG_OUTPUT1_SUBZONE + i  ) ) 
				{
				#ifdef PRINT_DM_EVENTS
				uart_puts( "subzone mismatch\n" );
				#endif
                continue;
                }	
            #ifdef PRINT_DM_EVENTS
			uart_puts( "subzone match\n" );
			#endif

			outputport |= _BV(i);
		
		}
	}
}

