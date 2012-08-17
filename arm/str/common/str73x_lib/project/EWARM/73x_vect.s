;******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
;* File Name          : 73x_vect.s
;* Author             : MCD Application Team
;* Date First Issued  : 09/27/2005 :  V1.0
;* Description        : This File used to initialize the exception and IRQ
;*                      vectors, and to enter/return to/from exceptions handlers.
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
                MODULE	?RESET
		COMMON	INTVEC:CODE(2)			
		CODE32	


EIC_base_addr         DEFINE    0xFFFFFC00 ; EIC base address
CICR_off_addr         DEFINE    0x04       ; Current Interrupt Channel Register
IVR_off_addr          DEFINE    0x18       ; Interrupt Vector Register
IPR0_off_addr         DEFINE    0x40       ; Interrupt Pending Register
IPR1_off_addr         DEFINE    0x44       ; Interrupt Pending Register


;*******************************************************************************
;              Import  the __program_start  from 73x_init.s
;*******************************************************************************

        IMPORT __program_start



;*******************************************************************************
;                      Import exception handlers
;*******************************************************************************

        IMPORT  Undefined_Handler
        IMPORT  SWI_Handler
        IMPORT  Prefetch_Handler
        IMPORT  Abort_Handler
        IMPORT  FIQ_Handler

;*******************************************************************************
;                   Import IRQ handlers from 73x_it.c
;*******************************************************************************

        IMPORT PRCCUCMU_IRQHandler
        IMPORT EXTIT01_IRQHandler
        IMPORT EXTIT02_IRQHandler
        IMPORT EXTIT03_IRQHandler
        IMPORT EXTIT04_IRQHandler
        IMPORT EXTIT05_IRQHandler
        IMPORT EXTIT06_IRQHandler
        IMPORT EXTIT07_IRQHandler
        IMPORT EXTIT08_IRQHandler
        IMPORT EXTIT09_IRQHandler
        IMPORT EXTIT10_IRQHandler
        IMPORT EXTIT11_IRQHandler
        IMPORT EXTIT12_IRQHandler
        IMPORT EXTIT13_IRQHandler
        IMPORT EXTIT14_IRQHandler
        IMPORT EXTIT15_IRQHandler
        IMPORT DMATRERR_IRQHandler
        IMPORT TIM1_IRQHandler
        IMPORT TIM2_IRQHandler
        IMPORT TIM3_IRQHandler
        IMPORT TIM4_IRQHandler
        IMPORT TB0_IRQHandler
        IMPORT TB1_IRQHandler
        IMPORT TB2_IRQHandler
        IMPORT TIM5_IRQHandler
        IMPORT TIM6_IRQHandler
        IMPORT TIM7_IRQHandler
        IMPORT TIM8_IRQHandler
        IMPORT TIM9_IRQHandler
	IMPORT UART2_IRQHandler
        IMPORT UART3_IRQHandler
        IMPORT FLASHEOP_IRQHandler
        IMPORT PWM0_IRQHandler
        IMPORT PWM1_IRQHandler
        IMPORT PWM2_IRQHandler
        IMPORT PWM3_IRQHandler
        IMPORT PWM4_IRQHandler
        IMPORT PWM5_IRQHandler
        IMPORT WIU_IRQHandler
        IMPORT WDGWUT_IRQHandler
        IMPORT BSPI0_IRQHandler
        IMPORT BSPI1_IRQHandler
        IMPORT BSPI2_IRQHandler
        IMPORT UART0_IRQHandler
        IMPORT UART1_IRQHandler
        IMPORT I2C0ITERR_IRQHandler
        IMPORT I2C1ITERR_IRQHandler
        IMPORT I2C0ITDDC_IRQHandler
        IMPORT I2C1ITDDC_IRQHandler
        IMPORT CAN0_IRQHandler
        IMPORT CAN1_IRQHandler
        IMPORT CAN2_IRQHandler
        IMPORT DMA0_IRQHandler
        IMPORT DMA1_IRQHandler
        IMPORT DMA2_IRQHandler
        IMPORT DMA3_IRQHandler
        IMPORT ADC_IRQHandler
        IMPORT RTC_IRQHandler

