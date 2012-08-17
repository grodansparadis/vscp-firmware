//
//  $Id: main.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/main.h $
//

#ifndef _MAIN_H_
#define _MAIN_H_

//
//
//
#define __VERSION "1.30"

//
//
//
typedef enum
{
  TASKHANDLE_GPS = 0,
  TASKHANDLE_SENSORS,
  TASKHANDLE_MONITOR,
  TASKHANDLE_LED,
  TASKHANDLE_LCD,
  TASKHANDLE_LAST
}
taskHandle_e;

extern xTaskHandle taskHandles [TASKHANDLE_LAST];

#endif
