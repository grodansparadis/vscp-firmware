;******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
;* File Name          : 73x_init.s
;* Author             : MCD Application Team
;* Date First Issued  : 09/27/2005 :  V1.0
;* Description        : This module performs:
;*                      - FLASH/RAM remapping (if required),
;*                      - Stack pointer initialisation for each mode,
;*                      - Interrupt Controller Initialisation,
;*                      - Branches to ?main in the C library
;*                        (which eventually calls main()).
;*
;*			On reset, the ARM core starts up in Supervisor (SVC) mode,
;*			in ARM state,with IRQ and FIQ disabled.
;*********************************************************************************
; History:
; 09/27/2005 :  V1.0
;*********************************************************************************
; THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
; CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
; AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
; OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
; OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
; CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;********************************************************************************/

	
        IMPORT PRCCUCMU_Addr	
        EXPORT ARMIRQ_Enable
        EXPORT ARMFIQ_Enable	
        EXPORT ARMIRQ_Disable
        EXPORT ARMFIQ_Disable	

; Depending in Your Application, Disable or Enable the Following Defines

;#define   FLASH_RAM_REMAP  ; Remap RAM at Address 0x0
#define   EIC_INIT         ; Configure and Initialize EIC

; --- Standard definitions of mode bits and interrupt (I & F) flags in PSRs
Mode_USR           EQU    0x10
Mode_FIQ           EQU    0x11
Mode_IRQ           EQU    0x12
Mode_SVC           EQU    0x13
Mode_ABT           EQU    0x17
Mode_UND           EQU    0x1B
Mode_SYS           EQU    0x1F 

I_Bit              EQU    0x80 ; when I bit is set, IRQ is disabled
F_Bit              EQU    0x40 ; when F bit is set, FIQ is disabled


; --- System memory locations

CFG_R0_Addr         EQU    0x40000000   ; Configuration Register 0 base address
CFG_PCGR0_Addr      EQU    0x40000008


; --- EIC Registers offsets
EIC_base_addr       EQU   0xFFFFFC00         ; EIC base address
ICR_off_addr        EQU   0x00               ; Interrupt Control register offset
CIPR_off_addr       EQU   0x08               ; Current Interrupt Priority Register offset
IVR_off_addr        EQU   0x18               ; Interrupt Vector Register offset
FIR_off_addr        EQU   0x1C               ; Fast Interrupt Register offset
IER0_off_addr       EQU   0x20               ; Interrupt Enable Register 0 offset
IER1_off_addr       EQU   0x24               ; Interrupt Enable Register 1 offset
IPR0_off_addr       EQU   0x40               ; Interrupt Pending Bit Register 0 offset
IPR1_off_addr       EQU   0x44               ; Interrupt Pending Bit Register 0 offset
SIR0_off_addr       EQU   0x60               ; Source Interrupt Register 0

;---------------------------------------------------------------
; ?program_start
;---------------------------------------------------------------
	        MODULE	?program_start
		SECTION	IRQ_STACK:DATA:NOROOT(3)
		SECTION	FIQ_STACK:DATA:NOROOT(3)
		SECTION	UND_STACK:DATA:NOROOT(3)
		SECTION	ABT_STACK:DATA:NOROOT(3)	
		SECTION	SVC_STACK:DATA:NOROOT(3)
		SECTION	CSTACK:DATA:NOROOT(3)
		SECTION	.text:CODE(2)
		PUBLIC	__iar_program_start
		EXTERN	?main
                CODE32

__iar_program_start
        LDR     pc, =NextInst

