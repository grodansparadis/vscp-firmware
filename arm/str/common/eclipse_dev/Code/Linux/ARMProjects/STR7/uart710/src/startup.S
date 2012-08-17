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

/**** Startup Code (executed after Reset) ****/

/* Frequency values */
/* set to suit target hardware */

	.equ	FOSC,			16000000
	.equ	FRTC,			32768

/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs */

	.equ    Mode_USR,       0x10
	.equ    Mode_FIQ,       0x11
	.equ    Mode_IRQ,       0x12
	.equ    Mode_SVC,       0x13
	.equ    Mode_ABT,       0x17
	.equ    Mode_UND,       0x1B
	.equ    Mode_SYS,       0x1F				/* available on ARM Arch 4 and later */

	.equ    I_Bit,          0x80				/* when I bit is set, IRQ is disabled */
	.equ    F_Bit,          0x40				/* when F bit is set, FIQ is disabled */

/* --- System memory locations */

	.equ	EIC_Base_addr,		0xFFFFF800		/* EIC base address */
	.equ	ICR_off_addr,		0x00			/* Interrupt Control register offset */
	.equ	CIPR_off_addr,		0x08			/* Current Interrupt Priority Register offset */
	.equ	IVR_off_addr,		0x18			/* Interrupt Vector Register offset */
	.equ	FIR_off_addr,		0x1C			/* Fast Interrupt Register offset */
	.equ	IER_off_addr,		0x20			/* Interrupt Enable Register offset */
	.equ	IPR_off_addr,		0x40			/* Interrupt Pending Bit Register offset */
	.equ	SIR0_off_addr,		0x60			/* Source Interrupt Register 0 */

	.equ	EMI_Base_addr,		0x6C000000		/* EMI base address */
	.equ	BCON0_off_addr,		0x00			/* Bank 0 configuration register offset */
	.equ	BCON1_off_addr,		0x04			/* Bank 1 configuration register offset */
	.equ	BCON2_off_addr,		0x08			/* Bank 2 configuration register offset */
	.equ	BCON3_off_addr,		0x0C			/* Bank 3 configuration register offset */

	.equ	GPIO2_Base_addr,	0xE0005000		/* GPIO2 base address */
	.equ	PC0_off_addr,		0x00			/* Port Configuration Register 0 offset */
	.equ	PC1_off_addr,		0x04			/* Port Configuration Register 1 offset */
	.equ	PC2_off_addr,		0x08			/* Port Configuration Register 2 offset */
	.equ	PD_off_addr,		0x0C			/* Port Data Register offset */

	.equ	CPM_Base_addr,		0xA0000040		/* CPM Base Address */
	.equ	BOOTCONF_off_addr,	0x10			/* CPM - Boot Configuration Register */
	.equ	FLASH_mask,			0x0000			/* to remap FLASH at 0x0 */
	.equ	RAM_mask,			0x0002			/* to remap RAM at 0x0 */
	.equ	EXTMEM_mask,		0x0003			/* to remap EXTMEM at 0x0 */

/*	define remapping, if using ram based vectors or rom based
   	REMAP will need to be define in vector.s aswell
	Valid Options:
	REMAP 0 - default config, no remapping, vectors in rom
	REMAP 1 - vectors in rom
	REMAP 2 - vectors in ram
	REMAP 3 - vectors in external ram
*/

.ifndef REMAP
	.equ	REMAP,	0
.endif

/* 	if we are debugging in ram then we need
	the vectors to point to the correct location
	only if BOOT0/BOOT1 have been set for Flash @ zero.
*/

.ifndef DBGRAM
	.equ	DBGRAM, 0
.endif

/* Startup Code must be linked first at Address at which it expects to run. */

	.text
	.arm
	.section .init, "ax"
	
	.global _start
	.global RCCU_Main_Osc
	.global RCCU_RTC_Osc

.if REMAP

/* Exception Vector (before Remap) */

/* Reset Handler */
/* On reset, an aliased copy of ROM is at 0x0. */
/* Continue execution from 'real' ROM rather than aliased copy */

		LDR     pc, =HardReset
HardReset:

/******************************************************************************
REMAPPING
Description  : Remapping  memory whether RAM,FLASH or External memory
               at Address 0x0 after the application has started executing.
               Remapping is generally done to allow RAM to replace FLASH
               or EXTMEM at 0x0.
               the remapping of RAM allow copying of vector table into RAM
******************************************************************************/

