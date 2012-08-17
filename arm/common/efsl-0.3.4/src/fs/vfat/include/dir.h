/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : dir.h                                                            *
* Description : Headerfile for dir.c The files are an extend of the fs.c fs.h *
*               pair.                                                         *
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
*                                                    (c)2004 Michael De Nil   *
******************************************************************************/

#ifndef __DDIR_H__
#define __DDIR_H__

/*****************************************************************************/
#include "config.h"
#include "error.h"
#include "fat.h"
#include "plibc.h"
#include "types.h"
#include "ioman.h"
#include "time.h"
#include "fs.h"
/*****************************************************************************/

#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

#define OFFSET_DE_FILENAME            0
#define OFFSET_DE_ATTRIBUTE          11
#define OFFSET_DE_NTRESERVED         12
#define OFFSET_DE_CRTIMETNT          13
#define OFFSET_DE_CREATETIME         14
#define OFFSET_DE_CREATEDATE         16
#define OFFSET_DE_LASTACCESSDATE     18
#define OFFSET_DE_FIRSTCLUSTERHIGH   20
#define OFFSET_DE_WRITETIME          22
#define OFFSET_DE_WRITEDATE          24
#define OFFSET_DE_FIRSTCLUSTERLOW    26
#define OFFSET_DE_FILESIZE           28

#define DIRFIND_FILE 0
#define DIRFIND_FREE 1

void dir_getFileStructure(FileSystem *fs,FileRecord *filerec,FileLocation *loc);
void dir_createDirectoryEntry(FileSystem *fs,FileRecord *filerec,FileLocation *loc);
void dir_createDefaultEntry(FileSystem *fs,FileRecord *filerec,eint8* fatfilename);
void dir_setFirstCluster(FileSystem *fs,FileLocation *loc,euint32 cluster_addr);
void dir_setFileSize(FileSystem *fs,FileLocation *loc,euint32 numbytes);
euint32 dir_findinRoot(FileSystem *fs,eint8 * fatname, FileLocation *loc);
euint32 dir_findinDir(FileSystem *fs, eint8 * fatname, euint32 startCluster, FileLocation *loc, euint8 mode);
euint32 dir_findinBuf(euint8 *buf,eint8 *fatname, FileLocation *loc, euint8 mode);
euint32 dir_findinCluster(FileSystem *fs,euint32 cluster,eint8 *fatname, FileLocation *loc, euint8 mode);
euint32 dir_findinRootArea(FileSystem *fs,eint8* fatname, FileLocation *loc, euint8 mode);
esint8 dir_getFatFileName(eint8* filename, eint8* fatfilename);
esint8 dir_updateDirectoryEntry(FileSystem *fs,FileRecord *entry,FileLocation *loc);
esint8 dir_addCluster(FileSystem *fs,euint32 firstCluster);
#endif
