/******************************************************************************
*                                                                             *
*  File:                                   TIMER.C                            *
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
*  	if( INTCONbits.TMR0IF && INTCONbits.TMR0IE )                              *
*  	{                                                                         *
*      timer_intr();                                                          *
*  	}                                                                         *
*  #endif                                                                     *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Insert into low priority interrupt handler...                              *
*                                                                             *
*  #if TIMER0_INT_PRIORITY==TIMER_PRIORITY_LOW                                *
*  	if( INTCONbits.TMR0IF && INTCONbits.TMR0IE )                              *
*  	{                                                                         *
*      timer_intr();                                                          *
*  	}                                                                         *
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

#include <limits.h>
#define  _TIMER_H_
#include "timer.h"
#undef   _TIMER_H_

/* Local macro and constant definitions... */
#define TIMER0_HARDWARE_DISABLE   T0CONbits.TMR0ON = 0
#define TIMER0_HARDWARE_ENABLE    T0CONbits.TMR0ON = 1

/* Local function declarations... */
static STDERROR init_timer_settings( ULONG ulClkFreq );

/* Local variable declarations... */
static _TIMER_ sFreeRunningTimer;
static UBYTE ucTimerReloadHigh;
static UBYTE ucTimerReloadLow;
static UBYTE ucClockSourceSelect;
static UBYTE ucFirstTime = TRUE;

/******************************************************************************
*                      GLOBAL FUNCTION DEFINITIONS                            *
******************************************************************************/

