//******************************************************************************
//                                                                    
// File name: Int_VSCP.h                                             
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   VSCP with RTOS v1.0 (PICos18 2.10)                                			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   Interrupt function            
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
#include "Int_VSCP.h"
#include "VSCP_drv.h"
#include "RMT_drv.h"

//******************************************************************************
// General interrupt vector. Do not modify.
// 
//******************************************************************************
#ifdef RELOCATE
#pragma code IT_vector_low = 0x218
#else
#pragma code IT_vector_low = 0x18
#endif

void Interrupt_low_vec(void)
{
   _asm goto InterruptVectorL  _endasm
}
#pragma code

#ifdef RELOCATE
#pragma code IT_vector_high = 0x208
#else
#pragma code IT_vector_high = 0x08
#endif

void Interrupt_high_vec(void)
{
   _asm goto InterruptVectorH  _endasm
}
#pragma code

//******************************************************************************
// General ISR router. Complete the function core with the if or switch
// case you need to jump to the function dedicated to the occuring IT.
// If your target has a small RAM space and if you do not use long type 
// or MATH routines you can avoid to save the MATH_DATA area and then
// have a smallest stack per task.
// If you are not sure let the line compiled by default.
// 
//******************************************************************************
#pragma	code _INTERRUPT_VECTORL = 0x003000
#pragma interruptlow InterruptVectorL 
void InterruptVectorL(void)
{
  EnterISR();

  if (INTCONbits.TMR0IF == 1)
    AddOneTick();						// Timer 0 Interrupt. No change please.

  if ((PIR3 & PIE3) != 0)
    VSCP_INT();							// CAN Interrupt. No change please.

#ifdef RMT_DRV

	if ( PIR1bits.TMR2IF == 1)
		RMT_INT();							// Timer 2 Interrupt. No change please.

#endif
  LeaveISR();
}
#pragma	code

// BE CARREFULL : ONLY BSR, WREG AND STATUS REGISTERS ARE SAVED  
// DO NOT CALL ANY FUNCTION AND USE PLEASE VERY SIMPLE CODE LIKE 
// VARIABLE OR FLAG SETTINGS. CHECK THE ASM CODE PRODUCED BY C18 
// IN THE LST FILE.                                              

#pragma	code _INTERRUPT_VECTORH = 0x003300
#pragma interrupt InterruptVectorH nosave=FSR0, TBLPTRL, TBLPTRH, TBLPTRU, TABLAT, PCLATH, PCLATU, PROD, section(".tmpdata"), section("MATH_DATA")
void InterruptVectorH(void)
{
	if (INTCONbits.INT0IF == 1)
  	INTCONbits.INT0IF = 0;
}
#pragma	code


//******************************************************************************
// Reset function
// 
//******************************************************************************
#pragma code _RESET_INTERRUPT_VECTOR = 0x003400
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code
