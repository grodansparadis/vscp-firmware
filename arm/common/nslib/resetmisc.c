/* 
 * resetmisc.c --
 *
 *	A function to do miscellaneous initialisation on startup.
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

#include <nslib.h>


/*
 *----------------------------------------------------------------------
 *
 * _ResetMisc --
 *
 *	Initialise some IO modules to save power.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The A-to-D comparators and VCO are switched off.
 *
 *----------------------------------------------------------------------
 */

void
_ResetMisc(void)
{
	ATODICR = 0;
	VID(FrequencySynthesisRegister, 0x0000C688);
}
