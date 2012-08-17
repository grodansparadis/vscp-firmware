/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename :  sd.h                                                            *
* Revision :  Initial developement                                            *
* Description : Headerfile for sd.c                                           *
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

#ifndef __SD_H__
#define __SD_H__
/*****************************************************************************/

#include "config.h"
#include "types.h"
#include "debug.h"
/*****************************************************************************/

#define CMDREAD     17
#define CMDWRITE    24
/*****************************************************************************/

struct SdSpiProtocol
{
	void *spiHwInterface;
	euint8 (*spiHwInit)(void* spiHwInterface);
	euint8 (*spiSendByte)(void* spiHwInterface,euint8 data);
};
typedef struct SdSpiProtocol SdSpiProtocol;


esint8  sd_Init(SdSpiProtocol *ssp);
void sd_Command(SdSpiProtocol *ssp,euint8 cmd, euint16 paramx, euint16 paramy);
euint8 sd_Resp8b(SdSpiProtocol *ssp);
void sd_Resp8bError(SdSpiProtocol *ssp,euint8 value);
euint16 sd_Resp16b(SdSpiProtocol *ssp);
esint8 sd_State(SdSpiProtocol *ssp);

esint8 sd_readSector(SdSpiProtocol *ssp,euint32 address,euint8* buf);
esint8 sd_writeSector(SdSpiProtocol *ssp,euint32 address, euint8* buf);
esint8 sd_ioctl(SdSpiProtocol* ssp,euint16 ctl,void* data);

#endif
