/* 
 * random.S --
 *
 *	A pseudo random sequence generator.  This code is taken from
 *	the ARM7500 data sheet, page 5-53.
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
 
	.data
seeda:	.word	1
seedb:	.word	0
	.text
sa:	.word	seeda
sb:	.word	seedb

/*
 *----------------------------------------------------------------------
 *
 * unsigned int random(void) --
 *
 *	Generate the next pseudo random number in the sequence.
 *
 * Results:
 *	Returns a 32 bit pseudo random value.
 *
 * Side effects:
 *	The random seed is updated with the return value.
 *
 *----------------------------------------------------------------------
 */

	.global _random
_random:
	
	ldr	r3, sa
	ldr	r0, [r3]
	ldr	r3, sb
	ldr	r1, [r3]
	tst	r1, r1, lsr#1
	movs	r2, r0, rrx
	adc	r1, r1, r1
	eor	r2, r2, r0, lsl#12
	eor	r0, r2, r2, lsr#20
	ldr	r3, sa
	str	r0, [r3]
	ldr	r3, sb
	str	r1, [r3]
	mov	pc, lr

