/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : i2c_ee.h
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Header for i2c_ee.c module
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
********************************************************************************/
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __I2C_EE_H
#define __I2C_EE_H

/* Includes ------------------------------------------------------------------*/
#include "75x_lib.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* The M24C08W contains 4 blocks (128byte each) with the adresses below */
/* EEPROM Addresses defines */
#define EEPROM_Block0_ADDRESS 0xA8   /* E2 = 1 */
//#define EEPROM_Block1_ADDRESS 0xAA
//#define EEPROM_Block2_ADDRESS 0xAC
//#define EEPROM_Block3_ADDRESS 0xAE

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void I2C_EE_Init(void);
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr);
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
void I2C_EE_WaitForWriteEnd(void);

#endif /* __I2C_EE_H */

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


