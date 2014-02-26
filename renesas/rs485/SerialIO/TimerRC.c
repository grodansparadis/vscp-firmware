/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2012-2013 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
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
* TimerRC.c - R8CM12A TimerRC Function Implementation
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  2-15-12	First derived from TimerRB2.c.
*******************************************************************************                
*/                                                                                             

#include "SerialIO.h"
#include "TimerRC.h"

// uncomment one of the following
#define	TIMERRC_RATE_HZ			1000UL	/* 1000Hz = 1000uSEC */

/* uncomment one of the following to select the Timer RB2 clock source */
//#define TIMERRC_CLK_F1 		// main oscillator
//#define TIMERRC_CLK_F2		// main oscillator / 2
#define TIMERRC_CLK_F4		// main oscillator / 4
//#define TIMERRC_CLK_F8			// main oscillator / 8
//#define TIMERRC_CLK_F32		// main oscillator / 32

/* calculate Timer RB2 clock source and divisor value */
#ifdef	TIMERRC_CLK_F1

	#define	TIMERRC_CLK_SRC	0x00
	#define	TIMERRC_DIVISOR	((CPU_CLOCK) / TIMERRC_RATE_HZ)
	#if (TIMERRC_DIVISOR > 65536L)
		#error TIMERRC_DIVISOR overflow - try TIMERRC_CLK_F2 !
	#endif
#endif
	
#ifdef	TIMERRC_CLK_F2

	#define	TIMERRC_CLK_SRC	0x10
	#define	TIMERRC_DIVISOR	((CPU_CLOCK / 2) / TIMERRC_RATE_HZ)
	#if (TIMERRC_DIVISOR > 65536L)
		#error TIMERRC_DIVISOR overflow - try TIMERRC_CLK_F4 !
	#endif
#endif

#ifdef	TIMERRC_CLK_F4

	#define	TIMERRC_CLK_SRC	0x20
	#define	TIMERRC_DIVISOR	((CPU_CLOCK / 4) / TIMERRC_RATE_HZ)
	#if (TIMERRC_DIVISOR > 65536L)
		#error TIMERRC_DIVISOR overflow - try TIMERRC_CLK_F8 !
	#endif
#endif

#ifdef	TIMERRC_CLK_F8

	#define	TIMERRC_CLK_SRC	0x30
	#define	TIMERRC_DIVISOR	((CPU_CLOCK / 8) / TIMERRC_RATE_HZ)
	#if (TIMERRC_DIVISOR > 65536L)
		#error TIMERRC_DIVISOR overflow - try TIMERRC_CLK_F32 ! !
	#endif
#endif

#ifdef	TIMERRC_CLK_F32

	#define	TIMERRC_CLK_SRC	0x40
	#define	TIMERRC_DIVISOR	((CPU_CLOCK / 32) / TIMERRC_RATE_HZ)
	#if (TIMERRC_DIVISOR > 65536L)
		#error TIMERRC_DIVISOR overflow - try TIMERRC_CLK_F64 !
	#endif
#endif

#ifndef	TIMERRC_CLK_SRC
	#error TIMER RC clock source undefined or not supported
#endif

// timer RC			(software int 7)
#pragma interrupt	_timer_rc(vect=7)
void _timer_rc(void);
void _timer_rc(void)
{
//	SCOPE_TRIGGER = 1;

	TimerRC_Process();

//	SCOPE_TRIGGER = 0;

	imfa_trcsr = 0; // clear the interrupt flag
}

/* innitialize Timer RC */
void TimerRC_Init(void)
{
	DISABLE_INTERRUPTS();		// non-interruptus
	
	msttrc = 0;					// Timer RC off standby
	
	trccnt = 0;
	trcgra = (USHORT)(TIMERRC_DIVISOR);
	trccr1 = 0x80 | TIMERRC_CLK_SRC;	// trccnt cleared by trcgra compare match, select timer clock source
	
	trcior0 = 0x88;				// IOB = output compare, output disabled, IOA = output compare, output disabled
	trcior1 = 0x00;				// IOD = output compare, output disabled, IOC = output compare, output disabled
	
	ilvl34 = 1;					// set TRC interrupt to priority level 01
	ilvl35 = 0;
	
	imiea_trcier = 1;			// enable the interrupt
	
	cts_trcmr = 1;				// start the timer
	
	RESTORE_INTERRUPTS();		// interruptus-maximus
}

