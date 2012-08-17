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

/***********************************************************************************
*			This Module gives also a simple example of MMU & CACHE  
*			setup according to the STR720-EVAL (MB397) board memory mapping
*			and can be customized to user application needs.
***********************************************************************************/

/**** Startup Code (executed after Reset) ****/

/* Frequency values */
/* set to suit target hardware */

	.equ	FOSC,			16000000
	
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

	.equ	EIC_Base_addr,		0xFFFFFC00		/* EIC base address */
	.equ	ICR_off_addr,		0x00			/* Interrupt Control register offset */
	.equ	CIPR_off_addr,		0x08			/* Current Interrupt Priority Register offset */
	.equ	IVR_off_addr,		0x18			/* Interrupt Vector Register offset */
	.equ	FIR_off_addr,		0x1C			/* Fast Interrupt Register offset */
	.equ	IER_off_addr,		0x20			/* Interrupt Enable Register offset */
	.equ	IPR_off_addr,		0x40			/* Interrupt Pending Bit Register offset */
	.equ	SIR0_off_addr,		0x60			/* Source Interrupt Register 0 */

	.equ	DRAMC_off_addr,		0xF0000000		/* DRAMC base address */
	.equ	SGCR1_off_addr,		0xF0000C00		/* S-APB Global Configuration Register 1 */
	.equ	CGCPCG_off_addr,	0xF0002C00		/* CGC Peripheral Clock Gating Register 1 */

# DRAMC Registers Offsets

	.equ	MB1Config,          0x00
	.equ	SDRAM1ConfigLo,     0x10
	.equ	SDRAM1ConfigHi,     0x14
	.equ	MEMConfig,          0x30
	.equ	Bank1Size,          0x34

# SDRAM Commands

	.equ	DRAMC_NOP,			0x0				/* No operation */
	.equ	DRAMC_ACT,			0x1				/* Bank active */
	.equ	DRAMC_READ,			0x2				/* Read */
	.equ	DRAMC_CBR,			0x3				/* Auto refresh */
	.equ	DRAMC_BST,			0x4				/* Burst stop */
	.equ	DRAMC_PRECH,		0x5				/* Precharge selected */
	.equ	DRAMC_WRITE,		0x6				/* Write */
	.equ	DRAMC_MRS,			0x7				/* Mode register set */

# MMU & CACHE Defines

	.equ	Fault,				0x000
	.equ	Section,			0x002
	.equ	B,					0x004			/* Bufferable */
	.equ	C,					0x008			/* Cachable */
	.equ	TTBit,				0x010
	.equ	Domain,				0x1E0
	.equ	FullAccess,			0xC00

.ifndef REMAP
	.equ	REMAP,	0
.endif

/* Startup Code must be linked first at Address at which it expects to run. */

	.text
	.arm
	.section .init, "ax"
	
	.global _start
	.global RCCU_Main_Osc
	.global Cache_Enable
	.global Cache_Disable

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

# Perform EMI/RAM remapping, if required
# On reset, an aliased copy of EMI (@0x40000000) is at 0x0.
# Continue execution from 'real' EMI memory rather than aliased copy

.if REMAP >= 2
		LDR		r1, =SGCR1_off_addr
		LDR		r0, [r1]
		ORR		r0, r0, #0x1
		STR		r0, [r1]
		
		LDR     r1, =_vecstart					/* r1 = start address from which to copy */
		LDR     r3, =_vecend
		SUB		r3, r3, r1						/* r3 = number of bytes to copy */
		LDR     r0, =_vectext					/* r0 = start address where to copy */
copy_ram:
		LDR     r2, [r0], #4					/* Read a word from the source */
		STR     r2, [r1], #4					/* copy the word to destination */
		SUBS    r3, r3, #4						/* Decrement number of words to copy */
		BNE     copy_ram
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

/* Enter System Mode and set its Stack Pointer */
		MSR     cpsr_c, #Mode_SYS
		LDR     sp, =__stack_end__

/* Setup a default Stack Limit (when compiled with "-mapcs-stack-check") */
		LDR     sl, =__bss_end__

# Initialize critical IO devices 
# The following devices are initialized (DRAMC/MMU/EIC)

/* DRAMC initialization */
/***********************************************************************************
Description  : Initialize the DRAMC :
             - Enter POWERSAVE.
             - Configure SDRAM and exit power save mode.
             - Precharge all the banks.
             - Execute the auto refresh command at least eight times.
             - Execute the mode register set command to initialize
               the mode register.
***********************************************************************************/
        
