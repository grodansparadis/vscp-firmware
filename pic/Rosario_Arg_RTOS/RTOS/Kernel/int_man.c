/**********************************************************************/
/*                                                                    */
/* File name: int_man.c                                               */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v2.10                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*                                                                    */
/* Purpose:   Interrupt manager (set and clear interrupts)            */
/*                                                                    */
/* Distribution: This file is part of PICos18.                        */
/*            PICos18 is free software; you can redistribute it       */
/*            and/or modify it under the terms of the GNU General     */
/*            Public License as published by the Free Software        */
/*            Foundation; either version 2, or (at your option)       */
/*            any later version.                                      */
/*                                                                    */
/*            PICos18 is distributed in the hope that it will be      */
/*            useful, but WITHOUT ANY WARRANTY; without even the      */
/*            implied warranty of MERCHANTABILITY or FITNESS FOR A    */
/*            PARTICULAR PURPOSE.  See the GNU General Public         */
/*            License for more details.                               */
/*                                                                    */
/*            You should have received a copy of the GNU General      */
/*            Public License along with gpsim; see the file           */
/*            COPYING.txt. If not, write to the Free Software         */
/*            Foundation, 59 Temple Place - Suite 330,                */
/*            Boston, MA 02111-1307, USA.                             */
/*                                                                    */
/*          > A special exception to the GPL can be applied should    */
/*            you wish to distribute a combined work that includes    */
/*            PICos18, without being obliged to provide the source    */
/*            code for any proprietary components.                    */
/*                                                                    */
/* History:                                                           */
/* 2004/09/20 [XM]  Create this file.                                 */
/*                                                                    */
/**********************************************************************/

#include "device.h"

extern unsigned char kernelState;
unsigned char kernel_copy;

/**********************************************************************
 * ROM area of the interrupt manager.
 *********************************************************************/
#pragma		code	KERNEL_ROM

/**********************************************************************
 * Set Global Interrupt Enable.
 *
 * @param void
 * @return void
 **********************************************************************/
void EnableAllInterrupts(void)
{
  INTCONbits.GIEH = 1;
  INTCONbits.GIEL = 1;
}

/**********************************************************************
 * Clear Global Interrupt Enable.
 *
 * @param void
 * @return void
 **********************************************************************/
void DisableAllInterrupts(void)
{
  INTCONbits.GIEH = 0;
  INTCONbits.GIEL = 0;
}

/**********************************************************************
 * Set Global Interrupt Enable.
 *
 * @param void
 * @return void
 **********************************************************************/
void ResumeAllInterrupts(void)
{
  INTCONbits.GIEH = 1;
  INTCONbits.GIEL = 1;
}

/**********************************************************************
 * Clear Global Interrupt Enable.
 *
 * @param void
 * @return void
 **********************************************************************/
void SuspendAllInterrupts(void)
{
  INTCONbits.GIEH = 0;
  INTCONbits.GIEL = 0;
}

/**********************************************************************
 * Set Global Interrupt Enable.
 *
 * @param void
 * @return void
 **********************************************************************/
void ResumeOSInterrupts(void)
{
  INTCONbits.GIEL = 1;
}

/**********************************************************************
 * Clear Global Interrupt Enable.
 *
 * @param void
 * @return void
 **********************************************************************/
void SuspendOSInterrupts(void)
{
  INTCONbits.GIEL = 0;
}

/**********************************************************************
 * Set the ISR settings to disble the Schedule function if called
 * during the ISR.
 *
 * @param void
 * @return void
 **********************************************************************/
void EnterISR(void)
{
  kernel_copy  = kernelState;
  kernelState |= ISR;
}

/**********************************************************************
 * Restore the kernel state and enable the Schedule function:
 * if a service like SetEvent has been called by an ISR then the service
 * didn't schedule when it's called but at the end of the ISR, thanks to
 * the LeaveISR function.
 *
 * @param void
 * @return void
 **********************************************************************/
void LeaveISR(void)
{    
  if (kernelState & SERVICES)
  {
    kernelState = kernel_copy;
    Schedule();
  }
  kernelState = kernel_copy;
}


/* End of File : int_man.c */
