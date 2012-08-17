/* 
 * putchar.c --
 *
 *	Maintain a text cursor at which to display characters for debugging.
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
 * putchar --
 *
 *	Output a character to the display.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The character c is drawn on the display at the current
 *	text cursor position, which is updated.
 *
 *----------------------------------------------------------------------
 */

void
putchar(char c)
{
	static int x = 0, y = 0;

	switch (c) {
	    case '\n':
	    	x = 0;
	    	y += 12;
	    	break;
	    case '\r':
	    	x = 0;
	    	break;
	    default:
	    	VidChar(c, x, y, 255, 0);
	    	x += 6;
	    	if (x >= (vidMaxX - 6)) {
	    		x = 0;
	    		y += 12;
	    	}
	    	if (y >= (vidMaxY - 12)) {
	    		y = 0;
	    	}
	}
}
