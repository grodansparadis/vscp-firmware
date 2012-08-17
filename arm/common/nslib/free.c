/* 
 * free.c --
 *
 *	Free memory from the heap.
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
 * HeapFree --
 *
 *	Deallocate memory from the heap.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The block of memory on the heap at ptr is deallocated.
 *	If ptr is not the start of a valid allocated block on
 *	the heap, the results are undefined.
 *
 *----------------------------------------------------------------------
 */

void
free(void *ptr)
{
	_HeapBlock *tmp;

	tmp = (_HeapBlock *)((uint)ptr - sizeof(_HeapBlock));
	tmp->prev->next = tmp->next;
	if (tmp->next != NULL) {
		tmp->next->prev = tmp->prev;
	}
}
