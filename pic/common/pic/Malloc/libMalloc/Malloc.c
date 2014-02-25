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
**
** Installation:At least version 11.02 of WizC is required.
**				Copy Malloc.h and (optionally) MallocConfig.h
**				to the directory "C:\Program Files\FED\PIXIE\Libs\LibsUser".
**				Copy Malloc.c to the directory (if not already present,
**				create the directory first)
**				"C:\Program Files\FED\PIXIE\Libs\LibsUser\libMalloc".
**				Use "#include <malloc.h>" in your application and all
**				should be well.
**				Modify MallocConfig.h for setting installation-wide
**				options. Copy MallocConfig.h into a project-directory
**				and modify it, to set different options for a specific
**				project.
**
** Overview:	This library implements a fully functional dynamic
**				memory allocation and de-allocation scheme for
**				Fed's wizC. A limited, but very memory-efficient, model
**				and an unlimited version are available. The limited model
**				can allocate segments with a maximum size of 127 bytes
**				each. The unlimited model can hold segments with 
**				a size up to the size of the heap. The size of the heap
**				is limited by available ram only for both models (PIC18).
**
** Strategy:	Depending on the model selected, a type (malloc_t) is
**				defined to hold the sizes (also for function-arguments)
**				and the memory-segment-headers.
**				Malloc_t is 8-bit unsigned for the small model and 16-bit
**				unsigned for the unlimited (large) model.
**				A private heap is used. The size is compile-time defined.
**				Each memorysegment on the heap has a header of type
**				malloc_t. The header is, therefore, 1 byte in size for
**				the small model and 2 bytes in size for the unlimited
**				(large) model. the uppermost bit of the header is used
**				to indicate if a segment is currently available or in use.
**				The remaining bits are used to store the size (in
**				malloc_t units) of the segment. A special header
**				(used, zero size) is used to indicate the end of the heap.
**				This way we have a heap on which a single linked list
**				of variable size memory segments is maintained.
*********************************************************************/

/*********************************************************************
** Heapsize:	The heapsize in bytes can be changed from the default
**				(128 bytes)	by inserting the following line into
**				MallocConfig.h in the LibsUser-directory or in the
**				projectdirectory:
**					#define MALLOC_HEAP_SIZE (HeapSize)
**
** MemoryModel: Two schemes are provided. By setting a definition in
**				MallocConfig.h (see above), a very memory-efficient
**				model or an	unlimited model can be selected.
**
**				The small (efficient) model is selected by setting:
**					#define MALLOC_SMALL
**				The large (unlimited) model is selected by setting:
**					#define MALLOC_LARGE
**
**				The small model	adds just 1 byte overhead to each
**				allocation but is restricted to a maximum per
**				allocation of 127 bytes.
**
**				The large model has no such restriction but adds
**				an overhead of 2 bytes to each allocation and rounds
**				each allocation-request up to an even amount of bytes.
**
** Scribble:	Optionally, the library can overwrite (scribble) the segment
**				contents with 0x55 when the segment is released (freed).
**				Activating this option is done by including the following
**				define into MallocConfig.h (see above):
**					#define MALLOC_SCRIBBLE
**				Because of the additional CPU-cycles needed to perform this
**				initialization, the use of this option is probably
**				limited to the debugging-phase of application-development.
*********************************************************************/

/*********************************************************************
** Is the malloc library to be included in this compile?
*********************************************************************/
#if		defined(___HMCSmalloc)			\
	||	defined(___HMCScalloc)			\
	||	defined(___HMCSrealloc)			\
 	||	defined(___HMCSfree)			\
 	||	defined(___HMCSmalloc_size)		\
 	||	defined(___HMCSmalloc_good_size)

/*********************************************************************
** Include our header
*********************************************************************/
#include <malloc.h>

/*********************************************************************
** Inform the user
*********************************************************************/
#ifdef MALLOC_DEBUG
	#ifdef MALLOC_LARGE
		#warning Malloc: Large model selected
	#else
		#warning Malloc: Small model selected, maximum allowed allocation size is 127 bytes
	#endif
#endif

