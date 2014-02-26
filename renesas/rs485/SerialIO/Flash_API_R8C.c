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
* File Name		: Flash_API_R8C.c
* Version 		: 1.00
* Device 		: R8C/L3AC (R5F2L3ACCNFP)
* Tool Chain 	: HEW, Renesas M16C Standard Toolchain v5.45
* H/W Platform	: RSKR8CL3AC
* Description 	: Contains Flash API functions
******************************************************************************/

/******************************************************************************
* History 		: 15.02.2010 Ver. 1.00 First Release
*””FILE COMMENT END””*********************************************************/

/******************************************************************************
User Includes (Project Level Includes)
******************************************************************************/
/* Following header file contains macro defines & Function prototypes for flash 
   API. */
#include "Flash_API_R8C.h"

/******************************************************************************
Function Prototypes
******************************************************************************/
static void SlowMCUClock(void);
static void RestoreMCUClock(void);

/******************************************************************************
Global Variables
******************************************************************************/
/* This macro has been defined in Flash_API_R8C.h file and will create an array
  called block_addresses. */
static DEFINE_ADDR_LOOKUP;
/* For saving the Flags register (must be in near RAM) */
static unsigned short near flags_reg; 
/* For saving the Clock Mode 0 register */
static unsigned char cm0_saved;     
/* For saving the Clock Mode 1 register */
static unsigned char cm1_saved;     
/*  Save Protection register */
static unsigned char prcr_saved;    

#if (EW_MODE == 0)
	/* This will serve act as a pointer to the code to be used */
	/* Note: 3 bytes needed for jmpi instruction */
	static long EW0_RAM_CODE_ptr; 
	/* For checking if we have enough RAM to hold our re-write code */                
	unsigned short reflash_code_size;  

	#if (STATIC_RAM_CODE == 1)
		/* Allocate RAM space statically to hold the CPU-Rewrite code. */
		static unsigned char near EW0_RAM_CODE[ RAM_CODE_SIZE ];
		/* (STATIC_RAM_CODE == 1) */
	#endif 

/* (EW_MODE == 0) */ 
#endif 

/******************************************************************************
User Program Code
******************************************************************************/

