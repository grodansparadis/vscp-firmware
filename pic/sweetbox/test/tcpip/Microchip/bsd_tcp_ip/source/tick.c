/*********************************************************************
 *
 *                  Timer Tick Manager
 *
 *********************************************************************
 * FileName:        Tick.h
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its dsPIC30F and PICmicro® Microcontroller is intended 
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip's dsPIC30F and PICmicro Microcontroller products. 
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * $Id: Tick.c,v 1.5 2006/10/24 16:39:06 C12923 Exp $
 *
 ********************************************************************/

#define TICK_INCLUDE

#include <plib.h>
#include "bsd_tcp_ip\etcpcfg.h"
#include "bsd_tcp_ip\tick.h"

#define TICK_TEMP_VALUE_1       ((CLOCK_FREQ / 2) / (TICKS_PER_SECOND * TICK_PRESCALE_VALUE))
									
#if TICK_TEMP_VALUE_1 > 65535
#error TICK_PER_SECOND value cannot be programmed with current CLOCK_FREQ
#error Either lower TICK_PER_SECOND or manually configure the Timer
#endif

#define TICK_COUNTER_16bit       TICK_TEMP_VALUE_1


#if (TICK_PRESCALE_VALUE == 2)
    #define TIMER_PRESCALE  (T1_PS_1_2)
#elif ( TICK_PRESCALE_VALUE == 4 )
    #define TIMER_PRESCALE  (T1_PS_1_4)
#elif ( TICK_PRESCALE_VALUE == 8 )
    #define TIMER_PRESCALE  (T1_PS_1_8)
#elif ( TICK_PRESCALE_VALUE == 16 )
    #define TIMER_PRESCALE  (T1_PS_1_16)
#elif ( TICK_PRESCALE_VALUE == 32 )
    #define TIMER_PRESCALE  (T1_PS_1_32)
#elif ( TICK_PRESCALE_VALUE == 64 )
    #define TIMER_PRESCALE  (T1_PS_1_64)
#elif ( TICK_PRESCALE_VALUE == 128 )
    #define TIMER_PRESCALE  (T1_PS_1_128)
#elif ( TICK_PRESCALE_VALUE == 256 )
    #define TIMER_PRESCALE  (T1_PS_1_256)
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
 * Overview:        Initializes Timer1 as a tick counter.
 *
 * Note:            None
 ********************************************************************/
void TickInit(void)
{
	OpenTimer1((TIMER_PRESCALE | T1_ON), TICK_COUNTER_16bit);
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_4);
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
 * Function:        void _ISR _T1Interrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TickCount variable is updated
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
#pragma interrupt Timer1IntHandler ipl4 vector 4
void Timer1IntHandler(void)
{
	mT1ClearIntFlag();
	TickCount ++;                      // increment TickCount

}







