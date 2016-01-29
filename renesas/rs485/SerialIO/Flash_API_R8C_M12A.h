/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/******************************************************************************
* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved.   */
/******************************************************************************
* File Name		: Flash_API_R8C_M12A.h
* Version 		: 1.00
* Device 		: R5F2M122ANSP
* Tool Chain 	: R8C Family C Compiler
* H/W Platform	: RPBR8CM12A
* Description   : Header file for CPU Re-write functions for R8C.
******************************************************************************/
/******************************************************************************
* History 		: 17.08.2010 Ver. 1.00 First Release(API ported from R8C35C 
*                                      Data flash API Rev-1.3) 
******************************************************************************/
/******************************************************************************
User Includes (Project level includes)
******************************************************************************/
/* Header file for C99 Types */
#include "inttypes.h"

#ifndef _FLASH_API_H
#define _FLASH_API_H

/* [ Availible Functions for R8C ]
unsigned char FlashErase( unsigned char block );
unsigned char FlashWrite( FLASH_PTR_TYPE  flash_addr,
                          BUF_PTR_TYPE    buffer_addr,
                          unsigned int    bytes);

    NOTE: "FLASH_PTR_TYPE" and "BUF_PTR_TYPE" are determined by MCU selection
*/

/********************************
 * SPECIFY YOUR CLOCK SPEED     *
 ********************************
 * You need to specify the speed of your system bus clock (BCLK)
 * so that the correct programming speed can be calculated
 * for communicating to the flash controller
 * The value should be in Hertz
 * Alternatively, you could define your Clock Speed in your Hew project settings
 *________________________________________________________________________*/
#define BCLK_FREQUENCY 18432000


/********************************
 * SELECT EW0 or EW1 Flash MODE *
 ********************************
 * You may select to use Erase-Write Mode 0 or Erase-Write Mode 1
 * for sending commands to the flash sequencer. Note EW0 mode
 * executes the flash commands to RAM and waits for the flash
 * operation to complete, where EW1 mode sends commands while
 * executing out of flash, but the MCU going into a hold state till
 * the flash operation is complete. 
 * It is recomended to use use EW1 mode.
 *
 * Alternatively, you could define this in your Hew project settings
 * Select only one.
 *________________________________________________________________________*/
//#define EW_MODE 0		/* OR */
#define EW_MODE 1

/********************************
 * SELECT YOUR MCU              *
 ********************************
 * Select your desired device by removing the comment markers
 * from the beginging of the line. 
 * Alternatively, you could define your MCU in your Hew project settings
 * Select only one.
 *________________________________________________________________________*/
#define R8C_M12A  1	/* Also determine "TOTAL_FLASH_SIZE" below */

/*********************************************
 * TOTAL_FLASH_SIZE               *
 *********************************************
 * Enter the total flash size in KB (not including any Data Flash A,B)
 *_______________________________________________________________________*/
 /*
  PartNo.     	   Flash     DataFlash      RAM
  ==============================================
  R5F2M120ANSP    2 Kbytes  1 Kbyte × 2    256 bytes 
  R5F2M121ANSP    4 Kbytes  1 Kbyte × 2    384 bytes 
  R5F2M122ANSP    8 Kbytes  1 Kbyte × 2    512 bytes 
  R5F2M120ANDD    2 Kbytes  1 Kbyte × 2    256 bytes 
  R5F2M121ANDD    4 Kbytes  1 Kbyte × 2    384 bytes 
  R5F2M122ANDD    8 Kbytes  1 Kbyte × 2    512 bytes 
  R5F2M120ADSP    2 Kbytes  1 Kbyte × 2    256 bytes 
  R5F2M121ADSP    4 Kbytes  1 Kbyte × 2    384 bytes
  R5F2M122ADSP    8 Kbytes  1 Kbyte × 2    512 bytes
*/
 #define TOTAL_FLASH_SIZE 8

