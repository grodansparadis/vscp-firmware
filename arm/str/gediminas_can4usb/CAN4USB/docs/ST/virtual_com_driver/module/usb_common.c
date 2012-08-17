/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : usb_common.c
 *    Description : USB common subroutines
 *
 *    History :
 *    1. Date        : August 25, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *    2. Date        : January 16, 2007
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *      Add log messages
 *
 *    $Revision: 16170 $
 **************************************************************************/

/*************************************************************************
 * Function Name: USB_UserFuncRegistering
 * Parameters: UserFunc_t UserFunc, UsbUserEvent_t UserFuncInd
 *
 * Return: UserFunc_t
 *
 * Description: Registered User callback function
 *
 *************************************************************************/
UserFunc_t USB_UserFuncRegistering (UserFunc_t UserFunc, UsbUserEvent_t UserFuncInd)
{
UserFunc_t PrevUserFunc = UsbUserFun[UserFuncInd];

  USB_T9M_TRACE_INFO("T9M USB_UserFuncRegistering() index - %d, address 0x%08x\n\r",
                     UserFuncInd,UserFunc);
  UsbUserFun[UserFuncInd] = (UserFunc_t) UserFunc;
  return (PrevUserFunc);
}

/*************************************************************************
 * Function Name: UsbCoreReq
 * Parameters:  UsbCoreReqType_t Type
 *
 * Return: Int32U
 *
 * Description: Return device states
 *
 *************************************************************************/
Int32U UsbCoreReq (UsbCoreReqType_t Type)
{
  switch(Type)
  {
  case UsbCoreReqDevState:
    return(UsbDevCtrl.State.DS);
  case UsbCoreReqDevSusState:
    return(UsbDevCtrl.State.Suspend);
  case UsbCoreReqConfiquration:
    return(UsbDevCtrl.Configuration);
  case UsbCoreReqInterface:
    return(UsbDevCtrl.Interface);
  case UsbCoreReqAlternateSetting:
    return(UsbDevCtrl.AlternateSetting);
  case UsbCoreReqDevOwnAddress:
    return(UsbDevCtrl.DevAdd);
  case UsbCoreReqWakeUpEnableStatus:
    return(UsbDevCtrl.Feature.RemoteWakeupEnable);
  case UsbCoreReqSelfPoweredStatus:
    return(UsbDevCtrl.Feature.SelfPowered);
  }
  return((Int32U) -1);
}

#if USB_REMOTE_WAKEUP != 0
/*************************************************************************
 * Function Name: UsbWakeUp
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Wake-up device from suspend mode
 *
 *************************************************************************/
void UsbWakeUp (void)
{
  USB_T9M_TRACE_INFO("T9M UsbWakeUp()\n\r");
  if(UsbDevCtrl.Feature.RemoteWakeupEnable && UsbDevCtrl.State.Suspend)
  {
    USB_WakeUp();
  }
}
#endif // USB_REMOTE_WAKEUP != 0

/*************************************************************************
 * Function Name: UsbSetDevState
 * Parameters:  UsbT9DevState_t DevState
 *
 * Return: none
 *
 * Description: Set device state
 *
 *************************************************************************/
static
void UsbSetDevState (UsbT9DevState_t DevState)
{
  UsbDevCtrl.State.DS = DevState;
  USB_T9M_TRACE_INFO("T9M UsbSetDevState() state - %d\n\r",DevState);
  switch (DevState)
  {
  case UsbDevStatusAttached:
  case UsbDevStatusPowered:
  case UsbDevStatusDefault:
    USB_Configure(FALSE);
    USB_SetDefAdd();  // Set address 0
    UsbUserFun[UsbUserConfigure](NULL);
    UsbDevCtrl.DevAdd = UsbDevCtrl.Configuration = \
    UsbDevCtrl.AlternateSetting = UsbDevCtrl.Interface = 0;
    break;
  case UsbDevStatusAddress:
    USB_Configure(FALSE);
    UsbDevCtrl.Configuration = UsbDevCtrl.AlternateSetting =\
    UsbDevCtrl.Interface = 0;
    break;
  case UsbDevStatusConfigured:
    USB_Configure(TRUE);
    break;
  default:  // unknow state - disconnected or in process of a connection
    USB_Configure(FALSE);
    UsbUserFun[UsbUserConfigure](NULL);
    UsbDevCtrl.DevAdd = UsbDevCtrl.Configuration = \
    UsbDevCtrl.AlternateSetting = UsbDevCtrl.Interface = 0;
  }
}

/*************************************************************************
 * Function Name: UsbSetDevSuspend
 * Parameters:  none
 *
 * Return: UsbDevState_t
 *
 * Description: Set device suspend mode
 *
 *************************************************************************/
static void UsbSetDevSuspend (Boolean Suspend)
{
  UsbDevCtrl.State.Suspend = Suspend ? UsbDevStatusSuspend : UsbDevStatusNoSuspend;
}

