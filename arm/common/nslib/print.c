/* 
 * print.c --
 *
 *	Display a string on screen using putchar, for debugging.
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
 * print --
 *
 *	Output a string to the display.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The string s is written to the display at the current text
 *	cursor position, which is updated.
 *
 *----------------------------------------------------------------------
 */

void
print(const char *s)
{
	for(; *s; s++) {
		putchar(*s);
	}
}
