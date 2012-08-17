/* 
 * trapreset.S --
 *
 *	A reset handler used when the program is the first piece of
 *	code linked in the ROM.
 *
 * Copyright (c) 1996 Ben Williamson.
 * All rights reserved.
 *
 * This file is part of nslib, a library used by programs
 * running on the Netslate Development Board.
 *
 * This software is released under the GNU Public License.
 * See the file COPYING for more information.
 */

/*
 * Bits in the PSR registers:
 */

#define SVC_32_MODE	0x13
#define IRQ_32_MODE	0x12
#define MODE_MASK	0x1F
#define INT_MASK	0xC0

/*
 * Bits in the control register:
 */

#define CTRL		0x00000040
#define CTRL_MMU	0x00000001
#define CTRL_IDC	0x00000004
#define CTRL_WB		0x00000008
#define CTRL_32P	0x00000010
#define CTRL_32D	0x00000020
#define CTRL_SYS	0x00000100
#define CTRL_ROM	0x00000200

/*
 * The two values for the control register that we use (before
 * and after setting up for the MMU)
 */

cpuCtrl0:	.word	(CTRL|CTRL_32P|CTRL_32D)
cpuCtrl1:	.word	(CTRL|CTRL_32P|CTRL_32D|CTRL_MMU|CTRL_IDC|CTRL_WB)

/*
 * A value with which to program the domain control register
 */

domainManager:	.word	0xFFFFFFFF


/*
 * Memory reserved in bss for an IRQ stack:
 */
 
#define IRQ_STACK_SIZE	0x1000

	.bss
	.align 2
	.lcomm irqStack	IRQ_STACK_SIZE
	
	.text

irqStackBottom:	.word	irqStack
irqStackTop:	.word	irqStack + IRQ_STACK_SIZE
svcStackTop:	.word	__topOfStackLogical
tempStackTop:	.word	__topOfStackPhysical
dramStart:	.word	__ram0StartPhysical
romStart:	.word	__romStartPhysical
romEnd:		.word	__edata
bssEnd:		.word	__end
l1TableAddr:	.word	__l1TableStartPhysical

/*
 *----------------------------------------------------------------------
 *
 * TrapReset --
 *
 *	Handle a reset exception caused by a hard reset of the system.
 *	This is the very first piece of code to run so it needs to
 *	set up everything in the right order for the program that
 *	follows.  After set-up, main() is called.  Until the MMU
 *	is enabled, this code is running from ROM.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Configuration and mode registers are initialised, the 
 *	MMU is set up and enabled, stacks are created, main() is
 *	called.
 *
 *----------------------------------------------------------------------
 */

	.text
	.global TrapReset
TrapReset:

	mov	r14, #0x70	/* Set PROG32 and DATA32 (and late abort) */
	mcr	p15, 0, r14, c1, c0
	mov	r14, #0xD3	/* I,F bits set; not Thumb; SVC32 mode */
	msr	cpsr, r14
	msr	spsr, r14

#if 0
	/*
	 * Set the control register: 32 bit program and data, no MMU
	 */
	ldr	r0, cpuCtrl0
	mcr	15, 0, r0, c1, c0
#endif
	/*
	 * Set CLKCTL for divide-by-one on all clocks
	 * (the Netslate board has a 32MHz crystal module)
	 */
	mov	r0, #0x03200000
	mov	r1, #0x07
	str	r1, [r0, #0x3C]

	/*
	 * Make sure we are in supervisor mode with interrupts off
	 * and set up a temporary stack in DRAM
	 */
	
	mrs	r0, cpsr
	bic	r0, r0, #MODE_MASK
	orr	r0, r0, #(SVC_32_MODE | INT_MASK)
	msr	cpsr, r0
	msr	spsr, r0
	ldr	sp, tempStackTop


	/*
	 * Call _ResetMisc() to do some miscellaneous initialisation.
	 * It's safe to call this C function as we now have a stack.
	 */
	bl	__ResetMisc
	
	/*
	 * Copy the text and data sections in ROM into DRAM
	 */
	ldr	r0, dramStart
	ldr	r1, romStart
	ldr	r2, romEnd
	sub	r2, r2, r1
	bl	_memcpy
	
	/*
	 * Zero out the bss section in DRAM
	 */
	ldr	r0, romEnd
	ldr	r1, romStart
	sub	r0, r0, r1
	ldr	r1, dramStart
	add	r0, r0, r1	/* r0 = start of bss in dram */
	ldr	r1, bssEnd
	ldr 	r2, romEnd
	sub	r1, r1, r2	/* r1 = length of bss */
	bl	_bzero
	
	/*
	 * Call _MmuInitTable() to write the L1 translation table
	 */
	bl	__MmuInitTable
	
	/*
	 * Switch to IRQ mode and set up a stack in the
	 * logical address space
	 */
	mrs	r0, cpsr
	bic	r0, r0, #MODE_MASK
	orr	r0, r0, #(IRQ_32_MODE | INT_MASK)
	msr	cpsr, r0
	ldr	sp, irqStackTop
	mov	sl, #0

	/*
	 * Switch back to supervisor mode and set up the
	 * stack in the logical address space
	 */
	mrs	r0, cpsr
	bic	r0, r0, #MODE_MASK
	orr	r0, r0, #(SVC_32_MODE | INT_MASK)
	msr	cpsr, r0
	ldr	sp, svcStackTop
	mov	sl, #0
	
	/*
	 * Program the Translation Table Base register: (register c2)
	 */

	ldr r0, l1TableAddr
	mcr 15, 0, r0, c2, c0

	/*
	 * Program the Domain Access Control register (register c3)
	 * for manager access to all domains.
	 */

	ldr r0, domainManager
	mcr 15, 0, r0, c3, c0

	/*
	 * Flush the TLB and IDC
	 */

	mcr 15, 0, r0, c5, c0	/* TLB */
	mcr 15, 0, r0, c7, c0	/* IDC */

	/*
	 * Enable the MMU, IDC and WB in the Control register: (register c1)
	 * Note: This has the effect of a delayed branch, so we branch
	 * immediately to make sure we know where we end up.  This happens
	 * to be a call to main()..
	 */

	ldr r0, cpuCtrl1
	mcr 15, 0, r0, c1, c0 	

	/*
	 * Call main(), and then if it exits loop forever.
	 */

	bl _main
forever:
	b forever

