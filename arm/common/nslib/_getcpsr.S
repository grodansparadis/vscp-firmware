/* 
 * _getcpsr.S --
 *
 *	Check the value of the Current Program Status Register
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
 * unsigned int _getcpsr(void) --
 *
 *	Get the value of cpsr
 *
 * Results:
 *	Returns cpsr
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

	.global __getcpsr
__getcpsr:

	mrs	r0, cpsr
	mov	pc, lr

