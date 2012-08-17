/*! \file processor.c \brief AT91SAM7S Processor Initialization and Support. */
//*****************************************************************************
//
// File Name	: 'processor.c'
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
//*****************************************************************************

// Include the processor definitions
#include "at91sam7s64.h"
// Include project-level definitions
#include "global.h"

#include "processor.h"

// The following functions must be writen in ARM mode this function called directly
// by exception vector
extern void AT91F_Spurious_handler(void);
extern void AT91F_Default_IRQ_handler(void);
extern void AT91F_Default_FIQ_handler(void);

// if you wish to change the interrupt priority for the system interrupt,
// DO NOT EDIT THE LINE BELOW.  Add your desired definition for
// xxx_INTERRUPT_LEVEL to your project's global.h and it will override
// this default definition.
#ifndef SYS_INTERRUPT_LEVEL
#define SYS_INTERRUPT_LEVEL		AT91C_AIC_PRIOR_HIGHEST
#endif

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr SysIntFunc[SYSPID_NUM];


// Low-level processor initialization and clock control
// ** requires valid stack setup
void processorInit( void)
{
	// make pointer to PMC to reduce code size
	//AT91PS_PMC pPMC = AT91C_BASE_PMC;
	
	// Set Flash wait state to 1WS
	// Single Cycle Access at Up to 30 MHz, or 40
	AT91C_BASE_MC->MC_FMR = AT91C_MC_FWS_1FWS;
	
	// Disable watchdog
	AT91C_BASE_WDTC->WDTC_WDMR= AT91C_WDTC_WDDIS;
	
	// Enable the Main Oscillator
	// SCK = 1/32768 = 30.51 uSecond
	// Start up time = 8 * 6 / SCK = 56 * 30.51 = 1.46484375 ms
	AT91C_BASE_PMC->PMC_MOR = (( AT91C_CKGR_OSCOUNT & (0x06<<8) ) | AT91C_CKGR_MOSCEN );
	// Wait the startup time
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCS));
	
	// Set PLL and divider:
	// - div by 5 Fin = 3.6864 =(18.432 / 5)
	// - Mul 25+1: Fout =	95.8464 =(3.6864*26)
	// for 96 MHz the erroe is 0.16%
	// Field out NOT USED = 0
	// PLLCOUNT pll startup time estimate at : 0.844 ms
	// PLLCOUNT 28 = 0.000844 /(1/32768)
	AT91C_BASE_PMC->PMC_PLLR = 
		((AT91C_CKGR_DIV & (OSC_DIV)) |
		(AT91C_CKGR_PLLCOUNT & (28<<8)) |
		(AT91C_CKGR_MUL & ((PLL_MUL-1)<<16)));
	
	// Wait for PLL to lock and for clock to become ready
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCK));
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));
	
	// Set Processor Clock and Master Clock to PLL Clock/2
	AT91C_BASE_PMC->PMC_MCKR = AT91C_PMC_PRES_CLK_2;
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));
	
	AT91C_BASE_PMC->PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK;
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

	// if 18.432MHz crystal, master clock now 47.9232MHz -> 48MHz for USB operation

	// setup initial AIC config
	processorAicInit();
	// attach the system interrupt distributer
	processorAicAttach(AT91C_ID_SYS, (SYS_INTERRUPT_LEVEL|AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL), processorSysIntService);
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

void processorAicInit(void)
{
	int i;

	// Set up the default interrupt handler vectors
	AT91C_BASE_AIC->AIC_SVR[0] = (unsigned int)AT91F_Default_FIQ_handler;
	for(i=1; i<31; i++)
	{
		AT91C_BASE_AIC->AIC_SVR[i] = (unsigned int)AT91F_Default_IRQ_handler;
	}
	AT91C_BASE_AIC->AIC_SPU  = (unsigned int)AT91F_Spurious_handler;
}

void processorAicAttach(int pid, int srcmode, void (*userFunc)(void) )
{
	// first disable the interrupt at the AIC
	AT91C_BASE_AIC->AIC_IDCR = (1<<pid);
	// set a new interrupt handler routine pointer
	AT91C_BASE_AIC->AIC_SVR[pid] = (unsigned int)userFunc;
	// configure the interrupt
	AT91C_BASE_AIC->AIC_SMR[pid] = srcmode;
	// clear the interrupt flag in the AIC
	AT91C_BASE_AIC->AIC_ICCR = (1<<pid);
	// enable the interrupt at the AIC
	AT91C_BASE_AIC->AIC_IECR = (1<<pid);
}

void processorAicDetach(int pid)
{
	// first disable the interrupt at the AIC
	AT91C_BASE_AIC->AIC_IDCR = (1<<pid);
	// clear the interrupt flag in the AIC
	AT91C_BASE_AIC->AIC_ICCR = (1<<pid);
	// clear the interrupt handler routine pointer
	AT91C_BASE_AIC->AIC_SVR[pid] = 0;
}

void processorAicAttachSys(int syspid, void (*userFunc)(void) )
{
	// make sure the interrupt number is within bounds
	if(syspid < SYSPID_NUM)
	{
		// set the interrupt service to run the supplied user function
		SysIntFunc[syspid] = userFunc;
	}
}

void processorSysIntService(void)
{
	// service most likely SYSINT interrupt sources first.
	// in each if(), we are careful to check only *enabled* interrupt sources!

	// check if sysint caused by PITC (periodic interval timer)
	if( (AT91C_BASE_PITC->PITC_PIMR & AT91C_PITC_PITIEN) && 
		(AT91C_BASE_PITC->PITC_PISR & AT91C_PITC_PITS) )
	{
		if(SysIntFunc[SYSPID_PITC]) SysIntFunc[SYSPID_PITC]();
		return;
	}
	// check if sysint caused by DBGU (debug uart)
	if(AT91C_BASE_DBGU->DBGU_IMR & AT91C_BASE_DBGU->DBGU_CSR)
	{
		if(SysIntFunc[SYSPID_DBGU]) SysIntFunc[SYSPID_DBGU]();
		return;
	}
	// check if sysint caused by RTT (real-time timer)
	if( ((AT91C_BASE_RTTC->RTTC_RTMR>>16) & AT91C_BASE_RTTC->RTTC_RTSR) &
			(AT91C_RTTC_RTTINC|AT91C_RTTC_ALMS) )
	{
		if(SysIntFunc[SYSPID_RTTC]) SysIntFunc[SYSPID_RTTC]();
		return;
	}
	// check if sysint caused by EFC (embedded flash controller)
	if( (AT91C_BASE_MC->MC_FMR & AT91C_BASE_MC->MC_FSR) &
			(AT91C_MC_FRDY|AT91C_MC_LOCKE|AT91C_MC_PROGE) )
	{
		if(SysIntFunc[SYSPID_EFC]) SysIntFunc[SYSPID_EFC]();
		return;
	}
	// check if sysint caused by PMC (power management controller)
	if( AT91C_BASE_PMC->PMC_IER & AT91C_BASE_PMC->PMC_SR )
	{
		if(SysIntFunc[SYSPID_PMC]) SysIntFunc[SYSPID_PMC]();
		return;
	}

	// could not determine source of interrupt
	// throw error?
}

