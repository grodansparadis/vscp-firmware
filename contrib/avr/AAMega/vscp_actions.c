/* This is an modification of the demo_vscp_node_can128, credits to Akhe
 * see original header below this file
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
#ifdef PRINT_DM_EVENTS
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
			if (( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + i + ((port-1) * 8)) ) & (vscp_imsg.data[ 1 ] != 255))
				{
				continue;
				}
		}

		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) 
		{
			if (( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + i + ((port-1) * 8)) ) & (vscp_imsg.data[ 2 ] != 255))
				{
				continue;
				}
		}
			
		if (port == 1) outputport1 ^= _BV(i);
		if (port == 2) outputport2 ^= _BV(portflip(i));
		if (port == 3) outputport3 ^= _BV(i);
		if (port == 4) outputport4 ^= _BV(i);

		
	}								
}

///////////////////////////////////////////////////////////////////////////////
// doActionON
void doActionOnOut( unsigned char port, unsigned char dmflags, unsigned char arg )
{
#ifdef PRINT_DM_EVENTS
uart_puts( "debug doActionOnOut\n" );
#endif
	unsigned char i;
	
	for ( i=0; i<8; i++ ) 
	{
		#ifdef PRINT_DM_EVENTS
		uart_puts( "debug doActionOnOut i" );
		#endif
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE ) 
		{
			if (( vscp_imsg.data[ 1 ] != (readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + i + ((port-1) * 8) )) ) & (vscp_imsg.data[ 1 ] != 255))
				{
				continue;
				}
		}

		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) 
		{
			if (( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + i  + ((port-1) * 8)) ) & (vscp_imsg.data[ 2 ] != 255))
				{
				continue;
				}
		}			
		if (port == 1) outputport1 &= ~ _BV(i);
		if (port == 2) outputport2 &= ~ _BV(portflip(i));
		if (port == 3) outputport3 &= ~ _BV(i);
		if (port == 4) outputport4 &= ~ _BV(i);

	}	
}



///////////////////////////////////////////////////////////////////////////////
// doActionOFF
void doActionOffOut( unsigned char port, unsigned char dmflags, unsigned char arg )
{
#ifdef PRINT_DM_EVENTS
uart_puts( "debug doActionOffOut\n" );
#endif
	unsigned char i;
	
	for ( i=0; i<8; i++ ) 
	{
		#ifdef PRINT_DM_EVENTS
		uart_puts( "debug doActionOffOut i" );
		#endif
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE ) 
		{
			if (( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + i  + ((port-1) * 8)) ) & (vscp_imsg.data[ 1 ] != 255))
				{
				continue;
				}
		}

		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) 
		{
			if (( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + i  + ((port-1) * 8)) ) & (vscp_imsg.data[ 2 ] != 255))
				{
				continue;
				}
		}		
		if (port == 1) outputport1 |= _BV(i);
		if (port == 2) outputport2 |= _BV(portflip(i));
		if (port == 3) outputport3 |= _BV(i);
		if (port == 4) outputport4 |= _BV(i);

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
		
		dmToggleflags = readEEPROM( VSCP_EEPROM_REGISTER + REG_DM_START  + VSCP_DM_POS_FLAGS + ( VSCP_SIZE_STD_DM_ROW * i ) );
		writeEEPROM(( VSCP_EEPROM_REGISTER + REG_DM_START  + VSCP_DM_POS_FLAGS + ( VSCP_SIZE_STD_DM_ROW * i )),dmToggleflags^VSCP_DM_FLAG_ENABLED );		


	}
}

///////////////////////////////////////////////////////////////////////////////
// doActionON DM
void doActionOnDM( unsigned char dmflags, unsigned char arg )
{
	#ifdef PRINT_DM_EVENTS
	uart_puts( "debug doActionOnDM\n" );
	#endif
	unsigned char i;
	uint8_t dmToggleflags;
	
	for ( i=0; i<8; i++ )
	{
		#ifdef PRINT_DM_EVENTS
		uart_puts( "debug doActionOnDM i" );
		#endif
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE )
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_SUBZONE ) )
			{
				continue;
			}
		}
		
		dmToggleflags = readEEPROM( VSCP_EEPROM_REGISTER + REG_DM_START  + VSCP_DM_POS_FLAGS + ( VSCP_SIZE_STD_DM_ROW * i ) );
		writeEEPROM(( VSCP_EEPROM_REGISTER + REG_DM_START  + VSCP_DM_POS_FLAGS + ( VSCP_SIZE_STD_DM_ROW * i )),dmToggleflags |VSCP_DM_FLAG_ENABLED );
		
		//outputport &= ~ _BV(i);


	}
}



///////////////////////////////////////////////////////////////////////////////
// doActionOFF DM
void doActionOffDM( unsigned char dmflags, unsigned char arg )
{
	#ifdef PRINT_DM_EVENTS
	uart_puts( "debug doActionOffDM\n" );
	#endif
	unsigned char i;
	uint8_t dmToggleflags;
	
	for ( i=0; i<8; i++ )
	{
		#ifdef PRINT_DM_EVENTS
		uart_puts( "debug doActionOffDM i" );
		#endif
		// If the rely should not be handled just move on
		if ( !( arg & ( 1 << i ) ) ) continue;
		
				// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE )
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_SUBZONE) )
			{
				continue;
			}
		}
		
		dmToggleflags = readEEPROM( VSCP_EEPROM_REGISTER + REG_DM_START  + VSCP_DM_POS_FLAGS + ( VSCP_SIZE_STD_DM_ROW * i ) );
		writeEEPROM(( VSCP_EEPROM_REGISTER + REG_DM_START  + VSCP_DM_POS_FLAGS + ( VSCP_SIZE_STD_DM_ROW * i )),dmToggleflags & ~(VSCP_DM_FLAG_ENABLED) );
				
		//outputport |= _BV(i);


	}
}

///////////////////////////////////////////////////////////////////////////////
// doActionSetTimer
void doActionSetTimer(unsigned char select_timer, unsigned char dmflags, unsigned char arg)
{
	#ifdef PRINT_DM_EVENTS
	uart_puts( "debug doActionSetTimer\n" );
	#endif
	unsigned char i;
	i = select_timer-1;

	#ifdef PRINT_TIMER_EVENTS
	char buf[50];
	sprintf(buf, "doActionSetTimer:%i",	select_timer);
	uart_puts(buf);
	uart_puts( "doActionSetTimer check zone" );
	//sprintf(buf, "doActionSetTimer check zone in reg: %i, %i",vscp_imsg.data[ 1 ],readEEPROM( VSCP_EEPROM_REGISTER + REG_TIMER1_ZONE ));
	//uart_puts(buf);
	#endif
			
		// Check if zone should match and if so if it match
			
		if ((!( dmflags & VSCP_DM_FLAG_CHECK_ZONE ))|(( dmflags & VSCP_DM_FLAG_CHECK_ZONE ) &&(vscp_imsg.data[ 1 ] == readEEPROM( VSCP_EEPROM_REGISTER + REG_TIMER1_ZONE +i*12))))
			{
			#ifdef PRINT_TIMER_EVENTS
			uart_puts( "doActionSetTimer check zone OK" );
			#endif
			if((!( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ))|(( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) &&(vscp_imsg.data[ 2 ] == readEEPROM( VSCP_EEPROM_REGISTER + REG_TIMER1_SUBZONE +i*12))))
				{
				#ifdef PRINT_TIMER_EVENTS
				uart_puts( "doActionSetTimer check SUBzone MATCH or not necessary)" );
				#endif
				VSCP_USER_TIMER[select_timer] = arg;

				}
				
			}
				
}

///////////////////////////////////////////////////////////////////////////////
// doActionShutterMove
// move all shutters to position arg
// use of (sub)zone optional
///////////////////////////////////////////////////////////////////////////////
void doActionShutterMove( unsigned char dmflags, unsigned char arg )
{
	#ifdef PRINT_SHUTTER_EVENTS
	uart_puts( "debug doActionshutterMove\n" );
	#endif
	unsigned char i;
	
	for ( i=1; i<=NRofShutters; i++ )
	{
		#ifdef PRINT_DM_EVENTS
		uart_puts( "debug doActionshutterMove i" );
		#endif
		
		// Check if zone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE ) 
		{
			if ( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + (i*2) -1 )) 
				{
				continue;
				}
		}

		// Check if subzone should match and if so if it match
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) 
		{
			if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + (i*2)  -1 )) 
				{
				continue;
				}
		}		
		
		if (readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_MAX+i-1) < arg){ arg = readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_MAX+i-1);} //if max value exceeds wanted, replace it
		VSCP_USER_SHUTTER_WANTED[i] = arg;
		

	}
}


///////////////////////////////////////////////////////////////////////////////
// doActionShutterMoveUp
// move all shutters to complete UP-position
// use of (sub)zone optional
///////////////////////////////////////////////////////////////////////////////
void doActionShutterMoveUp( unsigned char dmflags)
{
	#ifdef PRINT_SHUTTER_EVENTS
	uart_puts( "debug doActionshutterMoveUp\n" );
	#endif
	unsigned char i;
	
	for ( i=1; i<=NRofShutters; i++ )
	{
		#ifdef PRINT_DM_EVENTS
		uart_puts( "debug doActionshutterMoveUp i" );
		#endif
		
		// Check if zone should match and if so if it match or 255
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE )
		{
			if (( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + (i*2) -1 )) && (vscp_imsg.data[ 1 ] != 255))
			{
				continue;
			}
		}

		// Check if subzone should match and if so if it match or 255
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE )
		{
			if (( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + (i*2)  -1 )) && (vscp_imsg.data[ 2 ] != 255))
			{
				continue;
			}
		}
		// reset shutter_actual to 1 so we are certain that max is reached, even if position is changed externally, or some drift because of difference between timing UP or DOWN
		VSCP_USER_SHUTTER_ACTUAL[i] = 1;
		
		VSCP_USER_SHUTTER_WANTED[i] = readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_MAX+i-1);
		

	}
}


///////////////////////////////////////////////////////////////////////////////
// doActionShutterMoveDown
// move all shutters to complete Down-position
// use of (sub)zone optional
///////////////////////////////////////////////////////////////////////////////
void doActionShutterMoveDown( unsigned char dmflags)
{
	#ifdef PRINT_SHUTTER_EVENTS
	uart_puts( "debug doActionshutterMoveDown\n" );
	#endif
	unsigned char i;
	
	for ( i=1; i<=NRofShutters; i++ )
	{
		#ifdef PRINT_DM_EVENTS
		uart_puts( "debug doActionshutterMoveDown i" );
		#endif
		
		// Check if zone should match and if so if it match or 255
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE )
		{
			if (( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + (i*2) -1 )) && (vscp_imsg.data[ 1 ] != 255))
			{
				continue;
			}
		}

		// Check if subzone should match and if so if it match or 255
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE )
		{
			if (( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + (i*2)  -1 )) && (vscp_imsg.data[ 2 ] != 255))
			{
				continue;
			}
		}
		// reset shutter_actual to max so we are certain that min is reached, even if position is changed externally, or some drift because of difference between timing UP or DOWN
		VSCP_USER_SHUTTER_ACTUAL[i] = readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_MAX+i-1);
		
		VSCP_USER_SHUTTER_WANTED[i] = 1;
		
	}
}

///////////////////////////////////////////////////////////////////////////////
// doActionShutterPreset
// move all shutters to position set in shutterpreset n-register
// use of (sub)zone optional
// arg = 1..4
///////////////////////////////////////////////////////////////////////////////
void doActionShutterPreset( unsigned char dmflags, unsigned char arg )
{
	#ifdef PRINT_SHUTTER_EVENTS
	uart_puts( "debug doActionshutterPreset\n" );
	#endif
	unsigned char i;
	
	for ( i=1; i<=NRofShutters; i++ )
	{
		#ifdef PRINT_DM_EVENTS
		uart_puts( "debug doActionshutterPreset i" );
		#endif
		
		// Check if zone should match and if so if it match or 255
		if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE )
		{
			if (( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + (i*2) -2 )) && (vscp_imsg.data[ 1 ] != 255))
			{
				continue;
			}
		}

		// Check if subzone should match and if so if it match or 255
		if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE )
		{
			if (( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + (i*2)  -2 )) && (vscp_imsg.data[ 2 ] != 255))
			{
				continue;
			}
		}
		
		if ((arg>=1) & (arg <= 4))	VSCP_USER_SHUTTER_WANTED[i] = readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_PRESET1+(i-1)+((arg-1))*4);
		

	}
}

///////////////////////////////////////////////////////////////////////////////
// doActionFixedShutterMove
// move 1 selected shutter to position arg
// use of (sub)zone optional
///////////////////////////////////////////////////////////////////////////////
void doActionFixedShutterMove( unsigned char select_shutter,unsigned char dmflags, unsigned char arg )
{
	#ifdef PRINT_SHUTTER_EVENTS
	uart_puts( "debug doActionFixedshutterMove\n" );
	#endif
	
	// Check if zone should match and if so if it match
	if ((!(dmflags & VSCP_DM_FLAG_CHECK_ZONE))|((dmflags & VSCP_DM_FLAG_CHECK_ZONE) && (vscp_imsg.data[ 1 ] == readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + (select_shutter*2) ))))
	{
		#ifdef PRINT_SHUTTER_EVENTS
		uart_puts( "doActionfixedshutter zone OK" );
		#endif
		// Check if subzone should match and if so if it match
		if ((!( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ))|(( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) && (vscp_imsg.data[ 2 ] == readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + (select_shutter*2) ))))
		{
			#ifdef PRINT_SHUTTER_EVENTS
			uart_puts( "doActionfixedshutter subzone OK" );
			#endif
			if (readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_MAX+ select_shutter) < arg){ arg = readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_MAX+ select_shutter);} //if max value exceeds wanted, replace it
			VSCP_USER_SHUTTER_WANTED[select_shutter+1] = arg;
		}

	}
	
}


///////////////////////////////////////////////////////////////////////////////
// doActionShutter1BUTTON
// control shutter with 1 button
// if any of 4 shutter is moving : stop shutter (stop = priority = safety)
// if shutter is stopped direction is reversed with next button push
// if any of selected shutter is down, move up
// if all selected shutters are up, move down
void doActionShutter1BUTTON( unsigned char dmflags, unsigned char arg )
{
	
	unsigned char i,wanted_temp=0;

		#ifdef PRINT_SHUTTER_EVENTS
		char shbuf[30];
		#endif
		
	
	#ifdef PRINT_SHUTTER_EVENTS
	uart_puts( "debug doActionshutter1button\n" );
	#endif
		
	//check if any shutter is moving
	for(i=NRofShutters;i>=1;i--)
	{
		
		//TODO using shutter config determine if output is used by shutters, if not, continue
		if (VSCP_USER_SHUTTER_WANTED[i] != 0) //if wanted != 0 it should be moving
		{
			#ifdef PRINT_SHUTTER_EVENTS
			sprintf(shbuf, "debug 1button %i is moving\n",i);
			uart_puts(shbuf);
			#endif
			//determine direction
			//check output if moving up is active
			if (CheckBit (read_output1,(i*2)-2))
			{
				//stop
				outputport1 |= _BV((i*2)-2);		//turn off "up"
				SendInformationEventExtendedData(VSCP_PRIORITY_MEDIUM,vscp_imsg.data[ 1 ],vscp_imsg.data[ 2 ],i , VSCP_USER_SHUTTER_ACTUAL[i], VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_STOP );
				latest_direction = latest_up_stopped;
				#ifdef PRINT_SHUTTER_EVENTS
				uart_puts( "debug 1button moving up" );
				#endif
			}
			//check output if moving down is active
			else if (CheckBit (read_output1,(i*2)-1))
			{
				//stop
				outputport1 |= _BV((i*2)-1);		//turn off "down"
				SendInformationEventExtendedData(VSCP_PRIORITY_MEDIUM,vscp_imsg.data[ 1 ],vscp_imsg.data[ 2 ],i , VSCP_USER_SHUTTER_ACTUAL[i], VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_STOP );
				latest_direction = latest_down_stopped;
				#ifdef PRINT_SHUTTER_EVENTS
				uart_puts( "debug 1button moving down" );
				#endif
			}
		}
	}
	if (!((latest_direction == latest_up_stopped) | (latest_direction == latest_down_stopped))) //if shutter was moving do not start again this time
	{
		#ifdef PRINT_SHUTTER_EVENTS
		uart_puts( "debug 1button nothing stopped" );
		#endif

		for (i=1; i<=NRofShutters ;i++)
		{
			#ifdef PRINT_SHUTTER_EVENTS
			sprintf(shbuf, "debug 1button %i\n",i);
			uart_puts(shbuf);
			#endif
		
			// If the shutter should not be handled just move on
			if ( !( arg & ( 1 << (i-1) ) ) ) continue;
			#ifdef PRINT_SHUTTER_EVENTS
			sprintf(shbuf, "debug 1button %i handled\n",i);
			uart_puts(shbuf);
			#endif

			// Check if zone should match and if so if it match
			if ((!(dmflags & VSCP_DM_FLAG_CHECK_ZONE))
			|((dmflags & VSCP_DM_FLAG_CHECK_ZONE) && (vscp_imsg.data[ 1 ] == readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + (i*2) -2 )))
			|(vscp_imsg.data[1] == 255))
			{
				#ifdef PRINT_SHUTTER_EVENTS
				uart_puts( "1button zone OK" );
				#endif
				// Check if subzone should match and if so if it match
				if ((!( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ))
					|(( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) && (vscp_imsg.data[ 2 ] == readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + (i*2)-2 )))
					| (vscp_imsg.data[ 2 ] == 255))
				{
					#ifdef PRINT_SHUTTER_EVENTS
					uart_puts( "1button subzone OK" );
					#endif
					if (latest_direction == latest_down)
					{
						wanted_temp = 100; //move up
						#ifdef PRINT_SHUTTER_EVENTS
						sprintf(shbuf, "1button Latest down move up %i\n",i);
						uart_puts(shbuf);
						#endif
						latest_direction = latest_up;
						i = NRofShutters;
						// send event to sync other modules, but use subzone/zone information from incoming message
						SendInformationEventExtendedData(VSCP_PRIORITY_MEDIUM,vscp_imsg.data[ 1 ],vscp_imsg.data[ 2 ],i , VSCP_USER_SHUTTER_ACTUAL[i], VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_OPEN );
						continue;
					}
					else if (latest_direction == latest_up)
					{
				 
						wanted_temp = 1; //move down
 						#ifdef PRINT_SHUTTER_EVENTS
						sprintf(shbuf, "1button latest up move down %i\n",i);
						uart_puts(shbuf);
						#endif
						latest_direction = latest_down;
						i = NRofShutters;
						// send event to sync other modules, but use subzone/zone information from incoming message
						SendInformationEventExtendedData(VSCP_PRIORITY_MEDIUM,vscp_imsg.data[ 1 ],vscp_imsg.data[ 2 ],i , VSCP_USER_SHUTTER_ACTUAL[i], VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_CLOSE );
						continue;
						
					}
					else if (latest_direction == latest_unknown)
					{
						 wanted_temp = 100; //move up is preferred
 						#ifdef PRINT_SHUTTER_EVENTS
						sprintf(shbuf, "1button Latest unknown %i\n",i);
						uart_puts(shbuf);
						#endif
						latest_direction = latest_up;
						i = NRofShutters;
						// send event to sync other modules
						SendInformationEventExtendedData(VSCP_PRIORITY_MEDIUM,vscp_imsg.data[ 1 ],vscp_imsg.data[ 2 ],i , VSCP_USER_SHUTTER_ACTUAL[i], VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_OPEN );
						continue;
					}
				}

			}

		}
		#ifdef PRINT_SHUTTER_EVENTS
			sprintf(shbuf, "write temp arg: %i",arg);
			uart_puts(shbuf);
			#endif
		
		//write temp to wanted registers
		for(i=NRofShutters;i>=1;i--)
		{	
			// If the shutter should not be handled just move on
			if ( !( arg & ( 1 << (i-1) ) ) ) continue;

			// Check if zone should match and if so if it match
			if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE )
			{
				if (( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + (i*2)  -2 )) && (vscp_imsg.data[ 1 ] != 255))
				{
					#ifdef PRINT_SHUTTER_EVENTS
				sprintf(shbuf, "check zone fail");
				uart_puts(shbuf);
				#endif
					continue;
				}
			}
			
			// Check if subzone should match and if so if it match or 255
			if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE )
			{
				if (( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + (i*2)  -2 )) && (vscp_imsg.data[ 2 ] != 255))
				{
					#ifdef PRINT_SHUTTER_EVENTS
				sprintf(shbuf, "check subzone fail");
				uart_puts(shbuf);
				#endif
					continue;
				}
			}

			if (readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_MAX+i-1) < wanted_temp){ wanted_temp = readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_MAX+i-1);} //if max value exceeds wanted, replace it
			#ifdef PRINT_SHUTTER_EVENTS
			sprintf(shbuf, "%i wanted %i corrected\n",i,wanted_temp);
			uart_puts(shbuf);
			#endif
			
			VSCP_USER_SHUTTER_WANTED[i] = wanted_temp;
			
			// to make sure that top will be reached, reduce the actual value to 2
			if (wanted_temp == readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_MAX+i-1)) {VSCP_USER_SHUTTER_ACTUAL[i-1] = 2;}
			
			// to make sure that bottom will be reached, set actual value to max
			if (wanted_temp == 1) {VSCP_USER_SHUTTER_ACTUAL[i-1] = readEEPROM(VSCP_EEPROM_REGISTER+REG_SHUTTER1_MAX+i-1);}
		}

		
	}
	//next time change direction
	if (latest_direction == latest_down_stopped) 
	{
		latest_direction = latest_down;
		#ifdef PRINT_SHUTTER_EVENTS
		uart_puts( "debug 1button Latest down stopped" );
		#endif
	}
		
	if (latest_direction == latest_up_stopped) 
	{
		latest_direction = latest_up;
		#ifdef PRINT_SHUTTER_EVENTS
		uart_puts( "debug 1button Latest up stopped" );
		#endif
	}
	
}



void doFixedActions()
{
	// process some pre-defined "FIXED" events:
	#ifdef PRINT_FIX_EVENTS
	uart_puts( "debug  doFixedActions\n" );
	#endif
	
	//set DMFLAGS check subzone & zone
	//CASE statement gebruiken?
	// control - turn on
	if ( vscp_imsg.vscp_class == VSCP_CLASS1_CONTROL )
	{
		uint8_t i;
		switch (vscp_imsg.vscp_type)
		{
		case VSCP_TYPE_CONTROL_TURNON:	
			#ifdef PRINT_FIX_EVENTS
			uart_puts( "turn on\n" );
			#endif
			doActionOnOut( 1, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TURNON1) );
			doActionOnOut( 2, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TURNON2) );
			doActionOnOut( 3, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TURNON3) );
			doActionOnOut( 4, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TURNON4) );
			break;
		
		case VSCP_TYPE_CONTROL_TURNOFF:
			#ifdef PRINT_FIX_EVENTS
			uart_puts( "turn off\n" );
			#endif
			doActionOffOut( 1, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TURNOFF1) );
			doActionOffOut( 2, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TURNOFF2) );
			doActionOffOut( 3, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TURNOFF3) );
			doActionOffOut( 4, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TURNOFF4) );
			break;
			
		case VSCP_TYPE_CONTROL_TOGGLE_STATE:
			#ifdef PRINT_FIX_EVENTS
			uart_puts( "toggle\n" );
			#endif
			doActionToggleOut( 1, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TOGGLE1) );
			doActionToggleOut( 2, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TOGGLE2) );
			doActionToggleOut( 3, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TOGGLE3) );
			doActionToggleOut( 4, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_TOGGLE4) );
			break;
		
		case VSCP_TYPE_CONTROL_ALL_LAMPS_ON:	
			#ifdef PRINT_FIX_EVENTS
			uart_puts( "lamp on\n" );
			#endif
			doActionOnOut( 1, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_LAMP1) );
			doActionOnOut( 2, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_LAMP2) );
			doActionOnOut( 3, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_LAMP3) );
			doActionOnOut( 4, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_LAMP4) );
			break;
		
		case VSCP_TYPE_CONTROL_ALL_LAMPS_OFF:
			#ifdef PRINT_FIX_EVENTS
			uart_puts( "lamp off\n" );
			#endif
			doActionOffOut( 1, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_LAMP1) );
			doActionOffOut( 2, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_LAMP2) );
			doActionOffOut( 3, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_LAMP3) );
			doActionOffOut( 4, 255, readEEPROM( VSCP_EEPROM_REGISTER + REG_FIXED_LAMP4) );
			break;
		
		case VSCP_TYPE_CONTROL_CLOSE:
			#ifdef PRINT_FIX_EVENTS
			uart_puts( "shutter 1down\n" );
			#endif
			doActionShutterMoveDown( 255 );
			break;
		
		case VSCP_TYPE_CONTROL_OPEN:
			#ifdef PRINT_FIX_EVENTS
			uart_puts( "shutter 1up\n" );
			#endif
			doActionShutterMoveUp( 255 );
			break;
			
		case VSCP_TYPE_CONTROL_STOP:
			#ifdef PRINT_FIX_EVENTS
			uart_puts( "shutter stop\n" );
			#endif
			
			for(i=1;i<=NRofShutters;i++)
			{
				VSCP_USER_SHUTTER_WANTED[i] = 0;
			}
			break;
		
		}
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




void vscp_outputevent(unsigned char board,unsigned int current,unsigned int previous)
{
	unsigned int change=0,i=0,j=1;
	uint16_t vscp_sendtimer=vscp_timer; 
	
	change = current^previous; //only changed bits are left
	#ifdef PRINT_IO_EVENTS
		uart_puts( "OUTPUT change detected!\n" );
   		char buf[40];
		sprintf(buf, "board/current/previous: %04x/%04x/%04x/",board, current, previous);
		uart_puts(buf);
	#endif
	if (board==0)
	{
		for (i=0; i<16; i++)
		{
			if ((change>>i)&0x01)
			{
				//wait at least 1 msec for next message to be sent
				//prevent overflow of bus or receiver
				//100�sec should be sufficient, but requires separate timer
				while (vscp_timer < vscp_sendtimer+2) ;
				if ((j & current) == j)
					{
						SendInformationEventExtended
							(7,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + i ))
							,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + i ))
							,0 , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );
						// save when last event is sent out
						vscp_sendtimer = vscp_timer;
					}

				if (!(j & current)) 
					{
						SendInformationEventExtended
							(7,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_ZONE + i ))
							,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT1_SUBZONE + i ))
							,0 , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );
						// save when last event is sent out
						vscp_sendtimer = vscp_timer;
					}
			}
			j = j*2;
		}
	}
	else if (board==1)
	{
		for (i=0; i<16; i++)
		{
			//wait at least 1 msec for next message to be sent
			//prevent overflow of bus or receiver
			//100�sec should be sufficient, but requires separate timer
			while (vscp_timer < vscp_sendtimer+2) ;
			if ((change>>i)&0x01)
			{
				if ((j & current) == j)
				{
					SendInformationEventExtended
					(7,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT17_ZONE + i ))
					,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT17_SUBZONE + i ))
					,0 , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF );
					// save when last event is sent out
						vscp_sendtimer = vscp_timer;
				}
				if (!(j & current))
				{
					SendInformationEventExtended
					(7,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT17_ZONE + i ))
					,(readEEPROM( VSCP_EEPROM_REGISTER + REG_OUTPUT17_SUBZONE + i ))
					,0 , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );
					// save when last event is sent out
						vscp_sendtimer = vscp_timer;
				}
			}
			j = j*2;
		}
	}
}

//flip over portpin value --> 8 becomes 1, 7 becomes 2 ,...
//only for 1 bit 
unsigned char portflip(unsigned char old_val)
{
	unsigned char i,reverse =7;

	for (i = 0; i!= old_val; i++)
	{
		reverse--;
	}
    return(reverse); 
}

// flip over complete byte (lsb becomes msb)
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
 *  Copyright (C) 2006-2025 Ake Hedman, Grodans Paradis AB
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
