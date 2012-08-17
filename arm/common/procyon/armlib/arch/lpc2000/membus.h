/*! \file membus.h \brief Software-driven Memory Bus for ARMmini-LPC210x. */
//*****************************************************************************
//
// File Name	: 'membus.h'
// Title		: Software-driven Memory Bus for ARMmini-LPC210x
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

#ifndef MEMBUS_H
#define MEMBUS_H

#include "global.h"

#define MEMBUS_IO		0xFFFF0000
#define MEMBUS_LATCH	0x00000800
#define MEMBUS_nRD		0x00001000
#define MEMBUS_nWR		0x00002000
#define P015			0x00008000

//#define MEMBUS_DELAY
//#define MEMBUS_DELAY	asm volatile ("nop\r\n")
//#define MEMBUS_DELAY	asm volatile ("nop\r\n nop\r\n")
//#define MEMBUS_DELAY	asm volatile ("nop\r\n nop\r\n nop\r\n nop\r\n")
//#define MEMBUS_DELAY	asm volatile ("nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n")
#define MEMBUS_DELAY	asm volatile ("nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n")
//#define MEMBUS_DELAY	asm volatile ("nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n nop\r\n")
//#define MEMBUS_DELAY	delay(1000)

void membusInit(void);
uint16_t membusRead(uint16_t addr);
void membusWrite(uint16_t addr, uint16_t data);

#endif
