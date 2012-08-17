/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : scsi.h
 *    Description : Definition of SCSI
 *
 *    History :
 *    1. Date        : September 27, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *    2. Date        : January 27, 2006
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *        Add Unit Start/Stop command implement
 *    3. Date        : November 23, 2006
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *        Adapt for STR912
 *
 *    $Revision: 15135 $
 **************************************************************************/
#include <includes.h>

#ifndef __SCSI_H
#define __SCSI_H

#include "bot.h"
#include "scsi_cmd.h"

#ifdef SCSI_GOBALS
#define SCSI_EXTERN
#else
#define SCSI_EXTERN extern
#endif

#define SCSI_LUN_NUMB   1

// DMA descriptor index for BOT
#define DMA_BOT_OUT_DES 0
#define DMA_BOT_IN_DES  1

#define ScsiStallIn     1
#define ScsiStallOut    2
#define ScsiStallBoth   3

typedef struct _ScsiDrvObj_t
{
  Int32U Message[3];  // Offset 0 Message Type
                      // Implement Only For IO
                      //  Offset 1 BlockAddress
                      //  Offset 2 NumbOfBocks
  ScsiStatusCode_t Status;
  StartStop_t      UnitSSStatus;
} ScsiDrvObj_t, *pScsiDrvObj_t;

typedef struct _ScsiEpStrl_t
{
  pInt8U pData;
  Int32U  Size;
} ScsiEpStrl_t, *pScsiEpStrl_t;

SCSI_EXTERN pInt32U pScsiMessage[SCSI_LUN_NUMB];

/*************************************************************************
 * Function Name: ScsiInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init SCSI
 *
 *************************************************************************/
void ScsiInit (void);

/*************************************************************************
 * Function Name: UsbClassBotInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB Bulk-only transport init
 *
 *************************************************************************/
void UsbClassBotInit (void);

/*************************************************************************
 * Function Name: UsbClassBotConfigure
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: USB Class storage configure
 *
 *************************************************************************/
void * UsbClassBotConfigure (void * pArg);

/*************************************************************************
 * Function Name: UsbBotRstInterface
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB Bulk-only transport interface reset
 *
 *************************************************************************/
void UsbBotRstInterface (void);

/*************************************************************************
 * Function Name: BotInEpStall
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Stall BOT In EP
 *
 *************************************************************************/
void BotInEpStall (void);

/*************************************************************************
 * Function Name: BotOutEpStall
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Stall BOT Out EP
 *
 *************************************************************************/
void BotOutEpStall (void);

/*************************************************************************
 * Function Name: UsbClassBotRequest
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: Implement USB Class storage requests
 *
 *************************************************************************/
void * UsbClassBotRequest (void * pArg);

/*************************************************************************
 * Function Name: BotOutEP
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: USB BOT Out EP handler
 *
 *************************************************************************/
void * BotOutEP (void * Arg);

/*************************************************************************
 * Function Name: BotInEP
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: USB BOT In EP handler
 *
 *************************************************************************/
void * BotInEP (void * Arg);

/*************************************************************************
 * Function Name: BotUnstallCallBack
 * Parameters: void * pArg
 *
 * Return: void *
 *
 * Description: Unstall callback function
 *
 *************************************************************************/
static
void * BotUnstallCallBack (void * pArg);

/*************************************************************************
 * Function Name: BotWrite
 * Parameters: pInt8U pData ,Int32U Size
 *
 * Return: none
 *
 * Description: Send data by BulkInEp
 *
 *************************************************************************/
static
void BotWrite(pInt8U pData ,Int32U Size);

/*************************************************************************
 * Function Name: BotReceiveDataInit
 * Parameters: pInt8U pData, Int32U DataSize, Boolean DataComplete
 *
 * Return: BotToUserStatus_t
 *
 * Description: Init Receive
 *
 *************************************************************************/
BotToUserStatus_t BotReceiveDataInit (pInt8U pData, Int32U DataSize,
                                      Boolean DataComplete);

/*************************************************************************
 * Function Name: BotSendDataInit
 * Parameters:  const pInt8U pData, Int32U DataSize, Boolean DataComplete
 *
 * Return: BotToUserStatus_t
 *
 * Description: Init Transmit
 *
 *************************************************************************/
BotToUserStatus_t BotSendDataInit (const pInt8U pData, Int32U DataSize,
                                   Boolean DataComplete);

/*************************************************************************
 * Function Name: BotSendStatus
 * Parameters:  Int32U DataSize, Int8U * pData, Boolean DataComplete
 *
 * Return: BotToUserStatus_t
 *
 * Description: Init Transmit of CSW
 *
 *************************************************************************/
void BotSendStatus (void);

/*************************************************************************
 * Function Name: ScsiCommImpl
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Scsi commands implement
 *
 *************************************************************************/
inline void ScsiCommImpl (void);

/*************************************************************************
 * Function Name: ScsiCbwValid
 * Parameters:  none
 *
 * Return: Boolean
 *
 * Description: CBW valid and meaningful check
 *
 *************************************************************************/
inline Boolean ScsiCbwValid(void);

/*************************************************************************
 * Function Name: ScsiTestUnitReadyImp
 * Parameters: none
 *
 * Return: none
 *
 * Description: TEST UNIT READY command implement
 *
 *************************************************************************/
