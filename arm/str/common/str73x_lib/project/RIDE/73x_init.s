/********************* (C) COPYRIGHT 2005 STMicroelectronics ********************
;* File Name          : 73x_init.s
;* Author             : MCD Application Team
;* Date First Issued  : 09/27/2005 :  V1.0
;* Description        : This module performs:
;*                      - To intialize the exception and IRQ vectors 
;*                      - To enter/return to/from exceptions handlers,                     
;*                      - Stack pointer initialisation for each mode,
;*                      - Interrupt Controller Initialisation,
;*                      - Branches to main in the C library
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



.extern main 
.global PRCCUCMU_Addr
.global ARMIRQ_Enable     
.global ARMFIQ_Enable	
.global ARMIRQ_Disable      
.global ARMFIQ_Disable	   


/* the following are useful for initializing the .data section */
.extern _sidata /* start address for the initialization values of the .data section. defined in linker script */
.extern _sdata /* start address for the .data section. defined in linker script */
.extern _edata /* end address for the .data section. defined in linker script */

/* the following are useful for initializing the .bss section */
.extern _sbss /* start address for the .bss section. defined in linker script */
.extern _ebss /* end address for the .bss section. defined in linker script */



/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs */
.set  Mode_USR, 0x10            /* User Mode */
.set  Mode_FIQ, 0x11            /* FIQ Mode */
.set  Mode_IRQ, 0x12            /* IRQ Mode */
.set  Mode_SVC, 0x13            /* Supervisor Mode */
.set  Mode_ABT, 0x17            /* Abort Mode */
.set  Mode_UNDEF, 0x1B          /* Undefined Mode */
.set  Mode_SYS, 0x1F            /* System Mode */

.equ  I_Bit, 0x80               /* when I bit is set, IRQ is disabled */
.equ  F_Bit, 0x40               ;/* when F bit is set, FIQ is disabled */

/* --- System memory locations */

/* init value for the stack pointer. defined in linker script */
.extern _estack

/* Stack Sizes. The default values are in the linker script, but they can be overriden. */
.extern _UND_Stack_Init
.extern _SVC_Stack_Init
.extern _ABT_Stack_Init
.extern _FIQ_Stack_Init
.extern _IRQ_Stack_Init
.extern _USR_Stack_Init

.extern _UND_Stack_Size
.extern _SVC_Stack_Size
.extern _ABT_Stack_Size
.extern _FIQ_Stack_Size
.extern _IRQ_Stack_Size
.extern _USR_Stack_Size

SVC_Stack           =     _SVC_Stack_Init /*_estack*/           /* 32 byte SVC stack at*/
                                              /* top of memory */
                                              
IRQ_Stack           =     _IRQ_Stack_Init /*SVC_Stack - 32*/     /* followed by IRQ stack */
USR_Stack           =     _USR_Stack_Init /*IRQ_Stack-256*/    /* followed by USR stack */
FIQ_Stack           =     _FIQ_Stack_Init /*USR_Stack-256*/    /* followed by FIQ stack*/
ABT_Stack           =     _ABT_Stack_Init /*FIQ_Stack-32*/     /* followed by ABT stack */
UNDEF_Stack         =     _UND_Stack_Init /*ABT_Stack-32*/     /* followed by UNDEF stack */

/* --- System memory locations*/

CFG_R0_Addr         =     0x40000000   /*; Configuration Register 0 base address*/
CFG_PCGR0_Addr      =     0x40000008

/* --- Stack Addres for each ARM mode*/
/* add FIQ_Stack, ABT_Stack, UNDEF_Stack here if you need them*/


/* --- EIC Registers offsets*/
EIC_base_addr       =    0xFFFFFC00         /* EIC base address*/
ICR_off_addr        =    0x00               /* Interrupt Control register offset*/
CICR_off_addr       =    0x04               /* Current Interrupt Channel Register*/
CIPR_off_addr       =    0x08               /* Current Interrupt Priority Register offset*/
IVR_off_addr        =    0x18               /* Interrupt Vector Register offset*/
FIR_off_addr        =    0x1C               /* Fast Interrupt Register offset*/
IER0_off_addr       =    0x20               /* Interrupt Enable Register 0 offset*/
IER1_off_addr       =    0x24               /* Interrupt Enable Register 1 offset*/
IPR0_off_addr       =    0x40               /* Interrupt Pending Bit Register 0 offset*/
IPR1_off_addr       =    0x44               /* Interrupt Pending Bit Register 0 offset*/
SIR0_off_addr       =    0x60               /* Source Interrupt Register 0*/

