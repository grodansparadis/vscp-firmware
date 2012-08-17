/******************************************************************************
 *
 * $RCSfile: config.h,v $
 * $Revision: 1.16 $
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
#define FOSC                (12000000)  			// Master Oscillator Freq.
#define PLL_MUL             (5)         			// PLL Multiplier
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
#define PIO0_TXD0_BIT            	BIT(0)      // P0.00 used by UART0.
#define PIO0_RXD0_BIT            	BIT(1)      // P0.01 used by UART0.
#define PIO0_I2C_SCL      			BIT(2)      // P0.02 I2C SCL
#define PIO0_I2C_SDA   				BIT(3)      // P0.03 I2C SDA.
#define PIO0_LCD_DB4_BIT      		BIT(4)      // P0.04 OUTPUT - LCD DB4.
#define PIO0_LCD_DB5_BIT      		BIT(5)      // P0.05 OUTPUT - LCD DB5.
#define PIO0_LCD_DB6_BIT      		BIT(6)      // P0.06 OUTPUT - LCD DB6.
#define PIO0_LCD_DB7_BIT         	BIT(7)      // P0.07 OUTPUT - LCD DB7.
#define PIO0_TXD1_BIT      			BIT(8)      // P0.08 used by UART1.
#define PIO0_RXD1_BIT      			BIT(9)      // P0.09 used by UART1.
#define PIO0_LCD_RS_BIT      		BIT(10)     // P0.10 OUTPUT - LCD  RS.
#define PIO0_P11_UNUSED_BIT     	BIT(11)     // P0.11 Unused
#define PIO0_LCD_RW_BIT      		BIT(12)     // P0.12 OUTPUT - LCD R/W.
#define PIO0_LCD_E_BIT      		BIT(13)     // P0.13 OUTPUT - LCD E.
#define PIO0_BTN_STATE0_BIT     	BIT(14)     // P0.14 INPUT - Button - State 0.
#define PIO0_ECORE_TEMP_BIT      	BIT(15)     // P0.15 INPUT – Electrical water heater core temperature sensor.
#define PIO0_OVER_TEMP_BIT       	BIT(16)     // P0.16 INPUT - Over temp sensor.
#define PIO0_FURNACE_TEMP_BIT       BIT(17)     // P0.17 INPUT - Furnace Temperature
#define PIO0_BTN_PG_FORWARD_BIT     BIT(18)     // P0.18 INPUT - Button - Display page forward.
#define PIO0_BTN_UP_BIT   			BIT(19)     // P0.19 INPUT - Button - Up.
#define PIO0_BTN_DOWN_BIT    		BIT(20)     // P0.20 INPUT - Button - Down.
#define PIO0_BTN_ENTER_BIT  		BIT(21)     // P0.21 INPUT - Button - Enter.
#define PIO0_BTN_FORCE_MON_BIT    	BIT(22)     // P0.22 INPUT - Button - Force transport motors on.
#define PIO0_P23_UNUSED_BIT      	BIT(23)     // P0.23 CAN LPC 212x.
#define PIO0_P24_UNUSED_BIT      	BIT(24)     // P0.24 CAN LPC 212x.
#define PIO0_CANRX0_BIT      		BIT(25)     // P0.25 AOUT - Sound   - CANRX0 on LPC 212x.
#define PIO0_CANTX0_BIT      		BIT(26)     // P0.26 CAN TX0 on LPC 212x.
#define PIO0_STOVE_FIRE_DETECT_BIT  BIT(27)     // P0.27 ANALOG INPUT - Stove Fire Detector. 
#define PIO0_STOVE_FIRE_LEVEL_BIT   BIT(28)     // P0.28 AVIN - POT (Sensivity for fire detector)
#define PIO0_P29_UNUSED_BIT      	BIT(29)     // P0.29 unused - low output
#define PIO0_P30_UNUSED_BIT			BIT(30)     // P0.30 unused - low output
#define PIO0_P31_UNUSED_BIT      	BIT(31)     // P0.31 unused - low output

#define PIO0_INPUT_BITS      (uint32_t) ( \
                                         PIO0_OVER_TEMP_BIT  | \
										 PIO0_ECORE_TEMP_BIT | \
										 PIO0_FURNACE_TEMP_BIT | \
										 PIO0_BTN_STATE0_BIT | \
										 PIO0_BTN_PG_FORWARD_BIT | \
										 PIO0_BTN_FORCE_MON_BIT | \
										 PIO0_BTN_PREV_BIT | \
										 PIO0_BTN_NEXT_BIT | \
										 PIO0_BTN_DOWN_BIT | \
										 PIO0_BTN_UP_BIT | \
										 PIO0_STOVE_FIRE_LEVEL_BIT | \
										 PIO0_STOVE_FIRE_DETECT_BIT | \
										 PIO0_SMOKE_SENSOR | \
                                         0 )

#define PIO0_ZERO_BITS       (uint32_t) ( \
										PIO0_LCD_DB4_BIT | \
										PIO0_LCD_DB5_BIT | \
										PIO0_LCD_DB6_BIT | \
										PIO0_LCD_DB7_BIT | \
										PIO0_LCD_RS_BIT  | \
										PIO0_LCD_RW_BIT  | \
										PIO0_LCD_E_BIT   | \
										PIO0_P11_UNUSED_BIT | \
										PIO0_P23_UNUSED_BIT | \
										PIO0_P24_UNUSED_BIT | \
										PIO0_P29_UNUSED_BIT | \
										PIO0_P30_UNUSED_BIT | \
										PIO0_P31_UNUSED_BIT | \
                                         0 )

#define PIO0_ONE_BITS        (uint32_t) ( \
										PIO0_I2C_SCL 	 | \
										PIO0_I2C_SDA 	 | \
											0 )

#define PIO0_OUTPUT_BITS     (uint32_t) ( \
										 PIO0_LCD_DB4_BIT | \
										 PIO0_LCD_DB5_BIT | \
										 PIO0_LCD_DB6_BIT | \
										 PIO0_LCD_DB7_BIT | \
										 PIO0_LCD_RS_BIT  | \
										 PIO0_LCD_RW_BIT  | \
										 PIO0_LCD_E_BIT   | \
										 PIO0_I2C_SCL 	  | \
										 PIO0_I2C_SDA 	  | \
                                         0 )
										 
										 
// **********************************************************
// Port1 Bit Definitions:    Description - initial conditions
// **********************************************************
#define PIO1_TR_MOTOR_A_BIT      	BIT(16)     // P1.16 OUT - Transport Motor A relay.
#define PIO1_TR_MOTOR_B_BIT      	BIT(17)     // P1.17 OUT - Transport Motor B relay.
#define PIO1_FAN_BIT      			BIT(18)     // P1.18 OUT - Fan relay.
#define PIO1_FURNACE_HEATER_BIT     BIT(19)     // P1.19 OUT - Heater element relay.
#define PIO1_STATUS_LED      		BIT(20)     // P1.20 OUT - Status LED
#define PIO1_ALARM_BIT      		BIT(21)     // P1.21 OUT – Alarm.
#define PIO1_EHEATER_ELEMENTA_BIT   BIT(22)     // P1.22 OUT - Electric water heater element A
#define PIO1_EHEATER_ELEMENTB_BIT   BIT(23)     // P1.23 OUT - Electric water heater element B

#define PIO1_P24_UNUSED_BIT      	BIT(24)     // P1.24 JTAG
#define PIO1_P25_UNUSED_BIT      	BIT(25)     // P1.25 JTAG
#define PIO1_P26_UNUSED_BIT      	BIT(26)     // P1.26 JTAG
#define PIO1_P27_UNUSED_BIT      	BIT(27)     // P1.27 JTAG
#define PIO1_P28_UNUSED_BIT      	BIT(28)     // P1.28 JTAG
#define PIO1_P29_UNUSED_BIT      	BIT(29)     // P1.29 JTAG
#define PIO1_P30_UNUSED_BIT      	BIT(30)     // P1.30 JTAG
#define PIO1_P31_UNUSED_BIT      	BIT(31)     // P1.31 JTAG

#define PIO1_INPUT_BITS      (uint32_t) ( \
                                         0 )

#define PIO1_ZERO_BITS       (uint32_t) ( \
                                         PIO1_TR_MOTOR_A_BIT | \
										 PIO1_TR_MOTOR_B_BIT | \
										 PIO1_FAN_BIT | \
										 PIO1_FURNACE_HEATER_BIT | \
										 PIO1_ALARM_BIT | \
										 PIO1_EHEATER_ELEMENTA_BIT | \
										 PIO1_EHEATER_ELEMENTB_BIT | \
										 PIO1_STATUS_LED |\
                                         0 )

#define PIO1_ONE_BITS        (uint32_t) ( \
                                         0 )

#define PIO1_OUTPUT_BITS     (uint32_t) ( \
                                         PIO1_TR_MOTOR_A_BIT | \
										 PIO1_TR_MOTOR_B_BIT | \
										 PIO1_FAN_BIT | \
										 PIO1_FURNACE_HEATER_BIT | \
										 PIO1_ALARM_BIT | \
										 PIO1_EHEATER_ELEMENTA_BIT | \
										 PIO1_EHEATER_ELEMENTB_BIT | \
										 PIO1_STATUS_LED |\
                                         0 )

#endif
