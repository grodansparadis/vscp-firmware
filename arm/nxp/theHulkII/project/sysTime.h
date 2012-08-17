/******************************************************************************
 *
 * $RCSfile: sysTime.h,v $
 * $Revision: 1.2 $
 *
 * This module provides the interface definitions for sysTime.c
 * Copyright 2004, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *****************************************************************************/
#ifndef INC_SYS_TIME_H
#define INC_SYS_TIME_H

#include "types.h"
#include "lpc213x.h"
#include "config.h"

// Timer Interrupt Register Bit Definitions
#define TIR_MR0I    (1 << 0)            // Interrupt flag for match channel 0
#define TIR_MR1I    (1 << 1)            // Interrupt flag for match channel 1
#define TIR_MR2I    (1 << 2)            // Interrupt flag for match channel 2
#define TIR_MR3I    (1 << 3)            // Interrupt flag for match channel 3
#define TIR_CR0I    (1 << 4)            // Interrupt flag for capture channel 0 event
#define TIR_CR1I    (1 << 5)            // Interrupt flag for capture channel 1 event
#define TIR_CR2I    (1 << 6)            // Interrupt flag for capture channel 2 event
#define TIR_CR3I    (1 << 7)            // Interrupt flag for capture channel 3 event

// PWM Interrupt Register Bit Definitions
#define PWMIR_MR0I  (1 << 0)            // Interrupt flag for match channel 0
#define PWMIR_MR1I  (1 << 1)            // Interrupt flag for match channel 1
#define PWMIR_MR2I  (1 << 2)            // Interrupt flag for match channel 2
#define PWMIR_MR3I  (1 << 3)            // Interrupt flag for match channel 3
#define PWMIR_MR4I  (1 << 8)            // Interrupt flag for match channel 4
#define PWMIR_MR5I  (1 << 9)            // Interrupt flag for match channel 5
#define PWMIR_MR6I  (1 << 10)           // Interrupt flag for match channel 6
#define PWMIR_MASK  (0x070F)

// Timer Control Register Bit Definitions
#define TCR_ENABLE  (1 << 0)
#define TCR_RESET   (1 << 1)

// PWM Control Register Bit Definitions
#define PWMCR_ENABLE (1 << 0)
#define PWMCR_RESET (1 << 1)

// Timer Match Control Register Bit Definitions
#define TMCR_MR0_I  (1 << 0)            // Enable Interrupt when MR0 matches TC
#define TMCR_MR0_R  (1 << 1)            // Enable Reset of TC upon MR0 match
#define TMCR_MR0_S  (1 << 2)            // Enable Stop of TC upon MR0 match
#define TMCR_MR1_I  (1 << 3)            // Enable Interrupt when MR1 matches TC
#define TMCR_MR1_R  (1 << 4)            // Enable Reset of TC upon MR1 match
#define TMCR_MR1_S  (1 << 5)            // Enable Stop of TC upon MR1 match
#define TMCR_MR2_I  (1 << 6)            // Enable Interrupt when MR2 matches TC
#define TMCR_MR2_R  (1 << 7)            // Enable Reset of TC upon MR2 match
#define TMCR_MR2_S  (1 << 8)            // Enable Stop of TC upon MR2 match
#define TMCR_MR3_I  (1 << 9)            // Enable Interrupt when MR3 matches TC
#define TMCR_MR3_R  (1 << 10)           // Enable Reset of TC upon MR3 match
#define TMCR_MR3_S  (1 << 11)           // Enable Stop of TC upon MR3 match

