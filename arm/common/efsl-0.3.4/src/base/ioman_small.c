/*****************************************************************************\
*                     libfat - General purpose FAT library                    *
*                      ----------------------------------                     *
*                                                                             *
* Filename : ioman.c                                                          *
* Description : The IO Manager receives all requests for sectors in a central *
*               allowing it to make smart decision regarding caching.         *
*               The IOMAN_NUMBUFFER parameter determines how many sectors     *
*               ioman can cache. ioman also supports overallocating and       *
*               backtracking sectors.                                         *
*               This is the small version of IOMan, for systems with limited  *
*               resources. It features only one fixed buffer, and has         *
*               simplified operation                                          *
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
*                                                    (c)2005 Lennart Yseboodt *
*                                                    (c)2004 Michael De Nil   *
\*****************************************************************************/

#include "ioman_small.h"

esint8 ioman_init(IOManager *ioman, Interface *iface, euint8* bufferarea)
{
	ioman->iface=iface;
	ioman_reset(ioman);
	return(0);
}
/*****************************************************************************/

void ioman_reset(IOManager *ioman)
{
	ioman->sector=ioman->status=ioman->itptr=0;
	ioman->stack.sector=ioman->stack.status=0;
		
	ioman_setError(ioman,IOMAN_NOERROR);
		
}
/*****************************************************************************/

void ioman_setAttr(IOManager *ioman,euint8 attribute,euint8 val)
{
	if(val){
		ioman->status|=1<<attribute;
	}else{
		ioman->status&=~(1<<attribute);
	}
}
/*****************************************************************************/

euint8 ioman_getAttr(IOManager *ioman,euint8 attribute)
{
	return(ioman->status&(1<<attribute));
}
/*****************************************************************************/


euint8* ioman_getSector(IOManager *ioman,euint32 address, euint8 mode)
{
	if(address==ioman->sector){
		if(mode==IOM_MODE_READWRITE)ioman_setWritable();
		return(ioman->bufptr);
	}
}

esint8 ioman_readSector(IOManager *ioman,euint32 address,euint8* buf)
{
	esint8 r;

	if(buf==0){
		return(-1);
	}
	
	r=if_readBuf(ioman->iface,address,buf);
	
	if(r!=0){
		ioman_setError(ioman,IOMAN_ERR_READFAIL);
		return(-1);
	}
	return(0);
}
/*****************************************************************************/

esint8 ioman_writeSector(IOManager *ioman, euint32 address, euint8* buf)
{
	esint8 r;

	if(buf==0)return(-1);
	
	r=if_writeBuf(ioman->iface,address,buf);

	if(r<=0){
		ioman_setError(ioman,IOMAN_ERR_WRITEFAIL);
		return(-1);
	}
	return(0);
}
/*****************************************************************************/
