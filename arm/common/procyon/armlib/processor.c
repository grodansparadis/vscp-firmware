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

#include "lpc210x.h"

#include "global.h"
#include "processor.h"

void processorInit(void)
{
	// set PLL multiplier & divisor.
	// values computed from config.h
	PLLCFG = PLLCFG_MSEL | PLLCFG_PSEL;

	// enable PLL
	PLLCON = PLLCON_PLLE;
	PLLFEED = 0xAA;                       // Make it happen.  These two updates
	PLLFEED = 0x55;                       // MUST occur in sequence.

	// wait for PLL lock
	while (!(PLLSTAT & PLLSTAT_LOCK))
		continue;

	// enable & connect PLL
	PLLCON = PLLCON_PLLE | PLLCON_PLLC;
	PLLFEED = 0xAA;                       // Make it happen.  These two updates
	PLLFEED = 0x55;                       // MUST occur in sequence.

	// setup & enable the MAM
	MAMTIM = MAMTIM_CYCLES;
	MAMCR = MAMCR_FULL;

	// set the peripheral bus speed
	// value computed from global.h
	VPBDIV = VPBDIV_VALUE;                // set the peripheral bus clock speed
}




#define IRQ_MASK 0x00000080
#define FIQ_MASK 0x00000040
#define INT_MASK (IRQ_MASK | FIQ_MASK)

static inline unsigned __get_cpsr(void)
{
  unsigned long retval;
  asm volatile (" mrs  %0, cpsr" : "=r" (retval) : /* no inputs */  );
  return retval;
}

static inline void __set_cpsr(unsigned val)
{
  asm volatile (" msr  cpsr, %0" : /* no outputs */ : "r" (val)  );
}

unsigned disableIRQ(void)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr(_cpsr | IRQ_MASK);
  return _cpsr;
}

unsigned restoreIRQ(unsigned oldCPSR)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr((_cpsr & ~IRQ_MASK) | (oldCPSR & IRQ_MASK));
  return _cpsr;
}

unsigned enableIRQ(void)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr(_cpsr & ~IRQ_MASK);
  return _cpsr;
}

unsigned disableFIQ(void)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr(_cpsr | FIQ_MASK);
  return _cpsr;
}

unsigned restoreFIQ(unsigned oldCPSR)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr((_cpsr & ~FIQ_MASK) | (oldCPSR & FIQ_MASK));
  return _cpsr;
}

unsigned enableFIQ(void)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr(_cpsr & ~FIQ_MASK);
  return _cpsr;
}
