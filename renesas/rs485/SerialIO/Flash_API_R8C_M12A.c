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
* File Name		: Flash_API_R8C_M12A.c
* Version 		: 1.00
* Device 		: R5F2M122ANSP
* Tool Chain 	: R8C Family C Compiler
* H/W Platform	: RPBR8CM12A
* Description   : CPU Re-write functions for R8C.
******************************************************************************/
/******************************************************************************
* History 		: 17.08.2010 Ver. 1.00 First Release (API ported from R8C35C 
*                                      Data flash API Rev-1.3)
******************************************************************************/
/******************************************************************************
User Includes (Project level includes)
******************************************************************************/
/* Defines FLASH API functions */
#include "Flash_API_R8C_M12A.h"
/* Header file for C99 Types */
//#include "C99Types.h"

/* Declare SFR bit structures */
struct bit_def
{
	uint8_t b0:1; uint8_t b1:1; uint8_t b2:1; uint8_t b3:1; uint8_t b4:1; uint8_t b5:1; uint8_t b6:1; uint8_t b7:1;
};
union byte_def
{
	struct bit_def	bit;
	uint8_t			byte;
};

/*********************************/
/* R8C/M12A Series Flash Registers */
/*********************************/
#if R8C_FLASH == 1
  /* Flash Memory Control Register 0 */
  #pragma ADDRESS	fmr0_addr	01AAH	
  /* Flash Memory Control Register 1 */
  #pragma ADDRESS	fmr1_addr	01ABH	
  /* Flash Memory Status Register 1 */
  #pragma ADDRESS	fst_addr	01A9H	

  /* Flash Memory Status Register (FST) */
  union byte_def fst_addr;
  #define	fst		fst_addr.byte
  /* Flash ready status interrupt request flag */
  #define	rdysti	fst_addr.bit.b0	
  /* Flash access error interrupt request flag */
  #define	bsyaei	fst_addr.bit.b1	
  /* LBDATA monitor flag */
  #define	fst2	fst_addr.bit.b2	
  /* Program error status flag */
  #define	fst4	fst_addr.bit.b4	
  /* Erase error status flag */
  #define	fst5	fst_addr.bit.b5	
  /* Erase-suspend status flag */
  #define	fst6	fst_addr.bit.b6	
  /* Ready/busy status flag */
  #define	fst7	fst_addr.bit.b7	
  
  /* Flash Memory Control Register 0 */
  union byte_def fmr0_addr;
  #define	fmr0	fmr0_addr.byte
  /* CPU rewrite mode select bit */
  #define	fmr01	fmr0_addr.bit.b1	
  /* EW1 mode select bit */
  #define	fmr02	fmr0_addr.bit.b2	
  /* Flash memory stop bit */
  #define	fmstp	fmr0_addr.bit.b3	
  /* Erase/write sequence reset bit */
  #define	cmdrst	fmr0_addr.bit.b4	
  /* Erase/write error interrupt enable bit */
  #define	cmderie	fmr0_addr.bit.b5	
  /* Flash access error interrupt enable bit */
  #define	bsyaeie	fmr0_addr.bit.b6	
  /* Flash ready status interrupt enable bit */
  #define	rdystie	fmr0_addr.bit.b7	

  /* Flash Memory Control Register 1 */
  union byte_def fmr1_addr;
  #define	fmr1	fmr1_addr.byte
  /* Flash memory stop bit in wait mode */
  #define	wtfmstp	fmr1_addr.bit.b2
  /* Lock bit disable select bit */
  #define	fmr13	fmr1_addr.bit.b3	
  /* Data flash block A rewrite */
  #define	fmr16	fmr1_addr.bit.b6	
  /* Data flash block B rewrite */
  #define	fmr17	fmr1_addr.bit.b7	
	
#endif

/* This macro has been define in the h file and will create an array called block_addresses */
/* This array is made static in order for it to not show up in the debugger */
static DEFINE_ADDR_LOOKUP;

/******************************************************************************
Macro definitions
******************************************************************************/

