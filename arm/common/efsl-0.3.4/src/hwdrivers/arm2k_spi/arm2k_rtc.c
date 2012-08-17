/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : arm2k_rtc.c                                                      *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to a Phillips         *
*               LPC2000 series SSP port                                       *
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
*                                                    (c)2003 IAR Systems      *
*                                                    (c)2005 Joel Winarske    *
\*****************************************************************************/

/*****************************************************************************/
#include "arm2k_rtc.h"


static int RTC_MonthVal[]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*****************************************************************************/

static eint8 IsLeapYear (euint16 year)
{
    if (!(year%4) && (year%100) || !(year%400))
      return 1;
    return 0;
}

/*****************************************************************************/

static euint16 GetDOY (euint16 year, euint8 month, euint8 day)
{
    euint16 i, doy=0;

    for(i=1; i<month; i++)
      doy+=RTC_MonthVal[i];

    if (month>2)
      if (IsLeapYear(year))
        doy++;

    return (doy+day);	
}

/*****************************************************************************/

static euint16 GetDOW (euint16 year, euint8 month, euint8 day)
{
    euint16 i, dow=0;

    for (i = RTC_BASEYEAR, dow = 0; i < year; i++)
    {
      dow +=365;
      if  (IsLeapYear(i))
        dow++;
    }

    dow +=  GetDOY (year, month, day) - 1;
    dow = (dow + RTC_BASEDOW) % 7;

    return dow;	
}

/*****************************************************************************/

static esint8 IsValidDay (euint16 year, euint8 month, euint8 day)
{
    if (year < RTC_YEARMIN || year > RTC_YEARMAX ||day <1)
      return 0;

    switch(month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
          if (day>31)
            return 0;
          else
            break;
        case 4:
        case 6:
        case 9:
        case 11:
          if (day>30)
            return 0;
          else
            break;
        case 2:
          if (IsLeapYear(year))
            if (day>29)
              return 0;
            else
              break;
          else
            if (day>28)
              return 0;
            else
              break;
        default:
          return 0;
    }

    return 1;
}

/*****************************************************************************/

euint8 RTC_SetTime(euint8 hour, euint8 min, euint8 sec)
{
    if ( hour > 23 || min > 59 || sec > 59)
      return 1;

    HOUR = hour;
    MIN = min;
    SEC = sec;
    return(0);
}

/*****************************************************************************/

euint8 RTC_SetDate(euint8 date, euint8 month, euint16 year)
{
    if (IsValidDay(year, month, date) == 0)
      return 1;

    DOM = date;
    MONTH = month;
    YEAR = year;

    DOY = GetDOY(year, month, date);
    DOW = GetDOW(year, month, date);

    return(0);
}

/*****************************************************************************/

void efsl_getTime(euint8 *hour, euint8 *min, euint8 *sec)
{
    *sec = SEC;
    *min = MIN;
    *hour = HOUR;
}

/*****************************************************************************/

void efsl_getDate(euint8 *date, euint8 *month, euint16 *year)
{
    *date = DOM;
    *month = MONTH;
    *year = YEAR;
}

/*****************************************************************************/

euint8 RTC_GetTimeDate(euint8 *hour, euint8 *min, euint8 *sec, euint8 *date, euint8 *month, euint16 *year)
{
    *sec = SEC;
    *min = MIN;
    *hour = HOUR;
    *date = DOM;
    *month = MONTH;
    *year = YEAR;

    return (0);
}

/*****************************************************************************/

euint8 RTC_Init(void)
{
    CLR_BIT(CCR,CCR_BIT_CLKEN);   /* rtc disable   */
    SET_BIT(CCR,CCR_BIT_CLKSRC);  /* 32kHz         */
    CLR_BIT(CCR,CCR_BIT_CTCRST);  /* disable reset */
    CLR_BIT(CCR,CCR_BIT_CTTEST);  /* disable test  */
    AMR = 0;                      /* initialize interrupt mask register of RTC */

    ILR=0x3;                      /* clear all interrupt of RTC */

    if(RTC_SetDate(DEF_DATE,DEF_MONTH,DEF_YEAR))  /* initialize date */
        return (1);
    if(RTC_SetTime(DEF_HOUR,DEF_MIN,DEF_SEC))     /* initialize time */
        return (1);

    SET_BIT(CCR,CCR_BIT_CLKEN);   /* rtc enable */
    return (0);
}

/*****************************************************************************/
