/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename : ui.h                                                             *
* Description : Headerfile for ui.c                                           *
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
*                                                    (c)2005 Michael De Nil   *
******************************************************************************/

#ifndef __UI_H__
#define __UI_H__

/*****************************************************************************/
#include "fs.h"
#include "types.h"
#include "fat.h"
#include "dir.h"
#include "config.h"
/*****************************************************************************/

short listFiles(FileSystem *fs, char *dir);
esint16 rmfile(FileSystem *fs,euint8* filename);
esint8 mkdir(FileSystem *fs,eint8* dirname);

#endif
