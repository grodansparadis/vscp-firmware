//******************************************************************************
//                                                                    
// File name: Task3_Loop.c                                             
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   v1.0                                 			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   General Initialization and Kernel Start            
//                                                                    
//                                                                    
// History:                                                           
// 2007/03/07 [MAP] Create this file. Ideas are taken from 
//																		PICos18 Tutorial 
//                                    VSCP Ake Hedman, eurosource             
//                                                                    
//            PICos18 is free software; you can redistribute it       
//            and/or modify it under the terms of the GNU General     
//            Public License as published by the Free Software        
//            Foundation; either version 2, or (at your option)       
//            any later version.                                      
//                                                                    
//            PICos18 is distributed in the hope that it will be      
//            useful, but WITHOUT ANY WARRANTY; without even the      
//            implied warranty of MERCHANTABILITY or FITNESS FOR A    
//            PARTICULAR PURPOSE.  See the GNU General Public         
//            License for more details.                               
//                                                                    
//            You should have received a copy of the GNU General      
//            Public License along with gpsim; see the file           
//            COPYING.txt. If not, write to the Free Software         
//            Foundation, 59 Temple Place - Suite 330,                
//            Boston, MA 02111-1307, USA.                             
//                                                                    
//          > A special exception to the GPL can be applied should    
//            you wish to distribute a combined work that includes    
//            PICos18, without being obliged to provide the source    
//            code for any proprietary components. 
//
//  	VSCP (Very Simple Control Protocol)                                            
//  	http://www.vscp.org                                                            
//                                                                                   
//  	akhe@eurosource.se                                                             
//                                                                                   
//   Copyright (C) 1995-2006 Ake Hedman, eurosource, <akhe@eurosource.se>            
//                                                                                   
//  This software is provided 'as-is', without any express or implied                
//  warranty.  In no event will the authors be held liable for any damages           
//  arising from the use of this software.                                           
//                                                                                   
//  Permission is granted to anyone to use this software for any purpose,            
//  including commercial applications, and to alter it and redistribute it           
//  freely, subject to the following restrictions:                                   
//                                                                                   
//  1. The origin of this software must not be misrepresented; you must not          
//     claim that you wrote the original software. If you use this software          
//     in a product, an acknowledgment in the product documentation would be         
//     appreciated but is not required.                                              
//  2. Altered source versions must be plainly marked as such, and must not be       
//     misrepresented as being the original software.                                
//  3. This notice may not be removed or altered from any source distribution.      
//                                                                                    
//   	This file is part of VSCP - Very Simple Control Protocol 	                      
//   	http://www.vscp.org                                                             
//                                                                                    
//******************************************************************************

#include "Define_VSCP.h"
#include "VSCP_Functions.h"
#include "VSCP_drv.h"
#include "VSCP_Class.h"
#include "VSCP_Type.h"
#include <Registers.h>
#include <eeprom.h>

//******************************************************************************
// Definition dedicated to the local functions.
//******************************************************************************
#define STATUS_CLOSE 0
#define STATUS_OPEN  1

void ReadAndOut( void);
 