/***************************************************************************************/
    

.globl _start
.globl _startup

.text
/*.section .flashtext*/
_startup:
_start:
        ldr     PC, Reset_Addr
        ldr     PC, Undefined_Addr
        ldr     PC, SWI_Addr
        ldr     PC, Prefetch_Addr
        ldr     PC, Abort_Addr
        nop                          /* Reserved vector*/
        ldr     PC, IRQ_Addr
        ldr     PC, FIQ_Addr
        
                 
        
       

Reset_Addr      : .long     Reset_Handler
Undefined_Addr  : .long     UndefinedHandler
SWI_Addr        : .long     SWIHandler
Prefetch_Addr   : .long     PrefetchAbortHandler
Abort_Addr      : .long     DataAbortHandler
                  .long 0      /* Reserved vector*/
IRQ_Addr        : .long     IRQHandler
FIQ_Addr        : .long     FIQHandler

.text
/********************************************************************************
;              Peripherals IRQ handlers address table
;********************************************************************************/

/* execution goes there when an interrupt occurs and there is no associated ISR */
.globl __wrongvector
__wrongvector:
	ldr     PC, __wrongvector_Addr	
__wrongvector_Addr:
	.long 0
	
PRCCUCMU_Addr     :.long  PRCCUCMUIRQHandler
EXTIT01_Addr      :.long  EXTIT01IRQHandler
EXTIT02_Addr      :.long   EXTIT02IRQHandler
EXTIT03_Addr      :.long  EXTIT03IRQHandler
EXTIT04_Addr      :.long  EXTIT04IRQHandler
EXTIT05_Addr      :.long  EXTIT05IRQHandler
EXTIT06_Addr      :.long  EXTIT06IRQHandler
EXTIT07_Addr      :.long  EXTIT07IRQHandler
EXTIT08_Addr      :.long  EXTIT08IRQHandler
EXTIT09_Addr      :.long  EXTIT09IRQHandler
EXTIT10_Addr      :.long  EXTIT10IRQHandler
EXTIT11_Addr      :.long  EXTIT11IRQHandler
EXTIT12_Addr      :.long  EXTIT12IRQHandler
EXTIT13_Addr      :.long  EXTIT13IRQHandler
EXTIT14_Addr      :.long  EXTIT14IRQHandler
EXTIT15_Addr      :.long  EXTIT15IRQHandler
DMATRERR_Addr     :.long  DMATRERRIRQHandler
TIM1_Addr         :.long  TIM1IRQHandler
TIM2_Addr         :.long  TIM2IRQHandler
TIM3_Addr         :.long  TIM3IRQHandler
TIM4_Addr         :.long  TIM4IRQHandler
TB0_Addr          :.long  TB0IRQHandler
TB1_Addr          :.long  TB1IRQHandler
TB2_Addr          :.long  TB2IRQHandler
TIM5_Addr         :.long  TIM5IRQHandler
TIM6_Addr         :.long  TIM6IRQHandler
TIM7_Addr         :.long  TIM7IRQHandler
TIM8_Addr         :.long  TIM8IRQHandler
TIM9_Addr         :.long   TIM9IRQHandler
                  .long   0
                  .long   0
UART2_Addr        :.long  UART2IRQHandler
UART3_Addr        :.long  UART3IRQHandler
FlashEOP_Addr     :.long  FLASHEOPIRQHandler
PWM0_Addr         :.long  PWM0IRQHandler
PWM1_Addr         :.long  PWM1IRQHandler
PWM2_Addr         :.long  PWM2IRQHandler
PWM3_Addr         :.long  PWM3IRQHandler
PWM4_Addr         :.long  PWM4IRQHandler
PWM5_Addr         :.long  PWM5IRQHandler
WIUI_Addr         :.long  WIUIRQHandler
WDGWUT_Addr       :.long  WDGWUTIRQHandler
BSPI0_Addr        :.long  BSPI0IRQHandler
BSPI1_Addr        :.long  BSPI1IRQHandler
BSPI2_Addr        :.long  BSPI2IRQHandler
UART0_Addr        :.long  UART0IRQHandler
UART1_Addr        :.long  UART1IRQHandler
I2C0ITERR_Addr    :.long  I2C0ITERRIRQHandler
I2C1ITERR_Addr    :.long  I2C1ITERRIRQHandler
                  .long   0
                  .long   0
