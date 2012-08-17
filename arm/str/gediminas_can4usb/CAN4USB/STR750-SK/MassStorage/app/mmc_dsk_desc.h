/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : mmc_dsk_desc.h
 *    Description : Define MMC/SD DISK descriptors
 *
 *    History :
 *    1. Date        : November 23, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/
#include "arm_comm.h"
#include "scsi_desc.h"

#ifndef __MMC_DSK_DESC_H
#define __MMC_DSK_DESC_H

#ifdef  MMC_DISK_DESC_GLOBAL
#define MMC_DISK_DESC_EXTERN
#else
#define MMC_DISK_DESC_EXTERN extern
#endif

#define SizeOfInquiryDescMmcDsk     36

MMC_DISK_DESC_EXTERN const Int8U MmcDskInquiry[];

#endif // __MMC_DSK_DESC_H