# Configure SDRAM Controller
# Enter POWERSAVE 
# Type	: SDRAM
# Only Block 1 enabled
# Refresh period: 16 
		LDR		r0, =DRAMC_off_addr
		LDR		r1, =0x3130
		STR		r1, [r0, #MEMConfig]
# BANK 1 configuration 
# ---------------------------------------------
# Data width   : WORD
# Data latency : 3 cycles
# Setup time   : 0
# Idle time    : 0
# Col. width   : 8 bit
# Size         : 16 Mb
		LDR		r1, =0x0B00						/* Bank1 configuration */
		STR		r1, [r0, #MB1Config]
		LDR		r1, =0x00FF						/* Bank1 size */
		STR		r1, [r0, #Bank1Size]

# Configure SDRAM device
		LDR		r1, =0x1130
		STR		r1, [r0, #MEMConfig]			/* Exit POWERSAVE */

# Precharge all banks PALL (enter idle state)
		LDR		r1, =0x0400						/* Bit 10 set stands for all banks */
		STR		r1, [r0, #SDRAM1ConfigLo]
		MOV		r1, #DRAMC_PRECH
		STR		r1, [r0, #SDRAM1ConfigHi]

# Issue 8 auto-refresh commands (CBR) to complete power-up sequence
		MOV		r1, #DRAMC_CBR
		STR		r1, [r0, #SDRAM1ConfigHi]		/* Low part of configuration data is not required */
		STR		r1, [r0, #SDRAM1ConfigHi]		/* Distance between CBRs must be greater than 70 ns */
		STR		r1, [r0, #SDRAM1ConfigHi]
		STR		r1, [r0, #SDRAM1ConfigHi]
		STR		r1, [r0, #SDRAM1ConfigHi]
		STR		r1, [r0, #SDRAM1ConfigHi]
		STR		r1, [r0, #SDRAM1ConfigHi]
		STR		r1, [r0, #SDRAM1ConfigHi]

# SDRAM mode register:  Burst length : 4, sequential
# CAS latency  : 3 cycles
# Op. mode     : standard
		LDR		r1, =0x0032
		STR		r1, [r0, #SDRAM1ConfigLo]
		MOV		r1, #DRAMC_MRS
		STR		r1, [r0, #SDRAM1ConfigHi]

/* Initialize memory system */

/***********************************************************************************
 Configure Translation Table (Flat mapping : Virtual Address = Physical Address)
 Base address   = 0xA0FFC000 (The Last 16KB of the SDRAM Memory)
 Size           = 16 Kb
 Descriptors    = SDRAM sections are cachable/bufferable    (0xA0000000-0xA0FFFFFF)
                  First EMI  section is cachable	     (0x40000000-0x400FFFFF)
				  First SRAM section is cachable/bufferable (0x60000000-0x60003FFF)
                  All the rest non cachable/non bufferable
***********************************************************************************/

/* IF MMU & CACHE are enabled -> Disable them */
		MRC		p15, 0, r0, c1, c0, 0		/* Read CP15 register 1 */
		BIC		r0, r0, #0x1				/* Clear Bit 0 (MMU) */
		MCR		p15, 0, r0, c1, c0, 0		/* Write back value */
		
		MOV		r0, #0
		MCR		p15, 0, r0, c7, c7, 0		/* invalidate caches */
		MCR		p15, 0, r0, c8, c7, 0		/* invalidate TLBs */
		
		LDR		r1, =SGCR1_off_addr			/* Clear CACHE Flag on SGCR1 register */
		LDR		r0, [r1]
		AND		r0, r0, #0xFFFFFFDF
		STR		r0, [r1]

# MMU & CACHE Init
	     
		LDR		r0, =0xA0FFC000				/* Set start of Translation Table base (16k Boundary) */
		MCR		p15, 0, r0, c2, c0, 0		/* write to CP15 register 2 */

# Create translation Table for flat mapping ( Virtual Address = Physical Address)
# Top 12 bits of VA is pointer into table
# Create 4096 entries from 000xxxxx to fffxxxxx

		LDR		r1,=0xFFF					/* loop counter */
		MOV		r2, #TTBit|Section			/* build descriptor pattern in register */
		ORR		r2, r2, #Domain|FullAccess

init_ttb:
		ORR		r3, r2, r1, lsl#20			/* use loop counter to create individual table entries */
		STR		r3,[r0, r1, lsl#2]			/* str r3 at TTB base + loopcount*4 */
		SUBS	r1, r1, #1					/* decrement loop counter */
		BPL		init_ttb
        
# Set cachable attributes for Physical Flash/EMI (3:2)
	
		LDR		r1, =0xA0FFD000
		LDR		r3, [r1]
		ORR		r3, r3, #C
		STR		r3, [r1]
        
# Set cachable and bufferable attributes for Physical SRAM (3:2)
	
		LDR		r1, =0xA0FFD800
		LDR		r3, [r1]
		ORR		r3, r3, #C|B
		STR		r3, [r1]
        
# Set cachable and bufferable attributes for Physical SDRAM (3:2)
		MOV		r4, #0
sdramloop:
		LDR		r1, =0xA0FFE800
		ADD		r1, r1, r4
		LDR		r3, [r1]
		ORR		r3, r3, #C|B
		STR		r3, [r1]
		ADD		r4, r4, #4
		CMP		r4, #0x40
		BLT		sdramloop

# init_domains
		MOV		r0, #(1 << 30)				/* must define behaviour for domain 15 (31:30), set client */
		MCR		p15, 0, r0, c3, c0, 0		/* write to CP15 register 5 */
# set global core configurations
		MRC		p15, 0, r0, c1, c0, 0		/* read CP15 register 1 into r0 */
		BIC		r0, r0, #(0x1 << 2)			/* ensure Cache disabled */
		ORR		r0, r0, #0x1				/* enable MMU */
		MCR		p15, 0, r0, c1, c0, 0		/* write cp15 register 1 */

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
		LDR     r4, =0x38
		STR     r4, [r3, #FIR_off_addr]		/* Disable FIQ channels and clear FIQ pending bits */
		LDR     r4, =0x00000000
		STR     r4, [r3, #CIPR_off_addr]	/* Reset the current priority register */
		LDR     r4, =0xE59F0000
		STR     r4, [r3, #IVR_off_addr]		/* Write the LDR pc,pc,#offset instruction code in IVR[31:16] */
		LDR     r2, =32						/* 32 Channel to initialize */
		LDR     r0, =EXT03IT_Addr			/* Read the address of the IRQs address table */
		LDR     r1, =0x00000FFF
		AND     r0, r0, r1
		LDR     r5, =SIR0_off_addr			/* Read SIR0 address */
		SUB     r4, r0, #8					/* subtract 8 for prefetch */
		LDR     r1, =0xF3E8					/* add the offset to the 0x00000000 address(IVR address + 3E8 = 0x00000000) */
											/* 0xF7E8 used to complete the LDR pc,pc,#offset opcode */
		ADD     r1, r4, r1					/* compute the jump offset */
EIC_INI:
		MOV     r4, r1, LSL #16				/* Left shift the result */
		STR     r4, [r3, r5]				/* Store the result in SIRx register */
		ADD     r1, r1, #4					/* Next IRQ address */
		ADD     r5, r5, #4					/* Next SIR */
		SUBS    r2, r2, #1					/* Decrement the number of SIR registers to initialize */
		BNE     EIC_INI						/* If more then continue */

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
		LDR 	r2, [r0], #+4
		STMFD 	sp!, {r0-r1}
		MOV 	lr, pc
		BX		r2
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
#		LDR		r2, [r0], #+4
#		STMFD	sp!, {r0-r1}
#		MOV		lr, pc
#		BX		r2
#		LDMFD	sp!, {r0-r1}
#		B		dtor_loop
dtor_end:

/* Return from main, loop forever. */
exit_loop:
#		B		exit_loop

/****************************************************************************
 Enable & Disable Cache Function definition, these two function may be
 called by application (main() or any code) whenever user wants to enable
 or Disable CACHE during program execution.
****************************************************************************/

	.func Cache_Enable
	
Cache_Enable:
	MOV		r0, #0					/* dummy */
	MCR		p15, 0, r0, c7, c7, 0   /* invalidate caches */
	MCR		p15, 0, r0, c8, c7, 0   /* invalidate TLBs */
	MRC		p15, 0, r0, c1, c0, 0	/* read CP15 register 1 into r0 */
	ORR		r0, r0, #(0x1 << 2)		/* enable Cache */
	MCR		p15, 0, r0, c1, c0, 0	/* write CP15 register 1 */
	MOV		pc, lr
	
	.endfunc
	
	.func Cache_Disable
	
Cache_Disable:
	LDR		r1, =SGCR1_off_addr		/* Clear CACHE Flag on SGCR1 register */
	LDR		r0, [r1]
	AND		r0, r0, #0xFFFFFFDF
	STR		r0, [r1]
	NOP								/* dummy */
	NOP								/* dummy */
	NOP								/* dummy */
	MOV		r0, #0
	MCR		p15, 0, r0, c7, c7, 0	/* invalidate caches */
	MCR		p15, 0, r0, c8, c7, 0   /* invalidate TLBs */
	MRC		p15, 0, r0, c1, c0, 0   /* read CP15 register 1 into r0 */
	BIC		r0, r0, #(0x1 << 2)     /* ensure Cache disabled */
	MCR		p15, 0, r0, c1, c0, 0   /* write cp15 register 1 */
	NOP								/* dummy */
	NOP								/* dummy */
	NOP								/* dummy */
	MOV		pc, lr

	.endfunc
	
/* Fosc values, used by libstr7 */

RCCU_Main_Osc:	.long	FOSC

	.weak libdebug
	
	.end
