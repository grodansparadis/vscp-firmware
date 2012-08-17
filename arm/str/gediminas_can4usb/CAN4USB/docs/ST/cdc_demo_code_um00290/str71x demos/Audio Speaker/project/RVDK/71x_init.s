;******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
;* File Name          : 71x_init.s
;* Author             : MCD Application Team
;* Date First Issued  : 10/01/2006 : V1.0
;* Description        : This is the first code executed after RESET.
;*                      This code used to initialize system stacks
;*                      and critical peripherals before entering the C code.
;*******************************************************************************
;* History:
;* 10/01/2006 : V1.0
;*******************************************************************************
; THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
; CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
; AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
; OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
; OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
; CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;*******************************************************************************/

        PRESERVE8
        AREA    Init, CODE, READONLY


; --- Standard definitions of mode bits and interrupt (I & F) flags in PSRs

Mode_USR            EQU    0x10
Mode_FIQ            EQU    0x11
Mode_IRQ            EQU    0x12
Mode_SVC            EQU    0x13
Mode_ABT            EQU    0x17
Mode_UNDEF          EQU    0x1B
Mode_SYS            EQU    0x1F ; available on ARM Arch 4 and later

I_Bit               EQU    0x80 ; when I bit is set, IRQ is disabled
F_Bit               EQU    0x40 ; when F bit is set, FIQ is disabled


; --- System memory locations

RAM_Base            EQU    0x20000000
RAM_Limit           EQU    0x20010000
SRAM_Base           EQU    0x60000000
Stack_Base          EQU    RAM_Limit


USR_Stack_Length    EQU    4096
IRQ_Stack_Length    EQU    1024
SVC_Stack_Length    EQU    256
FIQ_Stack_Length    EQU    256
ABT_Stack_Length    EQU    256
UNDEF_Stack_Length  EQU    256


USR_Stack           EQU    Stack_Base                 ; USR stack
IRQ_Stack           EQU    USR_Stack-USR_Stack_Length ; followed by IRQ stack
SVC_Stack           EQU    IRQ_Stack-IRQ_Stack_Length ; followed by SVC stack
FIQ_Stack           EQU    SVC_Stack-SVC_Stack_Length ; followed by FIQ stack
ABT_Stack           EQU    FIQ_Stack-FIQ_Stack_Length ; followed by ABT stack
UNDEF_Stack         EQU    ABT_Stack-ABT_Stack_Length ; followed by UNDEF stack

EIC_Base_addr       EQU    0xFFFFF800; EIC base address
ICR_off_addr        EQU    0x00      ; Interrupt Control register offset
CIPR_off_addr       EQU    0x08      ; Current Interrupt Priority Register offset
IVR_off_addr        EQU    0x18      ; Interrupt Vector Register offset
FIR_off_addr        EQU    0x1C      ; Fast Interrupt Register offset
IER_off_addr        EQU    0x20      ; Interrupt Enable Register offset
IPR_off_addr        EQU    0x40      ; Interrupt Pending Bit Register offset
SIR0_off_addr       EQU    0x60      ; Source Interrupt Register 0

EMI_Base_addr       EQU    0x6C000000; EMI base address
BCON0_off_addr      EQU    0x00      ; Bank 0 configuration register offset
BCON1_off_addr      EQU    0x04      ; Bank 1 configuration register offset
BCON2_off_addr      EQU    0x08      ; Bank 2 configuration register offset
BCON3_off_addr      EQU    0x0C      ; Bank 3 configuration register offset

EMI_ENABLE          EQU    0x8000
EMI_SIZE_16         EQU    0x0001

GPIO2_Base_addr     EQU    0xE0005000; GPIO2 base address
PC0_off_addr        EQU    0x00      ; Port Configuration Register 0 offset
PC1_off_addr        EQU    0x04      ; Port Configuration Register 1 offset
PC2_off_addr        EQU    0x08      ; Port Configuration Register 2 offset
PD_off_addr         EQU    0x0C      ; Port Data Register offset

CPM_Base_addr       EQU    0xA0000040; CPM Base Address
BOOTCR_off_addr     EQU    0x10      ; CPM - Boot Configuration Register
FLASH_mask          EQU    0x0000    ; to remap FLASH at 0x0
RAM_mask            EQU    0x0002    ; to remap RAM at 0x0
EXTMEM_mask         EQU    0x0003    ; to remap EXTMEM at 0x0

