/******************************************************************************
 *
 * $RCSfile: sysTime.c,v $
 * $Revision: 1.4 $
 *
 * Ported for STR 2006 <akhe@eurosource.se>
 *
 * This module provides the interface routines for initializing and
 * accessing the system timing functions.
 * Copyright 2004, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *****************************************************************************/

//
// The TB2_IRQHandler handler in vectors.c  must be implemented fot this to work
// This rotine should update the sysTICs value once each millisecond.
// Something like this is needed
//
// 	sysTICs++;
//	TB_FlagClear( TB2 );
//

#include "sysTime.h"
#include "73x_lib.h"

volatile vu32 sysTICs;

/******************************************************************************
 *
 * Function Name: initTimebase2()
 *
 * Description:
 *    This function initializes the TB2 for use as the system timer.
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
void initTimebase2(void)
{
  TB_InitTypeDef    TB_InitStructure;

  sysTICs = 0;

  CFG_PeripheralClockConfig( CFG_CLK_TB2, ENABLE );

	// TB2 Configuration to get timbase IRQ each 1ms
  TB_InitStructure.TB_CLK_Source = TB_CLK_INTERNAL  ;
  TB_InitStructure.TB_Prescaler = 50;   					// The internal TB Frequency = 160KHz
  TB_InitStructure.TB_Preload = 160;    					// 160000Hz * 0.001 s  = 160

  TB_Init( TB2, &TB_InitStructure );
  TB_ITConfig( TB2, ENABLE );
	
	EIC_IRQChannelConfig( TB2_IRQChannel, ENABLE );	// Enable IRQ
	
	TB_Cmd( TB2, ENABLE );													// Enable the timer
}

/******************************************************************************
 *
 * Function Name: getSysTICs()
 *
 * Description:
 *    This function returns the current syetem time in TICs.
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    The current time in TICs as represented by sysTICs
 *
 *****************************************************************************/
u32 getSysTICs(void)
{
  return sysTICs;
}


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
u32 getElapsedSysTICs( u32 startTime )
{
  return ( getSysTICs() - startTime );
}


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
void pause( u32 duration )
{
  u32 startTime = getSysTICs();

  while ( getElapsedSysTICs( startTime ) < duration ) ;
	
}


