/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_i2c.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      I2C software library.
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
#ifndef __73x_I2C_H
#define __73x_I2C_H

/* Includes ----------------------------------------------------------------- */
#include "73x_map.h"
#include "73x_prccu.h"

/* Exported types ----------------------------------------------------------- */
/* I2C Init structure define */
typedef struct
{
   u8  I2C_GeneralCall;
   u8  I2C_Ack;
   u16 I2C_OwnAddress;
   u32 I2C_CLKSpeed;
}I2C_InitTypeDef;

/* Exported constants --------------------------------------------------------*/
/* General Call */
#define  I2C_GeneralCallEnable      0x10
#define  I2C_GeneralCallDisable     0x00

/* Acknowledgement */
#define  I2C_AckEnable      0x04
#define  I2C_AckDisable     0x00

/* Fast Standard Mode */
#define  I2C_CLKMode_Fast         0x80
#define  I2C_CLKMode_Standard     0x00

/* I2C IT enable */
#define  I2C_ITEnable     0x01
#define  I2C_ITDisable    0xFE

/* I2C Flags */
#define  I2C_FLAG_SB      0x0001
#define  I2C_FLAG_M_SL    0x0002
#define  I2C_FLAG_ADSL    0x0004
#define  I2C_FLAG_BTF     0x0008
#define  I2C_FLAG_BUSY    0x0010
#define  I2C_FLAG_TRA     0x0020
#define  I2C_FLAG_ADD10   0x0040
#define  I2C_FLAG_EVF     0x0080
#define  I2C_FLAG_GCAL    0x0100
#define  I2C_FLAG_BERR    0x0200
#define  I2C_FLAG_ARLO    0x0400
#define  I2C_FLAG_STOPF   0x0800
#define  I2C_FLAG_AF      0x1000
#define  I2C_FLAG_ENDAD   0x2000
#define  I2C_FLAG_ACK     0x4000

/* I2C Events */
#define  I2C_EVENT_SLAVE_ADDRESS_MATCHED   ( I2C_FLAG_EVF | I2C_FLAG_BUSY | I2C_FLAG_ADSL)    
#define  I2C_EVENT_SLAVE_BYTE_RECEIVED     ( I2C_FLAG_EVF | I2C_FLAG_BUSY | I2C_FLAG_BTF )     
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTED  ( I2C_FLAG_EVF | I2C_FLAG_BUSY | I2C_FLAG_BTF  | I2C_FLAG_TRA ) 
#define  I2C_EVENT_MASTER_MODE_SELECT      ( I2C_FLAG_EVF | I2C_FLAG_BUSY | I2C_FLAG_M_SL | I2C_FLAG_SB )                 
#define  I2C_EVENT_MASTER_MODE_SELECTED    ( I2C_FLAG_EVF | I2C_FLAG_BUSY | I2C_FLAG_M_SL | I2C_FLAG_ENDAD )    
#define  I2C_EVENT_MASTER_BYTE_RECEIVED    ( I2C_FLAG_EVF | I2C_FLAG_BUSY | I2C_FLAG_M_SL | I2C_FLAG_BTF )                
#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED ( I2C_FLAG_EVF | I2C_FLAG_BUSY | I2C_FLAG_M_SL | I2C_FLAG_BTF | I2C_FLAG_TRA )  
#define  I2C_EVENT_MASTER_MODE_ADDRESS10   ( I2C_FLAG_EVF | I2C_FLAG_BUSY | I2C_FLAG_M_SL |I2C_FLAG_ADD10 )               
#define  I2C_EVENT_SLAVE_STOP_DETECTED       I2C_FLAG_STOPF                                  
#define  I2C_EVENT_SLAVE_ACK_FAILURE       ( I2C_FLAG_EVF | I2C_FLAG_BUSY | I2C_FLAG_BTF  | I2C_FLAG_TRA | I2C_FLAG_AF)    

/* I2C Events */
#define  I2C_BUS_ERROR_DETECTED       I2C_FLAG_BERR
#define  I2C_ARBITRATION_LOST         I2C_FLAG_ARLO
#define  I2C_SLAVE_GENERAL_CALL       (I2C_FLAG_BUSY | I2C_FLAG_GCAL)

/* I2C Addressing Modes */
#define  I2C_MODE_ADDRESS10   0x0
#define  I2C_MODE_ADDRESS7    0x1

/* Master/Receiver Mode */
#define  I2C_MODE_TRANSMITTER         0x00
#define  I2C_MODE_RECEIVER            0x01
#define  I2C_MODE_MASTER_TRANSMITTER  0x02
#define  I2C_MODE_MASTER_RECEIVER     0x03
#define  I2C_MODE_SLAVE_TRANSMITTER   0x04
#define  I2C_MODE_SLAVE_RECEIVER      0x05

/* I2C Enable, Disable, Start, Stop defines */
#define  I2C_PESET_Mask 0x20
#define  I2C_PERESET    0xDF
#define  I2C_START_Mask 0x08
#define  I2C_STOP_Mask  0x02
#define  I2C_ACK_Mask   0x04

/* I2C Registers offset */
#define  I2C_CR     0x00
#define  I2C_SR1    0x04
#define  I2C_SR2    0x08
#define  I2C_CCR    0x0C
#define  I2C_OAR1   0x10
#define  I2C_OAR2   0x14
#define  I2C_DR     0x18
#define  I2C_ECCR   0x1C

/* I2C Init Mask */
#define I2C_CR_Mask 0x01

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void I2C_Init (I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);
void I2C_DeInit (I2C_TypeDef* I2Cx);
void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct);
void I2C_Cmd (I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_STARTGenerate (I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_STOPGenerate (I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_AcknowledgeConfig (I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_ITConfig (I2C_TypeDef* I2Cx, FunctionalState NewState);
u16 I2C_GetStatus(I2C_TypeDef* I2Cx);
u16 I2C_GetLastEvent(I2C_TypeDef* I2Cx);
ErrorStatus I2C_EventCheck(I2C_TypeDef* I2Cx, u16 NewEvent );
void I2C_ByteSend (I2C_TypeDef* I2Cx, u8 I2C_Data);
u8 I2C_ByteReceive (I2C_TypeDef* I2Cx);
void I2C_AddressSend (I2C_TypeDef* I2Cx, u16 I2C_Address, u8 I2C_AddMode, u8 I2C_Direction);
ErrorStatus I2C_BufferSend (I2C_TypeDef* I2Cx, u8* PtrToBuffer, u8 NbOfBytes);
ErrorStatus I2C_BufferReceive (I2C_TypeDef* I2Cx, u8* PtrToBuffer, u8 NbOfBytes);
u8 I2C_RegisterRead (I2C_TypeDef* I2Cx, u16 I2C_Register);
FlagStatus I2C_FlagStatus (I2C_TypeDef* I2Cx, u16 I2C_Flag);
void I2C_FlagClear (I2C_TypeDef* I2Cx, u16 I2C_Flag, ...);

#endif /* __73x_I2C_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
