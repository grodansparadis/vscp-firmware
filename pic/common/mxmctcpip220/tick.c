/*********************************************************************
 * mac.c    -       MAC Module for Microchip TCP/IP Stack
 * Dependencies:    stacktsk.h,  tick.h, mac.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Provides a system tick and software timers.
 *
 *********************************************************************
 * File History
 *
 * 2002-05-22, Nilesh Rajbharti - Rev 2.0 (See version.log for detail)
 * 2002-02-09, Nilesh Rajbharti - Cleanup
 * 2001-06-28, Nilesh Rajbharti - Original (Rev 1.0)
 *
 *********************************************************************
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
 *
 ********************************************************************/


#define TICK_INCLUDE

#include "tick.h"

#define TICK_TEMP_VALUE_1       \
        ((CLOCK_FREQ / 4) / (TICKS_PER_SECOND * TICK_PRESCALE_VALUE))

#if TICK_TEMP_VALUE_1 > 60000
#error TICK_PER_SECOND value cannot be programmed with current CLOCK_FREQ
#error Either lower TICK_PER_SECOND or manually configure the Timer
#endif

#define TICK_TEMP_VALUE         (65535 - TICK_TEMP_VALUE_1)

#define TICK_COUNTER_HIGH       ((TICK_TEMP_VALUE >> 8) & 0xff)
#define TICK_COUNTER_LOW        (TICK_TEMP_VALUE & 0xff)

#if (TICK_PRESCALE_VALUE == 2)
    #define TIMER_PRESCALE  (0)
#elif ( TICK_PRESCALE_VALUE == 4 )
    #define TIMER_PRESCALE  (1)
#elif ( TICK_PRESCALE_VALUE == 8 )
    #define TIMER_PRESCALE  (2)
#elif ( TICK_PRESCALE_VALUE == 16 )
    #define TIMER_PRESCALE  (3)
#elif ( TICK_PRESCALE_VALUE == 32 )
    #define TIMER_PRESCALE  (4)
#elif ( TICK_PRESCALE_VALUE == 64 )
    #define TIMER_PRESCALE  (5)
#elif ( TICK_PRESCALE_VALUE == 128 )
    #define TIMER_PRESCALE  (6)
#elif ( TICK_PRESCALE_VALUE == 256 )
    #define TIMER_PRESCALE  (7)
#else
    #error Invalid TICK_PRESCALE_VALUE specified.
#endif




TICK TickCount = 0;


/*********************************************************************
 * Function:        void TickInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Tick manager is initialized.
 *
 * Side Effects:    None
 *
 * Overview:        Initializes Timer0 as a tick counter.
 *
 * Note:            None
 ********************************************************************/
void TickInit(void)
{
    // Start the timer.
    TMR0L = TICK_COUNTER_LOW;
    TMR0H = TICK_COUNTER_HIGH;

    // 16-BIT, internal timer, PSA set to 1:256
    T0CON = 0b00000000 | TIMER_PRESCALE;

    // Start the timer.
    T0CON_TMR0ON = 1;

    INTCON_TMR0IF = 1;
    INTCON_TMR0IE = 1;
}


/*********************************************************************
 * Function:        TICK TickGet(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Current second value is given
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
TICK TickGet(void)
{
    return TickCount;
}


/*********************************************************************
 * Function:        void TickUpdate(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Internal Tick and Seconds count are updated.
 *
 * Note:            None
 ********************************************************************/
void TickUpdate(void)
{
    TMR0H = TICK_COUNTER_HIGH;
    TMR0L = TICK_COUNTER_LOW;

    TickCount++;
}









