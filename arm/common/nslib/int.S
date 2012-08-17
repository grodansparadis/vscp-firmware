/* 
 * int.S --
 *
 *	Assembly routines for enabling and disabling interrupts
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

#define INT_MASK	0xC0
#define IRQ_MASK	0x80
#define FIQ_MASK	0x40


/*
 *----------------------------------------------------------------------
 *
 * void IntEnable(void) --
 *
 *	Enable IRQ and FIQ interrupts.  Must be called from a
 *	priveliged mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The IRQ and FIQ mask bits in the CPSR are cleared.
 *
 *----------------------------------------------------------------------
 */

	.global _IntEnable
_IntEnable:

	mrs	r0, cpsr
#if 0
	bic	r0, r0, #INT_MASK
#else
	bic	r0, r0, #IRQ_MASK
#endif
	msr	cpsr, r0
	mov	pc, lr
	


/*
 *----------------------------------------------------------------------
 *
 * void IntDisable(void) --
 *
 *	Disable IRQ and FIQ interrupts.  Must be called from a
 *	priveliged mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The IRQ and FIQ mask bits in the CPSR are set.
 *
 *----------------------------------------------------------------------
 */

	.global _IntDisable
_IntDisable:

	mrs	r0, cpsr
	orr	r0, r0, #INT_MASK
	msr	cpsr, r0
	mov	pc, lr
	

