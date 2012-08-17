/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : scsi.c
 *    Description : USB Mass storage device bulk-only transport and
 *                  SCSI SPC, SBC, MMC commands set
 *
 *    History :
 *    1. Date        : September 8, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *    2. Date        : January 27, 2006
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *        Add Unit Start/Stop command implement
 *    3. Date        : November 20, 2006
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *        Adapt for STR750 (without USB DMA).
 *
 *    $Revision: 16170 $
 **************************************************************************/

#define SCSI_GOBALS
#include "scsi.h"

#pragma data_alignment=4
const Int8U LunMax = SCSI_LUN_NUMB-1;

#pragma data_alignment=4
__no_init Cbw_t Cbw;

#pragma data_alignment=4
__no_init Csw_t Csw;

ScsiDrvObj_t  ScsiDrv [SCSI_LUN_NUMB];
Int32U Lun = 0;

BotState_t  BotState;
BotStatus_t BotStatus;

Boolean RdyMessEna;
volatile ScsiEpStrl_t ScsiInEp, ScsiOutEp;

/*************************************************************************
 * Function Name: ScsiInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init SCSI
 *
 *************************************************************************/
void ScsiInit (void)
{
  UsbClassBotInit();
  // SCSI Drive Objects init
  for (Int32U i = 0; i < SCSI_LUN_NUMB; ++i)
  {
    ScsiDrv[i].Status = ScsiCommandNoKey;
    ScsiDrv[i].UnitSSStatus = UnitStarted;
    // Send init LUN message
    ScsiDrv[i].Message[0] = LunInitMsg;
    pScsiMessage[i] = ScsiDrv[i].Message;
  }
}

/*************************************************************************
 * Function Name: UsbClassBotInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB Bulk-only transport init
 *
 *************************************************************************/
void UsbClassBotInit (void)
{
  UsbBotRstInterface();
  // Registering Configure callback function
  USB_UserFuncRegistering(UsbClassBotRequest,UsbUserClass);
  // Registering Unstall callback function
  USB_UserFuncRegistering(BotUnstallCallBack,UsbUserEpUnStall);
}

/*************************************************************************
 * Function Name: UsbClassBotConfigure
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: USB Class storage configure
 *
 *************************************************************************/
void * UsbClassBotConfigure (void * pArg)
{
  if((Int32U)pArg == 0)
  {
    if(UsbCoreReq(UsbCoreReqConfiquration) != 0)
    {
      // Reset Interface
      UsbBotRstInterface();
      // disable all class EPs
      USB_RealizeEp((USB_Endpoint_t)BulkInEp ,0,EP_BULK_DOUB_BUF,EP_SLOT1);
      USB_UserFuncRegistering(NULL,BulkInEp);
      USB_RealizeEp((USB_Endpoint_t)BulkOutEp,0,EP_BULK_DOUB_BUF,EP_SLOT2);
      USB_UserFuncRegistering(NULL,BulkOutEp);
    }
  }
  else
  {
    // Realize Class EPs
    USB_UserFuncRegistering(BotInEP ,BulkInEp);
    USB_RealizeEp((USB_Endpoint_t)BulkInEp ,BulkInEpMaxSize,EP_BULK_DOUB_BUF,EP_SLOT1);
    USB_UserFuncRegistering(BotOutEP,BulkOutEp);
    USB_RealizeEp((USB_Endpoint_t)BulkOutEp,BulkOutEpMaxSize,EP_BULK_DOUB_BUF,EP_SLOT2);
    // Reset Interface
    UsbBotRstInterface();
  }
  return(NULL);
}

/*************************************************************************
 * Function Name: UsbBotRstInterface
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB Bulk-only transport interface reset
 *
 *************************************************************************/
void UsbBotRstInterface (void)
{
  RdyMessEna = FALSE;
  //BOT Init
  BotState = BotWaitCbw;
  Lun = 0;
  // Clear all flags
  BotStatus.Flags = 0;
  ScsiInEp.Size   = 0;
  // Init receiving of CBW
  ScsiOutEp.Size = sizeof(Cbw_t);
  ScsiOutEp.pData = (pInt8U)&Cbw;
  Csw.dCSWSignature = CswSignature;
  // Imlement data from EP buffers (if they are filled)
  BotOutEP((void *)UsbDataOutPacket);
}

/*************************************************************************
 * Function Name: BotInEpStall
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Stall BOT In EP
 *
 *************************************************************************/
void BotInEpStall (void)
{
  USB_SetStallEP((USB_Endpoint_t)BulkInEp,TRUE);
}

/*************************************************************************
 * Function Name: BotOutEpStall
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Stall BOT Out EP
 *
 *************************************************************************/
void BotOutEpStall (void)
{
  USB_SetStallEP((USB_Endpoint_t)BulkOutEp,TRUE);
}

/*************************************************************************
 * Function Name: UsbClassBotRequest
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: Implement USB Class storage requests
 *
 *************************************************************************/
