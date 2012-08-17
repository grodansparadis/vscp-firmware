;******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
;* File Name          : 75x_vect.c
;* Author             : MCD Application Team
;* Date First Issued  : 10/01/2006 : V1.0
;* Description        : This file used to initialize the exception and IRQ
;*                      vectors, and to enter/return to/from exceptions handlers.
;*******************************************************************************
; History:
; 10/01/2006 : V1.0
;*******************************************************************************
;* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
;* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
;* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
;* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
;* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
;* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;*******************************************************************************

        PRESERVE8
        AREA Vect, CODE, READONLY

EIC_base_addr         EQU    0xFFFFF800 ; EIC base address
CICR_off_addr         EQU    0x04       ; Current Interrupt Channel Register
IVR_off_addr          EQU    0x18       ; Interrupt Vector Register
IPR_off_addr          EQU    0x40       ; Interrupt Pending Register

        
;*******************************************************************************
;              Import  the Reset_Handler address from 75x_init.s
;*******************************************************************************

        IMPORT  Reset_Handler

        ENTRY

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

        IMPORT WAKUP_IRQHandler
        IMPORT TIM2_OC2_IRQHandler
        IMPORT TIM2_OC1_IRQHandler
        IMPORT TIM2_IC12_IRQHandler
        IMPORT TIM2_UP_IRQHandler
        IMPORT TIM1_OC2_IRQHandler
        IMPORT TIM1_OC1_IRQHandler
        IMPORT TIM1_IC12_IRQHandler
        IMPORT TIM1_UP_IRQHandler
        IMPORT TIM0_OC2_IRQHandler
        IMPORT TIM0_OC1_IRQHandler
        IMPORT TIM0_IC12_IRQHandler
        IMPORT TIM0_UP_IRQHandler
        IMPORT PWM_OC123_IRQHandler
        IMPORT PWM_EM_IRQHandler
        IMPORT PWM_UP_IRQHandler
        IMPORT I2C_IRQHandler
        IMPORT SSP1_IRQHandler
        IMPORT SSP0_IRQHandler
        IMPORT UART2_IRQHandler
        IMPORT UART1_IRQHandler
        IMPORT UART0_IRQHandler
        IMPORT CAN_IRQHandler
        IMPORT USB_LP_IRQHandler
        IMPORT USB_HP_IRQHandler
        IMPORT ADC_IRQHandler
        IMPORT DMA_IRQHandler
        IMPORT EXTIT_IRQHandler
        IMPORT MRCC_IRQHandler
        IMPORT FLASHSMI_IRQHandler
        IMPORT RTC_IRQHandler
        IMPORT TB_IRQHandler

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

Reset_Addr      DCD     Reset_Handler
Undefined_Addr  DCD     UndefinedHandler
SWI_Addr        DCD     SWIHandler
Prefetch_Addr   DCD     PrefetchAbortHandler
Abort_Addr      DCD     DataAbortHandler
                DCD     0               ; Reserved vector
IRQ_Addr        DCD     IRQHandler
FIQ_Addr        DCD     FIQHandler

;*******************************************************************************
;              Peripherals IRQ handlers address table
;*******************************************************************************

WAKUP_Addr         DCD	WAKUPIRQHandler
TIM2_OC2_Addr      DCD	TIM2_OC2IRQHandler
TIM2_OC1_Addr      DCD	TIM2_OC1IRQHandler
TIM2_IC12_Addr     DCD	TIM2_IC12IRQHandler
TIM2_UP_Addr       DCD	TIM2_UPIRQHandler
TIM1_OC2_Addr      DCD	TIM1_OC2IRQHandler
TIM1_OC1_Addr      DCD	TIM1_OC1IRQHandler
TIM1_IC12_Addr     DCD	TIM1_IC12IRQHandler
TIM1_UP_Addr       DCD	TIM1_UPIRQHandler
TIM0_OC2_Addr      DCD	TIM0_OC2IRQHandler
TIM0_OC1_Addr      DCD	TIM0_OC1IRQHandler
TIM0_IC12_Addr     DCD	TIM0_IC12IRQHandler
TIM0_UP_Addr       DCD	TIM0_UPIRQHandler
PWM_OC123_Addr     DCD	PWM_OC123IRQHandler
PWM_EM_Addr        DCD	PWM_EMIRQHandler
PWM_UP_Addr        DCD	PWM_UPIRQHandler
I2C_Addr           DCD	I2CIRQHandler
SSP1_Addr          DCD	SSP1IRQHandler
SSP0_Addr          DCD	SSP0IRQHandler
UART2_Addr         DCD	UART2IRQHandler
UART1_Addr         DCD	UART1IRQHandler
UART0_Addr         DCD	UART0IRQHandler
CAN_Addr           DCD	CANIRQHandler
USB_LP_Addr        DCD	USB_LPIRQHandler
USB_HP_Addr        DCD	USB_HPIRQHandler
ADC_Addr           DCD	ADCIRQHandler
DMA_Addr           DCD	DMAIRQHandler
EXTIT_Addr         DCD	EXTITIRQHandler
MRCC_Addr          DCD	MRCCIRQHandler
FLASHSMI_Addr      DCD	FLASHSMIIRQHandler
RTC_Addr           DCD	RTCIRQHandler
TB_Addr            DCD	TBIRQHandler

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
        MACRO
        SaveContext $reg1,$reg2
        STMFD  sp!,{$reg1-$reg2,lr} ; Save The workspace plus the current return
                              ; address lr_ mode into the stack.
        MRS    r1,spsr        ; Save the spsr_mode into r1.
        STMFD  sp!,{r1}       ; Save spsr.
        MEND