inline void ScsiTestUnitReadyImp (void);

/*************************************************************************
 * Function Name: ScsiTestUntilReadyData
 * Parameters:  Int32U MediaReady
 *
 * Return: void
 *
 * Description: Prepare  TEST UNIT READY data for sending
 *
 *************************************************************************/
void ScsiTestUntilReadyData (Int32U MediaReady);

/*************************************************************************
 * Function Name: ScsiRequestSenseImp
 * Parameters:  none
 *
 * Return: Boolean
 *
 * Description: REQUEST SENSE command implement
 *
 *************************************************************************/
inline void ScsiRequestSenseImp (void);

/*************************************************************************
 * Function Name: ScsiModeSelect6Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: MODE SELECT command implement
 *
 *************************************************************************/
inline void ScsiModeSelect6Imp (void);

/*************************************************************************
 * Function Name: ScsiModeSense6Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: MODE SENSE command implement
 *
 *************************************************************************/
inline void ScsiModeSense6Imp (void);

/*************************************************************************
 * Function Name: ScsiModeSenseData
 * Parameters:  Int32U WriteProtect
 *
 * Return: none
 *
 * Description: Prepare MODE SENSE data for sending
 *
 *************************************************************************/
void ScsiModeSenseData (Int32U WriteProtect);

/*************************************************************************
 * Function Name: ScsiInquiryImp
 * Parameters:  none
 *
 * Return: none
 *
 * Description:INQUIRY command implement
 *
 *************************************************************************/
inline void ScsiInquiryImp (void);

/*************************************************************************
 * Function Name: ScsiInquiryData
 * Parameters: const pInt8U pData, Int32U Size
 *
 * Return: none
 *
 * Description: Prepare INQUIRY data for sending
 *
 *************************************************************************/
void ScsiInquiryData (const pInt8U pData, Int32U Size);

/*************************************************************************
 * Function Name: ScsiMediaRemovalImp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: PREVENT ALLOW MEDIUM REMOVAL command implement
 *
 *************************************************************************/
inline void ScsiMediaRemovalImp(void);

/*************************************************************************
 * Function Name: ScsiReadCapacity10Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: READ CAPACITY (10) command implement
 *
 *************************************************************************/
inline void ScsiReadCapacity10Imp (void);

/*************************************************************************
 * Function Name: ScsiReadCapacityData
 * Parameters:  Int32U LogBlockAdd, Int32U BlockSize
 *
 * Return: none
 *
 * Description: Prepare READ CAPACITY data for sending
 *
 *************************************************************************/
void ScsiReadCapacityData (Int32U LogBlockAdd, Int32U BlockSize);

/*************************************************************************
 * Function Name: ScsiRead10Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: READ(10) command implement
 *
 *************************************************************************/
inline void ScsiRead10Imp (void);

/*************************************************************************
 * Function Name: ScsiReadData
 * Parameters: const pInt8U pData, Int32U Size, Boolean DataComplete
 *
 * Return: none
 *
 * Description: Prepare READ for sending
 *
 *************************************************************************/
void ScsiReadData(const pInt8U pData, Int32U Size, Boolean DataComplete);

/*************************************************************************
 * Function Name: ScsiWrite10Imp
 * Parameters: none
 *
 * Return: none
 *
 * Description: WRITE(10) command implement
 *
 *************************************************************************/
inline void ScsiWrite10Imp (void);

/*************************************************************************
 * Function Name: ScsiMediaStartStopImp
 * Parameters: none
 *
 * Return: none
 *
 * Description: START STOP UNIT command implement
 *
 *************************************************************************/
inline void ScsiMediaStartStopImp (void);

/*************************************************************************
 * Function Name: ScsiWriteData
 * Parameters:  pInt8U pData, Int32U Size, Boolean DataComplete
 *
 * Return: none
 *
 * Description: Prepare WRITE data for receiving
 *
 *************************************************************************/
void ScsiWriteData(pInt8U pData, Int32U Size, Boolean DataComplete);

/*************************************************************************
 * Function Name: ScsiVerify10Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: VERIFY (10) command implement
 *
 *************************************************************************/
void ScsiVerify10Imp (void);

/*************************************************************************
 * Function Name: ScsiReadFormatCapcityImp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: READ FORMAT CAPACITIES implement
 *
 *************************************************************************/
void ScsiReadFormatCapcityImp (void);

/*************************************************************************
 * Function Name: ScsiReadFormatCapcityData
 * Parameters:  pInt8U pData, Int32U Size
 *
 * Return: none
 *
 * Description: Prepare Data of READ FORMAT CAPACITIES command request
 *
 *************************************************************************/
void ScsiReadFormatCapcityData (pInt8U pData, Int32U Size);

/*************************************************************************
 * Function Name: ScsiCmdStatus
 * Parameters:  ScsiStatusCode_t Status
 *
 * Return: none
 *
 * Description: Prepare Status data for sending
 *
 *************************************************************************/
void ScsiCmdStatus (ScsiStatusCode_t Status);

/*************************************************************************
 * Function Name: ScsiCmdError
 * Parameters:  Int32U Status, Int32U Stall
 *
 * Return: none
 *
 * Description: Prepare Error data for sending
 *
 *************************************************************************/
void ScsiCmdError (Int32U Status, Int32U Stall);

#endif // __SCSI_H
