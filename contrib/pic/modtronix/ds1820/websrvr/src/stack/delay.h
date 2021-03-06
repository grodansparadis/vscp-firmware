/*********************************************************************
 *
 *                  General Delay rouines
 *
 *********************************************************************
 * FileName:        delay.c
 * Dependencies:    compiler.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PICmicro� Microcontroller is intended and
 * supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -Zg -E -C
 *
 * Note: HITECH portions is adopted from PICC18 sample "delay.c".
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/9/02  Original        (Rev 1.0)
 * Nilesh Rajbharti     6/10/02 Fixed C18 ms and us routines
 ********************************************************************/
#ifndef DELAY_H
#define DELAY_H

#include "compiler.h"

#if defined(MCHP_C18)
#include <delays.h>
#endif

/*
 *  Delay functions for HI-TECH C on the PIC18
 *
 *  Functions available:
 *      DelayUs(x)  Delay specified number of microseconds
 *      DelayMs(x)  Delay specified number of milliseconds
 *
 *  Note that there are range limits:
 *  - on small values of x (i.e. x<10), the delay becomes less
 *  accurate. DelayUs is accurate with xtal frequencies in the
 *  range of 4-16MHZ, where x must not exceed 255.
 *  For xtal frequencies > 16MHz the valid range for DelayUs
 *  is even smaller - hence affecting DelayMs.
 *  To use DelayUs it is only necessary to include this file.
 *  To use DelayMs you must include delay.c in your project.
 *
 *  Note that this is the crystal frequency, the CPU clock is
 *  divided by 4.
 *
 *  MAKE SURE this code is compiled with full optimization!!!
*/
#define MHZ *1


#ifndef CLOCK_FREQ
#error CLOCK_FREQ must be defined.
#endif

#if CLOCK_FREQ < 8MHZ
#define uS_CNT  24 // 238         /* 4x to make 1 mSec */
#endif
#if CLOCK_FREQ == 8MHZ
#define uS_CNT  25 // 244
#else
#define uS_CNT  25 // 246
#endif

#define FREQ_MULT   (CLOCK_FREQ)/(4MHZ)

#if defined(HITECH_C18)
    #define Delay10us(x)                \
    {                                   \
        unsigned char _dcnt;            \
        if(x>=4)                        \
            _dcnt=(x*(FREQ_MULT))/4;    \
        else                            \
            _dcnt=1;                    \
        while(--_dcnt > 0)              \
          continue;                     \
    }


void DelayMs(unsigned char ms);

#elif defined(MCHP_C18)
#define Delay10us(us)                   Delay10TCYx(((CLOCK_FREQ/4000000)*us))
#define DelayMs(ms)                     Delay1KTCYx((((CLOCK_FREQ/1000)*ms)/4000))

#endif



#endif
