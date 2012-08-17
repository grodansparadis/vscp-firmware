/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : efs.h                                                            *
* Description : Headerfile for efs.c                                          *
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
*                                                    (c)2005 Lennart Yseboody *
*                                                    (c)2005 Michael De Nil   *
\*****************************************************************************/

#ifndef __EFS_H__
#define __EFS_H__

/*****************************************************************************/
#include "types.h"
#include "config.h"
#include "extract.h"
#include "sextract.h"
#include "interface.h"
#include "disc.h"
#include "partition.h"

/* WARNING !!!!
 * These includes DO NOT BELONG HERE,
 * remove them when the VFS layer is implemented !!!!
 */
#include "fs.h"
#include "file.h"
#include "time.h"
#include "ui.h"
/*****************************************************************************/

struct EFSL_Storage {
	Interface interface;
	IOManager ioman;
	Disc disc;
};
typedef struct EFSL_Storage EFSL_Storage;

struct EFSL_Storage_Config {
	
	void *hwObject;
	esint8 (*if_init_fptr)(void*);
	esint8 (*if_read_fptr)(void*,euint32,euint8*);
	esint8 (*if_write_fptr)(void*,euint32,euint8*);
	esint8 (*if_ioctl_fptr)(void*,euint16,void*);
	void *ioman_bufmem;
};
typedef	struct EFSL_Storage_Config EFSL_Storage_Config;

/*****************************************************************************/

struct EFSL_Filesystem {
	EFSL_Storage *storage;
	Partition partition;
	FileSystem filesystem;
};
typedef struct EFSL_Filesystem EFSL_Filesystem;

struct EFSL_Filesystem_Config {
	EFSL_Storage *storage;
	euint8 no_partitions;
};
typedef struct EFSL_Filesystem_Config EFSL_Filesystem_Config;

/*****************************************************************************/

struct EFSL_File {
	File file;
};
typedef struct EFSL_File EFSL_File;

/*****************************************************************************/

esint8 EFSL_Storage_Init(EFSL_Storage *efsl_storage,EFSL_Storage_Config *config);
esint8 EFSL_Filesystem_Init(EFSL_Filesystem *efsl_filesystem,EFSL_Filesystem_Config *config);

#endif

