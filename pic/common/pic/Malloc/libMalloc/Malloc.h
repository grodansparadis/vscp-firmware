/*********************************************************************
** Title:		Malloc - Memory (de-)allocation library for wizC.
**
** Author:		Marcel van Lieshout
**
** Copyright:	(c) 2005, HMCS, Marcel van Lieshout
**
** License:		This software is released to the public domain and comes
**				without	warranty and/or guarantees of any kind. You have
**				the right to use, copy, modify and/or (re-)distribute the
**				software as long as the reference to the author is
**				maintained in the software (in it's original or in
**				a modified form).
*********************************************************************/
#ifndef _MALLOC_H
#define _MALLOC_H

/*********************************************************************
** Load general/project settings (optional)
*********************************************************************/
#pragma wizcpp includeoptional "MallocConfig.h"

/*********************************************************************
** Test for a correct memorymodel, set small as default
*********************************************************************/
#if !defined(MALLOC_SMALL) && !defined(MALLOC_LARGE)
	#define MALLOC_SMALL
#elif defined(MALLOC_SMALL) && defined(MALLOC_LARGE)
	#error Malloc: Cannot use both memorymodels in a single application
#endif

/*********************************************************************
** The size of the used type depends on the memorymodel selected
*********************************************************************/
#ifdef MALLOC_LARGE
	typedef unsigned int	malloc_t;
#else
	typedef unsigned char	malloc_t;
#endif

/*********************************************************************
** Some common definitions
*********************************************************************/
#ifndef NULL
	#define NULL			(0)
#endif
#ifndef FALSE
	#define FALSE			(0)
#endif
#ifndef TRUE
	#define TRUE			(1)
#endif

/*********************************************************************
The functions, provided by this library
*********************************************************************/
extern	void				*__HMCSmalloc(malloc_t size);
extern	void				*__HMCScalloc(malloc_t count, malloc_t size);
extern	void				*__HMCSrealloc(void *ptr, malloc_t size);
extern	void				__HMCSfree(void *ptr);
extern	malloc_t			__HMCSmalloc_size(void *ptr);
extern	malloc_t			__HMCSmalloc_good_size(malloc_t size);

#define	malloc(s)			__HMCSmalloc(s)
#define calloc(c, s)		__HMCScalloc(c, s)
#define valloc(s)			__HMCSmalloc(s)
#define realloc(p, s)		__HMCSrealloc(p, s)
#define free(p)				__HMCSfree(p)
#define malloc_size(p)		__HMCSmalloc_size(p)
#define malloc_good_size(s)	__HMCSmalloc_good_size(s)

/*********************************************************************
** Tell the world about our library
*********************************************************************/
#pragma wizcpp uselib "$__PATHNAME__/libMalloc/Malloc.c"

#endif	/*_MALLOC_H*/
