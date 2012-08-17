/**
 * @brief           Tick Module for Modtronix TCP/IP Stack
 * @file            tick.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 **********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2005-12-21, David Hosken (DH):
 *    - Tick code redone. Added support for 16bit and 8bit tick counters.
 *      This optimizes code size and speed.
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 * 2002-12-05, Nilesh Rajbharti:
 *    - Modified TCPProcess() to include localIP as third param.
 *      This was done to allow this function to calculate checksum correctly.
 * 2002-11-01, Nilesh Rajbharti:
 *    - Fixed TCPTick() SYN Retry bug.
 * 2002-05-22, Nilesh Rajbharti:
 *    - Rev 2.0 (See version.log for detail)
 * 2001-05-08, Nilesh Rajbharti:
 *    - Original Rev 1.0
 *********************************************************************/

#define TICK_INCLUDE

#include "projdefs.h"
#include "net\checkcfg.h" 
#include "net\tick.h"


//If TICK_USE_ACCESSRAM has been defined, we use access RAM variables
//to optiomize speed and code size
#ifdef TICK_USE_ACCESSRAM
    #define NEAR_TICK near
    //Microchip C18 compiler
    #if defined(__18CXX)
        #pragma udata access accessram  //Indicate following data is in Access RAM section
    #endif
#else
    #define NEAR_TICK
#endif

NEAR_TICK TICK           tickCount;
NEAR_TICK TICK16         tickSec;
NEAR_TICK unsigned char  tickHelper;

#ifdef TICK_USE_ACCESSRAM
    //Microchip C18 compiler
    #if defined(__18CXX)
        #pragma udata   //Return to normal section
    #endif
#endif


/**
 * Tick manager is initialized. Initializes Timer0 as a tick counter.
 */
void TickInit(void)
{
    tickCount = 0;
    tickSec = 0;
    
    // Start the timer.
    TMR0L = TICK_COUNTER_LOW;
    TMR0H = TICK_COUNTER_HIGH;

    // 16-BIT, internal timer, PSA set to 1:4
    //Assign the prescaller value as low as possible to minimize the affect of the prescaller
    //being reset each time we update the timer count! The prescaller should also not be too
    //low, else TMR0H might already have incremented once we add the reload value to TMR0L
    T0CON = 0b00000001;

    // Start the timer.
    T0CON_TMR0ON = 1;

    INTCON_TMR0IF = 1;
    INTCON_TMR0IE = 1;
}
