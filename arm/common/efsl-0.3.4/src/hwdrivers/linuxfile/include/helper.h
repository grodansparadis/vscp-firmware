/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : helper.h                                                         *
* Description : Headerfile for helper.c                                       *
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
\*****************************************************************************/

#ifndef __HELPER_H__
#define __HELPER_H__

/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "types.h"
#include "config.h"
/*****************************************************************************/

void* Malloc(eint32 size);
void Fopen(FILE **f,eint8* filename);
void MMap(eint8* filename,void**x,eint32* size);
int getFileSize(FILE* file);

#endif
