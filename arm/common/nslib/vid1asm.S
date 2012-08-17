/* 
 * vid1asm.S --
 *
 *	Assembly support for video mode 1 - CRT 640x480x8bit
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

#define DISPLAY_SIZE	(640 * 480 * 1)
#define MEM_START_PHYS	0x10000000

	.global __vid1Display0Logical
	.global __vid1Display1Logical
	.global __vid1Display0Physical
	.global __vid1Display1Physical

	.bss
	.align 4
	.lcomm __vid1Display0Logical DISPLAY_SIZE
	.lcomm __vid1Display1Logical DISPLAY_SIZE

	.equ __vid1Display0Physical, __vid1Display0Logical + MEM_START_PHYS
	.equ __vid1Display1Physical, __vid1Display1Logical + MEM_START_PHYS


