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
	
	.global	EXT03IT_Addr
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

EXT03IT_Addr:	.word	EXT03ITIRQHandler
EXT4IT_Addr:	.word	EXT4ITIRQHandler
WIUIT_Addr:		.word	WIUITIRQHandler
EFT1IT_Addr:	.word	EFT1ITIRQHandler
EFT2IT_Addr:	.word	EFT2ITIRQHandler
UART1IT_Addr:	.word	UART1ITIRQHandler
UART2IT_Addr:	.word	UART2ITIRQHandler
DMAIT_Addr:		.word	DMAITIRQHandler
DMAS0IT_Addr:	.word	DMAS0ITIRQHandler
DMAS1IT_Addr:	.word	DMAS1ITIRQHandler
USBHPIT_Addr:	.word	USBHPITIRQHandler
USBLPIT_Addr:	.word	USBLPITIRQHandler
CANIT_Addr:		.word	CANITIRQHandler
BSPI1IT_Addr:	.word	BSPI1ITIRQHandler
BSPI2IT_Addr:	.word	BSPI2ITIRQHandler
IDEPIT_Addr:	.word	IDEPITIRQHandler
RTCIT_Addr:		.word	RTCITIRQHandler
				.word	0				/* reserved */
				.word	0				/* reserved */
				.word	0				/* reserved */
				.word	0				/* reserved */
				.word	0				/* reserved */
				.word	0				/* reserved */
ADCIT_Addr:		.word	ADCITIRQHandler
				.word	0				/* reserved */
				.word	0				/* reserved */
				.word	0				/* reserved */
				.word	0				/* reserved */
				.word	0				/* reserved */
EFT2OCA_Addr:	.word	EFT2OCAIRQHandler
EFT2OCB_Addr:	.word	EFT2OCBIRQHandler
WDGIT_Addr:		.word	WDGITIRQHandler

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
* Function Name  : EXT03ITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXT03ITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   EXT03ITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXT03ITIRQHandler:
        IRQ_to_SYS
        mBLX	EXT03IT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : EXT4ITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EXT4ITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   EXT4ITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EXT4ITIRQHandler:
        IRQ_to_SYS
        mBLX	EXT4IT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : WIUITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the WIUITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   WIUITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

WIUITIRQHandler:
        IRQ_to_SYS
        mBLX	WIUIT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : EFT1ITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EFT1ITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   EFT1ITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EFT1ITIRQHandler:
        IRQ_to_SYS
        mBLX	EFT1IT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : EFT2ITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EFT2ITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   EFT2ITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EFT2ITIRQHandler:
        IRQ_to_SYS
        mBLX	EFT2IT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : UART1ITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the UART1IRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   UART1ITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

UART1ITIRQHandler:
        IRQ_to_SYS
        mBLX	UART1IT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : UART2ITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the UART2ITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   UART2ITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

UART2ITIRQHandler:
        IRQ_to_SYS
        mBLX	UART2IT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : DMAITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the DMAITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   DMAITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

DMAITIRQHandler:
        IRQ_to_SYS
        mBLX	DMAIT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : DMAS0ITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the DMAS0ITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   DMAS0ITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

DMAS0ITIRQHandler:
        IRQ_to_SYS
        mBLX	DMAS0IT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : DMAS1ITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the DMAS1ITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   DMAS1ITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

DMAS1ITIRQHandler:
        IRQ_to_SYS
        mBLX	DMAS1IT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : USBHPITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the USBHPITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   USBHPITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

USBHPITIRQHandler:
        IRQ_to_SYS
        mBLX	USBHPIT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : USBLPITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the USBLPITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   USBLPITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

USBLPITIRQHandler:
        IRQ_to_SYS
        mBLX	USBLPIT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : CANITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the CANITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   CANITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

CANITIRQHandler:
        IRQ_to_SYS
        mBLX	CANIT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : BSPI1ITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the BSPI1ITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   BSPI1ITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

BSPI1ITIRQHandler:
        IRQ_to_SYS
        mBLX	BSPI1IT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : BSPI2ITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the BSPI2ITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   BSPI2ITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

BSPI2ITIRQHandler:
        IRQ_to_SYS
        mBLX	BSPI2IT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : IDEPITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the IDEPITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   IDEPITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

IDEPITIRQHandler:
        IRQ_to_SYS
        mBLX	IDEPIT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : RTCITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the RTCITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   RTCITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

RTCITIRQHandler:
        IRQ_to_SYS
        mBLX	RTCIT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : ADCITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the ADCITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   ADCITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

ADCITIRQHandler:
        IRQ_to_SYS
        mBLX	ADCIT_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : EFT2OCAIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EFT2OCAIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   EFT2OCAIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EFT2OCAIRQHandler:
        IRQ_to_SYS
        mBLX	EFT2OCA_IRQHandler
        SYS_to_IRQ

/*******************************************************************************
* Function Name  : EFT2OCBIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the EFT2OCBIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   EFT2OCBIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

EFT2OCBIRQHandler:
        IRQ_to_SYS
        mBLX	EFT2OCB_IRQHandler
        SYS_to_IRQ
        
/*******************************************************************************
* Function Name  : WDGITIRQHandler
* Description    : This function used to switch to SYS mode before entering
                   the WDGITIRQHandler function located in 720_it.c.
                   Then to return to IRQ mode after the
                   WDGITIRQHandler function termination.
* Input          : none
* Output         : none
*******************************************************************************/

WDGITIRQHandler:
        IRQ_to_SYS
        mBLX	WDGIT_IRQHandler
        SYS_to_IRQ

	.end
