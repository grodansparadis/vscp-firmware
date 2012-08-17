/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : interface.c                                                      *
* Description : This file defines the general I/O interface functions         *
*               that can be performed on hardware.                            *
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
******************************************************************************/

#include "interface.h"

esint8 if_init(Interface *iface, void* hwData,
                                              esint8 (*initInterface)(void*),
                                              esint8 (*readBuf) (void*, euint32,euint8*),
                                              esint8 (*writeBuf) (void*, euint32,euint8*),
					      esint8 (*ioctl) (void*,euint16,void*))
{
	esint8 r;
	
	iface->flags=0;
	iface->sectorCount=0;
	iface->interface_data=hwData;
#ifdef MULTIPLE_INTERFACE_SUPPORT
	iface->initInterface=initInterface;
	iface->readBuf=readBuf;
	iface->writeBuf=writeBuf;
	iface->ioctl=ioctl;
	r=iface->initInterface(iface->interface_data);
#else
	r=HWIFUNC_INIT(iface->interface_data);
#endif

	if(!r){ /* Init OK, try to get some info */
		if(iface->ioctl(iface->interface_data,IOCTL_SECTORCOUNT,(void*)&(iface->sectorCount))){
			iface->sectorCount=0; /* Device doesn't know */
		}
	}
	return(r);
}

esint8 if_readBuf(Interface *iface, euint32 address, euint8* buf)
{
#ifdef MULTIPLE_INTERFACE_SUPPORT
	return(iface->readBuf(iface->interface_data,address,buf));
#else
	return(HWIFUNC_READ(iface->interface_data,address,buf));
#endif
}


esint8 if_writeBuf(Interface *iface, euint32 address, euint8* buf)
{
#ifdef MULTIPLE_INTERFACE_SUPPORT
	return(iface->writeBuf(iface->interface_data,address,buf));
#else
	return(HWIFUNC_WRITE(iface->interface_data,address,buf));
#endif
}
