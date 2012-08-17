/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : cdc_cnfg.h
 *    Description : Communication Device Class configuration
 *
 *    History :
 *    1. Date        : July 4, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
**************************************************************************/
#include "includes.h"

#ifndef __CDC_CFG_H
#define __CDC_CFG_H

#define CDC_DEVICE_SUPPORT_LINE_CODING  1
#define CDC_DEVICE_SUPPORT_LINE_STATE   1

#define CDC_DEVICE_SUPPORT_BREAK        1

#define CDC_DATA_RATE                   CBR_9600
#define CDC_DATA_BITS                   8
#define CDC_PARITY                      NOPARITY
#define CDC_STOP_BITS                   ONESTOPBIT

#define CDC_LINE_DTR                    1
#define CDC_LINE_RTS                    1

#endif //__CDC_CFG_H
