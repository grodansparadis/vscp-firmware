/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename : plibc.h                                                          *
* Description : Headerfile for plibc.c                                        *
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

#ifndef __PLIBC_H__
#define __PLIBC_H__

/*****************************************************************************/
#include "debug.h"
#include "types.h"
#include "config.h"
/*****************************************************************************/

euint16 strMatch(eint8* bufa, eint8*bufb,euint32 n);
void memCpy(void* psrc, void* pdest, euint32 size);
void memClr(void *pdest,euint32 size);
void memSet(void *pdest,euint32 size,euint8 data);


#endif