/******************************************************************************
*                                                                             *
*  Function: STDERROR timer_init( ULONG ulFreq )                              *
*  Description: This function initializes the timer and its interrupt to      *
*               generate a free running 1msec timer which can be used to time *
*               scheduled tasks in the main application. This function is     *
*               passed the core clock frequency and calculates the correct    *
*               settings for the timer control registers based on the input   *
*               frequency. This feature can be used to adjust the timer       *
*               properly when switching frequency of operation when enabling  *
*               or disabling the PLL or switching from internal to external   *
*               clock sources. Note that any active timers must be re-started *
*               after this call or they will not function properly...         *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
STDERROR timer_init( ULONG ulFreq )
{
  STDERROR erReturnCode;

  /* Disable the timer hardware and interrupts (in case already active)... */
  TIMER0_HARDWARE_DISABLE;
  TIMER0_INT_MASK_DISABLE;
  TIMER0_CLEAR_PENDING_INT;

  /* Calculate and load timer settings based on "ulFreq" input freuency... */
  erReturnCode = init_timer_settings( ulFreq );

  /* See if the timer was correctly configured before enabling... */
  if ( GET_ERROR(erReturnCode) == SUCCESS )
  {
    /* Setup the timer interrupt bits... */
#if (TIMER0_INT_PRIORITY == TIMER_PRIORITY_HIGH)
    INTCON2bits.TMR0IP = 1; /* Set for High priority interrupt... */
#else
    INTCON2bits.TMR0IP = 0; /* Set for Low priority interrupt... */
#endif

    /* Enable the timer interrupt mask... */
    TIMER0_INT_MASK_ENABLE; /* Enable the timer interrupt mask... */

    /* Enable the timer hardware... */
    TIMER0_HARDWARE_ENABLE;
  }

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: STDERROR timer_sel_clk_source( UBYTE ucClkSrcSel )               *
*  Description: This function allows the user to change the active clock      *
*               source for the timer. New settings will not take effect until *
*               after a call to timer_init(). If this is called before the    *
*               first call to timer_init() then the defaut clock selection    *
*               will be overridden...                                         *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
STDERROR timer_sel_clk_source( UBYTE ucClkSrcSel )
{
  STDERROR erReturnCode = ERR(TIMER,SUCCESS);

  /* Attempt to store the new value for the selected clock source... */
  ucFirstTime = FALSE;
  switch ( ucClkSrcSel )
  {
    case TIMER_SEL_INTRUCT_CLK:
    case TIMER_SEL_EXT_CLK:
      /* Store the new clock select value... */
      ucClockSourceSelect = ucClkSrcSel;
      break;

     default:
       /* Invalid value, restore the default value... */
       ucClockSourceSelect = TIMER0_DEF_CLOCK_SOURCE;
       erReturnCode = ERR(TIMER,INVALID_PARAM);
       break;
  }

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: void timer_start( _TIMER_ *pUserTimer, ULONG ulDelayMs )         *
*  Description: This function starts a timer by writing to the users struct   *
*               data the value that the free running timer must obtain to be  *
*               considered expired including the number of overflows required *
*               for longer delays...                                          *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void timer_start( _TIMER_ *pUserTimer, ULONG ulDelayMs )
{
  ULONG ulTempCount;
  UWORD uwTempOverFlow;
  
  /* Begin Critical region... */
  TIMER0_INT_MASK_DISABLE;

  /* Save copy of free running timer... */
  ulTempCount = sFreeRunningTimer.ulTimer;

  /* Save copy of timer over flow counter...    */
  uwTempOverFlow = sFreeRunningTimer.uwOverflowCnt;

  /* End critical region... */
  TIMER0_INT_MASK_ENABLE;

  /* Initialize the users timer variable... */
  pUserTimer->ulTimer = ulTempCount + ulDelayMs; /* Let it overflow... */

  /* See if we need to take into account counter overflow... */
  if ( (ULONG_MAX - ulTempCount) >= ulDelayMs )
  {
    /* Timout will occur BEFORE counter overflow... */
    pUserTimer->uwOverflowCnt = uwTempOverFlow;
  }
  else
  {
    /* Timout will occur AFTER counter overflow... */
    pUserTimer->uwOverflowCnt = uwTempOverFlow + 1;
  }
}

/******************************************************************************
*                                                                             *
*  Function: TIMER_STATUS timer_check( _TIMER_ *pUserTimer )                  *
*  Description: This function checks the timer value to see if it has expired *
*               and returns the result to the application...                  *
*  Returns: TIMER_STATUS                                                      *
*                                                                             *
******************************************************************************/
TIMER_STATUS timer_check( _TIMER_ *pUserTimer )
{
  ULONG ulTempCount;
  UWORD uwTempOverFlow;
  
  /* Begin Critical region... */
  TIMER0_INT_MASK_DISABLE;

  /* Save copy of free running timer... */
  ulTempCount = sFreeRunningTimer.ulTimer;

  /* Save copy of over flow count...    */
  uwTempOverFlow = sFreeRunningTimer.uwOverflowCnt;

  /* End critical region... */
  TIMER0_INT_MASK_ENABLE;

  /* See if the timer has expired... */
  if ( pUserTimer->uwOverflowCnt > uwTempOverFlow )
  {
    return TIMER_RUNNING;
  }
  else if ( pUserTimer->uwOverflowCnt < uwTempOverFlow )
  {
    return TIMER_EXPIRED;
  }
  else /* Overflow counts are equal... */
  {
    if ( pUserTimer->ulTimer > ulTempCount )
    {
      return TIMER_RUNNING;
    }
    else
    {
      return TIMER_EXPIRED;
    }
  }
}

/******************************************************************************
*                                                                             *
*  Function: void  timer_get_current( _TIMER_ *pCurrentTime )                 *
*  Description: This function returns the current value of the free running   *
*               timer. This function should be used to grab the current value *
*               of the free-running timer before changing core clock          *
*               frequencies so that timers can be re-started after a call to  *
*               timer_init() if required...                                   *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void timer_get_current( _TIMER_ *pCurrentTime )
{
  /* Make sure that the timer pointer is valid... */
  if ( pCurrentTime != NULL )
  {
    /* Begin Critical region... */
    TIMER0_INT_MASK_DISABLE;

    /* Copy the current free running timer value to user memory... */
    *(pCurrentTime) = sFreeRunningTimer;

    /* End critical region... */
    TIMER0_INT_MASK_ENABLE;
  }
}

/******************************************************************************
*                                                                             *
*  Function: void timer_restart( _TIMER_ *pUserTimer, _TIMER_ *pCurrentTime ) *
*  Description: This function will re-start the remaining time on an existing *
*               timer based on the last know free-running timer value that was*
*               read using a call to timer_get_current() and the current value*
*               of the local timer. This is used to re-start a timer that was *
*               already active after a run-time call to timer_init() when     *
*               oscillator/clock freeequencies are changed on the fly...      *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void timer_restart( _TIMER_ *pUserTimer, _TIMER_ *pCurrentTime )
{
  ULONG ulRemainingTimeMs = 0; /* Must be initialized to zero... */

  /* Calculate the remaining time on the timer... */
  if ( (pCurrentTime->uwOverflowCnt) == (pUserTimer->uwOverflowCnt) )
  {
    /* See if there is any positive remaining time... */
    if ( (pUserTimer->ulTimer) > (pCurrentTime->ulTimer) )
    {
      ulRemainingTimeMs = pUserTimer->ulTimer - pCurrentTime->ulTimer;
    }
  }
  else if ( (pCurrentTime->uwOverflowCnt) < (pUserTimer->uwOverflowCnt) )
  {
    ulRemainingTimeMs = ((0xFFFF - pCurrentTime->ulTimer) + (pUserTimer->ulTimer));
  }

  /* Initialize the timer to the remaining time in msec (or zero if already expired)... */
  timer_start( pUserTimer, ulRemainingTimeMs );
}