I2C0ITDDC_Addr    :.long  I2C0ITDDCIRQHandler
I2C1ITDDC_Addr    :.long  I2C1ITDDCIRQHandler
                  .long   0
                  .long   0
CAN0_Addr         :.long  CAN0IRQHandler
CAN1_Addr         :.long  CAN1IRQHandler
CAN2_Addr         :.long  CAN2IRQHandler
DMA0_Addr         :.long  DMA0IRQHandler
DMA1_Addr         :.long  DMA1IRQHandler
DMA2_Addr         :.long  DMA2IRQHandler
DMA3_Addr         :.long  DMA3IRQHandler
ADC_Addr          :.long  ADCIRQHandler
RTC_Addr          :.long  RTCIRQHandler

/********************************************************************************
;                         Exception Handlers
;********************************************************************************/

/********************************************************************************
;* Macro Name     : SaveContext
;* Description    : This macro used to save the context before entering
;                   an exception handler.
;* Input          : The range of registers to store.
;* Output         : none
;********************************************************************************/

       .macro SaveContext $r0,$r12
        STMFD  sp!,{r0-r12,lr} /* Save The workspace plus the current return*/
                              /* address lr_ mode into the stack.*/
        MRS    r1,spsr        /* Save the spsr_mode into r1.*/
        STMFD  sp!,{r1}       /* Save spsr.*/
        .endm

/********************************************************************************
;* Macro Name     : RestoreContext
;* Description    : This macro used to restore the context to return from
;                   an exception handler and continue the program execution.
;* Input          : The range of registers to restore.
;* Output         : none
;********************************************************************************/

        .macro RestoreContext $r0,$r12
        LDMFD   sp!,{r1}        /* Restore the saved spsr_mode into r1.*/
        MSR     spsr_cxsf,r1    /* Restore spsr_mode.*/
        LDMFD   sp!,{r0-r12,pc}^/* Return to the instruction following...*/
                                /* ...the exception interrupt.*/
       .endm



/********************************************************************************
;* Function Name  : UndefinedHandler
;* Description    : This function called when undefined instruction
;                   exception is entered.
;* Input          : none
;* Output         : none
;*********************************************************************************/

UndefinedHandler:
        SaveContext r0,r12    /* Save the workspace plus the current*/
                              /* return address lr_ und and spsr_und.*/
        BL      Undefined_Handler/* Branch to Undefined_Handler*/
        RestoreContext r0,r12 /* Return to the instruction following...*/
                              /* ...the undefined instruction.*/

/********************************************************************************
;* Function Name  : SWIHandler
;* Description    : This function called when SWI instruction executed.
;* Input          : none
;* Output         : none
;********************************************************************************/

SWIHandler:
        SaveContext r0,r12    /* Save the workspace plus the current*/
                              /* return address lr_ svc and spsr_svc.*/
        BL       SWI_Handler  /* Branch to SWI_Handler*/
        RestoreContext r0,r12 /* Return to the instruction following...*/
                              /* ...the SWI instruction.*/

/********************************************************************************
;* Function Name  : IRQHandler
;* Description    : This function called when IRQ exception is entered.
;* Input          : none
;* Output         : none
;********************************************************************************/

IRQHandler:
        SUB    lr,lr,#4       /* Update the link register*/
        SaveContext r0,r12    /* Save the workspace plus the current*/
                              /* return address lr_ irq and spsr_irq.*/
        LDR    lr, =ReturnAddress /* Read the return address.*/
        LDR    r0, =EIC_base_addr
        LDR    r1, =IVR_off_addr
        ADD    pc,r0,r1      /* Branch to the IRQ handler.*/