;*******************************************************************************
;            Export Peripherals IRQ handlers table address
;*******************************************************************************

        EXPORT  PRCCUCMU_Addr

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

PRCCUCMU_Addr     DCD PRCCUCMUIRQHandler
EXTIT01_Addr      DCD EXTIT01IRQHandler
EXTIT02_Addr      DCD EXTIT02IRQHandler
EXTIT03_Addr      DCD EXTIT03IRQHandler
EXTIT04_Addr      DCD EXTIT04IRQHandler
EXTIT05_Addr      DCD EXTIT05IRQHandler
EXTIT06_Addr      DCD EXTIT06IRQHandler
EXTIT07_Addr      DCD EXTIT07IRQHandler
EXTIT08_Addr      DCD EXTIT08IRQHandler
EXTIT09_Addr      DCD EXTIT09IRQHandler
EXTIT10_Addr      DCD EXTIT10IRQHandler
EXTIT11_Addr      DCD EXTIT11IRQHandler
EXTIT12_Addr      DCD EXTIT12IRQHandler
EXTIT13_Addr      DCD EXTIT13IRQHandler
EXTIT14_Addr      DCD EXTIT14IRQHandler
EXTIT15_Addr      DCD EXTIT15IRQHandler
DMATRERR_Addr     DCD DMATRERRIRQHandler
TIM1_Addr         DCD TIM1IRQHandler
TIM2_Addr         DCD TIM2IRQHandler
TIM3_Addr         DCD TIM3IRQHandler
TIM4_Addr         DCD TIM4IRQHandler
TB0_Addr          DCD TB0IRQHandler
TB1_Addr          DCD TB1IRQHandler
TB2_Addr          DCD TB2IRQHandler
TIM5_Addr         DCD TIM5IRQHandler
TIM6_Addr         DCD TIM6IRQHandler
TIM7_Addr         DCD TIM7IRQHandler
TIM8_Addr         DCD TIM8IRQHandler
TIM9_Addr         DCD TIM9IRQHandler
                  DCD 0
                  DCD 0
UART2_Addr        DCD UART2IRQHandler
UART3_Addr        DCD UART3IRQHandler
FlashEOP_Addr     DCD FLASHEOPIRQHandler
PWM0_Addr         DCD PWM0IRQHandler
PWM1_Addr         DCD PWM1IRQHandler
PWM2_Addr         DCD PWM2IRQHandler
PWM3_Addr         DCD PWM3IRQHandler
PWM4_Addr         DCD PWM4IRQHandler
PWM5_Addr         DCD PWM5IRQHandler
WIUI_Addr         DCD WIUIRQHandler
WDGWUT_Addr       DCD WDGWUTIRQHandler
BSPI0_Addr        DCD BSPI0IRQHandler
BSPI1_Addr        DCD BSPI1IRQHandler
BSPI2_Addr        DCD BSPI2IRQHandler
UART0_Addr        DCD UART0IRQHandler
UART1_Addr        DCD UART1IRQHandler
I2C0ITERR_Addr    DCD I2C0ITERRIRQHandler
I2C1ITERR_Addr    DCD I2C1ITERRIRQHandler
                  DCD 0
                  DCD 0
I2C0ITDDC_Addr    DCD I2C0ITDDCIRQHandler
I2C1ITDDC_Addr    DCD I2C1ITDDCIRQHandler
                  DCD 0
                  DCD 0
CAN0_Addr         DCD CAN0IRQHandler
CAN1_Addr         DCD CAN1IRQHandler
CAN2_Addr         DCD CAN2IRQHandler
DMA0_Addr         DCD DMA0IRQHandler
DMA1_Addr         DCD DMA1IRQHandler
DMA2_Addr         DCD DMA2IRQHandler
DMA3_Addr         DCD DMA3IRQHandler
ADC_Addr          DCD ADCIRQHandler
RTC_Addr          DCD RTCIRQHandler