static void * UsbClassBotRequest (void * pArg)
{
UsbEpCtrl_t * UsbEp = (UsbEpCtrl_t *) pArg;
UsbSetupPacket_t *pData = (UsbSetupPacket_t *)UsbEp->pData;
  switch (pData->bRequest)
  {
  case UsbBotInterfaceGetMaxLun: // return number of LUN
    if ((pData->wValue.Word  == 0) &&
        (pData->wLength.Word == 1))
    {
      UsbEp->pData = (Int8U *)&LunMax;
      UsbEp->Counter = 1;
      return((void*)UsbPass);
    }
    break;
  case MassStorageClassReqReset:  // reset BOT interface
    if ((pData->wValue.Word  == 0) &&
        (pData->wLength.Word == 0))
    {
      UsbBotRstInterface();
      UsbEp->Counter = 0;
      return((void*)UsbPass);
    }
    break;
  }
   return((void *)UsbFault);
}

/*************************************************************************
 * Function Name: BotOutEP
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: USB BOT Out EP handler
 *
 *************************************************************************/
void * BotOutEP (void *pArg)
{
Int32U CurrSize;
  if((Int32U)pArg == UsbDataOutPacket)
  {
    CurrSize = ScsiOutEp.Size;
    if(CurrSize)
    {
      USB_EpRead((USB_Endpoint_t)BulkOutEp,
                 ScsiOutEp.pData,
                 &CurrSize);
      ScsiOutEp.Size -= CurrSize;
      if((ScsiOutEp.Size == 0) ||
         (CurrSize && (CurrSize < BulkOutEpMaxSize)))
      {
        // When all data was received or when short packet was received
        switch (BotState)
        {
        case BotWaitCbw:    // Implement command request
          BotStatus.Flags = 0;
          ScsiCommImpl();
          return(NULL);
        case BotEpDataOut:  // The buffer is full
          ScsiDrv[Lun].Message[0] = LunDataReadyMsg;
          pScsiMessage[Lun]       = ScsiDrv[Lun].Message;
          return(NULL);
        }
      }
      else
      {
        // The buffer is not filled yet
        USB_EpValidate((USB_Endpoint_t)BulkOutEp,TRUE);
        ScsiOutEp.pData += CurrSize;
        return(NULL);
      }
    }
    else
    {
      // data was received but buffer isn't assign yet.
      return(NULL);
    }
  }
  ScsiDrv[Lun].Message[0] = LunResetReqMsg;
  pScsiMessage[Lun]       = ScsiDrv[Lun].Message;
  BotOutEpStall();
  BotInEpStall();
  // Something is wrong
  UsbBotRstInterface();
  return(NULL);
}

/*************************************************************************
 * Function Name: BotInEP
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: USB BOT In EP handler
 *
 *************************************************************************/
void * BotInEP (void *pArg)
{
Int32U CurrSize;
  if((Int32U)pArg == UsbDataInPacket)
  {
    CurrSize = ScsiInEp.Size;
    if(CurrSize)
    {
      // The buffer isn't drained
      USB_EpWrite((USB_Endpoint_t)BulkInEp,ScsiInEp.pData,&CurrSize);
      ScsiInEp.Size  -= CurrSize;
      ScsiInEp.pData += CurrSize;
      return(NULL);
    }
    else
    {
      // The buffer is empty
      switch (BotState)
      {
      case BotEpDataIn:
        if(BotStatus.DataComplete)
        {
          if (Csw.dCSWDataResidue && !BotStatus.ShortPacket)
          {
            // The host expect more data
            BotStatus.ShortPacket = FALSE;
            CurrSize = 0;
            USB_EpWrite((USB_Endpoint_t)BulkInEp,NULL,&CurrSize);
          }
          else
          {
            BotSendStatus();
          }
          return(NULL);
        }
        if(RdyMessEna)
        {
          RdyMessEna = FALSE;
          ScsiDrv[Lun].Message[0] = LunDataReadyMsg;
          pScsiMessage[Lun]       = ScsiDrv[Lun].Message;
        }
        return(NULL);
      case BotWaitCbw:
        return(NULL);
      }
    }
  }
  ScsiDrv[Lun].Message[0] = LunResetReqMsg;
  pScsiMessage[Lun]       = ScsiDrv[Lun].Message;
  BotOutEpStall();
  BotInEpStall();
  // Something is wrong
  UsbBotRstInterface();
  return(NULL);
}

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
void * BotUnstallCallBack (void * pArg)
{
  if((Int32U) pArg == BulkOutEp)
  {
    if (!BotStatus.BotStatus)
    {
      UsbBotRstInterface();
    }
  }
  else if ((Int32U) pArg == BulkInEp)
  {
    if (BotStatus.BotStatus)
    {
      // Send a status after unstall the IN EP
      BotSendStatus();
    }
    else
    {
      UsbBotRstInterface();
    }
  }
  return(NULL);
}

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
void BotWrite(pInt8U pData ,Int32U Size)
{
Int32U CurrSize;
  CurrSize = Size;
  USB_EpWrite((USB_Endpoint_t)BulkInEp,pData,(pInt32U)&CurrSize);
  // Update buffer pointer and remaining data size
  ScsiInEp.pData = pData + CurrSize;
  ScsiInEp.Size = Size - CurrSize;
}