#define DISABLE_INTERRUPTS { _asm("STC FLG,_flags_reg"); _asm("FCLR I");}
#define RESTORE_INTERRUPTS { _asm("LDC _flags_reg,FLG"); }

/******************************************************************************
Global variables and functions private to the file
******************************************************************************/
/* For saving the Flags register (must be in near RAM) */
static uint16_t near flags_reg;	    
#if (EW_MODE == 0)
/* This will server as a pointer to the code to use */
/* Note: 3 bytes needed for jmpi instruction */
static int32_t EW0_RAM_CODE_ptr;	

/* For checking if we have enough RAM to hold our re-write code */
uint8_t reflash_code_size; 
  #if (STATIC_RAM_CODE == 1)
  /* Allocate RAM space statically to hold the CPU-Rewrite code. */
  static uint8_t near EW0_RAM_CODE[ RAM_CODE_SIZE ];
  #endif /* (STATIC_RAM_CODE == 1) */
#endif /* (EW_MODE == 0) */ 

/******************************************************************************
* Outline 		: FlashEraseEW0 (Should be called using the name FlashErase)
* Description 	: Erases an entire flash block.
* Argument  	: block
*			      The block number to erase (BLOCK_0, BLOCK_1, etc...)
* Return value  : 		
*		0 = Erase Successful
*		1 = Erase error reported by flash control
*		2 = Command Sequence Error (locked block, incorrect command, etc)
*       3 = Not enough RAM space allocated for re-write code
******************************************************************************/

