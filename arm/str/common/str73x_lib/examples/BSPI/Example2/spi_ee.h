/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : spi_ee.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Header for the spi_ee.c file.
********************************************************************************
* History:
* 09/27/2005 :  V1.0
********************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __SPI_EE_H
#define __SPI_EE_H

/* Includes ------------------------------------------------------------------*/
#include "73x_lib.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* EEPROM instruction set */
#define WREN     0x06   /* Write Enable */ 
#define RDSR     0x05   /* Read Status Register */ 
#define WRSR     0x01   /* Write Status Register */ 
#define READ     0x03   /* Read from Memory */ 
#define WRITE    0x02   /* Write to Memory */

/* ChipSelect state */
#define Low     0x00
#define High    0x01

/* Program  flag */
#define WIP_Flag             0x01     /* Write In Progress (WIP) flag */
#define Write_In_Progress    0x01

#define Dummy_Byte    0xA5

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SPI_EE_Init(void);
void SPI_EE_DeInit(void);
void SPI_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite);
void SPI_EE_BlockWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite); 
void SPI_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite); 
void SPI_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead);
void SPI_EE_DMABufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead);
void SPI_EE_Erase(void);

#endif /* __SPI_EE_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
