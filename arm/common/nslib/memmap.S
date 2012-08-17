/* 
 * memmap.S --
 *
 *	Declarations of symbols defining blocks in the memory map.
 *	This includes the L1 translation table as well as the
 *	locations of various blocks of physical and logical address
 *	spaces.  This information is used in writing the MMU tables.
 *	The stack and heap ranges are also defined here.  The stack
 *	defined is the supervisor mode stack, as all programs run
 *	in supervisor mode.
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
	 * The top of the stack in physical memory,
	 * used temporarily at startup before the MMU is enabled,
	 * and the physical adress of the L1 translation table:
	 */
	 
	.global __topOfStackPhysical
	.global __l1TableStartPhysical
	
	.equ __topOfStackPhysical,	0x10400000 - 0x4000
	.equ __l1TableStartPhysical,	0x10400000 - 0x4000

	/*
	 * Some basic logical memory locations:
	 */

	.global __bottomOfHeapLogical
	.global __topOfHeapLogical
	.global __bottomOfStackLogical
	.global	__topOfStackLogical
	.global __l1TableStartLogical
	.global	__topOfMemoryLogical

	.equ	__bottomOfHeapLogical,	__end
	.equ	__topOfHeapLogical,	0x00300000
	.equ	__bottomOfStackLogical,	0x00300000
	.equ	__topOfStackLogical,	0x00400000 - 0x4000
	.equ	__l1TableStartLogical,	0x00400000 - 0x4000
	.equ	__topOfMemoryLogical,	0x00400000


	/*
	 * The physical addresses of the sections to be mapped:
	 */

	.global	__romStartPhysical
	.global __pcioStartPhysical
	.global __armioStartPhysical
	.global __sioStartPhysical
	.global __vidStartPhysical
	.global __memStartPhysical
	.global __ram0StartPhysical
	.global __ram1StartPhysical
	.global __ram2StartPhysical
	.global __ram3StartPhysical
	
	.equ	__romStartPhysical,	0x00000000
	.equ	__pcioStartPhysical,	0x03010000
	.equ	__armioStartPhysical,	0x03200000
	.equ	__sioStartPhysical,	0x03210000
	.equ	__vidStartPhysical,	0x03400000
	.equ	__memStartPhysical,	0x10000000
	.equ	__ram0StartPhysical,	0x10000000
	.equ	__ram1StartPhysical,	0x10100000
	.equ	__ram2StartPhysical,	0x10200000
	.equ	__ram3StartPhysical,	0x10300000


	/*
	 * The logical addresses of the sections after mapping:
	 */
	
	.global __memStartLogical
	.global __ram0StartLogical
	.global __ram1StartLogical
	.global __ram2StartLogical
	.global __ram3StartLogical
	.global __pcioStartLogical
	.global __armioStartLogical
	.global __sioStartLogical
	.global __vidStartLogical
	.global	__romStartLogical
	
	.equ	__memStartLogical,	0x00000000
	.equ	__ram0StartLogical,	0x00000000
	.equ	__ram1StartLogical,	0x00100000
	.equ	__ram2StartLogical,	0x00200000
	.equ	__ram3StartLogical,	0x00300000
	.equ	__pcioStartLogical,	0x03010000
	.equ	__armioStartLogical,	0x03200000
	.equ	__sioStartLogical,	0x03210000
	.equ	__vidStartLogical,	0x03400000
	.equ	__romStartLogical,	0x20000000

	
