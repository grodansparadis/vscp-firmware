/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : mmc_dsk_desc.c
 *    Description : MMC/SD descriptor
 *
 *    History :
 *    1. Date        : September 22, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/
#define  MMC_DISK_DESC_GLOBAL
#include "mmc_dsk_desc.h"

#pragma data_alignment=4
const Int8U MmcDskInquiry[] =
{
// 0 PERIPHERAL QUALIFIER PERIPHERAL DEVICE TYPE
  SbcDirectAccess,
//1 RMB
  SpcInquiryRemovableMedium,
// 2 VERSION
  SpcInquiryStandartVersion,
// 3 AERC Obsolete NORMACA HISUP RESPONSE DATA FORMAT
  0x02,
// 4 ADDITIONAL LENGTH (n-4)
  36-4,
// 5 SCCS
  0x00,
// for parallel SCSI only
// 6 BQUE ENCSERV VS MULTIP MCHNGR Obsolete Obsolete ADDR16†
  0x00,
// 7 RELADR Obsolete WBUS16† SYNC† LINKED Obsolete CMDQUE VS
  0x00,
// 8 - 15 VENDOR IDENTIFICATION
  'I','A','R',' ','S','y','s','.',
// 16 - 31 PRODUCT IDENTIFICATION
  'S','T','R','7','5','0','F',' ','S','t','o','r','a','g','e',' ',
// 32 - 35 PRODUCT REVISION LEVEL
  '1','.','0','0',
};


