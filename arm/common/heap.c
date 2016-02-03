/***************************************************************************
   Copyright © Intel Corporation, March 18th 1998.  All rights reserved.
   Copyright ARM Limited 1998 - 2001.  All rights reserved.
**************************************************************************** 

   This is a simple heap allocator.  It allocates memory in fixed "chunk" 
   sizes.  The chunk size is a power of 2, for example 128.  It allocates
   as many chunks as it takes to hold the memory size requested and a chunk
   header which describes the block of chunks.  All blocks of chunks are
   kept in the memory list pointed at by "first".  The header describes the
   number of chunks in this block, whether or not it is free and how much
   memory the caller asked for.  It also contains a magic pattern that
   allows the deallocation code to check for memory corruption.  The
   smaller the chunk size the less wasted memory, however a chunk must be
   bigger than the chunk header.

   The memory allocation algorithm is to find the first block of chunks big
   enough.  If this block of chunks is too large, the block is broken into 
   two blocks.  The first block of chunks is used for the memory and second
   is made free.  This is a "first found" algorithm, it could be more
   efficient but the search would take longer.
  
   When freeing memory we could check to see if the block of chunks being
   freed could be merged with either the previous or next block of chunks
   (or both) if they are already free.  This collects the memory back into
   larger chunks and helps prevent excessive fragmentation.  Rather than do
   this every time memory is deallocated, we wait until either we cannot
   find a free block of chunks big enough or we are concerned that there is
   too much fragmentation.

	$Id: heap.c,v 1.1 2005/12/19 11:45:57 akhe Exp $

****************************************************************************/

#include "uhal.h"

#define MAX_NO_REGIONS          10
#define MAX_NO_MEMORY_AREAS     MAX_NO_REGIONS + 1

struct tImageRegions
{
    unsigned int *RegionBasePtr;   // Base of region
    unsigned int *RegionEndPtr;    // Limit of region
};

struct tBoardMemoryDesc
{
    unsigned int *ptrLocation;
    unsigned int *ptrEndLocation;
    unsigned int Allocated;
};

//struct tBoardMemoryDesc uHALiv_TableOfMemoryAreas[MAX_NO_MEMORY_AREAS];
//unsigned int uHALiv_NumOfMemoryAreas = 0;

#if uHAL_HEAP != 0

/* memory chunk descriptor */
typedef struct chunkHeader
{
    unsigned int Magic;		// magic number

    unsigned int chunkSize;	// size of chunk in chunks

    unsigned int allocatedSize;	// allocated size in bytes

    struct chunkHeader *next;	// next chunk in the list

}
chunkHeader_t;

struct chunkStatistics
{
    unsigned int totalMemory;
    unsigned int allocatedMemory;
    unsigned int maxChunks;
    unsigned int totalBlocks;
};

static struct chunkStatistics statistics;

#define HEADER_SIZE sizeof(chunkHeader_t)
#define CHUNK_MAGIC 0xDEADBEEF

/* minimum size of memory to be allocated */
#define CHUNK_SIZE_IN_BITS 7
#define CHUNK_SIZE (1 << CHUNK_SIZE_IN_BITS)

/* How many times to try and merge the chunks when freeing them */
#define MAX_MERGE_ATTEMPTS 2

/* How many times do we try to allocate memory when there are not any 
 * blocks of chunks big enough */
#define MAX_ALLOCATE_ATTEMPTS 5

/* first memory chunk pointers (held in ascending memory order) */
chunkHeader_t *first = NULL;

/* local (static) routines */
static int easyMergeChunks(chunkHeader_t * blockP);
static void mergeChunks(void);
static chunkHeader_t *firstFit(unsigned int chunks);
static int uHAL_heap_inited = 0;

// The following __weak symbols are required to 
// determine C library support level.
__weak void free ( void * );
__weak void * malloc ( unsigned int );
__weak void _init_alloc(unsigned /*base*/, unsigned /*top*/);

extern unsigned int uHALiv_TopOfMemory;
extern unsigned int uHALiv_BaseOfMemory;
extern unsigned int uHALiv_TopOfHeap;

