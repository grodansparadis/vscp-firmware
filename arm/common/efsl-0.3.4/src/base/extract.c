/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  extract.c                                                       *
* Description : This file contains functions to copy structures that get      *
*               corrupted when using direct memory copy                       *
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

/*****************************************************************************/
#include "extract.h"
/*****************************************************************************/

#if !(defined(BYTE_ALIGNMENT))
#warning "Compiling f_setbxx"
void ex_setb16(euint8* buf,euint16 data)
{
	buf[1] = data>>8;
	buf[0] = data>>0;
}

void ex_setb32(euint8* buf,euint32 data)
{
	buf[3] = data>>24;
	buf[2] = data>>16;
	buf[1] = data>>8;
	buf[0] = data>>0;
}

#endif


