/***********************************************************************************
*	Copyright 2005 Anglia Design
*	This demo code and associated components are provided as is and has no warranty,
*	implied or otherwise.  You are free to use/modify any of the provided
*	code at your own risk in your applications with the expressed limitation
*	of liability (see below)
* 
*	LIMITATION OF LIABILITY:   ANGLIA OR ANGLIA DESIGNS SHALL NOT BE LIABLE FOR ANY
*	LOSS OF PROFITS, LOSS OF USE, LOSS OF DATA, INTERRUPTION OF BUSINESS, NOR FOR
*	INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND WHETHER UNDER
*	THIS AGREEMENT OR OTHERWISE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
*	Author			: Spencer Oliver
*	Web     		: www.anglia-designs.com
*
***********************************************************************************/

	.equ	EIC_base_addr,	0xFFFFFC00	/* EIC base address. */
	.equ	CICR_off_addr,	0x04		/* Current Interrupt Channel Register. */
	.equ	IVR_off_addr,	0x18		/* Interrupt Vector Register. */
	.equ	IPR0_off_addr,	0x40		/* Interrupt Pending Register. */
	.equ	IPR1_off_addr,	0x44		/* Interrupt Pending Register. */

	/*	set HIRAM 1 for Interrupt Handlers reside in ram Vectors in ram uses 192 bytes
		Vectors/Handlers uses 1452 bytes, normally 0 */

.ifndef HIRAM
	.equ	HIRAM,	0
.endif

.ifndef REMAP
	.equ	REMAP,	0
.endif

	.text
	.arm
	
.if REMAP > 1
	.section .vectram, "ax"
.else
	.section .vectrom, "ax"
.endif
	
	.global	PRCCUCMU_Addr
	.global	Reset_Vec

/* Note: LDR PC instructions are used here, though branch (B) instructions */
/* could also be used, unless the ROM is at an address >32MB. */

/*******************************************************************************
                        Exception vectors
*******************************************************************************/

Reset_Vec:	LDR     pc, Reset_Addr		/* Reset Handler */
Undef_Vec:	LDR     pc, Undefined_Addr
SWI_Vec:	LDR     pc, SWI_Addr
PAbt_Vec:	LDR     pc, Prefetch_Addr
DAbt_Vec:	LDR     pc, Abort_Addr
			NOP							/* Reserved vector */
IRQ_Vec:	LDR     pc, IRQ_Addr
FIQ_Vec:	LDR     pc, FIQ_Addr

/*******************************************************************************
               Exception handlers address table
*******************************************************************************/

Reset_Addr:		.word	_start
Undefined_Addr:	.word	UndefinedHandler
SWI_Addr:		.word	SWIHandler
Prefetch_Addr:	.word	PrefetchHandler
Abort_Addr:		.word	AbortHandler
				.word	0				/* reserved */
IRQ_Addr:		.word	IRQHandler
FIQ_Addr:		.word	FIQHandler

/*******************************************************************************
              Peripherals IRQ handlers address table
*******************************************************************************/

PRCCUCMU_Addr:	.word	PRCCUCMUIRQHandler
EXTIT01_Addr:	.word	EXTIT01IRQHandler
EXTIT02_Addr:	.word	EXTIT02IRQHandler
EXTIT03_Addr:	.word	EXTIT03IRQHandler
EXTIT04_Addr:	.word	EXTIT04IRQHandler
EXTIT05_Addr:	.word	EXTIT05IRQHandler
EXTIT06_Addr:	.word	EXTIT06IRQHandler
EXTIT07_Addr:	.word	EXTIT07IRQHandler
EXTIT08_Addr:	.word	EXTIT08IRQHandler
EXTIT09_Addr:	.word	EXTIT09IRQHandler
EXTIT10_Addr:	.word	EXTIT10IRQHandler
EXTIT11_Addr:	.word	EXTIT11IRQHandler
EXTIT12_Addr:	.word	EXTIT12IRQHandler
EXTIT13_Addr:	.word	EXTIT13IRQHandler
EXTIT14_Addr:	.word	EXTIT14IRQHandler
EXTIT15_Addr:	.word	EXTIT15IRQHandler
DMATRERR_Addr:	.word	DMATRERRIRQHandler
TIM1_Addr:		.word	TIM1IRQHandler
TIM2_Addr:		.word	TIM2IRQHandler
TIM3_Addr:		.word	TIM3IRQHandler
TIM4_Addr:		.word	TIM4IRQHandler
TB0_Addr:		.word	TB0IRQHandler
TB1_Addr:		.word	TB1IRQHandler
TB2_Addr:		.word	TB2IRQHandler
TIM5_Addr:		.word	TIM5IRQHandler
TIM6_Addr:		.word	TIM6IRQHandler
TIM7_Addr:		.word	TIM7IRQHandler
TIM8_Addr:		.word	TIM8IRQHandler
TIM9_Addr:		.word	TIM9IRQHandler
				.word	0				/* reserved */
				.word	0				/* reserved */