#if (EW_MODE == 0)
uint8_t FlashEraseEW0( uint8_t block )
{
	volatile FLASH_PTR_TYPE flash_addr;
    uint8_t i, ret_value;

#if (STATIC_RAM_CODE == 0)
	/* Allocate RAM space to hold the CPU-Rewrite code on the stack. */
	uint8_t EW0_RAM_CODE[ RAM_CODE_SIZE ];
#endif

    /* Check if the RAM space is big enough to hold our re-write code */
   	asm("mov.w #((ERASE_CODE_END-ERASE_CODE_START+1)), _reflash_code_size");
    if( RAM_CODE_SIZE < reflash_code_size )
    {
        return 3;
    }

	/* Get highest even block address*/
	flash_addr = (FLASH_PTR_TYPE) block_addresses[ block ];

	/* Turn off maskable interrupts*/
	DISABLE_INTERRUPTS;	

	/* Specify to the assemble code below the RAM code address */
	EW0_RAM_CODE_ptr = (int32_t) EW0_RAM_CODE;

	/* Copy code to RAM using SMOVF instruction (faster than a loop in 'C') */
	/* Can use debugger to determine how big your RAM space needs to be */
	/* R8C and M16C */
#pragma ASM
	pushm	R1,R3,A0,A1
	/* transfer count (number of words ) */
	mov.w	#((ERASE_CODE_END-ERASE_CODE_START+1) / 2 ),R3	;
	/* source address */
	mov.b	#((ERASE_CODE_START >> 16) & 0FFh),R1H	;
	/* source address */
	mov.w	#(ERASE_CODE_START & 0FFFFh),A0			;
	/* destination address (stored in EW0_ARM_CODE_ptr)  */
	mov.w	_EW0_RAM_CODE_ptr,A1					;
	/* copy 16 bits at a time */
	smovf.w											;
	popm	R1,R3,A0,A1
#pragma ENDASM

	/* Set EW0 select bit */
	fmr01 = 0;
	fmr01 = 1;			

	/* !! DISABLE ALL FLASH MEMORY LOCK AND PROTECT FUNCTIONALITY !!
	 * NOTE: In order to use lock/protect bit functionality, please refer to
	 * the Flash Memory section in the data sheet for your specific device.
	 * Also note the that following bits clear back to 0 every time you enter
	 * CPU Rewrite Mode.
	 * Note that for some MCUs, if the lock bit is disabled, the error bits
	 * do not function.
	*/
#if R8C_FLASH == 1
	/* R8C/M12A Series */
	/* Lock bit disable select bit */
	fmr13 = 0;		
	/*   Must write a 0 then a 1 in succession to SET */
	fmr13 = 1;		
	/* Data flash block A rewrite enabled */
	fmr16 = 0;		
	/* Data flash block B rewrite enabled */
	fmr17 = 0;		

#endif 	

	/* Jump into RAM routine spcified by EW0_RAM_CODE_ptr */
	_asm("jmpi.a _EW0_RAM_CODE_ptr");	
	/* Create an address label so we know where copy code from	*/
	_asm("ERASE_CODE_START:");		
    /* Attempt to erase flash up to 3 times before returning failed */
    for( i = 0; i<3; i++)
    {
    	/* Clear status register	*/
		*flash_addr = 0x50;		
    	/* Send erase command	*/
		*flash_addr = 0x20;		
    	/* Send erase confirm command	*/
		*flash_addr = 0xD0;		

    	/* Note: In EW0 Mode, we need to poll the Ready/Busy bit until the operation completed */
        /* Wait for ready bit if executing in RAM for EW0 mode.*/
		/* Perform other time critical operations such as "kicking" the watchdog timer.*/
#if R8C_FLASH == 1
		/* R8C/M12A Series */
    	while(!fst7) 
		{ 
		}
#endif

    	/* Check if operation was successful */
#if R8C_FLASH == 1
		/* R8C/M12A Series */
		/* Erasing Successful?*/
    	if( !fst5 )     	
#endif
        {
            break;
        }
    }

   	/* Send Read Array command in order to tell flash controller
	   to go back into Flash Read mode (as opposed to Read Status mode) */
	*flash_addr = 0xFF;		

	/* Disable CPU rewriting commands by clearing EW entry bit */
	fmr0 = 0;
	_asm("nop");

	/* If in RAM, this will jump you back to flash memory */
	_asm("jmp.a ERASE_CODE_END");	
	_asm("ERASE_CODE_END:");
	

#if R8C_FLASH == 1
	/* R8C/M12A Series */
    /* Command Sequence Error?*/
	if( fst4 && fst5 )	
    {
        ret_value =  2;
    }
	/* Erasing error?*/
	else if( fst5 )		
	{
		ret_value =  1;
	}
    
	/* Erase Pass!!*/
	else                    
    {
        ret_value =  0;          
    }
#endif

	/* Restore I flag to previsous setting*/
	RESTORE_INTERRUPTS;         
	return ret_value;
}
#endif /* EW_MODE == 0*/
/******************************************************************************
End of function FlashEraseEW0
*******************************************************************************/

/******************************************************************************
* Outline 		: FlashWriteEW0 (Should be called using the name FlashWrite)
* Description 	: Writes bytes into flash.
* Argument  	: flash_addr (Flash address location to write to.)
*				  buffer_addr (Address location of data buffer to write to flash)
*		          bytes (The number of bytes to write. An even or odd number of 
*				  bytes is OK.)
* Return value  : 		
*		0 = Operation Successful
*		1 = Write Error reported by flash control register
*		2 = Invalid parameter passed
*		3 = Command Sequence Error (locked block, incorrect command, etc)
*       4 = Not enough RAM space allocated for re-write code
******************************************************************************/

