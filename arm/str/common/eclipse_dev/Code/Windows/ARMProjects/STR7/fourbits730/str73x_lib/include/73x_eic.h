/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : 73x_eic.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      EIC software library.
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
#ifndef __73x_EIC_H
#define __73x_EIC_H

/* Includes ------------------------------------------------------------------*/
#include "73x_map.h"

/* Exported types ------------------------------------------------------------*/
/* EIC Init structure define */
typedef struct
{
  u8 EIC_IRQChannel;             /* IRQ Interrupt Vector */
  u8 EIC_FIQChannel;             /* FIQ Interrupt Vector */
  FunctionalState EIC_IRQCmd;    /* ENABLE or DISABLE the selected IRQ Channels */
  FunctionalState EIC_FIQCmd;    /* ENABLE or DISABLE the selected FIQ Channels */
  u8 EIC_IRQChannelPriority;     /* EIC_IRQChannel priority */
}EIC_InitTypeDef;

/* External interrupt triggering edge/level */
typedef enum
{
  EXTIT_TRIGGER_Rising_Falling,
  EXTIT_TRIGGER_Rising,
  EXTIT_TRIGGER_Falling,
  EXTIT_TRIGGER_HIGH_Level,
  EXTIT_TRIGGER_LOW_Level
} EXTIT_TRIGGER_TypeDef;

/* Exported constants --------------------------------------------------------*/
/* IRQ channels defines*/
#define PRCCUCMU_IRQChannel    0
#define EXTIT01_IRQChannel     1
#define EXTIT02_IRQChannel     2
#define EXTIT03_IRQChannel     3
#define EXTIT04_IRQChannel     4
#define EXTIT05_IRQChannel     5
#define EXTIT06_IRQChannel     6
#define EXTIT07_IRQChannel     7
#define EXTIT08_IRQChannel     8
#define EXTIT09_IRQChannel     9
#define EXTIT10_IRQChannel     10
#define EXTIT11_IRQChannel     11
#define EXTIT12_IRQChannel     12
#define EXTIT13_IRQChannel     13
#define EXTIT14_IRQChannel     14
#define EXTIT15_IRQChannel     15
#define DMATRERR_IRQChannel    16
#define TIM1_IRQChannel        17
#define TIM2_IRQChannel        18
#define TIM3_IRQChannel        19
#define TIM4_IRQChannel        20
#define TB0_IRQChannel         21
#define TB1_IRQChannel         22
#define TB2_IRQChannel         23
#define TIM5_IRQChannel        24
#define TIM6_IRQChannel        25
#define TIM7_IRQChannel        26
#define TIM8_IRQChannel        27
#define TIM9_IRQChannel        28
#define UART2_IRQChannel       31
#define UART3_IRQChannel       32
#define FLASHEOP_IRQChannel    33
#define PWM0_IRQChannel        34
#define PWM1_IRQChannel        35
#define PWM2_IRQChannel        36
#define PWM3_IRQChannel        37
#define PWM4_IRQChannel        38
#define PWM5_IRQChannel        39
#define WIU_IRQChannel         40
#define WDGWUT_IRQChannel      41
#define BSPI0_IRQChannel       42
#define BSPI1_IRQChannel       43
#define BSPI2_IRQChannel       44
#define UART0_IRQChannel       45
#define UART1_IRQChannel       46
#define I2C0ITERR_IRQChannel   47
#define I2C1ITERR_IRQChannel   48
#define I2C0ITDDC_IRQChannel   51
#define I2C1ITDDC_IRQChannel   52
#define CAN0_IRQChannel        55
#define CAN1_IRQChannel        56
#define CAN2_IRQChannel        57
#define DMA0_IRQChannel        58
#define DMA1_IRQChannel        59
#define DMA2_IRQChannel        60
#define DMA3_IRQChannel        61
#define ADC_IRQChannel         62
#define RTC_IRQChannel         63

/* External Interrupts requests */

#define EXTERNAL_IT0       0x0001
#define EXTERNAL_IT1       0x0002
#define EXTERNAL_IT2       0x0004
#define EXTERNAL_IT3       0x0008
#define EXTERNAL_IT4       0x0010
#define EXTERNAL_IT5       0x0020
#define EXTERNAL_IT6       0x0040
#define EXTERNAL_IT7       0x0080
#define EXTERNAL_IT8       0x0100
#define EXTERNAL_IT9       0x0200
#define EXTERNAL_IT10      0x0400
#define EXTERNAL_IT11      0x0800
#define EXTERNAL_IT12      0x1000
#define EXTERNAL_IT13      0x2000
#define EXTERNAL_IT14      0x4000
#define EXTERNAL_IT15      0x8000


/* FIQ channels defines */
#define EXTIT01_FIQChannel     1
#define TIM0_FIQChannel        2

#define EIC_IRQEnable_Mask     0x01
#define EIC_IRQDisable_Mask    0xFFFE

#define EIC_FIQEnable_Mask     0x02
#define EIC_FIQDisable_Mask    0xFFFD

#define EIC_EXTIT_FILTER_Mask   0x04

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void EIC_DeInit(void);
void EIC_StructInit(EIC_InitTypeDef* EIC_InitStruct);
void EIC_Init( EIC_InitTypeDef* EIC_InitStruct);
void EIC_CurrentPriorityLevelConfig(u8 NewPriorityLevel);
void EIC_IRQChannelPriorityConfig(u8 IRQChannel, u8 Priority);
void EIC_IRQChannelConfig(u8 IRQChannel, FunctionalState NewStatus);
void EIC_FIQChannelConfig(u8 FIQChannel,FunctionalState NewStatus);
void EIC_IRQCmd(FunctionalState NewState);
void EIC_FIQCmd(FunctionalState NewState);
u8 EIC_CurrentPriorityLevelValue(void);
u8 EIC_CurrentIRQChannelValue(void);
u8 EIC_CurrentFIQChannelValue(void);
void EIC_FIQPendingBitClear(u8 FIQChannel);
void EIC_ExternalITFilterConfig(FunctionalState NewStatus);
void EIC_ExternalITTriggerConfig(u16 EXTERNAL_IT, EXTIT_TRIGGER_TypeDef EXTIT_Trigger);


#endif /* __73x_EIC_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