/*********************************************************************
** Memory model specific definitions
*********************************************************************/
#ifdef MALLOC_LARGE
	#define HEADERSIZEHEAP				(1)
	#define HEADERSIZEBYTES				(2)
	#define SEGMENT_SIZEMAX				(0x7FF0)
	#define SEGMENT_SIZEMASK			(0x7FFF)
	#define SEGMENT_STATMASK			(0x8000)
	#define SEGMENT_FREE				(0x0000)
	#define SEGMENT_USED				(0x8000)
	#define SEGMENT_INIT				(0x0000)
	#define SEGMENT_SCRIBBLE			(0x5555)
	#define ENDOFHEAP					(SEGMENT_USED)
#else
	#define HEADERSIZEHEAP				(1)
	#define HEADERSIZEBYTES				(1)
	#define	SEGMENT_SIZEMAX				(0x7F)
	#define SEGMENT_SIZEMASK			(0x7F)
	#define SEGMENT_STATMASK			(0x80)
	#define SEGMENT_FREE				(0x00)
	#define SEGMENT_USED				(0x80)
	#define SEGMENT_INIT				(0x00)
	#define SEGMENT_SCRIBBLE			(0x55)
	#define ENDOFHEAP					(SEGMENT_USED)
#endif

/*********************************************************************
** Heap definitions
** When large-model and size is odd, round upwards to even
*********************************************************************/
#define DEFAULT_HEAP_SIZE				(128)

#ifdef MALLOC_HEAP_SIZE
	#define _HEAP_SIZE					(MALLOC_HEAP_SIZE)
#else
	#define _HEAP_SIZE					(DEFAULT_HEAP_SIZE)
#endif

#if defined(MALLOC_LARGE) && (_HEAP_SIZE & 1)
	#define HEAP_SIZE					(_HEAP_SIZE + 1)
	#warning Malloc: HeapSize adjusted upwards
#else
	#define HEAP_SIZE					(_HEAP_SIZE)
#endif

/*********************************************************************
** Macro's to manipulate the heap
**********************************************************************/
#define GETNEXT(hdr)			((hdr) + GETSIZE(hdr) + HEADERSIZEHEAP)
#define GETHEADER(ptr)			(((malloc_t *)(ptr))  - HEADERSIZEHEAP)
#define GETCONTENTS(hdr)		((hdr) + HEADERSIZEHEAP)
#define SETHEADER(hdr, size, status)	*(hdr) = 				\
								(((status) & SEGMENT_STATMASK)	\
								| ((size) & SEGMENT_SIZEMASK))
/*********************************************************************
** Macro's to manipulate the segmentstatus on the heap
**********************************************************************/
#define GETSTAT(hdr)			((*(hdr)) &   SEGMENT_STATMASK)
#define SETUSED(hdr)			((*(hdr)) |=  SEGMENT_USED)
#define SETFREE(hdr)			((*(hdr)) &= ~SEGMENT_USED)

/*********************************************************************
** Macro's to manipulate the segmentsize on the heap
**********************************************************************/
#define GETSIZE(hdr)			((*(hdr)) &  SEGMENT_SIZEMASK)
#define SETSIZE(hdr, size)		*(hdr) =						\
								((*(hdr)) & ~SEGMENT_SIZEMASK)	\
								| ((size) &  SEGMENT_SIZEMASK)
#ifdef MALLOC_LARGE
	#define BtoHSIZE(size)		(((size) + 1) >> 1)
	#define HtoBSIZE(size)		((size) << 1)
#else
	#define BtoHSIZE(size)		(size)
	#define HtoBSIZE(size)		(size)
#endif

/*********************************************************************
** Init macro: Set segment contents to <value>
**********************************************************************/
#define INIT_SEGMENT(hdr, value)			\
	do {									\
		malloc_t *sptr, *eptr;				\
											\
		sptr = GETCONTENTS(hdr);			\
		eptr = sptr + GETSIZE(hdr);			\
		while(sptr < eptr) {				\
			*sptr++ = value;				\
		}									\
	} while(FALSE)

/*********************************************************************
** Scribble macro: Set segment contents to SEGMENT_SCRIBBLE
**********************************************************************/
#ifdef MALLOC_SCRIBBLE
	#define SCRIBBLE_SEGMENT(hdr)			\
		INIT_SEGMENT(hdr, SEGMENT_SCRIBBLE)
#else
	#define SCRIBBLE_SEGMENT(hdr)
#endif

/*********************************************************************
** Heap allocation
*********************************************************************/
static malloc_t 	_MallocHeap[(HEAP_SIZE) / sizeof(malloc_t)];

