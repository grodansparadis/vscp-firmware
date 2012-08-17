/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : ls.h                                                             *
* Description : Headerfile for ls.c                                           *
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

#ifndef __LS_H__
#define __LS_H__

/*****************************************************************************/
#include "config.h"
#include "fs.h"
#include "dir.h"
#include "fat.h"
/*****************************************************************************/

struct ListDirEntry{
	euint8 FileName[LIST_MAXLENFILENAME];
	euint32 FileSize;
	euint8 Attribute;
};
typedef struct ListDirEntry ListDirEntry;

struct DirList{
	FileSystem *fs;
	euint16 cEntry,rEntry;
	/*FileRecord currentEntry;*/
	ListDirEntry currentEntry;
	ClusterChain Cache;
};
typedef struct DirList DirList;

esint8 ls_openDir(DirList *dlist,FileSystem *fs,eint8* dirname);
esint8 ls_getNext(DirList *dlist);

esint8 ls_getDirEntry(DirList *dlist);
esint8 ls_getRealDirEntry(DirList *dlist);
esint8 ls_getRootAreaEntry(DirList *dlist);
esint8 ls_isValidFileEntry(ListDirEntry *entry);
void   ls_fileEntryToDirListEntry(DirList *dlist, euint8* buf, euint16 offset);

#endif
