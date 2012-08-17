/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : scsi_cmd.h
 *    Description : Definition of SCSI commands (SPC, SBP, MMC command set)
 *
 *    History :
 *    1. Date        : September 27, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *    2. Date        : January 27, 2006
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *        Add Unit Start/Stop command definitions
 *
 *    $Revision: 15135 $
 **************************************************************************/
#include "arm_comm.h"

#ifndef __SCSI_CMD_H
#define __SCSI_CMD_H

// Data Command block definition
typedef enum _ScsiCommands_t
{
  ScsiTestUnitReady     = 0x00,
  ScsiRequestSense      = 0x03,
  ScsiInquiry           = 0x12,
  ScsiModeSelect6       = 0x15,
  ScsiModeSense6        = 0x1A,
  ScsiMediaStartStop    = 0x1B,
  ScsiMediaRemoval      = 0x1E,
  ScsiReadFormatCapcity = 0x23,
  ScsiReadCapacity10    = 0x25,
  ScsiRead10            = 0x28,
  ScsiWrite10           = 0x2A,
  ScsiFerify10          = 0x2F,
} ScsiCommands_t;

typedef enum _SpcSkd_t
{
  NoSense = 0, RecoveredError, NotReady, MediumError, HardwareError,
  IllegalRequest, UnitAttention, DataProtect, BlankCheck, VendorSpecific,
  CopyAborted, AbortedCommand, VolumeOverflow = 0xD, Miscompare,
} SpcSkd_t;

typedef enum _SpcAscAscq_t
{
  NoAdditionalSenseInformation                    = 0x0000UL,
  LogicalUnitNotReadyCauseNotReportable           = 0x0004UL,
  InvalidFieldInCdb                               = 0x0024UL,
  InvalidCommandOperationCode                     = 0x0020UL,
  NotReadyToReadyChangeMediumMayHaveChanged       = 0x0028UL,
  MediumNotPresent                                = 0x003AUL,
  MiscompareDuringVerifyOperation                 = 0x001DUL
} SpcAscAscq_t;

typedef enum _ScsiStatusCode_t
{
  ScsiCommandNoKey = 0, ScsiMediamNotReady, ScsiInvalidCbd,
  ScsiUnknowCommand, ScsiMediaNotPresent, ScsiMediaChanged,
  ScsiMiscompare, ScsiFatalError,
} ScsiStatusCode_t;

typedef enum _StartStop_t
{
  UnitEjected = 0, UnitStarted,
} StartStop_t;

typedef enum _FormatCapacityDescType_t
{
  UnformattedMedia = 1, FormattedMedia, NoMediaPresent,
} FormatCapacityDescType_t;

typedef union _ScsiCmdField_t
{
  Int8U Cmd;
  struct
  {
    Int8U CommandCode : 5;
    Int8U GroupCode   : 3;
  };
} ScsiCmdField_t, * pScsiCmdField_t;

typedef union _SpcControl_t
{
  Int8U Control;
  struct
  {
    Int8U Link        : 1;
    Int8U             : 1;
    Int8U NACA        : 1;
    Int8U             : 3;
    Int8U VendorSpec  : 2;
  };
} SpcControl_t, *pSpcControl_t;

#pragma pack(1)
// Commands definitions

typedef struct _Spc3TestUnitReady_t
{
  Int8U  OperationCode;
  Int8U  Reserved0;
  Int8U  Reserved1;
  Int8U  Reserved2;
  Int8U  Reserved3;
  SpcControl_t  Control;
} Spc3TestUnitReady_t, *pSpc3TestUnitReady_t;

typedef struct _Spc3RequestSense_t
{
  Int8U OperationCode;
  struct
  {
    Int8U DESC : 1;
    Int8U      : 7;
  };
  Int8U Reserved0;
  Int8U Reserved1;
  Int8U AllocationLenght;
  SpcControl_t Control;
} Spc3RequestSense_t, * pSpc3RequestSense_t;

typedef struct _Spc3Inquiry_t
{
  Int8U OperationCode;
  struct
  {
    Int8U EVPD : 1;
    Int8U      : 7;
  };
  Int8U PageCode;
  Int8U AllocationLenght[2];
  SpcControl_t Control;
} Spc3Inquiry_t, * pSpc3Inquiry_t;

typedef struct _Spc3ModeSelect6_t
{
  Int8U OperationCode;
  struct
  {
    Int8U SP   : 1;
    Int8U      : 3;
    Int8U PF   : 1;
    Int8U      : 3;
  };
  Int8U Reserved0;
  Int8U Reserved1;
  Int8U Reserved2;
  Int8U ParameterListLenght;
  SpcControl_t Control;
} Spc3ModeSelect6_t, * pSpc3ModeSelect6_t;

typedef struct _Spc3ModeSense6_t
{
  Int8U OperationCode;
  struct
  {
    Int8U      : 3;
    Int8U DBD  : 1;
    Int8U      : 4;
  };
  struct
  {
    Int8U PageCode : 6;
    Int8U PC       : 2;
  };
  Int8U SubPageCode;
  Int8U AllocationLenght;
  SpcControl_t Control;
} Spc3ModeSense6_t, * pSpc3ModeSense6_t;

typedef struct _Spc3MediaRemoval_t
{
  Int8U OperationCode;
  Int8U Reserved0;
  Int8U Reserved1;
  Int8U Reserved2;
  struct
  {
    Int8U Prevent : 2;
    Int8U         : 6;
  };
  SpcControl_t Control;
} Spc3MediaRemoval_t, * pSpc3MediaRemoval_t;