/* ----------------------- static source code ----------------------------- */
static void mergeChunks(void)
{
    chunkHeader_t *blockP;
    int i;
    int count;

    /* See if you can merge this freed chunk with another one */
    for (i = 0; i < MAX_MERGE_ATTEMPTS; i++)
    {

	count = 0;
	blockP = first;

	while (blockP)
	{
	    count += easyMergeChunks(blockP);
	    blockP = blockP->next;
	}

	/* if we didn't merge any chunks this pass, then give up */
	if (count == 0)
	    return;
    }
}

static chunkHeader_t *firstFit(unsigned int chunks)
{
    chunkHeader_t *blockP = first;

    /* very, very simple algorithm, find the first chunk big enough */
    while (blockP)
    {
#ifdef DEBUG
	    if (blockP->Magic != CHUNK_MAGIC)
	    {
	        uHALr_printf("ERROR: corrupt heap @ 0x%x\n", blockP);
	        return NULL;
	    }
#endif
	    if ((blockP->chunkSize > chunks) && (blockP->allocatedSize == 0))
	    {
	        return blockP;
	    }
	    blockP = blockP->next;
    }
    return NULL;
}

/* merge this chunk and the next, if they are both free */
static int easyMergeChunks(chunkHeader_t * blockP)
{
    chunkHeader_t *nextP = blockP->next;

    if (nextP)
    {
	    if ((blockP->allocatedSize == 0) && (nextP->allocatedSize == 0))
	    {

	        /* swallow (merge) the next chunk. */
	        blockP->next = nextP->next;
	        blockP->chunkSize += nextP->chunkSize;
	        nextP->Magic = ~CHUNK_MAGIC;

	        /* account for the death of a block of chunks */
	        statistics.totalBlocks--;
	        return 1;
	    }
    }
    return 0;
}

/* ------------------ externally available routines ------------------------ */
int uHALr_HeapAvailable(void)
{
    return TRUE;
}

/* Reinitializes the heap, either the uHAL or C library */
/* version. */
void uHALir_ReInitHeap(void)
{
    uHAL_heap_inited = 0;

#ifdef ADS_BUILD
    if ((void *)_init_alloc == NULL)
    {   // Init the uHAL Heap.
        uHALr_InitHeap();       
    }
    else
    {   // Reinit the C library Heap.
        _init_alloc(uHALiv_BaseOfMemory, uHALiv_TopOfHeap);
    }
#else   // In the SDT case we use the uHAL malloc routines
        // so init those.
    uHALr_InitHeap();       
#endif
}

/* initialize the heap allocation scheme */
void uHALr_InitHeap(void)
{
    unsigned int heap_size;

#ifdef DEBUG
    uHALr_printf("uHALr_InitHeap() called, uHAL_HEAP_BASE = 0x%x\n",
		 uHAL_HEAP_BASE);
#endif

    /* do not initialise more than once */
    if (!uHAL_heap_inited)
    {
	    uHAL_heap_inited = 1;

	    /*Tests to see if the library version of malloc is available */
	    /*but does not allocate any memory size = 0. */
        if ((void *)malloc == NULL)
        {
    #ifdef ADS_BUILD
	        heap_size = uHALr_SizeOfFreeRam();

	        /* set up the free block (one big one to start with) */
	        first = (chunkHeader_t *) uHALr_StartOfFreeRam();
	        first->Magic = CHUNK_MAGIC;
	        first->allocatedSize = 0;
	        first->chunkSize = heap_size >> CHUNK_SIZE_IN_BITS;
	        first->next = NULL;

	        /* set up the statistics */
	        statistics.totalMemory = heap_size;
	        statistics.allocatedMemory = 0;
	        statistics.maxChunks = heap_size >> CHUNK_SIZE_IN_BITS;
	        statistics.totalBlocks = 1;
    #endif
	    }
    }
}

