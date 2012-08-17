/* 
 * number.c --
 *
 *	Convert an integer to an ASCII string for printing.
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
 * number --
 *
 *	Convert an integer to a hexadecimal string.
 *
 * Results:
 *	Returns a pointer to a statically allocated string containing
 *	a hexadecimal representation of n.
 *
 * Side effects:
 *	The statically allocated string is altered.
 *
 *----------------------------------------------------------------------
 */

char *
number(unsigned int n)
{
	static const char hexchars[] = "0123456789ABCDEF";

	static char s[11];
	
	s[0] = '0';
	s[1] = 'x';
	s[2] = hexchars[(n >> 28) & 0x0F];
	s[3] = hexchars[(n >> 24) & 0x0F];
	s[4] = hexchars[(n >> 20) & 0x0F];
	s[5] = hexchars[(n >> 16) & 0x0F];
	s[6] = hexchars[(n >> 12) & 0x0F];
	s[7] = hexchars[(n >>  8) & 0x0F];
	s[8] = hexchars[(n >>  4) & 0x0F];
	s[9] = hexchars[(n >>  0) & 0x0F];
	s[10] = '\0';
	return s;
}
