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
 *    1. Date        : November 15, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 16170 $
 **************************************************************************/
#include "arm_comm.h"

#ifndef __DISK_H
#define __DISK_H

typedef enum _DiskStatusCode_t
{
  DiskCommandPass = 0, DiskNotReady, DiskNotPresent,
  DiskParametersError, DiskMiscompareError, DiskChanged,
  DiskUknowError,
} DiskStatusCode_t;

typedef enum _DiskIoRequest_t
{
  DiskWrite = 0, DiskRead, DiskVerify,
} DiskIoRequest_t;

typedef enum _DiskInfoType_t
{
  DiskInquiry = 0, DiskFormatCapacity,
} DiskInfoType_t;

typedef enum _DiskType_t
{
  DiskMMC, DiskSD, DiskUnknow,
} DiskType_t;

typedef struct _DiskCtrlBlk_t
{
  Int32U  BlockNumb;
  Int32U  BlockSize;
  DiskStatusCode_t DiskStatus;
  DiskType_t DiskType;
  Boolean WriteProtect;
  Boolean MediaChanged;
} DiskCtrlBlk_t, *pDiskCtrlBlk_t;

typedef void            (* DiskInitFpnt_t)(void);
typedef Int32U          (* DiskInfoFpnt_t)(pInt8U,DiskInfoType_t);
typedef pDiskCtrlBlk_t  (* DiskStatusFpnt_t)(void);
typedef DiskStatusCode_t(* DiskIoFpnt_t)(pInt8U,Int32U,Int32U,DiskIoRequest_t);


#endif //__LUN_H
