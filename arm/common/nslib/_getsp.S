/* 
 * _getsp.S --
 *
 *	Check the value of the Stack Pointer
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
 * unsigned int _getsp(void) --
 *
 *	Get the value of the stack pointer
 *
 * Results:
 *	Returns sp
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

	.global __getsp
__getsp:

	mov	r0, sp
	mov	pc, lr

