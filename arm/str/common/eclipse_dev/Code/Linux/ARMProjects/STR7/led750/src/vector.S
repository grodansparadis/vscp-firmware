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

	.equ	EIC_base_addr,	0xFFFFF800	/* EIC base address. */
	.equ	CICR_off_addr,	0x04		/* Current Interrupt Channel Register. */
	.equ	IVR_off_addr,	0x18		/* Interrupt Vector Register. */
	.equ	IPR_off_addr,	0x40		/* Interrupt Pending Register. */

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
	
	.global	WAKUP_Addr
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

WAKUP_Addr:		.word	WAKUPIRQHandler
TIM2_OC2_Addr:	.word	TIM2_OC2IRQHandler
TIM2_OC1_Addr:	.word	TIM2_OC1IRQHandler
TIM2_IC12_Addr:	.word	TIM2_IC12IRQHandler
TIM2_UP_Addr:	.word	TIM2_UPIRQHandler
TIM1_OC2_Addr:	.word	TIM1_OC2IRQHandler
TIM1_OC1_Addr:	.word	TIM1_OC1IRQHandler
TIM1_IC12_Addr:	.word	TIM1_IC12IRQHandler
TIM1_UP_Addr:	.word	TIM1_UPIRQHandler
TIM0_OC2_Addr:	.word	TIM0_OC2IRQHandler
TIM0_OC1_Addr:	.word	TIM0_OC1IRQHandler
TIM0_IC12_Addr:	.word	TIM0_IC12IRQHandler
TIM0_UP_Addr:	.word	TIM0_UPIRQHandler
PWM_OC123_Addr:	.word	PWM_OC123IRQHandler
PWM_EM_Addr:	.word	PWM_EMIRQHandler
PWM_UP_Addr:	.word	PWM_UPIRQHandler
I2C_Addr:		.word	I2CIRQHandler
SSP1_Addr:		.word	SSP1IRQHandler
SSP0_Addr:		.word	SSP0IRQHandler
UART2_Addr:		.word	UART2IRQHandler
UART1_Addr:		.word	UART1IRQHandler
UART0_Addr:		.word	UART0IRQHandler
CAN_Addr:		.word	CANIRQHandler
USB_LP_Addr:	.word	USB_LPIRQHandler
USB_HP_Addr:	.word	USB_HPIRQHandler
ADC_Addr:		.word	ADCIRQHandler
DMA_Addr:		.word	DMAIRQHandler
EXTIT_Addr:		.word	EXTITIRQHandler
MRCC_Addr:		.word	MRCCIRQHandler
FLASHSMI_Addr:	.word	FLASHSMIIRQHandler
RTC_Addr:		.word	RTCIRQHandler
TB_Addr:		.word	TBIRQHandler

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
		MOV		r3, #1
		MOV		r3, r3, LSL r2
		STR		r3, [r0, #IPR_off_addr]	/* Clear the corresponding IPR bit. */
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
* Function Name  : WAKUPIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the WAKUP_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the WAKUP_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
WAKUPIRQHandler:
		IRQ_to_SYS
        mBLX	WAKUP_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM2_OC2IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM3_OC2_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM2_OC2_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM2_OC2IRQHandler:
        IRQ_to_SYS
        mBLX	TIM2_OC2_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM2_OC1IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM2_OC1_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM2_OC1_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM2_OC1IRQHandler:
        IRQ_to_SYS
        mBLX	TIM2_OC1_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM2_IC12IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM2_IC12_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM2_IC12_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM2_IC12IRQHandler:
        IRQ_to_SYS
        mBLX	TIM2_IC12_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM2_UPIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM2_UP_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM3_UP_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM2_UPIRQHandler:
        IRQ_to_SYS
        mBLX	TIM2_UP_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM1_OC2IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM1_OC2_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM1_OC2_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM1_OC2IRQHandler:
        IRQ_to_SYS
        mBLX	TIM1_OC2_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM1_OC1IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM1_OC1_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM1_OC1_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM1_OC1IRQHandler:
        IRQ_to_SYS
        mBLX	TIM1_OC1_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM1_IC12IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM1_IC12_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM1_IC12_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM1_IC12IRQHandler:
        IRQ_to_SYS
        mBLX	TIM1_IC12_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM1_UPIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM1_UP_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM1_UP_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM1_UPIRQHandler:
		IRQ_to_SYS
		mBLX	TIM1_UP_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM0_OC2IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM0_OC2_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM0_OC2_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM0_OC2IRQHandler:
        IRQ_to_SYS
        mBLX	TIM0_OC2_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM0_OC1IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM0_OC1_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM0_OC1_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM0_OC1IRQHandler:
        IRQ_to_SYS
        mBLX	TIM0_OC1_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM0_IC12IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM0_IC12_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM0_IC12_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM0_IC12IRQHandler:
        IRQ_to_SYS
        mBLX	TIM0_IC12_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TIM0_UPIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TIM0_UP_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TIM0_UP_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
TIM0_UPIRQHandler:
        IRQ_to_SYS
        mBLX	TIM0_UP_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : PWM_OC123IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the PWM_OC123_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the PWM_OC123_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
PWM_OC123IRQHandler:
        IRQ_to_SYS
        mBLX	PWM_OC123_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : PWM_EMIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the PWM_EM_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the PWM_EM_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
PWM_EMIRQHandler:
        IRQ_to_SYS
        mBLX	PWM_EM_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : PWM_UPIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the PWM_UP_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the PWM_UP_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
PWM_UPIRQHandler:
        IRQ_to_SYS
        mBLX	PWM_UP_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : I2CIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the I2C_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the I2C_IRQHandler function
*                  termination.
* Input          : none
* Output         : none
*******************************************************************************/
I2CIRQHandler:
        IRQ_to_SYS
        mBLX	I2C_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : SSP1IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the SSP1_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the SSP1_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
SSP1IRQHandler:
        IRQ_to_SYS
        mBLX	SSP1_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : SSP0IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the SSP0_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the SSP0_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
SSP0IRQHandler:
        IRQ_to_SYS
        mBLX	SSP0_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : UART2IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the UART2_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the UART2_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
UART2IRQHandler:
        IRQ_to_SYS
        mBLX	UART2_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : UART1IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the UART1_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the UART1_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
UART1IRQHandler:
        IRQ_to_SYS
        mBLX	UART1_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : UART0IRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the UART0_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the UART0_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
UART0IRQHandler:
        IRQ_to_SYS
        mBLX	UART0_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : CANIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the CAN_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the CAN_IRQHandler function
*                  termination.
* Input          : none
* Output         : none
*******************************************************************************/
CANIRQHandler:
        IRQ_to_SYS
        mBLX	CAN_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : USB_LPIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the USB_LP_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the USB_LP_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
USB_LPIRQHandler:
        IRQ_to_SYS
        mBLX	USB_LP_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : USB_HPIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the USB_HP_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the USB_HP_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
USB_HPIRQHandler:
        IRQ_to_SYS
        mBLX	USB_HP_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : ADCIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the ADC_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the ADC_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
ADCIRQHandler:
        IRQ_to_SYS
        mBLX	ADC_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : DMAIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the DMA_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the DMA_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
DMAIRQHandler:
        IRQ_to_SYS
        mBLX	DMA_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXTITIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the EXTIT_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the EXTIT_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
EXTITIRQHandler:
        IRQ_to_SYS
        mBLX	EXTIT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : MRCCIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the MRCC_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the MRCC_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
MRCCIRQHandler:
        IRQ_to_SYS
        mBLX	MRCC_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : FLASHSMIIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the FLASHSMI_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the FLASHSMI_IRQHandler
*                  function termination.
* Input          : none
* Output         : none
*******************************************************************************/
FLASHSMIIRQHandler:
        IRQ_to_SYS
        mBLX	FLASHSMI_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : RTCIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the RTC_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the RTC_IRQHandler function
*                  termination.
* Input          : none
* Output         : none
*******************************************************************************/
RTCIRQHandler:
        IRQ_to_SYS
        mBLX	RTC_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : TBIRQHandler
* Description    : This function used to switch to SYS mode before entering
*                  the TB_IRQHandler function located in 75x_it.c.
*                  Then to return to IRQ mode after the TB_IRQHandler function
*                  termination.
* Input          : none
* Output         : none
*******************************************************************************/
TBIRQHandler:
        IRQ_to_SYS
        mBLX	TB_IRQHandler
        SYS_to_IRQ

	.end
