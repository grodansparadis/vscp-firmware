/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  dsp67xx.h                                                       *
* Revision :  Initial developement                                            *
* Description : Headerfile for dsp67xx.h                                      *
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
\*****************************************************************************/

#ifndef __DSP67XX_H_ 
#define __DSP67XX_H_ 

#include "config.h"
#include "mcbsp/mcbsp.h"

/*************************************************************\
              hwInterface
               ----------
* FILE* 	imagefile		File emulation of hw interface.
* long		sectorCount		Number of sectors on the file.
\*************************************************************/
struct hwInterface{
	/*FILE 	*imageFile;*/
	long  	sectorCount;
	BspPort *port;
};
typedef struct hwInterface hwInterface;

short if_initInterface(hwInterface* file,char* opts);
short if_readBuf(hwInterface* file,euint32 address,unsigned char* buf);
short if_writeBuf(hwInterface* file,unsigned long address,unsigned char* buf);
short if_setPos(hwInterface* file,unsigned long address);

signed char if_spiInit(hwInterface *iface);
unsigned char if_spiSend(hwInterface *iface,euint8 outgoing);

#endif
