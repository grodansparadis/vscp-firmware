/* 
 * halt.S --
 *
 *	Halt execution, used when certain error conditions arise.
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
 * void halt(void) --
 *
 *	Disable interrupts and sit in an infinite loop.
 *	This could be a lot more helpful depending on what sort
 *	of debug feedback channels are available.
 *
 * Results:
 *	Never returns.
 *
 * Side effects:
 *	Nothing happens.
 *
 *----------------------------------------------------------------------
 */

	.global _halt
_halt:

	bl _IntDisable
halt1:
	b halt1

