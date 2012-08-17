/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : spi_ee.h
* Author             : MCD Application Team
* Date First Issued  : 07/17/2006 
* Description        : Header for spi_ee.c file.
********************************************************************************
* History:
* 07/17/2006 : V1.0
*******************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_EE_H
#define __SPI_EE_H

/* Includes ------------------------------------------------------------------*/
#include "75x_lib.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SPI_EE_Init(void);
void SPI_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite);
void SPI_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite); 
void SPI_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead);

#endif /* __SPI_EE_H */

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
