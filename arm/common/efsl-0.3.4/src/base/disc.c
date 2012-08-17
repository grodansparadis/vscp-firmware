/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : disc.c                                                           *
* Description : This file contains the functions regarding the whole disc     *
*               such as loading the MBR and performing read/write tests.      *
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

/*****************************************************************************/
#include "disc.h"
/*****************************************************************************/

/* ****************************************************************************
 * void disc_initDisc(Disc *disc,hcInterface* source)
 * Description: This initialises the disc by loading the MBR and setting the
 * pointer to the hardware object.
*/
void disc_initDisc(Disc *disc,IOManager* ioman)
{
	disc->ioman=ioman;
	disc_setError(disc,DISC_NOERROR);
	disc_loadMBR(disc);
}
/*****************************************************************************/

/* ****************************************************************************
 * void disc_loadMBR(Disc *disc)
 * Description: This functions copies the partitiontable to the partitions field.
*/
void disc_loadMBR(Disc *disc)
{
	euint8 x;
	euint8 *buf;
	
	buf=ioman_getSector(disc->ioman,LBA_ADDR_MBR,IOM_MODE_READONLY|IOM_MODE_EXP_REQ);
	for(x=0;x<4;x++){
		ex_getPartitionField(buf,&(disc->partitions[x]),PARTITION_TABLE_OFFSET+(x*SIZE_PARTITION_FIELD));
	}
	ioman_releaseSector(disc->ioman,buf);
}
/*****************************************************************************/