;|----------------------------------------------------------------------------------|
;| - APB Bridge  (System Peripheral)                                               |
;|----------------------------------------------------------------------------------|
APB1_base_addr      EQU    0xC0000000          ; APB Bridge1 Base Address
APB2_base_addr      EQU    0xE0000000          ; APB Bridge2 Base Address
CKDIS_off_addr      EQU    0x10                ; APB Bridge1 - Clock Disable  Register
SWRES_off_addr      EQU    0x14                ; APB Bridge1 - Software Reset Register
CKDIS1_config_all   EQU    0x27FB              ; To enable/disable clock of all APB1's peripherals
SWRES1_config_all   EQU    0x27FB              ; To reset all APB1's peripherals
CKDIS2_config_all   EQU    0x7FDD              ; To enable/disable clock of all APB2's peripherals
SWRES2_config_all   EQU    0x7FDD              ; To reset all APB2's peripherals

;*******************************************************************************
;*******                         -- MACROS --                            *******
;*******************************************************************************
;*******************************************************************************
;* Macro Name     : EMI_INIT
;* Description    : This macro Initialize EMI bank 1: 16-bit 7 wait state
;* Input          : None.
;* Output         : None.
;*******************************************************************************
        MACRO   
        EMI_INIT
        LDR     r0, =GPIO2_Base_addr      ; Configure P2.0 -> 7 in AF_PP mode
        LDR     r2, [r0, #PC0_off_addr]
        ORR     r2, r2,#0x000000FF
        STR     r2, [r0, #PC0_off_addr]
        LDR     r2, [r0, #PC1_off_addr]
        ORR     r2, r2,#0x000000FF
        STR     r2, [r0, #PC1_off_addr]
        LDR     r2, [r0, #PC2_off_addr]
        ORR     r2, r2,#0x000000FF
        STR     r2, [r0, #PC2_off_addr]
        LDR     r0, =EMI_Base_addr
        LDR     r1, =0x4:OR:EMI_ENABLE:OR:EMI_SIZE_16
        STR     r1, [r0, #BCON1_off_addr] ; Enable bank 1 16-bit 1 wait state
        MEND
;*******************************************************************************
;* Macro Name     : EIC_INIT
;* Description    : This macro Initialize the EIC as following :
;                 - IRQ disabled
;                 - FIQ disabled
;                 - IVR contain the load PC opcode (0xE59FFXXX)
;                 - Current priority level equal to 0
;                 - All channels are disabled
;                 - All channels priority equal to 0
;                 - All SIR registers contain offset to the related IRQ
;                   table entry
;* Input          : None.
;* Output         : None.
;*******************************************************************************
        MACRO   
        EIC_INIT
        LDR     r3, =EIC_Base_addr
        LDR     r4, =0xE59F0000

        STR     r4, [r3, #IVR_off_addr]; Write the LDR pc,[pc,#offset]
                                       ; instruction code in IVR[31:16]
        LDR     r2, =32                ; 32 Channel to initialize
        LDR     r0, =T0TIMI_Addr       ; Read the address of the IRQs
                                       ; address table
        LDR     r1, =0x00000FFF
        AND     r0,r0,r1
        LDR     r5, =SIR0_off_addr     ; Read SIR0 address
        SUB     r4,r0,#8               ; Subtract 8 for prefetch
        LDR     r1, =0xF7E8            ; Add the offset from IVR to 0x00000000
                                       ; address(IVR address + 7E8 = 0x00000000)
                                       ; 0xF7E8 used to complete the
                                       ; LDR pc,[pc,#offset] opcode (0xE59FFXXX)
        ADD     r1,r4,r1               ; Compute the jump offset from IVR to the
                                       ; IRQ table entry.
EIC_INI MOV     r4, r1, LSL #16        ; Left shift the result
        STR     r4, [r3, r5]           ; Store the result in SIRx register
        ADD     r1, r1, #4             ; Next IRQ address
        ADD     r5, r5, #4             ; Next SIR
        SUBS    r2, r2, #1             ; Decrement the number of SIR registers
                                       ; to initialize
        BNE     EIC_INI                ; If more then continue
        MEND
;*******************************************************************************
;* Macro Name     : PERIPHERAL_INIT
;* Description    : This macro reset all device peripherals.
;* Input          : None.
;* Output         : None.
;*******************************************************************************
        MACRO   
        PERIPHERAL_INIT
        LDR     r1, =APB1_base_addr      ; r0= APB1 base address
        LDR     r2, =APB2_base_addr      ; r0= APB2 base address
        LDR     r0, =CKDIS1_config_all
        STRH    r0, [r1, #CKDIS_off_addr]; Clock Disabling for all APB1 peripherals
        LDR     r0, =CKDIS2_config_all
        STRH    r0, [r2, #CKDIS_off_addr]; Clock Disabling for all APB2 peripherals
        LDR     r0, =SWRES1_config_all
        STRH    r0, [r1, #SWRES_off_addr]; Keep all APB1 peripherals under reset 
        LDR     r0, =SWRES2_config_all
        STRH    r0, [r2, #SWRES_off_addr]; Keep all APB2 peripherals under reset 
        MOV     r7, #10                  ; Wait that the selected macrocells exit from reset
loop1   SUBS    r7, r7, #1
        BNE     loop1
        MOV     r0, #0
        STRH    r0, [r1, #SWRES_off_addr]; Enable all all APB1 peripherals
        STRH    r0, [r2, #SWRES_off_addr]; Enable all all APB2 peripherals
        STRH    r0, [r1, #CKDIS_off_addr]; Clock Enabling for all APB1 peripherals
        STRH    r0, [r2, #CKDIS_off_addr]; Clock Enabling for all APB2 peripherals
        MOV     r7, #10                  ; Wait that the selected macrocells exit from reset
loop2   SUBS    r7, r7, #1
        BNE     loop2
        MEND

; define remapping
; If you need to remap memory before entring the main program
; uncomment next ligne
            GBLL  remapping

; Then define which memory to remap to address 0x00000000
;  Uncomment next line if you want to remap RAM
            GBLL  remap_ram

;  Uncomment next line if you want to remap FLASH
;            GBLL  remap_flash

;  Uncomment next line if you want to remap the external memory
;            GBLL  remap_ext

        ENTRY
        
        IMPORT  |Image$$STACK$$ZI$$Limit|       ; imported from scatter-file
        IMPORT  |Image$$HEAP$$ZI$$Base|         ; imported from scatter-file

        EXPORT	__user_initial_stackheap

        EXPORT  Reset_Handler
        IMPORT  T0TIMI_Addr

Reset_Handler
         LDR     pc, =NextInst
NextInst
		NOP		; Wait for OSC stabilization
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP

        MSR     CPSR_c, #Mode_ABT:OR:F_Bit:OR:I_Bit
        LDR     SP, =ABT_Stack

        MSR     CPSR_c, #Mode_UNDEF:OR:F_Bit:OR:I_Bit
        LDR     SP,= UNDEF_Stack

        MSR     CPSR_c, #Mode_SVC:OR:F_Bit:OR:I_Bit
        LDR     SP, =RAM_Limit
; Uncomment next ligne if you need to reset all device pripherals
        PERIPHERAL_INIT           ; Reset all device peripherals

; Uncomment next ligne if you need to enable the EMI Bank 1
        EMI_INIT                  ; Initialize EMI Bank 1

;Uncomment next ligne if you need to initialize the EIC
        EIC_INIT                  ; Initialize EIC

;******************************************************************************
;REMAPPING
;Description  : Remapping  memory whether RAM,FLASH or External memory
;               at Address 0x0 after the application has started executing.
;               Remapping is generally done to allow RAM  to replace FLASH
;               or EXTMEM at 0x00000000.
;               the remapping of RAM allow copying of vector table into RAM
;               To enable the memory remapping uncomment: (see above)
;               GBLL  remapping to enable memory remapping
;                  AND
;               GBLL  remap_ram to remap RAM
;                  OR
;               GBLL  remap_flash to remap FLASH
;                  OR
;               GBLL  remap_ext to remap External memory Bank0
;******************************************************************************
  IF :DEF: remapping
    IF :DEF: remap_flash
        MOV     r0, #FLASH_mask
    ENDIF
    IF :DEF: remap_ram
        MOV     r0, #RAM_mask
    ENDIF
    IF :DEF: remap_ext
        MOV     r0, #EXTMEM_mask
    ENDIF
        LDR     r1, =CPM_Base_addr
        LDRH    r2, [r1, #BOOTCR_off_addr]; Read BOOTCR Register
        BIC     r2, r2, #0x03             ; Reset the two LSB bits of BOOTCR
        ORR     r2, r2, r0                ; change the two LSB bits of BOOTCR
        STRH    r2, [r1, #BOOTCR_off_addr]; Write BOOTCR Register
  ENDIF

       	MSR     CPSR_c, #Mode_FIQ:OR:I_Bit; Change to FIQ mode
        LDR     SP, =FIQ_Stack            ; Initialize FIQ stack pointer

       	MSR     CPSR_c, #Mode_IRQ:OR:I_Bit; Change to IRQ mode
        LDR     SP, =IRQ_Stack            ; Initialize IRQ stack pointer

        MSR     CPSR_c, #Mode_USR         ; Change to User mode, Enable IRQ and FIQ
        LDR     SP, =USR_Stack            ; Initialize USR stack pointer

        IMPORT  __main

; --- Now branches to a C lib function that copies RO data from their
;     load region to their execute region, create the RW and ZI regions
;     then jumps to user C main program.

        B       __main   ; Note : use B not BL, because an application will
                         ; never return this way

; Implementation of __user_initial_stackheap that places the heap at
; the location of the symbol Image$$HEAP$$ZI$$Base placed by the scatter file.
  

__user_initial_stackheap    

    LDR   r0, =|Image$$HEAP$$ZI$$Base|
    MOV   pc,lr 
        LTORG


        LTORG
        
        END
;******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****

