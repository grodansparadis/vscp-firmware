/******************************************************************************
 *
 * $RCSfile: sysTime.h,v $
 * $Revision: 1.2 $
 *
 * This module provides the interface definitions for sysTime.c
 * Copyright 2004, R O SoftWare
 *
 * Ported for STR 2006 <akhe@eurosource.se>
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *****************************************************************************/
#ifndef INC_SYS_TIME_H
#define INC_SYS_TIME_H


// setup parameters
#define sysTICSperSEC   1000

// some helpful times for pause()
#define ONE_MS          (u32)((  1e-3 * sysTICSperSEC) + .5)
#define TWO_MS          (u32)((  2e-3 * sysTICSperSEC) + .5)
#define FIVE_MS         (u32)((  5e-3 * sysTICSperSEC) + .5)
#define TEN_MS          (u32)(( 10e-3 * sysTICSperSEC) + .5)
#define TWENTY_MS       (u32)(( 20e-3 * sysTICSperSEC) + .5)
#define THIRTY_MS       (u32)(( 30e-3 * sysTICSperSEC) + .5)
#define FIFTY_MS        (u32)(( 50e-3 * sysTICSperSEC) + .5)
#define HUNDRED_MS      (u32)((100e-3 * sysTICSperSEC) + .5)
#define ONE_FIFTY_MS    (u32)((150e-3 * sysTICSperSEC) + .5)
#define QUARTER_SEC     (u32)((250e-3 * sysTICSperSEC) + .5)
#define HALF_SEC        (u32)((500e-3 * sysTICSperSEC) + .5)
#define ONE_SEC         (u32)(( 1.0   * sysTICSperSEC) + .5)
#define TWO_SEC         (u32)(( 2.0   * sysTICSperSEC) + .5)
#define FIVE_SEC        (u32)(( 5.0   * sysTICSperSEC) + .5)
#define TEN_SEC         (u32)((10.0   * sysTICSperSEC) + .5)


/******************************************************************************
 *
 * Function Name: initTimebase2()
 *
 * Description:
 *    Init. TB2 for 1ms interrupts.
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
void initTimebase2( void );

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
unsigned long getSysTICs( void );

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
unsigned long getElapsedSysTICs( unsigned long startTime);

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
void pause( unsigned long duration);

#endif
