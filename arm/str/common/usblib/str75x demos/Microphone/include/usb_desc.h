/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_desc.h
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Descriptor Header for Microphone Demo
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define MICRO_SIZ_DEVICE_DESC                 18
#define	MICRO_SIZ_CONFIG_DESC                 101
#define MICRO_SIZ_INTERFACE_DESC_SIZE         9

#define	MICRO_SIZ_STRING_LANGID               4 
#define MICRO_SIZ_STRING_VENDOR               0x26
#define MICRO_SIZ_STRING_PRODUCT              0x1E
#define MICRO_SIZ_STRING_SERIAL               0x1A


/* USB Descriptor Types */
#define USB_DEVICE_DESCRIPTOR_TYPE            0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE     0x02
#define USB_STRING_DESCRIPTOR_TYPE            0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE         0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE          0x05

/* Audio Descriptor Types */
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE        0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE         0x25

#define USB_ENDPOINT_TYPE_ISOCHRONOUS          0x01

#define USB_DEVICE_CLASS_AUDIO                 0x01
#define AUDIO_SUBCLASS_AUDIOCONTROL            0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING          0x02
#define AUDIO_PROTOCOL_UNDEFINED               0x00
#define AUDIO_STREAMING_GENERAL                0x01
#define AUDIO_STREAMING_FORMAT_TYPE            0x02

/* Audio Control Interface Descriptor Subtypes */
#define AUDIO_CONTROL_HEADER                   0x01
#define AUDIO_CONTROL_INPUT_TERMINAL           0x02
#define AUDIO_CONTROL_OUTPUT_TERMINAL          0x03
#define AUDIO_INPUT_TERMINAL_DESC_SIZE         0x0C
#define AUDIO_OUTPUT_TERMINAL_DESC_SIZE        0x09
#define AUDIO_STREAMING_INTERFACE_DESC_SIZE    0x07
#define AUDIO_FORMAT_TYPE_I                    0x01
#define AUDIO_ENDPOINT_GENERAL                 0x01

#define AUDIO_STANDARD_ENDPOINT_DESC_SIZE      0x09
#define AUDIO_STREAMING_ENDPOINT_DESC_SIZE     0x07

/* Exported functions ------------------------------------------------------- */
extern const u8 Micro_DeviceDescriptor[MICRO_SIZ_DEVICE_DESC];
extern const u8 Micro_ConfigDescriptor[MICRO_SIZ_CONFIG_DESC];
extern const u8 Micro_StringLangID[MICRO_SIZ_STRING_LANGID];
extern const u8 Micro_StringVendor[MICRO_SIZ_STRING_VENDOR];
extern const u8 Micro_StringProduct[MICRO_SIZ_STRING_PRODUCT];
extern const u8 Micro_StringSerial[MICRO_SIZ_STRING_SERIAL];
#endif /* __USB_DESC_H */
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
