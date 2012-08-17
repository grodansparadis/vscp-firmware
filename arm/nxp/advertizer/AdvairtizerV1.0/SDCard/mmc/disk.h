/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : disk.h
 *    Description : Disk common definitions module
 *
 *    History :
 *    1. Data        : November 15, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 1.4 $
**************************************************************************/
#ifndef __DISK_H
#define __DISK_H

typedef enum
{
  DiskCommandPass = 0, DiskNotReady, DiskNotPresent,
  DiskParametersError, DiskMiscompareError, DiskChanged,
  DiskUknowError,
} DiskStatusCode_t;

typedef enum
{
  DiskWrite = 0, DiskRead, DiskVerify,
} DiskIoRequest_t;

typedef enum
{
  DiskInquiry = 0, DiskFormatCapacity,
} DiskInfoType_t;

typedef enum
{
  DiskMMC, DiskSD, DiskUnknow,
} DiskType_t;

typedef struct
{
  U32  BlockNumb;
  U32  BlockSize;
  DiskStatusCode_t DiskStatus;
  DiskType_t DiskType;
  bool WriteProtect;
  bool MediaChanged;
} DiskStatus_t, *pDiskStatus_t;

typedef void            (* DiskInitFpnt_t)(void);
typedef U32          (* DiskInfoFpnt_t)(U8,DiskInfoType_t);
typedef pDiskStatus_t   (* DiskStatusFpnt_t)(void);
typedef DiskStatusCode_t(* DiskIoFpnt_t)(U8,U32,U32,DiskIoRequest_t);


#endif //__LUN_H
