/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  sextract.h                                                      *
* Description : Headerfile for sextract.c                                     *
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

#ifndef __SEXTRACT_H_
#define __SEXTRACT_H_

#include "config.h"
#include "types.h"
#include "extract.h"
#include "disc.h"
#include "partition.h"

void ex_setPartitionField(euint8* buf,PartitionField* pf, euint32 offset);
void ex_getPartitionField(euint8* buf,PartitionField* pf, euint32 offset);

#endif



