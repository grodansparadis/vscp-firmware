;*********************************************************************
;* FileName:		Console.asm
;* Dependencies:    Console.inc, external definitions including:
;*                  ENABLE_CONSOLE, CLOCK_FREQ, and BAUD_RATE
;* Processor:   	PIC18F877A
;* Hardware:		PICDEM Z
;* Assembler:	    MPASMWIN 5.01 or higher
;* Linker:		    MPLINK 4.05 or higher
;* Company:		    Microchip Technology, Inc.
;*
;* Copyright and Disclaimer Notice for MiWi Software:
;*
;* Microchip Technology Inc. (“Microchip”) licenses this software to you 
;* solely for use with Microchip products, as described in the license 
;* agreement accompanying this software (“License”). The software is owned 
;* by Microchip, and is protected under applicable copyright laws.  
;* All rights reserved.
;* 
;* Distribution of this software (in object code or source code) is not 
;* permitted, except as described in Section 2 of the License.
;* 
;* SOFTWARE IS PROVIDED “AS IS.”  MICROCHIP EXPRESSLY DISCLAIM ANY 
;* WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT 
;* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
;* PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP 
;* BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
;* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
;* TECHNOLOGY OR SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT 
;* LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, 
;* OR OTHER SIMILAR COSTS.
;*
;********************************************************************/
;* File Description:
;* 
;* Functions provided for interfacing to the UART module
;*   Functions provided: ConsoleInit, PrintChar, ConsolePut
;*
;* This module requires ENABLE_CONSOLE to be defined otherwise this
;* module will compile out.
;* 
;* Change History:
;*  Rev   Date         Description
;*  0.1   11/09/2006   Initial revision
;*  1.0   01/15/2007   Initial release
;********************************************************************/

#define CONSOLE_ASM

;************************* INCLUDE FILES ****************************
	#include "P16F877A.INC"
	#include "Console.inc"
	#include "MiWiDefs.inc"

;************************* MODULE ENABLE ****************************
#ifdef ENABLE_CONSOLE

;************************* MACROS ***********************************
	#define USART_USE_BRGH_HIGH
    #ifdef USART_USE_BRGH_HIGH
        #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/d'16') - 1)
    #else
        #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/d'64') - 1)
    #endif

;************************* VARIABLES ********************************
CONSOLE_ASM_VARS UDATA
PRINTCHAR_TEMP	res 0x01
PRINTCHAR_TEMP2	res 0x01

;************************* CODE *************************************
CONSOLE_ASM_CODE CODE

;********************************************************************
; Function: void ConsolePut(BYTE WREG)
;
; PreCondition:    ConsoleInit has been successfully called and the
;                  appropriate I/O pins have been configured.
;
; Input:    WREG                : 8-bit ASCII value of the data to be
;                               ; sent to the terminal
;
; Output:   none
;
; Side Effects: Databank is changed to the bank containing TXREG
;
; Stack Requirements: 1 level deep
;
; Overview: Prints one ASCII character to the UART
;*********************************************************************
ConsolePut
	banksel TXSTA
	btfss TXSTA,TRMT    ;verify the buffer is empty
	goto  ConsolePut    ;if not loop back
	banksel TXREG
	movwf TXREG         ;copy the data in WREG to the transmit buffer
	return

;********************************************************************
; Function: void ConsoleInit(void)
;
; PreCondition:    The port pins are correctly setup for this module
;                  SPBRG_VAL is calculated via the preprocessor based
;                  on the desired BAUD_RATE and the system CLOCK_FREQ
;
; Input:    none           
;
; Output:   none
;
; Side Effects: Databank is changed to the bank containing RCSTA
;
; Stack Requirements: 1 level deep
;
; Overview: Initialize the UART for the specified speed.
;*********************************************************************
ConsoleInit
	banksel SPBRG
	
	movlw SPBRG_VAL             ;Initialize the Baud Rate
	movwf SPBRG

#ifdef USART_USE_BRGH_HIGH
    movlw 0x24                  ;Initialize Tx settings
    movwf TXSTA                 ;8 bit, ASync, High Speed, Enabled
#else
    movlw 0x20                  ;Initialize Tx settings
    movwf TXSTA                 ;8 bit, ASync, Low Speed, Enabled
#endif

	banksel RCSTA
	movlw 0x90                  ;Initialize Rx settings
	movwf RCSTA                 ;8 bit continuous, no address detection
	
	bsf TXSTA,TRMT              ;set the empty bit to indicate no data

	return
	

;********************************************************************
; Function: void PrintChar(BYTE WREG)
;
; PreCondition:    ConsoleInit has been successfully called
;
; Input:    WREG            :WREG contains the hex value that needs
;                            to be displayed for example if WREG=0x5C
;                            then '5C' will print on the UART           
;
; Output:   none
;
; Side Effects: Databank is changed to the bank containing 
;               PRINTCHAR_TEMP
;
; Stack Requirements: 1 level deep
;
; Overview: Converts a hex value into 2 ASCII characters and prints
;           them on the UART
;*********************************************************************
PrintChar
	banksel PRINTCHAR_TEMP
	movwf PRINTCHAR_TEMP        ;save the initial data
	andlw 0xF0                  ;mask up upper nibble
	movwf PRINTCHAR_TEMP2       ;save the upper nibble
    swapf PRINTCHAR_TEMP2,f     ;swap the nibbles - X
	sublw 0x09                  
	btfss STATUS,C
	addlw 0xF9                  ;if(X>0x09){X-=7;}
	sublw 0x39                  ;X=0x39-X; //X is now ASCII
	call ConsolePut             ;Print X
	
	banksel PRINTCHAR_TEMP
	movf PRINTCHAR_TEMP,W       ;X
	andlw 0x0F                  ;mask of lower nibble
	sublw 0x09                  
	btfss STATUS,C
	addlw 0xF9                  ;if(X>0x09){X-=7;}
	sublw 0x39                  ;X=0x39-X; //X is now ASCII
	call ConsolePut             ;Print X
	
	movf PRINTCHAR_TEMP,W       ;restore the original value of WREG
	return

#endif  ;#ifdef ENABLE_CONSOLE
	
	end
 