;*******************************************************************************
;* Macro Name     : RestoreContext
;* Description    : This macro used to restore the context to return from
;*                  an exception handler and continue the program execution.
;* Input          : The range of registers to restore.
;* Output         : none
;*******************************************************************************
        MACRO
        RestoreContext $reg1,$reg2
        LDMFD   sp!,{r1}        ; Restore the saved spsr_mode into r1.
        MSR     spsr_cxsf,r1    ; Restore spsr_mode.
        LDMFD   sp!,{$reg1-$reg2,pc}^; Return to the instruction following...
                                ; ...the exception interrupt.
        MEND

;*******************************************************************************
;* Function Name  : UndefinedHandler
;* Description    : This function called when undefined instruction exception
;*                  is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************
UndefinedHandler
        SaveContext r0,r12    ; Save the workspace plus the current
                              ; return address lr_ und and spsr_und.
        BL      Undefined_Handler; Branch to Undefined_Handler
        RestoreContext r0,r12 ; Return to the instruction following...
                              ; ...the undefined instruction.

;*******************************************************************************
;* Function Name  : SWIHandler
;* Description    : This function called when SWI instruction executed.
;* Input          : none
;* Output         : none
;*******************************************************************************
SWIHandler
        SaveContext r0,r12    ; Save the workspace plus the current
                              ; return address lr_ svc and spsr_svc.
        BL       SWI_Handler  ; Branch to SWI_Handler
        RestoreContext r0,r12 ; Return to the instruction following...
                              ; ...the SWI instruction.

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
;* Function Name  : PrefetchAbortHandler
;* Description    : This function called when Prefetch Abort exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************
PrefetchAbortHandler
        SUB    lr,lr,#4        ; Update the link register.
        SaveContext r0,r7      ; Save the workspace plus the current
                               ; return address lr_abt and spsr_abt.
        BL     Prefetch_Handler; Branch to Prefetch_Handler.
        RestoreContext r0,r7   ; Return to the instruction following that...
                               ; ...has generated the prefetch abort exception.

;*******************************************************************************
;* Function Name  : DataAbortHandler
;* Description    : This function is called when Data Abort exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************
DataAbortHandler
        SUB    lr,lr,#8       ; Update the link register.
        SaveContext r0,r12    ; Save the workspace plus the current
                              ; return address lr_ abt and spsr_abt.
        BL     Abort_Handler  ; Branch to Abort_Handler.
        RestoreContext r0,r12 ; Return to the instruction following that...
                              ; ...has generated the data abort exception.

;*******************************************************************************
;* Function Name  : FIQHandler
;* Description    : This function is called when FIQ exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************
FIQHandler
        SUB    lr,lr,#4       ; Update the link register.
        SaveContext r0,r7     ; Save the workspace plus the current
                              ; return address lr_ fiq and spsr_fiq.
        BL      FIQ_Handler   ; Branch to FIQ_Handler.
        RestoreContext r0,r7  ; Restore the context and return to the...
                              ; ...program execution.

;*******************************************************************************
;* Macro Name     : IRQ_to_SYS
;* Description    : This macro used to switch form IRQ mode to SYS mode.
;* Input          : none.
;* Output         : none
;*******************************************************************************
       MACRO
       IRQ_to_SYS
        MSR    cpsr_c,#0x1F
        STMFD  sp!,{lr}
       MEND

;*******************************************************************************
;* Macro Name     : SYS_to_IRQ
;* Description    : This macro used to switch from SYS mode to IRQ mode.
;* Input          : none.
;* Output         : none
;*******************************************************************************
       MACRO
        SYS_to_IRQ
        LDMFD  sp!,{lr}
        MSR    cpsr_c,#0xD2
        MOV    pc,lr
       MEND

