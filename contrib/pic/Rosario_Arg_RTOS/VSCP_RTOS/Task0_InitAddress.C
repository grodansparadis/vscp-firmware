//******************************************************************************
//                                                                    
// File name: Task0_InitAddress.c                                             
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   v1.0                                 			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   Main VSCP TASK            
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
#include "Registers.h"
#include <VSCP_drv.h>
#include <VSCP_Functions.h>
#include <VSCP_Class.h>
#include <VSCP_Type.h>


/**********************************************************************
 * Variables shared with the rest of application.
 **********************************************************************/
extern uint08_t RXQueueCount;
uint08_t initbutton_cnt;
EventMaskType INIToLED_event;
/**********************************************************************
 *
 * The task
 *
 **********************************************************************/
TASK(VSCP_InitAddress)
{
  SetRelAlarm( ALARM_INIT, 2, 2);	// 2 mseg probe in
  SetRelAlarm( ALARM_LED, 250, 250);

  if ( !vscp_check_pstorage()) 				// Check VSCP persistent storage and restore if needed
  { 		
		vscp_setDefaultEeprom();					// No CRC Segment, then restore default parameters
	}			
																
	vscp_init();												// Initialize the VSCP functionality

  while(1)
  {
    WaitEvent(INIT_EVENT | LED_EVENT);
    GetEvent( VSCP_INIT_ID, &INIToLED_event); 


    if( INIToLED_event & INIT_EVENT)
    {
	    ClearEvent(INIT_EVENT);
    	ClrWdt();													// Clear WD
	    vscp_timer++;											// Timer 2 mseg
	
			switch ( vscp_node_state ) {
				case VSCP_STATE_STARTUP:										// Cold/warm reset, Get nickname from EEPROM
					if ( VSCP_ADDRESS_FREE == vscp_nickname ) {
						vscp_node_state = VSCP_STATE_INIT;	 		// new on segment need a nickname
					}
					else {
						vscp_node_state = VSCP_STATE_ACTIVE; 		// been here before - go on
						vscp_goActiveState();
						CancelAlarm( ALARM_INIT);
						ActivateTask( VSCP_PROT_ID);									// Go to Active State
					}
					break;
	
				case VSCP_STATE_INIT:					// Assigning nickname
					vscp_handleProbeState();
					break;
	
				case VSCP_STATE_PREACTIVE:		// Waiting for host initialisation
					vscp_handleReactive();					
					break;
	
				case VSCP_STATE_ERROR:			// Everything is *very* *very* bad.
					vscp_error();
					break;
				}// endswith
			}//endif INIT

    if( INIToLED_event & LED_EVENT)
    {
	    ClearEvent(LED_EVENT);
	
			switch ( vscp_initledfunc)
			{       //vscp_initledfunc
				case VSCP_LED_BLINK1:
					PORTBbits.RB5 = ~PORTBbits.RB5;
					break;
				case VSCP_LED_ON:
					PORTBbits.RB5 = 1;	
					break;
				case VSCP_LED_OFF:
					PORTBbits.RB5 = 0;	
					break;
			}
	
			if ( PORTBbits.RB4)
			{		// Check for init button
				initbutton_cnt = 0;
			}
			else 
			{
				initbutton_cnt++;			// Active
			}												// Init VSCP if press button for >= 2 seconds 
			if (initbutton_cnt >= 0x08)
			{
				initbutton_cnt=0;
				if ( VSCP_STATE_INIT != vscp_node_state) 
				{
					vscp_setNickname( VSCP_ADDRESS_FREE);
					vscp_setControlByte( VSCP_CONTROL_STARTUP);		// Initial startup, write allowed
					Reset();				// Go to Main Function, and start again
				}
			}
			SetEvent( VSCP_PROT_ID, WD_EVENT);
		}//endif LED
	} // endwhile
}