#if (EW_MODE == 0)
uint8_t FlashWriteEW0(FLASH_PTR_TYPE flash_addr,
							BUF_PTR_TYPE buffer_addr,
							uint16_t bytes)
{
	uint8_t ret_value = 0;

#if (STATIC_RAM_CODE == 0)
	/* Allocate RAM space to hold the CPU-Rewrite code on the stack. */
	uint8_t EW0_RAM_CODE[ RAM_CODE_SIZE ];
#endif

    /* Check if the RAM space is big enough to hold our re-write code */
   	asm("mov.w #((WRITE_CODE_END-WRITE_CODE_START+1)), _reflash_code_size");
    if( RAM_CODE_SIZE < reflash_code_size )
    {
        return 4;
    }
	/* Turn off maskable interrupts*/
	DISABLE_INTERRUPTS;	

	/* Specify to the assemble code below the RAM code address */
	EW0_RAM_CODE_ptr = (int32_t) EW0_RAM_CODE;

	/* Copy code to RAM using SMOVF instruction (faster than a loop in 'C') */
	/* Can use debugger to determine how big your RAM space needs to be */
	/* R8C and M16C */
#pragma ASM
	pushm	R1,R3,A0,A1
	/* transfer count (number of words )*/
	mov.w	#((WRITE_CODE_END-WRITE_CODE_START+1) / 2 ),R3	;
	/* source address*/
	mov.b	#((WRITE_CODE_START >> 16) & 0FFh),R1H	;
	/* source address*/
	mov.w	#(WRITE_CODE_START & 0FFFFh),A0			;
	/* destination address (stored in EW0_RAM_CODE_ptr) */
	mov.w	_EW0_RAM_CODE_ptr,A1					;
	/* copy 16 bits at a time*/
	smovf.w											;
	popm	R1,R3,A0,A1
#pragma ENDASM

	fmr01 = 0;
	/* Set EW0 select bit*/
	fmr01 = 1;				

	/* !! DISABLE ALL FLASH MEMORY LOCK AND PROTECT FUNCTIONALITY !!
	 * NOTE: In order to use lock/protect bit functionality, please refer to
	 * the Flash Memory section in the data sheet for your specific device.
	 * Also note the that following bits clear back to 0 every time you enter
	 * CPU Rewrite Mode.
	 * Note that for some MCUs, if the lock bit is disabled, the error bits
	 * do not function.
	 */
#if R8C_FLASH == 1
	/* R8C/M12A Series */
	/* Lock bit disable select bit*/
	fmr13 = 0;		
	/* Must write a 0 then a 1 in succession to SET*/
	fmr13 = 1;		
	/* Data flash block A rewrite enabled*/
	fmr16 = 0;		
	/* Data flash block B rewrite enabled*/
	fmr17 = 0;		
#endif 	

	/* Jump into RAM routine spcified by EW0_RAM_CODE_ptr*/
	_asm("jmpi.a _EW0_RAM_CODE_ptr");	
	/* Create an address label so we know where copy code from*/
	_asm("WRITE_CODE_START:");			
	/* Clear status register*/
	*flash_addr = 0x50;		
	while(bytes)
	{
		/* Write to the flash sequencer by writting to that area of flash memory */
		/* Send write command	*/
		*flash_addr = 0x40;				
		/* Write next word of data	*/
		*flash_addr = *buffer_addr;		

    	/* Note: In EW0 Mode, we need to poll the Ready/Busy bit until the operation completed */
        /* Wait for ready bit if executing in RAM for EW0 mode.*/
		/* Perform other time critical operations such as "kicking" the watchdog timer.*/
#if R8C_FLASH == 1
		/* R8C/M12A Series */
    	while(!fst7) 
		{ 
		}
#endif

	#if R8C_FLASH == 1
		/* R8C/M12A Series */
		/* Read flash program status flag */
		/* Write error?*/
		if( fst4 )		
		{
            /* Command Sequence error ?*/
			if( fst5 ) 
    		{
				/* Signal a Command Sequence error*/
				ret_value = 3;		
			}	
			else
    		{
				/* Signal the flash would not write*/
				ret_value = 1;		
			}	
				
			/* Break out of while loop*/
			break;				
		}
	#endif

		/* Advance to next flash write address*/
		flash_addr++;			
		/* Advance to next data buffer address*/
		buffer_addr++;			

		/* NOTE: The R8C writes to flash a 8 bits at a time where as the M16C
		 * writes to flash 16 bits at a time. */
		/* Subract 1 from byte counter */ 
		bytes--;				
	}

	/* Send Read Array command in order to tell flash controller*/
	*flash_addr = 0xFF;		
	/* to go back into Flash Read mode (as opposed to Read Status mode)*/
	/* disable EW mode by clearing EW entry bit*/
	fmr0 = 0;				
	_asm("nop");
	/* If in RAM, this will jump you back to flash memory*/
	_asm("jmp.a WRITE_CODE_END");	
	_asm("WRITE_CODE_END:");
	/* Restore I flag to previous setting */
	RESTORE_INTERRUPTS;     
	/* Return Pass/Fail*/
	return ret_value;		
}
#endif /* EW_MODE == 0*/
/******************************************************************************
End of function FlashWriteEW0
*******************************************************************************/

