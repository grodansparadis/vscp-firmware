;******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
;* File Name          : 91x_init.s
;* Author             : MCD Application Team
;* Date First Issued  : 10/01/2006 : V1.0
;* Description        : This module performs:
;*                      - FLASH/RAM initialization,
;*                      - Stack pointer initialization for each mode ,
;*                      - Branches to ?main in the C library (which eventually
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

; Depending in Your Application, Disable or Enable the following Define

	  #define  BUFFERED_Mode       ; Work on Buffered mode, when enabling this define
                               ; just enable the Buffered define on 91x_conf.h

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



; STR9X register specific definition

SCRO_AHB_UNB    EQU     0x5C002034

;---------------------------------------------------------------
; ?program_start
;---------------------------------------------------------------
		MODULE	?program_start
		RSEG	IRQ_STACK:DATA(2)
		RSEG	FIQ_STACK:DATA(2)
		RSEG	UND_STACK:DATA(2)
		RSEG	ABT_STACK:DATA(2)		
	        RSEG	SVC_STACK:DATA(2)
		RSEG	CSTACK:DATA(2)
		RSEG	ICODE:CODE(2)
		PUBLIC	__program_start
		EXTERN	?main
                CODE32


__program_start:
        LDR     pc, =NextInst


NextInst


        NOP   ; execute some instructions to access CPU registers after wake
        NOP  ; up from Reset, while waiting for OSC stabilization
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP

#ifdef BUFFERED_Mode

  ; BUFFERED_Mode
  ; -------------------------------------------------------------------------------
  ; Description  :   Enable the Buffered mode.
  ;                  When enable, just enable the buffered define on the 91x_conf.h
  ; -------------------------------------------------------------------------------

        MRC     p15, 0, r0, c1, c0, 0   ; Read CP15 register 1 into r0
	  ORR     r0, r0, #0x8            ; Enable Write Buffer on AHB
        MCR     p15, 0, r0, c1, c0, 0   ; Write CP15 register 1
#endif


; --- Remap Flash Bank 0 at address 0x0 and Bank 1 at address 0x80000,
;     when the bank 0 is the boot bank, then enable the Bank 1.

;        LDR R6, =0x54000000
;        LDR R7, =0x4
;        STR R7, [R6]

;        LDR R6, =0x54000004
;        LDR R7, =0x3
;        STR R7, [R6]

;        LDR R6, =0x5400000C
;        LDR R7, =0x0
;        STR R7, [R6]

;        LDR R6, =0x54000010
;        LDR R7, =0x20000
;        STR R7, [R6]

;        LDR R6, =0x54000018
;        LDR R7, =0x18
;        STR R7, [R6]

       LDR R6, =0x54000004     ; non boot bank size
       LDR R7, =0x6            ; 512K
       STR R7, [R6]

       LDR R6, =0x5400000C     ; boot bank address = Bank 1
       LDR R7, =0x0            ; 0x0
       STR R7, [R6]

       LDR R6, =0x54000010     ; non boot bank address =  Bank 0
       LDR R7, =0x20000         ; 0x10000 = 4x 0x4000
       STR R7, [R6]

       LDR R6, =0x54000018     ; enable Non Boot bank
       LDR R7, =0x18
       STR R7, [R6]


; --- Enable 96K RAM
        LDR     R0, = SCRO_AHB_UNB
        LDR     R1, = 0x0196
        STR     R1, [R0]

; --- Initialize Stack pointer registers

        ; Enter each mode in turn and set up the stack pointer

	MSR     CPSR_c, #Mode_FIQ|I_Bit|F_Bit    ; No interrupts
	LDR     SP, =SFE(FIQ_STACK) & 0xFFFFFFF8

	MSR     CPSR_c, #Mode_IRQ|I_Bit|F_Bit    ; No interrupts
	LDR     SP, = SFE(IRQ_STACK) & 0xFFFFFFF8

	MSR     CPSR_c, #Mode_ABT|I_Bit|F_Bit    ; No interrupts
	LDR     SP, = SFE(ABT_STACK) & 0xFFFFFFF8
	
	MSR     CPSR_c, #Mode_UND|I_Bit|F_Bit    ; No interrupts
        LDR     SP, = SFE(UND_STACK) & 0xFFFFFFF8

	MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit    ; No interrupts
	LDR     SP, = SFE(SVC_STACK) & 0xFFFFFFF8



; --- Set bits 17-18 of the Core Configuration Control Register

      MOV     r0, #0x60000
      MCR     p15,0x1,r0,c15,c1,0

; --- Now change to USR/SYS mode and set up User mode stack,

        MSR     CPSR_c, #Mode_SYS               ; IRQs & FIQs are now enabled
        LDR     SP, = SFE(CSTACK) & 0xFFFFFFF8

; --- Now enter the C code
        LDR R0,=?main   ; Note : use B not BL, because an application will
        BX R0                 ; never return this way

        LTORG


        END
;******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****




