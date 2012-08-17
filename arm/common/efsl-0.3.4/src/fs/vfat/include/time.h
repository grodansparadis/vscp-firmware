/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : types.h                                                          *
* Description : Headerfile for types.c                                        *
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
*                                                    (c)2005 Joel Winarske    *
******************************************************************************/

#ifndef __TIME_H_
#define __TIME_H_

/*****************************************************************************/
#include "types.h"
/*****************************************************************************/

#ifdef DATE_TIME_SUPPORT
        euint16 fs_makeDate(void);
        euint16 fs_makeTime(void);

        extern void efsl_getTime(euint8 *hour, euint8 *min, euint8 *sec);
        extern void efsl_getDate(euint8 *date, euint8 *month, euint16 *year);

	#define time_getDate(void) fs_makeDate()
	#define time_getTime(void) fs_makeTime()
#else
        #define time_getYear(void) 0x0;
	#define time_getMonth(void) 0x0;
	#define time_getDay(void) 0x0;
	#define time_getHour(void) 0x0;
	#define time_getMinute(void) 0x0;
	#define time_getSecond(void) 0x0;
#endif

euint8 fs_hasTimeSupport(void);

#endif