;*******************************************************************************
;* Function Name  : WAKUPIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the WAKUP_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the WAKUP_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
WAKUPIRQHandler
        IRQ_to_SYS
        BL     WAKUP_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM2_OC2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM3_OC2_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM2_OC2_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM2_OC2IRQHandler
        IRQ_to_SYS
        BL     TIM2_OC2_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM2_OC1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM2_OC1_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM2_OC1_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM2_OC1IRQHandler
        IRQ_to_SYS
        BL     TIM2_OC1_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM2_IC12IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM2_IC12_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM2_IC12_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM2_IC12IRQHandler
        IRQ_to_SYS
        BL     TIM2_IC12_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM2_UPIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM2_UP_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM3_UP_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM2_UPIRQHandler
        IRQ_to_SYS
        BL     TIM2_UP_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM1_OC2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM1_OC2_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM1_OC2_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM1_OC2IRQHandler
        IRQ_to_SYS
        BL     TIM1_OC2_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM1_OC1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM1_OC1_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM1_OC1_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM1_OC1IRQHandler
        IRQ_to_SYS
        BL     TIM1_OC1_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM1_IC12IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM1_IC12_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM1_IC12_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM1_IC12IRQHandler
        IRQ_to_SYS
        BL     TIM1_IC12_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM1_UPIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM1_UP_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM1_UP_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM1_UPIRQHandler
        IRQ_to_SYS
        BL     TIM1_UP_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM0_OC2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM0_OC2_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM0_OC2_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM0_OC2IRQHandler
        IRQ_to_SYS
        BL     TIM0_OC2_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM0_OC1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM0_OC1_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM0_OC1_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM0_OC1IRQHandler
        IRQ_to_SYS
        BL     TIM0_OC1_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM0_IC12IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM0_IC12_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM0_IC12_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM0_IC12IRQHandler
        IRQ_to_SYS
        BL     TIM0_IC12_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM0_UPIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TIM0_UP_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TIM0_UP_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM0_UPIRQHandler
        IRQ_to_SYS
        BL     TIM0_UP_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : PWM_OC123IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the PWM_OC123_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the PWM_OC123_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
PWM_OC123IRQHandler
        IRQ_to_SYS
        BL     PWM_OC123_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : PWM_EMIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the PWM_EM_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the PWM_EM_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
PWM_EMIRQHandler
        IRQ_to_SYS
        BL     PWM_EM_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : PWM_UPIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the PWM_UP_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the PWM_UP_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
PWM_UPIRQHandler
        IRQ_to_SYS
        BL     PWM_UP_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : I2CIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the I2C_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the I2C_IRQHandler function
;*                  termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
I2CIRQHandler
        IRQ_to_SYS
        BL     I2C_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : SSP1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the SSP1_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the SSP1_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
SSP1IRQHandler
        IRQ_to_SYS
        BL     SSP1_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : SSP0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the SSP0_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the SSP0_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
SSP0IRQHandler
        IRQ_to_SYS
        BL     SSP0_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : UART2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the UART2_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the UART2_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
UART2IRQHandler
        IRQ_to_SYS
        BL     UART2_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : UART1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the UART1_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the UART1_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
UART1IRQHandler
        IRQ_to_SYS
        BL     UART1_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : UART0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the UART0_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the UART0_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
UART0IRQHandler
        IRQ_to_SYS
        BL     UART0_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : CANIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the CAN_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the CAN_IRQHandler function
;*                  termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
CANIRQHandler
        IRQ_to_SYS
        BL     CAN_IRQHandler
        SYS_to_IRQ

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
        BL     USB_LP_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : USB_HPIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the USB_HP_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the USB_HP_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
USB_HPIRQHandler
        IRQ_to_SYS
        BL     USB_HP_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : ADCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the ADC_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the ADC_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
ADCIRQHandler
        IRQ_to_SYS
        BL     ADC_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : DMAIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the DMA_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the DMA_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
DMAIRQHandler
        IRQ_to_SYS
        BL     DMA_IRQHandler
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
        BL     EXTIT_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : MRCCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the MRCC_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the MRCC_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
MRCCIRQHandler
        IRQ_to_SYS
        BL     MRCC_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : FLASHSMIIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the FLASHSMI_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the FLASHSMI_IRQHandler
;*                  function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
FLASHSMIIRQHandler
        IRQ_to_SYS
        BL     FLASHSMI_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : RTCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the RTC_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the RTC_IRQHandler function
;*                  termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
RTCIRQHandler
        IRQ_to_SYS
        BL     RTC_IRQHandler
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TBIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;*                  the TB_IRQHandler function located in 75x_it.c.
;*                  Then to return to IRQ mode after the TB_IRQHandler function
;*                  termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TBIRQHandler
        IRQ_to_SYS
        BL     TB_IRQHandler
        SYS_to_IRQ

       LTORG

        END
;******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE*****
