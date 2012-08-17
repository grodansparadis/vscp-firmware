/*! \file timer128.c \brief System Timer function library for Mega128. */
//*****************************************************************************
//
// File Name	: 'timer128.c'
// Title		: System Timer function library for Mega128
// Author		: Pascal Stang - Copyright (C) 2000-2003
// Created		: 11/22/2000
// Revised		: 02/24/2003
// Version		: 1.2
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef WIN32
	#include <avr/io.h>
	#include <avr/signal.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
#endif

#include "global.h"
#include "timer128.h"

// Program ROM constants
// the prescale division values stored in 2^n format
// STOP, CLK, CLK/8, CLK/64, CLK/256, CLK/1024
short __attribute__ ((progmem)) TimerPrescaleFactor[] = {0,0,3,6,8,10};
// the prescale division values stored in 2^n format
// STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024
short __attribute__ ((progmem)) TimerRTCPrescaleFactor[] = {0,0,3,5,6,7,8,10};

// Global variables
// time registers
volatile unsigned long TimerPauseReg;
volatile unsigned long Timer0Reg0;
volatile unsigned long Timer0Reg1;
volatile unsigned long Timer2Reg0;
volatile unsigned long Timer2Reg1;

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntFunc[TIMER_NUM_INTERRUPTS];

// delay for a minimum of <us> microseconds 
// the time resolution is dependent on the time the loop takes 
// e.g. with 4Mhz and 5 cycles per loop, the resolution is 1.25 us 
void delay(unsigned short us) 
{
	unsigned short delay_loops;
	register unsigned short i;

	delay_loops = (us+3)/5*CYCLES_PER_US; // +3 for rounding up (dirty) 

	// one loop takes 5 cpu cycles 
	for (i=0; i < delay_loops; i++) {};
} 

void timerInit(void)
{
	u08 intNum;
	// detach all user functions from interrupts
	for(intNum=0; intNum<TIMER_NUM_INTERRUPTS; intNum++)
		timerDetach(intNum);

	// initialize all timers
	timer0Init();
	timer1Init();
	timer2Init();
	timer3Init();
	// enable interrupts
	sei();
}

void timer0Init()
{
	// initialize timer 0
	timer0SetPrescaler( TIMER0PRESCALE );	// set prescaler
	outp(0, TCNT0);							// reset TCNT0
	sbi(TIMSK, TOIE0);						// enable TCNT0 overflow interrupt

	timer0ClearOverflowCount();				// initialize time registers
}

void timer1Init(void)
{
	// initialize timer 1
	timer1SetPrescaler( TIMER1PRESCALE );	// set prescaler
	outp(0, TCNT1H);						// reset TCNT1
	outp(0, TCNT1L);
	sbi(TIMSK, TOIE1);						// enable TCNT1 overflow
}

void timer2Init(void)
{
	// initialize timer 2
	timer2SetPrescaler( TIMER2PRESCALE );	// set prescaler
	outp(0, TCNT2);							// reset TCNT2
	sbi(TIMSK, TOIE2);						// enable TCNT2 overflow

	timer2ClearOverflowCount();				// initialize time registers
}

void timer3Init(void)
{
	// initialize timer 3
	timer3SetPrescaler( TIMER3PRESCALE );	// set prescaler
	outp(0, TCNT3H);						// reset TCNT3
	outp(0, TCNT3L);
	sbi(ETIMSK, TOIE3);						// enable TCNT3 overflow
}

void timer0SetPrescaler(u08 prescale)
{
	// set prescaler on timer 0
	outp( (inp(TCCR0) & ~TIMER_PRESCALE_MASK) | prescale, TCCR0);
}

void timer1SetPrescaler(u08 prescale)
{
	// set prescaler on timer 1
	outp( (inp(TCCR1B) & ~TIMER_PRESCALE_MASK) | prescale, TCCR1B);
}

void timer2SetPrescaler(u08 prescale)
{
	// set prescaler on timer 2
	outp( (inp(TCCR2) & ~TIMER_PRESCALE_MASK) | prescale, TCCR2);
}

