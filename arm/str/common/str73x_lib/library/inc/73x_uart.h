/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_uart.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      UART software library.
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
#ifndef __73x_UART_H
#define __73x_UART_H

/* Includes ------------------------------------------------------------------*/
#include "73x_map.h"

/* Exported types ------------------------------------------------------------*/
/* UART Init structure definition */
typedef struct
{
    u32 UART_BaudRate;      /*Configure BaudRate register */
    u16 UART_Mode;          /*Choose Mode*/
    u16 UART_StopBits;      /*Choose Stop Bits*/
    u16 UART_Parity;        /*Choose Parity*/
    u16 UART_Loop_Standard; /*Choose loopBack or Standard transmit/receive mode*/
    u16 UART_Rx;            /*Enable or disable receive mode*/
    u16 UART_FIFO;          /*Enable or disable FIFO mode*/
} UART_InitTypeDef;

/* Exported constants --------------------------------------------------------*/
/*UART Mode*/

#define  UART_Mode_8D    0x0001
#define  UART_Mode_7D_P  0x0003
#define  UART_Mode_9D    0x0004
#define  UART_Mode_8D_W  0x0005
#define  UART_Mode_8D_P  0x0007

/*UART StopBits*/
#define  UART_StopBits_0_5  0x0000
#define  UART_StopBits_1    0x0008
#define  UART_StopBits_1_5  0x0010
#define  UART_StopBits_2    0x0018

/*UART Parity*/
#define  UART_Parity_Even  0x0000
#define  UART_Parity_Odd   0x0020

/*UART Loop_Standard*/
#define  UART_LoopBack 0x0040
#define  UART_Standard 0x0000

/*UART Rx*/
#define UART_Rx_Enable  0x0100
#define UART_Rx_Disable 0x0000

/*UART FIFO*/
#define UART_FIFO_Enable  0x0400
#define UART_FIFO_Disable 0x0000


/*UART Reset FIFO*/
#define  UART_RxFIFO 0xFFFF
#define  UART_TxFIFO 0x0000

/*UART RUN*/
#define  UART_RUN_Enable_Mask  0x0080
#define  UART_RUN_Disable_Mask 0xFF7F

#define DUMMY 0

/*UART flags definition*/
#define UART_Flag_TxFull          0x0200
#define UART_Flag_RxHalfFull      0x0100
#define UART_Flag_TimeOutIdle     0x0080
#define UART_Flag_TimeOutNotEmpty 0x0040
#define UART_Flag_OverrunError    0x0020
#define UART_Flag_FrameError      0x0010
#define UART_Flag_ParityError     0x0008
#define UART_Flag_TxHalfEmpty     0x0004
#define UART_Flag_TxEmpty         0x0002
#define UART_Flag_RxBufFull       0x0001

/*UART Interrupt definition*/
#define UART_IT_TxFull          0x0200
#define UART_IT_RxHalfFull      0x0100
#define UART_IT_TimeOutIdle     0x0080
#define UART_IT_TimeOutNotEmpty 0x0040
#define UART_IT_OverrunError    0x0020
#define UART_IT_FrameError      0x0010
#define UART_IT_ParityError     0x0008
#define UART_IT_TxHalfEmpty     0x0004
#define UART_IT_TxEmpty         0x0002
#define UART_IT_RxBufFull       0x0001

#define UART_BR_MASK 0x0001
#define UART_CR_MASK 0xFA80

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void UART_DeInit(UART_TypeDef *UARTx);
void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef* UART_InitStruct);
void UART_StructInit(UART_InitTypeDef* UART_InitStruct);
void UART_ByteBufferSend(UART_TypeDef *UARTx, u8* PtrToBuffer, u8 NbOfBytes);
void UART_9BitByteBufferSend(UART_TypeDef *UARTx, u16* PtrToBuffer, u8 NbOfBytes);
void UART_StringSend(UART_TypeDef *UARTx, u8 *PtrToString);
ErrorStatus UART_ByteBufferReceive(UART_TypeDef *UARTx, u8 *PtrToBuffer, u8 NbOfBytes);
ErrorStatus UART_9BitBufferReceive(UART_TypeDef *UARTx, u16 *PtrToBuffer, u8 NbOfBytes);
void UART_StringReceive(UART_TypeDef *UARTx, u8 *PtrToString);
void UART_SetTimeOutValue(UART_TypeDef *UARTx, u16 UART_TimeOut);
u8 UART_ByteReceive(UART_TypeDef *UARTx);
u16 UART_9BitReceive(UART_TypeDef *UARTx);
void UART_ByteSend(UART_TypeDef *UARTx, u8 data);
void UART_9BitSend(UART_TypeDef *UARTx, u16 data);
void UART_Cmd(UART_TypeDef *UARTx, FunctionalState NewState);
void UART_FifoReset(UART_TypeDef *UARTx, u16 FIFO_Reset);
void UART_ITConfig(UART_TypeDef *UARTx,u16 UART_IT, FunctionalState NewState);
FlagStatus UART_FlagStatus(UART_TypeDef *UARTx, u16 UART_Flag);

#endif /* __73x_UART_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
