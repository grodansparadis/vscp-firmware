/***************************************************************************
   Copyright ARM Limited 1999 - 2000.  All rights reserved.
****************************************************************************

  armload.c
  
  Purpose:
	Simple loader program which moves an image from flash to ram and
	then executes it.
	NOTE: This program simply looks in the application flash at the
	specified block number, but it would be simple to modify this to
	look for a given number (or take a parameter).

	It also shows how easy zlib makes handling compressed images.
   

	RCS $Revision: 1.1 $
	Checkin $Date: 2005/12/19 11:46:08 $
     Revising $Author: akhe $
  
****************************************************************************/

// Which block is the image programmed into 
#define IMAGE_BLOCK	10

// If the compressed image has an AIF header it has to be removed so that
// the image starts from the correct address. AFU does this when an image
// is programmed into flash. To fix the start, set this to 1:
#define ZIPPED_AIF	0

// If the image isn't compressed, but still has an AIF header set this to 1:
#define AIF_HEADER	0

// Base Address in RAM (start of image) & Execute address (might be different)
#define IMAGE_BASE	0x00100000
#define	IMAGE_EXEC	IMAGE_BASE

// This is the maximum image size. Since both source & dest are statically
// defined there are no worries about running out of memory.
#define MAX_IMAGE_SIZE	(SZ_8M + SZ_2M)


#define AIF_HEADER_SIZE	0x80

/***************************************************************************/

#include	"platform.h"
#include	"uhal.h"
#include	"flash_lib.h"
#include	"zlib.h"


typedef struct exec
{
    unsigned int *source;
    unsigned int *dest;
    unsigned int src_size;
    unsigned int dst_size;
    PrVoid start;
}
exec_t;


extern tFlash RW_Flash_setup[];
extern void exit(int);

unsigned int *find_AppFlash(unsigned int *blockSize)
{
    int num_devices;
    tFlash *ARM_Flash;

    fLib_DefinePlat((tFlash **) RW_Flash_setup);

    num_devices = fLib_FindFlash(&ARM_Flash);

    if (num_devices == 0)
    {
        // No devices were found
        printf("ERROR: Failed to find flash descriptors.\n");
        exit(1);
    }

    // Scan descriptors looking for APP Flash
    while ((ARM_Flash->type & APP_FTYPE) != APP_FTYPE)
    {
    	if (ARM_Flash->next == 0)
	{
	    // No Application devices were found
	    printf("ERROR: Failed to find application flash.\n");
	    exit(1);
	}
    	ARM_Flash = ARM_Flash->next;
    }

    *blockSize = ARM_Flash->devices->logicalSize;

    return (unsigned int *)(ARM_Flash->devices->base + (*blockSize * ARM_Flash->offset));
}


int main(int argc, int **argv)
{
    register unsigned int i;
    exec_t target;
    unsigned int blockSize, offset;
    unsigned int *source;

    // Find flash & calculate offset to required block
    source = find_AppFlash(&blockSize);
    offset = (IMAGE_BLOCK * blockSize) >> 2;
    source += offset;

    /* Setup target memory structure.
     *
     * NOTE: Assumes that the image is built for IMAGE_BASE
     *       and is a maximum size of IMAGE_SIZE
     */
    target.source = source;
    target.dest = (unsigned int *)IMAGE_BASE;
    target.start = (PrVoid) (IMAGE_EXEC);
    target.src_size = MAX_IMAGE_SIZE;
    target.dst_size = MAX_IMAGE_SIZE;

    /* So, we have a card, now copy it's contents to RAM */
    uHALr_printf("Copying data from 0x%08X to 0x%08X.\n\n",
		 target.source, target.dest);

    uHALr_printf("Copying data from 0x%08X to 0x%08X.\n\n",
		 source, target.dest);

    // Check for gzip's magic number
    i = *(target.source) & 0xFFFF;

    i = validZipID((void *)target.source);

    if (Z_OK == i)
    {
	// Watch out for zip images with AIF headers!
	if (ZIPPED_AIF != 0)
	{
	    target.dest -= AIF_HEADER_SIZE / sizeof(unsigned int);
	}

	// Try and uncompress the image first
	i = unzip((void *)target.dest, (unsigned long *)&target.dst_size,
		  (void *)target.source, target.src_size);

	if (Z_OK != i)
	{
	    uHALr_printf("Couldn't decompress image!\n");
	    return -1;
	}
    }
    else
    {
	register unsigned int *source = target.source;
	register unsigned int *dest = target.dest;

	uHALr_printf("Image isn't compressed. ");
	uHALr_printf("Copying up to %dKB to RAM...\n",
		     (target.dst_size / 1024));

	// Watch out for images with AIF headers!
	if (AIF_HEADER != 0)
	{
	    target.dest -= AIF_HEADER_SIZE / sizeof(unsigned int);
	}

	for (i = target.src_size; i > 0; i -= 16)
	{
	    *dest++ = *source++;
	    *dest++ = *source++;
	    *dest++ = *source++;
	    *dest++ = *source++;
	}
    }

    /* Make sure all of image is copied to RAM */
    uHALr_printf("Disable cache...\n");
    uHALr_DisableCache();

    /* Just say goodbye */
    uHALr_printf("Image copied. Starting...\n");
    uHALr_EnableCache();

    target.start();

    /* It's never coming back, but the routine deserves a tidy finish */
    return 0;
}

// End of file - armload.c

