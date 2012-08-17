/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : cdc_desc.h
 *    Description : Communication Device Class descriptors definitions
 *
 *    History :
 *    1. Date        : June 30, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 16170 $
 **************************************************************************/
#include "arm_comm.h"
#include "usb_desc.h"

#ifndef __CDC_DESC_H
#define __CDC_DESC_H

// Parity
#define NOPARITY                0
#define ODDPARITY               1
#define EVENPARITY              2
#define MARKPARITY              3
#define SPACEPARITY             4

// StopBits
#define ONESTOPBIT              0
#define ONE5STOPBITS            1
#define TWOSTOPBITS             2

// Baud rates
#define CBR_110                 110
#define CBR_300                 300
#define CBR_600                 600
#define CBR_1200                1200
#define CBR_2400                2400
#define CBR_4800                4800
#define CBR_9600                9600
#define CBR_14400               14400
#define CBR_19200               19200
#define CBR_38400               38400
#define CBR_56000               56000
#define CBR_57600               57600
#define CBR_115200              115200
#define CBR_128000              128000
#define CBR_256000              256000
typedef enum
{
  DirectLineControlModel = 1, AbstractControlModel, TelephoneControlModel,
  MultiChannelControlModel, CAPI_ControlModel, EthernetNetworkingControlModel,
  ATM_NetworkingControlModel,
} CommInterfaceClassSubClassCodes_t;

typedef enum
{
  USB_SpecificationNoClass = 1, V_0_25ter, USB_SpecificationVendorSpecClass = 255
} CommInterfaceClassControlProtocolCodes_t;

typedef enum
{
  CS_INTERFACE = 0x24,CS_ENDPOINT,
} CD_FunctionalDescriptorTypes_t;

typedef enum
{
  Header = 0, CallManagement, AbstractControlManagement, DirectLineManagement,
  TelephoneRinger, TelephoneCallLineCapabilities, Union, CountrySelection,
  TelephoneOperationalModes, USB_Terminal, NetworkChannelTerminal, ProtocolUnit,
  ExtensionUnit, MultiChannelManagement, CAPI_ControlManagement, EthernetNetworking,
  ATMNetworking,
} CD_FunctionalcDDescriptorSubTypes_t;

typedef struct _ManCallManCap_t
{
  Int8U NoCapability          : 1;
  Int8U OverDataClassInterface: 1;
  Int8U                       : 6;
} ManCallManCap_t, *pManCallManCap_t;


typedef struct _AbsCntlCap_t
{
  Int8U Comm_Feature        : 1;
  Int8U Control_Line        : 1;
  Int8U Send_Break          : 1;
  Int8U Network_Connection  : 1;
  Int8U                     : 4;
} AbsCntlCap_t, *pAbsCntlCap_t;

#pragma pack(1)
typedef struct _CDC_FuncDescriptor_t
{
  Int8U bFunctionLength;
  Int8U bDescriptorType;
  Int8U bDescriptorSubtype;
} CDC_FuncDescriptor_t,*pCDC_FuncDescriptor_t;

typedef struct _CDC_HeaderFuncDescriptor_t
{
  Int8U bFunctionLength;
  Int8U bDescriptorType;
  Int8U bDescriptorSubtype;
  Int16U bcdCDC;
} CDC_HeaderFuncDescriptor_t, *pCDC_HeaderFuncDescriptor_t;

typedef struct _CDC_CallManagementFuncDescriptor_t
{
  Int8U bFunctionLength;
  Int8U bDescriptorType;
  Int8U bDescriptorSubtype;
  ManCallManCap_t bmCapabilities;
  Int8U bDataInterface;
} CDC_CallManagementFuncDescriptor_t, *pCDC_CallManagementFuncDescriptor_t;

typedef struct _CDC_AbstrCtrlFuncDescriptor_t
{
  Int8U bFunctionLength;
  Int8U bDescriptorType;
  Int8U bDescriptorSubtype;
  AbsCntlCap_t bmCapabilities;
} CDC_AbstrCtrlFuncDescriptor_t, *pCDC_CDC_AbstrCtrlFuncDescriptor_t;

typedef struct _CDC_UnionFuncDescriptor_t
{
  Int8U bFunctionLength;
  Int8U bDescriptorType;
  Int8U bDescriptorSubtype;
  Int8U bMasterInterface;
} CDC_UnionFuncDescriptor_t, *pCDC_UnionFuncDescriptor_t;

#pragma pack()

#endif //__CDC_DESC_H
