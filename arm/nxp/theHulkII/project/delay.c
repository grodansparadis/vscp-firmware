/* 
   Precise Delay Functions for LPC2100
   Martin Thomas, 11/2004
*/

#include "delay.h"
#include "sysTime.h"
#include "config.h"

void delay_us_(uint32_t delayval)
{
	asm volatile (
		"L_LOOPUS_%=: 		\n\t" \
		"subs	%0, %0, #1 	\n\t" \
		"bne	L_LOOPUS_%=	\n\t" \
		:  /* no outputs */ : "r" (delayval)
	);
	
#if 0
	asm volatile (
		"ldr	r1,=%0 		\n\t" \
		".L_LOOPUS_%=: 		\n\t" \
		"subs	r1, r1, #1 	\n\t" \
		"bne	L_LOOPUS_%=	\n\t" \
		: "=w" (delayval)
		: "0" (delayval)
	);
#endif
}

void delay_ms(uint32_t delayval)
{
	uint32_t d = (delayval * 1e-3 * sysTICSperSEC) + .5;
	pause( d ); 
}
