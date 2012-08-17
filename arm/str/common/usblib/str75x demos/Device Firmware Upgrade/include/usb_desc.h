/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_desc.h
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Descriptor Header for Device Firmware Upgrade (DFU)
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
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/

//#define _STR710_EVAL    /* Comment this line if STR710-Eval board is not used */
#define _STR75x_EVAL

#define	DFU_SIZ_DEVICE_DESC        18

#ifdef _STR710_EVAL
#define	DFU_SIZ_CONFIG_DESC        45
#elif defined _STR75x_EVAL
#define	DFU_SIZ_CONFIG_DESC        45
#else
#define	DFU_SIZ_CONFIG_DESC        36
#endif


#define DFU_SIZ_STRING_LANGID           4
#define DFU_SIZ_STRING_VENDOR           38
#define DFU_SIZ_STRING_PRODUCT          22
#define DFU_SIZ_STRING_SERIAL           8
#define DFU_SIZ_STRING_INTERFACE0       122    /* Flash Bank 0 */
#define DFU_SIZ_STRING_INTERFACE1       74     /* Flash Bank 1 */
#ifdef _STR710_EVAL
#define DFU_SIZ_STRING_INTERFACE2       92     /* External Flash M28W320CB */
#endif
#ifdef _STR75x_EVAL
#define DFU_SIZ_STRING_INTERFACE2       98     /* SPI Flash : M25P64*/
#endif



extern const u8 DFU_DeviceDescriptor[DFU_SIZ_DEVICE_DESC];
extern const u8 DFU_ConfigDescriptor[DFU_SIZ_CONFIG_DESC];


extern const u8 DFU_StringLangId     [DFU_SIZ_STRING_LANGID];
extern const u8 DFU_StringVendor     [DFU_SIZ_STRING_VENDOR];
extern const u8 DFU_StringProduct    [DFU_SIZ_STRING_PRODUCT];
extern const u8 DFU_StringSerial     [DFU_SIZ_STRING_SERIAL];
extern const u8 DFU_StringInterface0 [DFU_SIZ_STRING_INTERFACE0];
extern const u8 DFU_StringInterface1 [DFU_SIZ_STRING_INTERFACE1];
#ifdef _STR710_EVAL
extern const u8 DFU_StringInterface2 [DFU_SIZ_STRING_INTERFACE2];
#endif
#ifdef _STR75x_EVAL
extern const u8 DFU_StringInterface2 [DFU_SIZ_STRING_INTERFACE2];
#endif

#define bMaxPacketSize0             0x40     /* bMaxPacketSize0 = 64 bytes   */
#define wTransferSize               0x0400   /* wTransferSize   = 1024 bytes */
                                             /* bMaxPacketSize0 <= wTransferSize <= 32kbytes */
#define wTransferSizeB0             0x00
#define wTransferSizeB1             0x04


#endif /* __USB_DESC_H */

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/