EventMaskType Loop_event;
uint08_t IteraSteep;
uint08_t IteraOut;
uint08_t IteraTimer;
uint08_t Ralenti;
uint08_t StatusDoor;
uint08_t Flag_Reverse;
uint08_t AlertRepeatTimes;
uint08_t AlertOnTime;
uint08_t AlertOffTime;
uint08_t AlertIndex;
uint08_t StatusAlert;
uint08_t AlertClosed;
uint08_t OpenLimit;
uint08_t CloseLimit;
uint08_t EnableLimits;
uint08_t LedPulse;
uint08_t PostScaler;
VSCP_msg_t  SND1_message;
//******************************************************************************
// ------------------------------ TASK3 -------------------------------
//
//******************************************************************************
TASK(VSCP_Loop) 
{

	IteraSteep = 0;
	IteraTimer = 0;
	Flag_Reverse = 0;
	EnableLimits = 0;
	StatusDoor = STATUS_CLOSE;
	StatusAlert = FALSE;
  SetRelAlarm(ALARM_LOOP, 20, 20);
  Ralenti = 0;
  while(1)
  {

    WaitEvent( TIC_EVENT | DOOR_EVENT | ALERT_EVENT_ON | ALERT_EVENT_OFF | WD_EVENT);
    GetEvent( VSCP_LOOP_ID, &Loop_event); 
    
 
    if( Loop_event & DOOR_EVENT)					
    {
      ClearEvent(DOOR_EVENT);
      if( STATUS_CLOSE == StatusDoor)
      { 
      	StatusDoor = STATUS_OPEN;				// Set Status of Door Open at receive event. Init Door Open cicle
      	OpenLimit = PORTAbits.RA0;			// Read Open and close limit switch (Normal Closed)
      	CloseLimit = PORTAbits.RA1;			// Read Open and close limit switch (Normal Closed)
				
				SND1_message.priority = VSCP_PRIORITY_HIGH;		// inform change of state to open
				SND1_message.vscp_class = VSCP_CLASS1_INFORMATION;				
				SND1_message.vscp_type = VSCP_TYPE_INFORMATION_OPENED;
				SND1_message.length  = 8;
				SND1_message.data[0] = 0x01;
				SND1_message.data[1] = 0xff;									// send to all zones
				SND1_message.data[2] = 0x10;									// send to all Subzone
				SND1_message.data[3] = 0xff;       
				SND1_message.data[4] = 0xff;       
				SND1_message.data[5] = 0xff;       
				SND1_message.data[6] = 0xff;
				SND1_message.data[7] = 0xff;
				if( VSCP_enqMsgTx( &SND1_message, TRUE)== E_OK);		//Send to Itself
      }
      else if( Flag_Reverse)
      {
      	IteraTimer = 0;									// if flag reverse is set (in E2PROM), timer is set to 0,  next ciclo is taken
      }
    }//endif DOOR_EVENT
    
     if( Loop_event & ALERT_EVENT_ON)		// If ALERT_EVENT_ON port RC1 is pulse 100ms for about 2 sec
    {
      ClearEvent(ALERT_EVENT_ON);
      StatusAlert = TRUE;
      AlertRepeatTimes = 3;
      AlertOnTime = 10;
      AlertOffTime = 5;
      AlertIndex = AlertOnTime;
    }//endif ALERT_EVENT_ON
 
      if( Loop_event & ALERT_EVENT_OFF)	// If ALERT_EVENT_OFF port RC1 is pulse 100ms for about 3 sec
    {
      ClearEvent(ALERT_EVENT_OFF);
      StatusAlert = TRUE;
      AlertRepeatTimes = 5;
      AlertOnTime = 20;
      AlertOffTime = 5;
      AlertIndex = AlertOnTime;
    }//endif ALERT_EVENT_OFF
      
    if( Loop_event & TIC_EVENT)					// Timer TIC every 100ms (ALARM_LOOP!)
    {
      ClearEvent(TIC_EVENT);																									
      OpenLimit = ( OpenLimit  <<= 1) | PORTAbits.RA0; // We need to detect transition with debounce 
      CloseLimit =( CloseLimit <<= 1) | PORTAbits.RA1; // We need to detect transition with debounce 

			if( LedPulse++ & 0x20)
			{
				if( LedPulse++ & 0x08)
					PORTCbits.RC2 = 1;
				else	
					PORTCbits.RC2 = 0;
			}

			if( Ralenti)											// Reduced velocity, 1 cycle on, 7 cycles off
			{
				Ralenti--;
				if(( Ralenti & 0x07) == 0x01)
					PORTCbits.RC4 = 1;
				else
					PORTCbits.RC4 = 0;	
			}	      

      if(( OpenLimit & 0x0f) == 0x07)  	// Detect low-2-hight transition in Open Limit Switch
			{
				if( EnableLimits)
				{
      		IteraTimer = 0;									// timer is set to 0,  next ciclo is taken
      		Ralenti = readEEPROM( DOOR_RALENTI_OPEN);
      	}
			}

      if(( CloseLimit & 0x0f) == 0x07)	// Detect low-2-hight transition in Close Limit Switch
			{
				if( EnableLimits)
				{
	      	IteraTimer = 0;									// timer is set to 0,  next ciclo is taken
  	    	Ralenti = readEEPROM( DOOR_RALENTI_CLOSED);
				}
			}
                                                                                             
////////      if(( OpenLimit & 0x0f) == 0x08)  	// Detect hight-2-low transition in Open Limit Switch
////////      if(( CloseLimit & 0x0f) == 0x08)	// Detect hight-2-low transition in Close Limit Switch

			if( !PostScaler--)									// POstScaler to set basetime = 100 ms
			{
				PostScaler = 4;
	      switch( StatusDoor)
	      {
	      	case STATUS_CLOSE:							// Reset timer and steep in STATUS_CLOSE 
	  	    	IteraSteep = 0;
						IteraTimer = 0;      		
	      		break;
	      	
	      	case STATUS_OPEN:								// if in STATUS_OPEN cycle, 
						if( !IteraTimer --)						// when IteraTimer is cero, execute ReadAndOut. At init is cero.
						{
							ReadAndOut();
						}
	      		break;
	      } //end switch StatusDoor
			}// endif PostScaler
			
      if( AlertRepeatTimes)								// Sequence repeat n times
      {
				switch( StatusAlert)							// Output Status 
				{	
					case TRUE:											// First time are here
						PORTCbits.RC1 = 1;
						if( ! --AlertIndex)						// When reach zero, change Status and load next status time
						{
							StatusAlert = FALSE;
							AlertIndex = AlertOffTime;
						}	
						break;
					
					case FALSE:
						PORTCbits.RC1 = 0;
						if( ! --AlertIndex)						// When reach zero, change Status and load next status time
						{
							StatusAlert = TRUE;
							AlertIndex = AlertOnTime;
							AlertRepeatTimes--;					// One cycle, decrement repeat times
						}	
						break;
				}//end switch StatusAlert
      }//endif AlertRepeatTimes
    } // endif TIC_EVENT

    if( Loop_event & WD_EVENT)					// Daisy chain WatchDog
    {
      ClearEvent(WD_EVENT);
    	ClrWdt();													// Clear WD
		}// endif WD_EVENT
  } // end while
} // end Task
 