// Timer Capture Control Register Bit Definitions
#define TCCR_CR0_R (1 << 0)            // Enable Rising edge on CAPn.0 will load TC to CR0
#define TCCR_CR0_F (1 << 1)            // Enable Falling edge on CAPn.0 will load TC to CR0
#define TCCR_CR0_I (1 << 2)            // Enable Interrupt on load of CR0
#define TCCR_CR1_R (1 << 3)            // Enable Rising edge on CAPn.1 will load TC to CR1
#define TCCR_CR1_F (1 << 4)            // Enable Falling edge on CAPn.1 will load TC to CR1
#define TCCR_CR1_I (1 << 5)            // Enable Interrupt on load of CR1
#define TCCR_CR2_R (1 << 6)            // Enable Rising edge on CAPn.2 will load TC to CR2
#define TCCR_CR2_F (1 << 7)            // Enable Falling edge on CAPn.2 will load TC to CR2
#define TCCR_CR2_I (1 << 8)            // Enable Interrupt on load of CR2
#define TCCR_CR3_R (1 << 9)            // Enable Rising edge on CAPn.3 will load TC to CR3
#define TCCR_CR3_F (1 << 10)           // Enable Falling edge on CAPn.3 will load TC to CR3
#define TCCR_CR3_I (1 << 11)           // Enable Interrupt on load of CR3

// Note: with a PCLK = CCLK/2 = 60MHz/2 and a Prescale divider of 3, we
// have a resolution of 100nSec.  Given the timer's counter register is
// 32-bits, we must make a call to one of the sysTime functions at least
// every ~430 sec.

// setup parameters
#define T0_PCLK_DIV     3
#define sysTICSperSEC   (PCLK / T0_PCLK_DIV)

// some helpful times for pause()
#define ONE_MS          (uint32_t)((  1e-3 * sysTICSperSEC) + .5)
#define TWO_MS          (uint32_t)((  2e-3 * sysTICSperSEC) + .5)
#define FIVE_MS         (uint32_t)((  5e-3 * sysTICSperSEC) + .5)
#define TEN_MS          (uint32_t)(( 10e-3 * sysTICSperSEC) + .5)
#define TWENTY_MS       (uint32_t)(( 20e-3 * sysTICSperSEC) + .5)
#define THIRTY_MS       (uint32_t)(( 30e-3 * sysTICSperSEC) + .5)
#define FIFTY_MS        (uint32_t)(( 50e-3 * sysTICSperSEC) + .5)
#define HUNDRED_MS      (uint32_t)((100e-3 * sysTICSperSEC) + .5)
#define ONE_FIFTY_MS    (uint32_t)((150e-3 * sysTICSperSEC) + .5)
#define QUARTER_SEC     (uint32_t)((250e-3 * sysTICSperSEC) + .5)
#define HALF_SEC        (uint32_t)((500e-3 * sysTICSperSEC) + .5)
#define ONE_SEC         (uint32_t)(( 1.0   * sysTICSperSEC) + .5)
#define TWO_SEC         (uint32_t)(( 2.0   * sysTICSperSEC) + .5)
#define FIVE_SEC        (uint32_t)(( 5.0   * sysTICSperSEC) + .5)
#define TEN_SEC         (uint32_t)((10.0   * sysTICSperSEC) + .5)


/******************************************************************************
 *
 * Function Name: initSysTime()
 *
 * Description:
 *    This function initializes the LPC's Timer 0 for use as the system
 *    timer.
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
void initSysTime(void);

/******************************************************************************
 *
 * Function Name: getSysTICs()
 *
 * Description:
 *    This function returns the current system time in TICs.
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    The current time in TICs
 *
 *****************************************************************************/
uint32_t getSysTICs(void);

/******************************************************************************
 *
 * Function Name: getElapsedSysTICs()
 *
 * Description:
 *    This function then returns the difference in TICs between the
 *    given starting time and the current system time.
 *
 * Calling Sequence: 
 *    The starting time.
 *
 * Returns:
 *    The time difference.
 *
 *****************************************************************************/
uint32_t getElapsedSysTICs(uint32_t startTime);

/******************************************************************************
 *
 * Function Name: pause()
 *
 * Description:
 *    This function does not return until the specified 'duration' in
 *    TICs has elapsed.
 *
 * Calling Sequence: 
 *    duration - length of time in TICs to wait before returning
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
void pause(uint32_t duration);

#endif