/*************************************************************************
 * Function Name: UsbFindInterface
 * Parameters:  UsbStandardConfigurationDescriptor_t * pConfiguration
 *              Int32U Interface,Int32U AlternativeSetting
 *
 * Return: UsbCommStatus_t
 *
 * Description: Set Interface and  Alternative setting state
 *
 *************************************************************************/
static
UsbCommStatus_t UsbFindInterface (UsbStandardConfigurationDescriptor_t * pConfiguration,
                                  UsbDevCtrl_t * UsbDev)
{
UsbStandardInterfaceDescriptor_t *pData;
  // Find Interface
  for( pData = (UsbStandardInterfaceDescriptor_t *)pConfiguration;
      (Int8U*)pData < (Int8U *)pConfiguration + pConfiguration->wTotalLength;
       pData =(UsbStandardInterfaceDescriptor_t *)((Int8U *)pData + pData->bLength))
  {
    if((pData->bDescriptorType == UsbDescriptorInterface) &&
       (pData->bInterfaceNumber == UsbDev->Interface) &&
       (pData->bAlternateSetting == UsbDev->AlternateSetting))
    {
      return(UsbPass);
    }
  }
  USB_T9W_TRACE_INFO("T9W UsbFindInterface() can't find interface\n\r");
  return(UsbFault);
}

/*************************************************************************
 * Function Name: UsbFindConfiguration
 * Parameters:  Int32U
 *
 * Return: UsbStandardConfigurationDescriptor_t *
 *
 * Description: Return pointer to current configuration
 *
 *************************************************************************/
static
UsbStandardConfigurationDescriptor_t * UsbFindConfiguration(Int32U Configuration)
{
UsbStandardConfigurationDescriptor_t * pData =
 (UsbStandardConfigurationDescriptor_t *) &UsbStandardConfigurationDescriptor[0];
  while(pData->bLength)
  {
    if(pData->bDescriptorType == UsbDescriptorConfiguration)
    {
      if(pData->bConfigurationValue == Configuration)
      {
         return(pData);
      }
    }
    pData = (UsbStandardConfigurationDescriptor_t *)((Int8U *)pData + pData->bLength);
  }
  USB_T9W_TRACE_INFO("T9W UsbFindConfiguration() can't find configuration - %d\n\r",
                     Configuration);
  return(NULL);
}

/*************************************************************************
 * Function Name: UsbSetConfigurtonState
 * Parameters:  Int32U
 *
 * Return: UsbCommStatus_t
 *
 * Description: Set configuration state
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbSetConfigurtonState (Int32U Configuration)
{
  if(Configuration)
  {
    // Find configuration
    UsbStandardConfigurationDescriptor_t * pData = UsbFindConfiguration(Configuration);
    UsbDevCtrl_t UsbDevCtrlNew = UsbDevCtrl;

    if(pData == NULL)
    {
      return(UsbFault);
    }
    // Set Interface and Alternative Setting
    UsbDevCtrlNew.Configuration = Configuration;
    UsbDevCtrlNew.Interface = UsbDevCtrlNew.AlternateSetting = 0;
    UsbDevCtrlNew.Feature.RemoteWakeupEnable = 0;
    UsbDevCtrlNew.Feature.SelfPowered = pData->bmAttributes.SelfPowered?1:0;
    // Whether the interface exist into current configuration?
    if(UsbFindInterface(pData,&UsbDevCtrlNew) != UsbPass)
    {
      return(UsbFault);
    }
    // User function that realize EP and init buffers
    UsbUserFun[UsbUserConfigure]((void *)&UsbDevCtrlNew);
    UsbDevCtrl = UsbDevCtrlNew;
    UsbSetDevState(UsbDevStatusConfigured);
    USB_T9M_TRACE_INFO("T9M UsbSetConfigurtonState()\n\r"
                       "    Configuration - %d\n\r",
                       Configuration);
  }
  else
  {
    USB_T9M_TRACE_INFO("T9M UsbSetConfigurtonState()\n\r"
                       "    Configuration - %d\n\r",
                       Configuration);
    UsbUserFun[UsbUserConfigure](NULL);
    UsbSetDevState(UsbDevStatusAddress);
  }
  return(UsbPass);
}

/*************************************************************************
 * Function Name: UsbDevStatus
 * Parameters:  Int8U * pData, Int16U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: Usb Return device status
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbDevStatus(Int8U * pData, Int16U Index)
{
  if(Index == 0)
  {
    UsbEp0Ctrl.pData = pData;
    UsbEp0Ctrl.Counter = 2;
    *pData++ = UsbDevCtrl.Feature.Data;
    USB_T9M_TRACE_INFO("T9M UsbDevStatus() device status - %d\n\r",
                       UsbDevCtrl.Feature.Data);
    *pData   = 0;
    return(UsbPass);
  }
  USB_T9W_TRACE_INFO("T9W UsbDevStatus()\n\r"
                     "    Index (0==) - %d\n\r",Index);
  return(UsbFault);
}

/*************************************************************************
 * Function Name: UsbInterfaceStatus
 * Parameters:  Int8U * pData, Int16U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB Return interface status into pData
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbInterfaceStatus(Int8U * pData, Int16U Index)
{
  USB_T9M_TRACE_INFO("T9M UsbInterfaceStatus()\n\r");
  UsbEp0Ctrl.pData = pData;
  UsbEp0Ctrl.Counter = 2;
  *pData++ = 0;
  *pData   = 0;
  return(UsbPass);
}

/*************************************************************************
 * Function Name: UsbEpStatus
 * Parameters:  Int8U * pData, Int16U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB Return EP status into pData
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbEpStatus(Int8U * pData, Int16U Index)
{
  if((Index & 0x7F) > 16)
  {
    USB_T9W_TRACE_INFO("T9W UsbEpStatus()\n\r"
                       "    Index (16<) - %d\n\r",
                       Index);
    return(UsbFault);
  }
  UsbEp0Ctrl.pData = pData;
  UsbEp0Ctrl.Counter = 2;
  USB_GetStallEP(USB_EpLogToPhysAdd(Index),pData);
  USB_T9M_TRACE_INFO("T9M UsbEpStatus() stall - %d\n\r", *pData);
  *++pData = 0;
  return(UsbPass);
}

/*************************************************************************
 * Function Name: UsbGetStatus
 * Parameters:  none
 *
 * Return: none
 *
 * Description: USB Get status implement
 *
 *************************************************************************/
