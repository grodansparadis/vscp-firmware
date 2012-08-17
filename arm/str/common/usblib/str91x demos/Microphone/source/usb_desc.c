/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_desc.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Descriptors for Microphone
********************************************************************************
* History:
* 10/01/2006 : V1.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants----------------------------------------------------------*/
/* USB Standard Device Descriptor */
const u8 Micro_DeviceDescriptor[] = {
  MICRO_SIZ_DEVICE_DESC,          /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,           /* bDescriptorType */
  0x00,          /* 2.00 */             /* bcdUSB */
  0x02,
  0x00,                                 /* bDeviceClass */
  0x00,                                 /* bDeviceSubClass */
  0x00,                                 /* bDeviceProtocol */
  0x40,                                 /* bMaxPacketSize0 */
  0x83,                                 /* idVendor STMicroelectronics*/
  0x04,
  0x31,                                 /* idProduct = 5731*/
  0x57,
  0x00,          /* 1.00 */             /* bcdDevice */
  0x01,
  1,                                 /* iManufacturer */
  2,                                 /* iProduct */
  3,                                 /* iSerialNumber */
  0x01      
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const u8 Micro_ConfigDescriptor[] = {
/* Configuration 1 */
  0x09,                                /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,   /* bDescriptorType */
  0x64,                               /* wTotalLength = 100  bytes */
  0x00,
  0x02,                                 /* bNumInterfaces */
  0x01,                                 /* bConfigurationValue */
  0x00,                                 /* iConfiguration */
  0x80,                                 /* bmAttributes  BUS Powred*/
  0x32,                                 /* bMaxPower = 100 mA*/
/* 09 byte*/
/* Interface 0, Alternate Setting 0, USB Microphone Standard AC Interface Descriptor */
  MICRO_SIZ_INTERFACE_DESC_SIZE,  /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x00,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* 09 byte*/
/* USB Microphone Class-specific AC Interface Descriptor */
  MICRO_SIZ_INTERFACE_DESC_SIZE,   /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
  0x01,          /* 1.00 */             /* bcdADC */
  0x00,
  0x1E,                                /* wTotalLength = 30*/
  0x00,
  0x01,                                 /* bInCollection */
  0x01,                                 /* baInterfaceNr */
   /* 09 byte*/
/* USB Microphone Input Terminal Descriptor */
  AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
  0x01,                                 /* bTerminalID */
  0x02,                                 /* wTerminalType: terminal is Micro = 0x0201 */
  0x01,
  0x00,                                 /* bAssocTerminal */
  0x01,                                 /* bNrChannels */
  0x00,                                 /* wChannelConfig 0x0000  Mono */
  0x00,
  0x00,                                 /* iChannelNames */
  0x00,                                 /* iTerminal */
  /* 12 byte*/
/* USB Microphone Output Terminal Descriptor*/
  AUDIO_OUTPUT_TERMINAL_DESC_SIZE,      /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
  0x02,                                 /* bTerminalID */
  0x01,                                 /* wTerminalType AUDIO_USB_STREAMING.  0x0101*/
  0x01,
  0x00,                                 /* bAssocTerminal */
  0x01,                                 /* bSourceID */
  0x00,                                 /* iTerminal */
  /* 09 byte*/
/* Interface 1, Alternate Setting 0, Audio Streaming - Zero Bandwith */
  MICRO_SIZ_INTERFACE_DESC_SIZE,    /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x01,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* 09 byte*/
/* Interface 1, Alternate Setting 1, Audio Streaming - Operational */
  MICRO_SIZ_INTERFACE_DESC_SIZE,    /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x01,                                 /* bInterfaceNumber */
  0x01,                                 /* bAlternateSetting */
  0x01,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* 09 byte*/
/* USB Microphone Class-specific AS General Interface Descriptor */
  AUDIO_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
  0x02,                                 /* bTerminalLink */
  0x01,                                 /* bDelay */
  0x02,                                 /* wFormatTag AUDIO_FORMAT_PCM8  0x0002*/
  0x00,
  /* 07 byte*/
/* USB Microphone Type I Format Type Descriptor */
  0x0B,                                 /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
  AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
  0x01,                                 /* bNrChannels */
  0x01,                                 /* bSubFrameSize */
  8,                                    /* bBitResolution */
  0x01,                                 /* bSamFreqType */
  0xF0,                               /* tSamFreq 22KHz = 0x55F0 */
  0x55,
  0x00,
  /* 11 byte*/
/* USB Microphone Standard Endpoint Descriptor */
  AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */
  0x81,                                 /* bEndpointAddress 1 IN endpoint*/
  USB_ENDPOINT_TYPE_ISOCHRONOUS,        /* bmAttributes */
  0x16,                                 /* wMaxPacketSize 22 bytes*/
  0x00,
  0x01,                                 /* bInterval */
  0x00,                                 /* bRefresh */
  0x00,                                 /* bSynchAddress */
  /* 09 byte*/
/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor */
  AUDIO_STREAMING_ENDPOINT_DESC_SIZE,   /* bLength */
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
  AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
  0x00,                                 /* bmAttributes */
  0x00,                                 /* bLockDelayUnits */
  0x00,                                 /* wLockDelay */
  0x00,
  /* 07 byte*/
};

/* USB String Descriptor (optional) */
const u8 Micro_StringLangID[MICRO_SIZ_STRING_LANGID] = {
/* Index 0x00: LANGID Codes */
  0x04,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x09, /* US English */    /* wLANGID */
  0x04
};
const u8 Micro_StringVendor[MICRO_SIZ_STRING_VENDOR]=
{
  0x26,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'S',0,'T',0,'M',0,'i',0,'c',0,'r',0,'o',0,'e',0,
  'l',0,'e',0,'c',0,'t',0,'r',0,'o',0,'n',0,'i',0,
  'c',0,
  's',0
};
const u8 Micro_StringProduct[MICRO_SIZ_STRING_PRODUCT]=
{
            
  0x1E,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'S',0,'T',0,'R',0,'9',0,'1',0,'x',0,' ',0,'M',0,
  'i',0,'c',0,'r',0,'o',0,' ',0,' ',0
};
const u8 Micro_StringSerial[MICRO_SIZ_STRING_SERIAL]=
{
  0x1A,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'D',0,'E',0,'M',0,'O',0,' ',0,'1',0,'.',0, '0',0,
  '0',0,'0',0,'0',0,'0',0
};
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

