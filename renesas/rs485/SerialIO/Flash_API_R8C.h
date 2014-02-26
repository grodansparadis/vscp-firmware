/******************************************************************************
* DISCLAIMER:
* The software supplied by Renesas Technology Europe Ltd is
* intended and supplied for use on Renesas Technology products.
* This software is owned by Renesas Technology Europe, Ltd. Or
* Renesas Technology Corporation and is protected under applicable
* copyright laws. All rights are reserved.
*
* THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS,
* IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* APPLY TO THIS SOFTWARE. RENESAS TECHNOLOGY AMERICA, INC. AND
* AND RENESAS TECHNOLOGY CORPORATION RESERVE THE RIGHT, WITHOUT
* NOTICE, TO MAKE CHANGES TO THIS SOFTWARE. NEITHER RENESAS
* TECHNOLOGY AMERICA, INC. NOR RENESAS TECHNOLOGY CORPORATION SHALL,
* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
* CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER ARISING OUT OF THE
* USE OR APPLICATION OF THIS SOFTWARE.
******************************************************************************/

/* Copyright (C) 2010. Renesas Technology Europe, All Rights Reserved */

/*””FILE COMMENT””******************************* Technical reference data ****
* File Name		: Flash_API_R8C.h
* Version 		: 1.00
* Device 		: R8C/L3AC (R5F2L3ACCNFP)
* Tool Chain 	: HEW, Renesas M16C Standard Toolchain v5.45
* H/W Platform	: RSKR8CL3AC
* Description 	: Header file for Flash_API_R8C.c 
******************************************************************************/

/******************************************************************************
* History 		: 15.02.2010 Ver. 1.00 First Release
*””FILE COMMENT END””*********************************************************/

#ifndef FLASH_API_R8C_H
#define FLASH_API_R8C_H

/******************************************************************************
Macro Defines
*******************************************************************************/
/* Parameter Types */
#define FLASH_PTR_TYPE 		unsigned char far *

/* Can be changed to "near" if buffers will only be in RAM */
#define BUF_PTR_TYPE   		unsigned char far *    

/* Size of data to be written at once	*/
#define LOCAL_BUFFER_SIZE 	8

/********************************
 * SPECIFY YOUR CLOCK SPEED     *
 ********************************
 * You need to specify the speed of your system bus clock (BCLK)
 * so that the correct programming speed can be calculated
 * for communicating to the flash controller
 * The value should be in Hertz
 * Alternatively, you could define your Clock Speed in your Hew project settings
 *****************************************************************************/
#define BCLK_FREQUENCY 20000000

/********************************
 * SELECT EW0 or EW1 Flash MODE *
 ********************************
 * You may select to use Erase-Write Mode 0 or Erase-Write Mode 1
 * for sending commands to the flash sequencer. Note, EW0 mode
 * executes the flash commands to RAM and waits for the flash
 * operation to complete, where EW1 mode sends commands while
 * executing out of flash, but the MCU going into a hold state till
 * the flash operation is complete. 
 * It is recommended to use use EW1 mode.
 *
 * Alternatively, you could define this in your HEW project settings
 * Select only one.
 *****************************************************************************/

/* #define EW_MODE 0 */
/*    or	*/
#define EW_MODE 1

/*******************
 * SELECT YOUR MCU *
 *******************              
 * Select your desired device by removing the comment markers
 * from the beginning of the line. 
 * Alternatively, you could define your MCU in your Hew project settings
 * Select only one.
 *****************************************************************************/
 #define R8C_L3AC  1

/*********************************************
 * SPECIFY EW0 MODE RAM CODE SIZE/ALLOCATION *
 *********************************************
 * If using Erase-Write Mode 0, code will need to be executed out
 * of RAM when sending commands to the Flash controller. The following
 * lets you specify the size and allocation of that RAM area. */
/*******************************************************************************/
/* MAX_RAM_CODE_SIZE - This is the size in bytesof the RAM area that will be 
 *                     allocated and used for both the Erase and Write routines.
 *                     Note that when determining this size, the FlashWrite 
 *                     routine will require more space. */
#define RAM_CODE_SIZE 320

/* STATIC_RAM_CODE - With this option, you can specify if you want the RAM
 *                   code area to be statically allocated, or allocated on
 *                   the stack when an Erase or Write function is called.
 *                   A value of '1' will allocate RAM as a static array.
 *                   A value of '0' will dynamically allocate RAM on the stack. 
 */
#define STATIC_RAM_CODE 1

/* !!!!! DO NOT MODIFY BELOW THIS LINE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#if !defined(BCLK_FREQUENCY)
 #if (EW_MODE == 1)
  #if defined(R8C_10) || defined(R8C_11)
    "ERROR !!! You must specify the Clock Frequency (BCLK_FREQUENCY) in Flash_API_R8C.h first !!!"
  #else
    /* Only need to slow MCU for EW1 mode for R8C/10 and R8C/11 */
  #endif
 #else
   "ERROR !!! You must specify the Clock Frequency (BCLK_FREQUENCY) in Flash_API_R8C.h first !!!"
 #endif
