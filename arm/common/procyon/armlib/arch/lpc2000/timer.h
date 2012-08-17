/*! \file timer.h \brief Timer Support Library for LPC2100. */
//*****************************************************************************
//
// File Name	: 'timer.h'
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

#ifndef TIMER_H
#define TIMER_H

#include "global.h"

// defines
#define TIMER0MATCH0_INT		0
#define TIMER0MATCH1_INT		1
#define TIMER0MATCH2_INT		2
#define TIMER0MATCH3_INT		3
#define TIMER0CAPTURE0_INT		4
#define TIMER0CAPTURE1_INT		5
#define TIMER0CAPTURE2_INT		6
//#define TIMER0CAPTURE3_INT	7
#define TIMER1MATCH0_INT		8
#define TIMER1MATCH1_INT		9
#define TIMER1MATCH2_INT		10
#define TIMER1MATCH3_INT		11
#define TIMER1CAPTURE0_INT		12
#define TIMER1CAPTURE1_INT		13
#define TIMER1CAPTURE2_INT		14
#define TIMER1CAPTURE3_INT		15

#define TIMER_NUM_INTERRUPTS	16

// edges
#define TIMER_CAPTURE_NONE		0x00
#define TIMER_CAPTURE_RISING	0x01
#define TIMER_CAPTURE_FALLING	0x02

// functions
void delay(unsigned long d);
#define delay_ms	timerPause

void timerInit(void);
void timer0Init(void);
void timer1Init(void);

//! Attach a user function to a timer interrupt
void timerAttach(u08 interruptNum, void (*userFunc)(void) );
//! Detach a user function from a timer interrupt
void timerDetach(u08 interruptNum);


void timer0PrescalerSet(u32 clockDiv);
void timer1PrescalerSet(u32 clockDiv);

void timerPause(unsigned long pause_ms);

void timer0ClearOverflowCount(void);
u32 timer0GetOverflowCount(void);
void timer1ClearOverflowCount(void);
u32 timer1GetOverflowCount(void);


void timer0Match0Set(u32 value);
void timer0Match1Set(u32 value);

void timer1Match0Set(u32 value);
void timer1Match1Set(u32 value);

void timer0Capture0Init(unsigned int edge);
void timer0Capture1Init(unsigned int edge);
void timer1Capture0Init(unsigned int edge);
void timer1Capture1Init(unsigned int edge);

void timer0Service(void) __attribute__((naked));
void timer1Service(void) __attribute__((naked));

#endif
