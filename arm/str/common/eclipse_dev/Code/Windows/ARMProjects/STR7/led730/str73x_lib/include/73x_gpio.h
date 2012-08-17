/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_gpio.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      GPIO software library.
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
* CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __73x_GPIO_H
#define __73x_GPIO_H

/* Includes ----------------------------------------------------------------- */
#include "73x_map.h"

/* Exported types ----------------------------------------------------------- */
/* GPIO Init structure define */
typedef struct
{
  u16 GPIO_Pins;
  u16 GPIO_Mode;
}GPIO_InitTypeDef;

/* Bit_SET and Bit_RESET enumeration */
typedef enum { Bit_RESET = 0, Bit_SET   = !RESET } BitAction;

/* Exported constants --------------------------------------------------------*/
/* GPIO Pins define */
#define GPIO_PIN_NONE   0x0000
#define GPIO_PIN_0      0x0001
#define GPIO_PIN_1      0x0002
#define GPIO_PIN_2      0x0004
#define GPIO_PIN_3      0x0008
#define GPIO_PIN_4      0x0010
#define GPIO_PIN_5      0x0020
#define GPIO_PIN_6      0x0040
#define GPIO_PIN_7      0x0080
#define GPIO_PIN_8      0x0100
#define GPIO_PIN_9      0x0200
#define GPIO_PIN_10     0x0400
#define GPIO_PIN_11     0x0800
#define GPIO_PIN_12     0x1000
#define GPIO_PIN_13     0x2000
#define GPIO_PIN_14     0x4000
#define GPIO_PIN_15     0x8000
#define GPIO_PIN_ALL    0xFFFF

/* GPIO Modes define */
#define GPIO_Mode_HI_AIN_TRI    0x0001
#define GPIO_Mode_IN_TRI_TTL    0x0002
#define GPIO_Mode_INOUT_WP      0x0003
#define GPIO_Mode_OUT_OD        0x0004
#define GPIO_Mode_OUT_PP        0x0005
#define GPIO_Mode_AF_OD         0x0006
#define GPIO_Mode_AF_PP         0x0007

/* GPIO MSB and LSB Bytes define */
#define GPIO_LSB   0x0000
#define GPIO_MSB   0x0008

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void GPIO_DeInit(GPIO_TypeDef* GPIOx);
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);
void GPIO_BitWrite(GPIO_TypeDef* GPIOx, u16 GPIO_Pin, BitAction Bit_Val);
u8 GPIO_BitRead(GPIO_TypeDef* GPIOx, u16 GPIO_Pin);
void GPIO_ByteWrite(GPIO_TypeDef* GPIOx, u8 GPIO_Byte, u8 Byte_Val);
u8 GPIO_ByteRead(GPIO_TypeDef* GPIOx, u8 GPIO_Byte);
void GPIO_WordWrite(GPIO_TypeDef* GPIOx, u16 Port_Val);
u16 GPIO_WordRead(GPIO_TypeDef* GPIOx);

#endif /* __73x_GPIO_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
