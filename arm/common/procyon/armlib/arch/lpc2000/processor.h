/*! \file processor.h \brief LPC2100 Processor Initialization and Support. */
//*****************************************************************************
//
// File Name	: 'processor.h'
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
///	\ingroup driver_arm_lpc2000
/// \defgroup processor_lpc2000 LPC2000 Processor Initialization and Support (armlib/arch/lpc2000/processor.c)
/// \code #include "processor.h" \endcode
/// \par Overview
///		To be written...
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
//@{

#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "global.h"

// masks for use with enable/disable ARM core interrupts
#define CPSR_MASK_IRQ 0x00000080
#define CPSR_MASK_FIQ 0x00000040
#define CPSR_MASK_INT (CPSR_MASK_IRQ | CPSR_MASK_FIQ)

// general processor functions
void processorInit(void);
unsigned int processorEnableInt(unsigned int cpsr_mask);
unsigned int processorDisableInt(unsigned int cpsr_mask);
unsigned int processorRestoreInt(unsigned int cpsr_orig);

// VIC functions

// initialize the VIC vectors
void processorVicInit(void);

// Attach an interrupt handler on specified peripheral-ID interrupt channel.
//	Also enables interrupt in VIC.
void processorVicAttach(int pid, int srcmode, void (*userFunc)(void) );

// Detach current interrupt handler on specified peripheral-ID interrupt channel.
//	Also disables interrupt in VIC.
void processorVicDetach(int pid);


// Macro for use upon entry to naked interrupt handler
// - saves non-banked CPU registers and SPSR onto IRQ/FIQ stack
#define ISR_ENTRY() asm volatile(" sub   lr, lr,#4\n" \
                                 " stmfd sp!,{r0-r12,lr}\n" \
                                 " mrs   r1, spsr\n" \
                                 " stmfd sp!,{r1}")

// Macro for use upon exit from naked interrupt handler
// - restores SPSR and non-banked CPU registers from IRQ/FIQ stack
#define ISR_EXIT()  asm volatile(" ldmfd sp!,{r1}\n" \
                                 " msr   spsr_c,r1\n" \
                                 " ldmfd sp!,{r0-r12,pc}^")

#endif
//@}