/******************************************************************************
* Outline 		: FlashEraseEW1 (Should be called using the name FlashErase)
* Description 	: Erases an entire flash block.
* Argument  	: block
*			      The block number to erase (BLOCK_0, BLOCK_1, etc...)
* Return value  : 		
*		0 = Erase Successful
*		1 = Erase error reported by flash control
*		2 = Command Sequence Error (locked block, incorrect command, etc)
******************************************************************************/

#if ( EW_MODE == 1)
uint8_t FlashEraseEW1( uint8_t block )
{
	volatile FLASH_PTR_TYPE flash_addr;
    uint8_t i, ret_value;

	/* Get highest even block address*/
	flash_addr = (FLASH_PTR_TYPE) block_addresses[ block ];
	/* Turn off maskable interrupts */
	DISABLE_INTERRUPTS;	

#if R8C_FLASH == 1
	/* R8C/M12A Series */
	fmr01 = 0;
	/* Set EW0 select bit*/
	fmr01 = 1;				
	fmr02 = 0;
	/* Set to EW1 mode */
	fmr02 = 1;				

#endif

	/* !! DISABLE ALL FLASH MEMORY LOCK AND PROTECT FUNCTIONALITY !!
	 * NOTE: In order to use lock/protect bit functionality, please refer to
	 * the Flash Memory section in the data sheet for your specific device.
	 * Also note the that following bits clear back to 0 every time you enter
	 * CPU Rewrite Mode.
	 * Note that for some MCUs, if the lock bit is disabled, the error bits
	 * do not function.
	 */
#if R8C_FLASH == 1
	/* R8C/M12A Series */
	/* Lock bit disable select bit*/
	fmr13 = 0;		
	/*   Must write a 0 then a 1 in succession to SET*/
	fmr13 = 1;		
	/* Data flash block A rewrite enabled */
	fmr16 = 0;		
	/* Data flash block B rewrite enabled */
	fmr17 = 0;		

#endif

    /* Attempt to erase flash up to 3 times before returning failed */
    for( i = 0; i<3; i++)
    {
    	/*Clear status register*/
		*flash_addr = 0x50;		
    	/* Send erase command */
		*flash_addr = 0x20;		
    	/* Send erase confirm command */
		*flash_addr = 0xD0;		

	#if R8C_FLASH == 1
		/* R8C/M12A Series: */
		/* For Data Flash areas, we have to wait for FST7 signal we are ready*/
		/* which means we could go off and do other things. But when programming*/
		/* non-Data Flash areas, the MCU is suspended until the operation completes.*/
		while(fst7 != 1 ) 
		{	
		}

	#endif
	
        /* Check if operation was successful */
#if R8C_FLASH == 1
		/* R8C/M12A Series */
    	/* Erasing Successful?*/
		if( !fst5 )     	
 	
#endif
        {
            break;
        }

    	/* Disable CPU rewriting commands by clearing EW entry bit */
    	fmr0 = 0;
    }

#if R8C_FLASH == 1
	/* R8C/M12A Series */
    /* Command Sequence Error? */
	if( fst4 && fst5 )	
    {
        ret_value =  2;
    }
	/* Erasing error? */
	else if( fst5 )		
	{
		ret_value =  1;
	}
    /* Erase Pass!! */
	else                    
    {
        ret_value =  0;          
    }
#endif

	/* Restore I flag to previsous setting */
	RESTORE_INTERRUPTS;         
	return ret_value;
}
#endif /* EW_MODE == 1*/
/******************************************************************************
End of function FlashEraseEW1
*******************************************************************************/

