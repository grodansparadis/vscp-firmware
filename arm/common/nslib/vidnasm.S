/* 
 * vidgenasm.S --
 *
 *	Assembly support for all video modes.
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

#define CURSOR_SIZE	(8 * 32)
#define MEM_START_PHYS	0x10000000

	.global __vidCursorLogical
	.global __vidCursorPhysical

	.bss
	.align 4
	.lcomm __vidCursorLogical CURSOR_SIZE

	.text

	.equ __vidCursorPhysical, __vidCursorLogical + MEM_START_PHYS


