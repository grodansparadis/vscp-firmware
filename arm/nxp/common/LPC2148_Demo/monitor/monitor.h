//
//  $Id: monitor.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/monitor/monitor.h $
//

#ifndef _MONITOR_H_
#define _MONITOR_H_

#include "FreeRTOS.h"
#include "task.h"

//
//
//
typedef enum
{
  CMDTYPE_CMDLIST = 0,
  CMDTYPE_FUNCTION
}
cmdType_e;

//
//
//
typedef struct commandList_s
{
  const portCHAR *command;
  portCHAR minArgs;
  portCHAR maxArgs;
  cmdType_e cmdType;
  union
  {
    const void *trickGCC;
    int (*handler) (int argc, portCHAR **argv);
    struct commandList_s *commandList;
  };
  const portCHAR *description;
  const portCHAR *parameters;
}
commandList_t;

//
//
//
portTASK_FUNCTION_PROTO (vMonitorTask, pvParameters);

#endif