void timer3SetPrescaler(u08 prescale)
{
	// set prescaler on timer 2
	outp( (inp(TCCR3B) & ~TIMER_PRESCALE_MASK) | prescale, TCCR3B);
}

void timerAttach(u08 interruptNum, void (*userFunc)(void) )
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		TimerIntFunc[interruptNum] = userFunc;
	}
}

void timerDetach(u08 interruptNum)
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run nothing
		TimerIntFunc[interruptNum] = 0;
	}
}

void timerPause(unsigned short pause_ms)
{
	// pauses for exactly <pause_ms> number of milliseconds
	u08 timerThres;
	u32 ticRateHz;
	u32 pause;

	// capture current pause timer value
	timerThres = inb(TCNT2);
	// reset pause timer overflow count
	TimerPauseReg = 0;
	// calculate delay for [pause_ms] milliseconds
	// prescaler division = 1<<(PRG_RDB(TimerPrescaleFactor+inp(TCCR2)))
	ticRateHz = F_CPU/(1<<(PRG_RDB(TimerPrescaleFactor+inp(TCCR2))));
	// precision management
	if( ticRateHz > 1000000)
		pause = pause_ms*(ticRateHz/1000);
	else
		pause = (pause_ms*ticRateHz)/1000;
	
	// loop until time expires
	while( ((TimerPauseReg<<8) | inb(TCNT2)) < (pause+timerThres) )
	{
		if( TimerPauseReg < (pause>>8));
		{
			// save power by idling the processor
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();
		}
	}
}

void timer0ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer0Reg0 = 0;	// initialize time registers
	Timer0Reg1 = 0;	// initialize time registers
}

long timer0GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer0ClearOverflowCount() command was called)
	return Timer0Reg0;
}

void timer2ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer2Reg0 = 0;	// initialize time registers
	Timer2Reg1 = 0;	// initialize time registers
}

long timer2GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer2ClearOverflowCount() command was called)
	return Timer2Reg0;
}


void timer1PWMInit(u08 bitRes)
{
	// configures timer1 for use with PWM output
	// on pins OC1A, OC1B, and OC1C

	// enable Timer1 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR1A,WGMA1);
		cbi(TCCR1A,WGMA0);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR1A,WGMA1);
		sbi(TCCR1A,WGMA0);
	}
	else
	{	// default 8bit mode
		cbi(TCCR1A,WGMA1);
		sbi(TCCR1A,WGMA0);
	}

	// set clear-timer-on-compare-match
	//cbi(TCCR1B,CTC1);
	// clear output compare value A
	outb(OCR1AH, 0);
	outb(OCR1AL, 0);
	// clear output compare value B
	outb(OCR1BH, 0);
	outb(OCR1BL, 0);
	// clear output compare value C
	outb(OCR1CH, 0);
	outb(OCR1CL, 0);
}

void timer1PWMOff(void)
{
	// turn off PWM on Timer1
	cbi(TCCR1A,WGMA1);
	cbi(TCCR1A,WGMA0);
	// clear (disable) clear-timer-on-compare-match
	//cbi(TCCR1B,CTC1);
	// set PWM1A/B/C (OutputCompare action) to none
	timer1PWMAOff();
	timer1PWMBOff();
	timer1PWMCOff();
}

void timer1PWMAOn(void)
{
	// turn on channel A (OC1A) PWM output
	// set OC1A as non-inverted PWM
	sbi(TCCR1A,COMA1);
	cbi(TCCR1A,COMA0);
}

void timer1PWMBOn(void)
{
	// turn on channel B (OC1B) PWM output
	// set OC1B as non-inverted PWM
	sbi(TCCR1A,COMB1);
	cbi(TCCR1A,COMB0);
}

void timer1PWMCOn(void)
{
	// turn on channel C (OC1C) PWM output
	// set OC1C as non-inverted PWM
	sbi(TCCR1A,COMC1);
	cbi(TCCR1A,COMC0);
}

