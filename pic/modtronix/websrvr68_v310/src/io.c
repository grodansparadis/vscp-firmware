 /**
 * @brief           General IO Code.
 * @file            io.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_io
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2009-04-22, David Hosken (DH):
 *    - Initial code
 *********************************************************************/

#define THIS_IS_IO

#include "projdefs.h"
#include "io.h"


/////////////////////////////////////////////////
// Defines



/////////////////////////////////////////////////
// Event Latches of Expansion Board Inputs

//Event latch of expansion board inputs. Corresponding bit in evtLatchExInUpdt indicates which bits contain new values
WORD evtLatchExIn;

//Indicates which expansion board input latches have been updated. When updated by io.c, this bit is set. When read by
//event code, bit is cleared.
WORD evtLatchExInUpdt;


/////////////////////////////////////////////////
// Function Prototypes


/**
 * Initialize
 */
void ioInit(void)
{
    evtLatchExIn = 0;
    evtLatchExInUpdt = 0;
}


/**
 * Service this module. This code is called every couple of ms
 */
void ioTask(void)
{
}



