/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : interface.h                                                      *
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

#include "config.h"
#include "types.h"
#include "ioctl.h"

#ifndef MULTIPLE_INTERFACE_SUPPORT
#include HWIFUNC_HEADER
#endif

#ifndef __INTERFACE_H_
#define __INTERFACE_H_

/* If no multiple interfaces are defined, check if the
   hw functions are defines */
#ifndef MULTIPLE_INTERFACE_SUPPORT

 #ifndef HWIFUNC_INIT
  #error "There is no HW_INIT function defined"
 #endif

 #ifndef HWIFUNC_READ
  #error "There is no HW_READ function defined"
 #endif

 #ifndef HWIFUNC_WRITE
  #error "There is no HW_WRITE function defined"
 #endif

#endif

struct Interface{
#ifdef MULTIPLE_INTERFACE_SUPPORT
	esint8 (*initInterface)(void*);
	esint8 (*readBuf)(void*,euint32,euint8*);
	esint8 (*writeBuf)(void*,euint32,euint8*);
	esint8 (*ioctl)(void*,euint16,void*);
#endif
	void* interface_data;
	euint32 sectorCount;
	euint8 flags;
};
typedef struct Interface Interface;

esint8 if_init(Interface *iface, void* hwData,
                                              esint8 (*initInterface)(void*),
                                              esint8 (*readBuf) (void*, euint32,euint8*),
                                              esint8 (*writeBuf) (void*, euint32,euint8*),
					      				      esint8 (*ioctl) (void*,euint16, void*));
esint8 if_readBuf(Interface *iface, euint32 address, euint8* buf);
esint8 if_writeBuf(Interface *iface, euint32 address, euint8* buf);

#endif
