/*********************************************************************
 *
 *                  Tick Manager for PIC18
 *
 *********************************************************************
 * FileName:        Tick.h
 * Dependencies:    None
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
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
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     6/28/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 ********************************************************************/

#ifndef TICK_H
#define TICK_H

#include "projdefs.h"

typedef unsigned long TICK;

#if !defined(TICKS_PER_SECOND)
    #error TICKS_PER_SECOND must be defined.
#endif

#if !defined(TICK_PRESCALE_VALUE)
    #error TICK_PRESCALE_VALUE must be defined.
#endif

#define TICK_SECOND             ((TICK)TICKS_PER_SECOND)

#define TickGetDiff(a, b)       (a-b)
//#define TickGetDiff(a, b)       (TICK)(a < b) ? (((TICK)0xffff - b) + a) : (a - b)

/*
 * Only Tick.c defines TICK_INCLUDE and thus defines Seconds
 * and TickValue storage.
 */
#ifndef TICK_INCLUDE
extern TICK TickCount;
#endif

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
void TickInit(void);



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
TICK TickGet(void);


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
//#define TickUpdate() {	/
void TickUpdate(void);

#endif