/*************************************************************************
 * Function Name: BotReceiveDataInit
 * Parameters: pInt8U pData, Int32U DataSize, Boolean DataComplete
 *
 * Return: BotToUserStatus_t
 *
 * Description: Init Receive
 *
 *************************************************************************/
BotToUserStatus_t BotReceiveDataInit (pInt8U pData, Int32U DataSize, Boolean DataComplete)
{
FunctionalState cpu_sr;
  if (DataSize > Csw.dCSWDataResidue)
  {
    // When user tries to receive more data from that host expect
    return BotError;
  }
  else if (!DataComplete && (DataSize % BulkInEpMaxSize))
  {
    // when user tries to receive not alignment data to
    // the EP max size and DataComplete flag isn't set
    return BotNotAlignment;
  }
  EIC_IRQDisCmd(cpu_sr);
  // Set BOT EP state
  BotState = BotEpDataOut;
  // Update DataResidue
  Csw.dCSWDataResidue -= DataSize;
  // Init receive of data form BOT OUT EP
  ScsiOutEp.pData = pData;
  ScsiOutEp.Size = DataSize;
  BotOutEP((void *)UsbDataOutPacket);
  if(ScsiOutEp.Size)
  {
    USB_EpValidate((USB_Endpoint_t)BulkOutEp,TRUE);
  }
  EIC_IRQRestoreCmd(cpu_sr);
  return BotPass;
}

/*************************************************************************
 * Function Name: BotSendDataInit
 * Parameters:  const pInt8U pData, Int32U DataSize, Boolean DataComplete
 *
 * Return: BotToUserStatus_t
 *
 * Description: Init Transmit
 *
 *************************************************************************/
BotToUserStatus_t BotSendDataInit (const pInt8U pData, Int32U DataSize, Boolean DataComplete)
{
FunctionalState cpu_sr;
  if (DataSize > Csw.dCSWDataResidue)
  {
    // When user tries to send more data from that host expect
    BotStatus.PhaseError = TRUE;
    DataSize = 0;
    return BotError;
  }
  else if (DataSize % BulkInEpMaxSize)
  {
    BotStatus.ShortPacket = TRUE;
    if(!DataComplete)
    {
      // when user tries to receive not alignment data to
      // the EP max size and DataComplete flag isn't set
      return BotNotAlignment;
    }
  }
  EIC_IRQDisCmd(cpu_sr);
  // Set BOT EP state
  BotState = BotEpDataIn;
  BotStatus.DataComplete = DataComplete;
  // Update DataResidue
  Csw.dCSWDataResidue -= DataSize;
  // Init Transfer to BOT IN EP
  BotWrite(pData,DataSize);
  RdyMessEna = TRUE;
  EIC_IRQRestoreCmd(cpu_sr);
  return BotPass;
}

/*************************************************************************
 * Function Name: BotSendStatus
 * Parameters:  Int32U DataSize, Int8U * pData, Boolean DataComplete
 *
 * Return: BotToUserStatus_t
 *
 * Description: Init Transmit of the CSW
 *
 *************************************************************************/
void BotSendStatus (void)
{
FunctionalState cpu_sr;
  USB_EpValidate((USB_Endpoint_t)BulkOutEp,TRUE);
  EIC_IRQDisCmd(cpu_sr);
  BotState = BotSendCsw;
  // check for some errors
  if(BotStatus.PhaseError)
  {
    Csw.bCSWStatus = CswPhaseError;
  }
  else if (BotStatus.CmdFault)
  {
    Csw.bCSWStatus = CswFailed;
  }
  else
  {
    Csw.bCSWStatus = CswPassed;
  }
  BotWrite((pInt8U)&Csw,sizeof(Csw_t));
  UsbBotRstInterface();
  EIC_IRQRestoreCmd(cpu_sr);
}

/*************************************************************************
 * Function Name: ScsiCommImpl
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Scsi commands implement
 *
 *************************************************************************/
