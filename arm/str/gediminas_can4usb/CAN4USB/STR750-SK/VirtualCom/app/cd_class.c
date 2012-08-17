/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : cd_class.c
 *    Description : Communication device class module
 *
 *    History :
 *    1. Date        : June 28, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 16170 $
**************************************************************************/
#define CD_CLASS_GLOBAL
#include "cd_class.h"

#pragma data_alignment=4
CDC_LineCoding_t CDC_LineCoding;

#if CDC_DEVICE_SUPPORT_LINE_CODING > 0
volatile Int32U LineCodingDelta;
#endif // CDC_DEVICE_SUPPORT_LINE_CODING > 0

#pragma data_alignment=4
CDC_LineState_t  CDC_LineState;

#if CDC_DEVICE_SUPPORT_LINE_STATE > 0
volatile Int32U SerialStateDelta;

volatile Int32U LineStateDelta;
#endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0

#if CDC_DEVICE_SUPPORT_BREAK > 0
volatile Int32U BreakCntr;
#endif // CDC_DEVICE_SUPPORT_BREAK > 0

#pragma data_alignment=4
UsbSetupPacket_t CdcReqPacket;

Int32U CDC_ReceiveIndx, CDC_ReceiveIndxHold;
Int32U CDC_TransmitSize;
volatile Boolean USB_CDC_OutEpBufferNotEmpty;
pInt8U volatile pCDC_TransmitUserBuffer;

#pragma data_alignment=4
Int8U CDC_ReceiveBuffer[CommOutEpMaxSize];
#pragma data_alignment=4
Int8U CDC_TransmitBuffer[CommInEpMaxSize];

volatile Boolean CDC_Configure;

/*************************************************************************
 * Function Name: Tim1Handler
 * Parameters: void
 *
 * Return: none
 *
 * Description: Timer 1 interrupt handler
 *
 *************************************************************************/
