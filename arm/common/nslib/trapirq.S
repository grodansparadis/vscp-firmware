/* 
 * trapirq.S --
 *
 *	A trap for IRQ exceptions.  Interrupts are dispatched by a
 *	high level C function.
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
 *----------------------------------------------------------------------
 *
 * TrapIrq --
 *
 *	Handle an IRQ exception.  This is a wrapper around IrqDispatch()
 *	to save and restore registers and status.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Whatever IrqDispatch does.  Note that nested interrupts are not
 *	allowed, so the SPSR is not saved on the stack.  (It is
 *	retained in spsr_irq.)
 *
 *----------------------------------------------------------------------
 */
	.global TrapIrq
TrapIrq:
	stmfd	sp!, {r0-r12, lr}
	mrs	r0, spsr
	stmfd	sp!, {r0}

	bl	_IrqDispatch

	ldmfd	sp!, {r0}
	msr	spsr, r0
	ldmfd	sp!, {r0-r12, lr}
	subs	pc, lr, #4

