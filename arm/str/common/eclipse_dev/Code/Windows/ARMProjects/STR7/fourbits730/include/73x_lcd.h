/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_lcd.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      LCD software library.
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

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __73x_LCD_H
#define __73x_LCD_H

/* Includes ----------------------------------------------------------------- */
#include "73x_lib.h"
#include "string.h"

/* Exported types ----------------------------------------------------------- */

#define LCD_LENGTH 32
#define LCD_HALF_LENGTH (LCD_LENGTH/2)

#define LCD_PORT_COMMAND       GPIO6
#define LCD_PORT_DATA          GPIO6

#define LCD_PIN_E     GPIO_PIN_0
#define LCD_PIN_RS    GPIO_PIN_1
#define LCD_PIN_RW    GPIO_PIN_10

#define LCD_CMD_4BIT     0x20

#define LCD_CMD_CLR_DISP        0x01
#define LCD_CMD_CRS_HOME        0x02
#define LCD_CMD_DECREM_NOSHIFT  0x04
#define LCD_CMD_DECREM_SHIFT    0x05
#define LCD_CMD_INCREM_NOSHIFT  0x06
#define LCD_CMD_INCREM_SHIFT    0x07

#define LCD_CMD_DISPOFF_CURSOFF_BLKOFF   0x08
#define LCD_CMD_DISPOFF_CURSOFF_BLKON    0x09
#define LCD_CMD_DISPOFF_CURSON_BLKOFF    0x0A
#define LCD_CMD_DISPOFF_CURSON_BLKON     0x0B
#define LCD_CMD_DISPON_CURSOFF_BLKOFF    0x0C
#define LCD_CMD_DISPON_CURSOFF_BLKON     0x0D
#define LCD_CMD_DISPON_CURSON_BLKOFF     0x0E
#define LCD_CMD_DISPON_CURSON_BLKON      0x0F

#define LCD_CMD_CURSMOVE_SHIFTL        0x10
#define LCD_CMD_CURSMOVE_SHIFTR        0x14
#define LCD_CMD_DISPSHIFT_SHIFTL       0x18
#define LCD_CMD_DISPSHIFT_SHIFTR       0x1C

#define LCD_CMD_4BIT_1LINE_8DOT     0x20
#define LCD_CMD_4BIT_1LINE_10DOT    0x24
#define LCD_CMD_4BIT_2LINE_8DOT     0x28
#define LCD_CMD_4BIT_2LINE_10DOT    0x2C


/* Module private variables ----------------------------------------------------*/

/* Exported macro --------------------------------------------------------------*/

/* Private functions -----------------------------------------------------------*/

/* Exported functions ----------------------------------------------------------*/
void LCD_Init(void);
void LCD_E_High(void);
void LCD_E_Low(void);
void LCD_Send_Command_4bit(u8 Cmd);
void LCD_SendData_4bits(u8 Data);
void LCD_SendString(u8 *pBuffer, u8 Line, u8 Column);
void LCD_SendStringByStep(u8 *pBuffer, u8 Line, u8 Column);
void LCD_SendStringPinPong(u8 *pBuffer, u8 Line, u32 nTime);
void LCD_SetPosCur(u8 Line, u8 Column);
void LCD_Clear(void);
void LCD_LineClear(u8 Line);
void LCD_DigitDisplay(u8 Digit, u8 Line, u8 Column);
void LCD_Wait(u32 nTime);

#endif /*__73x_LCD_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE******/


