/*! \file processor.c \brief LPC2100 Processor Initialization and Support. */
//*****************************************************************************
//
// File Name	: 'processor.c'
// Title		: LPC2100 Processor Initialization and Support
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

#include "lpc2000.h"

#include "global.h"
#include "processor.h"

void processorInit(void)
{
	// setup processor PLL clock
	// set the PLL multiplier and divisor (auto-computed by lpcSCB.h)
	PLLCFG = PLLCFG_MSEL | PLLCFG_PSEL;

	// PLL procedure as per LPC2000 datasheet
	PLLCON = PLLCON_PLLE;				// enable PLL
	PLLFEED = 0xAA;	PLLFEED = 0x55;		// load the changes
	while(!(PLLSTAT & PLLSTAT_LOCK));	// wait until PLL locks
	PLLCON = PLLCON_PLLE | PLLCON_PLLC;	// now connect PLL as system clock
	PLLFEED = 0xAA;	PLLFEED = 0x55;		// load the changes

	// setup memory access timing (memory accelerator module)
	MAMTIM = MAMTIM_CYCLES;				// set cycle time
	MAMCR = MAMCR_FULL;					// set full acceleration features

	// setup peripheral bus speed (set by global.h)
	VPBDIV = VPBDIV_VALUE;

	// initialize processor and VIC interrupt handling
	processorVicInit();
}

unsigned int processorDisableInt(unsigned int cpsr_mask)
{
	unsigned int cpsr;
	// read CPSR
	asm volatile ("mrs  %0, cpsr" : "=r" (cpsr) : );
	// set interrupt disable bit and write CPSR
	asm volatile ("msr  cpsr, %0" : : "r" (cpsr|(cpsr_mask&CPSR_MASK_INT)) );
	// return the original CPSR
	return cpsr;
}

unsigned int processorEnableInt(unsigned int cpsr_mask)
{
	unsigned int cpsr;
	// read CPSR
	asm volatile ("mrs  %0, cpsr" : "=r" (cpsr) : );
	// clear interrupt disable bit(s) and write CPSR
	asm volatile ("msr  cpsr, %0" : : "r" (cpsr&~(cpsr_mask&CPSR_MASK_INT)) );
	// return the original CPSR
	return cpsr;
}

unsigned int processorRestoreInt(unsigned int cpsr_orig)
{
	unsigned int cpsr;
	// read CPSR
	asm volatile ("mrs  %0, cpsr" : "=r" (cpsr) : );
	// clear interrupt disable bit(s) and write CPSR
	asm volatile ("msr  cpsr, %0" : : "r" ( (cpsr&~CPSR_MASK_INT) | (cpsr_orig&CPSR_MASK_INT)) );
	// return the original CPSR
	return cpsr;
}

void processorVicInit(void)
{
	int i;

	// first disable all interrupts at the VIC
	VICIntEnClear = 0xFFFF;
	// clear soft interrupts
	VICSoftIntClear = 0xFFFF;
	// reset all interrupts as IRQ (not FIQ)
	VICIntSelect = 0;

	// TODO: make and register a spurious/default intr handler

	// reset all vectors in the VIC
	for(i=0; i<16; i++)
	{
		(&VICVectCntl0)[i] = 0;
		(&VICVectAddr0)[i] = 0;
	}
	// set default handler
	VICDefVectAddr = 0;
}

void processorVicAttach(int pid, int srcmode, void (*userFunc)(void) )
{
	// first disable the interrupt at the AIC
	VICIntEnClear = (1<<pid);
	// assign a VIC slot
	(&VICVectCntl0)[pid] = VIC_ENABLE | pid;
	// set a new interrupt handler routine pointer
	(&VICVectAddr0)[pid] = (unsigned int)userFunc;
	// set interrupt as IRQ
	VICIntSelect &= ~(1<<pid);
	// enable the interrupt at the VIC
	VICIntEnable |= (1<<pid);
}

void processorVicDetach(int pid)
{
	// first disable the interrupt at the VIC
	VICIntEnClear = (1<<pid);
	// clear the interrupt flag in the VIC
	// = (1<<pid);
	// clear the interrupt handler routine pointer
	(&VICVectAddr0)[pid] = 0;
}