/* allocate memory, make this as quick as possible */
void *uHALr_malloc(unsigned int size)
{
    unsigned int chunks;
    void *test_support_ptr = NULL;
    chunkHeader_t *blockP;
    int tries;

    if (size != 0)
    {
        if ((void *)malloc == NULL)
        {
	        /* figure out the number of chunks we need */
	        chunks = (~(CHUNK_SIZE - 1) &
	        (CHUNK_SIZE + size + sizeof(chunkHeader_t))) >> CHUNK_SIZE_IN_BITS;

	        /* find the best fit for this number of blocks (retry if neccessary) */
	        for (tries = 0; tries < MAX_ALLOCATE_ATTEMPTS; tries++)
	        {

		    /* find the first block that fits */
		    blockP = firstFit(chunks);

		    /* if we got a chunk, do we need to break it into smaller 
		     * * * chunks? */
		    if (blockP)
		    {
		        if (blockP->chunkSize > chunks)
		        {
			        chunkHeader_t *newP;

			        /* make a new chunk and thread it into the list, make sure
			         * * that the allocator gets the chunk at the end, this
			         * * tends to keep the freeblocks of chunks at the start of
			         * * the memory list. */
			        newP = (chunkHeader_t *) ((char *)blockP
					               + ((blockP->chunkSize - chunks)
						          * CHUNK_SIZE));
			        newP->Magic = CHUNK_MAGIC;
			        newP->next = blockP->next;
			        newP->allocatedSize = 0;
			        newP->chunkSize = blockP->chunkSize - chunks;

			        blockP->next = newP;
			        blockP->chunkSize -= chunks;

			        /* account for the birth of a chunk */
			        statistics.totalBlocks++;

			        /* Use the new block of chunks */
			        blockP = newP;
		            }
		            /* account for this memory */
		            statistics.allocatedMemory += (chunks * CHUNK_SIZE);

		            /* mark this memory allocated */
		            blockP->Magic = CHUNK_MAGIC;
		            blockP->allocatedSize = size;
		            blockP->chunkSize = chunks;

		            /* return the caller a pointer to its memory */
		            return (void *)((unsigned char *)blockP +
				            sizeof(chunkHeader_t));
		        }
    		    /* try and merge any free chunks that are next to each other 
	    	     * * * and then retry the allocate */
		        mergeChunks();
	        }
	    }	    /* ran out of retries, give up */
        else
        {
            test_support_ptr = malloc(size);
        }
    }

    return test_support_ptr;
}

void uHALr_free(void *where)
{
    /* Check if memory was really allocated. */
    if (where != NULL)
    {
        if ((void *)free == NULL)
        {
	        chunkHeader_t *blockP = (chunkHeader_t *) ((char *)where - sizeof(chunkHeader_t));

	        /* check the magic header number */
	        if (blockP->Magic != CHUNK_MAGIC)
	        {
    		    return;
	        }

	        /* check that this block is indeed allocated */
	        if (blockP->allocatedSize == 0)
	        {
	    	    return;
	        }

	        /* account for this memory */
	        statistics.allocatedMemory -= (blockP->chunkSize * CHUNK_SIZE);

	        /* mark this block of chunks as free, don't attempt to merge (yet) */
	        blockP->allocatedSize = 0;
	    }
        else
        {
            free(where);
        }
    }
    return;
}

#else
/* ------------------ externally available routines ------------------------ */
int uHALr_HeapAvailable(void)
{
    return FALSE;
}
#endif

