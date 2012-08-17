/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : usb_dev_desc.h
 *    Description : Descriptor definitions
 *
 *    History :
 *    1. Date        : September 22, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/
#include "includes.h"

#ifndef __USB_DEV_DESC_H
#define __USB_DEV_DESC_H

extern const UsbStandardDeviceDescriptorStr_t UsbStandardDeviceDescriptorStr;
extern const Int8U UsbStandardConfigurationDescriptor[];
extern const Int8U UsbLanguagesStr [];
extern const Int8U * const * const UsbString[];

#endif //__USB_DEV_DESC_H
