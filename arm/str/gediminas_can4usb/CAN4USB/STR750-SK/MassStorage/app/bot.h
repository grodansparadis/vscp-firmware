/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : bot.h
 *    Description : Define USB Mass storage device bulk-only transport module
 *
 *    History :
 *    1. Date        : September 8, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/
#include "arm_comm.h"

#ifndef __BOT_H
#define __BOT_H

#define CbwSignature 0x43425355
#define CswSignature 0x53425355

typedef enum _BotState_t
{
  BotWaitCbw = 0, BotEpDataOut, BotEpDataIn, BotSendCsw, BotFatalError,
} BotState_t;

typedef enum _MassStorageClassRequest_t
{
  UsbBotInterfaceGetMaxLun = 0xFE,
  MassStorageClassReqReset ,
} MassStorageClassRequest_t;

typedef enum _BotToUserStatus_t
{
  BotPass = 0, BotError, BotNotReady, BotNotAlignment
} BotToUserStatus_t;

typedef enum _CSWStatus_t
{
  CswPassed = 0, CswFailed, CswPhaseError
} CSWStatus_t;

typedef enum _BotDataDir_t
{
  BotDataFormHostToDev = 0,BotDataFormDevToHost,
} BotDataDir_t;

typedef union _CbwFlags_t
{
  Int8U Data;
  struct
  {
    Int8U     : 7;
    Int8U Dir : 1;
  };
} CbwFlags_t, *pCbwFlags_t;

#pragma pack(1)

typedef union _Cbw_t
{
  Int8U Data[31];
  struct
  {
    Int32U      dCBWSignature;
    Int32U      dCBWTag;
    Int32U      dCBWDataTransferLength;
    CbwFlags_t  bmCBWFlags;
    Int8U       bCBWLUN;
    Int8U       bCBWCBLength;
    Int8U       CBWCB[16];
  };
} Cbw_t, * pCbw_t;

typedef union _Csw_t
{
  Int8U Data[13];
  struct
  {
    Int32U dCSWSignature;
    Int32U dCSWTag;
    Int32U dCSWDataResidue;
    Int8U  bCSWStatus;
  };
} Csw_t, * pCsw_t;

typedef union _BotStatus_t
{
  Int8U Flags;
  struct
  {
    Int8U PhaseError  : 1;
    Int8U CmdFault    : 1;
    Int8U BotStatus   : 1;
    Int8U DataComplete: 1;
    Int8U ShortPacket : 1;
  };
} BotStatus_t, * pBotStatus_t;

#define bPhaseErrorMask_t       0x01
#define bCmdFaultMask_t         0x02
#define bBotStatusMask_t        0x04
#define bBotDataCompleteMask_t  0x10

#pragma pack()

#endif //__BOT_H
