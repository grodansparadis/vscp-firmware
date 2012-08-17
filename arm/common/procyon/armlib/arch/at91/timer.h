/*! \file timer.h \brief Timer Support Library for AT91SAM7S. */
//*****************************************************************************
//
// File Name	: 'timer.h'
// Title		: Timer Support Library for AT91SAM7S
// Author		: Pascal Stang - Copyright (C) 2005-2006
// Created		: 2005.05.26
// Revised		: 2006.07.27
// Version		: 0.1
// Target MCU	: Atmel ARM AT91SAM7S Series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
///	\ingroup driver_arm_at91
/// \defgroup timer_at91 Timer Support Library for AT91SAM7S (armlib/arch/at91/timer.c)
/// \code #include "timer.h" \endcode
/// \par Overview
///		To be written...
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
//@{

#ifndef ARMLIB_TIMER_H
#define ARMLIB_TIMER_H

#include "global.h"

// timer interrupt handler indexes
#define TIMER_PITOVERFLOW_INT	0
#define TIMER_NUM_INTERRUPTS	1

// short-duration delay loop
// (traditionally included in the timer library)
void delay_us(unsigned long t);

// initialize timers in general
// sets default pit rate to 1000Hz
void timerInit(void);

// initialize PIT timer (specify overflow 'rate' in Hz)
void timerInitPit(int rate);

// Attach a user function to a timer interrupt
void timerAttach(u08 interruptNum, void (*userFunc)(void) );

// timerPause busy-waits for the specified number of milliseconds
// (if PIT rate is modified from default of 1000Hz, units of 'pause' will be PIT intervals)
void timerPause(int pause);


// interrupt handlers for timers

// interrupt handler for pit
void timerServicePit(void);


#endif
//@}


