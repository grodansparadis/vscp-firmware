/******************************************************************************
*                                                                             *
*  File:                       INTVECS.C                                      *
*                                                                             *
*  Description: This file contains the main entry point for the <PIC18F4680>  *
*               low and high priority interrupt dispatch routines...          *
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
*  Created By:  Thomas R. Shelburne 11/05/04                                  *
*                                                                             *
*  Versions:                                                                  *
*                                                                             *
*  MAJ.MIN - AUTHOR - CHANGE DESCRIPTION                                      *
*                                                                             *
*   1.0    - T.R.S. - Initial release of <PIC18F4680> interrupt handlers...   *
*                                                                             *
******************************************************************************/

#include <p18cxxx.h>
#include "define.h"
#include "dwcan.h"
#include "system.h"
#include "timer.h"

/******************************************************************************
*                                                                             *
*  Function: void InterruptHandlerHigh(void)                                  *
*  Description: This is the HIGH priority interrupt dispatch routine which    *
*               determines what enabled interrupt source triggered the event. *
*               The ".tmpdata" section is saved so that if you make function  *
*               calls from the interrupt routine you don't clobber the        *
*               .tmpdata which already holds data for the main application    *
*               loop. TBLPTR, TABLAT, and PROD are commonly used by the       *
*               compiler and should also be saved upon entry to interrupts    *
*               service routines. ECANCON must be saved to make sure that the *
*               EWIN bits get restored after the interrupt exits so that the  *
*               same CAN registers get mapped into access RAM as were mapped  *
*               before the interrupt occurred.                                *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
#pragma code
//#pragma interruptlow InterruptHandlerHigh save = INTSAVELOCS, section(".tmpdata"), ECANCON
#pragma interruptlow InterruptHandlerHigh save = ECANCON
void InterruptHandlerHigh(void)
{
#ifdef _PROFILE_
    LED_5_HIGH;
#endif

#if TIMER0_INT_PRIORITY==TIMER_PRIORITY_HIGH
#ifdef _BUILD_MESSAGES_
#message TIMER0 interrupt is high priority...
#endif
  /* Did HIGH priority Timer0 overflow interrupt occur... */
  if( (INTCONbits.TMR0IF == 1) && (INTCONbits.TMR0IE == 1) )
  {
#ifdef _PROFILE_
    LED_0_HIGH;
#endif
    /* Call the free-running 1 msec timer 0 interrupt... */
    timer_intr();
#ifdef _PROFILE_
    LED_0_LOW;
#endif
  }
#endif

#if DWCAN_RX_PRIORITY==DWCAN_PRIORITY_HIGH
#ifdef _BUILD_MESSAGES_
#message DWCAN RX interrupt is high priority...
#endif
  /* Check for HIGH priority CAN RX interrupts... */
  if ( (PIR3bits.RXBnIF == 1) && (PIE3bits.RXBnIE == 1) )
  {
#ifdef _PROFILE_
    LED_1_HIGH;
#endif
    dwcan_rx_int();
#ifdef _PROFILE_
    LED_1_LOW;
#endif
  }
#endif

#if DWCAN_TX_PRIORITY==DWCAN_PRIORITY_HIGH
#ifdef _BUILD_MESSAGES_
#message DWCAN TX interrupt is high priority...
#endif
  /* Check for unmasked HIGH priority CAN TX interrupts... */
  if ( (PIR3bits.TXBnIF == 1) && (PIE3bits.TXBnIE == 1) )
  {
#ifdef _PROFILE_
    LED_2_HIGH;
#endif
    /* Call the CAN TX handler... */
    dwcan_tx_int();
#ifdef _PROFILE_
    LED_2_LOW;
#endif
  }
#endif

#if DWCAN_ERROR_PRIORITY==DWCAN_PRIORITY_HIGH
#ifdef _BUILD_MESSAGES_
#message DWCAN ERROR interrupt is high priority...
#endif
  /* Check for HIGH priority CAN ERROR interrupt... */
  if ( ((PIR3bits.ERRIF == 1) && (PIE3bits.ERRIE == 1)) ||
       ((PIR3bits.IRXIF == 1) && (PIE3bits.IRXIE == 1)) )
  {
#ifdef _PROFILE_
    LED_3_HIGH;
#endif
    /* Call the CAN error handler... */
    dwcan_error_int();
#ifdef _PROFILE_
    LED_3_LOW;
#endif
  }
#endif

#if DWCAN_WAKEUP_PRIORITY==DWCAN_PRIORITY_HIGH
#ifdef _BUILD_MESSAGES_
#message DWCAN wakeup interrupt is high priority...
#endif
  /* Check for HIGH priority CAN wakeup interrupt... */
  if ( (PIR3bits.WAKIF == 1) && (PIE3bits.WAKIE == 1) )
  {
#ifdef _PROFILE_
    LED_4_HIGH;
#endif
    /* Call the CAN wake-up interrupt... */
    dwcan_wake_int();
#ifdef _PROFILE_
    LED_4_LOW;
#endif
  }
#endif

