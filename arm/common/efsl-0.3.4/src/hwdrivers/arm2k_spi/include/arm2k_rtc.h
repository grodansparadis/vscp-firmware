/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  arm2k_rtc.h                                                     *
* Description : Headerfile for arm2k_rtc.c                                    *
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
*                                                                             *
*                                                    (c)2005 Joel Winarske    *
\*****************************************************************************/

#ifndef __ARM2K_RTC_H__
#define __ARM2K_RTC_H__

#include "debug.h"
#include "config.h"
#include "LPC214X.h"
#include "arm2k_macros.h"

#define CCR_BIT_CLKEN   0
#define CCR_BIT_CTCRST  1
#define CCR_BIT_CTTEST  2
#define CCR_BIT_CLKSRC  4

#define CIIR_BIT_IMSEC  0

#define DEF_HOUR        12
#define DEF_MIN         0
#define DEF_SEC         0
#define DEF_DATE        21
#define DEF_MONTH       11
#define DEF_YEAR        2005

#define RTC_CountPerSec 32768

#define RTC_YEARMIN     1901
#define RTC_YEARMAX     2099

#define RTC_BASEYEAR    1901
#define RTC_BASEMONTH   1
#define RTC_BASEDAY     1
#define RTC_BASEDOW     2

euint8 RTC_Init(void);
euint8 RTC_SetTime(euint8 hour, euint8 min, euint8 sec);
euint8 RTC_SetDate(euint8 date, euint8 month, euint16 year);
euint8 RTC_GetTimeDate(euint8 *hour, euint8 *min, euint8 *sec, euint8 *date, euint8 *month, euint16 *year);

void efsl_getTime(euint8 *hour, euint8 *min, euint8 *sec);
void efsl_getDate(euint8 *date, euint8 *month, euint16 *year);

#endif