inline
void ScsiCommImpl (void)
{
  if(ScsiCbwValid())
  {
    Csw.dCSWDataResidue = Cbw.dCBWDataTransferLength;
    Csw.dCSWTag = Cbw.dCBWTag;
    Lun = Cbw.bCBWLUN;
    switch (Cbw.CBWCB[0])
    {
    case ScsiTestUnitReady:
      ScsiTestUnitReadyImp();
      return;
    case ScsiRequestSense:
      ScsiRequestSenseImp();
      return;
    case ScsiInquiry:
      ScsiInquiryImp();
      return;
    case ScsiModeSelect6:
      ScsiModeSelect6Imp();
      return;
    case ScsiModeSense6:
      ScsiModeSense6Imp();
      return;
    case ScsiMediaStartStop:
      ScsiMediaStartStopImp();
      return;
    case ScsiMediaRemoval:
      ScsiMediaRemovalImp();
      return;
    case ScsiReadFormatCapcity:
      ScsiReadFormatCapcityImp();
      return;
    case ScsiReadCapacity10:
      ScsiReadCapacity10Imp();
      return;
    case ScsiRead10:
      ScsiRead10Imp();
      return;
    case ScsiWrite10:
      ScsiWrite10Imp();
      return;
    case ScsiFerify10:
      ScsiVerify10Imp();
      return;
    default:
      ScsiCmdError(ScsiUnknowCommand,
       (Cbw.bmCBWFlags.Dir == BotDataFormDevToHost)?ScsiStallIn:ScsiStallOut);
    }
    return;
  }
  // CBW Not valid or not meaningful
  BotInEpStall();
  BotOutEpStall();
}

/*************************************************************************
 * Function Name: ScsiCbwValid
 * Parameters:  none
 *
 * Return: Boolean
 *
 * Description: CBW valid and meaningful check
 *
 *************************************************************************/
inline
Boolean ScsiCbwValid(void)
{
ScsiCmdField_t ScsiCmdField;
  if ((Cbw.dCBWSignature != CbwSignature) ||
      (Cbw.bCBWLUN >= SCSI_LUN_NUMB) ||
      BotStatus.BotStatus)
  {
    return(FALSE);
  }
  ScsiCmdField.Cmd = Cbw.CBWCB[0];
  switch (ScsiCmdField.GroupCode)
  {
  case 0:
    if(Cbw.bCBWCBLength >= 6)
      return(TRUE);
    break;
  case 1:
  case 2:
    if(Cbw.bCBWCBLength >= 10)
      return(TRUE);
    break;
  case 3:
    return(TRUE);
  case 4:
    if(Cbw.bCBWCBLength >= 16)
      return(TRUE);
    break;
  case 5:
    if(Cbw.bCBWCBLength >= 12)
      return(TRUE);
  }
  return(FALSE);
}

/*************************************************************************
 * Function Name: ScsiTestUnitReadyImp
 * Parameters: none
 *
 * Return: none
 *
 * Description: TEST UNIT READY command implement
 *
 *************************************************************************/
inline
void ScsiTestUnitReadyImp (void)
{
  if(Cbw.dCBWDataTransferLength == 0)
  {
    ScsiDrv[Lun].Message[0] = LunTestUntilReadyReqMsg;
    pScsiMessage[Lun] = ScsiDrv[Lun].Message;
  }
  else
  {
    ScsiCmdError(ScsiFatalError,0);
  }
}

/*************************************************************************
 * Function Name: ScsiTestUntilReadyData
 * Parameters:  Int32U MediaReady
 *
 * Return: void
 *
 * Description: Prepare Test until ready data for sending
 *
 *************************************************************************/
void ScsiTestUntilReadyData (Int32U MediaReady)
{
  if(ScsiDrv[Lun].UnitSSStatus == UnitEjected)
  {
     if(MediaReady != ScsiMediaChanged)
     {
        ScsiCmdStatus(ScsiMediaNotPresent);
        BotSendStatus();
        return;
     }
     ScsiDrv[Lun].UnitSSStatus = UnitStarted;
  }
  ScsiCmdStatus((ScsiStatusCode_t)MediaReady);
  BotSendStatus();
}

/*************************************************************************
 * Function Name: ScsiRequestSenseImp
 * Parameters:  none
 *
 * Return: Boolean
 *
 * Description: REQUEST SENSE command implement
 *
 *************************************************************************/
