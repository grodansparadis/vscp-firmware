;******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
;* File Name          : 75x_vect.c
;* Author             : MCD Application Team
;* Date First Issued  : 10/01/2006 : V1.0
;* Description        : This file used to initialize the exception and IRQ
;*                      vectors, and to enter/return to/from exceptions handlers.
;*******************************************************************************
;* History:
;* 10/01/2006 : V1.0
;*******************************************************************************
;* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
;* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
;* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
;* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
;* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
;* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;*******************************************************************************

                PROGRAM	?RESET
		COMMON	INTVEC:CODE(2)			
		CODE32

EIC_base_addr         EQU    0xFFFFF800 ; EIC base address
CICR_off_addr         EQU    0x04       ; Current Interrupt Channel Register
IVR_off_addr          EQU    0x18       ; Interrupt Vector Register
IPR_off_addr          EQU    0x40       ; Interrupt Pending Register


;*******************************************************************************
;              Import  the __program_start address from 75x_init.s
;*******************************************************************************

        IMPORT  __program_start



;*******************************************************************************
;                      Import exception handlers
;*******************************************************************************

        IMPORT  Undefined_Handler
        IMPORT  SWI_Handler
        IMPORT  Prefetch_Handler
        IMPORT  Abort_Handler
        IMPORT  FIQ_Handler

;*******************************************************************************
;                   Import IRQ handlers from 75x_it.c
;*******************************************************************************


        IMPORT USB_LP_IRQHandler
        IMPORT EXTIT_IRQHandler


;*******************************************************************************
;            Export Peripherals IRQ handlers table address
;*******************************************************************************

        EXPORT  WAKUP_Addr

;*******************************************************************************
;                        Exception vectors
;*******************************************************************************


        LDR     PC, Reset_Addr
        LDR     PC, Undefined_Addr
        LDR     PC, SWI_Addr
        LDR     PC, Prefetch_Addr
        LDR     PC, Abort_Addr
        NOP                             ; Reserved vector
        LDR     PC, IRQ_Addr
        LDR     PC, FIQ_Addr

;*******************************************************************************
;               Exception handlers address table
;*******************************************************************************

Reset_Addr      DCD     __program_start
Undefined_Addr  DCD     0
SWI_Addr        DCD     0
Prefetch_Addr   DCD     0
Abort_Addr      DCD     0
                DCD     0               ; Reserved vector
IRQ_Addr        DCD     IRQHandler
FIQ_Addr        DCD     0

;*******************************************************************************
;              Peripherals IRQ handlers address table
;*******************************************************************************

WAKUP_Addr         DCD	0
TIM2_OC2_Addr      DCD	0
TIM2_OC1_Addr      DCD	0
TIM2_IC12_Addr     DCD	0
TIM2_UP_Addr       DCD	0
TIM1_OC2_Addr      DCD	0
TIM1_OC1_Addr      DCD	0
TIM1_IC12_Addr     DCD	0
TIM1_UP_Addr       DCD	0
TIM0_OC2_Addr      DCD	0
TIM0_OC1_Addr      DCD	0
TIM0_IC12_Addr     DCD	0
TIM0_UP_Addr       DCD	0
PWM_OC123_Addr     DCD	0
PWM_EM_Addr        DCD	0
PWM_UP_Addr        DCD	0
I2C_Addr           DCD	0
SSP1_Addr          DCD	0
SSP0_Addr          DCD	0
UART2_Addr         DCD	0
UART1_Addr         DCD	0
UART0_Addr         DCD	0
CAN_Addr           DCD	0
USB_LP_Addr        DCD	USB_LPIRQHandler
USB_HP_Addr        DCD	0
ADC_Addr           DCD	0
DMA_Addr           DCD	0
EXTIT_Addr         DCD	EXTITIRQHandler
MRCC_Addr          DCD	0
FLASHSMI_Addr      DCD	0
RTC_Addr           DCD	0
TB_Addr            DCD	0

;*******************************************************************************
;                         Exception Handlers
;*******************************************************************************

;*******************************************************************************
;* Macro Name     : SaveContext
;* Description    : This macro used to save the context before entering
;*                  an exception handler.
;* Input          : The range of registers to store.
;* Output         : none
;*******************************************************************************
SaveContext MACRO reg1,reg2
        STMFD  sp!,{reg1-reg2,lr} ; Save The workspace plus the current return
                              ; address lr_ mode into the stack.
        MRS    r1,spsr        ; Save the spsr_mode into r1.
        STMFD  sp!,{r1}       ; Save spsr.
        ENDM

;*******************************************************************************
;* Macro Name     : RestoreContext
;* Description    : This macro used to restore the context to return from
;*                  an exception handler and continue the program execution.
;* Input          : The range of registers to restore.
;* Output         : none
;*******************************************************************************
RestoreContext MACRO reg1,reg2
        LDMFD   sp!,{r1}        ; Restore the saved spsr_mode into r1.
        MSR     spsr_cxsf,r1    ; Restore spsr_mode.
        LDMFD   sp!,{reg1-reg2,pc}^; Return to the instruction following...
                                ; ...the exception interrupt.
        ENDM



;*******************************************************************************
;* Function Name  : IRQHandler
;* Description    : This function called when IRQ exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************
IRQHandler
        SUB    lr,lr,#4       ; Update the link register
        SaveContext r0,r12    ; Save the workspace plus the current
                              ; return address lr_ irq and spsr_irq.
        LDR    lr, =ReturnAddress ; Read the return address.
        LDR    r0, =EIC_base_addr
        LDR    r1, =IVR_off_addr
        ADD    pc,r0,r1      ; Branch to the IRQ handler.
ReturnAddress
                             ; Clear pending bit in EIC (using the proper IPRx)
        LDR    r0, =EIC_base_addr
        LDR    r2, [r0, #CICR_off_addr] ; Get the IRQ channel number
        MOV    r3,#1
        MOV    r3,r3,LSL r2
        STR    r3,[r0, #IPR_off_addr]   ; Clear the corresponding IPR bit.

        RestoreContext r0,r12  ; Restore the context and return to the...
                               ; ...program execution.



;*******************************************************************************
;* Macro Name     : IRQ_to_SYS
;* Description    : This macro used to switch form IRQ mode to SYS mode.
;* Input          : none.
;* Output         : none
;*******************************************************************************
IRQ_to_SYS MACRO
        MSR    cpsr_c,#0x1F
        STMFD  sp!,{lr}
       ENDM

;*******************************************************************************
;* Macro Name     : SYS_to_IRQ
;* Description    : This macro used to switch from SYS mode to IRQ mode.
;* Input          : none.
;* Output         : none
;*******************************************************************************
SYS_to_IRQ MACRO
        LDMFD  sp!,{lr}
        MSR    cpsr_c,#0xD2
        ;MOV    pc,lr
        bx lr
       ENDM



;*******************************************************************************
;* Function Name  : USB_LPIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the USB_LP_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the USB_LP_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
USB_LPIRQHandler
        IRQ_to_SYS
        ldr r0,=USB_LP_IRQHandler
        ldr lr,=USB_LP_IRQHandler_end
        bx r0
USB_LP_IRQHandler_end:
        SYS_to_IRQ



;*******************************************************************************
;* Function Name  : EXTITIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the EXTIT_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the EXTIT_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTITIRQHandler
        IRQ_to_SYS
        ldr r0,=EXTIT_IRQHandler
        ldr lr,=EXTIT_IRQHandler_end
        bx r0
EXTIT_IRQHandler_end:
        SYS_to_IRQ



       LTORG

        END
;******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE*****