/******************************************************************************
* Outline 		: FlashWriteEW1 (Should be called using the name FlashWrite)
* Description 	: Writes bytes into flash.
* Argument  	: flash_addr (Flash address location to write to.)
*				  buffer_addr (ddress location of data buffer to write to flash)
*				  bytes (The number of bytes to write. An even or odd number of
*				  bytes is OK.)
* Return value  : 		
*		0 = Operation Successful
*		1 = Write Error reported by flash control register
*		2 = Invalid parameter passed
*		3 = Command Sequence Error (locked block, incorrect command, etc)
******************************************************************************/

#if ( EW_MODE == 1)
uint8_t FlashWriteEW1(FLASH_PTR_TYPE  flash_addr,
                            BUF_PTR_TYPE    buffer_addr,
                            uint16_t    bytes)
{
	uint8_t ret_value = 0;

	/* Turn off maskable interrupts	*/
	DISABLE_INTERRUPTS;	

#if R8C_FLASH == 1
	/* R8C/M12A Series */
	fmr01 = 0;
	/* Set EW0 select bit */
	fmr01 = 1;				
	fmr02 = 0;
	/* Set to EW1 mode*/
	fmr02 = 1;				
		
#endif

	/* !! DISABLE ALL FLASH MEMORY LOCK AND PROTECT FUNCTIONALITY !!
	 * NOTE: In order to use lock/protect bit functionality, please refer to
	 * the Flash Memory section in the data sheet for your specific device.
	 * Also note the that following bits clear back to 0 every time you enter
	 * CPU Rewrite Mode.
	 * Note that for some MCUs, if the lock bit is disabled, the error bits
	 * do not function.
	 */
#if R8C_FLASH == 1
	/* R8C/M12A Series */
	/* Lock bit disable select bit*/
	fmr13 = 0;		
	/*   Must write a 0 then a 1 in succession to SET*/
	fmr13 = 1;		
	/* Data flash block A rewrite enabled*/
	fmr16 = 0;		
	/* Data flash block B rewrite enabled */
	fmr17 = 0;		
	 	
#endif 	
	/* Clear status register*/
	*flash_addr = 0x50;		
	while(bytes)
	{
		/* Write to the flash sequencer by writting to that area of flash memory */
		/* Send write command*/
		*flash_addr = 0x40;			
		/* Write next word of data*/
		*flash_addr = *buffer_addr;		

	#if R8C_FLASH == 1
		/* R8C/M12A Series: */
		/* For Data Flash areas, we have to wait for FST7 signal we are ready*/
		/* which means we could go off and do other things. But when programming*/
		/* non-Data Flash areas, the MCU is suspended until the operation completes.*/
		while(fst7 != 1 ) 
		{	
		}
	#endif

	#if R8C_FLASH == 1
		/* R8C/M12A Series */
		/* Read flash program status flag */
		/* Write error?	*/
		if( fst4 )		
		{
			/* Command Sequence error ?*/
            if( fst5 ) 
    		{
				/* Signal a Command Sequence error*/
				ret_value = 3;		
			}	
            else
			{
    			/* Signal the flash would not write*/
				ret_value = 1;		
			}
			/* Break out of while loop*/
			break;				
		}

	#endif

	/* Advance to next flash write address*/
		flash_addr++;			
	/* Advance to next data buffer address*/
		buffer_addr++;			

		/* NOTE: The R8C writes to flash a 8 bits at a time where as the M16C
		 * writes to flash 16 bits at a time. 
		 Subract 1 from byte counter	*/
		bytes--;				
	}

	/* Disable CPU rewriting commands by clearing EW entry bit */
	/* (must be 16-bit write for M32C while in EW1 Mode)*/
	fmr0 = 0;				

	/* Restore I flag to previous setting*/
	RESTORE_INTERRUPTS;	

	/* Return Pass/Fail*/
	return ret_value;		
}
#endif /* EW_MODE == 1*/
/******************************************************************************
End of function FlashWriteEW1
*******************************************************************************/
