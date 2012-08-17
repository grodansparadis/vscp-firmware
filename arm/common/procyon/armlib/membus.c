/*! \file membus.c \brief Software-driven Memory Bus for ARMmini-LPC210x. */
//*****************************************************************************
//
// File Name	: 'membus.c'
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

#include "lpc210x.h"
#include "global.h"
#include "timer.h"
#include "membus.h"

void membusInit(void)
{
	// setup I/O lines
	IOCLR = MEMBUS_LATCH;
	IOSET = MEMBUS_nRD;
	IOSET = MEMBUS_nWR;
	// set inputs
	IODIR &= ~P015;
	// set outputs
	IODIR |= (MEMBUS_IO | MEMBUS_LATCH | MEMBUS_nRD | MEMBUS_nWR);

}

uint16_t membusRead(uint16_t addr)
{
	uint16_t data;

	// switch bus to output
	IODIR |= MEMBUS_IO;
	// assert address
	IOCLR = MEMBUS_IO;
	IOSET = addr<<16;
	// latch address
	IOSET = MEMBUS_LATCH;
	// delay
	MEMBUS_DELAY;
	IOCLR = MEMBUS_LATCH | MEMBUS_IO;
	
	// switch bus to input
	IODIR &= ~MEMBUS_IO;
	// assert read
	IOCLR = MEMBUS_nRD;
	// delay
	MEMBUS_DELAY;
	// read in data
	data = IOPIN>>16;
	// release read
	IOSET = MEMBUS_nRD;
	
	return data;
}

void membusWrite(uint16_t addr, uint16_t data)
{
	// switch bus to output
	IODIR |= MEMBUS_IO;
	// assert address
	IOCLR = MEMBUS_IO;
	IOSET = addr<<16;
	// latch address
	IOSET = MEMBUS_LATCH;
	// delay
	MEMBUS_DELAY;
	IOCLR = MEMBUS_LATCH | MEMBUS_IO;
	
	// output data
	IOCLR = MEMBUS_IO;
	IOSET = ((uint32_t)data)<<16;
	// assert write
	IOCLR = MEMBUS_nWR;
	// delay
	MEMBUS_DELAY;
	// release write
	IOSET = MEMBUS_nWR;
}
