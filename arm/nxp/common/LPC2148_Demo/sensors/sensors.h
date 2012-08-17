//
//  $Id: sensors.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/sensors/sensors.h $
//

#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "semphr.h"

//
//
//
typedef struct sensorData_s
{
  int  sensorCount;
  int  adcChanges;
  int  dacValue;
  int  dacDirection;
}
sensorData_t;

//
//
//
int sensorsCopyData (sensorData_t *dst);
portTASK_FUNCTION_PROTO (vSensorsTask, pvParameters __attribute__ ((unused)));

#endif
