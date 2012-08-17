/* unzip.c -- decompress a memory buffer formatted by zip or gzip.
 *
 * Copyright ARM Limited 1999 - 2000.  All rights reserved.
 */

/* @(#) $Id: unzip.c,v 1.1 2005/12/19 11:46:08 akhe Exp $ */

#include "zlib.h"

#define	GZIP_MAGIC	0x1f8b
#define	PKZIP_MAGIC	0x504b
#define BLOCKSIZE 	512

/* Bit in the GZIP flags byte (offset 3) indicating filename specified. */
#define Z_FILENAME 8

/* ===========================================================================
 * validZipID - validate that the file looks like a zip file.
 */
int validZipID(const Bytef *zbuffer)
{
    int id;
    
    id = *zbuffer << 8;
    id += *(zbuffer + 1);
    
    if ((GZIP_MAGIC == id) || (PKZIP_MAGIC == id))
    	return Z_OK;

    return Z_ERRNO;
}


/* ===========================================================================
 * unzip
 *
 * This routine is called exactly like uncompress, but is much less rigorous
 * in checking the header (zip & gzip don't work quite the same).
 * Decompresses the source buffer into the destination buffer.  zLen is
 * the byte length of the source buffer. Upon entry, dSize is the total
 * size of the destination buffer, which must be large enough to hold the
 * entire uncompressed data. (The size of the uncompressed data must have
 * been saved previously by the compressor and transmitted to the decompressor
 * by some mechanism outside the scope of this compression library.)
 * Upon exit, destLen is the actual size of the compressed buffer.
 * This function can be used to decompress a whole file at once if the
 * input file is mmap'ed.
 * 
 * unzip returns Z_OK if success, Z_MEM_ERROR if there was not
 * enough memory, Z_BUF_ERROR if there was not enough room in the output
 * buffer, or Z_DATA_ERROR if the input data was corrupted.
 */

int unzip(Bytef *bdest, uLong *dSize, const Bytef *zbuffer, uLongf zlen)
{
    z_stream d_stream;
    Bytef *pData;
    int err;
    int flags;
    int id;
    uLong start = (uLong)bdest;

    // Initial file header block sizes
    int headerSize   = 10;
    int filenameSize =  0;
    int extraSize    =  0;
    
    id = *zbuffer << 8;
    id += *(zbuffer + 1);

    if (GZIP_MAGIC == id)
    {
	flags = zbuffer[3];
	if ((flags & Z_FILENAME) == Z_FILENAME)
	{
	    // Step over the filename
	    pData = (Bytef *)zbuffer;
	    pData += headerSize + filenameSize;
	    while (*pData++ != 0)
		filenameSize++;
	    
	    // skip the terminating 0
	    filenameSize++;
	}
	/* Read the compression format into flags */
	flags = zbuffer[2];
    }
    else if (PKZIP_MAGIC == id)
    {
	flags = zbuffer[8];
	headerSize   = 0x1e;
	filenameSize = (zbuffer[0x1b] << 8) + zbuffer[0x1a];
	extraSize    = (zbuffer[0x1d] << 8) + zbuffer[0x1c];

    }

    if (flags != Z_DEFLATED)
    {
    	return Z_DATA_ERROR;
    }

    // Step over the header stuff
    zbuffer += headerSize + filenameSize + extraSize;
    zlen    -= headerSize + filenameSize + extraSize;

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in = (Bytef *)zbuffer;
    d_stream.avail_in = zlen;

    err = inflateInit2(&d_stream,-MAX_WBITS);

    if (err != Z_OK)
    	return err;
    do
    {
	d_stream.next_out = bdest;
	d_stream.avail_out = (uInt)BLOCKSIZE;
	err = inflate(&d_stream, Z_NO_FLUSH);

	bdest += BLOCKSIZE;
    }
    while (err == Z_OK);

    inflateEnd(&d_stream);

    // If we have decoded everything, that's OK    
    if (Z_STREAM_END == err)
	err = Z_OK;

    *dSize = (uLong)bdest - start;
    return err;
}


// End of file - unzip.c

