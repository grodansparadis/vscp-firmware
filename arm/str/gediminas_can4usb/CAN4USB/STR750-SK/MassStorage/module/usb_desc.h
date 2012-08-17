/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : usb_des.h
 *    Description : Define standard USB descriptors module
 *
 *    History :
 *    1. Date        : August 10, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/
#include "includes.h"

#ifndef __USB_DESC_H
#define __USB_DESC_H

#define UsbConfigPower_mA(mA)   ((mA)/2)
#define UsbEpOut(addr)          ((addr) | 0x00)
#define UsbEpIn(addr)           ((addr) | 0x80)

#define UsbDescLengthOffset     0
#define UsbDescDescriptorOffset 1

typedef enum _UsbDescriptorType_t
{
  UsbDescriptorDevice = 1, UsbDescriptorConfiguration, UsbDescriptorString,
  UsbDescriptorInterface, UsbDescriptorEp, UsbDescriptorDeviceQualifier,
  UsbDescriptorOtherSeedConfiguration, UsbDescriptorInterfacePower
} UsbDescriptorType_t;

typedef enum _UsbStringsIndex_t
{
  iLanguagesStr = 0, iManufacturerStr, iProductStr, iSerialNumberStr,
  iConfigurationStr,
  iInterface,
} UsbStringsIndex_t;

typedef enum _UsbDeviceClassses_t
{
/* USB Device Classes */
  UsbDeviceClassReserved = 0, UsbDeviceClassAudio,
  UsbDeviceClassCommunications,UsbDeviceClassHumanInterface,
  UsbDeviceClassMonitor,UsbDeviceClassPhysicalInterface,
  UsbDeviceClassPower,UsbDeviceClassPrinter,
  UsbDeviceClassStorage,UsbDeviceClassHub,
  UsbDeviceClassVendorSpecific = 0xFF
} UsbDeviceClassses_t;

typedef union _CnfgArtr_t
{
  Int8U Data;
  struct
  {
    Int8U Reserved      : 5;
    Int8U RemoteWakeup  : 1;
    Int8U SelfPowered   : 1;
    Int8U MustBeOne     : 1;
  };
} CnfgArtr_t, * pCnfgArtr_t;

#define USBPowerMask          0x40
#define USBRemoteWakeUpMask   0x20

typedef enum _UsbConfigurationAttr_t
{
  UsbRemoteWakeup = 0x20, UsbSelfPowered = 0x40,  UsbConfigurationCommmonAttr = 0x80
} UsbConfigurationAttr_t;

typedef enum _UsbEpTransferType_t
{
  UsbEpTransferControl = 0, UsbEpTransferIsochronous,
  UsbEpTransferBulk,UsbEpTransferInterrupt
}UsbEpTransferType_t;

typedef enum _UsbEpSynchronizationType_t
{
  UsbEpSynchNoSynchronization = 0, UsbEpSynchAsynchronous = 1<<2,
  UsbEpSynchAdaptive = 2<<2, UsbEpSynchSynchronous = 3<<2
}UsbEpSynchronizationType_t;

typedef enum _UsbEpUsageType_t
{
  UsbEpUsageData = 0, UsbEpUsageFeedback = 1<<4,
  UsbEpUsageImplicit = 2<<4, UsbEpUsageReserved = 3<<4
}UsbEpUsageType_t;

typedef union _EpAttr_t
{
  Int8U Data;
  struct
  {
    Int8U TransferType        : 2;
    Int8U SynchronizationType : 2;
    Int8U UsageType           : 2;
  };
} EpAttr_t, * pEpAttr_t;

#pragma pack(1)
typedef struct _UsbStandardDeviceDescriptorStr_t
{
  Int8U       bLength;
  Int8U       bDescriptorType;
  Int16U      bcdUSB;
  Int8U       bDeviceClass;
  Int8U       bDeviceSubClass;
  Int8U       bDeviceProtocol;
  Int8U       bMaxPacketSize0;
  Int16U      idVendor;
  Int16U      idProduct;
  Int16U      bcdDevice;
  Int8U       iManufacturer;
  Int8U       iProduct;
  Int8U       iSerialNumber;
  Int8U       bNumConfigurations;
} UsbStandardDeviceDescriptorStr_t, * pUsbStandardDeviceDescriptorStr_t;


typedef struct _UsbStandardConfigurationDescriptor_t
{
  Int8U       bLength;
  Int8U       bDescriptorType;
  Int16U      wTotalLength;
  Int8U       bNumInterfaces;
  Int8U       bConfigurationValue;
  Int8U       iConfiguration;
  CnfgArtr_t  bmAttributes;
  Int8U       bMaxPower;
} UsbStandardConfigurationDescriptor_t, * pUsbStandardConfigurationDescriptor_t;

typedef struct _UsbStandardInterfaceDescriptor_t
{
  Int8U       bLength;
  Int8U       bDescriptorType;
  Int8U       bInterfaceNumber;
  Int8U       bAlternateSetting;
  Int8U       bNumEndpoints;
  Int8U       bInterfaceClass;
  Int8U       bInterfaceSubClass;
  Int8U       bInterfaceProtocol;
  Int8U       iInterface;
} UsbStandardInterfaceDescriptor_t, * pUsbStandardInterfaceDescriptor_t;

typedef struct _UsbStandardEpDescriptor_t
{
  Int8U       bLength;
  Int8U       bDescriptorType;
  Int8U       bEndpointAddress;
  EpAttr_t    bmAttributes;
  Int16U      wMaxPacketSize;
  Int8U       bInterval;
} UsbStandardEpDescriptor_t, * pUsbStandardEpDescriptor_t;

#pragma pack()

extern const UsbStandardDeviceDescriptorStr_t UsbStandardDeviceDescriptorStr;
extern const Int8U UsbStandardConfigurationDescriptor[];
extern const Int8U UsbLanguagesStr [];
extern const Int8U * const * const UsbString[];

#endif //__USB_DESC_H
