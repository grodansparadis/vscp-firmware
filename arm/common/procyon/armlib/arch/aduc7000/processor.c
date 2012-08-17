/*! \file processor.c \brief ADuC7000 Processor Initialization and Support. */
//*****************************************************************************
//
// File Name	: 'processor.c'
// Title		: ADuC7000 Processor Initialization and Support
// Author		: Pascal Stang - Copyright (C) 2006
// Created		: 2006.01.30
// Revised		: 2006.02.20
// Version		: 0.1
// Target MCU	: ARM processors
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
//*****************************************************************************

// Include the processor definitions
#include "aduc7026.h"
// Include project-level definitions
#include "global.h"

#include "processor.h"

// Low-level processor initialization and clock control
void processorInit( void)
{
	// setup PLL
	PLLKEY1 = 0xAA;
	PLLCON =  0x01;
	PLLKEY2 = 0x55;

	POWKEY1 = 0x01;
	POWCON =  0x00;
	POWKEY2 = 0xF4;
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
	asm volatile ("msr  cpsr, %0" : : "r" ( (cpsr&~&CPSR_MASK_INT) | (cpsr_orig&CPSR_MASK_INT)) );
	// return the original CPSR
	return cpsr;
}