inline
void ScsiRequestSenseImp (void)
{
pSpc3RequestSense_t pRequestSense = (pSpc3RequestSense_t)Cbw.CBWCB;
pSpc3RequestSenseResponse_t pRequestSenseResponse;
  if ((Cbw.bmCBWFlags.Dir == BotDataFormDevToHost) &&
      (Cbw.dCBWDataTransferLength >= pRequestSense->AllocationLenght))
  {
    pRequestSenseResponse = (pSpc3RequestSenseResponse_t)&Cbw;
    // clear buffer
    memset(&Cbw,0,sizeof(Spc3RequestSenseResponse_t));
    if ((pRequestSense->Control.Control == 0) && !pRequestSense->DESC)
    {
      pRequestSenseResponse->ResponceCode = 0x70;
      pRequestSenseResponse->Valid = TRUE;
      switch (ScsiDrv[Lun].Status)
      {
      case ScsiMediamNotReady:
        pRequestSenseResponse->Ascq = LogicalUnitNotReadyCauseNotReportable >> 8;
        pRequestSenseResponse->Asc  = LogicalUnitNotReadyCauseNotReportable;
        pRequestSenseResponse->SenseKey  = NotReady;
        break;
      case ScsiInvalidCbd:
        pRequestSenseResponse->Ascq = InvalidFieldInCdb >> 8;
        pRequestSenseResponse->Asc  = InvalidFieldInCdb;
        pRequestSenseResponse->SenseKey   = IllegalRequest;
        break;
      case ScsiUnknowCommand:
        pRequestSenseResponse->Ascq = InvalidCommandOperationCode >> 8;
        pRequestSenseResponse->Asc  = InvalidCommandOperationCode;
        pRequestSenseResponse->SenseKey   = IllegalRequest;
        break;
      case ScsiMediaNotPresent:
        pRequestSenseResponse->Ascq = MediumNotPresent >> 8;
        pRequestSenseResponse->Asc  = MediumNotPresent;
        pRequestSenseResponse->SenseKey   = NotReady;
        break;
      case ScsiMiscompare:
        pRequestSenseResponse->Ascq = MiscompareDuringVerifyOperation >> 8;
        pRequestSenseResponse->Asc  = MiscompareDuringVerifyOperation;
        pRequestSenseResponse->SenseKey   = Miscompare;
        break;
      case ScsiMediaChanged:
        pRequestSenseResponse->Ascq = NotReadyToReadyChangeMediumMayHaveChanged >> 8;
        pRequestSenseResponse->Asc  = NotReadyToReadyChangeMediumMayHaveChanged;
        pRequestSenseResponse->SenseKey   = UnitAttention;
        break;
      default:
        pRequestSenseResponse->Ascq = NoAdditionalSenseInformation >> 8;
        pRequestSenseResponse->Asc  = NoAdditionalSenseInformation;
        pRequestSenseResponse->SenseKey   = NoSense;
        break;
      }
      pRequestSenseResponse->AddSenseKeyLength = sizeof(Spc3RequestSenseResponse_t) - 8;
      // Clear condition code info
      ScsiDrv[Lun].Status = ScsiCommandNoKey;
      // Send Status after data packet
      BotSendDataInit((Int8U *)pRequestSenseResponse,
                      MIN(pRequestSense->AllocationLenght,sizeof(Spc3RequestSenseResponse_t)),
                      TRUE);
      return;
    }
    else
    {
      ScsiCmdStatus(ScsiInvalidCbd);
    }
  }
  else
  {
    ScsiCmdStatus(ScsiFatalError);
  }
  BotStatus.BotStatus = TRUE;
  BotInEpStall();
}

/*************************************************************************
 * Function Name: ScsiModeSelect6Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: MODE SELECT command implement
 *
 *************************************************************************/
inline
void ScsiModeSelect6Imp (void)
{
pSpc3ModeSelect6_t pModeSelect = (pSpc3ModeSelect6_t)Cbw.CBWCB;
  if ((Cbw.bmCBWFlags.Dir == BotDataFormHostToDev) &&
      (Cbw.dCBWDataTransferLength >= pModeSelect->ParameterListLenght))
  {
    ScsiCmdStatus(ScsiInvalidCbd);
  }
  else
  {
    ScsiCmdStatus(ScsiFatalError);
    BotInEpStall();
  }
  BotOutEpStall();
  BotSendStatus();
}

/*************************************************************************
 * Function Name: ScsiModeSense6Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: MODE SENSE command implement
 *
 *************************************************************************/
inline
void ScsiModeSense6Imp (void)
{
pSpc3ModeSense6_t pModeSense = (pSpc3ModeSense6_t)Cbw.CBWCB;
  if ((Cbw.bmCBWFlags.Dir == BotDataFormDevToHost) &&
      (Cbw.dCBWDataTransferLength >= pModeSense->AllocationLenght))
  {
    if ((pModeSense->PageCode == 0x3F) && (pModeSense->PC == 0) &&
        (pModeSense->SubPageCode   == 0x00))
    {
      // Clear condition code info
      ScsiDrv[Lun].Status = ScsiCommandNoKey;
      ScsiDrv[Lun].Message[0] = LunModeSense6ReqMsg;
      pScsiMessage[Lun] = ScsiDrv[Lun].Message;
      return;
    }
    else
    {
      ScsiCmdStatus(ScsiInvalidCbd);
    }
  }
  else
  {
    ScsiCmdStatus(ScsiFatalError);
  }
  BotStatus.BotStatus = TRUE;
  BotInEpStall();
}

/*************************************************************************
 * Function Name: ScsiModeSenseData
 * Parameters:  Int32U WriteProtect
 *
 * Return: none
 *
 * Description: Prepare MODE SENSE data for sending
 *
 *************************************************************************/
void ScsiModeSenseData (Int32U WriteProtect)
{
pSpc3ModeSense6Response_t pModeSense = (pSpc3ModeSense6Response_t)&Cbw;
  // clear buffer
  memset(pModeSense,0,sizeof(Spc3ModeSense6Response_t));
  pModeSense->ModeDataLength = sizeof(Spc3ModeSense6Response_t)-1;
  pModeSense->WP = WriteProtect;
  BotSendDataInit((Int8U *)pModeSense,
                  MIN(Csw.dCSWDataResidue,sizeof(Spc3ModeSense6Response_t)),
                  TRUE);
}

