/***************************************************************************
  ARM Firmware Suite: mem.c

   Copyright ARM Limited 2001, 2002.  All rights reserved.

****************************************************************************

    This file contains various routines related to memory management
    within uHAL.

	Revision $Revision: 1.1 $
	Checkin $Date: 2005/12/19 11:45:57 $
	Revising $Author: akhe $

****************************************************************************/

#include	"uhal.h"

extern void _init_alloc(unsigned /*base*/, unsigned /*top*/);

// The variables __heap_limit and __heap_base
// are solely for use with the SDT C library.
__weak int * __heap_limit;
__weak int * __heap_base;

// These values are written by the start-up system and then used by the
// rest of the library to access memory.
unsigned int uHALiv_TopOfHeap;
unsigned int uHALiv_BaseOfMemory;
unsigned int uHALiv_StackTop;
unsigned int uHALiv_StackLimit;
unsigned int uHALiv_TopOfMemory;

unsigned int uHALiv_MemorySize;
unsigned int uHALiv_FileCount;
unsigned int uHALv_BootSwitcherStatus;

extern unsigned int uHAL_EndOfBSS;


// Deprecated routine - just a duplicated function
#define uHALir_FindRAMTop	uHALr_EndOfRam


// Returns 1st available uninitialised RAM
int
uHALr_StartOfRam ( void )
{
    return uHAL_EndOfBSS;
}

// Returns the top of RAM
int
uHALr_EndOfRam ( void )
{
    return uHALiv_TopOfMemory;
}

// Returns the top of the heap as the end of free RAM.
int
uHALr_EndOfFreeRam ( void )
{
    if (&__heap_limit == NULL)
    {
        return uHALiv_TopOfHeap;
    }
    else
    {
        return *__heap_limit;
    }
}

// Returns the bottom of the heap as the start of free RAM.
int
uHALr_StartOfFreeRam ( void )
{
    if (&__heap_base == NULL)
    {
        return uHALiv_BaseOfMemory;
    }
    else
    {
        return *__heap_base;
    }
}

// Subtracts the top and bottom of the heap returning the size
// of free RAM.
int
uHALr_SizeOfFreeRam ( void )
{
    if ((&__heap_base == NULL) || (&__heap_limit == NULL))
    {
        return (int)(uHALiv_TopOfHeap - uHALiv_BaseOfMemory);
    }
    else
    {
        return (*__heap_limit - *__heap_base);
    }
}

/* End of file - mem.c */