;*******************************************************************************
;                         Exception Handlers
;*******************************************************************************

;*******************************************************************************
;* Macro Name     : SaveContext
;* Description    : This macro used to save the context before entering
;                   an exception handler.
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
;                   an exception handler and continue the program execution.
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
;* Function Name  : UndefinedHandler
;* Description    : This function called when undefined instruction
;                   exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************

UndefinedHandler
        SaveContext r0,r12           ; Save the workspace plus the current
                                     ; return address lr_ und and spsr_und.
        ldr r0, =Undefined_Handler        
        ldr lr,=Undefined_Handler_end
        bx r0                        ; Branch to Undefined_Handler
Undefined_Handler_end:
        RestoreContext r0,r12        ; Return to the instruction following...
                                     ; ...the undefined instruction.

;*******************************************************************************
;* Function Name  : SWIHandler
;* Description    : This function called when SWI instruction executed.
;* Input          : none
;* Output         : none
;*******************************************************************************

SWIHandler
        SaveContext r0,r12       ; Save the workspace plus the current
                                 ; return address lr_ svc and spsr_svc.
        ldr r0, =SWI_Handler        
        ldr lr,=SWI_Handler_end
        bx r0                    ; Branch to SWI_Handler
SWI_Handler_end:
        RestoreContext r0,r12    ; Return to the instruction following...
                                 ; ...the SWI instruction.

;*******************************************************************************
;* Function Name  : IRQHandler
;* Description    : This function called when IRQ exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************

IRQHandler
        SUB    lr,lr,#4          ; Update the link register
        SaveContext r0,r12       ; Save the workspace plus the current
                                 ; return address lr_ irq and spsr_irq.
        LDR    lr, =ReturnAddress; Read the return address.
        LDR    r0, =EIC_base_addr
        LDR    r1, =IVR_off_addr
        ADD    pc,r0,r1         ; Branch to the IRQ handler.