/*************************************************************************
 * Function Name: ScsiInquiryImp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: INQUIRY command implement
 *
 *************************************************************************/
inline
void ScsiInquiryImp (void)
{
pSpc3Inquiry_t pInquiry = (pSpc3Inquiry_t)Cbw.CBWCB;
Int32U AllocationLenght =
  ((Int32U)pInquiry->AllocationLenght[0]<<8) + pInquiry->AllocationLenght[1];
  if ((Cbw.bmCBWFlags.Dir == BotDataFormDevToHost) &&
      (Cbw.dCBWDataTransferLength >= AllocationLenght))
  {
    if ((!pInquiry->EVPD) && (pInquiry->PageCode == 0))
    {
      ScsiDrv[Lun].Message[0] = LunInquiryReqMsg;
      pScsiMessage[Lun] = ScsiDrv[Lun].Message;
      return;
    }
    else
    {
      ScsiCmdStatus(ScsiInvalidCbd);
    }
  }
  else
  {
   ScsiCmdStatus(ScsiFatalError);
  }
  BotStatus.BotStatus = TRUE;
  BotInEpStall();
}

/*************************************************************************
 * Function Name: ScsiInquiryData
 * Parameters: const pInt8U pData, Int32U Size
 *
 * Return: none
 *
 * Description: Prepare INQUIRY data for sending
 *
 *************************************************************************/
void ScsiInquiryData (const pInt8U pData, Int32U Size)
{
  BotSendDataInit(pData,MIN(Csw.dCSWDataResidue,Size),TRUE);
}

/*************************************************************************
 * Function Name: ScsiMediaRemovalImp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: PREVENT ALLOW MEDIUM REMOVAL command implement
 *
 *************************************************************************/
inline
void ScsiMediaRemovalImp(void)
{
pSpc3MediaRemoval_t pMediaRemoval = (pSpc3MediaRemoval_t)Cbw.CBWCB;
  if (Cbw.dCBWDataTransferLength == 0)
  {
    if ((pMediaRemoval->Prevent == 0) || (pMediaRemoval->Prevent == 1))
    {
      // Clear condition code info
      ScsiCmdStatus(ScsiCommandNoKey);
      BotSendStatus();
    }
    else
    {
      ScsiCmdStatus(ScsiInvalidCbd);
      BotSendStatus();
    }
    return;
  }
  else
  {
    ScsiCmdStatus(ScsiFatalError);
  }
  UsbBotRstInterface();
}

/*************************************************************************
 * Function Name: ScsiReadCapacity10Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: READ CAPACITY (10) command implement
 *
 *************************************************************************/
inline
void ScsiReadCapacity10Imp (void)
{
  if ((Cbw.bmCBWFlags.Dir == BotDataFormDevToHost) &&
      (Cbw.dCBWDataTransferLength >= sizeof(Sbc2ReadCapacity10Response_t)))
  {
    ScsiDrv[Lun].Message[0] = LunReadCapacity10ReqMsg;
    pScsiMessage[Lun] = ScsiDrv[Lun].Message;
    // Clear condition code info
    ScsiDrv[Lun].Status = ScsiCommandNoKey;
    return;
  }
  else
  {
    ScsiCmdStatus(ScsiFatalError);
  }
  BotStatus.BotStatus = TRUE;
  BotInEpStall();
}

/*************************************************************************
 * Function Name: ScsiReadCapacityData
 * Parameters:  Int32U LogBlockAdd, Int32U BlockSize
 *
 * Return: none
 *
 * Description: Prepare Read capacity data for sending
 *
 *************************************************************************/
void ScsiReadCapacityData (Int32U LogBlockAdd, Int32U BlockSize)
{
pSbc2ReadCapacity10Response_t pReadCapacity = (pSbc2ReadCapacity10Response_t)&Cbw;

  // clear buffer
  memset(pReadCapacity,0,sizeof(Sbc2ReadCapacity10Response_t));

  pReadCapacity->LogicalBlockAddress[0] = (LogBlockAdd >> 24) & 0xFF;
  pReadCapacity->LogicalBlockAddress[1] = (LogBlockAdd >> 16) & 0xFF;
  pReadCapacity->LogicalBlockAddress[2] = (LogBlockAdd >> 8 ) & 0xFF;
  pReadCapacity->LogicalBlockAddress[3] = (LogBlockAdd >> 0 ) & 0xFF;

  pReadCapacity->BlockLength[0]         = (BlockSize   >> 24) & 0xFF;
  pReadCapacity->BlockLength[1]         = (BlockSize   >> 16) & 0xFF;
  pReadCapacity->BlockLength[2]         = (BlockSize   >> 8 ) & 0xFF;
  pReadCapacity->BlockLength[3]         = (BlockSize   >> 0 ) & 0xFF;

  BotSendDataInit((Int8U *)pReadCapacity,
                  MIN(Csw.dCSWDataResidue,sizeof(Sbc2ReadCapacity10Response_t)),
                  TRUE);
}

