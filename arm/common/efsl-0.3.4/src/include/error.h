/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : error.h                                                          *
* Description : Header file containing error-defines.                         *
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

#ifndef __ERROR_H_
#define __ERROR_H_

/*****************************************************************************/
#include "config.h"
#include "interface.h"
/*****************************************************************************/

#if defined (FULL_ERROR_SUPPORT)
	#define ERRSUP_IOMAN
	#define ERRSUP_DISC
	#define ERRSUP_PARTITION
	#define ERRSUP_FAT_FILESYSTEM
	#define ERRSUP_FAT_FILESYSTEM
#elif defined (BASE_ERROR_SUPPORT)
	#define ERRSUP_IOMAN
	#define ERRSUP_DISC
	#define ERRSUP_PARTITION
#endif

#if defined (HWINT_HAS_ERROR_SUPPORT) && defined (INTERFACE_ERROR_SUPPORT)
	#define ERRSUP_HWINTERFACE
#endif

#ifdef ERRSUP_IOMAN
	#define IOMAN_ERR_EUINT8 euint8 error;
	#define ioman_setError(ioman,errval) ioman->error = errval
	#define ioman_getError(ioman)        ioman->error
#else
	#define IOMAN_ERR_EUINT8
	#define ioman_setError(ioman,errval)
	#define ioman_getError(ioman) 0
#endif

#ifdef ERRSUP_DISC
	#define DISC_ERR_EUINT8 euint8 error;
	#define disc_setError(disc,errval) disc->error = errval
	#define disc_getError(disc)        disc->error
#else
	#define DISC_ERR_EUINT8
	#define disc_setError(disc,errval)
	#define disc_getError(disc) 0
#endif

#ifdef ERRSUP_PART
	#define PART_ERR_EUINT8 euint8 error;
	#define part_setError(part,errval) part->error = errval
	#define part_getError(part)        part->error
#else
	#define PART_ERR_EUINT8
	#define part_setError(part,errval)
	#define part_getError(part) 0
#endif

#ifdef ERRSUP_FAT_FILESYSTEM
	#define FILESYSTEM_ERR_EUINT8 euint8 error;
	#define fs_setError(fs,errval) fs->error = errval
	#define fs_getError(fs)        fs->error
#else
	#define FILESYSTEM_ERR_EUINT8
	#define fs_setError(fs,errval)
	#define fs_getError(fs) 0
#endif

#ifdef ERRSUP_FILE
	#define FILE_ERR_EUINT8 euint8 error;
	#define file_setError(file,errval) file->error = errval
	#define file_getError(file)        file->error
#else
	#define FILE_ERR_EUINT8
	#define file_setError(file,errval)
	#define file_getError(file) 0
#endif

#define IOMAN_NOERROR                     0
#define IOMAN_ERR_SETATTROUTOFBOUNDS      1
#define IOMAN_ERR_GETATTROUTOFBOUNDS      2
#define IOMAN_ERR_READFAIL                3
#define IOMAN_ERR_WRITEFAIL               4
#define IOMAN_ERR_OPOUTOFBOUNDS           5
#define IOMAN_ERR_PUSHBEYONDSTACK         6
#define IOMAN_ERR_POPEMPTYSTACK           7
#define IOMAN_ERR_CACHEPTROUTOFRANGE      8
#define IOMAN_ERR_WRITEREADONLYSECTOR     9
#define IOMAN_ERR_NOMEMORY                10

#define DISC_NOERROR                      0

#define PART_NOERROR                      0


#endif