/******************************************************************************
*                                                                             *
*  Function: void timer_intr(void)                                            *
*  Description: This function implements a free running 1msec timer. In the   *
*               PIC18F258 the timer interrupt is generated when Timer0        *
*               overflows...                                                  *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void timer_intr(void)
{
  /* Clear the pending timer interrupt... */
  TIMER0_CLEAR_PENDING_INT;

  /* Start the timer again for next time... */
  TMR0H = ucTimerReloadHigh; /* Write timer high value to buffer first... */
  TMR0L = ucTimerReloadLow; /* High and low take effect on write to low... */

  /* Increment the free running 1msec counter...let it overflow... */
  sFreeRunningTimer.ulTimer++;

  /* See if we have overflowed... */
  if ( sFreeRunningTimer.ulTimer == 0 ) sFreeRunningTimer.uwOverflowCnt++;
}

/******************************************************************************
*                       LOCAL FUNCTION DEFINITIONS                            *
******************************************************************************/

/******************************************************************************
*                                                                             *
*  Function: STDERROR init_timer_settings(ULONG ulFreq)                       *
*  Description: This function calculates the correct timer settings based on  *
*               the input clock frequency to generate a 1msec free-running    *
*               timer that is as accurate as possible...                      *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
static STDERROR init_timer_settings(ULONG ulFreq)
{
  STDERROR erReturnCode = ERR(TIMER,SUCCESS);
  UWORD    uwTimerReload; /* Calculated 16-bit timer reload value... */
  UBYTE    ucPreScalerBits;

  /* Initialize the free running counter variables... */
  sFreeRunningTimer.ulTimer = 0;
  sFreeRunningTimer.uwOverflowCnt = 0;

  /* Make sure that the clock source has been configured and if */
  /* not set it to the default value...                         */
  if ( ucFirstTime == TRUE )
  {
    timer_sel_clk_source( TIMER0_DEF_CLOCK_SOURCE );
  }

  /* Calculate timer configuration based on selected frequency... */
  if ( (ulFreq >= TIMER0_MIN_FREQ) && (ulFreq <= TIMER0_MAX_FREQ) )
  {
    /* Make pre-scaler bit selections for this frequency range... */
    ucPreScalerBits = 0x01;  /* Pre-scaler enabled, divide by 4 */

    /* Calculate the 16-bit timer reload value for 1msec timer period... */
    uwTimerReload = (ulFreq / 1000) / 4; /* Calculate counts/msec... */
    uwTimerReload = 0xFFFF - uwTimerReload; /* 16-bit counter counts UP to rollover... */

    /* Store the timer reload value... */
    ucTimerReloadHigh = ((UBYTE)(uwTimerReload >> 8));
    ucTimerReloadLow  = ((UBYTE)uwTimerReload);
  }
  else /* Frequency outside of supported range... */
  {
    /* Set safe values and flag an error, desired frequency out of supported range... */
    erReturnCode = ERR(TIMER,INVALID_PARAM);
    ucPreScalerBits   = 0x00;
    ucTimerReloadHigh = 0x00;
    ucTimerReloadLow  = 0x00;
  }

  /* Set the pre-scaler enable, pre-scaler select, and clock select */
  /* modes properly. We will always use the timer in 16-bit mode... */
  T0CON = ucPreScalerBits; /* Including PSA bit... */
  T0CONbits.T0CS = ucClockSourceSelect; /* Select the correct clock source... */

  /* Setup the timer to generate a 1msec interrupt. The timer interrupt */
  /* fires when the 16-bit timer overflows. Timer must be manually      */
  /* reloaded in the timer interrupt routine...                         */
  TMR0H = ucTimerReloadHigh; /* Write timer high value to buffer first... */
  TMR0L = ucTimerReloadLow; /* High and low regs latch to shadow on write to low reg... */

  return erReturnCode;
}
