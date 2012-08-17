/*! \file timer.c \brief Timer Support Library for LPC2100. */
//*****************************************************************************
//
// File Name	: 'timer.c'
// Title		: Timer Support Library for LPC2100
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.05.05
// Revised		: 2004.07.12
// Version		: 0.1
// Target MCU	: ARM processors
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "lpc210x.h"
#include "processor.h"

#include "global.h"
#include "timer.h"
#include "rprintf.h"

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntrFunc[TIMER_NUM_INTERRUPTS];

volatile u32 Timer0Pause;
volatile u32 Timer0OverflowCount;
volatile u32 Timer1OverflowCount;

void delay(unsigned long d)
{     
	for(; d; --d)
	{
		asm volatile ("nop");
	}
}


void timerInit(void)
{
	u08 intNum;
	// detach all user functions from interrupts
	for(intNum=0; intNum<TIMER_NUM_INTERRUPTS; intNum++)
		timerDetach(intNum);

	// initialize timer0
	timer0Init();
	// initialize timer1
	timer1Init();
	// enable interrupts
	enableIRQ();
}

void timer0Init(void)
{
	// setup timer0
	// set prescaler
	timer1PrescalerSet(1);
	// reset timer
	T0TCR = TCR_RESET;
	delay(10);
	// start timer
	T0TCR = TCR_ENABLE;

	// setup timer0 for IRQ
	// set interrupt as IRQ
	VICIntSelect &= ~(1<<VIC_TIMER0);
	// assign VIC slot
	VICVectCntl4 = VIC_ENABLE | VIC_TIMER0;
	VICVectAddr4 = (u32)timer0Service;
	// enable interrupt
	VICIntEnable |= (1<<VIC_TIMER0);

	// setup MR0 value
	T0MR0 = CCLK/1000;
	// enable timer0 interrupt and reset on MR0 match
	T0MCR |= TMCR_MR0_I | TMCR_MR0_R;

}

void timer1Init(void)
{
	// setup timer1
	// set prescaler
	timer1PrescalerSet(1);
	// reset timer
	T1TCR = TCR_RESET;
	delay(10);
	// start timer
	T1TCR = TCR_ENABLE;

	// setup timer1 for IRQ
	// set interrupt as IRQ
	VICIntSelect &= ~(1<<VIC_TIMER1);
	// assign VIC slot
	VICVectCntl5 = VIC_ENABLE | VIC_TIMER1;
	VICVectAddr5 = (u32)timer1Service;
	// enable interrupt
	VICIntEnable |= (1<<VIC_TIMER1);
}

void timer0PrescalerSet(u32 clockDiv)
{
	// timer0 increments every PR+1 cycles
	// subtract 1 so the argument is a true division ratio
	T0PR = clockDiv-1;
}
void timer1PrescalerSet(u32 clockDiv)
{
	// timer1 increments every PR+1 cycles
	// subtract 1 so the argument is a true division ratio
	T1PR = clockDiv-1;
}

void timerAttach(u08 interruptNum, void (*userFunc)(void) )
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		TimerIntrFunc[interruptNum] = userFunc;
	}
}

void timerDetach(u08 interruptNum)
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run nothing
		TimerIntrFunc[interruptNum] = 0;
	}
}

void timerPause(unsigned long pause_ms)
{
	Timer0Pause = pause_ms;

	while(Timer0Pause);
}

void timer0ClearOverflowCount(void)
{
	Timer0OverflowCount = 0;
}

u32 timer0GetOverflowCount(void)
{
	return Timer0OverflowCount;
}

void timer1ClearOverflowCount(void)
{
	Timer1OverflowCount = 0;
}

u32 timer1GetOverflowCount(void)
{
	return Timer1OverflowCount;
}

void timer0Match0Set(u32 value)
{
	T0MR0 = value;
}

void timer0Match1Set(u32 value)
{
	T0MR1 = value;
}

void timer1Match0Set(u32 value)
{
	T1MR0 = value;
}

void timer1Match1Set(u32 value)
{
	T1MR1 = value;
}


void timer0Capture0Init(int on)
{
	// setup timer0 capture0
	if(on)
	{
		// pin select
		PINSEL0 &= ~(3<<4);	// clear pin select bits for P0.2 -> GPIO
		PINSEL0 |=  (2<<4);	// set pin select bits for P0.2 -> Capture0.0
		// enable timer0 interrupt on rising edge of CR0 capture
		T0CCR |= TCCR_CR0_I | TCCR_CR0_R;
	}
	else
	{
		// pin select
		PINSEL0 &= ~(3<<4);	// clear pin select bits for P0.2 -> GPIO
		// disable timer0 interrupt on rising edge of CR0 capture
		T0CCR &= ~(TCCR_CR0_I | TCCR_CR0_R);
	}
}

void timer0Capture1Init(int on)
{
	// setup timer0 capture1
	if(on)
	{
		// pin select
		PINSEL0 &= ~(3<<8);	// clear pin select bits for P0.4 -> GPIO
		PINSEL0 |=  (2<<8);	// set pin select bits for P0.4 -> Capture0.1
		// enable timer0 interrupt on rising edge of CR1 capture
		T0CCR |= TCCR_CR1_I | TCCR_CR1_R;
	}
	else
	{
		// pin select
		PINSEL0 &= ~(3<<8);	// clear pin select bits for P0.4 -> GPIO
		// disable timer0 interrupt on rising edge of CR1 capture
		T0CCR &= ~(TCCR_CR1_I | TCCR_CR1_R);
	}
}

void timer0Service(void)
{
	ISR_ENTRY();
	
	// check the interrupt sources
	if(T0IR & TIR_MR0I)
	{
		// clear MR0 Interrupt
		T0IR |= TIR_MR0I;
		if(Timer0Pause)
			Timer0Pause--;
		Timer0OverflowCount++;
		// if a user function is defined, execute it
		if(TimerIntrFunc[TIMER0MATCH0_INT])
			TimerIntrFunc[TIMER0MATCH0_INT]();
	}
	else if(T0IR & TIR_CR0I)
	{
		// clear CR0 Interrupt
		T0IR |= TIR_CR0I;
		// if a user function is defined, execute it
		if(TimerIntrFunc[TIMER0CAPTURE0_INT])
			TimerIntrFunc[TIMER0CAPTURE0_INT]();
	}
	else if(T0IR & TIR_CR1I)
	{
		// clear CR1 Interrupt
		T0IR |= TIR_CR1I;
		// if a user function is defined, execute it
		if(TimerIntrFunc[TIMER0CAPTURE1_INT])
			TimerIntrFunc[TIMER0CAPTURE1_INT]();
	}
	
	VICSoftIntClear = (1<<VIC_TIMER0);
	VICVectAddr = 0x00000000;             // clear this interrupt from the VIC
	ISR_EXIT();                           // recover registers and return
}

void timer1Service(void)
{
	ISR_ENTRY();

	if(T0IR & TIR_MR0I)
	{
		// clear MR0 Interrupt
		T1IR |= TIR_MR0I;
		Timer1OverflowCount++;
		// if a user function is defined, execute it
		if(TimerIntrFunc[TIMER1MATCH0_INT])
			TimerIntrFunc[TIMER1MATCH0_INT]();
	}
	
	VICSoftIntClear = (1<<VIC_TIMER1);
	VICVectAddr = 0x00000000;             // clear this interrupt from the VIC
	ISR_EXIT();                           // recover registers and return
}
