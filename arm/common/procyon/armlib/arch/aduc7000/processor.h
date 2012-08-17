/*! \file processor.h \brief ADuC7000 Processor Initialization and Support. */
//*****************************************************************************
//
// File Name	: 'processor.h'
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
///	\ingroup driver_arm_aduc7000
/// \defgroup processor_aduc7000 ADuC7000 Processor Initialization and Support (armlib/arch/aduc7000/processor.c)
/// \code #include "processor.h" \endcode
/// \par Overview
///		To be written...
//
//*****************************************************************************
//@{

#ifndef PROCESSOR_H
#define PROCESSOR_H

// masks for use with enable/disable ARM core interrupts
#define CPSR_MASK_IRQ 0x00000080
#define CPSR_MASK_FIQ 0x00000040
#define CPSR_MASK_INT (CPSR_MASK_IRQ | CPSR_MASK_FIQ)

// general processor functions
void processorInit(void);
unsigned int processorEnableInt(unsigned int cpsr_mask);
unsigned int processorDisableInt(unsigned int cpsr_mask);
unsigned int processorRestoreInt(unsigned int cpsr_orig);


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
