;******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
;* File Name          : 91x_init.s
;* Author             : MCD Application Team
;* Date First Issued  : 10/01/2006 : V1.0
;* Description        : This module performs:
;*                      - FLASH/RAM initialization,
;*                      - Stack pointer initialization for each mode ,
;*                      - Branches to __main in the C library (which eventually 
;*                        calls main()).
;*
;*		      On reset, the ARM core starts up in Supervisor (SVC) mode,
;*		      in ARM state,with IRQ and FIQ disabled.
;*******************************************************************************
; History:
; 10/01/2006 : V1.0
;*******************************************************************************
;* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
;* CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS
;* A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
;* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
;* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
;* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;******************************************************************************/

        
        PRESERVE8
        AREA    Init, CODE, READONLY
        
        IMPORT  |Image$$STACK$$ZI$$Limit|       ; imported from scatter-file
        IMPORT  |Image$$HEAP$$ZI$$Base|         ; imported from scatter-file

        EXPORT	__user_initial_stackheap
        
; Ensure no functions that use semihosting SWIs are linked in from the C library
; Comment this line if application uses SWI

       ;IMPORT __use_no_semihosting_swi

; --- Standard definitions of mode bits and interrupt (I & F) flags in PSRs

Mode_USR           EQU     0x10
Mode_FIQ           EQU     0x11
Mode_IRQ           EQU     0x12
Mode_SVC           EQU     0x13
Mode_ABT           EQU     0x17
Mode_UND           EQU     0x1B
Mode_SYS           EQU     0x1F ; available on ARM Arch 4 and later

I_Bit              EQU     0x80 ; when I bit is set, IRQ is disabled
F_Bit              EQU     0x40 ; when F bit is set, FIQ is disabled

;--- BASE ADDRESSES
; System memory locations

RAM_Base       EQU     0x04000000
RAM_Limit      EQU     0x04018000          ; at the top of 96 KB SRAM

; STR9X register specific definition

SCRO_AHB_UNB    EQU     0x5C002034

; --- Amount of memory (in bytes) allocated for stacks

Stack_Base          EQU    RAM_Limit

USR_Stack_Length    EQU    1024
IRQ_Stack_Length    EQU    256
SVC_Stack_Length    EQU    64
FIQ_Stack_Length    EQU    64
ABT_Stack_Length    EQU    0
UND_Stack_Length    EQU    0

; Add lengths >0 for  ABT_Stack, UND_Stack if you need them.
; Offsets will be loaded as immediate values.
; Offsets must be 8 byte aligned.

Offset_FIQ_Stack         EQU     0
Offset_IRQ_Stack         EQU     Offset_FIQ_Stack + FIQ_Stack_Length
Offset_ABT_Stack         EQU     Offset_IRQ_Stack + IRQ_Stack_Length
Offset_UND_Stack         EQU     Offset_ABT_Stack + ABT_Stack_Length
Offset_SVC_Stack         EQU     Offset_UND_Stack + UND_Stack_Length
Offset_USR_Stack         EQU     Offset_SVC_Stack + SVC_Stack_Length


        ENTRY

        EXPORT  Reset_Handler

Reset_Handler
        LDR     pc, = NextInst

NextInst


        NOP  ; execute some instructions to access CPU registers after wake
        NOP  ; up from Reset, while waiting for OSC stabilization
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
  

; --- Remap Flash Bank 0 at address 0x80000 and Bank 1 at address 0x0 ( Default), 
;     when the bank 0 is the boot bank, then enable the Bank 1.

       
       LDR R6, =0x54000004     ; non boot bank size
       LDR R7, =0x6            ; 512K
       STR R7, [R6]

       LDR R6, =0x5400000C     ; boot bank address = Bank 1
       LDR R7, =0x0            ; 0x0
       STR R7, [R6]

       LDR R6, =0x54000010     ; non boot bank address =  Bank 0
       LDR R7, =0x20000        ; 0x10000 = 4x 0x4000
       STR R7, [R6]

       LDR R6, =0x54000018     ; enable Non Boot bank
       LDR R7, =0x18
       STR R7, [R6]
    

; --- Enable 96K RAM & Config
        LDR     R0, = SCRO_AHB_UNB
        LDR     R1, = 0x0191
        STR     R1, [R0]

     

        MRC     p15, 0, r0, c1, c0, 0       ; Read CP15 register 1 into r0
        ORR     r0, r0, #(0x1 << 0xC)       ; Enable ITCM
        ORR     r0, r0, #(0x1 << 2)         ; Enable DTCM
        ORR     r0, r0, #(0x1 << 3)	    ; Enable Write Buffer on AHB 
        MCR     p15, 0, r0, c1, c0, 0       ; Write CP15 register 1
        
; --- Initialize stack pointer registers

        ; Enter each mode in turn and set up the stack pointer
        LDR     r0, =|Image$$STACK$$ZI$$Limit|  ; top of stack region

        MSR     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_FIQ_Stack

        MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_IRQ_Stack

        MSR     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_ABT_Stack

        MSR     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_UND_Stack

        MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_SVC_Stack
 

; --- Now change to USR/SYS mode and set up User mode stack,

        LDR     r0, =|Image$$STACK$$ZI$$Limit|  ; Top of stack region
        MSR     CPSR_c, #Mode_SYS               ; IRQs & FIQs are now enabled
        SUB     sp, r0, #Offset_USR_Stack


      IMPORT  __main

; --- Now enter the C code
      B       __main   ; Note : use B not BL, because an application will
                       ; never return this way
                       
                       
; Implementation of __user_initial_stackheap that places the heap at
; the location of the symbol Image$$HEAP$$ZI$$Base placed by the scatter file.
  

__user_initial_stackheap    

    LDR   r0, =|Image$$HEAP$$ZI$$Base|
    MOV   pc,lr 


      LTORG


      END
;******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****

