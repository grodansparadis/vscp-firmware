/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2012 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
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
* TimerRB2.c - R8CM12A TimerRB2 Function Implementation
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  2-16-12	First written.
*******************************************************************************                
*/                                                                                             

#include "SerialIO.h"
#include "TimerRB2.h"

// uncomment one of the following
#define	TIMERRB2_RATE_HZ		10000UL	/* 100uSEC = 10KHz */

/* uncomment one of the following to select the Timer RB2 clock source */
//#define TIMERRB2_CLK_F1 		// main oscillator
//#define TIMERRB2_CLK_F2		// main oscillator / 2
#define TIMERRB2_CLK_F4		// main oscillator / 4
//#define TIMERRB2_CLK_F8			// main oscillator / 8
//#define TIMERRB2_CLK_F32		// main oscillator / 32
//#define TIMERRB2_CLK_F64		// main oscillator / 64
//#define TIMERRB2_CLK_F128		// main oscillator / 128
//#define TIMERRB2_CLK_FRJ2UF	// timer rj2 underflow

/* calculate Timer RB2 clock source and divisor value */
#ifdef	TIMERRB2_CLK_F1

	#define	TIMERRB2_CLK_SRC	0x00
	#define	TIMERRB2_DIVISOR	((CPU_CLOCK) / TIMERRB2_RATE_HZ)
	#if (TIMERRB2_DIVISOR > 65536L)
		#error TIMERRB2_DIVISOR overflow - try TIMERRB2_CLK_F2 !
	#endif
#endif
	
#ifdef	TIMERRB2_CLK_F2

	#define	TIMERRB2_CLK_SRC	0x30
	#define	TIMERRB2_DIVISOR	((CPU_CLOCK / 2) / TIMERRB2_RATE_HZ)
	#if (TIMERRB2_DIVISOR > 65536L)
		#error TIMERRB2_DIVISOR overflow - try TIMERRB2_CLK_F4 !
	#endif
#endif

#ifdef	TIMERRB2_CLK_F4

	#define	TIMERRB2_CLK_SRC	0x40
	#define	TIMERRB2_DIVISOR	((CPU_CLOCK / 4) / TIMERRB2_RATE_HZ)
	#if (TIMERRB2_DIVISOR > 65536L)
		#error TIMERRB2_DIVISOR overflow - try TIMERRB2_CLK_F8 !
	#endif
#endif

#ifdef	TIMERRB2_CLK_F8

	#define	TIMERRB2_CLK_SRC	0x10
	#define	TIMERRB2_DIVISOR	((CPU_CLOCK / 8) / TIMERRB2_RATE_HZ)
	#if (TIMERRB2_DIVISOR > 65536L)
		#error TIMERRB2_DIVISOR overflow - try TIMERRB2_CLK_F32 ! !
	#endif
#endif

#ifdef	TIMERRB2_CLK_F32

	#define	TIMERRB2_CLK_SRC	0x50
	#define	TIMERRB2_DIVISOR	((CPU_CLOCK / 32) / TIMERRB2_RATE_HZ)
	#if (TIMERRB2_DIVISOR > 65536L)
		#error TIMERRB2_DIVISOR overflow - try TIMERRB2_CLK_F64 !
	#endif
#endif

#ifdef	TIMERRB2_CLK_F64

	#define	TIMERRB2_CLK_SRC	0x60
	#define	TIMERRB2_DIVISOR	((CPU_CLOCK / 64) / TIMERRB2_RATE_HZ)
	#if (TIMERRB2_DIVISOR > 65536L)
		#error TIMERRB2_DIVISOR overflow - try TIMERRB2_CLK_F128 ! !
	#endif
#endif

#ifdef	TIMERRB2_CLK_F128

	#define	TIMERRB2_CLK_SRC	0x70
	#define	TIMERRB2_DIVISOR	((CPU_CLOCK / 128) / TIMERRB2_RATE_HZ)
	#if (TIMERRB2_DIVISOR > 65536L)
		#error TIMERRB2_DIVISOR overflow - try TIMERRB2_CLK_FRU2UF ! !
	#endif
#endif

#ifdef	TIMERRB2_CLK_FRJ2UF
	#error TIMER RJ2 must be configured first
#endif

#ifndef	TIMERRB2_CLK_SRC
	#error TIMER RB2 clock source undefined or not supported
#endif

// timer RB2		(software int 24)
#pragma interrupt	_timer_rb2(vect=24)
void _timer_rb2(void);
void _timer_rb2(void)
{
//	SCOPE_TRIGGER = 1;

//	TimerRB2_Process();

//	SCOPE_TRIGGER = 0;

	trbif_trbir = 0; // clear the interrupt flag
}

/* innitialize Timer RB2 */
void TimerRB2_Init(void)
{
	DISABLE_INTERRUPTS();		// non-interruptus
	
	msttrb = 0;					// Timer RB2 off standby
	
	trbmr = TIMERRB2_CLK_SRC;	// select timer clock source
	
	tcnt16_trbmr = 1;
	
	trbpr = (BYTE)(TIMERRB2_DIVISOR >> 8);		// trb freq = TIMERRB2_CLK_SRC / TIMERRB2_DIVISOR
	trbpre = (BYTE)(TIMERRB2_DIVISOR & 0xff);	// 
	
	trbioc = 0x00;				// controls output stuff, which we're not using here.
	
	ilvlc0 = 1;					// set TRB interrupt to priority level 01
	ilvlc1 = 0;
	
//	trbie_trbir = 1;			// enable the interrupt
	
//	tstart_trbcr = 1;			// start the timer
	
	RESTORE_INTERRUPTS();		// interruptus-maximus
}

void TimerRB2_OneShotWait(void)
{
	trbif_trbir = 0;
	tstart_trbcr = 1;		// start the timer
	while (trbif_trbir == 0)
		;
	tstart_trbcr = 0;		// stop the timer
}

void TimerRB2_Start(void)
{
	trbif_trbir = 0;
	tstart_trbcr = 1;		// start the timer
}

BOOLEAN TimerRB2_IsExpired(void)
{
	return (trbif_trbir == 1);
}

void TimerRB2_Stop(void)
{
	tstart_trbcr = 0;		// stop the timer
}



