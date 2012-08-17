/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : usb_9.h
 *    Description : Define usb chapter 9 (device framework) module
 *
 *    History :
 *    1. Date        : August 21, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/
#include "arm_comm.h"

#ifndef __USB_9_H
#define __USB_9_H

typedef enum _UsbCommStatus_t
{
  UsbPass = 0, UsbFault, UsbNotSupport,
} UsbCommStatus_t;

typedef enum _UsbT9DevState_t
{
  UsbDevStatusUnknow = 0, UsbDevStatusAttached, UsbDevStatusPowered,
  UsbDevStatusDefault, UsbDevStatusAddress, UsbDevStatusConfigured
} UsbT9DevState_t;

typedef enum _UsbEpStatusEn_t
{
	// EP States
  UsbEpStall = 0,UsbEpUnstall,
	
	// Framework States
	UsbSetupPhase,UsbDataPhase,UsbStatusPhase

} UsbEpStatusEn_t;

typedef enum _UsbSetupType_t
{
  UsbTypeStandart = 0,UsbTypeClass,UsbTypeVendor,UsbTypeReserved
} UsbSetupType_t;

typedef enum _UsbSetupStandatRecuest_t
{
  GET_STATUS = 0,CLEAR_FEATURE, Reserved0, SET_FEATURE, Reserved1,
  SET_ADDRESS, GET_DESCRIPTOR, SET_DESCRIPTOR, GET_CONFIGURATION,
  SET_CONFIGURATION, GET_INTERFACE, SET_INTERFACE, SYNCH_FRAME
} UsbSetupStandatRecuest_t;

typedef enum _UsbSetupRecipient_t
{
  UsbRecipientDevice = 0,UsbRecipientInterface,UsbRecipientEndpoint,
  UsbRecipientOther,UsbRecipientReserved
} UsbSetupRecipient_t;

typedef enum _UsbFeatureSelector_t
{
  UsbEpHaltSelector= 0, UsbDeviceRemoteWakeupSelector, UsbTestModeSelector
} UsbFeatureSelector_t;

typedef enum _UsbCoreReqType_t
{
  UsbCoreReqDevState = 0, UsbCoreReqDevSusState, UsbCoreReqConfiquration,
  UsbCoreReqInterface, UsbCoreReqAlternateSetting, UsbCoreReqDevOwnAddress,
  UsbCoreReqWakeUpEnableStatus, UsbCoreReqSelfPoweredStatus,
} UsbCoreReqType_t;

typedef enum _UsbUserFuncResponse_t
{
  UsbUserError = 0, UsbUserSendPacket, UsbUserReceivePacket, UsbUserSendAckn,
  UsbUserStallCtrlEp,
} UsbUserFuncResponse_t;

typedef enum _UsbDataState_t
{
	UsbHost2Device = 0, UsbDevice2Host
} UsbDataState_t;
#define UsbDevStatusNoSuspend 0
#define UsbDevStatusSuspend   1

#pragma pack(1)
typedef struct _UsbDevState_t
{
  Int8U DS          : 3;
  Int8U             : 4;
  Int8U Suspend     : 1;
} UsbDevState_t, * pUsbDevState_t;

typedef union _UsbRequestType_t
{
  Int8U mRequestTypeData;
  struct
  {
    Int8U Recipient : 5;
    Int8U Type      : 2;
    Int8U Dir       : 1;
  };
}UsbRequestType_t, * pUsbRequestType_t;

typedef union _TwoBytes_t
{
  Int16U Word;
  struct
  {
    Int8U Lo;
    Int8U Hi;
  };
}TwoBytes_t, * pTwoBytes_t;

typedef struct _UsbSetupPacket_t
{
  UsbRequestType_t  mRequestType;
  Int8U             bRequest;
  TwoBytes_t        wValue;
  TwoBytes_t        wIndex;
  TwoBytes_t        wLength;
} UsbSetupPacket_t, * pUsbSetupPacket_t;

#pragma pack()

typedef union _UsbDefFeature_t
{
  Int8U Data;
  struct
  {
    Int8U SelfPowered         :1;
    Int8U RemoteWakeupEnable  :1;
    Int8U                     :6;
  };
} UsbDefFeature_t, * pUsbDefFeature_t;

typedef struct _UsbDevCtrl_t
{
  UsbDevState_t     State;
  Int32U            Configuration;
  Int32U            Interface;
  Int32U            AlternateSetting;
  Int32U            DevAdd;
  UsbDefFeature_t   Feature;
} UsbDevCtrl_t, * pUsbDevCtrl_t;

typedef struct _UsbEpStatus_t
{
  UsbEpStatusEn_t   Status;
  Boolean           NoZeroLength;
} UsbEpStatus_t, * pUsbEpStatus_t;

typedef struct _UsbEpCtrl_t
{
  Int8U *pData;
  Int32U Counter;
  UsbEpStatus_t EpStatus;
}UsbEpCtrl_t, *pUsbEpCtrl_t;


#endif //__USB_9_H
