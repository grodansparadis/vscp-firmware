/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : smi_flash.h
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Header for smi_flash.c file.
********************************************************************************
* History:
* 07/17/2006 : V1.0
* 03/10/2006 : V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SMI_FLASH_H
#define __SMI_FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "75x_lib.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SMI_FLASH_Init(void);
void SMI_FLASH_SectorErase(u32 SectorAddr);
void SMI_FLASH_BulkErase(void);
void SMI_FLASH_WritebBuffer(u8* bBuffer,  u32 WriteAddr, u16 NumByteToWrite);
void SMI_FLASH_WritehBuffer(u16* hBuffer, u32 WriteAddr, u16 NumHalfWordToWrite);
void SMI_FLASH_WritewBuffer(u32* wBuffer, u32 WriteAddr, u16 NumWordToWrite);
void SMI_FLASH_ReadbBuffer(u8* bBuffer, u32 ReadAddr, u16 NumByteToRead);
void SMI_FLASH_ReadhBuffer(u16* hBuffer, u32 ReadAddr, u16 NumHalfWordToRead);
void SMI_FLASH_ReadwBuffer(u32* wBuffer, u32 ReadAddr, u16 NumWordToRead);
u32  SMI_FLASH_ReadID(void);
  
#endif /* __SMI_FLASH_H */

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
