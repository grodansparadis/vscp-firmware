//
//  $Id: rtc.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/rtc/rtc.h $
//

#ifndef _RTC_H_
#define _RTC_H_

#include <time.h>

//
//  Define this for using non-vectored IRQs.  Undef it if a regular vectored
//  IRQ is preferred.
//
#define RTC_NONVECTOREDIRQ

//
//
//
void rtcInit (void);
time_t rtcGetEpochSeconds (unsigned int *milliseconds);
void rtcSetEpochSeconds (time_t now);
int rtcSetAlarm (struct tm *tm);
struct tm *rtcGetAlarmTm (struct tm *tm);
time_t rtcGetAlarmEpochSeconds (void);
int rtcPeriodicAlarm (int mode);

#endif
