/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : usb_dev_desc.c
 *    Description : usb descriptors module
 *
 *    History :
 *    1. Date        : September 25, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/
#include "usb_desc.h"

#pragma data_alignment=4
const UsbStandardDeviceDescriptorStr_t UsbStandardDeviceDescriptorStr =
{
  sizeof(UsbStandardDeviceDescriptorStr_t),
  UsbDescriptorDevice,
  0x200,
  0,
  0,
  0,
  Ep0MaxSize,
  0xFFFF,
  0x0009,
  0x0000,
  iManufacturerStr,
  iProductStr,
  iSerialNumberStr,
  1,
};

#pragma data_alignment=4
const Int8U UsbStandardConfigurationDescriptor[] =
{
  sizeof(UsbStandardConfigurationDescriptor_t),
  UsbDescriptorConfiguration,
  (1*sizeof(UsbStandardConfigurationDescriptor_t)+
   1*sizeof(UsbStandardInterfaceDescriptor_t)+
   2*sizeof(UsbStandardEpDescriptor_t)),
  (1*sizeof(UsbStandardConfigurationDescriptor_t)+
   1*sizeof(UsbStandardInterfaceDescriptor_t)+
   2*sizeof(UsbStandardEpDescriptor_t)) >> 8,
  1,
  1,
  0,
  UsbConfigurationCommmonAttr,
  UsbConfigPower_mA(100),
  sizeof(UsbStandardInterfaceDescriptor_t),
  UsbDescriptorInterface,
  0,
  0,
  2,
  UsbDeviceClassStorage,
  MscSubClassScsi,
  MscProtocolBulkOnly,
  0,
  sizeof(UsbStandardEpDescriptor_t),
  UsbDescriptorEp,
  UsbEpIn(BulkInEp>>1),
  UsbEpTransferBulk,
  BulkInEpMaxSize,
  BulkInEpMaxSize>>8,
  0,
  sizeof(UsbStandardEpDescriptor_t),
  UsbDescriptorEp,
  UsbEpOut(BulkOutEp>>1),
  UsbEpTransferBulk,
  BulkOutEpMaxSize,
  BulkOutEpMaxSize>>8,
  0,
  0,
};

#pragma data_alignment=4
const Int8U UsbLanguagesStr [] =
{
  // Length of LanguagesStr + 2
  6,
  // Descriptor Type
  UsbDescriptorString,
  // Languages ID
  // Languages1 ID English
  0x09,0x04,
  // Languages2 ID Bulgarian
  0x02,0x04
};

#pragma data_alignment=4
const Int8U ManufacturerStrLan1 [] =
{
  24, // length
  UsbDescriptorString,  // Descriptor
  'I',0,'A',0,'R',0,' ',0,'S',0,'y',0,'s',0,'t',0,'e',0,'m',0,'s',0, // Str
};

#pragma data_alignment=4
const Int8U ManufacturerStrLan2 [] =
{
  24, //length
  UsbDescriptorString, // Descriptor
  'È',0,'À',0,'Ð',0,' ',0,'Ñ',0,'è',0,'ñ',0,'ò',0,'å',0,'ì',0,'è',0, // Str
};

#pragma data_alignment=4
const Int8U ProductStrLan1 [] =
{
  24, //length
  UsbDescriptorString, // Descriptor
  'I',0,'A',0,'R',0,' ',0,'S',0,'T',0,'R',0,'7',0,'5',0,'0',0,'F',0, // Str
};

#pragma data_alignment=4
const Int8U ProductStrLan2 [] =
{
  24, //length
  UsbDescriptorString, // Descriptor
  'È',0,'À',0,'Ð',0,' ',0,'Ñ',0,'Ò',0,'Ð',0,'7',0,'5',0,'0',0,'Ô',0, // Str
};

#pragma data_alignment=4
const Int8U SerialNumberStrLan1 [] =
{
  18, //length
  UsbDescriptorString, // Descriptor
  '0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'1',0, // Str
};

const Int8U * const UsbLanguages1Strings[] =
{
  // iManufacturerStr
  ManufacturerStrLan1,
  // iProductStr
  ProductStrLan1,
  // iSerialNumberStr
  SerialNumberStrLan1,
  // Terminator
  NULL
};

const Int8U * const UsbLanguages2Strings[] =
{
  // iManufacturerStr
  ManufacturerStrLan2,
  // iProductStr
  ProductStrLan2,
  // iSerialNumberStr
  SerialNumberStrLan1,
  // Terminator
  NULL
};

const Int8U * const * const UsbString[] =
{
  UsbLanguages1Strings,
  UsbLanguages2Strings,
};