/*************************************************************************
 * Function Name: ScsiRead10Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: READ (10) command implement
 *
 *************************************************************************/
inline
void ScsiRead10Imp (void)
{
pSbc2Read10_t pRead = (pSbc2Read10_t)Cbw.CBWCB;
  if (Cbw.bmCBWFlags.Dir == BotDataFormDevToHost)
  {
    if(ScsiDrv[Lun].UnitSSStatus == UnitEjected)
    {
      ScsiCmdStatus(ScsiMediaNotPresent);
    }
    else
    {
      // Clear condition code info
      ScsiDrv[Lun].Status = ScsiCommandNoKey;
      ScsiDrv[Lun].Message[2] = ((Int32U)pRead->TransferLength[0] << 8) + pRead->TransferLength[1];
      if(ScsiDrv[Lun].Message[2] != 0)
      {
        ScsiDrv[Lun].Message[0] = LunRead10ReqMsg;
        ScsiDrv[Lun].Message[1] = ((Int32U)pRead->LogicalBockAddress[0] << 24) +\
                                  ((Int32U)pRead->LogicalBockAddress[1] << 16) +\
                                  ((Int32U)pRead->LogicalBockAddress[2] << 8 ) +\
                                  pRead->LogicalBockAddress[3];
        pScsiMessage[Lun] = ScsiDrv[Lun].Message;
      }
      else
      {
        BotSendStatus();
      }
      return;
    }
  }
  else
  {
    ScsiCmdStatus(ScsiFatalError);
  }
  BotStatus.BotStatus = TRUE;
  BotInEpStall();
}

/*************************************************************************
 * Function Name: ScsiReadData
 * Parameters: const pInt8U pData, Int32U Size, Boolean DataComplete
 *
 * Return: none
 *
 * Description: Prepare Read data for sending
 *
 *************************************************************************/
void ScsiReadData(const pInt8U pData, Int32U Size, Boolean DataComplete)
{
  BotSendDataInit(pData,Size,DataComplete);
}

/*************************************************************************
 * Function Name: ScsiWrite10Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Write (10) command implement
 *
 *************************************************************************/
inline
void ScsiWrite10Imp (void)
{
pSbc2Write10_t pWrite = (pSbc2Write10_t)Cbw.CBWCB;
  if (Cbw.bmCBWFlags.Dir == BotDataFormHostToDev)
  {
    if(ScsiDrv[Lun].UnitSSStatus == UnitEjected)
    {
      ScsiCmdStatus(ScsiMediaNotPresent);
    }
    else
    {
      // Clear condition code info
      ScsiDrv[Lun].Status = ScsiCommandNoKey;
      ScsiDrv[Lun].Message[2] = ((Int32U)pWrite->TransferLength[0] << 8) + pWrite->TransferLength[1];
      if(ScsiDrv[Lun].Message[2] != 0)
      {
        ScsiDrv[Lun].Message[0] = LunWrite10ReqMsg;
        ScsiDrv[Lun].Message[1] = ((Int32U)pWrite->LogicalBockAddress[0] << 24) +\
                                  ((Int32U)pWrite->LogicalBockAddress[1] << 16) +\
                                  ((Int32U)pWrite->LogicalBockAddress[2] << 8 ) +\
                                  pWrite->LogicalBockAddress[3];
        pScsiMessage[Lun] = ScsiDrv[Lun].Message;
        BotState = BotEpDataOut;
        return;
      }
      else
      {
        BotSendStatus();
      }
      return;
    }
  }
  else
  {
    ScsiCmdStatus(ScsiFatalError);
  }
  BotSendStatus();
  BotOutEpStall();
}

/*************************************************************************
 * Function Name: ScsiWriteData
 * Parameters:  pInt8U pData, Int32U Size, Boolean DataComplete
 *
 * Return: none
 *
 * Description: Prepare Write data for receiving
 *
 *************************************************************************/
void ScsiWriteData(pInt8U pData, Int32U Size, Boolean DataComplete)
{
  if(DataComplete)
  {
    BotSendStatus();
  }
  else
  {
    BotReceiveDataInit(pData,Size,DataComplete);
  }
}

/*************************************************************************
 * Function Name: ScsiVerify10Imp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Verify (10) command implement
 *
 *************************************************************************/