#ifdef _PROFILE_
    LED_5_LOW;
#endif
}

/******************************************************************************
*                                                                             *
*  Function: void InterruptVectorHigh(void)                                   *
*  Description: This vector creates the branch instruction which will vector  *
*               to the HIGH priority interrupt handler routine.               *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh(void)
{
  _asm
    goto InterruptHandlerHigh
  _endasm
}

/******************************************************************************
*                                                                             *
*  Function: void InterruptHandlerLow(void)                                   *
*  Description: This is the LOW priority interrupt dispatch routine which     *
*               determines what enabled interrupt source triggered the event. *
*               The ".tmpdata" section is saved so that if you make function  *
*               calls from the interrupt routine you don't clobber the        *
*               .tmpdata which already holds data for the main application    *
*               loop. TBLPTR, TABLAT, and PROD are commonly used by the       *
*               compiler and should also be saved upon entry to interrupts    *
*               service routines. ECANCON must be saved to make sure that the *
*               EWIN bits get restored after the interrupt exits so that the  *
*               same CAN registers get mapped into access RAM as were mapped  *
*               before the interrupt occurred.                                *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
#pragma code
//#pragma interruptlow InterruptHandlerLow save = INTSAVELOCS, section(".tmpdata"), ECANCON
#pragma interruptlow InterruptHandlerLow save = ECANCON
void InterruptHandlerLow(void)
{
#ifdef _PROFILE_
    LED_6_HIGH;
#endif

#if TIMER0_INT_PRIORITY==TIMER_PRIORITY_LOW
#ifdef _BUILD_MESSAGES_
#message TIMER0 interrupt is low priority...
#endif
  /* Did LOW priority Timer0 overflow interrupt occur... */
  if( (INTCONbits.TMR0IF == 1) && (INTCONbits.TMR0IE == 1) )
  {
#ifdef _PROFILE_
    LED_0_HIGH;
#endif
    /* Call the free-running 1 msec timer 0 interrupt... */
    timer_intr();
#ifdef _PROFILE_
    LED_0_LOW;
#endif
  }
#endif

#if DWCAN_RX_PRIORITY==DWCAN_PRIORITY_LOW
#ifdef _BUILD_MESSAGES_
#message DWCAN RX interrupt is low priority...
#endif
  /* Check for LOW priority CAN RX interrupts... */
  if ( (PIR3bits.RXBnIF == 1) && (PIE3bits.RXBnIE == 1) )
  {
#ifdef _PROFILE_
    LED_1_HIGH;
#endif
    dwcan_rx_int();
#ifdef _PROFILE_
    LED_1_LOW;
#endif
  }
#endif

#if DWCAN_TX_PRIORITY==DWCAN_PRIORITY_LOW
#ifdef _BUILD_MESSAGES_
#message DWCAN TX interrupt is low priority...
#endif
  /* Check for unmasked LOW priority CAN TX interrupts... */
  if ( (PIR3bits.TXBnIF == 1) && (PIE3bits.TXBnIE == 1) )
  {
#ifdef _PROFILE_
    LED_2_HIGH;
#endif
    /* Call the CAN TX handler... */
    dwcan_tx_int();
#ifdef _PROFILE_
    LED_2_LOW;
#endif
  }
#endif

#if DWCAN_ERROR_PRIORITY==DWCAN_PRIORITY_LOW
#ifdef _BUILD_MESSAGES_
#message DWCAN ERROR interrupt is low priority...
#endif
  /* Check for LOW priority CAN ERROR interrupt... */
  if ( ((PIR3bits.ERRIF == 1) && (PIE3bits.ERRIE == 1)) ||
       ((PIR3bits.IRXIF == 1) && (PIE3bits.IRXIE == 1)) )
  {
#ifdef _PROFILE_
    LED_3_HIGH;
#endif
    /* Call the CAN error handler... */
    dwcan_error_int();
#ifdef _PROFILE_
    LED_3_LOW;
#endif
  }
#endif

#if DWCAN_WAKEUP_PRIORITY==DWCAN_PRIORITY_LOW
#ifdef _BUILD_MESSAGES_
#message DWCAN wakeup interrupt is low priority...
#endif
  /* Check for LOW priority CAN wakeup interrupt... */
  if ( (PIR3bits.WAKIF == 1) && (PIE3bits.WAKIE == 1) )
  {
#ifdef _PROFILE_
    LED_4_HIGH;
#endif
    /* Call the CAN wake-up interrupt... */
    dwcan_wake_int();
#ifdef _PROFILE_
    LED_4_LOW;
#endif
  }
#endif

#ifdef _PROFILE_
    LED_6_LOW;
#endif
}

/******************************************************************************
*                                                                             *
*  Function: void InterruptVectorLow(void)                                    *
*  Description: This vector creates the branch instruction which will vector  *
*               to the LOW priority interrupt handler routine.                *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
#pragma code InterruptVectorLow = 0x18
void InterruptVectorLow(void)
{
  _asm
    goto  InterruptHandlerLow
  _endasm
}
#pragma code