NextInst


        NOP   ; Wait for OSC stabilization
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        


        ; Reset all Peripheral Clocks except RAM
        ; This is usefull only when using debugger to Reset\Run the application

        MOV r0,#1
        LDR r1,=CFG_PCGR0_Addr
        STR r0,[r1]
        MOV r0,#0
        STR r0,[r1,#4]

 ; --- Initialize stack pointer registers

 ; Enter each mode in turn and set up the stack pointer


        MSR     CPSR_c, #Mode_FIQ|I_Bit|F_Bit        ; No interrupts
        ldr      sp,=SFE(FIQ_STACK)      ; End of FIQ_STACK

        MSR     CPSR_c, #Mode_IRQ|I_Bit|F_Bit        ; No interrupts
        ldr     sp,=SFE(IRQ_STACK)       ; End of IRQ_STACK

        MSR     CPSR_c, #Mode_ABT|I_Bit|F_Bit        ; No interrupts
        ldr     sp,=SFE(ABT_STACK)       ; End of ABT_STACK

        MSR     CPSR_c, #Mode_UND|I_Bit|F_Bit        ; No interrupts
        ldr     sp,=SFE(UND_STACK)       ; End of UND_STACK

        MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit        ; No interrupts
        ldr     sp,=SFE(SVC_STACK)       ; End of SVC_STACK


#ifdef  FLASH_RAM_REMAP

  ; FLASH_RAM_REMAP
  ; -------------------------------------------------------------------------------
  ; Description  :   On reset, an aliased copy of FLASH  is at 0x0.
  ;                  Perform FLASH/RAM remapping, if required in order to have
  ;                  an aliased copy of RAM at 0x0
  ; -------------------------------------------------------------------------------

        LDR     r1, =CFG_R0_Addr
        LDR     r0, [r1]
        ORR     r0, r0, #0x1
        STR     r0, [r1]
#endif

; RAM is now at 0x0.
; The exception vectors (in 73x_vect.s) must be copied from FLASH to the RAM
; The copying is done later by the C library code inside ?main

#ifdef EIC_INIT

	;EIC initialization
	;-------------------------------------------------------------------------------
	;Description  : Initialize the EIC as following :
	;              - IRQ disabled
	;              - FIQ disabled
	;              - IVR contains the load PC opcode 
	;              - All channels are disabled
	;              - All channels priority equal to 0
	;              - All SIR registers contain offset to the related IRQ table entry
	;-------------------------------------------------------------------------------


        ;Enable the EIC Clock

        LDR     r1, =CFG_PCGR0_Addr
	LDR     r0, [r1]
	ORR     r0, r0, #0x20000000
        STR     r0, [r1]

      ; Wait for EIC stabilization              
        NOP   
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP



        LDR     r3, =EIC_base_addr
        LDR     r4, =0x00000000
        STR     r4, [r3, #ICR_off_addr]    ; Disable FIQ and IRQ
        STR     r4, [r3, #IER0_off_addr]   ; Disable 0 ->31  interrupts channels
        STR     r4, [r3, #IER1_off_addr]   ; Disable 32->63  interrupts channels
        LDR     r4, =0xFFFFFFFF
        STR     r4, [r3, #IPR0_off_addr]   ; Clear 0 ->31 IRQ pending bits
        STR     r4, [r3, #IPR1_off_addr]   ; Clear 32->63 IRQ pending bits
        LDR     r4, =0x18
        STR     r4, [r3, #FIR_off_addr]    ; Disable FIQ channels and clear FIQ pending bits
        LDR     r4, =0x00000000
        STR     r4, [r3, #CIPR_off_addr]   ; Reset the current priority register
        LDR     r4, =0xE59F0000
        STR     r4, [r3, #IVR_off_addr]    ; Write the LDR pc,pc,#offset instruction code in IVR[31:16]
        LDR     r2,= 64                    ; 64 Channel to initialize
        LDR     r0, =PRCCUCMU_Addr          ; Read the address of the IRQs address table
        LDR     r1, =0x00000FFF
        AND     r0,r0,r1
        LDR     r5,=SIR0_off_addr         ; Read SIR0 address
        SUB     r4,r0,#8                  ; subtract 8 for prefetch
        LDR     r1, =0xF3E8               ; add the offset to the 0x00000000 address(IVR address + 3E8 = 0x00000000)
                                          ; 0xF3E8 used to complete the LDR pc,offset opcode
        ADD     r1,r4,r1                  ; compute the jump offset
EIC_INI
        MOV     r4, r1, LSL #16           ; Left shift the result
        STR     r4, [r3, r5]              ; Store the result in SIRx register
        ADD     r1, r1, #4                ; Next IRQ address
        ADD     r5, r5, #4                ; Next SIR
        SUBS    r2, r2, #1                ; Decrement the number of SIR registers to initialize
        BNE     EIC_INI                   ; If more then continue

#endif



; --- Now change to USR/SYS mode and set up User mode stack,

        MSR     CPSR_c, #Mode_SYS                 ; IRQs & FIQs are now enabled
        ldr     sp,=SFE(CSTACK)         ; End of CSTACK



; --- Branch to C Library entry point
        B       ?main   ;  use B not BL, because an application will never return this way


; --- The following functions are exported to the EIC configuration file
; --- and may be used by application whenever they are needed


ARMIRQ_Disable:      ; Disable the ARM7 core IRQ line.

 MRS  r0,CPSR
 ORR  r0, r0, #I_Bit	
 MSR  CPSR_c, r0
 BX lr



ARMFIQ_Disable:      ; Disable the ARM7 core FIQ line.

 MRS  r0,CPSR
 ORR  r0, r0, #F_Bit	
 MSR  CPSR_c, r0
 BX lr




ARMIRQ_Enable:       ; Enable the ARM7 core IRQ line.

 MRS  r0,CPSR
 BIC  r0, r0,#I_Bit	
 MSR  CPSR_c, r0
 BX lr





ARMFIQ_Enable:       ; Enable the ARM7 core FIQ line.

 MRS  r0,CPSR
 BIC  r0, r0,#F_Bit	
 MSR  CPSR_c, r0
 BX lr






        LTORG

        
        END
;******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
