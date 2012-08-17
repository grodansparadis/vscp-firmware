/* 
 * vid2asm.S --
 *
 *	Assembly support for video mode 2 - CRT 320x240x24bit
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

#define DISPLAY_SIZE	(320 * 240 * 4)
#define MEM_START_PHYS	0x10000000

	.global __vid2Display0Logical
	.global __vid2Display1Logical
	.global __vid2Display0Physical
	.global __vid2Display1Physical

	.bss
	.align 4
	.lcomm __vid2Display0Logical DISPLAY_SIZE
	.lcomm __vid2Display1Logical DISPLAY_SIZE

	.equ __vid2Display0Physical, __vid2Display0Logical + MEM_START_PHYS
	.equ __vid2Display1Physical, __vid2Display1Logical + MEM_START_PHYS