/*********************************************************************
** Private variables
**********************************************************************/
static bit			_HeapInited = FALSE;

/*********************************************************************
** Private function prototypes
*********************************************************************/
static void			___HMCSInitHeap(void);
static malloc_t		___HMCSmerge(malloc_t *hdrA);
static void			___HMCSsplit(malloc_t *hdr, malloc_t newsize);

/*********************************************************************
** We don't want to use precious locopt-ram for these functions.
** By using asmfunc we force parameters and locals on the stack.
*********************************************************************/
#pragma	asmfunc				__HMCSmalloc
#pragma	asmfunc				__HMCScalloc
#pragma	asmfunc				__HMCSrealloc
#pragma	asmfunc				__HMCSfree
#pragma	asmfunc				__HMCSmalloc_size
#pragma	asmfunc				__HMCSmalloc_good_size
#pragma	asmfunc				___HMCSInitHeap
#pragma	asmfunc				___HMCSmerge
#pragma	asmfunc				___HMCSsplit

/*********************************************************************
** Function:        void *__HMCSmalloc(malloc_t size)
**
** PreCondition:	none.
**
** Input:        	malloc_t size - Number of bytes to allocate.
**                  
** Output:         	void * - A pointer to the requested block
**							 of memory or NULL if not successfull.
**
** Side Effects:    none.
**
** Overview:        This function allocates a memorysegment with size 
**					<size> bytes from the heap. The new segment is
**					not initialized. The maximum segment size depends
**					on the model used (see definition of SEGMENT_SIZEMAX).
**					Allocations of zero bytes are not permitted (NULL
**					is returned).
**
** Note:            The calling function must maintain the returned
**					pointer	to correctly free memory at runtime through 
**					__HMCSfree().
**
** Strategy:		If the first free segment on the heap is of exactly
**					the correct size, it is allocated. If this segment is
**					too large, it is split. If this segment is too small,
**					an attempt is made to extend the segment by merging
**					with successive free segment(s). If all these actions
**					fail, the pointer is moved to the next free segment
**					and the above attempts are carried out on this new 
**					segment until a segment of the requested size is
**					succesfully found/created and allocated or the end of
**					the heap is reached. If a segment was allocated a
**					pointer to it is returned, if the end of the heap
**					was reached a NULL pointer is returned. A new segment
**					is not initialized.
*********************************************************************/
#if		defined(___HMCSmalloc)			\
	||	defined(___HMCScalloc)			\
	||	defined(___HMCSrealloc)
