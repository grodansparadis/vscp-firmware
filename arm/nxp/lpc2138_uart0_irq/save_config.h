/******************************************************************************
 *
 * $RCSfile: save_config.h,v $
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
#include "lpc213x.h"

// some handy DEFINES
#ifndef FALSE
#define FALSE               0
#ifndef TRUE
#define TRUE                !FALSE
#endif
#endif

#ifndef BIT
#define BIT(n)              (1 << (n))
#endif

// declare functions and values from crt0.S & the linker control file
extern void reset(void);
// extern void exit(void);
extern void abort(void);
// maybe add interrupt vector addresses

#define HOST_BAUD           (57600)

#define WDOG()

///////////////////////////////////////////////////////////////////////////////
// MAM defines
#define MAMCR_OFF     0
#define MAMCR_PART    1
#define MAMCR_FULL    2

#define MAMTIM_CYCLES (((CCLK) + 19999999) / 20000000)

///////////////////////////////////////////////////////////////////////////////
// MEMMAP defines
#define MEMMAP_BBLK   0                 // Interrupt Vectors in Boot Block
#define MEMMAP_FLASH  1                 // Interrupt Vectors in Flash
#define MEMMAP_SRAM   2                 // Interrupt Vectors in SRAM

// PLL setup values are computed within the LPC include file
// It relies upon the following defines
#define FOSC                (12000000)  		// Master Oscillator Freq.
#define PLL_MUL             (5)         		// PLL Multiplier
#define CCLK                (FOSC * PLL_MUL) 	// CPU Clock Freq.

#define CCLK          		(FOSC * PLL_MUL)  	// CPU Clock Freq.
#define FCCO_MAX      		(320000000)       	// Max CC Osc Freq.
#define PLL_DIV       		(FCCO_MAX / (2 * CCLK)) // PLL Divider
#define FCCO          		(FOSC * PLL_MUL * 2 * PLL_DIV) // CC Osc. Freq.

// Pheripheral Bus Speed Divider
#define PBSD                2           // MUST BE 1, 2, or 4
#define PCLK                (CCLK / PBSD) // Pheripheal Bus Clock Freq.

// PLLCON Register Bit Definitions
#define PLLCON_PLLE   (1 << 0)          // PLL Enable
#define PLLCON_PLLC   (1 << 1)          // PLL Connect

// PLLCFG Register Bit Definitions
#define PLLCFG_MSEL   ((PLL_MUL - 1) << 0) // PLL Multiplier
#define PLLCFG_PSEL   ((PLL_DIV - 1) << 5) // PLL Divider

// PLLSTAT Register Bit Definitions
#define PLLSTAT_LOCK  (1 << 10)         // PLL Lock Status Bit

///////////////////////////////////////////////////////////////////////////////
// VPBDIV defines & computations
#define VPBDIV_VALUE  (PBSD & 0x03)     // VPBDIV value

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

// The following are for the Keil MCB-2130 TESTER Board
// Port Bit Definitions & Macros:    Description - initial conditions
#define TXD0_BIT            BIT(0)      // used by UART0
#define RXD0_BIT            BIT(1)      // used by UART0
#define P02_UNUSED_BIT      BIT(2)      // P0.02 unused - low output
#define P03_UNUSED_BIT      BIT(3)      // P0.03 unused - low output
#define P04_UNUSED_BIT      BIT(4)      // P0.04 unused - low output
#define P05_UNUSED_BIT      BIT(5)      // P0.05 unused - low output
#define P06_UNUSED_BIT      BIT(6)      // P0.06 unused - low output
#define LED1_BIT            BIT(7)      // LED 1 - active low output
#define P08_UNUSED_BIT      BIT(8)      // P0.08 unused - low output
#define P09_UNUSED_BIT      BIT(9)      // P0.09 unused - low output
#define P10_UNUSED_BIT      BIT(10)     // P0.10 unused - low output
#define P11_UNUSED_BIT      BIT(11)     // P0.11 unused - low output
#define P12_UNUSED_BIT      BIT(12)     // P0.12 unused - low output
#define P13_UNUSED_BIT      BIT(13)     // P0.13 unused - low output
#define P14_UNUSED_BIT      BIT(14)     // P0.14 unused - low output
#define P15_UNUSED_BIT      BIT(15)     // P0.15 unused - low output
#define P16_UNUSED_BIT      BIT(16)     // P0.16 unused - low output
#define P17_UNUSED_BIT      BIT(17)     // used by JTAG
#define P18_UNUSED_BIT      BIT(18)     // used by JTAG
#define P19_UNUSED_BIT      BIT(19)     // used by JTAG
#define P20_UNUSED_BIT      BIT(20)     // used by JTAG
#define P21_UNUSED_BIT      BIT(21)     // used by JTAG
#define P22_UNUSED_BIT      BIT(22)     // P0.22 unused - low output
#define P23_UNUSED_BIT      BIT(23)     // P0.23 unused - low output
#define P24_UNUSED_BIT      BIT(24)     // P0.24 unused - low output
#define P25_UNUSED_BIT      BIT(25)     // P0.25 unused - low output
#define P26_UNUSED_BIT      BIT(26)     // P0.26 unused - low output
#define P27_UNUSED_BIT      BIT(27)     // P0.27 unused - low output
#define P28_UNUSED_BIT      BIT(28)     // P0.28 unused - low output
#define P29_UNUSED_BIT      BIT(29)     // P0.29 unused - low output
#define P30_UNUSED_BIT      BIT(30)     // P0.30 unused - low output
#define SW1_BIT             BIT(31)     // Switch 1 - active low input

#define PIO_INPUT_BITS      (uint32_t) ( \
                                         SW1_BIT | \
                                         0 )

#define PIO_ZERO_BITS       (uint32_t) ( \
                                         P02_UNUSED_BIT | \
                                         P03_UNUSED_BIT | \
                                         P04_UNUSED_BIT | \
                                         P05_UNUSED_BIT | \
                                         P06_UNUSED_BIT | \
                                         P08_UNUSED_BIT | \
                                         P09_UNUSED_BIT | \
										 P10_UNUSED_BIT | \
										 P11_UNUSED_BIT | \
										 P12_UNUSED_BIT | \
										 P13_UNUSED_BIT | \
										 P14_UNUSED_BIT | \
										 P15_UNUSED_BIT | \
										 P16_UNUSED_BIT | \
                                         P22_UNUSED_BIT | \
                                         P23_UNUSED_BIT | \
                                         P24_UNUSED_BIT | \
                                         P25_UNUSED_BIT | \
                                         P26_UNUSED_BIT | \
                                         P27_UNUSED_BIT | \
                                         P28_UNUSED_BIT | \
                                         P29_UNUSED_BIT | \
                                         P30_UNUSED_BIT | \
                                         0 )

#define PIO_ONE_BITS        (uint32_t) ( \
                                         LED1_BIT | \
                                         0 )

#define PIO_OUTPUT_BITS     (uint32_t) ( \
                                         PIO_ZERO_BITS | \
                                         PIO_ONE_BITS )

#endif