void ReadAndOut( void) 
{
	IteraOut   = readEEPROM( DOOR_TABLE_START + ( 2*IteraSteep));		// Read Out Position
	IteraTimer = readEEPROM( DOOR_TABLE_START + ( 2*IteraSteep+1)); // Read Timer Position

	if( 0x00 == IteraTimer)
	{
		StatusDoor = STATUS_CLOSE;					// if E2PROM timer Position is 0, end of cycle
	}

	IteraSteep++;													// Implement E2PROM Out Position 
	PORTCbits.RC0 = ( IteraOut)& 0x01;
	AlertClosed 	= ( IteraOut >>= 1)& 0x01;	
	EnableLimits  = ( IteraOut >>= 1)& 0x01;	
	PORTCbits.RC3 = ( IteraOut >>= 1)& 0x01;		
	PORTCbits.RC4 = ( IteraOut >>= 1)& 0x01;	
	PORTCbits.RC5 = ( IteraOut >>= 1)& 0x01;	
	PORTAbits.RA4 = ( IteraOut >>= 1)& 0x01;	
	Flag_Reverse  = ( IteraOut >>= 1)& 0x01;

	if( AlertClosed)
	{
		SND1_message.priority = VSCP_PRIORITY_HIGH;		// inform change of state to closed
		SND1_message.vscp_class = VSCP_CLASS1_INFORMATION;				
		SND1_message.vscp_type = VSCP_TYPE_INFORMATION_CLOSED;
		SND1_message.length  = 8;
		SND1_message.data[0] = 0x01;
		SND1_message.data[1] = 0xff;									// send to all zones
		SND1_message.data[2] = 0xff;									// send to all Subzone
		SND1_message.data[3] = 0xff;       
		SND1_message.data[4] = 0xff;       
		SND1_message.data[5] = 0xff;       
		SND1_message.data[6] = 0xff;
		SND1_message.data[7] = 0xff;			
		if( VSCP_enqMsgTx( &SND1_message, TRUE)== E_OK);		//Send to Itself
	}// Endif AlertClosed
}