void timer1PWMAOff(void)
{
	// turn off channel A (OC1A) PWM output
	// set OC1A (OutputCompare action) to none
	cbi(TCCR1A,COMA1);
	cbi(TCCR1A,COMA0);
}

void timer1PWMBOff(void)
{
	// turn off channel B (OC1B) PWM output
	// set OC1B (OutputCompare action) to none
	cbi(TCCR1A,COMB1);
	cbi(TCCR1A,COMB0);
}

void timer1PWMCOff(void)
{
	// turn off channel C (OC1C) PWM output
	// set OC1C (OutputCompare action) to none
	cbi(TCCR1A,COMC1);
	cbi(TCCR1A,COMC0);
}

void timer1PWMASet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel A
	// this PWM output is generated on OC1A pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outp( (pwmDuty>>8), OCR1AH);		// set the high 8bits of OCR1A
	outp( (pwmDuty&0x00FF), OCR1AL);	// set the low 8bits of OCR1A
}

void timer1PWMBSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC1B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outp( (pwmDuty>>8), OCR1BH);		// set the high 8bits of OCR1B
	outp( (pwmDuty&0x00FF), OCR1BL);	// set the low 8bits of OCR1B
}

void timer1PWMCSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel C
	// this PWM output is generated on OC1C pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outp( (pwmDuty>>8), OCR1CH);		// set the high 8bits of OCR1C
	outp( (pwmDuty&0x00FF), OCR1CL);	// set the low 8bits of OCR1C
}


void timer3PWMInit(u08 bitRes)
{
	// configures timer1 for use with PWM output
	// on pins OC3A, OC3B, and OC3C

	// enable Timer3 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR3A,WGMA1);
		cbi(TCCR3A,WGMA0);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR3A,WGMA1);
		sbi(TCCR3A,WGMA0);
	}
	else
	{	// default 8bit mode
		cbi(TCCR3A,WGMA1);
		sbi(TCCR3A,WGMA0);
	}

	// set clear-timer-on-compare-match
	//cbi(TCCR3B,CTC1);
	// clear output compare value A
	outb(OCR3AH, 0);
	outb(OCR3AL, 0);
	// clear output compare value B
	outb(OCR3BH, 0);
	outb(OCR3BL, 0);
	// clear output compare value B
	outb(OCR3CH, 0);
	outb(OCR3CL, 0);
}

void timer3PWMOff(void)
{
	// turn off PWM mode on Timer3
	cbi(TCCR3A,WGMA1);
	cbi(TCCR3A,WGMA0);
	// clear (disable) clear-timer-on-compare-match
	//cbi(TCCR3B,CTC1);
	// set OC3A/B/C (OutputCompare action) to none
	timer3PWMAOff();
	timer3PWMBOff();
	timer3PWMCOff();
}

void timer3PWMAOn(void)
{
	// turn on channel A (OC3A) PWM output
	// set OC3A as non-inverted PWM
	sbi(TCCR3A,COMA1);
	cbi(TCCR3A,COMA0);
}

void timer3PWMBOn(void)
{
	// turn on channel B (OC3B) PWM output
	// set OC3B as non-inverted PWM
	sbi(TCCR3A,COMB1);
	cbi(TCCR3A,COMB0);
}

void timer3PWMCOn(void)
{
	// turn on channel C (OC3C) PWM output
	// set OC3C as non-inverted PWM
	sbi(TCCR3A,COMC1);
	cbi(TCCR3A,COMC0);
}

void timer3PWMAOff(void)
{
	// turn off channel A (OC3A) PWM output
	// set OC3A (OutputCompare action) to none
	cbi(TCCR3A,COMA1);
	cbi(TCCR3A,COMA0);
}

void timer3PWMBOff(void)
{
	// turn off channel B (OC3B) PWM output
	// set OC3B (OutputCompare action) to none
	cbi(TCCR3A,COMB1);
	cbi(TCCR3A,COMB0);
}

void timer3PWMCOff(void)
{
	// turn off channel C (OC3C) PWM output
	// set OC3C (OutputCompare action) to none
	cbi(TCCR3A,COMC1);
	cbi(TCCR3A,COMC0);
}

