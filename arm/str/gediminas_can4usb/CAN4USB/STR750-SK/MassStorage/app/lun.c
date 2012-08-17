/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : lun.c
 *    Description : USB Mass storage device LUNs
 *
 *    History :
 *    1. Datå        : November 15, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 16170 $
 **************************************************************************/
#define LUN_GOBALS
#include "lun.h"

LunFpnt_t LunFun [SCSI_LUN_NUMB];

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
              DiskStatusFpnt_t DiskStatusFpnt, DiskIoFpnt_t DiskIoFpnt)
{
  LunFun[LunInd].DiskInitFpnt   = DiskInitFpnt;
  LunFun[LunInd].DiskInfoFpnt   = DiskInfoFpnt;
  LunFun[LunInd].DiskStatusFpnt = DiskStatusFpnt;
  LunFun[LunInd].DiskIoFpnt     = DiskIoFpnt;
}

/*************************************************************************
 * Function Name: LunImp
 * Parameters:  none
 *
 * Return: Boolean 0 - not activity
 *                 1 - activity
 *
 * Description: LUN commands implementation
 *
 *************************************************************************/
Boolean LunImp (Int32U LunInd)
{
static LunState_t LunState;
static Int32U BlockStart, BlockNum;
pLunFpnt_t Lun = &LunFun[LunInd];
pDiskCtrlBlk_t pDiskCrtl;

  if(pScsiMessage[LunInd] == NULL)
  {
    return(LunState != LunCommandDecode);
  }
  // Get a message
  Int32U Message   = *pScsiMessage[LunInd];
  pInt32U pMessage = pScsiMessage[LunInd]+1;
  // Clear the message pointer
  pScsiMessage[LunInd] = NULL;
  if (Message == LunInitMsg)
  {
    Lun->DiskInitFpnt();
    LunState = LunCommandDecode;
    return(LunState != LunCommandDecode);
  }
  if (Message == LunResetReqMsg)
  {
    LunState = LunCommandDecode;
    return(LunState != LunCommandDecode);
  }
  switch (LunState)
  {
  case LunCommandDecode:
    switch (Message)
    {
    case LunInquiryReqMsg:
      ScsiInquiryData(Lun0Buffer,Lun->DiskInfoFpnt(Lun0Buffer,DiskInquiry));
      break;
    case LunTestUntilReadyReqMsg:
      pDiskCrtl = Lun->DiskStatusFpnt();
      switch (pDiskCrtl->DiskStatus)
      {
      case DiskCommandPass:
        if (pDiskCrtl->MediaChanged)
        {
          ScsiTestUntilReadyData(ScsiMediaChanged);
          pDiskCrtl->MediaChanged = FALSE;
        }
        else
        {
          ScsiTestUntilReadyData(ScsiCommandNoKey);
        }
        break;
      case DiskNotReady:
        ScsiTestUntilReadyData(ScsiMediamNotReady);
        break;
      case DiskNotPresent:
        ScsiTestUntilReadyData(ScsiMediaNotPresent);
        break;
      case DiskChanged:
        ScsiTestUntilReadyData(ScsiMediaChanged);
        break;
      default:
        ScsiTestUntilReadyData(ScsiFatalError);
        break;
      }
      break;
    case LunModeSense6ReqMsg:
      ScsiModeSenseData(Lun->DiskStatusFpnt()->WriteProtect);
      break;
    case LunReadFormatCapacityReqMsg:
      ScsiReadFormatCapcityData(Lun0Buffer,Lun->DiskInfoFpnt(Lun0Buffer,DiskFormatCapacity));
      break;
    case LunReadCapacity10ReqMsg:
      ScsiReadCapacityData(Lun->DiskStatusFpnt()->BlockNumb-1,
                           Lun->DiskStatusFpnt()->BlockSize);
      break;
    case LunRead10ReqMsg:
      BlockStart = *pMessage;
      BlockNum = *++pMessage;
      if ((BlockStart + BlockNum) > Lun->DiskStatusFpnt()->BlockNumb)
      {
        ScsiCmdError(ScsiInvalidCbd,ScsiStallIn);
        break;
      }
      switch(Lun->DiskIoFpnt(Lun0Buffer,BlockStart++,1,DiskRead))
      {
      case DiskCommandPass:
        ScsiReadData(Lun0Buffer,
                     Lun->DiskStatusFpnt()->BlockSize,
                     (--BlockNum == 0));
        if(BlockNum)
        {
          LunState = LunRead;
        }
        break;
      case DiskNotReady:
        // the Media not ready
        ScsiCmdError(ScsiMediamNotReady,ScsiStallIn);
        break;
      case DiskNotPresent:
        // the Media not present
        ScsiCmdError(ScsiMediaNotPresent,ScsiStallIn);
        break;
      default:
        ScsiCmdError(ScsiFatalError,ScsiStallIn);
        break;
      }
      break;
    case LunWrite10ReqMsg:
      BlockStart = *pMessage;
      BlockNum = *++pMessage;

      if ((BlockStart + BlockNum) > Lun->DiskStatusFpnt()->BlockNumb)
      {
        ScsiCmdError(ScsiInvalidCbd,ScsiStallOut);
        break;
      }
      ScsiWriteData(Lun0Buffer,
                    Lun->DiskStatusFpnt()->BlockSize,
                    FALSE);
      LunState = LunWrite;
      break;
    case LunVerify10BytChkReqMsg:
      BlockStart = *pMessage;
      BlockNum = *++pMessage;

      if ((BlockStart + BlockNum) > Lun->DiskStatusFpnt()->BlockNumb)
      {
        ScsiCmdError(ScsiInvalidCbd,ScsiStallOut);
        break;
      }
      ScsiWriteData(Lun0Buffer,
                    Lun->DiskStatusFpnt()->BlockSize,
                    FALSE);
      LunState = LunVerify;
      break;
    case LunVerify10ReqMsg:
      BlockStart = *pMessage;
      BlockNum = *++pMessage;
      if ((BlockStart + BlockNum) > Lun->DiskStatusFpnt()->BlockNumb)
      {
        ScsiCmdError(ScsiInvalidCbd,ScsiStallOut);
        break;
      }
      // Always pass
      ScsiWriteData(NULL,0,TRUE);
      break;
    default:
      // Unknown command
      ScsiCmdError(ScsiUnknowCommand,ScsiStallBoth);
      break;
    }
    break;
  case LunRead:
    if(Message == LunDataReadyMsg)
    {
      switch(Lun->DiskIoFpnt(Lun0Buffer,BlockStart++,1,DiskRead))
      {
      case DiskCommandPass:
        ScsiReadData(Lun0Buffer,
                     Lun->DiskStatusFpnt()->BlockSize,
                     (--BlockNum == 0));
        if(BlockNum == 0)
        {
          LunState = LunCommandDecode;
        }
        break;
      case DiskNotReady:
        // the Media not ready
        ScsiCmdError(ScsiMediamNotReady,ScsiStallIn);
        LunState = LunCommandDecode;
        break;
      case DiskNotPresent:
        // the Media not present
        ScsiCmdError(ScsiMediaNotPresent,ScsiStallIn);
        LunState = LunCommandDecode;
        break;
      default:
        ScsiCmdError(ScsiFatalError,ScsiStallIn);
        LunState = LunCommandDecode;
        break;
      }
    }
    else
    {
      // synchronization lost
      ScsiCmdError(ScsiFatalError,ScsiStallBoth);
      LunState = LunCommandDecode;
    }
    break;
  case LunWrite:
    if(Message == LunDataReadyMsg)
    {
      switch(Lun->DiskIoFpnt(Lun0Buffer,BlockStart++,1,DiskWrite))
      {
      case DiskCommandPass:
        if(--BlockNum == 0)
        {
          ScsiWriteData(NULL,0,TRUE);
          LunState = LunCommandDecode;
        }
        else
        {
        ScsiWriteData(Lun0Buffer,
                      Lun->DiskStatusFpnt()->BlockSize,
                      FALSE);
        }
        break;
      case DiskNotReady:
        // the Media not ready
        ScsiCmdError(ScsiMediamNotReady,ScsiStallOut);
        LunState = LunCommandDecode;
        break;
      case DiskNotPresent:
        // the Media not present
        ScsiCmdError(ScsiMediaNotPresent,ScsiStallOut);
        LunState = LunCommandDecode;
        break;
      default:
        ScsiCmdError(ScsiFatalError,ScsiStallOut);
        LunState = LunCommandDecode;
        break;
      }
    }
    else
    {
      // synchronization lost
      ScsiCmdError(ScsiFatalError,ScsiStallBoth);
      LunState = LunCommandDecode;
    }
    break;
  case LunVerify:
    if(Message == LunDataReadyMsg)
    {
      switch(Lun->DiskIoFpnt(Lun0Buffer,BlockStart++,1,DiskVerify))
      {
      case DiskCommandPass:
        if(--BlockNum == 0)
        {
          ScsiWriteData(NULL,0,TRUE);
          LunState = LunCommandDecode;
        }
        else
        {
        ScsiWriteData(Lun0Buffer,
                      Lun->DiskStatusFpnt()->BlockSize,
                      FALSE);
        }
        break;
      case DiskMiscompareError:
        ScsiCmdError(ScsiMediaNotPresent,ScsiStallOut);
        LunState = LunCommandDecode;
        break;
      case DiskNotReady:
        // the Media not ready
        ScsiCmdError(ScsiMediamNotReady,ScsiStallOut);
        LunState = LunCommandDecode;
        break;
      case DiskNotPresent:
        // the Media not present
        ScsiCmdError(ScsiMediaNotPresent,ScsiStallOut);
        LunState = LunCommandDecode;
        break;
      default:
        ScsiCmdError(ScsiFatalError,ScsiStallOut);
        LunState = LunCommandDecode;
        break;
      }
    }
    else
    {
      // synchronization lost
      ScsiCmdError(ScsiFatalError,ScsiStallBoth);
      LunState = LunCommandDecode;
    }
    break;
  default:
    // something is wrong
    ScsiCmdError(ScsiFatalError,ScsiStallBoth);
    LunState = LunCommandDecode;
    break;
  }
  return(LunState != LunCommandDecode);
}
