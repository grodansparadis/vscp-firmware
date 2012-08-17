/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : plibc.c                                                          *
* Description : This file contains replacements of common c library functions *
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
#include "plibc.h"
/*****************************************************************************/

/* ****************************************************************************
 * unsigned short strMatch(char* bufa, char*bufb, unsigned long n)
 * Description: Compares bufa and bufb for a length of n bytes.
 * Return value: Returns the number of character NOT matching.
*/
euint16 strMatch(eint8* bufa, eint8*bufb,euint32 n)
{
	euint32 c;
	euint16 res=0;
	for(c=0;c<n;c++)if(bufa[c]!=bufb[c])res++;
	return(res);
}
/*****************************************************************************/


/* ****************************************************************************
 * void memCpy(void* psrc, void* pdest, unsigned long size)
 * Description: Copies the contents of psrc into pdest on a byte per byte basis.
 * The total number of bytes copies is size.
*/
void memCpy(void* psrc, void* pdest, euint32 size)
{
	while(size>0){
		*((eint8*)pdest+size-1)=*((eint8*)psrc+size-1);
		size--;
	}
}
/*****************************************************************************/

void memClr(void *pdest,euint32 size)
{
	while(size>0){
		*(((eint8*)pdest)+size-1)=0x00;
		size--;
	}
}

void memSet(void *pdest,euint32 size,euint8 data)
{
	while(size>0){
		*(((eint8*)pdest)+size-1)=data;
		size--;
	}
}


