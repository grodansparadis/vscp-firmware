/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  extract.h                                                       *
* Description : Headerfile for extract.c                                      *
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

#include "sextract.h"

void ex_setPartitionField(euint8* buf,PartitionField* pf, euint32 offset)
{

}
/*****************************************************************************/

void ex_getPartitionField(euint8* buf,PartitionField* pf, euint32 offset)
{
	pf->bootFlag       = *(buf + offset);
	pf->CHS_begin[0]   = *(buf + offset + 1);
	pf->CHS_begin[1]   = *(buf + offset + 2);
	pf->CHS_begin[2]   = *(buf + offset + 3);
	pf->type           = *(buf + offset + 4);
	pf->CHS_end[0]     = *(buf + offset + 5);
	pf->CHS_end[1]     = *(buf + offset + 6);
	pf->CHS_end[2]     = *(buf + offset + 7);
	pf->LBA_begin      = ex_getb32(buf + offset + 8);
	pf->numSectors     = ex_getb32(buf + offset + 12);
}
/*****************************************************************************/



