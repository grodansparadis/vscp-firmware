/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename :  linuxfile.h                                                     *
* Description : Headerfile for linuxfile.c                                    *
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
*                                                    (c)2005 Michael De Nil   *
\*****************************************************************************/

#ifndef __LINUXFILE_H__
#define __LINUXFILE_H__

/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "helper.h"
#include "debug.h"
#include "types.h"
#include "ioctl.h"
#include "config.h"
/*****************************************************************************/

#define _LARGEFILE_SOURCE 
#define _GNU_SOURCE 
#define _FILE_OFFSET_BITS 64

/*************************************************************\
              hwInterface
               ----------
* FILE* 	imagefile		File emulation of hw interface.
* long		sectorCount		Number of sectors on the file.
\*************************************************************/
struct linuxFileInterface{
	FILE 	*imageFile;
	euint8* fileName;
	eint32  sectorCount;
};
typedef struct linuxFileInterface linuxFileInterface;

esint8 lf_init(void* LFI);
esint8 lf_readBuf(void* LFI,euint32 address,euint8* buf);
esint8 lf_writeBuf(void* LFI,euint32 address,euint8* buf);
esint8 lf_ioctl(void* LFI,euint16 ctl,void* data);

esint8 lf_setPos(void* LFI,euint32 address);

#endif