/*””FUNC COMMENT””*************************************************************
* Outline 		: FlashEraseEW0 (Should be called using the name FlashErase)
* Description 	: Erases an entire flash block.
* Argument  	: (unsigned char) block - The block number to erase (BLOCK_0,     
*				  BLOCK_1, etc...) 
* Return value  : (unsigned char)
*				   0 = Erase Successful
*    			:  1 = Erase error reported by flash control
*    			:  2 = Command Sequence Error (locked block, incorrect command, etc)
*        		:  3 = Not enough RAM space allocated for re-write code			  
*””FUNC COMMENT END””*********************************************************/
#if (EW_MODE == 0)
unsigned char FlashEraseEW0( unsigned char block )
{
	volatile FLASH_PTR_TYPE flash_addr;
    unsigned char i, ret_value;

#if (STATIC_RAM_CODE == 0)
	/* Allocate RAM space to hold the CPU-Rewrite code on the stack. */
	unsigned char EW0_RAM_CODE[ RAM_CODE_SIZE ];

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
	DISABLE_INTERRUPTS	

	/* Specify to the assemble code below the RAM code address */
	EW0_RAM_CODE_ptr = (long) EW0_RAM_CODE;

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

	/* Must change main clock speed to meet flash requirements */
	SlowMCUClock();			

	/* CPU rewrite mode enabled */
	fmr01 = 0;
	fmr01 = 1;			
	/* Set to EW0 mode */
	fmr02 = 0;
		
	/* !! DISABLE ALL FLASH MEMORY LOCK AND PROTECT FUNCTIONALITY !!
	 * NOTE: In order to use lock/protect bit functionality, please refer to
	 * the Flash Memory section in the data sheet for your specific device.
	 * Also note the that following bits clear back to 0 every time you enter
	 * CPU Rewrite Mode.
	 * Note that for some MCUs, if the lock bit is disabled, the error bits
	 * do not Function.
	*/
#if (R8C_FLASH == 4)
	/* R8C/3X Series */
	/* Lock bit disable select bit */
	/* Must write a 0 then a 1 in succession to SET */
	fmr13 = 0;		
	fmr13 = 1;		
	
	/* Data flash block A rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
	fmr14 = 1;
	fmr14 = 0;		
	/* Data flash block B rewrite enabled	*/
	/* Must write a 1 then a 0 in succession to SET */
	fmr15 = 1;
	fmr15 = 0;		
	/* Data flash block C rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
	fmr16 = 1;
	fmr16 = 0;		
	/* Data flash block D rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
	fmr17 = 1;		
	fmr17 = 0;			
#else
	/* R8C/1X & R8C2X Series */
	/* Lock bit disable select bit */
	/*   Must write a 0 then a 1 in succession to SET 	*/ 
	fmr02 = 0;		
	fmr02 = 1;		
#endif 	

	/* Jump into RAM routine specified by EW0_RAM_CODE_ptr */
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

    	/* Note: In EW0 Mode, we need to poll the Ready/Busy bit until the 
		   operation completed */
        /* Wait for ready bit if executing in RAM for EW0 mode.*/
		/* Perform other time critical operations such as "kicking"
		   the watchdog timer.*/
#if (R8C_FLASH == 4)
		/* R8C/3X Series */
    	while(!fst7) 
		{
			/* Wait until the operation is completed */ 
		}
#else
		/* R8C/1X & R8C2X Series */
    	while(!fmr00) 
		{ 
			/* Wait until the operation is completed */
		}
#endif

    	/* Check if operation was successful */
#if (R8C_FLASH == 4)
		/* R8C/3X Series */
		/* Erasing Successful?*/
    	if( !fst5 )  
		{
			break;
		}
#else
		/* R8C/1X & R8C2X Series */
    	/* Erasing Successful?*/
		if( !fmr07 )     	

        {
            break;
        }
#endif
    }

   	/* Send Read Array command in order to tell flash controller
	   to go back into Flash Read mode  */
	*flash_addr = 0xFF;		

	/* Disable CPU rewriting commands by clearing EW entry bit */
	fmr0 = 0;
	_asm("nop");

	/* If in RAM, this will jump you back to flash memory */
	_asm("jmp.a ERASE_CODE_END");	
	_asm("ERASE_CODE_END:");
	
	/* Restore clock back to original speed	*/
	RestoreMCUClock();		

#if (R8C_FLASH == 4)
	/* R8C/3X Series */
    /* Command Sequence Error */
	if( fst4 && fst5 )	
    {
        ret_value =  2;
    }
	/* Erasing error*/
	else if( fst5 )		
	{
		ret_value =  1;
	}
    
	/* Erase Pass*/
	else                    
    {
        ret_value =  0;          
    }
#else
	/* R8C/1X & R8C2X Series */
    
	/* Command Sequence Error*/
	if( fmr06 && fmr07 )	
    {
        ret_value =  2;
    }
	/* Erasing error */
	else if( fmr07 )		
	{
		ret_value =  1;
	}
    /* Erase Pass */
	else                    
    {
        ret_value =  0;          
    }
#endif

	/* Restore I flag to previous setting*/
	RESTORE_INTERRUPTS         
	return ret_value;
}
/* EW_MODE == 0*/
#endif 
/******************************************************************************
End of Function FlashEraseEW0
******************************************************************************/

