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
	
	.global	T0TIMI_Addr
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

T0TIMI_Addr:	.word	T0TIMIIRQHandler
FLASH_Addr:		.word	FLASHIRQHandler
RCCU_Addr:		.word	RCCUIRQHandler
RTC_Addr:		.word	RTCIRQHandler
WDG_Addr:		.word	WDGIRQHandler
XTI_Addr:		.word	XTIIRQHandler
USBHP_Addr:		.word	USBHPIRQHandler
I2C0ITERR_Addr:	.word	I2C0ITERRIRQHandler
I2C1ITERR_ADDR:	.word	I2C1ITERRIRQHandler
UART0_Addr:		.word	UART0IRQHandler
UART1_Addr:		.word	UART1IRQHandler
UART2_ADDR:		.word	UART2IRQHandler
UART3_ADDR:		.word	UART3IRQHandler
BSPI0_ADDR:		.word	BSPI0IRQHandler
BSPI1_Addr:		.word	BSPI1IRQHandler
I2C0_Addr:		.word	I2C0IRQHandler
I2C1_Addr:		.word	I2C1IRQHandler
CAN_Addr:		.word	CANIRQHandler
ADC12_Addr:		.word	ADC12IRQHandler
T1TIMI_Addr:	.word	T1TIMIIRQHandler
T2TIMI_Addr:	.word	T2TIMIIRQHandler
T3TIMI_Addr:	.word	T3TIMIIRQHandler
				.word	0				/* reserved */
				.word	0				/* reserved */
				.word	0				/* reserved */
HDLC_Addr:		.word	HDLCIRQHandler
USBLP_Addr:		.word	USBLPIRQHandler
				.word	0				/* reserved */
				.word	0				/* reserved */
T0TOI_Addr:		.word	T0TOIIRQHandler
T0OC1_Addr:		.word	T0OC1IRQHandler
T0OC2_Addr:		.word	T0OC2IRQHandler

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
* Function Name  : T0TIMIIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the T0TIMI_IRQHandler function
                   Then to return to IRQ mode after the
                   T0TIMI_IRQHandler function termination.
* Input          : none.
* Output         : none.
*******************************************************************************/

T0TIMIIRQHandler:
		IRQ_to_SYS
		mBLX	T0TIMI_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : FLASHIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the FLASH_IRQHandler function
                   Then to return to IRQ mode after the
                   FLASH_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

FLASHIRQHandler:
		IRQ_to_SYS
		mBLX	FLASH_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : RCCUIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the RCCU_IRQHandler function
                   Then to return to IRQ mode after the
                   RCCU_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

RCCUIRQHandler:
		IRQ_to_SYS
		mBLX	RCCU_IRQHandler
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

/*******************************************************************************
* Function Name  : WDGIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the WDG_IRQHandler function
                   Then to return to IRQ mode after the
                   WDG_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

WDGIRQHandler:
		IRQ_to_SYS
		mBLX	WDG_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : XTIIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the XTI_IRQHandler function
                   Then to return to IRQ mode after the
                   XTI_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

XTIIRQHandler:
		IRQ_to_SYS
		mBLX	XTI_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : USBHPIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the USBHP_IRQHandler function
                   Then to return to IRQ mode after the
                   USBHP_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

USBHPIRQHandler:
		IRQ_to_SYS
		mBLX	USBHP_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : I2C0ITERRIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the I2C0ITERR_IRQHandler function
                   Then to return to IRQ mode after the
                   I2C0ITERR_IRQHandler function termination.
* Input          : none
* Output         : none
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
* Function Name  : I2C0IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the I2C0_IRQHandler function
                   Then to return to IRQ mode after the
                   I2C0_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

I2C0IRQHandler:
		IRQ_to_SYS
		mBLX	I2C0_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : I2C1IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the I2C1_IRQHandler function
                   Then to return to IRQ mode after the
                   I2C1_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

I2C1IRQHandler:
		IRQ_to_SYS
		mBLX	I2C1_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : CANIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the CAN_IRQHandler function
                   Then to return to IRQ mode after the
                   CAN_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

CANIRQHandler:
		IRQ_to_SYS
		mBLX	CAN_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : ADC12IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the ADC12_IRQHandler function
                   Then to return to IRQ mode after the
                   ADC12_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

ADC12IRQHandler:
		IRQ_to_SYS
		mBLX	ADC12_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : T1TIMIIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the T1TIMI_IRQHandler function
                   Then to return to IRQ mode after the
                   T1TIMI_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

T1TIMIIRQHandler:
		IRQ_to_SYS
		mBLX	T1TIMI_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : T2TIMIIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the T2TIMI_IRQHandler function
                   Then to return to IRQ mode after the
                   T2TIMI_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

T2TIMIIRQHandler:
		IRQ_to_SYS
		mBLX	T2TIMI_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : T3TIMIIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the T3TIMI_IRQHandler function
                   Then to return to IRQ mode after the
                   T3TIMI_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

T3TIMIIRQHandler:
		IRQ_to_SYS
		mBLX	T3TIMI_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : HDLCIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the HDLC_IRQHandler function
                   Then to return to IRQ mode after the
                   HDLC_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

HDLCIRQHandler:
		IRQ_to_SYS
		mBLX	HDLC_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : USBLPIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the USBLP_IRQHandler function
                   Then to return to IRQ mode after the
                   USBLP_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

USBLPIRQHandler:
		IRQ_to_SYS
		mBLX	USBLP_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : T0TOIIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the T0TOI_IRQHandler function
                   Then to return to IRQ mode after the
                   T0TOI_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

T0TOIIRQHandler:
		IRQ_to_SYS
		mBLX	T0TOI_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : T0OC1IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the T0OC1_IRQHandler function
                   Then to return to IRQ mode after the
                   T0OC1_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

T0OC1IRQHandler:
		IRQ_to_SYS
		mBLX	T0OC1_IRQHandler
		SYS_to_IRQ

/*******************************************************************************
* Function Name  : T0OC2IRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the T0OC2_IRQHandler function
                   Then to return to IRQ mode after the
                   T0OC2_IRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

T0OC2IRQHandler:
		IRQ_to_SYS
		mBLX	T0OC2_IRQHandler
		SYS_to_IRQ

	.end
