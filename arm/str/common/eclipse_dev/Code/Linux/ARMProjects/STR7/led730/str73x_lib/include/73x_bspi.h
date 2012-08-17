/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_bspi.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      BSPI software library.
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
#ifndef __73x_BSPI_H
#define __73x_BSPI_H

/* Includes --------------------------------------------------------------------*/
#include "73x_map.h"

/* Exported types --------------------------------------------------------------*/
/* BSPI Init structure definition */
typedef struct
{
  u8  BSPI_RxFifoSize ;
  u8  BSPI_TxFifoSize ;
  u8  BSPI_SSPin ;
  u8  BSPI_CLKDivider ;
  u16 BSPI_CPOL ;
  u16 BSPI_CPHA ;
  u16 BSPI_WordLength ;
  u16 BSPI_Mode ;
}BSPI_InitTypeDef;

/* Exported constants ---------------------------------------------------------*/
/* BSPI System Enable */
#define BSPI_Enable     0x0001
#define BSPI_Disable    0xFFFE

/* BSPI Master/Slave Select */
#define BSPI_Mode_Master    0x0002
#define BSPI_Mode_Slave     0x0000

/* BSPI Receive Interrupt */
#define BSPI_RxIT_NONE    0x0000
#define BSPI_RxIT_RFNE    0x0004
#define BSPI_RxIT_RFF     0x000C
#define BSPI_RxIT_Mask    0xFFF3

/* BSPI Interrupts */
#define BSPI_IT_REI    0x0010
#define BSPI_IT_BEI    0x0080

/* BSPI Clock Polarity */
#define BSPI_CPOL_Low     0x0100
#define BSPI_CPOL_High    0x0000

/* BSPI Clock Phase */
#define  BSPI_CPHA_1Edge    0x0000
#define  BSPI_CPHA_2Edge    0x0200

/* BSPI Word Length */
#define BSPI_WordLength_16b    0x0400
#define	BSPI_WordLength_8b     0x0000

/* BSPI Transmit Interrupt */
#define BSPI_TxIT_NONE    0x0000
#define BSPI_TxIT_TFE     0x4000
#define BSPI_TxIT_TUFL    0x8000
#define BSPI_TxIT_TFF     0xC000
#define BSPI_TxIT_Mask    0x3FFF
 
/* BSPI Flags */
#define BSPI_FLAG_BERR    0x004
#define BSPI_FLAG_RFNE    0x008
#define BSPI_FLAG_RFF     0x010
#define BSPI_FLAG_ROFL    0x020
#define BSPI_FLAG_TFE     0x040
#define BSPI_FLAG_TUFL    0x080
#define BSPI_FLAG_TFF     0x100
#define BSPI_FLAG_TFNE    0x200

/* BSPI DMA Interface Enable */
#define BSPI_DMA_Enable     0x02
#define BSPI_DMA_Disable    0xFD

/* BSPI DMA Transmit Request */
#define BSPI_DMAReq_Tx    0x40

/* BSPI DMA Receive Request */
#define BSPI_DMAReq_Rx    0x80

/* BSPI DMA Transmit Burst Length */
#define BSPI_DMA_TxBurst_1Word     0x00
#define BSPI_DMA_TxBurst_4Word     0x04
#define BSPI_DMA_TxBurst_8Word     0x08
#define BSPI_DMA_TxBurst_16Word    0x0C
#define BSPI_DMA_TxBurst_Mask      0xF3

/* BSPI DMA Receive Burst Length */
#define BSPI_DMA_RxBurst_1Word     0x00
#define BSPI_DMA_RxBurst_4Word     0x10
#define BSPI_DMA_RxBurst_8Word     0x20
#define BSPI_DMA_RxBurst_16Word    0x30
#define BSPI_DMA_RxBurst_Mask      0xCF

/* BSPI Mask Slave Select */
#define BSPI_SSPin_Masked    0x01
#define BSPI_SSPin_Used      0x00

/* BSPI receive FIFO disable */ 
#define BSPI_DFIFO    0x01

#define BSPI_RxFifoSize_Mask   0x0FFF
#define BSPI_TxFifoSize_Mask   0xC3FF

#define BSPI_CSR1_Mask    0x00FD
#define BSPI_CSR2_Mask    0xC3FF
#define BSPI_CSR3_Mask    0xFFFE

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void BSPI_DeInit(BSPI_TypeDef* BSPIx);
void BSPI_StructInit(BSPI_InitTypeDef* BSPI_InitStruct);
void BSPI_Init(BSPI_TypeDef* BSPIx, BSPI_InitTypeDef* BSPI_InitStruct);
void BSPI_Cmd(BSPI_TypeDef* BSPIx, FunctionalState NewState);
void BSPI_ITConfig(BSPI_TypeDef* BSPIx, u16 BSPI_IT, FunctionalState NewState);
void BSPI_TxITConfig(BSPI_TypeDef* BSPIx, u16 BSPI_TxIT);
void BSPI_RxITConfig(BSPI_TypeDef* BSPIx, u16 BSPI_RxIT);
void BSPI_DMAConfig(BSPI_TypeDef* BSPIx, u8 BSPI_DMAReq, FunctionalState NewState);
void BSPI_DMATxBurstConfig(BSPI_TypeDef* BSPIx, u8 BSPI_DMA_TxBurst);
void BSPI_DMARxBurstConfig(BSPI_TypeDef* BSPIx, u8 BSPI_DMA_RxBurst);
void BSPI_DMACmd(BSPI_TypeDef* BSPIx, FunctionalState NewState);
void BSPI_WordSend(BSPI_TypeDef* BSPIx, u16 Data);
u16 BSPI_WordReceive(BSPI_TypeDef* BSPIx);
ErrorStatus BSPI_BufferReceive(BSPI_TypeDef* BSPIx, u8* PtrToBuffer, u8 NbOfBytes);
ErrorStatus BSPI_BufferSend(BSPI_TypeDef* BSPIx, u8* PtrToBuffer, u8 NbOfBytes);
void BSPI_RxFifoDisable(BSPI_TypeDef* BSPIx);
FlagStatus BSPI_FlagStatus(BSPI_TypeDef* BSPIx, u16 BSPI_Flag);

#endif /* __73x_BSPI_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
