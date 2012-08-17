/* 
 * bzero.c --
 *
 *	Write zeros to a byte string.
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
 * bzero --
 *
 *	Set bytes of a string to zero.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The first n bytes of the string s are set to zero.
 *
 *----------------------------------------------------------------------
 */

void
bzero(void *s, int n)
{
	char *p = s;

	while (((int)p & 3) && n > 0) {
		*p++ = 0;
		n--;
	}
	while (n > 3) {
		*((int *)p) = 0;
		p += sizeof(int);
		n -= sizeof(int);
	}
	while (n > 0) {
		*p++ = 0;
		n--;
	}
}