UART2_Addr:		.word	UART2IRQHandler
UART3_Addr:		.word	UART3IRQHandler
FlashEOP_Addr:	.word	FLASHEOPIRQHandler
PWM0_Addr:		.word	PWM0IRQHandler
PWM1_Addr:		.word	PWM1IRQHandler
PWM2_Addr:		.word	PWM2IRQHandler
PWM3_Addr:		.word	PWM3IRQHandler
PWM4_Addr:		.word	PWM4IRQHandler
PWM5_Addr:		.word	PWM5IRQHandler
WIUI_Addr:		.word	WIUIRQHandler
WDGWUT_Addr:	.word	WDGWUTIRQHandler
BSPI0_Addr:		.word	BSPI0IRQHandler
BSPI1_Addr:		.word	BSPI1IRQHandler
BSPI2_Addr:		.word	BSPI2IRQHandler
UART0_Addr:		.word	UART0IRQHandler
UART1_Addr:		.word	UART1IRQHandler
I2C0ITERR_Addr:	.word	I2C0ITERRIRQHandler
I2C1ITERR_Addr:	.word	I2C1ITERRIRQHandler
				.word	0				/* reserved */
				.word	0				/* reserved */
I2C0ITDDC_Addr:	.word	I2C0ITDDCIRQHandler
I2C1ITDDC_Addr:	.word	I2C1ITDDCIRQHandler
				.word	0				/* reserved */
				.word	0				/* reserved */
CAN0_Addr:		.word	CAN0IRQHandler
CAN1_Addr:		.word	CAN1IRQHandler
CAN2_Addr:		.word	CAN2IRQHandler
DMA0_Addr:		.word	DMA0IRQHandler
DMA1_Addr:		.word	DMA1IRQHandler
DMA2_Addr:		.word	DMA2IRQHandler
DMA3_Addr:		.word	DMA3IRQHandler
ADC_Addr:		.word	ADCIRQHandler
RTC_Addr:		.word	RTCIRQHandler

/*******************************************************************************
                         Exception Handlers
*******************************************************************************/

.ifeq HIRAM
	.section .text, "ax"
.endif

/*******************************************************************************
* macro for long jump from ram-rom if handlers are
* located in ram and are thumb mode
*******************************************************************************/

.macro	mBLX brAddr
	.ifeq HIRAM
		BL		\brAddr
	.else
		LDR		r0,=\brAddr
		MOV		lr, pc
		BX		r0
	.endif
.endm

/*******************************************************************************
* Macro Name     : SaveContext
* Description    : This macro used to save the context before entering
                   an exception handler.
* Input          : The range of registers to store.
* Output         : none
*******************************************************************************/

.macro	SaveContext reg1 reg2
		STMFD	sp!,{\reg1-\reg2,lr}	/* Save The workspace plus the current return */
										/* address lr_ mode into the stack */
		MRS		r1, spsr				/* Save the spsr_mode into r1 */
		STMFD	sp!, {r1}				/* Save spsr */
.endm

/*******************************************************************************
* Macro Name     : RestoreContext
* Description    : This macro used to restore the context to return from
                   an exception handler and continue the program execution.
* Input          : The range of registers to restore.
* Output         : none
*******************************************************************************/

.macro	RestoreContext reg1 reg2
		LDMFD	sp!, {r1}				/* Restore the saved spsr_mode into r1 */
		MSR		spsr_cxsf, r1			/* Restore spsr_mode */
		LDMFD	sp!, {\reg1-\reg2,pc}^	/* Return to the instruction following */
										/* the exception interrupt */
.endm

/*******************************************************************************
* Function Name  : IRQHandler
* Description    : This function called when IRQ exception is entered.
* Input          : none
* Output         : none
*******************************************************************************/

IRQHandler:
		SUB		lr, lr, #4				/* Update the link register */
		SaveContext r0, r12
		LDR		lr, =ReturnAddress		/* Read the return address. */
		LDR		r0, =EIC_base_addr
		LDR		r1, =IVR_off_addr
		ADD		pc, r0, r1				/* Branch to the IRQ handler. */
