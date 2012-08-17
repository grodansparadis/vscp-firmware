/**************************************************************************
 *
 * FILE NAME:			common.h
 * FILE DESCRIPTION:	Common routines and macros
 *
 * FILE CREATION DATE:	24-05-2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,24may03 erd	written
 *
 ***************************************************************************/

#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdio.h>

//
// time routines 
//

// delay one microsecond
#define time_delayOneUs() {asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");}

// declare delay counter
extern unsigned char time_delayUsLeft;

// delay one uS
#define time_delayUs(x) 							\
{													\
	time_delayUsLeft = (unsigned char)(x);			\
	asm("movlb (_time_delayUsLeft) >> 8");			\
	time_delayOneUs();								\
	asm("decfsz (_time_delayUsLeft)&0ffh, f");		\
	asm("goto $ - 6");								\
}

// delay one mS
//
// Arguments:
//	ms: the amount of miliseconds to delay
// 
extern void time_delayMs(int_16 ms);

//
// bit definitions
//

#define BIT0			(1 << 0)
#define BIT1			(1 << 1)
#define BIT2			(1 << 2)
#define BIT3			(1 << 3)
#define BIT4			(1 << 4)
#define BIT5			(1 << 5)
#define BIT6			(1 << 6)
#define BIT7			(1 << 7)

//
// bit routines
//

// set / clear bits
#define BIT_SET(bit)					(bit = 1)
#define BIT_CLR(bit)					(bit = 0)

// clear a bit and wait for it to set
#define BIT_CLR_WAIT_SET(bit) 			{bit = 0; while(!bit);}
#define BIT_SET_WAIT_CLR(bit) 			{bit = 1; while(bit);}

// pulse a bit
#define BIT_PULSE(bit)					{bit = 1; bit = 0;}
#define BIT_PULSE_WAIT_US(bit, delay)	{bit = 1; time_delayUs(delay); bit = 0;}
#define BIT_PULSE_WAIT_MS(bit, delay)	{bit = 1; time_delayMs(delay); bit = 0;}
#define BIT_CLR_NOWAIT_SET(bit)			{bit = 0; bit = 1;}
#define BIT_CLR_WAIT_SET_US(bit, delay)	{bit = 0; time_delayUs(delay); bit = 1;}
#define BIT_CLR_WAIT_SET_MS(bit, delay)	{bit = 0; time_delayMs(delay); bit = 1;}

// masking routines

#define MSK_HI_BYTE(myInt)				((myInt >> 8) & 0xFF)	// get high byte from an int
#define MSK_LO_BYTE(myInt)				((myInt) & 0xFF)		// get low byte from an int


//
// Generic routines
// 

// uint_8 calculateChecksum(uint_8 *buffer, uint_8 size)
//
// Caluclates an 8bit checksum over contents of buffer
// 
// Arguments:
//	buffer: ptr to start of buffer on which chksum is calculated over
//	size: size, in bytes, of buffer
//
// Return value:
//	checksum value
//
uint_8 sys_calculateChecksum(uint_8 *buffer, uint_8 size);


// uint_16 calculateChecksum16(uint_8 *buffer, uint_16 size)
//
// Caluclates an 16bit checksum over contents of buffer
// 
// Arguments:
//	buffer: ptr to start of buffer on which chksum is calculated over
//	size: size, in bytes, of buffer
//
// Return value:
//	checksum value
//

uint_16 sys_calculateChecksum16(uint_8 *buffer, uint_16 size);


//
// Button handler
// 

// control byte map
//
// BIT0: 0 if previously not pressed, 1 if pressed
// BIT1-7: TBD
//
#define BUTTON_STATE_BIT BIT0

// button_checkClick(ctrl, status, clickFunction)
//
// Checks if a button's onClick routine needs to be called. The decision is
// as follows: if the button is currently pressed, but was not pressed before, 
// then the button's ctrl STATE bit is set to 1. If the button is not pressed but
// the button's ctrl STATE bit is 1 - this means that it was just released: the
// STATE bit is cleared and the clickFunction is envoked
//
// Arguments:
//	ctrl: control byte of the button. stored in the using module
//	status: the current status of the button: TRUE if pressed, FALSE if not																	
//	clickFunction: a routine which received no arguments which is called on click detect
//
#define button_checkClick(ctrl, status, clickFunction)			\
{																\
	if (status && !(ctrl & BUTTON_STATE_BIT))					\
	{															\
		ctrl |= BUTTON_STATE_BIT;                               \
	}                                                           \
	else                                                        \
	{                                                           \
		if (!status && (ctrl & BUTTON_STATE_BIT))               \
		{                                                       \
			ctrl &= ~BUTTON_STATE_BIT;                          \
			clickFunction();                                    \
		}                                                       \
	}                                                           \
}
																
// void time_startSyncTimerMs(const uint_16 ms)
//
// Starts a synchronous hardware timer. The function returns immediately and
// can be accompanied by time_waitSyncTimer() to perform blocks for precise
// delays. On expiration of the timer, no interrupt is generated. Uses TMR0.
// 
// Arguments:
//	ms: number of miliseconds for timer expiration
//

extern void time_startSyncTimerMs(const uint_16 ms);

// void time_waitSyncTimer()
//
// Blocks until the timer started by time_startSyncTimerMs expires.
// 

extern void time_waitSyncTimer();

// void time_stopSyncTimer()
//
// Stops the hardware timer. If the timer isn't started, this routine has no effect.
// 

extern void time_stopSyncTimer();

// void time_getSyncTimerMsLeft()
//
// Returns values:
//	Number of ms left before the timer started by time_startSyncTimerMs expires. 
//

extern uint_16 time_getSyncTimerMsLeft();

//
// logger facility
//

extern char logBuffer[32];
extern void logOutput(char *);

#define log(str, arg1, arg2, arg3)								\
sprintf(logBuffer, str, arg1, arg2, arg3);						\
logOutput(logBuffer);

#endif /* __COMMON_H_ */

