/* 
 * crt0.S --
 *
 *	A start file containing the exception vector table.  This
 *	must appear at address zero.
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

	.text
	.org 0

	/*
	 * The exception vector table:
	 */

	.global start
start:
	b TrapReset
	b _halt			/* TrapUndefined */
	b _halt			/* TrapSwi */
	b _halt			/* TrapAbortPrefetch */
	b _halt			/* TrapAbortData */
	b _halt			/* TrapReserved */
	b TrapIrq
	b _halt			/* TrapFiq */