/*””FUNC COMMENT””*************************************************************
* Outline 		: FlashWriteEW0 (Should be called using the name FlashWrite)
* Description 	: Write bytes into flash. The value of BUF_PTR_TYPE can be
*                 made a "far" pointer for R8C devices if you wish to pass a
*  				  buffer address that is greater than 0xFFFF. This is set in
*				  the header file.
* Argument  	: (unsigned char far *) flash_addr -  Flash address location
*				  to write to. For R8C devices, this MUST be an EVEN address. 
*    			  (unsigned char far *) buffer_addr - Address location of data
*				  buffer to write to flash 
*				  (unsigned short) bytes - The number of bytes to write. 
* Return value  : 0 = Operation Successful
*    			  1 = Write Error reported by flash control register
*    			  2 = Invalid parameter passed
*    			  3 = Command Sequence Error (locked block, incorrect command, etc)
*        		  4 = Not enough RAM space allocated for re-write code
*””FUNC COMMENT END””*********************************************************/

#if (EW_MODE == 0)
unsigned char FlashWriteEW0(FLASH_PTR_TYPE flash_addr,
							BUF_PTR_TYPE buffer_addr,
							unsigned short bytes)
{
	unsigned char ret_value = 0;

#if (STATIC_RAM_CODE == 0)
	/* Allocate RAM space to hold the CPU-Rewrite code on the stack. */
	unsigned char EW0_RAM_CODE[ RAM_CODE_SIZE ];
#endif

    /* Check if the RAM space is big enough to hold our re-write code */
   	asm("mov.w #((WRITE_CODE_END-WRITE_CODE_START+1)), _reflash_code_size");
    if( RAM_CODE_SIZE < reflash_code_size )
    {
        return 4;
    }
	/* Turn off maskable interrupts*/
	DISABLE_INTERRUPTS	

	/* Specify to the assemble code below the RAM code address */
	EW0_RAM_CODE_ptr = (long) EW0_RAM_CODE;

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

	/* Must change main clock speed to meet flash requirements */
	SlowMCUClock();			

	/* CPU rewrite mode enabled */
	fmr01 = 0;
	fmr01 = 1;	
	/* Set to EW0 mode */
	fmr02 = 0;			

	/* !! DISABLE ALL FLASH MEMORY LOCK AND PROTECT FUNCTIONALITY !!
	 * NOTE: In order to use lock/protect bit functionality, please refer to
	 * the Flash Memory section in the data sheet for your specific device.
	 * Also note the that following bits clear back to 0 every time you enter
	 * CPU Rewrite Mode.
	 * Note that for some MCUs, if the lock bit is disabled, the error bits
	 * do not Function.
	 */
#if (R8C_FLASH == 4)
	/* R8C/3X Series */
	/* Lock bit disable select bit*/
	/*   Must write a 0 then a 1 in succession to SET*/
	fmr13 = 0;		
	fmr13 = 1;		
	
	/* Data flash block A rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
	fmr14 = 1;
	fmr14 = 0;		
	/* Data flash block B rewrite enabled	*/
	/* Must write a 1 then a 0 in succession to SET */
	fmr15 = 1;
	fmr15 = 0;		
	/* Data flash block C rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
	fmr16 = 1;
	fmr16 = 0;		
	/* Data flash block D rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
	fmr17 = 1;		
	fmr17 = 0;			
#else
	/* R8C/1X & R8C2X Series */
	/* Lock bit disable select bit*/
	fmr02 = 0;		
	/* Must write a 0 then a 1 in succession to SET */	
	fmr02 = 1;		
#endif 	

	/* Jump into RAM routine specified by EW0_RAM_CODE_ptr*/
	_asm("jmpi.a _EW0_RAM_CODE_ptr");	
	/* Create an address label so we know where copy code from*/
	_asm("WRITE_CODE_START:");			
	
	/* Clear status register*/
	*flash_addr = 0x50;		
	
	while(bytes)
	{
		/* Write to the flash sequencer by writing to that area of 
		   flash memory */
		/* Send write command	*/
		*flash_addr = 0x40;				
		/* Write next word of data	*/
		*flash_addr = *buffer_addr;		

    	/* Note: In EW0 Mode, we need to poll the Ready/Busy bit until the
		   operation completed */
        /* Wait for ready bit if executing in RAM for EW0 mode.*/
		/* Perform other time critical operations such as "kicking" the 
		   watchdog timer.*/
#if (R8C_FLASH == 4)
		/* R8C/3X Series */
    	while(!fst7) 
		{
			/* Wait until the operation is completed */ 
		}
#else
		/* R8C/1X & R8C2X Series */
    	while(!fmr00) 
		{ 
			/* Wait until the operation is completed */
		}
#endif

	#if (R8C_FLASH == 4)
		/* R8C/3X Series */
		/* Read flash program status flag */
		/* Write error */
		if( fst4 )		
		{
            /* Command Sequence error */
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
	#else
		/* R8C/1X & R8C2X Series */
		/* Read flash program status flag */
		/* Write error */
		if( fmr06 )		
		{
            /* Command Sequence error */
			if( fmr07 ) 
    		{
				/* Signal a Command Sequence error */
				ret_value = 3;		
			}	
            else
			{
    			/* Signal the flash would not write*/
				ret_value = 1;		
			}	
			/* Break out of while loop */
			break;				
		}
	#endif

		/* Advance to next flash write address*/
		flash_addr++;			
		/* Advance to next data buffer address*/
		buffer_addr++;			

		/* NOTE: The R8C writes to flash 8 bits at a time where as the M16C
		 * writes to flash 16 bits at a time. */
		/* Subtract 1 from byte counter */ 
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
	
	/* Restore clock back to original speed*/
	RestoreMCUClock();		
	
	/* Restore I flag to previous setting */
	RESTORE_INTERRUPTS     
	
	/* Return Pass/Fail*/
	return ret_value;	
}
/* EW_MODE == 0*/
#endif 
/******************************************************************************
End of Function FlashWriteEW0
******************************************************************************/

/*””FUNC COMMENT””*************************************************************
* Outline 		: FlashEraseEW1 (Should be called using the name FlashErase)
* Description 	: Erases an entire flash block.
* Argument  	: (unsigned char) block - The block number to erase (BLOCK_0, 
*				  BLOCK_1, etc...)
* Return value  : (unsigned char)   
*				  0 = Erase Successful
*    			: 1 = Erase error reported by flash control
*    			: 2 = Command Sequence Error (locked block, incorrect command, 
*				  etc)
*””FUNC COMMENT END””*********************************************************/

#if (EW_MODE == 1)
unsigned char FlashEraseEW1( unsigned char block )
{
	volatile FLASH_PTR_TYPE flash_addr;
    unsigned char i, ret_value;

	/* Get highest even block address*/
	flash_addr = (FLASH_PTR_TYPE) block_addresses[ block ];
	/* Turn off maskable interrupts */
	DISABLE_INTERRUPTS	

	/* NOTE: You only have to slow the MCU in EW1 mode for R8C/10 and R8C/11 */
#if (INCLUDE_SLOW_MCU)
	SlowMCUClock();
#endif

#if (R8C_FLASH == 4)
	/* R8C/3X Series */
	fmr01 = 0;
	/* CPU rewrite mode enabled */
	fmr01 = 1;				
	fmr02 = 0;
	/* Set to EW1 mode */
	fmr02 = 1;				
#else
	/* R8C/1X & R8C2X Series */
	fmr01 = 0;
	/* Set EW0 select bit*/
	fmr01 = 1;				
	fmr11 = 0;
	/* Set to EW1 mode */
	fmr11 = 1;				
#endif

	/* !! DISABLE ALL FLASH MEMORY LOCK AND PROTECT FUNCTIONALITY !!
	 * NOTE: In order to use lock/protect bit functionality, please refer to
	 * the Flash Memory section in the data sheet for your specific device.
	 * Also note the that following bits clear back to 0 every time you enter
	 * CPU Rewrite Mode.
	 * Note that for some MCUs, if the lock bit is disabled, the error bits
	 * do not Function.
	 */
#if (R8C_FLASH == 4)
	/* R8C/3X Series */
	/* Lock bit disable select bit*/
	fmr13 = 0;		
	/*   Must write a 0 then a 1 in succession to SET*/
	fmr13 = 1;		
	
	/* Data flash block A rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
//@@	fmr14 = 1;
//@@	fmr14 = 0;		
	/* Data flash block B rewrite enabled	*/
	/* Must write a 1 then a 0 in succession to SET */
//@@	fmr15 = 1;
//@@	fmr15 = 0;		
	/* Data flash block A rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
	fmr16 = 1;
	fmr16 = 0;		
	/* Data flash block B rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
	fmr17 = 1;		
	fmr17 = 0;			
#else
	/* R8C/1X & R8C2X Series */
	/* Lock bit disable select bit */
	fmr02 = 0;		
	/*   Must write a 0 then a 1 in succession to SET 	*/
	fmr02 = 1;		
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

	#if (R8C_FLASH == 4)
		/* R8C/3X Series: */
		/* For Data Flash areas, we have to wait for FST7 signal we are ready*/
		/* which means we could go off and do other things. But when 
		   programming*/
		/* non-Data Flash areas, the MCU is suspended until the operation 
		   completes.*/
		while(fst7 != 1 ) 
		{	
			/* Wait until the operation is completed */ 
		}
	#else
		/* R8C/1X & R8C/2X Series: In EW1 Mode, the MCU is suspended until 
		   the operation completed */
	#endif
	
        /* Check if operation was successful */
#if (R8C_FLASH == 4)
		/* R8C/3X Series */
    	/* Erasing Successful?*/
		if( !fst5 )  
		{
			break;
		}    	
#else
		/* R8C/1X & R8C2X Series */
    	/* Erasing Successful?*/
		if( !fmr07 )     	
#endif
        {
            break;
        }

    	/* Disable CPU rewriting commands by clearing EW entry bit */
    	fmr0 = 0;
    }

	/* NOTE: You only had to slow the MCU in EW1 mode for R8C/10 and R8C/11 */
#if (defined(R8C_10) || defined(R8C_11))
	RestoreMCUClock();
#endif

#if (R8C_FLASH == 4)
	/* R8C/3X Series */
    /* Command Sequence Error? */
	if( (fst4) && (fst5) )	
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
#else
	/* R8C/1X & R8C2X Series */
    /* Command Sequence Error? */
	if( fmr06 && fmr07 )	
    {
        ret_value =  2;
    }
	/* Erasing error? */
	else if( fmr07 )		
	{
		ret_value =  1;
	}
    /* Erase Pass!! */
	else                    
    {
        ret_value =  0;          
    }
#endif

	/* Restore I flag to previous setting */
	RESTORE_INTERRUPTS         
	return ret_value;
}
/* EW_MODE == 1*/
#endif 
/******************************************************************************
End of Function FlashEraseEW1
******************************************************************************/

/*””FUNC COMMENT””*************************************************************
* Outline 		: FlashWriteEW1
* Description 	: Writes bytes into flash. 
* Argument  	: flash_addr  -  Flash address location to write to. For R8C 
*				  devices, this MUST be an EVEN address. 
*    	 		: buffer_addr - Address location of data buffer to write to flash 
*				  bytes - The number of bytes to write. 
* Return value  : (unsigned char)   
*				  0 = Operation Successful
*				  1 = Write Error reported by flash control register
*				  2 = Invalid parameter passed
*				  3 = Command Sequence Error (locked block, incorrect command, etc)
*””FUNC COMMENT END””*********************************************************/
#if ( EW_MODE == 1)
unsigned char FlashWriteEW1(FLASH_PTR_TYPE  flash_addr,
                            BUF_PTR_TYPE    buffer_addr,
                            unsigned short  bytes)
{
	unsigned char ret_value = 0;

	/* Turn off maskable interrupts	*/
	DISABLE_INTERRUPTS	

	/* NOTE: You only have to slow the MCU in EW1 mode for R8C/10 
	   and R8C/11 */
#if (INCLUDE_SLOW_MCU)
	SlowMCUClock();
#endif

#if (R8C_FLASH == 4)
	/* R8C/3X Series */
	fmr01 = 0;
	/* CPU rewrite mode enabled */
	fmr01 = 1;				
	fmr02 = 0;
	/* Set to EW1 mode */
	fmr02 = 1;				
#else
	/* R8C/1X & R8C2X Series */
	fmr01 = 0;
	/* CPU rewrite mode enabled */
	fmr01 = 1;				
	fmr11 = 0;
	/* Set to EW1 mode */
	fmr11 = 1;				
#endif

	/* !! DISABLE ALL FLASH MEMORY LOCK AND PROTECT FUNCTIONALITY !!
	 * NOTE: In order to use lock/protect bit functionality, please refer to
	 * the Flash Memory section in the data sheet for your specific device.
	 * Also note the that following bits clear back to 0 every time you enter
	 * CPU Rewrite Mode.
	 * Note that for some MCUs, if the lock bit is disabled, the error bits
	 * do not Function.
	 */
#if (R8C_FLASH == 4)
	/* R8C/3X Series */
	/* Lock bit disable select bit*/
	/*   Must write a 0 then a 1 in succession to SET*/
	fmr13 = 0;		
	fmr13 = 1;		
	
	/* Data flash block A rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
//@@	fmr14 = 1;
//@@	fmr14 = 0;		
	/* Data flash block B rewrite enabled	*/
	/* Must write a 1 then a 0 in succession to SET */
//@@	fmr15 = 1;
//@@	fmr15 = 0;		
	/* Data flash block A rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
	fmr16 = 1;
	fmr16 = 0;		
	/* Data flash block B rewrite enabled */
	/* Must write a 1 then a 0 in succession to SET */
	fmr17 = 1;		
	fmr17 = 0;					
#else
	/* R8C/1X & R8C2X Series */
	/* Lock bit disable select bit*/
	/*   Must write a 0 then a 1 in succession to SET*/
	fmr02 = 0;		
	fmr02 = 1;		 	
#endif 	

	/* Clear status register*/
	*flash_addr = 0x50;		
	
	while(bytes)
	{
		/* Write to the flash sequencer by writing to that area of flash
		   memory */
		/* Send write command*/
		*flash_addr = 0x40;			
		/* Write next word of data*/
		*flash_addr = *buffer_addr;		

	#if (R8C_FLASH == 4)
		/* R8C/3X Series: */
		/* For Data Flash areas, we have to wait for FST7 signal we 
		   are ready*/
		/* which means we could go off and do other things. But when
		   programming*/
		/* non-Data Flash areas, the MCU is suspended until the operation 
		   completes.*/
		while(fst7 != 1 ) 
		{
			/* Wait until the operation is completed */ 
		}
	#else
		/* R8C/1X & R8C/2X Series: In EW1 Mode, the MCU is suspended until
		   the operation completed */
	#endif

	#if (R8C_FLASH == 4)
		/* R8C/3X Series */
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
	#else
		/* R8C/1X & R8C2X Series */
		/* Read flash program status flag */
		/* Write error? */
		if( fmr06 )		
		{
          /* Command Sequence error ?*/
		    if( fmr07 ) 
    		{
				/* Signal a Command Sequence error*/
				ret_value = 3;		
			}	
            else
			{
    			/* Signal the flash would not write*/
				ret_value = 1;		
				/* Break out of while loop */
			}
			break;				
		}
	#endif

	/* Advance to next flash write address*/
		flash_addr++;			
	/* Advance to next data buffer address*/
		buffer_addr++;			

		/* NOTE: The R8C writes to flash a 8 bits at a time where as the M16C
		 * writes to flash 16 bits at a time. 
		 Subtract 1 from byte counter	*/
		bytes--;				
	}

	/* Disable CPU rewriting commands by clearing EW entry bit */
	/* (must be 16-bit write for M32C while in EW1 Mode)*/
	fmr0 = 0;				

	/* NOTE: You only had to slow the MCU in EW1 mode for R8C/10 and R8C/11 */
#if (defined(R8C_10) || defined(R8C_11))
	RestoreMCUClock();
#endif

	/* Restore I flag to previous setting*/
	RESTORE_INTERRUPTS	

	/* Return Pass/Fail*/
	return ret_value;		
}
/* EW_MODE == 1*/
#endif 
/******************************************************************************
End of Function FlashWriteEW1
******************************************************************************/

/*””FUNC COMMENT””*************************************************************
* Outline		: SlowMCUClock
* Description	: Sets the processor mode for programming flash and saves 
*              	  current settings to restore later. You cannot run the processor 
*              	  faster than 5MHz when sending commands to the flash 
*				  controller.
* 			 	  NOTE: This applies only for EW0 mode only for R8C/12 and up.
* Argument		: none                     
* Return value	: nothing     
*””FUNC COMMENT END””*********************************************************/
#if (INCLUDE_SLOW_MCU)
static void SlowMCUClock(void)
{
	/* Unprotect registers CM0 and CM1 and PM0 registers by writing to 
	   protection register */
	/* Save Protection register*/
	prcr_saved = *((char *)0xA);	
	/* Allow writing to protected system registers */
	*((char *)0xA) = 3;				

	/* Clock settings for R8C  */
    /* Save cm0 register */
	cm0_saved = *((char *)0x6);		
    /* Save cm1 register */
	cm1_saved = *((char *)0x7);		

	/* Now slow the clock down to at least 5MHz */
#if (BCLK_FREQUENCY <= 5000000)
	/* NO NEED TO SLOW DOWN CLOCK */
#elif (BCLK_FREQUENCY <= 10000000 )
/* Set main clock divide by 2 */
	/* Set bit CM16 */	
	_asm("BSET	6,07h");		
	/* Clear bit CM17 */
	_asm("BCLR	7,07h");		
	/* Clear bit CM06 */
	_asm("BCLR	6,06h");		
#elif (BCLK_FREQUENCY <= 20000000)
	/* Set main clock divide by 4 */
	/* Clear bit CM16 */
	_asm("BCLR	6,07h");			
	/* Set bit CM17   */
	_asm("BSET	7,07h");		
	/* Clear bit CM06 */
	_asm("BCLR	6,06h");		
#else /* BCLK_FREQUENCY > 20000000 */
	/* Set main clock divide by 8 */
	/* Set bit CM06 */
	_asm("BSET	6,06h");		
#endif /* BCLK_FREQUENCY */
}
#endif /* INCLUDE_SLOW_MCU */

/******************************************************************************
End of Function SlowMCUClock
******************************************************************************/

/*””FUNC COMMENT””*************************************************************
* Outline		: RestoreMCUClock
* Description	: Restores the processor mode back to original settings.
* 				  NOTE: This applies only for EW0 mode only for R8C/12+.
* Argument		: none                     
* Return value	: nothing
*””FUNC COMMENT END””*********************************************************/
#if (INCLUDE_SLOW_MCU)
static void RestoreMCUClock(void)			
{
	/* Clock settings for R8C */
    /* Restore cm1 register*/
	*((char *)0x7) = cm1_saved;		
    /* Restore cm0 register */
	*((char *)0x6) = cm0_saved;		
	/* Protection back on*/
	*((char *)0xA) = prcr_saved;	
}
#endif /* INCLUDE_SLOW_MCU */
/******************************************************************************
End of Function RestoreMCUClock
******************************************************************************/