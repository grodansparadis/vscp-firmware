/* 
 * heapinit.c --
 *
 *	Initialise data structures to manage the heap.
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

_HeapBlock *_heapStart;
_HeapBlock *_heapLimit;

/*
 *----------------------------------------------------------------------
 *
 * HeapInit --
 *
 *	Initialise the data structures that maintain the heap.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The heap is set up.
 *
 *----------------------------------------------------------------------
 */

void
HeapInit(void)
{
	_heapStart = (_HeapBlock *)&_bottomOfHeapLogical;
	_heapLimit = (_HeapBlock *)&_topOfHeapLogical;

	_heapStart->length = sizeof(_HeapBlock);
	_heapStart->next = NULL;
	_heapStart->prev = NULL;
}
