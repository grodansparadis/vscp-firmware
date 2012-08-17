/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_desc.h
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Descriptor Header for Joystick Mouse Demo
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
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define HID_DESCRIPTOR_TYPE                     0x21
#define JOYSTICK_SIZ_HID_DESC                   0x09
#define JOYSTICK_OFF_HID_DESC                   0x12

#define JOYSTICK_SIZ_DEVICE_DESC                   18
#define	JOYSTICK_SIZ_CONFIG_DESC                   34
#define	JOYSTICK_SIZ_REPORT_DESC	           74
#define	JOYSTICK_SIZ_STRING_LANGID                 4
#define JOYSTICK_SIZ_STRING_VENDOR                 38
#define JOYSTICK_SIZ_STRING_PRODUCT                32
#define JOYSTICK_SIZ_STRING_SERIAL                 26


#define STANDARD_ENDPOINT_DESC_SIZE             0x09

/* Exported functions ------------------------------------------------------- */
extern const u8 Joystick_DeviceDescriptor[JOYSTICK_SIZ_DEVICE_DESC];
extern const u8 Joystick_ConfigDescriptor[JOYSTICK_SIZ_CONFIG_DESC];

extern const u8 Joystick_ReportDescriptor[JOYSTICK_SIZ_REPORT_DESC];

extern const u8 Joystick_StringLangID[JOYSTICK_SIZ_STRING_LANGID];
extern const u8 Joystick_StringVendor[JOYSTICK_SIZ_STRING_VENDOR];
extern const u8 Joystick_StringProduct[JOYSTICK_SIZ_STRING_PRODUCT];
extern const u8 Joystick_StringSerial[JOYSTICK_SIZ_STRING_SERIAL];

#endif /* __USB_DESC_H */

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
