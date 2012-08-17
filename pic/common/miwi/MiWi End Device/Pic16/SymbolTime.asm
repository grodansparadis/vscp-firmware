;*********************************************************************
;* FileName:		SymbolTime.asm
;* Dependencies:    SymbolTime.inc, external definitions including:
;*                  CLOCK_FREQ
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
;*   Provides functions for keeping time in IEEE 802.15.4 symbols
;*
;* Change History:
;*  Rev   Date         Description
;*  0.1   11/15/2006   Initial revision
;*  1.0   01/15/2007   Initial release
;********************************************************************/
#define SYMBOL_TIME_ASM

;************************* INCLUDE FILES ****************************
	#include "MiWiDefs.inc"
    #include "SymbolTime.inc"

;************************* VARIABLES ********************************
SECT_SYMBOL_TIME_VARS   UDATA
SymbolTime              res 0x04
TimerExtension          res 0x02

;************************* CODE *************************************
SECT_SYMBOL_TIME_CODE   CODE

;********************************************************************
; Function: InitSymbolTimer
;
; PreCondition: CLOCK_DIVIDER_SETTING defined from SymobolTime.inc
;
; Input: none      
;
; Output: none
;
; Side Effects: Timer 1 is enabled with interrupts for the correct
;               Symbol time
;
; Stack Requirements: 1 level deep
;
; Overview: Initializes the Timer 1 module for symbol time use
;*********************************************************************
InitSymbolTimer
    banksel T1CON
    ;set the clock divider
    movlw CLOCK_DIVIDER_SETTING;
    movwf T1CON;
    ;clear the interrupt flag
    bcf PIR1,TMR1IF
    banksel PIE1
    ;set the interrupt enable
    bsf PIE1,TMR1IE
    banksel T1CON
    ;start the timer
    bsf T1CON,TMR1ON
    return
    
;********************************************************************
; Function: TickGet
;
; PreCondition: InitSymbolTimer has been called
;
; Input: none
;
; Output: SymbolTime (4 bytes) now has the value of the current time
;
; Side Effects: TMR1IE interrupt disabled for the duration of the
;                 the function
;
; Stack Requirements: 1 level deep
;
; Overview: gets the current symbol time reading
;*********************************************************************
TickGet
    banksel PIE1
    ;disable the interrupt
    bcf PIE1,TMR1IE
    banksel SymbolTime
    
    ;capture the time
    movf TMR1H,W
    movwf SymbolTime+1
    movf TMR1L,W
    movwf SymbolTime+0
    movf TimerExtension+0,W
    movwf SymbolTime+2
    movf TimerExtension+1,W
    movwf SymbolTime+3
    banksel PIE1
    
    ;enable the interrupt again
    bsf PIE1,TMR1IE
    banksel 0
    return
    end