void *__HMCSmalloc(malloc_t size)
{
	malloc_t		*pHeap;
	malloc_t		segLen;

	if(!_HeapInited) {						// If not already done,
		___HMCSInitHeap();					//   initialize the heap
	}

	if ((size == 0) || (size > SEGMENT_SIZEMAX)) {
		return((void *) NULL);				// Request zero or too big
	}
	
	size = BtoHSIZE(size);					// ByteSize to HeapSize
	pHeap = _MallocHeap;					// Init the heappointer

	while (*pHeap != ENDOFHEAP) {			// Until end-of-heap
		if (GETSTAT(pHeap) == SEGMENT_USED) { // If segment allocated
			pHeap = GETNEXT(pHeap);			// 	   Point to next segment
		} else {							// Segment is free
			segLen = GETSIZE(pHeap);		// Get the segment length
			if (size > segLen) {			// If the free segment is too small then try to merge
				if (!___HMCSmerge(pHeap)) {	// If the merge fails then move on to the next segment
					pHeap = GETNEXT(pHeap);	//	Point to next segment
				}
			} else {
				___HMCSsplit(pHeap, size); // Split it up
				SETUSED(pHeap);			// Allocate the segment
				return((void *) GETCONTENTS(pHeap);	// Return the pointer
			}			
		}
	}
	return((void *) NULL);
}
#endif


/*********************************************************************
** Function:        void *__HMCScalloc(malloc_t count, malloc_t size)
**
** PreCondition:	none.
**
** Input:        	malloc_t count - Number of elements to allocate.
**					malloc_t size  - Size of each element in bytes.
**                  
** Output:         	void * - A pointer to the requested block
**							 of memory or NULL if not successfull.
**
** Side Effects:    none.
**
** Overview:        This function allocates a memorysegment with size 
**					<count> * <size> bytes from	the heap. The new segment
**					is initialized to all 0x00. The maximum segment
**					size depends on the model used (see definition of 
**					SEGMENT_SIZEMAX). Allocations of zero bytes
**					are not permitted (A NULL pointer is returned).
**
** Note:            The calling function must maintain the returned
**					pointer	to correctly free memory at runtime through 
**					__HMCSfree().
**
** Strategy:		This function allocates a memorysegment with size
**					(<count> * <size>) bytes by calling ___HMCSmalloc().
**					if this call is unsuccesfull, a NULL pointer is
**					returned. If the memory was successfully allocated
**					it is intialized to all 0x00 and a pointer to 
**					the segment's contents is returned.
*********************************************************************/
#ifdef ___HMCScalloc
void *__HMCScalloc(malloc_t count, malloc_t size)
{
	malloc_t *ptr;

	/*
	** Request the memory from the heap
	*/	 
	if((ptr = (malloc_t *)__HMCSmalloc(count * size)) == NULL) {
		return((void *) NULL);
	}

	/*
	** Initialize the aquired memory
	*/
	INIT_SEGMENT(GETHEADER(ptr), SEGMENT_INIT);
	
	return((void *)ptr);
}
#endif


/*********************************************************************
** Function:        void *__HMCSrealloc(void *oldptr, malloc_t newsize)
**
** PreCondition:	<oldptr> must be a valid pointer returned by a 
**					previous call to __HMCSmalloc(), __HMCScalloc() or
**					__HMCSrealloc(). When <oldptr> is NULL, a normal
**					__HMCSmalloc() is executed. When <oldptr> is nonNULL
**					and <newsize> is zero, the memory pointed to by
**					<oldptr> is freed and NULL is returned.
**
** Input:			void *oldptr	- A pointer previously returned by
**									  __HMCSmalloc(), __HMCScalloc() or
**									  __HMCSrealloc() or NULL.
**		        	malloc_t newsize- Number of bytes to allocate.	
**                  
** Output:         	void * - A pointer to the adjusted block
**							 of memory.
**
** Side Effects:    if <oldptr> is not a valid pointer returned from
**					__HMCSmalloc, __HMCScalloc() or __HMCSrealloc(), the
**					result of this function is unpredictable.
**
** Overview:        This function adjusts the size of a memorysegment,
**					previously aquired through a call to __HMCSmalloc(),
**					__HMCScalloc() or __HMCSrealloc(), to a new size of
**                  <newsize> bytes, the adjusted segment is initialized
**					with the contents of the old segment pointed to
**					by <oldptr>. If succesfull, a (possibly new) pointer
**					to the adjusted	segment is returned. The old pointer
**					<oldptr> is no longer valid. When not succesfull,
**					a NULL-pointer is returned. Requests to	truncate a
**					segment to zero bytes will free the segment and
**					return a NULL-pointer.
**
** Note:            The calling function must maintain the returned
**					pointer	to correctly free memory at runtime through 
**					__HMCSfree().
**
** Strategy:		If the current segment fits the new request, the
**					segmentheader is updated to reflect the new size and
**					if necesary a new segment is created to contain the
**					truncated memory.
**					If <newsize> is larger than the current segment, an
**					attempt is made to merge succesive free segments
**					until the total size satisfies the request.
**					If merging fails, a new segment of <newsize> bytes is
**					allocated through a call to __HMCSmalloc(),
**					the contents of the old segment is copied to the
**					new segment, finally the old segment is released.
**					In all cases a pointer to the adjusted segment
**					is returned upon success. A NULL pointer is returned
**					when the reallocation failed.
**					
*********************************************************************/
#ifdef ___HMCSrealloc
void *__HMCSrealloc(void *oldptr, malloc_t newsize)
{
	malloc_t	*oldhdr, *newhdr, *newptr, *temp;
	malloc_t	oldsize;
	
	if(oldptr == NULL) {				// I have a forgiving nature
		return((void *) __HMCSmalloc(newsize)); // Treat as call to malloc()
	}

	if(newsize == 0) {					// Truncate to zero?
		__HMCSfree(oldptr);				//   Treat as call to free()
		return((void *) NULL;
	}

	if(newsize > SEGMENT_SIZEMAX) {
		return((void *) NULL);			// Request too big
	}

	if(!_HeapInited) {					// Sorry?
		return((void *) NULL);			// you can't be serious...
	}
	
	oldhdr	= GETHEADER((malloc_t *) oldptr); // Get the current header
	oldsize	= GETSIZE(oldhdr);			// Get current segmentsize
	
	newsize = BtoHSIZE(newsize);		// Transform bytesize to heapsize
	
	if(newsize == oldsize) {			// Nothing to do
		return((void *) oldptr);
	}
	
	if(newsize < oldsize) {				// A truncate is requested
		___HMCSsplit(oldhdr, newsize);	// Split current segment
		return((void *) oldptr);		// The old pointer is still valid
	}

	/*
	** A size-increase is requested
	*/
	
	/*
	** Try to merge the current segment with it's successors
	** until the new request is satisfied
	*/
	while (___HMCSmerge(oldhdr)) {		// Keep on merging
		oldsize = GETSIZE(oldhdr);
		if (oldsize >= newsize) {		// Merge successfull
			___HMCSsplit(oldhdr, newsize); // Split current segment
			return((void *) oldptr);	// Return the pointer
		}
	}
	
	/*
	** Merging was not successfull. We have to allocate a new segment
	** and copy the data from the old segment to this new segment.
	*/
	if((temp = newptr = (malloc_t *)__HMCSmalloc(HtoBSIZE(newsize))) == NULL) { // Allocate
		return((void *) NULL);
	}
	
	while(oldsize--) {				// Copy the contents from the old segment
		*newptr++ = *(malloc_t *)oldptr++;	// to the new segment
	}	

	SCRIBBLE_SEGMENT(oldhdr);
	SETFREE(oldhdr);				// Release the old segment
	
	return((void *)temp);			// Return the new pointer
}
#endif


/*********************************************************************
** Function:        void __HMCSfree(void *ptr)
**
** PreCondition:	<ptr> must be a valid pointer returned by a 
**					previous call to __HMCSmalloc(), __HMCScalloc() or
**					__HMCSrealloc().
**
** Input:        	void *ptr - pointer to the allocated memory to
**								be freed.
**                  
** Output:         	none
**
** Side Effects:    if <ptr> is not a valid pointer returned from
**					__HMCSmalloc, __HMCScalloc() or __HMCSrealloc(), the
**					result of this function is unpredictable. When <ptr>
**					is NULL, no action is performed.
**
** Overview:       	This function de-allocates a previously allocated
**					segment of memory. The freed segment is returned
**					to the heap.
**
** Note:			none
**
** Strategy:		If configured, the segment is initialized with
**					a scribble-value of 0x55.
**					The segmentstatus is updated from allocated to
**					free. No attempt is made to merge free segments.
*********************************************************************/
#ifdef ___HMCSfree
void __HMCSfree(void *ptr)
{	
	malloc_t	*hdr;

	if(ptr != NULL) {
		hdr = GETHEADER(ptr);
		SCRIBBLE_SEGMENT(hdr);
		SETFREE(hdr);		// Release the segment
	}
}
#endif


/*********************************************************************
** Function:        malloc_t __HMCSmalloc_size(void *ptr)
**
** PreCondition:	<ptr> must be a valid pointer returned by a 
**					previous call to __HMCSmalloc(), __HMCScalloc() or
**					__HMCSrealloc().
**
** Input:        	void *ptr - pointer to the allocated memory of which
**								the size is to be returned.
**                  
** Output:         	malloc_t  - The actual size (including slack) of the
**								segment pointed	to by <ptr>.
**
** Side Effects:    if <ptr> is not a valid pointer returned from
**					__HMCSmalloc(), __HMCScalloc() or __HMCSrealloc, the
**					result of this function is unpredictable.
**
** Overview:       	This function returns the size of the segment
**					pointed to by <ptr>.
**
** Note:			none
**
** Strategy:		The actual size (including slack) of the segment
**					pointed to by <ptr> is returned.
*********************************************************************/
#ifdef ___HMCSmalloc_size
malloc_t __HMCSmalloc_size(void *ptr)
{
	return(HtoBSIZE(GETSIZE(GETHEADER(ptr)));
}
#endif


/*********************************************************************
** Function:        malloc_t __HMCSmalloc_good_size(malloc_t size)
**
** PreCondition:	none.
**
** Input:        	malloc_t size - The size that needs to be adjusted.
**                  
** Output:         	malloc_t  - The adjusted size.
**
** Side Effects:    none.
**
** Overview:       	This function rounds <size> up to a value that
**					the can be allocated without adding any padding
**					and returns that rounded up value. If <size> is
**					larger than the maximum allowed segmentsize, this
**					maximum is returned.
**
** Note:			none
*********************************************************************/
#ifdef ___HMCSmalloc_good_size
malloc_t __HMCSmalloc_good_size(malloc_t size)
{
	if (size < SEGMENT_SIZEMAX) {
		return(HtoBSIZE(BtoHSIZE(size)));
	}

	return(SEGMENT_SIZEMAX);
}
#endif


/*********************************************************************
** Supporting functions
*********************************************************************/

/*********************************************************************
** Initialize the heap by creating unused segments that are as large 
** as possible.
*********************************************************************/
static
void ___HMCSInitHeap(void)
{
	malloc_t		*pHeap;
	unsigned int	count;
	
	pHeap = _MallocHeap;
	count = HEAP_SIZE - HEADERSIZEBYTES;	// Account for end-of-heap-header

	while (TRUE) {
		if (count > SEGMENT_SIZEMAX) {
			SETHEADER(pHeap, BtoHSIZE(SEGMENT_SIZEMAX), SEGMENT_FREE);
			SCRIBBLE_SEGMENT(pHeap);
			pHeap		= GETNEXT(pHeap);
			count	   -= (SEGMENT_SIZEMAX + HEADERSIZEBYTES);
		} else {
			SETHEADER(pHeap, BtoHSIZE(count - HEADERSIZEBYTES), SEGMENT_FREE);
			SCRIBBLE_SEGMENT(pHeap);
			pHeap		= GETNEXT(pHeap);
			*pHeap		= ENDOFHEAP;		// signal end-of-heap
			break;
		}
	}
	_HeapInited = TRUE;
}


/*********************************************************************
** Try to combine the segment pointed to by <hdrA> with it's successor.
*********************************************************************/
static
malloc_t ___HMCSmerge(malloc_t *hdrA)
{
	malloc_t *hdrB;
	malloc_t sizeA;
	malloc_t sizeB;
	
	hdrB = GETNEXT(hdrA);					// Next segment
	
	if(*hdrB == ENDOFHEAP) {				// end-of-heap?
		return(FALSE);
	}
	
	if(GETSTAT(hdrB) == SEGMENT_USED) {		// next segment is in use
		return(FALSE);
	}

	sizeA = GETSIZE(hdrA);
	sizeB = GETSIZE(hdrB);
	if((sizeA + sizeB + HEADERSIZEHEAP) <= SEGMENT_SIZEMAX) {
		/*
		** We can combine the two segments into one large segment
		*/
		SETSIZE(hdrA, (sizeA + sizeB + HEADERSIZEHEAP));
	} else {
		/*
		** Simply combining the two segments would result in a new
		** segment that is larger than SEGMENT_SIZEMAX.
		** We extend the first segment to the maximum and adjust
		** the second segment for the remaining space.
		** We have to be carefull not to overflow the malloc_t type.
		*/
		sizeB -= (SEGMENT_SIZEMAX - sizeA);	// New segmentB size
		SETSIZE(hdrA, SEGMENT_SIZEMAX);		// New segmentA size
		SETHEADER(GETNEXT(hdrA), sizeB, SEGMENT_FREE); // New B header
	}
	
	return(TRUE);							// Succesfull merge!
}


/*********************************************************************
** Split segment <hdr> into two segments. The first segment will
** contain <newsize> bytes, the second segment will contain the
** remaining bytes. This might result in the second segment containing
** zero bytes, this is ok.
*********************************************************************/
static
void ___HMCSsplit(malloc_t *hdr, malloc_t newsize)
{
	malloc_t oldsize;
	
	oldsize	= GETSIZE(hdr);				// Get current size
	if(oldsize > newsize) {
		SETSIZE(hdr, newsize);			// Change current segment
		hdr = GETNEXT(hdr);				// Here starts the second (new) segment
		newsize = oldsize - newsize;	// Get size of new segment
		SETHEADER(hdr, (newsize - HEADERSIZEHEAP), SEGMENT_FREE); // Create it
		SCRIBBLE_SEGMENT(hdr);
	}
}

#endif	/* Is the malloc library to be included in this compile? */