void ScsiVerify10Imp (void)
{
pSbc2Verify10_t pVerify = (pSbc2Verify10_t)Cbw.CBWCB;
  if (Cbw.bmCBWFlags.Dir == BotDataFormHostToDev)
  {
    if(ScsiDrv[Lun].UnitSSStatus == UnitEjected)
    {
      ScsiCmdStatus(ScsiMediaNotPresent);
    }
    else
    {
      // Clear condition code info
      ScsiDrv[Lun].Status = ScsiCommandNoKey;
      ScsiDrv[Lun].Message[2] = ((Int32U)pVerify->TransferLength[0] << 8) + pVerify->TransferLength[1];
      if(ScsiDrv[Lun].Message[2] != 0)
      {
        ScsiDrv[Lun].Message[1] = ((Int32U)pVerify->LogicalBockAddress[0] << 24) +\
                                  ((Int32U)pVerify->LogicalBockAddress[1] << 16) +\
                                  ((Int32U)pVerify->LogicalBockAddress[2] << 8 ) +\
                                  pVerify->LogicalBockAddress[3];
        ScsiDrv[Lun].Message[0] = (pVerify->BYTCHK)?LunVerify10BytChkReqMsg:LunVerify10ReqMsg;
        pScsiMessage[Lun] = ScsiDrv[Lun].Message;
        BotState = BotEpDataOut;
        return;
      }
      else
      {
        BotSendStatus();
      }
      return;
    }
  }
  else
  {
    ScsiCmdStatus(ScsiFatalError);
  }
  BotSendStatus();
  BotOutEpStall();
}

/*************************************************************************
 * Function Name: ScsiReadFormatCapcityImp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: READ FORMAT CAPACITIES implement
 *
 *************************************************************************/
void ScsiReadFormatCapcityImp (void)
{
  if (Cbw.bmCBWFlags.Dir == BotDataFormDevToHost)
  {
    // Clear condition code info
    ScsiDrv[Lun].Status = ScsiCommandNoKey;
    ScsiDrv[Lun].Message[0] = LunReadFormatCapacityReqMsg;
    pScsiMessage[Lun] = ScsiDrv[Lun].Message;
    return;
  }
  else
  {
    ScsiCmdStatus(ScsiFatalError);
  }
  BotStatus.BotStatus = TRUE;
  BotInEpStall();
}

/*************************************************************************
 * Function Name: ScsiReadFormatCapcityData
 * Parameters:  pInt8U pData, Int32U Size
 *
 * Return: none
 *
 * Description: Prepare Data of READ FORMAT CAPACITIES command request
 *
 *************************************************************************/
void ScsiReadFormatCapcityData (pInt8U pData, Int32U Size)

{
pMmc3ReadFormatCapacity_t pFormatCapacity = (pMmc3ReadFormatCapacity_t)Cbw.CBWCB;
Int32U AllocationLength = (pFormatCapacity->AllocationLenght[0]<<8) +\
                           pFormatCapacity->AllocationLenght[1];
  BotSendDataInit(pData,MIN(Size,AllocationLength),TRUE);
}

/*************************************************************************
 * Function Name: ScsiCmdStatus
 * Parameters:  ScsiStatusCode_t Status
 *
 * Return: none
 *
 * Description: Prepare Status data for sending
 *
 *************************************************************************/
void ScsiCmdStatus (ScsiStatusCode_t Status)
{
  ScsiDrv[Lun].Status = Status;
  switch (Status)
  {
  case ScsiCommandNoKey:
    break;
  case ScsiMediamNotReady:
  case ScsiInvalidCbd:
  case ScsiUnknowCommand:
  case ScsiMediaNotPresent:
  case ScsiMediaChanged:
  case ScsiMiscompare:
    BotStatus.CmdFault = TRUE;
    break;
  default:
    BotStatus.PhaseError = TRUE;
  }
}

/*************************************************************************
 * Function Name: ScsiCmdError
 * Parameters:  Int32U Status, Int32U Stall
 *
 * Return: none
 *
 * Description: Prepare Error data for sending
 *
 *************************************************************************/
void ScsiCmdError (Int32U Status, Int32U Stall)
{
  if(Stall & ScsiStallIn)
  {
    BotStatus.BotStatus = TRUE;
    BotInEpStall();
  }
  if (Stall & ScsiStallOut)
  {
    BotOutEpStall();
  }
  ScsiCmdStatus((ScsiStatusCode_t)Status);
  if(!(Stall & ScsiStallIn))
  {
    BotSendStatus();
  }
}

/*************************************************************************
 * Function Name: ScsiMediaStartStopImp
 * Parameters: none
 *
 * Return: none
 *
 * Description: START STOP UNIT command implement
 *
 *************************************************************************/
inline
void ScsiMediaStartStopImp (void)
{
pSpc3UnitStartStop_t pMediaStartStop = (pSpc3UnitStartStop_t)Cbw.CBWCB;
  if (Cbw.dCBWDataTransferLength == 0)
  {
    if (pMediaStartStop->PC == 0)
    {
      if(pMediaStartStop->LOEJ)
      {
        if(pMediaStartStop->START)
        {
          ScsiDrv[Lun].UnitSSStatus = UnitStarted;
        }
        else
        {
          ScsiDrv[Lun].UnitSSStatus = UnitEjected;
        }
      }
      // Clear condition code info
      ScsiCmdStatus(ScsiCommandNoKey);
      BotSendStatus();
    }
    else
    {
      ScsiCmdStatus(ScsiInvalidCbd);
      BotSendStatus();
    }
    return;
  }
  else
  {
    ScsiCmdStatus(ScsiFatalError);
  }
  UsbBotRstInterface();
}

