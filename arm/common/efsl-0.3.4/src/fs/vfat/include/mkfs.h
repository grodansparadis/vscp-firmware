/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename :  mkfs.h                                                          *
* Description : Headerfile for mkfs.c                                         *
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
*                                                    (c)2005 Lennart Yseboodt *
******************************************************************************/

#ifndef __MKFS_H_
#define __MKFS_H_

/*****************************************************************************/
#include "partition.h"
#include "plibc.h"
#include "debug.h"
#include "types.h"
#include "extract.h"
#include "config.h"
/*****************************************************************************/

#define MKFS_ERR_TOOLITTLESECTORS 1

esint16 mkfs_makevfat(Partition *part);

#endif
