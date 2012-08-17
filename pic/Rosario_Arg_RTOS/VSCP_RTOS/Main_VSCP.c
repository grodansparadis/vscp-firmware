//******************************************************************************
//                                                                    
// File name: Main_VSCP.c                                             
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   VSCP with RTOS v1.0 (PICos18 2.10)                                 			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   General VSCP initialization and kernel start            
//                                                                    
//                                                                    
// History:                                                           
// 2007/03/07 [MAP] Create this file from PICos18 Tutorial            
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
//******************************************************************************

#include "Define_VSCP.h"
#include "Main_VSCP.h"
#include "TypeDefs.h"
#include "VSCP_Functions.h"
#include "Application.h"
extern union Timers Tmr0;
extern void vscp_init_app(void);  

AppModeType SelectedMode;

//******************************************************************************
// -------------------------- main function ---------------------------
//
// Setup the different alarms and start the kernel.
//
//******************************************************************************
void main(void)
{
	while(1)
	{
	  STKPTR = 0;
	  SelectedMode = DEFAULT_MODE;
	  OS_Init();															// PICOS18 Operating System specific Init

	  vscp_init_app();  									// specific application init in init_app()
	  StartOS(SelectedMode);							// Start Operating System
	}
}

//******************************************************************************
// Init TMR 0 and call Init_App() for specific application init 
//
// @return void
//******************************************************************************
void OS_Init(void)
{
  FSR0H = 0;
  FSR0L = 0;

  Tmr0.lt = _40MHZ; //user setting : actual PIC frequency

  T0CON = 0x08;			// Timer OFF - Enabled by Kernel
  TMR0H = Tmr0.bt[1];
  TMR0L = Tmr0.bt[0];
}

//******************************************************************************
// Hook routine called just before entering in kernel.
// 
// @param error      IN The new error stored in buffer
// @return error     Error level
//******************************************************************************
void StartupHook(void)
{
}

//******************************************************************************
// Hook routine called just after leaving the kernel.
//
// @param error      IN The last error detected by OS
// @return void
//******************************************************************************
void ShutdownHook(StatusType error)
{
}

//******************************************************************************
// Store a new error in a global buffer keeping a track of the 
// application history.
//
// @param error      IN The new error stored in buffer
// @return void
//******************************************************************************
void ErrorHook(StatusType error)
{
}

//******************************************************************************
// Hook routine called just before entering in a task.
//
// @return void
//******************************************************************************
void PreTaskHook(void)
{
}

//******************************************************************************
// Hook routine called just after leaving a task.
//
// @return void
//******************************************************************************
void PostTaskHook(void)
{
}
