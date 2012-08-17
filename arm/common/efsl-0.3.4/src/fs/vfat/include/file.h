/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : file.h                                                           *
* Description : Headerfile for file.c                                         *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) any later version.          *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
*                                                    (c)2004 Lennart Yseboodt *
*                                                    (c)2005 Michael De Nil   *
*                                                    (c)2005 Joel Winarske    *
******************************************************************************/

#ifndef __FILE_H_
#define __FILE_H_

/*****************************************************************************/
#include "config.h"
#include "error.h"
#include "time.h"
#include "fs.h"
#include "dir.h"
#include "plibc.h"
#include "debug.h"
#include "types.h"
#include "fat.h"
/*****************************************************************************/

#define MODE_READ 0x72    /* 'r' */
#define MODE_WRITE 0x77   /* 'w' */
#define MODE_APPEND 0x61  /* 'a' */

#define FILE_STATUS_OPEN 0
#define FILE_STATUS_WRITE 1

/******************************************************************************
 *                      File
 *                      ------
 * FileRecord		DirEntry		Copy of the FileRecord for this file
 * FileLocation	        Location		Location of the direntry
 * FileSystem		*fs			Pointer to the filesystem this file is on
 * FileCache		Cache			Pointer to the cache object of the file
 * euint8		FileStatus		Contains bitfield regarding filestatus
 * euint32		FilePtr			Offsetpointer for fread/fwrite functions
 * euint32 		FileSize		Working copy of the filesize, always use this,
 						it is more up to date than DirEntry->FileSize,
						which is only updated when flushing to disc.
******************************************************************************/
struct File{
	FileRecord DirEntry;
	FileLocation Location; /* Location in directory!! */
	FileSystem *fs;
	ClusterChain Cache;
	euint8	FileStatus;
	euint32 FilePtr;
	euint32 FileSize;
};
typedef struct File File;


esint8 file_fopen(File *file, FileSystem *fs,eint8 *filename, eint8 mode);
esint8 file_fclose(File *file);
esint16 file_setpos(File *file,euint32 pos);
euint32 file_fread(File *file,euint32 offset, euint32 size,euint8 *buf);
euint32 file_read (File *file,euint32 size,euint8 *buf);
euint32 file_fwrite(File* file,euint32 offset,euint32 size,euint8* buf);
euint32 file_write (File* file,euint32 size,euint8* buf);
eint8* file_normalToFatName(eint8* filename,eint8* fatfilename);
euint8 file_validateChar(euint8 c);
void file_initFile(File *file, FileSystem *fs, FileLocation *loc);
eint16 file_allocClusterChain(File *file,euint32 num_clusters);
void file_setAttr(File* file,euint8 attribute,euint8 val);
euint8 file_getAttr(File* file,euint8 attribute);
euint32 file_requiredCluster(File *file,euint32 offset, euint32 size);

euint8 file_feof(File* file);
esint16 file_getc(File* file);
esint16 file_putc(File* file, euint8* byte);

#endif
