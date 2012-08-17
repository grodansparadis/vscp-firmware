/* 
 * _setcpsr.S --
 *
 *	Set the value of the Current Program Status Register
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
 * void _setcpsr(unsigned int cpsr) --
 *
 *	Set the value of cpsr
 *
 * Results:
 *	None
 *
 * Side effects:
 *	The CPSR is set to cpsr.
 *
 *----------------------------------------------------------------------
 */

	.global __setcpsr
__setcpsr:

	msr	cpsr, r0
	mov	pc, lr