ReturnAddress:
                             /* Clear pending bit in EIC (using the proper IPRx)*/
        LDR    r0, =EIC_base_addr
        LDR    r2, [r0, #CICR_off_addr] /* Get the IRQ channel number*/
        CMP    r2,#31
        SUBHI  r2, r2, #32
        MOV    r3,#1
        MOV    r3,r3,LSL r2
        STRHI    r3,[r0, #IPR1_off_addr]   /* Clear the corresponding IPR bit.*/
        STRLS    r3,[r0, #IPR0_off_addr]   /* Clear the corresponding IPR bit.*/

        RestoreContext r0,r12  /* Restore the context and return to the...*/
                              /* ...program execution./*

/********************************************************************************
;* Function Name  : PrefetchAbortHandler
;* Description    : This function called when Prefetch Abort
;                   exception is entered.
;* Input          : none
;* Output         : none
;********************************************************************************/

PrefetchAbortHandler:
        SUB    lr,lr,#4       /* Update the link register.*/
        SaveContext r0,r12    /* Save the workspace plus the current*/
                              /* return address lr_abt and spsr_abt.*/
        BL     Prefetch_Handler/* Branch to Prefetch_Handler.*/
        RestoreContext r0,r12 /* Return to the instruction following that...*/
                              /* ...has generated the prefetch abort exception.*/

/*******************************************************************************
;* Function Name  : DataAbortHandler
;* Description    : This function is called when Data Abort
;                   exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************/

DataAbortHandler:
        SUB    lr,lr,#8       /* Update the link register.*/
        SaveContext r0,r12    /* Save the workspace plus the current*/
                             /*  return address lr_ abt and spsr_abt.*/
        BL     Abort_Handler  /* Branch to Abort_Handler.*/
        RestoreContext r0,r12 /* Return to the instruction following that...*/
                              /* ...has generated the data abort exception.*/

/*******************************************************************************
;* Function Name  : FIQHandler
;* Description    : This function is called when FIQ
;                   exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************/

FIQHandler:
        SUB    lr,lr,#4       /* Update the link register.*/
        SaveContext r0,r7     /* Save the workspace plus the current*/
                              /* return address lr_ fiq and spsr_fiq.*/
        BL      FIQ_Handler   /* Branch to FIQ_Handler.*/
        RestoreContext r0,r7  /* Restore the context and return to the...*/
                              /* ...program execution.*/

/*******************************************************************************
;* Macro Name     : IRQ_to_SYS
;* Description    : This macro used to switch form IRQ mode to SYS mode
;* Input          : none.
;* Output         : none
;*******************************************************************************/
    .macro IRQ_to_SYS
        MSR    cpsr_c,#0x1F
        STMFD  sp!,{lr}
    .endm
/*******************************************************************************
;* Macro Name     : SYS_to_IRQ
;* Description    : This macro used to switch from SYS mode to IRQ mode
;* Input          : none.
;* Output         : none
;******************************************************************************/
      .macro SYS_to_IRQ
        LDMFD  sp!,{lr}
        MSR    cpsr_c,#0xD2
        MOV    pc,lr
      .endm
/*******************************************************************************
;* Function Name  : PRCCUCMUIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PRCCUCMUIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PRCCUCMUIRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
PRCCUCMUIRQHandler:
        IRQ_to_SYS
        BL     PRCCUCMU_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : EXTIT01IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT01IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT01IRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
EXTIT01IRQHandler:
        IRQ_to_SYS
        BL     EXTIT01_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : EXTIT02IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT02IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT02IRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
EXTIT02IRQHandler:
        IRQ_to_SYS
        BL     EXTIT02_IRQHandler
        SYS_to_IRQ
/*******************************************************************************
;* Function Name  : EXTIT03IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT03IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT03IRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
EXTIT03IRQHandler:
        IRQ_to_SYS
        BL     EXTIT03_IRQHandler
        SYS_to_IRQ
/*******************************************************************************
;* Function Name  : EXTIT04IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT04IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT04IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
EXTIT04IRQHandler:
        IRQ_to_SYS
        BL     EXTIT04_IRQHandler
        SYS_to_IRQ
/*******************************************************************************
;* Function Name  : EXTIT05IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT05IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT05IRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
EXTIT05IRQHandler:
        IRQ_to_SYS
        BL     EXTIT05_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : EXTIT06IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT06IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT06IRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
EXTIT06IRQHandler:
        IRQ_to_SYS
        BL     EXTIT06_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : EXTIT07IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT07IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT07IRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
EXTIT07IRQHandler:
        IRQ_to_SYS
        BL     EXTIT07_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : EXTIT08IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT08IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT08IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
EXTIT08IRQHandler:
        IRQ_to_SYS
        BL     EXTIT08_IRQHandler
        SYS_to_IRQ

/******************************************************************************
;* Function Name  : EXTIT09IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT09IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT09IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
EXTIT09IRQHandler:
        IRQ_to_SYS
        BL     EXTIT09_IRQHandler
        SYS_to_IRQ

/******************************************************************************
;* Function Name  : EXTIT10IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT10IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT10IRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
EXTIT10IRQHandler:
        IRQ_to_SYS
        BL     EXTIT10_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : EXTIT11IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT11IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT11IRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
EXTIT11IRQHandler:
        IRQ_to_SYS
        BL     EXTIT11_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : EXTIT12IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT12IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT12IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
EXTIT12IRQHandler:
        IRQ_to_SYS
        BL     EXTIT12_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : EXTIT13IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT13IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT13IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
EXTIT13IRQHandler:
        IRQ_to_SYS
        BL     EXTIT13_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : EXTIT14IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT14IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT14IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
EXTIT14IRQHandler:
        IRQ_to_SYS
        BL     EXTIT14_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : EXTIT15IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the EXTIT15IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   EXTIT15IRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
EXTIT15IRQHandler:
        IRQ_to_SYS
        BL     EXTIT15_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : DMATRERRIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the DMATRERRIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   DMATRERRIRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
DMATRERRIRQHandler:
        IRQ_to_SYS
        BL     DMATRERR_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
;* Function Name  : TIM1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM1IRQHandler function termination.
;* Input          : none
;* Output         : none
;******************************************************************************/
TIM1IRQHandler:
        IRQ_to_SYS
        BL     TIM1_IRQHandler
        SYS_to_IRQ

/******************************************************************************
;* Function Name  : TIM2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*****************************************************************************/
TIM2IRQHandler:
        IRQ_to_SYS
        BL     TIM2_IRQHandler
        SYS_to_IRQ

/******************************************************************************
;* Function Name  : TIM3IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM3IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM3IRQHandler function termination.
;* Input          : none
;* Output         : none
;*****************************************************************************/
TIM3IRQHandler:
        IRQ_to_SYS
        BL     TIM3_IRQHandler
        SYS_to_IRQ

/*****************************************************************************
;* Function Name  : TIM4IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM4IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM4IRQHandler function termination.
;* Input          : none
;* Output         : none
;*****************************************************************************/
TIM4IRQHandler:
        IRQ_to_SYS
        BL     TIM4_IRQHandler
        SYS_to_IRQ

/*****************************************************************************
;* Function Name  : TB0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TB0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TB0IRQHandler function termination.
;* Input          : none
;* Output         : none
;****************************************************************************/
TB0IRQHandler:
        IRQ_to_SYS
        BL     TB0_IRQHandler
        SYS_to_IRQ

/*****************************************************************************
;* Function Name  : TB1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TB1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TB1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*****************************************************************************/
TB1IRQHandler:
        IRQ_to_SYS
        BL     TB1_IRQHandler
        SYS_to_IRQ

/*****************************************************************************
;* Function Name  : TB2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TB2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TB2IRQHandler function termination.
;* Input          : none
;* Output         : none
;****************************************************************************/
TB2IRQHandler:
        IRQ_to_SYS
        BL     TB2_IRQHandler
        SYS_to_IRQ

/****************************************************************************
;* Function Name  : TIM5IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM5IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM5IRQHandler function termination.
;* Input          : none
;* Output         : none
;****************************************************************************/
TIM5IRQHandler:
        IRQ_to_SYS
        BL     TIM5_IRQHandler
        SYS_to_IRQ

/****************************************************************************
;* Function Name  : TIM6IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM6IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM6IRQHandler function termination.
;* Input          : none
;* Output         : none
;****************************************************************************/
TIM6IRQHandler:
        IRQ_to_SYS
        BL     TIM6_IRQHandler
        SYS_to_IRQ

/****************************************************************************
;* Function Name  : TIM7IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM7IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM7IRQHandler function termination.
;* Input          : none
;* Output         : none
;****************************************************************************/
TIM7IRQHandler:
        IRQ_to_SYS
        BL     TIM7_IRQHandler
        SYS_to_IRQ

/*****************************************************************************
;* Function Name  : TIM8IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM8IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM8IRQHandler function termination.
;* Input          : none
;* Output         : none
;*****************************************************************************/
TIM8IRQHandler:
        IRQ_to_SYS
        BL     TIM8_IRQHandler
        SYS_to_IRQ

/******************************************************************************
;* Function Name  : TIM9IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the TIM9IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   TIM9IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
TIM9IRQHandler:
        IRQ_to_SYS
        BL     TIM9_IRQHandler
        SYS_to_IRQ

/*********************************************************************************
;* Function Name  : UART2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the UART2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   UART2IRQHandler function termination.
;* Input          : none
;* Output         : none
;********************************************************************************/
UART2IRQHandler:
        IRQ_to_SYS
        BL     UART2_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : UART3IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the UART3IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   UART3IRQHandler function termination.
;* Input          : none
;* Output         : none
;********************************************************************************/
UART3IRQHandler:
        IRQ_to_SYS
        BL     UART3_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : FLASHEOPIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the FLASHEOPIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   FLASHEOPIRQHandler function termination.
;* Input          : none
;* Output         : none
;********************************************************************************/
FLASHEOPIRQHandler:
        IRQ_to_SYS
        BL     FLASHEOP_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : PWM0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
PWM0IRQHandler:
        IRQ_to_SYS
        BL     PWM0_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : PWM1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
PWM1IRQHandler:
        IRQ_to_SYS
        BL     PWM1_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : PWM2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
PWM2IRQHandler:
        IRQ_to_SYS
        BL     PWM2_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : PWM3IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM3IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM3IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
PWM3IRQHandler:
        IRQ_to_SYS
        BL     PWM3_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : PWM4IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM4IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM4IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
PWM4IRQHandler:
        IRQ_to_SYS
        BL     PWM4_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : PWM5IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the PWM5IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   PWM5IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
PWM5IRQHandler:
        IRQ_to_SYS
        BL     PWM5_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : WIUIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the WIUIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   WIUIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
WIUIRQHandler:
        IRQ_to_SYS
        BL     WIU_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : WDGWUTIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the WDGWUTIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   WDGWUTIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
WDGWUTIRQHandler:
        IRQ_to_SYS
        BL     WDGWUT_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : BSPI0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the BSPI0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   BSPI0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
BSPI0IRQHandler:
        IRQ_to_SYS
        BL     BSPI0_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : BSPI1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the BSPI1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   BSPI1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
BSPI1IRQHandler:
        IRQ_to_SYS
        BL     BSPI1_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : BSPI2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the BSPI2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   BSPI2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
BSPI2IRQHandler:
        IRQ_to_SYS
        BL     BSPI2_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : UART0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the UART0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   UART0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
UART0IRQHandler:
        IRQ_to_SYS
        BL     UART0_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : UART1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the UART1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   UART1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
UART1IRQHandler:
        IRQ_to_SYS
        BL     UART1_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : I2C0ITERRIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the I2C0ITERRIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   I2C0ITERRIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
I2C0ITERRIRQHandler:
        IRQ_to_SYS
        BL     I2C0ITERR_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : I2C1ITERRIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the I2C1ITERRIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   I2C1ITERRIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
I2C1ITERRIRQHandler:
        IRQ_to_SYS
        BL     I2C1ITERR_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : I2C0ITDDCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the I2C0ITDDCIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   I2C0ITDDCIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
I2C0ITDDCIRQHandler:
        IRQ_to_SYS
        BL     I2C0ITDDC_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : I2C1ITDDCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the I2C1ITDDCIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   I2C1ITDDCIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
I2C1ITDDCIRQHandler:
        IRQ_to_SYS
        BL     I2C1ITDDC_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : CAN0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the CAN0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   CAN0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
CAN0IRQHandler:
        IRQ_to_SYS
        BL     CAN0_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : CAN1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the CAN1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   CAN1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
CAN1IRQHandler:
        IRQ_to_SYS
        BL     CAN1_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : CAN2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the CAN2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   CAN2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
CAN2IRQHandler:
        IRQ_to_SYS
        BL     CAN2_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : DMA0IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the DMA0IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   DMA0IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
DMA0IRQHandler:
        IRQ_to_SYS
        BL     DMA0_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : DMA1IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the DMA1IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   DMA1IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
DMA1IRQHandler:
        IRQ_to_SYS
        BL     DMA1_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : DMA2IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the DMA2IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   DMA2IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
DMA2IRQHandler:
        IRQ_to_SYS
        BL     DMA2_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : DMA3IRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the DMA3IRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   DMA3IRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
DMA3IRQHandler:
        IRQ_to_SYS
        BL     DMA3_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : ADCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the ADCIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   ADCIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
ADCIRQHandler:
        IRQ_to_SYS
        BL     ADC_IRQHandler
        SYS_to_IRQ

/********************************************************************************
;* Function Name  : RTCIRQHandler
;* Description    : This function used to switch to SYS mode before entering
;                   the RTCIRQHandler function located in 73x_it.c.
;                   Then to return to IRQ mode after the
;                   RTCIRQHandler function termination.
;* Input          : none
;* Output         : none
;*******************************************************************************/
RTCIRQHandler:
        IRQ_to_SYS
        BL     RTC_IRQHandler
        SYS_to_IRQ
/***********************************************************************************/
Reset_Handler:
        LDR     pc, =NextInst

NextInst:


        NOP   /* Wait for OSC stabilization*/
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
 /* Reset all Peripheral Clocks except RAM*/
       /* This is usefull only when using debugger to Reset\Run the application*/

        MOV r0,#1  
        LDR r1,=CFG_PCGR0_Addr
        STR r0,[r1]
        MOV r0,#0
        STR r0,[r1,#4]                    



 /* Enter each mode in turn and set up the stack pointer*/

       

        MSR     CPSR_c, #Mode_FIQ|I_Bit|F_Bit /* No interrupts*/
        ldr     sp, =FIQ_Stack

        MSR     CPSR_c, #Mode_IRQ|I_Bit|F_Bit /* No interrupts*/
        ldr     sp, =IRQ_Stack

        MSR     CPSR_c, #Mode_ABT|I_Bit|F_Bit /* No interrupts*/
        ldr     sp, =ABT_Stack

        MSR     CPSR_c, #Mode_UNDEF|I_Bit|F_Bit /* No interrupts*/
        ldr     sp,  =UNDEF_Stack

        MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit /* No interrupts*/
        ldr     sp, =_estack


  /* FLASH_RAM_REMAP
  ; -------------------------------------------------------------------------------
  ; Description  :   On reset, an aliased copy of FLASH  is at 0x0.
  ;                  Perform FLASH/RAM remapping, if required in order to have
  ;                  an aliased copy of RAM at 0x0
  ; -------------------------------------------------------------------------------*/

  /*    LDR     r1, =CFG_R0_Addr
        LDR     r0, [r1]
        ORR     r0, r0, #0x1
        STR     r0, [r1]

 */

/* RAM is now at 0x0.*/
/* The exception vectors  must be copied from FLASH to the RAM*/




	/*EIC initialization
	;-------------------------------------------------------------------------------
	;Description  : Initialize the EIC as following :
	;              - IRQ disabled
	;              - FIQ disabled
	;              - IVR contain the load PC opcode (0xF59FF00)
	;              - All channels are disabled
	;              - All channels priority equal to 0
	;              - All SIR registers contain offset to the related IRQ table entry
	;-------------------------------------------------------------------------------*/

                       
        /*Enable the EIC Clock*/

        LDR     r1, =CFG_PCGR0_Addr
        LDR     r0, [r1]
        ORR     r0, r0, #0x20000000
        STR     r0, [r1] 


        NOP   /* Wait for EIC stabilization*/
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
      


        LDR     r3, =EIC_base_addr
        LDR     r4, =0x00000000
        STR     r4, [r3, #ICR_off_addr]    /* Disable FIQ and IRQ*/
        STR     r4, [r3, #IER0_off_addr]   /* Disable 0 ->31  interrupts channels*/
        STR     r4, [r3, #IER1_off_addr]   /* Disable 32->63  interrupts channels*/
        LDR     r4, =0xFFFFFFFF
        STR     r4, [r3, #IPR0_off_addr]   /* Clear 0 ->31 IRQ pending bits*/
        STR     r4, [r3, #IPR1_off_addr]   /* Clear 32->63 IRQ pending bits*/
        LDR     r4, =0x18
        STR     r4, [r3, #FIR_off_addr]    /* Disable FIQ channels and clear FIQ pending bits*/
        LDR     r4, =0x00000000
        STR     r4, [r3, #CIPR_off_addr]   /* Reset the current priority register*/
        LDR     r4, =0xE59F0000
        STR     r4, [r3, #IVR_off_addr]    /* Write the LDR pc,pc,#offset instruction code in IVR[31:16]*/
        LDR     r2,= 64                    /* 64 Channel to initialize*/
        LDR     r0, =PRCCUCMU_Addr          /* Read the address of the IRQs address table*/
        LDR     r1, =0x00000FFF
        AND     r0,r0,r1
        LDR     r5,=SIR0_off_addr         /* Read SIR0 address*/
        SUB     r4,r0,#8                  /* subtract 8 for prefetch*/
        LDR     r1, =0xF3E8               /* add the offset to the 0x00000000 address(IVR address + 3E8 = 0x00000000)*/
                                          /* 0xF3E8 used to complete the LDR pc,offset opcode*/
        ADD     r1,r4,r1                  /* compute the jump offset*/
EIC_INI:
        MOV     r4, r1, LSL #16           /* Left shift the result*/
        STR     r4, [r3, r5]              /* Store the result in SIRx register*/
        ADD     r1, r1, #4                /* Next IRQ address*/
        ADD     r5, r5, #4                /* Next SIR*/
        SUBS    r2, r2, #1                /* Decrement the number of SIR registers to initialize*/
        BNE     EIC_INI                   /* If more then continue*/





/* --- Now change to USR mode and set up User mode stack, if needed*/
        
        MSR     CPSR_c, #Mode_SYS               /* IRQs & FIQs are now enabled*/
        ldr    sp, =USR_Stack 


        /* copy the initial values for .data section from FLASH to RAM */
	ldr	R1, =_sidata
	ldr	R2, =_sdata
	ldr	R3, =_edata
_reset_inidata_loop:
	cmp	R2, R3
	ldrlO	R0, [R1], #4
	strlO	R0, [R2], #4
	blO	_reset_inidata_loop

	/* Clear the .bss section */
	mov   r0,#0						/* get a zero */
	ldr   r1,=_sbss				/* point to bss start */
	ldr   r2,=_ebss				/* point to bss end */
_reset_inibss_loop:
	cmp   r1,r2						/* check if some data remains to clear */
	strlo r0,[r1],#4				/* clear 4 bytes */
	blo   _reset_inibss_loop	/* loop until done */

/* --- Now enter the C code*/
        B       main   /* Note : use B not BL, because an application will*/
                         /* never return this way*/


/* --- The following functions are exported to the EIC configuration file*/
/* --- and may be used by application whenever they are needed*/
        

ARMIRQ_Disable:      /* Disable the ARM7 core IRQ line.*/

 MRS  r0,CPSR
 ORR  r0, r0, #I_Bit	
 MSR  CPSR_c, r0
 BX lr



ARMFIQ_Disable:      /* Disable the ARM7 core FIQ line.*/

 MRS  r0,CPSR
 ORR  r0, r0, #F_Bit	
 MSR  CPSR_c, r0
 BX lr




ARMIRQ_Enable:       /* Enable the ARM7 core IRQ line.*/

 MRS  r0,CPSR
 BIC  r0, r0,#I_Bit	
 MSR  CPSR_c, r0
 BX lr





ARMFIQ_Enable:       /* Enable the ARM7 core FIQ line.*/

 MRS  r0,CPSR
 BIC  r0, r0,#F_Bit	
 MSR  CPSR_c, r0
 BX lr

/****************** (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/












