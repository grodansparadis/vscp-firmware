/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : str75x_usb_cfg.h
 *    Description : USB definitions
 *
 *    History :
 *    1. Date        : February 10, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/

#include "includes.h"

#ifndef __STR75X_USB_CFG_H
#define __STR75X_USB_CFG_H

#define USB_EXT_CLK               0   // External 48MHz clock

/* USB Events */
#define USB_SOF_EVENT             0
#define USB_ERROR_EVENT           0   // for debug
#define USB_HIGH_PRIORITY_EVENT   1   // ISO and Double buffered bulk
#define USB_DOVR_EVENT            0   // for speed up retransfer

/* Endpoint definitions */
#define Ep0MaxSize        			  8

#define BulkOutEp                 UsbEp1Out
#define BulkOutEpMaxSize          64

#define BulkInEp                  UsbEp1In
#define BulkInEpMaxSize           64

#define MaxIndOfRealizeEp         2

/* Other definitions */

#endif //__STR75X_USB_CFG_H