static inline
void UsbGetStatus (void)
{
  if((UsbDevCtrl.State.DS >= UsbDevStatusAddress) &&
     // Length must be 2
     (UsbEp0SetupPacket.wLength.Word == 2) &&
     // Direction of data from device
     (UsbEp0SetupPacket.mRequestType.Dir == 1) &&
     // wValue must be 0
     (UsbEp0SetupPacket.wValue.Word == 0) )
  {
    // This request is valid only for Address and Configured device's states
    switch(UsbEp0SetupPacket.mRequestType.Recipient)
    {
    case UsbRecipientDevice:
      if(UsbDevStatus(EpCtrlDataBuf,UsbEp0SetupPacket.wIndex.Word) == UsbPass)
      {
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    case UsbRecipientInterface:
      if(UsbInterfaceStatus(EpCtrlDataBuf,UsbEp0SetupPacket.wIndex.Word) == UsbPass)
      {
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    case UsbRecipientEndpoint:
      if(UsbEpStatus(EpCtrlDataBuf,UsbEp0SetupPacket.wIndex.Word) == UsbPass)
      {
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    default:
      USB_T9W_TRACE_INFO("T9W UsbGetStatus() unknow recipient\n\r");
      UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
    }
  }
  else
  {
    USB_T9W_TRACE_INFO("T9W UsbGetStatus()\n\r"
                       "    State (4>=) - %d\n\r"
                       "    wLength.Word (2==) - %d\n\r"
                       "    mRequestType.Dir (1==) - %d\n\r"
                       "    wValue.Word (0==) - %d\n\r",
                       UsbDevCtrl.State.DS,UsbEp0SetupPacket.wLength.Word,
                       UsbEp0SetupPacket.mRequestType.Dir,UsbEp0SetupPacket.wValue.Word);
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
}

/*************************************************************************
 * Function Name: UsbClearFeature
 * Parameters:  Int16U Feature,Int16U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB Clear Device feature implement
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbClearDevFeature(Int16U Feature,Int16U Index)
{
  if((Index != 0) || (Feature != UsbDeviceRemoteWakeupSelector))
  {
    USB_T9W_TRACE_INFO("T9W UsbSetEpFeature()\n\r"
                       "    Index (0==) - %d\n\r"
                       "    Feature (1==) - %d\n\r",
                       Feature,Index);
    return(UsbFault);
  }
  USB_T9M_TRACE_INFO("T9M UsbSetEpFeature()\n\r"
                       "    Index  - %d\n\r"
                       "    Feature  - %d\n\r",
                       Feature,Index);
  UsbDevCtrl.Feature.RemoteWakeupEnable = 0;
  return(UsbPass);
}
/*************************************************************************
 * Function Name: UsbClearInterfaceFeature
 * Parameters:  Int16U Feature,Int16U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB Clear Interface feature implement
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbClearInterfaceFeature(Int16U Feature,Int16U Index)
{
  USB_T9W_TRACE_INFO("T9W UsbClearInterfaceFeature() unsupported\n\r");
  return(UsbFault);
}

/*************************************************************************
 * Function Name: UsbClearEpFeature
 * Parameters:  Int16U Feature,Int16U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB Clear EP feature implement
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbClearEpFeature(Int16U Feature,Int16U Index)
{
USB_Endpoint_t UsbEpAdd;
  if(((Index & 0x7F) > 16) || (Feature != UsbEpHaltSelector))
  {
    USB_T9W_TRACE_INFO("T9W UsbClearEpFeature()\n\r"
                       "    Index (16<) - %d\n\r"
                       "    Feature (0==) - %d\n\r",
                       Feature,Index);
    return(UsbFault);
  }
  UsbEpAdd = USB_EpLogToPhysAdd(Index);
  USB_SetStallEP(UsbEpAdd,FALSE);
  if (UsbUserFun[UsbUserEpUnStall] != NULL)
  {
    UsbUserFun[UsbUserEpUnStall]((void *)UsbEpAdd);
  }
  USB_T9M_TRACE_INFO("T9M UsbClearEpFeature()\n\r"
                       "    Index  - %d\n\r"
                       "    Feature  - %d\n\r",
                       Feature,Index);
  return(UsbPass);
}

/*************************************************************************
 * Function Name: UsbClearFeature
 * Parameters:  none
 *
 * Return: none
 *
 * Description: USB Clear feature implement
 *
 *************************************************************************/
static inline
void UsbClearFeature (void)
{
  if((UsbDevCtrl.State.DS >= UsbDevStatusAddress) &&
     // Length must be 0
     (UsbEp0SetupPacket.wLength.Word == 0))
  {
    switch(UsbEp0SetupPacket.mRequestType.Recipient)
    {
    case UsbRecipientDevice:
      if(UsbClearDevFeature(UsbEp0SetupPacket.wValue.Word,UsbEp0SetupPacket.wIndex.Word) == UsbPass)
      {
        UsbEp0Ctrl.Counter = 0;
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    case UsbRecipientInterface:
      if((UsbDevCtrl.State.DS > UsbDevStatusAddress) &&
         (UsbClearInterfaceFeature(UsbEp0SetupPacket.wValue.Word,UsbEp0SetupPacket.wIndex.Word) == UsbPass))
      {
        UsbEp0Ctrl.Counter = 0;
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    case UsbRecipientEndpoint:
      if(((UsbDevCtrl.State.DS > UsbDevStatusAddress) || ((UsbEp0SetupPacket.wIndex.Word & 0x7F) == 0)) &&
         (UsbClearEpFeature(UsbEp0SetupPacket.wValue.Word,UsbEp0SetupPacket.wIndex.Word) == UsbPass))
      {
        UsbEp0Ctrl.Counter = 0;
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    default:
      USB_T9W_TRACE_INFO("T9W UsbClearFeature() unknow recipient\n\r");
      UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
    }
  }
  else
  {
    USB_T9W_TRACE_INFO("T9W UsbClearFeature()\n\r"
                       "    State.DS (4>=) - %d\n\r"
                       "    wLength.Word (0==) - %d\n\r",
                       UsbDevCtrl.State.DS,UsbEp0SetupPacket.wLength.Word);
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
}

/*************************************************************************
 * Function Name: UsbSetFeature
 * Parameters:  Int16U Feature,Int16U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB Set Device feature implement
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbSetDevFeature(Int16U Feature,Int16U Index)
{
  if((Index != 0) || (Feature != UsbDeviceRemoteWakeupSelector))
  {
    USB_T9W_TRACE_INFO("T9W UsbSetEpFeature()\n\r"
                       "    Index (0==) - %d\n\r"
                       "    Feature (1==) - %d\n\r",
                       Feature,Index);
    return(UsbFault);
  }
  USB_T9M_TRACE_INFO("T9M UsbSetEpFeature()\n\r"
                       "    Index  - %d\n\r"
                       "    Feature  - %d\n\r",
                       Feature,Index);
  UsbDevCtrl.Feature.RemoteWakeupEnable = 1;
  return(UsbPass);
}
/*************************************************************************
 * Function Name: UsbSetInterfaceFeature
 * Parameters:  Int16U Feature,Int16U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB Set Interface feature implement
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbSetInterfaceFeature(Int16U Feature,Int16U Index)
{
  USB_T9W_TRACE_INFO("T9W UsbSetInterfaceFeature() unsupported\n\r");
  return(UsbFault);
}

/*************************************************************************
 * Function Name: UsbSetEpFeature
 * Parameters:  Int16U Feature,Int16U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB Set EP feature implement
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbSetEpFeature(Int16U Feature,Int16U Index)
{
USB_Endpoint_t UsbEpAdd;
  if(((Index & 0x7F) > 16) || (Feature != UsbEpHaltSelector))
  {
    USB_T9W_TRACE_INFO("T9W UsbSetEpFeature()\n\r"
                       "    Index (16<) - %d\n\r"
                       "    Feature (0==) - %d\n\r",
                       Feature,Index);
    return(UsbFault);
  }
  UsbEpAdd = USB_EpLogToPhysAdd(Index);
  USB_SetStallEP(UsbEpAdd,TRUE);
  if (UsbUserFun[UsbUserEpStall] != NULL)
  {
    UsbUserFun[UsbUserEpStall]((void *)UsbEpAdd);
  }
  USB_T9M_TRACE_INFO("T9M UsbSetEpFeature()\n\r"
                       "    Index - %d\n\r"
                       "    Feature - %d\n\r",
                       Feature,Index);
  return(UsbPass);
}

/*************************************************************************
 * Function Name: UsbSetFeature
 * Parameters:  none
 *
 * Return: none
 *
 * Description: USB Set feature implement
 *
 *************************************************************************/
static inline
void UsbSetFeature (void)
{
  if((UsbDevCtrl.State.DS >= UsbDevStatusAddress) &&
     // Length must be 0
     (UsbEp0SetupPacket.wLength.Word == 0))
  {
    switch(UsbEp0SetupPacket.mRequestType.Recipient)
    {
    case UsbRecipientDevice:
      if(UsbSetDevFeature(UsbEp0SetupPacket.wValue.Word,UsbEp0SetupPacket.wIndex.Word) == UsbPass)
      {
        UsbEp0Ctrl.Counter = 0;
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    case UsbRecipientInterface:
      if((UsbDevCtrl.State.DS > UsbDevStatusAddress) &&
         (UsbSetInterfaceFeature(UsbEp0SetupPacket.wValue.Word,UsbEp0SetupPacket.wIndex.Word) == UsbPass))
      {
        UsbEp0Ctrl.Counter = 0;
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    case UsbRecipientEndpoint:
      if(((UsbDevCtrl.State.DS > UsbDevStatusAddress) || ((UsbEp0SetupPacket.wIndex.Lo & 0x7F) == 0)) &&
         (UsbSetEpFeature(UsbEp0SetupPacket.wValue.Word,UsbEp0SetupPacket.wIndex.Word) == UsbPass))
      {
        UsbEp0Ctrl.Counter = 0;
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    default:
      USB_T9W_TRACE_INFO("T9W UsbSetFeature() unknow recipient\n\r");
      UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
    }
  }
  else
  {
    USB_T9W_TRACE_INFO("T9W UsbSetFeature()\n\r"
                       "    State.DS (4>=) - %d\n\r"
                       "    wLength.Word (0==) - %d\n\r",
                       UsbDevCtrl.State.DS,UsbEp0SetupPacket.wLength.Word);
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
}

/*************************************************************************
 * Function Name: UsbSetAddress
 * Parameters:  none
 *
 * Return: none
 *
 * Description: USB Set address request implement
 *
 *************************************************************************/
static inline
void UsbSetAddress (void)
{
  if((UsbDevCtrl.State.DS >= UsbDevStatusDefault) &&
     (UsbEp0SetupPacket.mRequestType.Recipient == 0) &&
     (UsbEp0SetupPacket.wIndex.Word  == 0) &&
     (UsbEp0SetupPacket.wLength.Word == 0) &&
     (UsbEp0SetupPacket.wValue.Word < 128))
  {
    USB_T9M_TRACE_INFO("T9M UsbSetAddress()\n\r"
                       "    Address - %d\n\r",
                       UsbEp0SetupPacket.wValue.Word);
    UsbEp0Ctrl.Counter = 0;
    UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
  }
  else
  {
    USB_T9W_TRACE_INFO("T9W UsbSetAddress()\n\r"
                       "    State.DS (3>=) - %d\n\r"
                       "    mRequestType.Recipient (0==) - %d\n\r"
                       "    wIndex.Word (0==) - %d\n\r"
                       "    wLength.Word (0==) - %d\n\r"
                       "    wValue.Word (<128) - %d\n\r",
                       UsbDevCtrl.State.DS,UsbEp0SetupPacket.mRequestType.Recipient,
                       UsbEp0SetupPacket.wIndex.Word,UsbEp0SetupPacket.wLength.Word,
                       UsbEp0SetupPacket.wValue.Word);
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
}

/*************************************************************************
 * Function Name: UsbGetDescriptorDevice
 * Parameters:  Int8U Index - must be 0
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB get device's descriptor request implement
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbGetDescriptorDevice(Int8U Index)
{
  if(Index == 0)
  {
    UsbEp0Ctrl.Counter = UsbStandardDeviceDescriptorStr.bLength;
    UsbEp0Ctrl.EpStatus.NoZeroLength = FALSE;
    if(UsbEp0Ctrl.Counter >= UsbEp0SetupPacket.wLength.Word)
    {
      UsbEp0Ctrl.Counter = UsbEp0SetupPacket.wLength.Word;
      UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
    }
    UsbEp0Ctrl.pData = (Int8U *)&UsbStandardDeviceDescriptorStr;
    UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
    return(UsbPass);
  }
  USB_T9W_TRACE_INFO("T9W UsbGetDescriptorConfiguration()\n\r"
                     "    Index (0==) - %d\n\r",Index);
  return(UsbFault);
}

/*************************************************************************
 * Function Name: UsbGetDescriptorConfiguration
 * Parameters:  Int8U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB get configuration's descriptor request implement
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbGetDescriptorConfiguration(Int8U Index)
{
UsbStandardConfigurationDescriptor_t *pData;
  for(pData = (UsbStandardConfigurationDescriptor_t *)UsbStandardConfigurationDescriptor;
      pData->bLength;
      pData=(UsbStandardConfigurationDescriptor_t*)((Int8U *)pData + pData->bLength))
  {
    if((pData->bDescriptorType == UsbDescriptorConfiguration) &&
       ((UsbStandardDeviceDescriptorStr.bNumConfigurations == 1) ||
       (pData->bConfigurationValue == Index)))
    {
      break;
    }
  }
  if(pData->bLength == 0)
  {
     USB_T9W_TRACE_INFO("T9W UsbGetDescriptorConfiguration()\n\r"
                        "    pData->bLength (0==) - %d\n\r",
                        pData->bLength);
     return(UsbFault);
  }
  UsbEp0Ctrl.Counter = ((UsbStandardConfigurationDescriptor_t*)pData)->wTotalLength;
  UsbEp0Ctrl.EpStatus.NoZeroLength = FALSE;
  if(UsbEp0Ctrl.Counter > UsbEp0SetupPacket.wLength.Word)
  {
    UsbEp0Ctrl.Counter = UsbEp0SetupPacket.wLength.Word;
    UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
  }
  UsbEp0Ctrl.pData = (Int8U *)pData;
  UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
  return(UsbPass);
}

/*************************************************************************
 * Function Name: UsbGetDescriptorString
 * Parameters:  Int8U Index
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB get string's descriptor request implement
 *
 *************************************************************************/
static inline
UsbCommStatus_t UsbGetDescriptorString(Int8U Index)
{
Int8U **pData;
Int32U i;
  if(Index == 0)
  {
    UsbEp0Ctrl.Counter = UsbLanguagesStr[UsbDescLengthOffset];
    UsbEp0Ctrl.EpStatus.NoZeroLength = FALSE;
    if(UsbEp0Ctrl.Counter >= UsbEp0SetupPacket.wLength.Word)
    {
      UsbEp0Ctrl.Counter = UsbEp0SetupPacket.wLength.Word;
      UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
    }
    UsbEp0Ctrl.pData = (Int8U *)&UsbLanguagesStr;
    UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
    return(UsbPass);
  }
  else
  {
    Int32U LanguagesOffset = 0;
    // Find Languages index
    for(i = 2; i < UsbLanguagesStr[UsbDescLengthOffset]; i+=2)
    {
      if((UsbLanguagesStr[i] == UsbEp0SetupPacket.wIndex.Lo) &&
         (UsbLanguagesStr[i+1] == UsbEp0SetupPacket.wIndex.Hi))
      {
        break;
      }
      ++LanguagesOffset;
    }
    if(i >= UsbLanguagesStr[UsbDescLengthOffset])
    {
      USB_T9W_TRACE_INFO("T9W UsbGetDescriptorString() can't find string\n\r");
      return(UsbFault);
    }
    // Find String index
    for(pData = (Int8U **)UsbString[LanguagesOffset]; pData ; ++pData)
    {
      if(--Index == 0)
      {
        break;
      }
    }
    if(pData == NULL)
    {
      USB_T9E_TRACE_INFO("T9E UsbGetDescriptorString() - NULL \n\r");
      return(UsbFault);
    }
    UsbEp0Ctrl.Counter = *(*pData+UsbDescLengthOffset);
    UsbEp0Ctrl.EpStatus.NoZeroLength = FALSE;
    if(UsbEp0Ctrl.Counter >= UsbEp0SetupPacket.wLength.Word)
    {
      UsbEp0Ctrl.Counter = UsbEp0SetupPacket.wLength.Word;
      UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
    }
    UsbEp0Ctrl.pData = *pData;
    UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
  }
  return(UsbPass);
}

/*************************************************************************
 * Function Name: UsbGetDescriptor
 * Parameters:  none
 *
 * Return: none
 *
 * Description: USB get descriptor request implement
 *
 *************************************************************************/
static inline
void UsbGetDescriptor (void)
{
    if(UsbDevCtrl.State.DS >= UsbDevStatusDefault)
    {
      switch(UsbEp0SetupPacket.wValue.Hi)
      {
      case UsbDescriptorDevice:
        if(UsbGetDescriptorDevice(UsbEp0SetupPacket.wValue.Lo) != UsbPass)
        {
          UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
        }
        else
        {
          USB_T9M_TRACE_INFO("T9M UsbGetDescriptor() UsbDescriptorDevice \n\r");
        }
        break;
      case UsbDescriptorConfiguration:
        if(UsbGetDescriptorConfiguration(UsbEp0SetupPacket.wValue.Lo) != UsbPass)
        {
          UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
        }
        else
        {
          USB_T9M_TRACE_INFO("T9M UsbGetDescriptor() UsbDescriptorConfiguration \n\r");
        }
        break;
      case UsbDescriptorString:
        if(UsbGetDescriptorString(UsbEp0SetupPacket.wValue.Lo) != UsbPass)
        {
          UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
        }
        else
        {
          USB_T9M_TRACE_INFO("T9M UsbGetDescriptor() UsbDescriptorString \n\r");
        }
        break;
      default:
        if ((UsbUserFun[UsbUserGetDescriptor] == NULL) ||
            ((Int32U)UsbUserFun[UsbUserGetDescriptor](&UsbEp0Ctrl) != UsbPass))
        {
          USB_T9M_TRACE_INFO("T9M UsbGetDescriptor() User defined\n\r");
          UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
        }
        else
        {
          USB_T9W_TRACE_INFO("T9W UsbGetDescriptor() user defined fault\n\r");
        }
      }
    }
    else
    {
      UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
    }
}

/*************************************************************************
 * Function Name: UsbSetDescriptor
 * Parameters:  none
 *
 * Return: none
 *
 * Description: USB Set descriptor
 *
 *************************************************************************/
static inline
void UsbSetDescriptor(void)
{
  USB_T9W_TRACE_INFO("T9W UsbSetDescriptor() unsupported\n\r");
  if(UsbDevCtrl.State.DS >= UsbDevStatusAddress)
  {
    // This request is valid only for Address state and Configured state.
    // If supported
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
  else
  {
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
}

/*************************************************************************
 * Function Name: UsbGetConfiguration
 * Parameters:  none
 *
 * Return: none
 *
 * Description: USB Get configuration implement
 *
 *************************************************************************/
static inline
void UsbGetConfiguration(void)
{
  if((UsbDevCtrl.State.DS >= UsbDevStatusAddress) &&
     (UsbEp0SetupPacket.wValue.Word == 0) &&
     (UsbEp0SetupPacket.wIndex.Word == 0) &&
     (UsbEp0SetupPacket.wLength.Word == 1))
  {
    UsbEp0Ctrl.Counter = 1;
    EpCtrlDataBuf[0] = UsbCoreReq(UsbCoreReqConfiquration);
    UsbEp0Ctrl.pData = EpCtrlDataBuf;
    UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
    USB_T9M_TRACE_INFO("T9M UsbGetConfiguration()\n\r"
                       "    UsbCoreReqConfiquration - %d\n\r",
                       EpCtrlDataBuf[0]);
  }
  else
  {
    USB_T9W_TRACE_INFO("T9W UsbGetConfiguration()\n\r"
                       "    State.DS (>=4) - %d\n\r"
                       "    wIndex.Word (==0) - %d\n\r"
                       "    wLength.Word (==1) - %d\n\r"
                       "    wValue.Wor (==0) - %d\n\r",
                       UsbDevCtrl.State.DS,UsbEp0SetupPacket.wIndex.Word,
                       UsbEp0SetupPacket.wLength.Word,UsbEp0SetupPacket.wValue.Word);
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
}

/*************************************************************************
 * Function Name: UsbSetConfiguration
 * Parameters:  none
 *
 * Return: none
 *
 * Description: USB Set configuration implement
 *
 *************************************************************************/
static inline
void UsbSetConfiguration(void)
{
  if((UsbDevCtrl.State.DS >= UsbDevStatusAddress) &&
     (UsbEp0SetupPacket.wIndex.Word == 0) &&
     (UsbEp0SetupPacket.wLength.Word == 0) &&
     (UsbSetConfigurtonState(UsbEp0SetupPacket.wValue.Lo) == UsbPass))
  {
    UsbEp0Ctrl.Counter = 0;
    UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
  }
  else
  {
    USB_T9W_TRACE_INFO("T9W UsbSetConfiguration()\n\r"
                       "    State.DS (==5) - %d\n\r"
                       "    wIndex.Word - %d\n\r"
                       "    wLength.Word (==0) - %d\n\r"
                       "    wValue.Wor (==0) - %d\n\r",
                       UsbDevCtrl.State.DS,UsbEp0SetupPacket.wIndex.Word,
                       UsbEp0SetupPacket.wLength.Word,UsbEp0SetupPacket.wValue.Word);
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
}

/*************************************************************************
 * Function Name: UsbGetInterface
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Implement get interface request
 *
 *************************************************************************/
static inline
void UsbGetInterface (void)
{
  if((UsbDevCtrl.State.DS == UsbDevStatusConfigured) &&
     (UsbEp0SetupPacket.wIndex.Word == UsbDevCtrl.Interface) &&
     (UsbEp0SetupPacket.wLength.Word == 1) &&
     (UsbEp0SetupPacket.wValue.Word == 0))
  {
    USB_T9M_TRACE_INFO("T9M UsbGetInterface()\n\r");
    UsbEp0Ctrl.Counter = 1;
    EpCtrlDataBuf[0] = UsbDevCtrl.AlternateSetting;
    UsbEp0Ctrl.pData = EpCtrlDataBuf;
    UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
  }
  else
  {
    volatile Int32U Interface = UsbDevCtrl.Interface;
    USB_T9W_TRACE_INFO("T9W UsbGetInterface()\n\r"
                       "    State.DS (==5) - %d\n\r"
                       "    wIndex.Word - %d\n\r"
                       "    Interface - %d\n\r"
                       "    wLength.Word (==1) - %d\n\r"
                       "    wValue.Wor (==0) - %d\n\r",
                       UsbDevCtrl.State.DS,UsbEp0SetupPacket.wIndex.Word,
                       Interface,UsbEp0SetupPacket.wLength.Word,
                       UsbEp0SetupPacket.wValue.Word);
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
}

/*************************************************************************
 * Function Name: UsbSetInterface
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Implement set interface request
 *
 *************************************************************************/
static inline
void UsbSetInterface (void)
{
  if((UsbDevCtrl.State.DS == UsbDevStatusConfigured) &&
     (UsbEp0SetupPacket.wLength.Word == 0))
  {

    UsbStandardConfigurationDescriptor_t * pData = UsbFindConfiguration(UsbDevCtrl.Configuration);
    UsbDevCtrl_t UsbDevCtrlNew = UsbDevCtrl;

    UsbDevCtrlNew.Interface = UsbEp0SetupPacket.wIndex.Word;
    UsbDevCtrlNew.AlternateSetting = UsbEp0SetupPacket.wValue.Word;
    UsbDevCtrlNew.Feature.RemoteWakeupEnable = 0;

    // Whether the interface is exist into current configuration?
    if(UsbFindInterface(pData,&UsbDevCtrlNew) != UsbPass)
    {
      USB_T9W_TRACE_INFO("T9W UsbSetInterface() can't find interface\n\r");
      UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      return;
    }

    USB_T9M_TRACE_INFO("T9M UsbSetInterface()\n\r");

    UsbUserFun[UsbUserConfigure]((void *)&UsbDevCtrlNew);
    UsbDevCtrl = UsbDevCtrlNew;

    UsbEp0Ctrl.Counter = 0;
    UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
  }
  else
  {
    USB_T9W_TRACE_INFO("T9W UsbSetInterface()\n\r"
                       "    State.DS (==5) - %d\n\r"
                       "    wLength.Word (==2) - %d\n\r",
                       UsbDevCtrl.State.DS,
                       UsbEp0SetupPacket.wLength.Word);
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
}

/*************************************************************************
 * Function Name: UsbSynchFrame
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Implement synch frame request
 *
 *************************************************************************/
static inline
void UsbSynchFrame (void)
{
  if( USB_SOF_EVENT &&
     (UsbDevCtrl.State.DS == UsbDevStatusConfigured) &&
     (UsbEp0SetupPacket.wLength.Word == 2) &&
     (UsbEp0SetupPacket.wValue.Word == 0))
  {
  Int32U FrameNumb = 0;
  #if USB_SOF_EVENT > 0
    FrameNumb = USB_GetFrameNumb();
  #endif
    UsbEp0Ctrl.Counter = 2;
    EpCtrlDataBuf[0]   = FrameNumb;
    EpCtrlDataBuf[1]   = FrameNumb>>8;
    UsbEp0Ctrl.pData   = EpCtrlDataBuf;
    UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
    USB_T9M_TRACE_INFO("T9M UsbSynchFrame() %d \n\r",FrameNumb);
  }
  else
  {
    USB_T9W_TRACE_INFO("T9W UsbSynchFrame()\n\r"
                       "    USB_SOF_EVENT - %d\n\r"
                       "    State.DS (==5) - %d\n\r"
                       "    wLength.Word (==2) - %d\n\r"
                       "    wValue.Word (==0) - %d\n\r",
                       USB_SOF_EVENT,UsbDevCtrl.State.DS,
                       UsbEp0SetupPacket.wLength.Word,
                       UsbEp0SetupPacket.wValue.Word);
    UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
  }
}

/*************************************************************************
 * Function Name: UsbDevConnectCallback
 * Parameters:  void * pArg
 *
 * Return: none
 *
 * Description: USB connect callback
 *
 *************************************************************************/
static inline
void UsbDevConnectCallback (Boolean Connect)
{
  UsbSetDevState(Connect?UsbDevStatusAttached:UsbDevStatusUnknow);
  if(UsbUserFun[UsbUserConnect] != NULL)
  {
    UsbUserFun[UsbUserConnect]((void*)Connect);
  }
}

/*************************************************************************
 * Function Name: UsbDevSuspendCallback
 * Parameters:  Boolean Suspend
 *
 * Return: none
 *
 * Description: USB suspend callback
 *
 *************************************************************************/
static inline
void UsbDevSuspendCallback (Boolean Suspend)
{
  UsbSetDevSuspend(Suspend?UsbDevStatusSuspend:UsbDevStatusNoSuspend);
  if(UsbUserFun[UsbUserSuspend] != NULL)
  {
    UsbUserFun[UsbUserSuspend]((void*)Suspend);
  }
}

/*************************************************************************
 * Function Name: UsbDevResetCallback
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB reset callback
 *
 *************************************************************************/
static inline
void UsbDevResetCallback (void)
{
  UsbSetDevState(UsbDevStatusDefault);
  if(UsbUserFun[UsbUserReset] != NULL)
  {
    UsbUserFun[UsbUserReset](NULL);
  }
}
