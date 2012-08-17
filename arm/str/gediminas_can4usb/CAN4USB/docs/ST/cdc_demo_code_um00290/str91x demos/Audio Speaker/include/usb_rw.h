/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_rw.h
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : PMA read & write header file
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
#ifndef __USB_RW_H
#define __USB_RW_H
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void PMA_Write(u32 *pbUsrBuf,u16 wPMABufAddr, u32 wNBytes);
void PMA_Read(u32 *pbUsrBuf,u16 wPMABufAddr, u32 wNBytes);
#endif /* __USB_RW_H */
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