/***********************************************************************/ 
//
// The following routine accepts the location of the Code and Data
// regions of the image and using uHALiv_BaseOfMemory, uHALiv_TopOfHeap
// and the FLASH_BASE it calculates the areas of RAM suitable for
// allocation to the heap, placing them in uHALiv_TopOfHeap and 
// uHALiv_BaseOfMemory.
//
// Assumption:  That the BSS image region is located within the range
//              uHALiv_BaseOfMemory -> uHALiv_TopOfHeap.
//
// To use the heap extend facility simply restore the commented out
// regions of code (including the defs at the top of the file). Aswell
// as the __rt_heap_extend code sections.
//
// Non contiguous lumps of memory can be used by simply adding them to
// the uHALiv_TableOfMemoryAreas structure.
//
/***********************************************************************/
void uHALir_StackHeapInit ( unsigned int *startofROM, unsigned int *topofROM,
                            unsigned int *startofBSS, unsigned int *endofBSS )
{
    unsigned int i;
    unsigned int savedUpper, savedLower;
    unsigned int upper, lower, testgap;
    unsigned int CurrentLargestGap;
    unsigned int RegionsCount = 0;
    struct tImageRegions uHALiv_TableOfRegions[MAX_NO_REGIONS];
    static unsigned int uHALiv_StackHeapInited = 0;
    
    if (uHALiv_StackHeapInited == 0)
    {   // Prevent us from being re init'd.
        uHALiv_StackHeapInited = 1;

        uHALiv_TableOfRegions[0].RegionBasePtr = startofBSS;
        uHALiv_TableOfRegions[0].RegionEndPtr = endofBSS;
        uHALiv_TableOfRegions[1].RegionBasePtr = startofROM;
        uHALiv_TableOfRegions[1].RegionEndPtr = topofROM;

        // Assume Code and Data are in RAM, so the number of memory regions is two.
        RegionsCount = 2;
        if ((startofROM >= (unsigned int *)BOOT_FLASH_BASE) && 
            (startofROM <= (unsigned int *)FLASH_END))
        {   // Code is in FLASH, so limit the memory regions to one.
            RegionsCount = 1;
        }
        else
        {
            if (startofROM <= startofBSS)
            {
                uHALiv_TableOfRegions[0].RegionBasePtr = startofROM;
                uHALiv_TableOfRegions[0].RegionEndPtr = topofROM;
                uHALiv_TableOfRegions[1].RegionBasePtr = startofBSS;
                uHALiv_TableOfRegions[1].RegionEndPtr = endofBSS;
            }
        }

        // Right now lets find the largest gap to give to the heap.
        upper = (unsigned int)uHALiv_TableOfRegions[0].RegionBasePtr - 4;
        lower = uHALiv_BaseOfMemory;
        CurrentLargestGap = savedUpper = savedLower = 0;
        for (i = 0; i <= RegionsCount; i++)
        {   
            if (lower > upper)
	            testgap = 0;
            else
    	    {   // We have a gap, how big is it ?.
                testgap = upper - lower;
//    	        if (testgap != 0)
//	            {   // Store this gap in case we want to extend the heap with it.
//	                uHALiv_TableOfMemoryAreas[uHALiv_NumOfMemoryAreas].ptrLocation = (unsigned *)lower;
//                    uHALiv_TableOfMemoryAreas[uHALiv_NumOfMemoryAreas].ptrEndLocation = (unsigned *)upper;
//                    uHALiv_TableOfMemoryAreas[uHALiv_NumOfMemoryAreas].Allocated = FALSE;
//    	            uHALiv_NumOfMemoryAreas++;
//	            }
    	    }
	
	        if (testgap > CurrentLargestGap)
	        {   // This is the largest gap so far so save it.
	            savedUpper = upper;
	            savedLower = lower;
	            CurrentLargestGap = testgap;
	        }
        
    	    if (i == (RegionsCount - 1))
	            upper = uHALiv_TopOfHeap; // Limit of memory area is defined by the top of the heap.
            else
	            upper = (unsigned int)uHALiv_TableOfRegions[i + 1].RegionBasePtr - 4;

            lower = (unsigned int) uHALiv_TableOfRegions[i].RegionEndPtr + 4;
        }

        // Store the largest gap found to be used as the heap.
        uHALiv_BaseOfMemory = savedLower;
        uHALiv_TopOfHeap = savedUpper;

//        for (i = 0; i < uHALiv_NumOfMemoryAreas; i++)
//        {
//            if ((unsigned int *)uHALiv_BaseOfMemory == uHALiv_TableOfMemoryAreas[i].ptrLocation)
//            {
//                uHALiv_TableOfMemoryAreas[i].Allocated = TRUE;
//            }            
//        }
    }
}


// Routine to provide additional memory to the heap.
// Could be non-contiguous memory.
unsigned int __rt_heap_extend ( unsigned int size, void ** location )
{
//    unsigned int i;
//    unsigned int block_size;
    unsigned int return_value = 0;

//    for (i = 0; i < uHALiv_NumOfMemoryAreas; i++)
//    {
//        block_size = (unsigned int) ((unsigned int)uHALiv_TableOfMemoryAreas[i].ptrEndLocation - 
//                                     (unsigned int)uHALiv_TableOfMemoryAreas[i].ptrLocation);
//
//        if (block_size >= size)
//        {
//            *location = (void *) (uHALiv_TableOfMemoryAreas[i].ptrLocation);
//            uHALiv_TableOfMemoryAreas[i].Allocated = TRUE;
//            return_value = block_size; 
//            break; // Dont need to continue since we have found the required space.
//        }
//    }

    return return_value;
}

// End of file - heap.c
