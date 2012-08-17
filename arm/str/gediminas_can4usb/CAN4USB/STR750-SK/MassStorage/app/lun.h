/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : lun.h
 *    Description : LUN common definitions module
 *
 *    History :
 *    1. Date        : November 15, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/
#include "includes.h"

#ifndef __LUN_H
#define __LUN_H

#ifdef  LUN_GOBALS
#define LUN_EXTERN
#else
#define LUN_EXTERN extern
#endif

typedef enum _LunState_t
{
  LunCommandDecode = 0, LunRead, LunWrite, LunVerify
} LunState_t;

typedef enum _LunMessages_t
{
  LunInitMsg = 0, LunResetReqMsg, LunInquiryReqMsg,
  LunTestUntilReadyReqMsg,LunModeSense6ReqMsg, LunReadCapacity10ReqMsg,
  LunRead10ReqMsg, LunWrite10ReqMsg,
  LunVerify10ReqMsg, LunVerify10BytChkReqMsg,
  LunReadFormatCapacityReqMsg,
  LunDataReadyMsg
} LunMessages_t;

typedef struct _LunFpnt_t
{
  DiskInitFpnt_t   DiskInitFpnt;
  DiskInfoFpnt_t   DiskInfoFpnt;
  DiskStatusFpnt_t DiskStatusFpnt;
  DiskIoFpnt_t     DiskIoFpnt;
} LunFpnt_t, *pLunFpnt_t;

extern Int8U Lun0Buffer[];

/*************************************************************************
 * Function Name: LunInit
 * Parameters: Int32U LunInd,
 *             DiskInitFpnt_t DiskInitFpnt, DiskInfoFpnt_t DiskInfoFpnt,
 *             DiskStatusFpnt_t DiskStatusFpnt, DiskIoFpnt_t, DiskIoFpnt
 *
 * Return: none
 *
 * Description: LUN Init
 *
 *************************************************************************/
void LunInit (Int32U LunInd,
              DiskInitFpnt_t DiskInitFpnt, DiskInfoFpnt_t DiskInfoFpnt,
              DiskStatusFpnt_t DiskStatusFpnt, DiskIoFpnt_t DiskIoFpnt);

/*************************************************************************
 * Function Name: LunImp
 * Parameters:  Int32U LunInd
 *
 * Return: Boolean 0 - not activity
 *                 1 - activity
 *
 * Description: LUN commands implementation
 *
 *************************************************************************/
Boolean LunImp (Int32U LunInd);

#endif //__LUN_H
