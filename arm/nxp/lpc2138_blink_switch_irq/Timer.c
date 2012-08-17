/******************************************************************************/
/*  TIMER.C: Time Functions for 10Hz Clock Tick using Timer0                  */
/******************************************************************************/
/*  Inspired by a sample application from Keil Elektronik                     */
/*  A lot of information has been found in a sample from R O Software.        */
/******************************************************************************/
/*  Sample for WinARM by M.Thomas <eversmith@heizung-thomas.de>               */
/******************************************************************************/

#include "lpc213x.h"
#include "config.h"

#include "Timer.h"

#define VICVectCntl0_ENABLE 	(1<<5)

#define VIC_Channel_Timer0  	4

#define TxTCR_COUNTER_ENABLE 	(1<<0)
#define TxTCR_COUNTER_RESET  	(1<<1)
#define TxMCR_INT_ON_MR0     	(1<<0)
#define TxMCR_RESET_ON_MR0   	(1<<1)
#define TxIR_MR0_FLAG        	(1<<0)

volatile unsigned long timeval;

void __attribute__ ((interrupt("IRQ"))) tc0_cmp(void) ;

/* Timer0 Compare-Match Interrupt Handler (ISR) */
void tc0_cmp(void) 
{
	timeval++;
	T0IR = TxIR_MR0_FLAG; 	// Clear interrupt flag by writing 1 to Bit 0
	VICVectAddr = 0;       	// Acknowledge Interrupt (rough?)
}

/* Setup Timer0 Compare-Match Interrupt         */
/* no prescaler timer runs at cclk = FOSC*PLL_M */
void init_timer (void) {
	
	T0MR0 = ((FOSC*PLL_M)/(1000/10))-1;     		// Compare-hit at 10mSec (-1 reset "tick")
	T0MCR = TxMCR_INT_ON_MR0 | TxMCR_RESET_ON_MR0; 	// Interrupt and Reset on MR0
	T0TCR = TxTCR_COUNTER_ENABLE;            		// Timer0 Enable

	VICVectAddr0 = (unsigned long)tc0_cmp;   // set interrupt vector in 0
	VICVectCntl0 = VICVectCntl0_ENABLE | VIC_Channel_Timer0; // use it for Timer 0 Interrupt:
	VICIntEnable = (1<<VIC_Channel_Timer0);    // Enable Timer0 Interrupt
}

