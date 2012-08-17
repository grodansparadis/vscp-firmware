/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  time.c                                                          *
* Description : This file contains functions for time support                  *
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
*                                                    (c)2005 Joel Winarske    *
******************************************************************************/

/*****************************************************************************/
#include "time.h"
/*****************************************************************************/

euint16 fs_makeDate(void)
{
#ifndef DATE_TIME_SUPPORT
	return(0);
#else
	euint8 m,d;
	euint16 y;
	
        efsl_getDate(&m,&d,&y);
        y -= 1980;

	return(
		(y>127?127<<9:(y&0x3F)<<9)   |
		((m==0||m>12)?1:(m&0xF)<<5)  |
		((d==0||d>31)?1:(d&0x1F))
	);
#endif
}
/*****************************************************************************/

euint16 fs_makeTime(void)
{
#ifndef DATE_TIME_SUPPORT
	return(0);
#else
	euint8 s,m,h;
	
        efsl_getTime(&s,&m,&h);
	
	return(
		(h>23?0:(h&0x1F)<<11) |
		(m>59?0:(m&0x3F)<<5)  |
		(s>59?0:(s-s%2)/2)
	);
#endif
}
/*****************************************************************************/

euint8 fs_hasTimeSupport(void)
{
#ifdef DATE_TIME_SUPPORT
	return(1);
#else
	return(0);
#endif
}
/*****************************************************************************/


