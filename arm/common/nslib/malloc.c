/* 
 * malloc.c --
 *
 *	Allocate memory on the heap.
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

#define ROUNDWORDS(x)	(((unsigned long)(x) + 3) & 0xFFFFFFFC)



/*
 *----------------------------------------------------------------------
 *
 * malloc --
 *
 *	Allocate a block of dynamic memory.  HeapInit must be called
 *	before attempting to use the heap.
 *
 * Results:
 *	Returns a pointer to a block of size bytes newly allocated
 *	on the heap, or NULL if the block could not be allocated.
 *
 * Side effects:
 *	Some memory may be allocated from the heap.  This implementation
 *	of malloc is really weak.
 *
 *----------------------------------------------------------------------
 */

void *
malloc(size_t size)
{
	_HeapBlock *cursor;
	_HeapBlock *tmp;

	if (size == 0) {
		return NULL;
	}
	size = ROUNDWORDS(size) + sizeof(_HeapBlock);
	cursor = _heapStart;
	while ( (cursor->next != NULL) &&
		(((uint)cursor->next - (uint)cursor - (uint)cursor->length)
			 < size) )
	{
		cursor = cursor->next;
	}

	if (    (cursor->next == NULL) &&
		(((uint)_heapLimit - (uint)cursor - (uint)cursor->length)
			 < size) )
	{
		return NULL;
	}

	tmp = (_HeapBlock *)((uint)cursor + cursor->length);
	tmp->length = size;
	tmp->next = cursor->next;
	tmp->prev = cursor;
	cursor->next = tmp;
	if (tmp->next != NULL) {
		tmp->next->prev = tmp;
	}
	return (_HeapBlock *)((uint)tmp + sizeof(_HeapBlock));
}