.if REMAP == 1
		MOV     r4, #FLASH_mask
.endif

.if REMAP == 2
		MOV     r4, #RAM_mask
.endif

.if REMAP == 3
		/* copying the program into SRAM */
		LDR     r0, =GPIO2_Base_addr			/* Configure P2.0 -> 3 in AF_PP mode */
		LDR     r1, =0x0000000F
		STR     r1, [r0, #PC0_off_addr]
		STR     r1, [r0, #PC1_off_addr]
		STR     r1, [r0, #PC2_off_addr]

		LDR     r0, =EMI_Base_addr				/* Enable the EMI */
		LDR     r1, =0x00008001
		STR     r1, [r0, #BCON0_off_addr]		/*  Enable bank 0 16-bit 0 wait state */

		MOV     r0, #EXTMEM_mask
		LDR     r1, =CPM_Base_addr
		LDRH    r2, [r1, #BOOTCONF_off_addr]	/* Read BOOTCONF Register */
		LDR     r4, =0x3
		AND     r2, r2, r4
		CMP     r2, r0
		BEQ     _start
		MOV     r4, #EXTMEM_mask
.endif

remap:
		LDR     r1, =_vecstart					/* r1 = start address from which to copy */
		LDR     r3, =_vecend
		SUB		r3, r3, r1						/* r3 = number of bytes to copy */
		LDR     r0, =_vectext					/* r0 = start address where to copy */
copy_ram:
		LDR     r2, [r0], #4					/* Read a word from the source */
		STR     r2, [r1], #4					/* copy the word to destination */
		SUBS    r3, r3, #4						/* Decrement number of words to copy */
		BNE     copy_ram
		
		LDR     r1, =CPM_Base_addr
		LDRH    r2, [r1, #BOOTCONF_off_addr]	/* Read BOOTCONF Register */
		BIC     r2, r2, #0x03					/* Reset the two LSB bits of BOOTCONF Register */
		ORR     r2, r2, r4						/* change the two LSB bits of BOOTCONF Register */
		STRH    r2, [r1, #BOOTCONF_off_addr]	/* Write BOOTCONF Register */
.endif

/* After remap this will be our reset handler */

_start:
		LDR     pc, =NextInst
NextInst:

		NOP		/* Wait for OSC stabilization */
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP

.if DBGRAM
		MOV     r0, #RAM_mask
		LDR     r1, =CPM_Base_addr
		LDRH    r2, [r1, #BOOTCONF_off_addr]	/* Read BOOTCONF Register */
		BIC     r2, r2, #0x03					/* Reset the two LSB bits of BOOTCONF Register */
		ORR     r2, r2, r0						/* change the two LSB bits of BOOTCONF Register */
		STRH    r2, [r1, #BOOTCONF_off_addr]	/* Write BOOTCONF Register */
.endif

/* Setup Stack for each mode */

/* Enter Abort Mode and set its Stack Pointer */
		MSR     cpsr_c, #Mode_ABT|I_Bit|F_Bit
		LDR     sp, =__stack_abt_end__

/* Enter Undefined Instruction Mode and set its Stack Pointer */
		MSR     cpsr_c, #Mode_UND|I_Bit|F_Bit
		LDR     sp, =__stack_und_end__

/* Enter Supervisor Mode and set its Stack Pointer */
		MSR     cpsr_c, #Mode_SVC|I_Bit|F_Bit
		LDR     sp, =__stack_svc_end__

/* Enter FIQ Mode and set its Stack Pointer */
		MSR     cpsr_c, #Mode_FIQ|I_Bit|F_Bit
		LDR     sp, =__stack_fiq_end__

/* Enter IRQ Mode and set its Stack Pointer */
		MSR     cpsr_c, #Mode_IRQ|I_Bit|F_Bit
		LDR     sp, =__stack_irq_end__

/* Enter User Mode and set its Stack Pointer */
		MSR     cpsr_c, #Mode_USR
		LDR     sp, =__stack_end__

/* Setup a default Stack Limit (when compiled with "-mapcs-stack-check") */
		LDR     sl, =__bss_end__

/******************************************************************************
EIC initialization
Description  : Initialize the EIC as following :
              - IRQ disabled
              - FIQ disabled
              - IVR contain the load PC opcode (0xF59FF00)
              - Current priority level equal to 0
              - All channels are disabled
              - All channels priority equal to 0
              - All SIR registers contain offset to the related IRQ table entry
******************************************************************************/

EIC_INIT:
		LDR     r3, =EIC_Base_addr
		LDR     r4, =0x00000000
		STR     r4, [r3, #ICR_off_addr]		/* Disable FIQ and IRQ */
		STR     r4, [r3, #IER_off_addr]		/* Disable all channels interrupts */
		LDR     r4, =0xFFFFFFFF
		STR     r4, [r3, #IPR_off_addr]		/* Clear all IRQ pending bits */
		LDR     r4, =0x0C
		STR     r4, [r3, #FIR_off_addr]		/* Disable FIQ channels and clear FIQ pending bits */
		LDR     r4, =0x00000000
		STR     r4, [r3, #CIPR_off_addr]	/* Reset the current priority register */
		LDR     r4, =0xE59F0000
		STR     r4, [r3, #IVR_off_addr]		/* Write the LDR pc,pc,#offset instruction code in IVR[31:16] */
		LDR     r2, =32						/* 32 Channel to initialize */
		LDR     r0, =T0TIMI_Addr			/* Read the address of the IRQs address table */
		LDR     r1, =0x00000FFF
		AND     r0, r0, r1
		LDR     r5, =SIR0_off_addr			/* Read SIR0 address */
		SUB     r4, r0, #8					/* subtract 8 for prefetch */
		LDR     r1, =0xF7E8					/* add the offset to the 0x00000000 address(IVR address + 7E8 = 0x00000000) */
											/* 0xF7E8 used to complete the LDR pc,pc,#offset opcode */
		ADD     r1, r4, r1					/* compute the jump offset */
EIC_INI:
		MOV     r4, r1, LSL #16				/* Left shift the result */
		STR     r4, [r3, r5]				/* Store the result in SIRx register */
		ADD     r1, r1, #4					/* Next IRQ address */
		ADD     r5, r5, #4					/* Next SIR */
		SUBS    r2, r2, #1					/* Decrement the number of SIR registers to initialize */
		BNE     EIC_INI						/* If more then continue */

/* Change to System mode */
		MSR     cpsr_c, #Mode_SYS

/* Relocate .data section (Copy from ROM to RAM) */
		LDR     r1, =_etext
		LDR     r2, =__data_start
		LDR     r3, =_edata
LoopRel:
		CMP     r2, r3
		LDRLO   r0, [r1], #4
		STRLO   r0, [r2], #4
		BLO     LoopRel

/* Clear .bss section (Zero init) */
		MOV     r0, #0
		LDR     r1, =__bss_start__
		LDR     r2, =__bss_end__
LoopZI:
		CMP     r1, r2
		STRLO   r0, [r1], #4
		BLO     LoopZI
  		
/* Call C++ constructors */
		LDR 	r0, =__ctors_start__
		LDR 	r1, =__ctors_end__
ctor_loop:
		CMP 	r0, r1
		BEQ 	ctor_end
		LDR 	r2, [r0], #4
		STMFD 	sp!, {r0-r1}
		MOV 	lr, pc
		MOV 	pc, r2
		LDMFD 	sp!, {r0-r1}
		B 		ctor_loop
ctor_end:

/* Need to set up standard file handles */
/* Only used under simulator, normally overide syscall.c */
#		BL		initialise_monitor_handles

/* if we use debug version of str7lib this will call the init function */
		
		BL		libdebug
libdebug:		

/* Enter the C code, use B instruction so as to never return */
/* use BL main if you want to use c++ destructors below */
		B		main

/* Call destructors */
#		LDR		r0, =__dtors_start__
#		LDR		r1, =__dtors_end__
dtor_loop:
#		CMP		r0, r1
#		BEQ		dtor_end
#		LDR		r2, [r0], #4
#		STMFD	sp!, {r0-r1}
#		MOV		lr, pc
#		MOV		pc, r2
#		LDMFD	sp!, {r0-r1}
#		B		dtor_loop
dtor_end:

/* Return from main, loop forever. */
exit_loop:
#		B		exit_loop
	
/* Fosc values, used by libstr7 */

RCCU_Main_Osc:	.long	FOSC
RCCU_RTC_Osc:	.long	FRTC

	.weak libdebug
	
	.end
