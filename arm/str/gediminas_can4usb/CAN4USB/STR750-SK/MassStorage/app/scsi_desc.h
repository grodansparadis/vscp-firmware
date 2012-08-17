/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : scsi_desc.h
 *    Description : SCSI descriptor definitions
 *
 *    History :
 *    1. Date        : September 13, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/
#ifndef __SCSI_DESC_H
#define __SCSI_DESC_H

typedef enum _SpcPeripheralQualifier_t
{
  DeviceIsConnectedToLU = 0,DeviceIsNotConnectedToLU, Reserved,
  NotSuportPhysicalDevice,
} SpcPeripheralQualifier_t;

typedef enum _SpcPeripheralDeviceType_t
{
  SbcDirectAccess = 0, SscSequentialAccess,SSCPrinter,SpcProcessor,
  ScbWriteOnce,Mmc2CDROM,Scsi2Scanner,SbcOpticalMemory,
  SmcMediumChanger,Scsi2Communications,DefinedByASCIT8_1,DefinedByASCIT8_2,
  Scc2StorageArrayController,SesEnclosureServices,RbcSimplifiedDirectAccess,
  OcrwOpticalCardReader,BridgingExpanders,OsdObjectBasedStorage, UnknowType = 0x1F,
} SpcPeripheralDeviceType_t;

typedef enum _MassStorageSubClass_t
{
  MscSubClassRbc = 1, MscSubClassSff8020iMmc2, MscSubClassQic157,
  MscSubClassUfi, MscSubClassSff8070i, MscSubClassScsi
} MassStorageSubClass_t;

typedef enum _MassStorageProtocol_t
{
  MscProtocolCbiInt = 0, MscProtocolCbiNoInt,MscProtocolBulkOnly = 0x50
} MassStorageProtocol_t;

#define SpcInquiryRemovableMedium        0x80
#define SpcInquiryNotRemovableMedium     0x00

//  SCSI Primary Commands - 3
#define SpcInquiryStandartVersion        5

#endif //__SCSI_DESC_H
