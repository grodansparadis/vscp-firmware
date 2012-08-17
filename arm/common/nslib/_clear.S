/* 
 * _clear.S --
 *
 *	Clear areas of quad word aligned memory.
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
 * void _clear(void *start, int len) --
 *
 *	Clear an area of memory.  The arguments are compatible with
 *	bzero if quad word aligned.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Fill the area of memory at start of length len with zeros.
 *	start and len should be quad word aligned.
 *
 *----------------------------------------------------------------------
 */

	.global __clear
__clear:
	stmfd sp!, {v1, v2, v3, v4}
	mov v1, #0
	mov v2, #0
	mov v3, #0
	mov v4, #0
	add a3, a1, a2
loop:
	stmia a1!, {v1, v2, v3, v4}
	cmp a1, a3
	bcc loop
	ldmfd sp!, {v1, v2, v3, v4}
	mov pc, lr

