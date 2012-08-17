/******************************************************************************
 *
 * $RCSfile: config.h,v $
 * $Revision: 1.1 $
 *
 * This module provides information about the project configuration
 * Copyright 2004, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *****************************************************************************/
#ifndef INC_CONFIG_H
#define INC_CONFIG_H

#include "types.h"
#include <lpc21xx.h>

// some handy DEFINES
#ifndef FALSE
#define FALSE               0
#ifndef TRUE
#define TRUE                !FALSE
#endif
#endif

#ifndef ON
#define ON					!FALSE
#endif

#ifndef OFF
#define OFF					FALSE
#endif

#ifndef BIT
#define BIT(n)              (1 << (n))
#endif


// declare functions and values from crt0.S & the linker control file
extern void reset(void);
// extern void exit(void);
//extern void abort(void);
// maybe add interrupt vector addresses

#define HOST_BAUD0          (57600)	// Baudrate channel 1
#define HOST_BAUD1          (57600)	// Baudrate channel 2

///////////////////////////////////////////////////////////////////////////////
// MAM defines
#define MAMCR_OFF     		0
#define MAMCR_PART    		1
#define MAMCR_FULL    		2

#define MAMTIM_CYCLES 		(((CCLK) + 19999999) / 20000000)

///////////////////////////////////////////////////////////////////////////////
// MEMMAP defines
#define MEMMAP_BBLK   		0                 		// Interrupt Vectors in Boot Block
#define MEMMAP_FLASH  		1                 		// Interrupt Vectors in Flash
#define MEMMAP_SRAM  	 	2                 		// Interrupt Vectors in SRAM

// PLL setup values are computed within the LPC include file
// It relies upon the following defines
#define FOSC                (14745600)				// Master Oscillator Freq.
#define PLL_MUL             (4)         			// PLL Multiplier
#define CCLK                (FOSC * PLL_MUL) 		// CPU Clock Freq.

#define CCLK          		(FOSC * PLL_MUL)  		// CPU Clock Freq.
#define FCCO_MAX      		(320000000)       		// Max CC Osc Freq.
#define PLL_DIV       		(FCCO_MAX / (2 * CCLK)) 		// PLL Divider
#define FCCO          		(FOSC * PLL_MUL * 2 * PLL_DIV)	// CC Osc. Freq.

// Pheripheral Bus Speed Divider
#define PBSD                2           			// MUST BE 1, 2, or 4
#define PCLK                (CCLK / PBSD) 			// Pheripheal Bus Clock Freq.

// PLLCON Register Bit Definitions
#define PLLCON_PLLE   		(1 << 0)          		// PLL Enable
#define PLLCON_PLLC   		(1 << 1)          		// PLL Connect

// PLLCFG Register Bit Definitions
#define PLLCFG_MSEL   		((PLL_MUL - 1) << 0) 	// PLL Multiplier
#define PLLCFG_PSEL   		((PLL_DIV - 1) << 5) 	// PLL Divider

// PLLSTAT Register Bit Definitions
#define PLLSTAT_LOCK  		(1 << 10)         		// PLL Lock Status Bit

///////////////////////////////////////////////////////////////////////////////
// VPBDIV defines & computations
#define VPBDIV_VALUE  		(PBSD & 0x03)     		// VPBDIV value

// Do some value range testing
#if ((FOSC < 10000000) || (FOSC > 25000000))
#error Fosc out of range (10MHz-25MHz)
#error correct and recompile
#endif

#if ((CCLK < 10000000) || (CCLK > 60000000))
#error cclk out of range (10MHz-60MHz)
#error correct PLL_MUL and recompile
#endif

#if ((FCCO < 150000000) || (FCCO > 320000000))
#error Fcco out of range (156MHz-320MHz)
#error internal algorithm error
#endif

#if ((PBSD != 1) && (PBSD != 2) && (PBSD != 4))
#error Pheripheal Bus Speed Divider (PBSD) illegal value (1, 2, or 4)
#endif


///////////////////////////////////////////////////////////////////////////////
// WatchDog

// Bits in Watchdog Mode Register
#define WDEN		BIT(0)	
#define WDRESET		BIT(1)
#define WDTOF		BIT(2)
#define WDINT		BIT(3)

//#define WDOG()	WDFEED = 0xAA; WDFEED = 0x55;	// Feed the dog 

// The following are for the Keil MCB-2130 TESTER Board

// **********************************************************
// Port0 Bit Definitions:    Description - initial conditions
// **********************************************************

#define PIO0_LED1     	BIT(8)      // P0.08 OUTPUT - LED1.
#define PIO0_BUTTON2    BIT(9)      // P0.09 INPUT - Button 2.
#define PIO0_LED2     	BIT(10)     // P0.10 OUTPUT - LED2.
#define PIO0_LED3     	BIT(11)     // P0.11 OUTPUT - LED3.
#define PIO0_BUTTON1    BIT(15)     // P0.15 INPUT – Button 1.


#define PIO0_INPUT_BITS      (uint32_t) ( \
                                         PIO0_BUTTON1  | \
										 PIO0_BUTTON2 | \
                                         0 )

#define PIO0_ZERO_BITS       (uint32_t) ( \
										PIO0_LED2 | \
                                         0 )

#define PIO0_ONE_BITS        (uint32_t) ( \
										 PIO0_LED1 | \
										 PIO0_LED3 | \
											0 )

#define PIO0_OUTPUT_BITS     (uint32_t) ( \
										 PIO0_LED1 | \
										 PIO0_LED2 | \
										 PIO0_LED3 | \
                                         0 )
										 
#endif
