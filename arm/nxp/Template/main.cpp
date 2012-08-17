/******************************************************************************\
 main.cpp
\******************************************************************************/

/******************************************************************************\
 Include files
\******************************************************************************/
#include "LPC210x.h"

/******************************************************************************\
 Initialize timer 1
\******************************************************************************/

typedef void (*IRQ)();
//IRQ *VICVectAddr = (IRQ *)&VICVectAddr0;			// non-wrapped IRQ vectors
#define VICVectCntl   (((volatile unsigned long *) 0xFFFFF200))		// array

extern IRQ &irqVector0;								// wrapped IRQ vector

extern "C" void __enable_interrupts();
extern "C" void __disable_interrupts();
extern "C" void IRQHandler();

/******************************************************************************\
 MyTimerInterrupt
\******************************************************************************/
extern "C" void MyTimerInterrupt()
{
	// toggle LED
	static int led = 1;
	led = !led;
	if (led) IOSET = 1<<10; else IOCLR = 1<<10;		// blink

	// prepare for next interrupt
	T1IR = 0xFF;					// clear interrupt flags
}

/******************************************************************************\
 Initialize timer 1
 (interrupt source 5)
\******************************************************************************/
void InitTimer()
{
	__disable_interrupts();

	// Initialise timer1
	T1TCR = 0;						// stop and reset counter and prescaler
	//T1TC  = 0;					// reset timer counter (again)
	T1PC  = 2/*div by 2*/ - 1;		// prescale counter
	//T1PR  = 0;					// reset timer prescaler value (again)
	//PLLCFG;						// default divider is 1. multiplier is 1.
	//VPBDIV = 0;					// VPB bus runs at 1/4 of cpu clk (default)
	T1MR0 = 6000000 / 4/*VPBDIV*/;	// max counter value
	T1MCR = 1<<1 | 1<<0;			// reset and interrupt on match
	T1IR = 0xFF;					// clear interrupt flags

	// Initialise VIC
	VICIntSelect &=~ (1<<5);		// select IRQ mode instead of FIQ
	VICVectCntl[0] = 0x20/*enable*/ | 5/*timer 1*/;
	irqVector0 = MyTimerInterrupt;	// wrapped interrupt vector
	VICIntEnable = 1<<5;			// enable interrupt source

	T1TCR = 1<<0;					// start

	VICVectAddr = 0xFF;				// update priority hardware

	 __enable_interrupts();
}

/******************************************************************************\
 main
\******************************************************************************/
extern "C" int main()
{
	/*
	 * Init GPIO
	 */
	PINSEL0 &=~ 3<<20;		// P0.10 = GPIO
	IODIR |= 1<<10;			// P0.10 = output
	IOCLR = 1<<10;			// on (inverted LED)
	IOSET = 1<<10;			// off (inverted LED)

	/*
	 * Init timer
	 */
	InitTimer();

	/*
	 * Main loop
	 */
	while (1)
	{
		// example of some inline assembly...
		asm
		(
			"ldr	r0, =0xFFFFF000 \n"		// VICIRQStatus
			"ldr  r1,[r0] \n"
	
			"ldr	r0, =0xFFFFF200 \n"		// VICVectCntl0
			"ldr  r2,[r0] \n"
			
			"ldr	r0, =0xFFFFF008 \n"		// VICRawIntr
			"ldr  r3,[r0] \n"
	
			"ldr	r0, =0xE0008000\n"		// T1IR
			"ldr  r4,[r0] \n"
	
			"ldr	r0, =0xE0008008\n"		// T1TC
			"ldr  r5,[r0] \n"
	
			"ldr	r0, =0xE0008018\n"		// T1MR0
			"ldr  r6,[r0] \n"
		);
	}

	return 0;								// don't ever come near this
}

