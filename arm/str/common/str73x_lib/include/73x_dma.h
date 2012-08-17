/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_dma.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      DMA software library.
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
#ifndef __73x_DMA_H
#define __73x_DMA_H

/* Includes ------------------------------------------------------------------*/
#include "73x_map.h"

/* Exported types ------------------------------------------------------------*/
/* DMA Init structure define */
typedef struct
{
  u8  DMA_Stream;
  u16 DMA_BufferSize;	
  u16 DMA_SRC;   
  u16 DMA_DST; 
  u16 DMA_SRCSize;
  u16 DMA_SRCBurst;
  u16 DMA_DSTSize;
  u16 DMA_Mode;  
  u16 DMA_M2M; 
  u16 DMA_Dir; 
  u32 DMA_SRCBaseAddr;
  u32 DMA_DSTBaseAddr;	
  u32 DMA_TriggeringSource;
}DMA_InitTypeDef;

/* Exported constants --------------------------------------------------------*/
/* DMAx data stream */
#define DMA_Stream0    0x00
#define DMA_Stream1    0x40
#define DMA_Stream2    0x80
#define DMA_Stream3    0xC0

/* DMA enable */
#define DMA_Enable     0x0001
#define DMA_Disable    0xFFFE

/* DMAx interrupt Mask */
#define DMA_IT_SI0    0x01
#define DMA_IT_SI1    0x02
#define DMA_IT_SI2    0x04
#define DMA_IT_SI3    0x08
#define DMA_IT_SE0    0x10
#define DMA_IT_SE1    0x20
#define DMA_IT_SE2    0x40
#define DMA_IT_SE3    0x80
#define DMA_IT_ALL    0xFF

/* DMAx Flags */
#define DMA_FLAG_INT0  	0x0001  
#define DMA_FLAG_INT1	0x0002
#define DMA_FLAG_INT2	0x0004
#define DMA_FLAG_INT3	0x0008
#define DMA_FLAG_ERR0  	0x0010  
#define DMA_FLAG_ERR1	0x0020
#define DMA_FLAG_ERR2	0x0040
#define DMA_FLAG_ERR3	0x0080
#define DMA_FLAG_ACT0  	0x0100  
#define DMA_FLAG_ACT1	0x0200
#define DMA_FLAG_ACT2	0x0400
#define DMA_FLAG_ACT3	0x0800

/* DMAx Increment Current Source Register */
#define DMA_SRC_INCR        0x0002
#define DMA_SRC_NOT_INCR    0x0000

/* DMAx Increment Current Destination Register */
#define DMA_DST_INCR        0x0004
#define DMA_DST_NOT_INCR    0x0000

/* Source to DMA data width */
#define DMA_SRCSize_Byte        0x0000
#define DMA_SRCSize_HalfWord    0x0008
#define DMA_SRCSize_Word        0x0010

/* DMA peripheral burst size */
#define DMA_SRCBurst_1Word     0x0000
#define DMA_SRCBurst_4Word     0x0020
#define DMA_SRCBurst_8Word     0x0040
#define DMA_SRCBurst_16Word    0x0060

/* DMA to Destination data width */
#define DMA_DSTSize_Byte        0x0000
#define DMA_DSTSize_HalfWord    0x0080
#define DMA_DSTSize_Word        0x0100

/* DMA mode */
#define DMA_Mode_Circular    0x0200
#define DMA_Mode_Normal      0x0000

/* Memory to Memory Transfer */
#define DMA_M2M_Enable     0x0800
#define DMA_M2M_Disable    0x0000

/* Direction Transfer */
#define DMA_Dir_PeriphDST    0x2000
#define DMA_Dir_PeriphSRC    0x0000

/* Selection of the triggering source for DMA0 Stream2, DMA0 Stream3,
   DMA2 Stream3 and DMA3 Stream2 */
#define DMA_TriggeringSource_BSPI0        0x00000010
#define DMA_TriggeringSource_BSPI1        0x00000020
#define DMA_TriggeringSource_TIM8_TIM9    0xFFFFFFDF

/* DMA Priority (defined in Native Arbiter PRIOR register) */
#define DMA_Priority_High    0x03
#define DMA_Priority_Low     0x00

/* Module private variables --------------------------------------------------*/
/* DMA streamx Registers */
#define DMA_SOURCEL    0x00      /* DMA source base address low register */
#define DMA_SourceH    0x04      /* DMA source base address high register */
#define DMA_DESTL      0x08      /* DMA destination base address low register */
#define DMA_DESTH      0x0C      /* DMA destination base address high register */
#define DMA_MAX        0x10      /* DMA Maximum count register */
#define DMA_CTRL       0x14      /* DMA Control register */
#define DMA_SOCURRH    0x18      /* DMA Current Source address high register */
#define DMA_SOCURRL    0x1C      /* DMA Current Source address low register */
#define DMA_DECURRH    0x20      /* DMA Current Destination address high register */
#define DMA_DECURRL    0x24      /* DMA Current Destination address low register */
#define DMA_TCNT       0x28      /* DMA Terminal Counter Register */
#define DMA_LUBUFF     0x2C      /* DMA Last Used Buffer location */

/* DMA Last Buffer Sweep */
#define DMA_LAST0    0x01
#define DMA_LAST1    0x02
#define DMA_LAST2    0x04
#define DMA_LAST3    0x08

/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void DMA_DeInit(DMA_TypeDef* DMAx, u8 DMA_Stream);
void DMA_Init(DMA_TypeDef* DMAx, DMA_InitTypeDef* DMA_InitStruct);
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct);
void DMA_AHBArbitrationConfig(u8 DMA_Priority);
void DMA_Cmd(DMA_TypeDef* DMAx, u8 DMA_Stream, FunctionalState NewState);
void DMA_ITConfig(DMA_TypeDef* DMAx, u16 DMA_IT, FunctionalState NewState);
u32 DMA_GetCurrDSTAddr(DMA_TypeDef* DMAx, u8 DMA_Stream);
u32 DMA_GetCurrSRCAddr(DMA_TypeDef* DMAx, u8 DMA_Stream);
u16 DMA_GetTerminalCounter(DMA_TypeDef* DMAx, u8 DMA_Stream);
void DMA_LastBufferSweepConfig(DMA_TypeDef* DMAx, u8 DMA_Stream);
void DMA_LastBufferAddrConfig(DMA_TypeDef* DMAx, u8 DMA_Stream, u16 DMA_LastBufferAddr);
void DMA_TimeOutConfig(u16 TimeOut_Value);
FlagStatus DMA_FlagStatus(DMA_TypeDef* DMAx, u16 DMA_Flag);
void DMA_FlagClear(DMA_TypeDef* DMAx, u16 DMA_Flag);

#endif /* __73x_DMA_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
