//
//  $Id: gps.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/gps/gps.h $
//

#ifndef _GPS_H_
#define _GPS_H_

#include "semphr.h"

//
//
//
typedef struct gpsData_s
{
  unsigned char valid;
  double latitude;
  double longitude;
  double height;
  float groundSpeed;
  float trueCourse;
  unsigned char utcDay;
  unsigned char utcMonth;
  unsigned short utcYear;
  unsigned char utcHours;
  unsigned char utcMinutes;
  unsigned char utcSeconds;
  int restarts;
}
gpsData_t;

//
//
//
int gpsCopyData (gpsData_t *dst);
portTASK_FUNCTION_PROTO (vGPSTask, pvParameters __attribute__ ((unused)));

#endif