ReturnAddress:
										/* Clear pending bit in EIC (using the proper IPRx) */
		LDR		r0, =EIC_base_addr
		LDR		r2, [r0, #CICR_off_addr]/* Get the IRQ channel number. */
		CMP		r2, #31
		SUBHI	r2, r2, #32
		MOV		r3, #1
		MOV		r3, r3, LSL r2
        STRHI	r3, [r0, #IPR1_off_addr] /* Clear the corresponding IPR bit. */
        STRLS	r3, [r0, #IPR0_off_addr] /* Clear the corresponding IPR bit. */
		RestoreContext r0, r12

/*******************************************************************************
* Function Name  : SWIHandler
* Description    : This function called when SWI instruction executed.
* Input          : none
* Output         : none
*******************************************************************************/

SWIHandler:
		SaveContext r0, r12			/* r0 holds swi number */
		MOV 	r1, sp				/* load regs */
		mBLX	SWI_Handler
		RestoreContext r0, r12

/*******************************************************************************
* Function Name  : UndefinedHandler
* Description    : This function called when undefined instruction
                   exception is entered.
* Input          : none
* Output         : none
*******************************************************************************/

UndefinedHandler:
		SaveContext r0, r12
		mBLX	Undefined_Handler
		RestoreContext r0, r12

/*******************************************************************************
* Function Name  : PrefetchAbortHandler
* Description    : This function called when Prefetch Abort
                   exception is entered.
* Input          : none
* Output         : none
*******************************************************************************/

PrefetchHandler:
		SUB		lr, lr, #4			/* Update the link register. */
		SaveContext r0, r12
		mBLX	Prefetch_Handler
		RestoreContext r0, r12

/*******************************************************************************
* Function Name  : DataAbortHandler
* Description    : This function is called when Data Abort
                   exception is entered.
* Input          : none
* Output         : none
*******************************************************************************/

AbortHandler:
		SUB		lr, lr, #8			/* Update the link register. */
		SaveContext r0, r12
		mBLX	Abort_Handler
		RestoreContext r0, r12

/*******************************************************************************
* Function Name  : FIQHandler
* Description    : This function is called when FIQ
                   exception is entered.
* Input          : none
* Output         : none
*******************************************************************************/

FIQHandler:
		SUB		lr, lr, #4			/* Update the link register. */
		SaveContext r0, r7
		mBLX	FIQ_Handler
		RestoreContext r0, r7
	
/*******************************************************************************
* Macro Name     : IRQ_to_SYS
* Description    : This macro used to switch form IRQ mode to SYS mode
* Input          : none.
* Output         : none
*******************************************************************************/

	.macro	IRQ_to_SYS
		MSR		cpsr_c, #0x1F	/* Switch to SYS mode */
		STMFD	sp!, {lr}		/* Save the link register. */
	.endm

/*******************************************************************************
* Macro Name     : SYS_to_IRQ
* Description    : This macro used to switch from SYS mode to IRQ mode
                   then to return to IRQHnadler routine.
* Input          : none.
* Output         : none.
*******************************************************************************/

	.macro	SYS_to_IRQ
		LDMFD	sp!, {lr}		/* Restore the link register. */
		MSR		cpsr_c, #0xD2	/* Switch to IRQ mode. */
		MOV		pc, lr			/* Return to IRQHandler routine to clear the */
								/* pending bit. */
	.endm

/*******************************************************************************
* Function Name  : PRCCUCMUIRQHandler
* Description    : This PRCCUCMUIRQHandler used to switch to SYS mode before entering
                   the T0TIMI_IRQHandler function
                   Then to return to IRQ mode after the
                   PRCCUCMUIRQHandler function termination.
* Input          : none.
* Output         : none.
*******************************************************************************/

PRCCUCMUIRQHandler:
		IRQ_to_SYS
		mBLX	PRCCUCMU_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT01IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT01_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT01_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT01IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT01_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT02IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT02_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT02_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT02IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT02_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT03IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT03_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT03_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT03IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT03_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT04IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT04_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT04_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT04IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT04_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT05IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT05_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT05_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT05IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT05_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT06IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT06_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT06_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT06IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT06_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT07IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT07_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT07_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT07IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT07_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT08IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT08_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT08_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT08IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT08_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT09IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT09_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT09_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT09IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT09_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT10IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT10_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT10_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT10IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT10_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT11IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT11_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT11_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT11IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT11_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT12IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT12_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT12_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT12IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT12_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT13IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT13_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT13_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT13IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT13_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT14IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT14_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT14_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT14IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT14_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTIT15IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXTIT15_IRQHandler function
                   Then to return to IRQ mode after the
                   EXTIT15_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXTIT15IRQHandler:
		IRQ_to_SYS
		mBLX	EXTIT15_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : DMATRERRIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the DMATRERR_IRQHandler function
                   Then to return to IRQ mode after the
                   DMATRERR_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

DMATRERRIRQHandler:
		IRQ_to_SYS
		mBLX	DMATRERR_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM1IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TIM1_IRQHandler function
                   Then to return to IRQ mode after the
                   TIM1_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TIM1IRQHandler:
		IRQ_to_SYS
		mBLX	TIM1_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM2IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TIM2_IRQHandler function
                   Then to return to IRQ mode after the
                   TIM2_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TIM2IRQHandler:
		IRQ_to_SYS
		mBLX	TIM2_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM3IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TIM3_IRQHandler function
                   Then to return to IRQ mode after the
                   TIM3_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TIM3IRQHandler:
		IRQ_to_SYS
		mBLX	TIM3_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM4IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TIM4_IRQHandler function
                   Then to return to IRQ mode after the
                   TIM4_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TIM4IRQHandler:
		IRQ_to_SYS
		mBLX	TIM4_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TB0IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TB0_IRQHandler function
                   Then to return to IRQ mode after the
                   TB0_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TB0IRQHandler:
		IRQ_to_SYS
		mBLX	TB0_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TB1IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TB1_IRQHandler function
                   Then to return to IRQ mode after the
                   TB1_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TB1IRQHandler:
		IRQ_to_SYS
		mBLX	TB1_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TB2IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TB2_IRQHandler function
                   Then to return to IRQ mode after the
                   TB2_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TB2IRQHandler:
		IRQ_to_SYS
		mBLX	TB2_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM5IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TIM5_IRQHandler function
                   Then to return to IRQ mode after the
                   TIM5_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TIM5IRQHandler:
		IRQ_to_SYS
		mBLX	TIM5_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM6IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TIM6_IRQHandler function
                   Then to return to IRQ mode after the
                   TIM6_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TIM6IRQHandler:
		IRQ_to_SYS
		mBLX	TIM6_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM7IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TIM7_IRQHandler function
                   Then to return to IRQ mode after the
                   TIM7_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TIM7IRQHandler:
		IRQ_to_SYS
		mBLX	TIM7_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM8IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TIM8_IRQHandler function
                   Then to return to IRQ mode after the
                   TIM8_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TIM8IRQHandler:
		IRQ_to_SYS
		mBLX	TIM8_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : TIM9IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the TIM9_IRQHandler function
                   Then to return to IRQ mode after the
                   TIM9_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

TIM9IRQHandler:
		IRQ_to_SYS
		mBLX	TIM9_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : UART2IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the UART2_IRQHandler function
                   Then to return to IRQ mode after the
                   UART2_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

UART2IRQHandler:
		IRQ_to_SYS
		mBLX	UART2_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : UART3IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the UART3_IRQHandler function
                   Then to return to IRQ mode after the
                   UART3_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

UART3IRQHandler:
		IRQ_to_SYS
		mBLX	UART3_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : FLASHEOPIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the FLASHEOP_IRQHandler function
                   Then to return to IRQ mode after the
                   FLASHEOP_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

FLASHEOPIRQHandler:
		IRQ_to_SYS
		mBLX	FLASHEOP_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : PWM0IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the PWM0_IRQHandler function
                   Then to return to IRQ mode after the
                   PWM0_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

PWM0IRQHandler:
		IRQ_to_SYS
		mBLX	PWM0_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : PWM1IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the PWM1_IRQHandler function
                   Then to return to IRQ mode after the
                   PWM1_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

PWM1IRQHandler:
		IRQ_to_SYS
		mBLX	PWM1_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : PWM2IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the PWM2_IRQHandler function
                   Then to return to IRQ mode after the
                   PWM2_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

PWM2IRQHandler:
		IRQ_to_SYS
		mBLX	PWM2_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : PWM3IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the PWM3_IRQHandler function
                   Then to return to IRQ mode after the
                   PWM3_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

PWM3IRQHandler:
		IRQ_to_SYS
		mBLX	PWM3_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : PWM4IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the PWM4_IRQHandler function
                   Then to return to IRQ mode after the
                   PWM4_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

PWM4IRQHandler:
		IRQ_to_SYS
		mBLX	PWM4_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : PWM5IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the PWM5_IRQHandler function
                   Then to return to IRQ mode after the
                   PWM5_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

PWM5IRQHandler:
		IRQ_to_SYS
		mBLX	PWM5_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : WIUIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the WIU_IRQHandler function
                   Then to return to IRQ mode after the
                   WIU_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

WIUIRQHandler:
		IRQ_to_SYS
		mBLX	WIU_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : WDGWUTIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the WDGWUT_IRQHandler function
                   Then to return to IRQ mode after the
                   WDGWUT_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

WDGWUTIRQHandler:
		IRQ_to_SYS
		mBLX	WDGWUT_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : BSPI0IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the BSPI0_IRQHandler function
                   Then to return to IRQ mode after the
                   BSPI0_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

BSPI0IRQHandler:
		IRQ_to_SYS
		mBLX	BSPI0_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : BSPI1IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the BSPI1_IRQHandler function
                   Then to return to IRQ mode after the
                   BSPI1_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

BSPI1IRQHandler:
		IRQ_to_SYS
		mBLX	BSPI1_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : BSPI2IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the BSPI2_IRQHandler function
                   Then to return to IRQ mode after the
                   BSPI2_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

BSPI2IRQHandler:
		IRQ_to_SYS
		mBLX	BSPI2_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : UART0IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the UART0_IRQHandler function
                   Then to return to IRQ mode after the
                   UART0_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

UART0IRQHandler:
		IRQ_to_SYS
		mBLX	UART0_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : UART1IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the UART1_IRQHandler function
                   Then to return to IRQ mode after the
                   UART1_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

UART1IRQHandler:
		IRQ_to_SYS
		mBLX	UART1_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : I2C0ITERRIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the I2C0ITERR_IRQHandler function
                   Then to return to IRQ mode after the
                   I2C0ITERR_IRQHandler function termination.
* Input          : none
* Output         : nonez
*******************************************************************************/

I2C0ITERRIRQHandler:
		IRQ_to_SYS
		mBLX	I2C0ITERR_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : I2C1ITERRIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the I2C1ITERR_IRQHandler function
                   Then to return to IRQ mode after the
                   I2C1ITERR_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

I2C1ITERRIRQHandler:
		IRQ_to_SYS
		mBLX	I2C1ITERR_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : I2C0ITDDCIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the I2C0ITDDC_IRQHandler function
                   Then to return to IRQ mode after the
                   I2C0ITDDC_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

I2C0ITDDCIRQHandler:
		IRQ_to_SYS
		mBLX	I2C0ITDDC_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : I2C1ITDDCIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the I2C1ITDDC_IRQHandler function
                   Then to return to IRQ mode after the
                   I2C1ITDDC_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

I2C1ITDDCIRQHandler:
		IRQ_to_SYS
		mBLX	I2C1ITDDC_IRQHandler
		SYS_to_IRQ
		
/*******************************************************************************
* Function Name  : CAN0IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the CAN0_IRQHandler function
                   Then to return to IRQ mode after the
                   CAN0_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

CAN0IRQHandler:
		IRQ_to_SYS
		mBLX	CAN0_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : CAN1IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the CAN1_IRQHandler function
                   Then to return to IRQ mode after the
                   CAN1_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

CAN1IRQHandler:
		IRQ_to_SYS
		mBLX	CAN1_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : CAN2IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the CAN2_IRQHandler function
                   Then to return to IRQ mode after the
                   CAN2_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

CAN2IRQHandler:
		IRQ_to_SYS
		mBLX	CAN2_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : DMA0IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the DMA0_IRQHandler function
                   Then to return to IRQ mode after the
                   DMA0_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

DMA0IRQHandler:
		IRQ_to_SYS
		mBLX	DMA0_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : DMA1IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the DMA1_IRQHandler function
                   Then to return to IRQ mode after the
                   DMA1_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

DMA1IRQHandler:
		IRQ_to_SYS
		mBLX	DMA1_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : DMA2IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the DMA2_IRQHandler function
                   Then to return to IRQ mode after the
                   DMA2_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

DMA2IRQHandler:
		IRQ_to_SYS
		mBLX	DMA2_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : DMA3IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the DMA3_IRQHandler function
                   Then to return to IRQ mode after the
                   DMA3_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

DMA3IRQHandler:
		IRQ_to_SYS
		mBLX	DMA3_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : ADCIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the ADC_IRQHandler function
                   Then to return to IRQ mode after the
                   ADC_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

ADCIRQHandler:
		IRQ_to_SYS
		mBLX	ADC_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : RTCIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the RTC_IRQHandler function
                   Then to return to IRQ mode after the
                   RTC_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

RTCIRQHandler:
		IRQ_to_SYS
		mBLX	RTC_IRQHandler
		SYS_to_IRQ
			
	.end