void Tim1Handler(void)
{
#if CDC_DEVICE_SUPPORT_BREAK > 0
  if(BreakCntr)
  {
    --BreakCntr;
  }
  else
  {
    CDC_BreakCallBack(NULL);
    // Disable TIM1 counter
    TIM_Cmd(TIM1, DISABLE);
  }
#endif // CDC_DEVICE_SUPPORT_BREAK > 0
  // Clear the Update pending Bit
  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

/*************************************************************************
 * Function Name: UsbCdcInit
 * Parameters: Int32U IntrPriority
 *
 * Return: none
 *
 * Description: USB communication device class init
 *
 *************************************************************************/
void UsbCdcInit (Int32U IntrPriority)
{
#if CDC_DEVICE_SUPPORT_BREAK > 0
TIM_InitTypeDef TIM_InitStructure;
EIC_IRQInitTypeDef EIC_IRQInitStructure;
MRCC_ClocksTypeDef MRCC_Clocks;
#endif // CDC_DEVICE_SUPPORT_BREAK > 0

  // Init CD Class variables
  CDC_Configure               = FALSE;

  CDC_LineCoding.dwDTERate    = CDC_DATA_RATE;
  CDC_LineCoding.bDataBits    = CDC_DATA_BITS;
  CDC_LineCoding.bParityType  = CDC_PARITY;
  CDC_LineCoding.bCharFormat  = CDC_STOP_BITS;

#if CDC_DEVICE_SUPPORT_LINE_CODING > 0
  // Update the line coding
  LineCodingDelta             = TRUE;
#endif // CDC_DEVICE_SUPPORT_LINE_CODING > 0

  CDC_LineState.DTR_State     = CDC_LINE_DTR;
  CDC_LineState.RTS_State     = CDC_LINE_RTS;

#if CDC_DEVICE_SUPPORT_LINE_STATE > 0
  // Update the line state
  LineStateDelta              = TRUE;
  SerialStateDelta            = FALSE;
#endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0

#if CDC_DEVICE_SUPPORT_BREAK > 0

  BreakCntr = 0;
  // Init Break timer resolution 1 ms
  // Enable TIM1 clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_TIM1, ENABLE);
  // Release TIM1 reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM1,DISABLE);
  // Calculate counter value
  MRCC_GetClocksStatus(&MRCC_Clocks);

  TIM_InitStructure.TIM_Period = (MRCC_Clocks.CKTIM_Frequency / 1000);
  TIM_InitStructure.TIM_Prescaler = 0;
  TIM_InitStructure.TIM_Mode = TIM_Mode_OCTiming;
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_InitStructure.TIM_Channel = TIM_Channel_2;
  TIM_Init(TIM1, &TIM_InitStructure);

  // Enable and configure the priority of the TIM1 Update IRQ Channel
  EIC_IRQInitStructure.EIC_IRQChannel = TIM1_UP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = IntrPriority;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
#endif // CDC_DEVICE_SUPPORT_BREAK > 0

  CDC_ReceiveIndx      = \
  CDC_ReceiveIndxHold  = \
  CDC_TransmitSize     = 0;

  USB_CDC_OutEpBufferNotEmpty = FALSE;

  pCDC_TransmitUserBuffer = NULL;

  // Registered the class request
  USB_UserFuncRegistering(UsbCdcRequest,UsbUserClass);
  // Registered the function for a data receive of the class request
  USB_UserFuncRegistering(UsbCdcData,UsbClassEp0OutPacket);
}

/*************************************************************************
 * Function Name: UsbCdcConfigure
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: USB communication device class configure
 *
 *************************************************************************/
void * UsbCdcConfigure (void * pArg)
{
UsbDevCtrl_t * pUsbDevCtrl = (UsbDevCtrl_t *)pArg;
  if(pUsbDevCtrl == NULL)
  {
    CDC_Configure = FALSE;
    if(UsbCoreReq(UsbCoreReqConfiquration) != 0)
    {
      // disable all class EPs
      USB_RealizeEp((USB_Endpoint_t)CommOutEp,0,EP_BULK_SING_BUFF,EP_SLOT2);
      USB_UserFuncRegistering(NULL,CommOutEp);
      USB_RealizeEp((USB_Endpoint_t)CommInEp,0,EP_BULK_SING_BUFF,EP_SLOT2);
      USB_UserFuncRegistering(NULL,CommInEp);
      USB_RealizeEp((USB_Endpoint_t)ReportEp,0,EP_INTERRUPT,EP_SLOT1);
      USB_UserFuncRegistering(NULL,ReportEp);
    }
  }
  else
  {
    // Init variables
    CDC_ReceiveIndx      = \
    CDC_ReceiveIndxHold  = \
    CDC_TransmitSize     = 0;

    USB_CDC_OutEpBufferNotEmpty = FALSE;

#if CDC_DEVICE_SUPPORT_BREAK > 0
    BreakCntr = 0;
#endif  // CDC_DEVICE_SUPPORT_BREAK > 0

    // enable all class EPs
    USB_UserFuncRegistering((UserFunc_t)UsbCdcInHadler,CommOutEp);
    USB_RealizeEp((USB_Endpoint_t)CommOutEp,CommOutEpMaxSize,EP_BULK_SING_BUFF,EP_SLOT2);
    USB_UserFuncRegistering((UserFunc_t)UsbCdcOutHadler,CommInEp);
    USB_RealizeEp((USB_Endpoint_t)CommInEp,CommInEpMaxSize,EP_BULK_SING_BUFF,EP_SLOT2);
#if CDC_DEVICE_SUPPORT_LINE_STATE > 0
    USB_UserFuncRegistering((UserFunc_t)UsbCdcReportHadler,ReportEp);
    USB_RealizeEp((USB_Endpoint_t)ReportEp,ReportEpMaxSize,EP_INTERRUPT,EP_SLOT1);
#else
    USB_UserFuncRegistering(NULL,ReportEp);
    USB_RealizeEp((USB_Endpoint_t)ReportEp,ReportEpMaxSize,EP_INTERRUPT,EP_SLOT1);
#endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0

    CDC_Configure = TRUE;
  }
  return(NULL);
}

#if CDC_DEVICE_SUPPORT_LINE_STATE > 0
/*************************************************************************
 * Function Name: UsbCdcReportHadler
 * Parameters:  void * pArg
 *
 * Return: none
 *
 * Description: USB Communication Device Class Report (In) EP handler
 *
 *************************************************************************/
void UsbCdcReportHadler (void *Arg)
{
  SerialStateDelta = FALSE;
}
#endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0

/*************************************************************************
 * Function Name: UsbCdcInHadler
 * Parameters:  void * pArg
 *
 * Return: none
 *
 * Description: USB Communication Device Class Out EP handler
 *
 *************************************************************************/
void UsbCdcInHadler (void *Arg)
{
    if(CDC_ReceiveIndx == 0)
    {
      CDC_ReceiveIndx = CommOutEpMaxSize;
      USB_EpRead((USB_Endpoint_t)CommOutEp,
                 (pInt8U)CDC_ReceiveBuffer,
                 &CDC_ReceiveIndx);
      USB_EpValidate((USB_Endpoint_t)CommOutEp,
                     TRUE);
    }
    else
    {
      USB_CDC_OutEpBufferNotEmpty = TRUE;
    }
}

/*************************************************************************
 * Function Name: UsbCdcOutHadler
 * Parameters:  void * pArg
 *
 * Return: none
 *
 * Description: USB Communication Device Class In EP handler
 *
 *************************************************************************/
void UsbCdcOutHadler (void *Arg)
{
Int32U Counter, CurrWriteSize;
  if(CDC_TransmitSize != 0)
  {
    CurrWriteSize = MIN(CDC_TransmitSize,CommInEpMaxSize);
    for(Counter = 0; Counter < CurrWriteSize; ++Counter)
    {
      CDC_TransmitBuffer[Counter] = *pCDC_TransmitUserBuffer++;
    }
    CDC_TransmitSize -= CurrWriteSize;
    USB_EpWrite((USB_Endpoint_t)CommInEp,CDC_TransmitBuffer,&CurrWriteSize);
  }
  else
  {
    pCDC_TransmitUserBuffer = NULL;
  }
}

/*************************************************************************
 * Function Name: UsbCdcRequest
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: The class requests processing
 *
 *************************************************************************/
void * UsbCdcRequest (void * pArg)
{
UsbEpCtrl_t * pCdcReqCtrl = (UsbEpCtrl_t *) pArg;
  CdcReqPacket = *(UsbSetupPacket_t *)pCdcReqCtrl->pData;
  // Validate Request
  if (CdcReqPacket.mRequestType.Recipient == UsbRecipientInterface)
  {
    switch (CdcReqPacket.bRequest)
    {
    case SET_LINE_CODING:
      if ((CdcReqPacket.wValue.Word == 0) &&
          (CdcReqPacket.wIndex.Word == 0))
      {
        pCdcReqCtrl->Counter = MIN(sizeof(CDC_LineCoding_t),
                                  ((CdcReqPacket.wLength.Hi << 8) + CdcReqPacket.wLength.Lo));
        pCdcReqCtrl->pData = (pInt8U)&CDC_LineCoding;
        return((void *)UsbPass);
      }
      break;
    case GET_LINE_CODING:
      if ((CdcReqPacket.wValue.Word == 0) &&
          (CdcReqPacket.wIndex.Word == 0))
      {
        pCdcReqCtrl->Counter = sizeof(CDC_LineCoding_t);
        pCdcReqCtrl->pData   = (pInt8U)&CDC_LineCoding;
        return((void*)UsbPass);
      }
      break;
    case SET_CONTROL_LINE_STATE:
      if ((CdcReqPacket.wLength.Word == 0) &&
          (CdcReqPacket.wIndex.Word == 0))
      {
        CDC_LineState.DTR_State = ((CdcReqPacket.wValue.Lo & 1) != 0);
        CDC_LineState.RTS_State = ((CdcReqPacket.wValue.Lo & 2) != 0);
        // Send AKN
        pCdcReqCtrl->Counter = 0;
#if CDC_DEVICE_SUPPORT_LINE_STATE > 0
        LineStateDelta = TRUE;
#endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0
        return((void*)UsbPass);
      }
      break;

#if CDC_DEVICE_SUPPORT_BREAK > 0
    case SEND_BREAK:
#if CDC_DEVICE_SUPPORT_LINE_STATE > 0
      BreakCntr = CdcReqPacket.wValue.Word;
      LineStateDelta = TRUE;
      if(BreakCntr != 0 && BreakCntr != 0xFFFF)
      {
        // Disable TIM1 counter
        TIM_Cmd(TIM1, DISABLE);
        // Reset TIM1 counter
        TIM_ResetCounter(TIM1);
        // Clear TIM1 flags
        TIM_ClearFlag(TIM1, TIM_FLAG_Update);
        // Enable TIM1 Update interrupt
        TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
        // Enable TIM1 counter
        TIM_Cmd(TIM1, ENABLE);
      }
      // Send AKN
      pCdcReqCtrl->Counter = 0;
#endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0
      return((void*)UsbPass);
#endif // CDC_DEVICE_SUPPORT_BREAK > 0
    }
  }
  CdcReqPacket.wLength.Word = 0;
  return((void *)UsbFault);
}

/*************************************************************************
 * Function Name: UsbCdcData
 * Parameters:  void * pArg
 *
 * Return: void *
 *
 * Description: USB Communication Device Class Data receive
 *
 *************************************************************************/
void * UsbCdcData (void * pArg)
{
  if ((CdcReqPacket.bRequest == SET_LINE_CODING) &&
      (CdcReqPacket.wLength.Word != 0))
  {
#if CDC_DEVICE_SUPPORT_LINE_CODING > 0
    LineCodingDelta = TRUE;
#endif // CDC_DEVICE_SUPPORT_LINE_CODING > 0
    return((void*)UsbPass);
  }
  return((void*)UsbFault);
}

/*************************************************************************
 * Function Name: UsbCdcRead
 * Parameters: pInt8U pBuffer, Int32U Size
 *
 * Return: Int32U
 *
 * Description: USB Communication Device Class data read. Return number
 * of received the bytes.
 *
 *************************************************************************/
Int32U UsbCdcRead (pInt8U pBuffer, Int32U Size)
{
Int32U ReadSize = 0, CurrReadSize = 0, Counter;
FunctionalState cpu_sr;
  while(Size)
  {
    if(CDC_ReceiveIndx)
    {
      CurrReadSize = MIN((CDC_ReceiveIndx - CDC_ReceiveIndxHold), Size);
      Size     -= CurrReadSize;
      ReadSize += CurrReadSize;
      for(Counter = 0; Counter < CurrReadSize; ++Counter)
      {
        *pBuffer++ = CDC_ReceiveBuffer[CDC_ReceiveIndxHold++];
      }
      if(CDC_ReceiveIndxHold == CDC_ReceiveIndx)
      {
        CDC_ReceiveIndxHold = 0;
        if (USB_CDC_OutEpBufferNotEmpty)
        {
          EIC_IRQDisCmd(cpu_sr);
          CDC_ReceiveIndx = CommOutEpMaxSize;
          USB_EpRead((USB_Endpoint_t)CommOutEp,
                      CDC_ReceiveBuffer,
                     &CDC_ReceiveIndx);
          USB_EpValidate((USB_Endpoint_t)CommOutEp,
                         TRUE);
          USB_CDC_OutEpBufferNotEmpty = FALSE;
          EIC_IRQRestoreCmd(cpu_sr);
        }
        else
        {
          CDC_ReceiveIndx = 0;
        }
      }
    }
    else if (USB_CDC_OutEpBufferNotEmpty)
    {
      EIC_IRQDisCmd(cpu_sr);
      CDC_ReceiveIndx = CommOutEpMaxSize;
      USB_EpRead((USB_Endpoint_t)CommOutEp,
                  CDC_ReceiveBuffer,
                 &CDC_ReceiveIndx);
      USB_EpValidate((USB_Endpoint_t)CommOutEp,
                     TRUE);
      USB_CDC_OutEpBufferNotEmpty = FALSE;
      EIC_IRQRestoreCmd(cpu_sr);
    }
    else
    {
      break;
    }
  }
  return(ReadSize);
}

/*************************************************************************
 * Function Name: UsbCdcWrite
 * Parameters:  pInt8U pBuffer, Int32U Size
 *
 * Return: CdcStatus_t
 *
 * Description: USB Communication Device Class data send.
 *
 *************************************************************************/
Boolean UsbCdcWrite (pInt8U pBuffer, Int32U Size)
{
Boolean Status = !UsbCdcIsSendProgess();
Int32U Counter, CurrWriteSize;
FunctionalState cpu_sr;
  if(Status == TRUE)
  {
    pCDC_TransmitUserBuffer = pBuffer;
    CDC_TransmitSize = Size;
    CurrWriteSize = MIN(CDC_TransmitSize,CommInEpMaxSize);
    for(Counter = 0; Counter < CurrWriteSize; ++Counter)
    {
      CDC_TransmitBuffer[Counter] = *pCDC_TransmitUserBuffer++;
    }
    CDC_TransmitSize -= CurrWriteSize;
    EIC_IRQDisCmd(cpu_sr);
    USB_EpWrite((USB_Endpoint_t)CommInEp,CDC_TransmitBuffer,&CurrWriteSize);
    EIC_IRQRestoreCmd(cpu_sr);
  }
  return(Status);
}

/*************************************************************************
 * Function Name: UsbCdcIsSendProgess
 * Parameters:  none
 *
 * Return: Boolean
 *
 * Description: Is all data send?
 *
 *************************************************************************/
Boolean UsbCdcIsSendProgess (void)
{
  return(pCDC_TransmitUserBuffer != NULL);
}

/*************************************************************************
 * Function Name: UsbCdcIsCdcConfigure
 * Parameters:  none
 *
 * Return: Boolean
 *
 * Description:  Is CDC configure yet?
 *
 *************************************************************************/
Boolean UsbCdcIsCdcConfigure (void)
{
  return(CDC_Configure);
}

#if CDC_DEVICE_SUPPORT_LINE_STATE > 0
/*************************************************************************
 * Function Name: UsbCdcIsNewLineStateSettings
 * Parameters:  none
 *
 * Return: Boolean
 *
 * Description: Is there a new modem settings received?
 * RTS and DTR signals
 *
 *************************************************************************/
Boolean UsbCdcIsNewLineStateSettings(void)
{
Boolean StateHold;
  StateHold = (AtomicExchange(FALSE,(pInt32U)&LineStateDelta) != 0);
  return(StateHold);
}

/*************************************************************************
 * Function Name: UsbCdcGetLineStateSettings
 * Parameters:  none
 *
 * Return: CDC_LineState_t
 *
 * Description: Return the Line Signals states structure
 * RTS and DTR signals
 *
 *************************************************************************/
CDC_LineState_t UsbCdcGetLineStateSettings(void)
{
  return(CDC_LineState);
}

/*************************************************************************
 * Function Name: UsbCdcReportSerialCommState
 * Parameters: SerialState_t SerialState
 *
 * Return: none
 *
 * Description: Report the current state of serial communication channel
 * Overrun Error,  Parity Error, Framing Error, Ring Signal, Break,
 * Tx Carrier, Rx Carrier
 *
 *************************************************************************/
void UsbCdcReportSerialCommState(SerialState_t SerialState)
{
FunctionalState cpu_sr;
Int32U Size;
SerialStatePacket_t SerialStatePacket;
  if(CDC_Configure == FALSE)
  {
    return;
  }
  SerialStatePacket.UsbSetupPacket.mRequestType.mRequestTypeData = 0xA1;
  SerialStatePacket.UsbSetupPacket.bRequest = SERIAL_STATE;
  SerialStatePacket.UsbSetupPacket.wValue.Word = 0;
  SerialStatePacket.UsbSetupPacket.wIndex.Word = CDC_DATA_INTERFACE_IND;
  SerialStatePacket.UsbSetupPacket.wLength.Word = sizeof(SerialState_t);
  SerialStatePacket.SerialState = SerialState;

  SerialStateDelta = TRUE;
  Size = sizeof(SerialStatePacket_t);
  // Disable interrupt and save current state of the interrupt flags
  EIC_IRQDisCmd(cpu_sr);
  USB_EpWrite((USB_Endpoint_t)ReportEp,(pInt8U)&SerialStatePacket,&Size);
  EIC_IRQRestoreCmd(cpu_sr);
  while(SerialStateDelta);
}

#endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0

#if CDC_DEVICE_SUPPORT_BREAK > 0
/*************************************************************************
 * Function Name: CDC_BreakCallBack
 * Parameters: void* arg
 *
 * Return: none
 *
 * Description: Break Timer callback routine that is called form the timer
 * interrupt routine after expire of break period.
 *
 *************************************************************************/
void CDC_BreakCallBack (void* arg)
{
  BreakCntr = 0;
}

/*************************************************************************
 * Function Name: UsbCdcGetBreakState
 * Parameters:  none
 *
 * Return: Boolean
 *
 * Description: Return Break event state
 *
 *************************************************************************/
Boolean UsbCdcGetBreakState(void)
{
  return(BreakCntr != 0);
}
#endif // CDC_DEVICE_SUPPORT_BREAK > 0

#if CDC_DEVICE_SUPPORT_LINE_CODING > 0
/*************************************************************************
 * Function Name: UsbCdcIsNewLineCtrlSettings
 * Parameters:  none
 *
 * Return: Boolean
 *
 * Description: Is there a new line settings received?
 * Baud rate, Data bits, Stop bits and Parity
 *
 *************************************************************************/
Boolean UsbCdcIsNewLineCodingSettings(void)
{
Boolean StateHold;
  StateHold = (AtomicExchange(FALSE,(pInt32U)&LineCodingDelta) != 0);
  return(StateHold);
}

/*************************************************************************
 * Function Name: UsbCdcGetLineCodingSettings
 * Parameters:  none
 *
 * Return: CDC_LineCoding_t
 *
 * Description: Return the Line Coding structure
 * Baud rate, Data bits, Stop bits and Parity
 *
 *************************************************************************/
CDC_LineCoding_t UsbCdcGetLineCodingSettings(void)
{
  return(CDC_LineCoding);
}
#endif // CDC_DEVICE_SUPPORT_LINE_CODING > 0
