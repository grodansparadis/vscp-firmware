/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2012-2014 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
*                                                                                              
*       This Information is Proprietary to Ackerman Computer Sciences, Sarasota,               
* Florida (ACS) and MAY NOT be copied by any method or incorporated into another               
* program without the express written consent of ACS. This Information or any                  
* portion thereof remains the property of ACS. The Information contained herein                
* is believed to be accurate and ACS assumes no responsibility or liability for                
* its use in any way and conveys no license or title under any patent or                       
* copyright and makes no representation or warranty that this Information is                   
* free from patent or copyright infringement.                                                  
*------------------------------------------------------------------------------                
* TimerRJ2.c - R8CM12A TimerRJ2 Function Implementation
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  11-28-12	First derived from Dennys\12Segment\Timers.c.
*******************************************************************************                
*/                                                                                             

#include "SerialIO.h"
#include "TimerRJ2.h"

#define	TIMERRJ2_RATE_HZ	50UL	/* 50Hz */

/* uncomment one of the following to select the Timer RJ2 clock source */
//#define TIMERRJ2_CLK_F1 			// main oscillator
#define TIMERRJ2_CLK_F8				// divide by 8
//#define TIMERRJ2_CLK_FHOCO		//
//#define TIMERRJ2_CLK_F2			// main oscillator / 2

/* calculate Timer RJ2 clock source and divisor value */
#ifdef	TIMERRJ2_CLK_F1

	#define	TIMERRJ2_CLK_SRC	0x00
	#define	TIMERRJ2_DIVISOR	((CPU_CLOCK) / TIMERRJ2_RATE_HZ)
	#if (TIMERRJ2_DIVISOR > 65536L)
		#error TIMERRJ2_DIVISOR overflow - try TIMERRJ2_CLK_F2 !
	#endif
#endif
	
#ifdef	TIMERRJ2_CLK_F2

	#define	TIMERRJ2_CLK_SRC	0x30
	#define	TIMERRJ2_DIVISOR	((CPU_CLOCK / 2) / TIMERRJ2_RATE_HZ)
	#if (TIMERRJ2_DIVISOR > 65536L)
		#error TIMERRJ2_DIVISOR overflow - try TIMERRJ2_CLK_F8 !
	#endif
#endif

#ifdef	TIMERRJ2_CLK_F8

	#define	TIMERRJ2_CLK_SRC	0x10
	#define	TIMERRJ2_DIVISOR	((CPU_CLOCK / 8) / TIMERRJ2_RATE_HZ)
	#if (TIMERRJ2_DIVISOR > 65536L)
		#error TIMERRJ2_DIVISOR overflow - examine CPU_CLOCK and TIMERRJ2_RATE_HZ values !
	#endif
#endif

#ifndef	TIMERRJ2_CLK_SRC
	#error TIMER RJ2 clock source undefined or not supported
#endif

/* TIMER RJ2 Interrupt Handler */
#pragma interrupt _timer_rj2(vect=22)
void _timer_rj2(void);
void _timer_rj2(void)
{
//	SCOPE_TRIGGER = 1;

	TimerRJ2_Process();

//	SCOPE_TRIGGER = 0;

	trjif_trjir = 0; // clear the interrupt flag
}

/* initialize Timer RJ2 */
void TimerRJ2_Init(void)
{
	DISABLE_INTERRUPTS();		// non-interruptus
	
	msttrj = 0;					// Timer RJ2 off standby
	
	trj = TIMERRJ2_DIVISOR;		// trj freq = TIMERRJ2_CLK_SRC / TIMERRJ2_DIVISOR
	
	trjioc = 0x00;				// controls output stuff, which we're not using here.
	
	trjmr = TIMERRJ2_CLK_SRC;	// select timer clock source
	
	trjisr = 0x00;				// controls output event stuff, which we're not using here
	
	ilvlb0 = 1;					// set TRJ interrupt to priority level 01
	ilvlb1 = 0;
	
	trjie_trjir = 1;			// enable the interrupt
	
	tstart_trjcr = 1;			// start the timer
	
	RESTORE_INTERRUPTS();		// restorus interruptus
}