void timer3PWMASet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel A
	// this PWM output is generated on OC3A pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outp( (pwmDuty>>8), OCR3AH);		// set the high 8bits of OCR3A
	outp( (pwmDuty&0x00FF), OCR3AL);	// set the low 8bits of OCR3A
}

void timer3PWMBSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC3B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outp( (pwmDuty>>8), OCR3BH);		// set the high 8bits of OCR3B
	outp( (pwmDuty&0x00FF), OCR3BL);	// set the low 8bits of OCR3B
}

void timer3PWMCSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC3C pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outp( (pwmDuty>>8), OCR3CH);		// set the high 8bits of OCR3C
	outp( (pwmDuty&0x00FF), OCR3CL);	// set the low 8bits of OCR3C
}


//! Interrupt handler for tcnt0 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW0)
{
	Timer0Reg0++;		// increment low-order counter
	if(!Timer0Reg0)		// if low-order counter rollover
		Timer0Reg1++;	// increment high-order counter	

	// if a user function is defined, execute it too
	if(TimerIntFunc[TIMER0OVERFLOW_INT])
		TimerIntFunc[TIMER0OVERFLOW_INT]();
}

//! Interrupt handler for Timer1 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW1)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OVERFLOW_INT])
		TimerIntFunc[TIMER1OVERFLOW_INT]();
}

//! Interrupt handler for Timer2 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW2)
{
	Timer2Reg0++;		// increment low-order counter
	if(!Timer2Reg0)		// if low-order counter rollover
		Timer2Reg1++;	// increment high-order counter	

	// increment pause counter
	TimerPauseReg++;

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OVERFLOW_INT])
		TimerIntFunc[TIMER2OVERFLOW_INT]();
}

//! Interrupt handler for Timer3 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW3)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3OVERFLOW_INT])
		TimerIntFunc[TIMER3OVERFLOW_INT]();
}

//! Interrupt handler for OutputCompare0 match (OC0) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE0)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0OUTCOMPARE_INT])
		TimerIntFunc[TIMER0OUTCOMPARE_INT]();
}

//! Interrupt handler for OutputCompare1A match (OC1A) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1A)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREA_INT])
		TimerIntFunc[TIMER1OUTCOMPAREA_INT]();
}

//! Interrupt handler for OutputCompare1B match (OC1B) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1B)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREB_INT])
		TimerIntFunc[TIMER1OUTCOMPAREB_INT]();
}

//! Interrupt handler for OutputCompare1C match (OC1C) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1C)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREC_INT])
		TimerIntFunc[TIMER1OUTCOMPAREC_INT]();
}

//! Interrupt handler for InputCapture1(IC1) interrupt
TIMER_INTERRUPT_HANDLER(SIG_INPUT_CAPTURE1)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1INPUTCAPTURE_INT])
		TimerIntFunc[TIMER1INPUTCAPTURE_INT]();
}

//! Interrupt handler for OutputCompare2 match (OC2) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE2)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OUTCOMPARE_INT])
		TimerIntFunc[TIMER2OUTCOMPARE_INT]();
}

//! Interrupt handler for OutputCompare3A match (OC3A) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE3A)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3OUTCOMPAREA_INT])
		TimerIntFunc[TIMER3OUTCOMPAREA_INT]();
}

//! Interrupt handler for OutputCompare3B match (OC3B) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE3B)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREB_INT])
		TimerIntFunc[TIMER1OUTCOMPAREB_INT]();
}

//! Interrupt handler for OutputCompare3C match (OC3C) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE3C)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3OUTCOMPAREC_INT])
		TimerIntFunc[TIMER3OUTCOMPAREC_INT]();
}

//! Interrupt handler for InputCapture3 (IC3) interrupt
TIMER_INTERRUPT_HANDLER(SIG_INPUT_CAPTURE3)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3INPUTCAPTURE_INT])
		TimerIntFunc[TIMER3INPUTCAPTURE_INT]();
}
