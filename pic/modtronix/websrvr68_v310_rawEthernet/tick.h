/**
 * @brief           Tick Manager for Modtronix TCP/IP Stack
 * @file            tick.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    none
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 * @section description Description
 **********************************
 * This module contains the Tick code.
 *
 *********************************************************************/

 /*********************************************************************
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
 * 2006-01-04, David Hosken (DH):
 *    - Fixed bug with second tick.
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

#ifndef TICK_H
#define TICK_H

typedef unsigned long   TICK;     //32 bit tick
typedef unsigned long   TICK32;   //32 bit tick
typedef unsigned short  TICK16;   //16 bit tick
typedef unsigned char   TICK8;    //8 bit tick

#define TICKS_PER_SECOND               (100ul)        // 10ms

/** Defines the number of ticks per second */
#define TICK_SECOND             ((TICK)TICKS_PER_SECOND)


//Configure for a timeout period of 10ms. To get an exact timeout period of 10ms, we set our prescaler
//to 4. At 40MHz, this causes the timer to increment each 4 x 0.1us = 0.4us. To get the desired 10ms
//timeout period, our timer must increment 10,000us / 0.4us = 25,000 times.
//Reload value = 65536 - 25000
#define TICK_COUNTER_RELOAD (65536 - (WORD)(((CLOCK_FREQ/4)/TICKS_PER_SECOND)/4) )

#define TICK_COUNTER_HIGH       ((TICK_COUNTER_RELOAD >> 8) & 0xff)
#define TICK_COUNTER_LOW        (TICK_COUNTER_RELOAD & 0xff)

/**
 * Returns the difference between the given value and the current 32 bit tick value
 * Has 32 Bits, range of 0 - 2^32 = 11930 hours = 497 days
 */
#define TickGetDiff(t)           ((TICK)(tickCount - ((TICK)t) ))

/**
 * Returns the difference between the given value and the current 16 bit tick value.
 * Has 16 Bits, range of 0 - 65535 = 65535 x 10ms = 655 seconds = 10.9 minutes
 */
#define TickGetDiff16bit(t)      ((TICK16)( ((TICK16)tickCount) - ((TICK16)t) ))

/**
 * Returns the difference between the given value and the current 8 bit tick value
 * Has 8 Bits, range of 0 - 255 = 255 x 10ms = 2.55 seconds
 */
#define TickGetDiff8bit(t)       ((TICK8)( ((TICK8)tickCount) - ((TICK8)t) ))

/**
 * Returns the difference between the given value and the current 8 bit second
 * tick value.
 * Has 8 Bits, range of 0 - 255 seconds
 */
#define TickGetDiff8bitSec(t)       ((TICK8)( ((TICK8)tickSec) - ((TICK8)t) ))

/**
 * Returns the difference between the given value and the current 16 bit
 * second counter.
 * Has 16 Bits, range of 0 - 65,536 seconds = 0 to 18.2 hours
 */
#define TickGetSecDiff(t)        ((TICK16)( ((TICK16)tickSec) - ((TICK16)t) ))

/**
 * Returns the difference between the given value and the current 16 bit
 * second counter.
 * Has 16 Bits, range of 0 - 65,536 seconds = 0 to 18.2 hours
 */
#define TickGetDiffSec(t)        ((TICK16)( ((TICK16)tickSec) - ((TICK16)t) ))

//#define TICK_USE_ACCESSRAM

/**
 * Only Tick.c defines TICK_INCLUDE and thus defines Seconds
 * and TickValue storage.
 */
#ifndef TICK_INCLUDE

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

    extern NEAR_TICK TICK           tickCount;
    extern NEAR_TICK TICK16         tickSec;
    extern NEAR_TICK unsigned char  tickHelper;

    #ifdef TICK_USE_ACCESSRAM
        //Microchip C18 compiler
        #if defined(__18CXX)
            #pragma udata   //Return to normal section
        #endif
    #endif
#endif


/**
 * Tick manager is initialized. Initializes Timer0 as a tick counter.
 */
void TickInit(void);


/**
 * Get the current 32 bit tick value. The given tick is incremented every 10ms
 * Has 32 Bits, range of 0 - 2^32 = 11930 hours = 497 days
 * @return The current 32 bit tick value.
 */
#define TickGet() tickCount

/**
 * Get the current 16 bit tick value. The given tick is incremented every 10ms
 * Has 16 Bits, range of 0 - 65535 = 65535 x 10ms = 655 seconds = 10.9 minutes
 * @return The current 16 bit tick value.
 */
#define TickGet16bit() ((WORD)tickCount)

/**
 * Get the current 8 bit tick value. The given tick is incremented every 10ms.
 * Has 8 Bits, range of 0 - 255 = 255 x 10ms = 2.55 seconds
 * @return The current 8 bit tick value.
 */
#define TickGet8bit() ((BYTE)tickCount)

/**
 * Get the current 16 bit second tick value. The given tick is incremented every 1 second.
 * Has 16 Bits, range of 0 - 65,536 seconds = 0 to 18.2 hours
 * @return The current 16 bit second tick value.
 */
#define TickGetSec() tickSec

/**
 * Get the current 8 bit second tick value. The given tick is incremented every 1 second
 * Has 8 Bits, range of 0 - 255 seconds
 * @return The current 8 bit second tick value.
 */
#define TickGet8bitSec() ((BYTE)tickSec)

/**
 * Internal Tick and Seconds count are updated.
 */
#define TickUpdate() { \
        TMR0H = TICK_COUNTER_HIGH;              \
        TMR0L = TICK_COUNTER_LOW;               \
        tickCount++;                            \
        if (--tickHelper == 0) {                \
            tickHelper = TICKS_PER_SECOND;      \
            tickSec++;                          \
        }                                       \
    }

#endif