ReturnAddress
                                ; Clear pending bit in EIC (using the proper IPRx)
        LDR    r0, =EIC_base_addr
        LDR    r2, [r0, #CICR_off_addr] ; Get the IRQ channel number
        CMP    r2,#31
        SUBHI  r2, r2, #32
        MOV    r3,#1
        MOV    r3,r3,LSL r2
        STRHI    r3,[r0, #IPR1_off_addr]   ; Clear the corresponding IPR bit.
        STRLS    r3,[r0, #IPR0_off_addr]   ; Clear the corresponding IPR bit.

        RestoreContext r0,r12  ; Restore the context and return to the...
                               ; ...program execution.

;*******************************************************************************
;* Function Name  : PrefetchAbortHandler
;* Description    : This function called when Prefetch Abort
;                   exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************

PrefetchAbortHandler
        SUB    lr,lr,#4       ; Update the link register.
        SaveContext r0,r12    ; Save the workspace plus the current
                              ; return address lr_abt and spsr_abt.
        ldr r0, =Prefetch_Handler        
        ldr lr,=Prefetch_Handler_end
        bx  r0                ; Branch to Prefetch_Handler.
Prefetch_Handler_end:
        RestoreContext r0,r12 ; Return to the instruction following that...
                              ; ...has generated the prefetch abort exception.

;*******************************************************************************
;* Function Name  : DataAbortHandler
;* Description    : This function is called when Data Abort
;                   exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************

DataAbortHandler
        SUB    lr,lr,#8       ; Update the link register.
        SaveContext r0,r12    ; Save the workspace plus the current
                              ; return address lr_ abt and spsr_abt.
        ldr r0, =Abort_Handler        
        ldr lr,=Abort_Handler_end
        bx r0                 ; Branch to Abort_Handler.
Abort_Handler_end:
        RestoreContext r0,r12 ; Return to the instruction following that...
                              ; ...has generated the data abort exception.

;*******************************************************************************
;* Function Name  : FIQHandler
;* Description    : This function is called when FIQ
;                   exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************

FIQHandler
        SUB    lr,lr,#4       ; Update the link register.
        SaveContext r0,r7     ; Save the workspace plus the current
                              ; return address lr_ fiq and spsr_fiq.
        ldr r0, =FIQ_Handler        
        ldr lr,=FIQ_Handler_end
        bx      r0            ; Branch to FIQ_Handler.
FIQ_Handler_end:
        RestoreContext r0,r7  ; Restore the context and return to the...
                              ; ...program execution.

;*******************************************************************************
;* Macro Name     : IRQ_to_SYS
;* Description    : This macro used to switch form IRQ mode to SYS mode
;* Input          : none.
;* Output         : none
;*******************************************************************************
IRQ_to_SYS  MACRO
        MSR    cpsr_c,#0x1F
        STMFD  sp!,{lr}
       ENDM
;*******************************************************************************
;* Macro Name     : SYS_to_IRQ
;* Description    : This macro used to switch from SYS mode to IRQ mode
;* Input          : none.
;* Output         : none
;*******************************************************************************
SYS_to_IRQ MACRO
        LDMFD  sp!,{lr}
        MSR    cpsr_c,#0xD2
        MOV    pc,lr
       ENDM
;*******************************************************************************
;* Function Name  : PRCCUCMUIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PRCCUCMUIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PRCCUCMUIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
PRCCUCMUIRQHandler
        IRQ_to_SYS
        ldr r0, =PRCCUCMU_IRQHandler        
        ldr lr,=PRCCUCMU_IRQHandler_end
        bx     r0
PRCCUCMU_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT01IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT01IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT01IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT01IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT01_IRQHandler        
        ldr lr,=EXTIT01_IRQHandler_end
        bx     r0
EXTIT01_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT02IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT02IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT02IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT02IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT02_IRQHandler        
        ldr lr,=EXTIT02_IRQHandler_end
        bx     r0
EXTIT02_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT03IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT03IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT03IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT03IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT03_IRQHandler        
        ldr lr,=EXTIT03_IRQHandler_end
        bx     r0
EXTIT03_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT04IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT04IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT04IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT04IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT04_IRQHandler        
        ldr lr,=EXTIT04_IRQHandler_end
        bx     r0
EXTIT04_IRQHandler_end:
        SYS_to_IRQ
;*******************************************************************************
;* Function Name  : EXTIT05IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT05IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT05IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT05IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT05_IRQHandler        
        ldr lr,=EXTIT05_IRQHandler_end
        bx     r0
EXTIT05_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT06IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT06IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT06IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT06IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT06_IRQHandler        
        ldr lr,=EXTIT06_IRQHandler_end
        bx     r0
EXTIT06_IRQHandler_end:
        SYS_to_IRQ
;*******************************************************************************
;* Function Name  : EXTIT07IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT07IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT07IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT07IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT07_IRQHandler        
        ldr lr,=EXTIT07_IRQHandler_end
        bx     r0
EXTIT07_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT08IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT08IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT08IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT08IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT08_IRQHandler        
        ldr lr,=EXTIT08_IRQHandler_end
        bx     r0
EXTIT08_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT09IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT09IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT09IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT09IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT09_IRQHandler        
        ldr lr,=EXTIT09_IRQHandler_end
        bx     r0
EXTIT09_IRQHandler_end:
        SYS_to_IRQ
;*******************************************************************************
;* Function Name  : EXTIT10IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT10IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT10IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT10IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT10_IRQHandler        
        ldr lr,=EXTIT10_IRQHandler_end
        bx     r0
EXTIT10_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT11IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT11IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT11IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT11IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT11_IRQHandler        
        ldr lr,=EXTIT11_IRQHandler_end
        bx     r0
EXTIT11_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT12IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT12IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT12IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT12IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT12_IRQHandler        
        ldr lr,=EXTIT12_IRQHandler_end
        bx     r0
EXTIT12_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT13IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT13IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT13IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT13IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT13_IRQHandler        
        ldr lr,=EXTIT13_IRQHandler_end
        bx     r0
EXTIT13_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT14IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT14IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT14IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT14IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT14_IRQHandler        
        ldr lr,=EXTIT14_IRQHandler_end
        bx     r0
EXTIT14_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : EXTIT15IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT15IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT15IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
EXTIT15IRQHandler
        IRQ_to_SYS
        ldr r0, =EXTIT15_IRQHandler        
        ldr lr,=EXTIT15_IRQHandler_end
        bx     r0
EXTIT15_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : DMATRERRIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the DMATRERRIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   DMATRERRIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
DMATRERRIRQHandler
        IRQ_to_SYS
        ldr r0, =DMATRERR_IRQHandler        
        ldr lr,=DMATRERR_IRQHandler_end
        bx     r0
DMATRERR_IRQHandler_end:
       SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM1IRQHandler
        IRQ_to_SYS
        ldr r0, =TIM1_IRQHandler        
        ldr lr,=TIM1_IRQHandler_end
        bx     r0
TIM1_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM2IRQHandler
        IRQ_to_SYS
        ldr r0, =TIM2_IRQHandler        
        ldr lr,=TIM2_IRQHandler_end
        bx     r0
TIM2_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM3IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM3IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM3IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM3IRQHandler
        IRQ_to_SYS
        ldr r0, =TIM3_IRQHandler        
        ldr lr,=TIM3_IRQHandler_end
        bx     r0
TIM3_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM4IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM4IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM4IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM4IRQHandler
        IRQ_to_SYS
        ldr r0, =TIM4_IRQHandler        
        ldr lr,=TIM4_IRQHandler_end
        bx     r0
TIM4_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TB0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TB0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TB0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TB0IRQHandler
        IRQ_to_SYS
        ldr r0, =TB0_IRQHandler        
        ldr lr,=TB0_IRQHandler_end
        bx     r0
TB0_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TB1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TB1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TB1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TB1IRQHandler
        IRQ_to_SYS
        ldr r0, =TB1_IRQHandler        
        ldr lr,=TB1_IRQHandler_end
        bx     r0
TB1_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TB2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TB2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TB2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TB2IRQHandler
        IRQ_to_SYS
        ldr r0, =TB2_IRQHandler        
        ldr lr,=TB2_IRQHandler_end
        bx     r0
TB2_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM5IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM5IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM5IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM5IRQHandler
        IRQ_to_SYS
        ldr r0, =TIM5_IRQHandler        
        ldr lr,=TIM5_IRQHandler_end
        bx     r0
TIM5_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM6IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM6IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM6IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM6IRQHandler
        IRQ_to_SYS
        ldr r0, =TIM6_IRQHandler        
        ldr lr,=TIM6_IRQHandler_end
        bx     r0
TIM6_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM7IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM7IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM7IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM7IRQHandler
        IRQ_to_SYS
        ldr r0, =TIM7_IRQHandler        
        ldr lr,=TIM7_IRQHandler_end
        bx     r0
TIM7_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM8IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM8IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM8IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM8IRQHandler
        IRQ_to_SYS
        ldr r0, =TIM8_IRQHandler        
        ldr lr,=TIM8_IRQHandler_end
        bx     r0
TIM8_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : TIM9IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM9IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM9IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
TIM9IRQHandler
        IRQ_to_SYS
        ldr r0, =TIM9_IRQHandler        
        ldr lr,=TIM9_IRQHandler_end
        bx     r0
TIM9_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : UART2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the UART2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   UART2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
UART2IRQHandler
        IRQ_to_SYS
        ldr r0, =UART2_IRQHandler        
        ldr lr,=UART2_IRQHandler_end
        bx     r0
UART2_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : UART3IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the UART3IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   UART3IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
UART3IRQHandler
        IRQ_to_SYS
        ldr r0, =UART3_IRQHandler        
        ldr lr,=UART3_IRQHandler_end
        bx     r0
UART3_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : FLASHEOPIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the FLASHEOPIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   FLASHEOPIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
FLASHEOPIRQHandler
        IRQ_to_SYS
        ldr r0, =FLASHEOP_IRQHandler        
        ldr lr,=FLASHEOP_IRQHandler_end
        bx     r0
FLASHEOP_IRQHandler_end:

             SYS_to_IRQ

;*******************************************************************************
;* Function Name  : PWM0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
PWM0IRQHandler
        IRQ_to_SYS
        ldr r0, =PWM0_IRQHandler        
        ldr lr,=PWM0_IRQHandler_end
        bx     r0
PWM0_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : PWM1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
PWM1IRQHandler
        IRQ_to_SYS
        ldr r0, =PWM1_IRQHandler        
        ldr lr,=PWM1_IRQHandler_end
        bx     r0
PWM1_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : PWM2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
PWM2IRQHandler
        IRQ_to_SYS
        ldr r0, =PWM2_IRQHandler        
        ldr lr,=PWM2_IRQHandler_end
        bx     r0
PWM2_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : PWM3IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM3IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM3IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
PWM3IRQHandler
        IRQ_to_SYS
        ldr r0, =PWM3_IRQHandler        
        ldr lr,=PWM3_IRQHandler_end
        bx     r0
PWM3_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : PWM4IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM4IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM4IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
PWM4IRQHandler
        IRQ_to_SYS
        ldr r0, =PWM4_IRQHandler        
        ldr lr,=PWM4_IRQHandler_end
        bx     r0
PWM4_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : PWM5IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM5IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM5IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
PWM5IRQHandler
        IRQ_to_SYS
        ldr r0, =PWM5_IRQHandler        
        ldr lr,=PWM5_IRQHandler_end
        bx     r0
PWM5_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : WIUIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the WIUIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   WIUIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
WIUIRQHandler
        IRQ_to_SYS
        ldr r0, =WIU_IRQHandler        
        ldr lr,=WIU_IRQHandler_end
        bx     r0
WIU_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : WDGWUTIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the WDGWUTIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   WDGWUTIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
WDGWUTIRQHandler
        IRQ_to_SYS
        ldr r0, =WDGWUT_IRQHandler        
        ldr lr,=WDGWUT_IRQHandler_end
        bx     r0
WDGWUT_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : BSPI0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the BSPI0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   BSPI0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
BSPI0IRQHandler
        IRQ_to_SYS
        ldr r0, =BSPI0_IRQHandler        
        ldr lr,=BSPI0_IRQHandler_end
        bx     r0
BSPI0_IRQHandler_end:
       SYS_to_IRQ

;*******************************************************************************
;* Function Name  : BSPI1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the BSPI1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   BSPI1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
BSPI1IRQHandler
        IRQ_to_SYS
        ldr r0, =BSPI1_IRQHandler        
        ldr lr,=BSPI1_IRQHandler_end
        bx     r0
BSPI1_IRQHandler_end:
       SYS_to_IRQ

;*******************************************************************************
;* Function Name  : BSPI2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the BSPI2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   BSPI2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
BSPI2IRQHandler
        IRQ_to_SYS
        ldr r0, =BSPI2_IRQHandler        
        ldr lr,=BSPI2_IRQHandler_end
        bx     r0
BSPI2_IRQHandler_end:
       SYS_to_IRQ

;*******************************************************************************
;* Function Name  : UART0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the UART0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   UART0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
UART0IRQHandler
        IRQ_to_SYS
        ldr r0, =UART0_IRQHandler        
        ldr lr,=UART0_IRQHandler_end
        bx     r0
UART0_IRQHandler_end:
       SYS_to_IRQ

;*******************************************************************************
;* Function Name  : UART1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the UART1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   UART1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
UART1IRQHandler
        IRQ_to_SYS
        ldr r0, =UART1_IRQHandler        
        ldr lr,=UART1_IRQHandler_end
        bx     r0
UART1_IRQHandler_end:
       SYS_to_IRQ

;*******************************************************************************
;* Function Name  : I2C0ITERRIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the I2C0ITERRIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   I2C0ITERRIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
I2C0ITERRIRQHandler
        IRQ_to_SYS
        ldr r0, =I2C0ITERR_IRQHandler        
        ldr lr,=I2C0ITERR_IRQHandler_end
        bx     r0
I2C0ITERR_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : I2C1ITERRIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the I2C1ITERRIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   I2C1ITERRIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
I2C1ITERRIRQHandler
        IRQ_to_SYS
        ldr r0, =I2C1ITERR_IRQHandler        
        ldr lr,=I2C1ITERR_IRQHandler_end
        bx     r0
I2C1ITERR_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : I2C0ITDDCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the I2C0ITDDCIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   I2C0ITDDCIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
I2C0ITDDCIRQHandler
        IRQ_to_SYS
        ldr r0, =I2C0ITDDC_IRQHandler        
        ldr lr,=I2C0ITDDC_IRQHandler_end
        bx     r0
I2C0ITDDC_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : I2C1ITDDCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the I2C1ITDDCIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   I2C1ITDDCIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
I2C1ITDDCIRQHandler
        IRQ_to_SYS
        ldr r0, =I2C1ITDDC_IRQHandler        
        ldr lr,=I2C1ITDDC_IRQHandler_end
        bx     r0
I2C1ITDDC_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : CAN0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the CAN0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   CAN0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
CAN0IRQHandler
        IRQ_to_SYS
        ldr r0, =CAN0_IRQHandler        
        ldr lr,=CAN0_IRQHandler_end
        bx     r0
CAN0_IRQHandler_end:
       SYS_to_IRQ

;*******************************************************************************
;* Function Name  : CAN1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the CAN1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   CAN1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
CAN1IRQHandler
        IRQ_to_SYS
        ldr r0, =CAN1_IRQHandler        
        ldr lr,=CAN1_IRQHandler_end
        bx     r0
CAN1_IRQHandler_end:

       SYS_to_IRQ

;*******************************************************************************
;* Function Name  : CAN2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the CAN2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   CAN2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
CAN2IRQHandler
        IRQ_to_SYS
        ldr r0, =CAN2_IRQHandler        
        ldr lr,=CAN2_IRQHandler_end
        bx     r0
CAN2_IRQHandler_end:
       SYS_to_IRQ

;*******************************************************************************
;* Function Name  : DMA0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the DMA0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   DMA0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
DMA0IRQHandler
        IRQ_to_SYS
        ldr r0, =DMA0_IRQHandler        
        ldr lr,=DMA0_IRQHandler_end
        bx     r0
DMA0_IRQHandler_end:
       SYS_to_IRQ

;*******************************************************************************
;* Function Name  : DMA1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the DMA1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   DMA1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
DMA1IRQHandler
        IRQ_to_SYS
        ldr r0, =DMA1_IRQHandler        
        ldr lr,=DMA1_IRQHandler_end
        bx     r0
DMA1_IRQHandler_end:
       SYS_to_IRQ


;*******************************************************************************
;* Function Name  : DMA2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the DMA2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   DMA2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
DMA2IRQHandler
        IRQ_to_SYS
        ldr r0, =DMA2_IRQHandler        
        ldr lr,=DMA2_IRQHandler_end
        bx     r0
DMA2_IRQHandler_end:
       SYS_to_IRQ


;*******************************************************************************
;* Function Name  : DMA3IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the DMA3IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   DMA3IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
DMA3IRQHandler
        IRQ_to_SYS
        ldr r0, =DMA3_IRQHandler        
        ldr lr,=DMA3_IRQHandler_end
        bx     r0
DMA3_IRQHandler_end:
       SYS_to_IRQ


;*******************************************************************************
;* Function Name  : ADCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the ADCIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   ADCIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
ADCIRQHandler
        IRQ_to_SYS
        ldr r0, =ADC_IRQHandler        
        ldr lr,=ADC_IRQHandler_end
        bx     r0
ADC_IRQHandler_end:
        SYS_to_IRQ

;*******************************************************************************
;* Function Name  : RTCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the RTCIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   RTCIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************
RTCIRQHandler
        IRQ_to_SYS
        ldr r0, =RTC_IRQHandler        
        ldr lr,=RTC_IRQHandler_end
        bx     r0
RTC_IRQHandler_end:
       SYS_to_IRQ



       LTORG

        END
;******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****

















