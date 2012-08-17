/*! \file processor.h \brief AT91SAM7S Processor Initialization and Support. */
//*****************************************************************************
//
// File Name	: 'processor.h'
// Title		: AT91SAM7S Processor Initialization and Support
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
///	\ingroup driver_arm_at91
/// \defgroup processor_at91 AT91SAM7S Processor Initialization and Support (armlib/arch/at91/processor.c)
/// \code #include "processor.h" \endcode
/// \par Overview
///		To be written...
//
//*****************************************************************************
//@{

#ifndef ARMLIB_PROCESSOR_H
#define ARMLIB_PROCESSOR_H

// masks for use with enable/disable ARM core interrupts
#define CPSR_MASK_IRQ 0x00000080
#define CPSR_MASK_FIQ 0x00000040
#define CPSR_MASK_INT (CPSR_MASK_IRQ | CPSR_MASK_FIQ)

// general processor functions
void processorInit(void);
unsigned int processorEnableInt(unsigned int cpsr_mask);
unsigned int processorDisableInt(unsigned int cpsr_mask);
unsigned int processorRestoreInt(unsigned int cpsr_orig);

// use at beginning of any critical code block to disable interrupts (prevents interruptions)
#define CRITICAL_SECTION_BEGIN		unsigned int _cpsr = processorDisableInt(CPSR_MASK_INT)
// use at end of any critical code block to restore interrupts
#define CRITICAL_SECTION_END		processorRestoreInt(_cpsr)

// SYSPID defines for use with processorAicAttachSys()
#define SYSPID_PITC	0	// periodic interval timer
#define SYSPID_DBGU	1	// debug uart
#define SYSPID_RTTC	2	// real-time timer
#define SYSPID_EFC	3	// embedded flash controller
#define SYSPID_PMC	4	// power management controller
#define SYSPID_NUM	5	// total number of system interrupt peripherals

// AIC functions

// initialize the AIC vectors
void processorAicInit(void);

// Attach an interrupt handler on specified peripheral-ID interrupt channel.
//	Also enables interrupt in AIC.
void processorAicAttach(int pid, int srcmode, void (*userFunc)(void) );

// Detach current interrupt handler on specified peripheral-ID interrupt channel.
//	Also disables interrupt in AIC.
void processorAicDetach(int pid);

// Attach an interrupt handler on specified sub-peripheral of system interrupt channel (int ch 2).
void processorAicAttachSys(int syspid, void (*userFunc)(void) );

// Interrupt handler for system interrupt (internal).
// function determines source of interrupt and calls appropriate attached function
void processorSysIntService(void);


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