#endif

#if !defined (EW_MODE)
 "ERROR !!! You must specify the Flash Mode (EW_MODE = 0 or 1) in Flash_API_R8C.h first !!!"
#endif
/*****************************************************************************/

/* ======== #define values ==========
  R8C_FLASH values
  1: R8C/1X series
  2: R8C/2X series
  3: R8C/2X series with extended flash above 64KB
  4: R8C/3X series with extended flash above 64KB
*/

/* Define device settings for R8CL3AC */
#if defined(R8C_L3AC)
 	#define R8C_FLASH 4
	/*  Following header file provides a structure to access on-chip I/O 
    registers. */
 	#include "sfr_r8m12a.h"
#else
 	#error "You must specify your device in Flash_API_R8C.h first!!"
#endif

/* Check on the Flash Mode setting */
   
#ifndef EW_MODE
 	#error "You must specify the Flash Mode (EW_MODE = 0 or 1) in Flash_API_R8C.h first!!"
#endif

/* Flash Erase selection as per the Flash mode set.
   Erases an entire flash block */
#if (EW_MODE == 0)
	#define FlashErase FlashEraseEW0
 	#define FlashWrite FlashWriteEW0
#elif (EW_MODE == 1)
 	#define FlashErase FlashEraseEW1
 	#define FlashWrite FlashWriteEW1
#else
 	#error "You must specify the Flash Mode (EW_MODE = 0 or 1) in Flash_API_R8C.h first!!"
#endif

/* Slowing down the MCU speed is only needed for EW0 mode. Except, R8C/10
 * and R8C/11 need them for EW1 Mode as well. */

#ifndef INCLUDE_SLOW_MCU
  	#if (EW_MODE == 0)
    	/* Must run under 5 MHz for EW0 Mode */
		#define INCLUDE_SLOW_MCU 1		
  	#else
    	/* Don't need to slow for EM1 Mode */
		#define INCLUDE_SLOW_MCU 0		
  	#endif
#endif

#ifndef	DISABLE_INTERRUPTS
/* This is used to turn off the maskable interrupts */
#define 	DISABLE_INTERRUPTS 	{ _asm("STC FLG,_flags_reg"); _asm("FCLR I");}
#endif

#ifndef	RESTORE_INTERRUPTS
/* This is used to restore I flag to previous setting */
#define 	RESTORE_INTERRUPTS 	{ _asm("LDC _flags_reg,FLG"); }
#endif

/* This is used to write the first command word of a flash command to */
#define FLASH_COMMAND_ADDRESS (*(FLASH_PTR_TYPE)0xFFFFF800)

/******************************************************************************
Function Prototypes
******************************************************************************/
unsigned char FlashErase( unsigned char block );
unsigned char FlashWrite( 	FLASH_PTR_TYPE 	flash_addr,
              				BUF_PTR_TYPE 	buffer_addr,
              				unsigned short 	bytes);

/******************************************************************************
Define Memory map                                           
******************************************************************************/
#if (R8C_FLASH == 4)

	#if defined(R8C_L3AC)
		/* User Block Area */
		/* 8KB:  0E000 - 0FFFF */
		#define BLOCK_0 0   
		/* 8KB:  0C000 - 0DFFF */
		#define BLOCK_2 1   
		/* 32KB: 04000 - 0BFFF */
		#define BLOCK_3 2   
		/* 8KB:  10000 - 11FFF */
		#define BLOCK_5 3   
		/* 8KB:  12000 - 13FFF */
		#define BLOCK_6 4   
		/* 32KB: 14000 - 1BFFF */
		#define BLOCK_7 5   
		/* 32KB: 1C000 - 23FFF */
		#define BLOCK_8 6   
	
		/* 1KB x 4 Data Block Area */
		/* 1KB: 3000 - 33FF */
	    #define BLOCK_A 7
		/* 1KB: 3400 - 37FF */		
	    #define BLOCK_B 8		
		/* 1KB: 3800 - 3BFF */
	    #define BLOCK_C 9		
		/* 1KB: 3C00 - 3FFF */
	    #define BLOCK_D 10		

		/* Defines an array of highest even addresses for each block */
		#define DEFINE_ADDR_LOOKUP const _near unsigned long block_addresses[11] =\
			{0x0FFFFu, 0x0DFFFu, 0x0BFFFu, 0x11FFFu, 0x13FFFu, 0x1BFFFu, 0x23FFFu, \
           	0x33FFu, 0x37FFu, 0x3BFFu, 0x3FFFu};
		
    #endif
                                                                                	       
/* (R8C_FLASH == 4) */
#endif 

/* FLASH_API_R8C_H */
#endif
