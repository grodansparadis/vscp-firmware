/******************************************************************************
 *
 * $RCSfile: sysTime.c,v $
 * $Revision: 1.4 $
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

#include "includes.h"
#include "75x_conf.h"
#include "75x_lib.h"
#include "sysTime.h"

volatile Int32U sysTICs;

/******************************************************************************
 *
 * Function Name: initTimebase()
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
void initTimebase( void )
{
  TB_InitTypeDef    TB_InitStructure;
  EIC_IRQInitTypeDef EIC_IRQInitStructure;

  sysTICs = 0;

  MRCC_PeripheralClockConfig( MRCC_Peripheral_TB, ENABLE );

	// TB2 Configuration to get timbase IRQ each 1ms
  TB_InitStructure.TB_ClockSource = TB_ClockSource_CKTIM;
  TB_InitStructure.TB_Prescaler = 100;             // The internal TB Frequency = 160KHz
  TB_InitStructure.TB_AutoReload = 160;           // 160000Hz * 0.001 s  = 160

  TB_Init( &TB_InitStructure );
  TB_ITConfig( TB_IT_Update, ENABLE );
	
  //EIC_IRQChannelConfig( TB_IRQChannel, ENABLE );  // Enable IRQ
  
  // Enable and configure the priority of the TIM0 Output compare IRQ Channel
  EIC_IRQInitStructure.EIC_IRQChannel = TB_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit( &EIC_IRQInitStructure );
	
  TB_Cmd( ENABLE );													// Enable the timer
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


