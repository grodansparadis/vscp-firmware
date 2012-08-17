/* 
 * vidn.c --
 *
 *	C support for all video modes.
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
#include "fixedfont.h"
#include "cursor.h"

VidConfig *vidConfigPtr = NULL;


/*
 *----------------------------------------------------------------------
 *
 * VidInit --
 *
 *	Initialise and configure for a given display mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The global variable vidConfigPtr is set to point
 *	to the appropriate configuration struct, and
 *	the *Init function for that configuration is called.
 *
 *----------------------------------------------------------------------
 */

void
VidInit(VidConfig *configPtr)
{
	IRQRQA = 0x08;		/* ack interrupt */
	vidConfigPtr = configPtr;
	IrqInstall(IRQ_FLYBACK, vidConfigPtr->frameIsrProc);
	VidStart();
}
