/******************************************************************************
*                                                                             *
*  File:                        TIMER.H                                       *
*                                                                             *
*  Description: This file contains the API for the PIC18F timing system which *
*               implements a free running 1msec timer using the timer 0       *
*               peripheral and interrupt. To use this timer install the       *
*               timer_intr() interrupt handler. Also note that the value      *
*               "ulFreq" passed as a parameter to timer_init() is the         *
*               frequency in Hz of the clock that enters the timer 0          *
*               pre-scaler block. If the internal/external oscillator is the  *
*               configured clock source then that frequency divided by 4      *
*               (the instruction clock frequency) is what you pass in. If you *
*               choose the external clock pin as the clock source then you    *
*               pass that frequency in unmodified. Include the header file    *
*               and following code in the appropriate high/low priority       *
*               interrupt depending on the interrupt selected by              *
*               TIMER0_INT_PRIORITY:                                          *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Insert into high priority interrupt handler...                             *
*                                                                             *
*  #if TIMER0_INT_PRIORITY==TIMER_PRIORITY_HIGH                               *
*   if( INTCONbits.TMR0IF && INTCONbits.TMR0IE )                              *
*   {                                                                         *
*      timer_intr();                                                          *
*   }                                                                         *
*  #endif                                                                     *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Insert into low priority interrupt handler...                              *
*                                                                             *
*  #if TIMER0_INT_PRIORITY==TIMER_PRIORITY_LOW                                *
*   if( INTCONbits.TMR0IF && INTCONbits.TMR0IE )                              *
*   {                                                                         *
*      timer_intr();                                                          *
*   }                                                                         *
*  #endif                                                                     *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Software License Agreement                                                 *
*                                                                             *
*  The software supplied herewith by Microchip Technology Incorporated        *
*  (the “Company”) for its PICmicro® Microcontroller is intended and          *
*  supplied to you, the Company’s customer, for use solely and                *
*  exclusively on Microchip PICmicro Microcontroller products. The            *
*  software is owned by the Company and/or its supplier, and is               *
*  protected under applicable copyright laws. All rights are reserved.        *
*   Any use in violation of the foregoing restrictions may subject the        *
*  user to criminal sanctions under applicable laws, as well as to            *
*  civil liability for the breach of the terms and conditions of this         *
*  license.                                                                   *
*                                                                             *
*  THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,          *
*  WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED          *
*  TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A                *
*  PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,          *
*  IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR                 *
*  CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.                          *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Created By:  Thomas R. Shelburne 05/27/04                                  *
*                                                                             *
*  Versions:                                                                  *
*                                                                             *
*  MAJ.MIN - AUTHOR - CHANGE DESCRIPTION                                      *
*                                                                             *
*   1.0    - T.R.S. - Initial release for PIC18F microcontroller family...    *
*                                                                             *
*******************************************************************************/

#include "define.h"
#include <p18cxxx.h>
#include "stderror.h"

/* Timer 0 interrupt control macro definitions... */
#define TIMER0_INT_MASK_ENABLE    INTCONbits.TMR0IE = 1
#define TIMER0_INT_MASK_DISABLE   INTCONbits.TMR0IE = 0
#define TIMER0_CLEAR_PENDING_INT  INTCONbits.TMR0IF = 0

/* Configure timer subsystem interrupt priority utilized... */
#define TIMER_PRIORITY_LOW        0 /* 0 - Do not change... */
#define TIMER_PRIORITY_HIGH       1 /* 1 - Do not change... */
#define TIMER0_INT_PRIORITY       TIMER_PRIORITY_HIGH /* Indicates whether high or low priority interrupt is used... */

/* Configure timer clock select... */
#define TIMER_SEL_INTRUCT_CLK     0 /* 0 - Do not change... */
#define TIMER_SEL_EXT_CLK         1 /* 1 - Do not change... */
#define TIMER0_DEF_CLOCK_SOURCE   TIMER_SEL_INTRUCT_CLK /* Indicates default selected clock source... */

/* Functional frequency range... */
#define TIMER0_MIN_FREQ           500000 /* 500kHz - Do not change... */
#define TIMER0_MAX_FREQ           40000000 /* 40MHz - Do not change... */

#ifdef _TIMER_H_
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Multiple inclusion protection... */
#ifndef _TIMER_TYPES_
#define _TIMER_TYPES_
/* Global typdefs... */
typedef enum
{
  TIMER_EXPIRED = 0,
  TIMER_RUNNING = 1
} TIMER_STATUS;

typedef struct
{
  ULONG ulTimer;
  UWORD uwOverflowCnt;
} _TIMER_;
#endif

/* Function declarations for timer subsystem API... */
GLOBAL STDERROR     timer_init( ULONG ulFreq );                                  /* Initialize the 1msec free running timer...     */
GLOBAL STDERROR     timer_sel_clk_source( UBYTE ucClkSrcSel );                   /* Select clock source used...                    */
GLOBAL void         timer_start( _TIMER_ *pUserTimer, ULONG ulDelayMs );         /* Start a new timer...                           */
GLOBAL TIMER_STATUS timer_check( _TIMER_ *pUserTimer );                          /* Check a timer for expiration...                */
GLOBAL void         timer_get_current( _TIMER_ *pCurrentTime );                  /* Get the latest free running timer value...     */
GLOBAL void         timer_restart( _TIMER_ *pUserTimer, _TIMER_ *pCurrentTime ); /* Re-start an active timer after clock change... */
GLOBAL void         timer_intr( void );                                          /* The timer 0 interrupt handler...               */

#undef GLOBAL