typedef struct _Sbc2ReadCapacity10_t
{
  Int8U OperationCode;
  Int8U Reserved0;
  Int8U LogicalBockAddress[4];
  Int8U Reserved1;
  Int8U Reserved2;
  struct
  {
    Int8U PMI     : 1;
    Int8U         : 7;
  };
  SpcControl_t Control;
} Sbc2ReadCapacity10_t, * pSbc2ReadCapacity10_t;

typedef struct _Sbc2Read10_t
{
  Int8U OperationCode;
  struct
  {
    Int8U Obsolete  : 1;
    Int8U FUA_NV    : 1;
    Int8U Reserved  : 1;
    Int8U FUA       : 1;
    Int8U DPO       : 1;
    Int8U RDPROTECT : 3;
  };
  Int8U LogicalBockAddress[4];
  struct
  {
    Int8U CroupNumb : 5;
    Int8U           : 3;
  };
  Int8U TransferLength[2];
  SpcControl_t Control;
} Sbc2Read10_t, * pSbc2Read10_t;

typedef struct _Sbc2Write10_t
{
  Int8U OperationCode;
  struct
  {
    Int8U Obsolete  : 1;
    Int8U FUA_NV    : 1;
    Int8U Reserved  : 1;
    Int8U FUA       : 1;
    Int8U DPO       : 1;
    Int8U WRPROTECT : 3;
  };
  Int8U LogicalBockAddress[4];
  struct
  {
    Int8U CroupNumb : 5;
    Int8U           : 3;
  };
  Int8U TransferLength[2];
  SpcControl_t Control;
} Sbc2Write10_t, * pSbc2Write10_t;

typedef struct _Sbc2Verify10_t
{
  Int8U OperationCode;
  struct
  {
    Int8U Obsolete  : 1;
    Int8U BYTCHK    : 1;
    Int8U Reserved  : 2;
    Int8U DPO       : 1;
    Int8U VRPROTECT : 3;
  };
  Int8U LogicalBockAddress[4];
  struct
  {
    Int8U CroupNumb : 5;
    Int8U           : 3;
  };
  Int8U TransferLength[2];
  SpcControl_t Control;
} Sbc2Verify10_t, * pSbc2Verify10_t;

typedef struct _Mmc3ReadFormatCapacity_t
{
  Int8U OperationCode;
  Int8U Reserved0;
  Int8U Reserved1;
  Int8U Reserved2;
  Int8U Reserved3;
  Int8U Reserved4;
  Int8U Reserved5;
  Int8U AllocationLenght[2];
  SpcControl_t Control;
} Mmc3ReadFormatCapacity_t, * pMmc3ReadFormatCapacity_t;

// Command responses definitions
typedef struct _Spc3RequestSenseResponse_t
{
  struct
  {
    Int8U ResponceCode : 7;
    Int8U Valid        : 1;
  };
  Int8U Obsolete;
  struct
  {
    Int8U SenseKey : 4;
    Int8U Reserved : 1;
    Int8U ILI      : 1;
    Int8U EOM      : 1;
    Int8U FILEMARK : 1;
  };
  Int8U Information[4];
  Int8U AddSenseKeyLength;
  Int8U CommSpecInfo[4];
  Int8U Asc;
  Int8U Ascq;
  Int8U Fruc;
  Int8U Sks[3];
} Spc3RequestSenseResponse_t, *pSpc3RequestSenseResponse_t;

typedef struct _Spc3ModeSense6Response_t
{
  Int8U ModeDataLength;
  Int8U MediumType;
  struct
  {
    Int8U         : 4;
    Int8U DPOFUA  : 1;
    Int8U         : 2;
    Int8U WP      : 1;
  };
  Int8U BlockDeskLength;
} Spc3ModeSense6Response_t, * pSpc3ModeSense6Response_t;

typedef struct _Sbc2ReadCapacity10Response_t
{
  Int8U LogicalBlockAddress[4];
  Int8U BlockLength[4];
} Sbc2ReadCapacity10Response_t, * pSbc2ReadCapacity10Response_t;

typedef struct _Mmc3FormatCapDescriptor_t
{
  Int8U NumberofBlocks[4];
  struct
  {
    Int8U DescriptorType : 2;
    Int8U FormatType     : 6;
  };
  Int8U BlockLength[3];
} Mmc3FormatCapDescriptor_t, *pMmc3FormatCapDescriptor_t;

typedef struct _Mmc3FormatCapResponse_t
{
  Int8U Reserved0;
  Int8U Reserved1;
  Int8U Reserved2;
  Int8U CapacityListLength;
  Mmc3FormatCapDescriptor_t MaximumDescriptor;
} Mmc3FormatCapResponse_t, * pMmc3FormatCapResponse_t;

typedef struct _Spc3UnitStartStop_t
{
  Int8U OperationCode;
  struct
  {
    Int8U IMMED : 1;
    Int8U       : 7;
  };
  Int8U Reserved0;
  Int8U Reserved1;
  struct
  {
    Int8U START : 1;
    Int8U LOEJ  : 1;
    Int8U       : 2;
    Int8U PC    : 4;
  };
  SpcControl_t Control;
} Spc3UnitStartStop_t, *pSpc3UnitStartStop_t;
#pragma pack()

#endif // __SCSI_CMD_H