/*********************************************
 * SPECIFY EW0 MODE RAM CODE SIZE/ALLOCATION *
 *********************************************
 * If using Erase-Write Mode 0, code will need to be executed out
 * of RAM when sending commands to the Flash controller. The following
 * lets you specify the size and allocation of that RAM area.
 *________________________________________________________________________
 * MAX_RAM_CODE_SIZE - This is the size in bytesof the RAM area that will be 
 *                     allocated and used for both the Erase and Write routines.
 *                     Note that when determining this size, the FlashWrite 
 *                     routine will require more space. */
#define RAM_CODE_SIZE 100

/* STATIC_RAM_CODE - With this option, you can specify if you want the RAM
 *                   code area to be statically allocated, or allocated on
 *                   the stack when an Erase or Write function is called.
 *                   A value of '1' will allocate RAM as a static array.
 *                   A value of '0' will dynamically allocate RAM on the stack. */
#define STATIC_RAM_CODE 1




/* !!!!!!!!!!!!!!!!! DO NOT MODIFY BELOW THIS LINE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


#if !defined(BCLK_FREQUENCY)
   "ERROR !!! You must specify the Clock Frequency (BCLK_FREQUENCY) in Flash_API_R8C.h first !!!"
 #endif

#if !defined (EW_MODE)
 "ERROR !!! You must specify the Flash Mode (EW_MODE = 0 or 1) in Flash_API_R8C.h first !!!"
#endif



/* ======== #define values ==========*/
/*
 R8C_FLASH values
  1: R8C/M12A series
*/
#if defined(R8C_M12A)
 #define R8C_FLASH 1
#else
 #error "!!! You must specify your device in Flash_API_R8C.h first !!!"
#endif

#if !defined(EW_MODE)
 #error "!!! You must specify the Flash Mode (EW_MODE = 0 or 1) in Flash_API_R8C.h first !!!"
#endif

#if   (EW_MODE == 0)
 #define FlashErase FlashEraseEW0
 #define FlashWrite FlashWriteEW0
#elif (EW_MODE == 1)
 #define FlashErase FlashEraseEW1
 #define FlashWrite FlashWriteEW1
#endif

/***************************************************************/
/* R8C/M12A          */
/***************************************************************/
#if (R8C_FLASH == 1)

  /* These parts may or may not be under 64Kbytes of total Flash */
  #if !defined(TOTAL_FLASH_SIZE)
    /* Set to large model just to be safe	*/
	#define TOTAL_FLASH_SIZE 8	
  #endif

  #if (TOTAL_FLASH_SIZE < 64)
    /* Set the length of the pointer into flash and of the source location */
    #define FLASH_PTR_TYPE uint8_t near *
    #define   BUF_PTR_TYPE uint8_t near *
  #else
    /* Set the length of the pointer into flash and of the source location */
    /* Has to be "far" to reach above 0xFFFF */
	#define FLASH_PTR_TYPE uint8_t far *          
    #if !defined(BUF_PTR_TYPE) 
      /* Could be changed to "near" if buffer is alwasy in RAM */
	  #define   BUF_PTR_TYPE uint8_t far *        
    #endif
  #endif

  #if defined(R8C_M12A)
    /* User Block Area */
    #define BLOCK_1 0		/*   4KB: F000 - FFFF */
    #define BLOCK_2 1		/*   4KB: E000 - EFFF */


	/* 1KB x 2 Data Block Area */
    #define BLOCK_A 2		/*   1KB: 3000 - 33FF */
    #define BLOCK_B 3		/*   1KB: 3400 - 37FF */


    /* Defines an array of highest addresses for each block */
    #define DEFINE_ADDR_LOOKUP const uint32_t block_addresses[4] = \
			{0x0FFFF,0x0EFFF,0x33FF,0x37FF};
  #endif
#endif /* R8C_FLASH == 1 */


/* Function Prototypes */
uint8_t FlashErase( uint8_t block );

uint8_t FlashWrite(	FLASH_PTR_TYPE  flash_addr,
							BUF_PTR_TYPE    buffer_addr,
							uint16_t    bytes);

#endif /* _FLASH_API_H (END OF FILE) */
