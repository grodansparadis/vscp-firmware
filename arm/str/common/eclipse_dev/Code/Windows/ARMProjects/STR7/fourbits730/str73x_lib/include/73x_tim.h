/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_tim.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      TIM software library.
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
#ifndef __73x_TIM_H
#define __73x_TIM_H

/* Includes ----------------------------------------------------------------- */
#include "73x_map.h"

/* Exported types ----------------------------------------------------------- */
/* TIM Init structure define */
typedef struct
{             
  u16 TIM_Mode;            /* Timer mode */
  u16 TIM_OCA_Modes;       /* Output Compare A Mode: Timing or Wave */
  u16 TIM_OCB_Modes;       /* Output Compare B Mode: Timing or Wave */
  u16 TIM_Clock_Source;    /* Timer Clock source: Internal or External */
  u16 TIM_Clock_Edge;      /* Timer Clock Edge: Rising or Falling Edge */
  u16 TIM_INPUT_Edge;      /* Timer Input Capture A Edge used in One Pulse Mode */ 
  u16 TIM_ICAPA_Modes;     /* Timer Input Capture A Edge used in ICAPA Mode */
  u16 TIM_ICAPB_Modes;     /* Timer Input Capture B Edge used in ICAPB Mode */
  u8  TIM_Prescaler;       /* Timer Prescaler factor */
  u16 TIM_Pulse_Level_A;   /* Level applied on the Output Compare Pin A */
  u16 TIM_Pulse_Level_B;   /* Level applied on the Output Compare Pin B */ 
  u16 TIM_Period_Level;    /* Level applied during the Period of a PWM Mode */
  u16 TIM_Pulse_Length_A;  /* Pulse A Length used in Output Compare A Mode */
  u16 TIM_Pulse_Length_B;  /* Pulse B Length used in Output Compare B Mode */
  u16 TIM_Full_Period;     /* Period Length used in PWM Mode */
} TIM_InitTypeDef;

typedef enum 
{ 
  TIM_START,
  TIM_STOP,
  TIM_CLEAR
} TIM_CounterOperations;

/* Exported constants --------------------------------------------------------*/

/*TIM MODE*/
#define TIM_PWMI      	    0x4000 /* PWM INPUT Mode */
#define TIM_OCM_CHANNELA    0x0100 /* OUTPUT COMPARE CHANNEL A Mode */
#define TIM_OCM_CHANNELB    0x0200 /* OUTPUT COMPARE CHANNEL B Mode */
#define TIM_OCM_CHANNELAB   0x0300 /* OUTPUT COMPARE CHANNEL A & CHANNEL B  Mode */
#define TIM_PWM             0x0010 /* PWM Mode */
#define TIM_OPM             0x0020 /* ONE PULSE Mode */
#define TIM_ICAP_CHANNELA   0x0400 /* INPUT CAPTURE A Mode */
#define TIM_ICAP_CHANNELB   0x0500 /* INPUT CAPTURE B Mode */
#define TIM_ICAP_CHANNELAB  0x0600 /* INPUT CAPTURE A & B Mode */

/*OUTPUT COMPARE MODE*/
#define TIM_Wave       0x0080
#define TIM_Timing     0x0800

/*CLOCK SOURCE*/
#define TIM_CLK_INTERNAL     0x8002
#define TIM_CLK_EXTERNAL     0x8001
#define TIM_ICAP_NONE        0x0000 
#define TIM_ICAP_0           0x0001
#define TIM_ICAP_1           0x0002
#define TIM_ICAP_2           0x0004
#define TIM_ICAP_3           0x0008
#define TIM_ICAP_4           0x0010
#define TIM_ICAP_5           0x0020
#define TIM_ICAP_6           0x0040
#define TIM_ICAP_7           0x0080
#define TIM_ICAP_8           0x0100
#define TIM_ICAP_9           0x0200

/*CLOCK EDGE*/
#define TIM_CLKEDGE_Falling  0x0004
#define TIM_CLKEDGE_Rising   0x0002

/*OUTPUT LEVEL*/
#define TIM_High       0x0200
#define TIM_Low        0x0300

/*OUTPUT EDGE*/
#define TIM_Rising     0x8000
#define TIM_Falling    0x0800

/*DMA SOURCE*/
#define TIM_DMA_ICA        0x0000 /* Input Capture Channel A DMA Source */
#define TIM_DMA_OCA        0x1000 /* OUTPUT Compare Channel A DMA Source */
#define TIM_DMA_ICB        0x2000 /* Input Capture Channel B DMA Source */
#define TIM_DMA_OCB        0x3000 /* OUTPUT Compare Channel A DMA Source */

/*DMA ENABLE/DISABLE*/
#define TIM_DMA_Enable      0x0400 /* DMA Enable */
#define TIM_DMA_Disable     0xFBFF /* DMA Disable */

/* Bits Masks */
#define TIM_ECK_ENABLE_Mask   0x0001
#define TIM_ECK_DISABLE_Mask  0xFFFE

#define TIM_EXEDG_ENABLE_Mask  0x0002
#define TIM_EXEDG_DISABLE_Mask 0xFFFD

#define TIM_IEDGA_ENABLE_Mask   0x0004
#define TIM_IEDGA_DISABLE_Mask  0xFFFB

#define TIM_IEDGB_ENABLE_Mask   0x0008
#define TIM_IEDGB_DISABLE_Mask  0xFFF7

#define TIM_PWM_Mask    0x0010
#define TIM_OMP_Mask    0x0020
#define TIM_OCA_ENABLE_Mask   0x0040
#define TIM_OCA_DISABLE_Mask  0xFFBF
#define TIM_OCB_ENABLE_Mask   0x0080
#define TIM_OCB_DISABLE_Mask  0xFF7F
#define TIM_PWMI_Mask   0x4000

#define TIM_OLVLA_Set_Mask    0x0100
#define TIM_OLVLA_Reset_Mask  0xFEFF

#define TIM_OLVLB_Set_Mask    0x0200
#define TIM_OLVLB_Reset_Mask  0xFDFF

#define TIM_ENABLE_Mask     0x8000
#define TIM_DISABLE_Mask    0x7FFF

/* TIM Interruption Sources*/
#define TIM_IT_ICA   0x8000 /* Input Capture Channel A Interrupt Source */ 
#define TIM_IT_OCA   0x4000 /* Output Compare Channel A Interrupt Source */
#define TIM_IT_TO    0x2000 /* Timer OverFlow Interrupt Source*/
#define TIM_IT_ICB   0x1000 /* Input Capture Channel B Interrupt Source */
#define TIM_IT_OCB   0x0800 /* Output Compare Channel B Interrupt Source */

/* TIM Flags */
#define TIM_FLAG_ICA     0x8000 /* Input Capture Channel A Flag */ 
#define TIM_FLAG_OCA     0x4000 /* Output Compare Channel A Flag */ 
#define TIM_FLAG_TO      0x2000 /* Timer OverFlow Flag */ 
#define TIM_FLAG_ICB     0x1000 /* Input Capture Channel B Flag */ 
#define TIM_FLAG_OCB     0x0800 /* Output Compare Channel B Flag */ 

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TIM_Init(TIM_TypeDef *TIMx, TIM_InitTypeDef  *TIM_InitStruct);
void TIM_DeInit(TIM_TypeDef *TIMx);
void TIM_StructInit(TIM_InitTypeDef *TIM_InitStruct);
void TIM_ClockSourceConfig(TIM_TypeDef *TIMx, u16 TIM_Clock);
void TIM_CounterCmd(TIM_TypeDef *TIMx, TIM_CounterOperations TIM_operation);
u16 TIM_GetClockStatus(TIM_TypeDef *TIMx);
void TIM_PrescalerConfig(TIM_TypeDef *TIMx, u8 TIM_Prescaler);
u8 TIM_GetPrescalerValue(TIM_TypeDef *TIMx);
u16 TIM_GetICAPAValue(TIM_TypeDef *TIMx);
u16 TIM_GetICAPBValue(TIM_TypeDef *TIMx);
FlagStatus TIM_FlagStatus(TIM_TypeDef *TIMx, u16 TIM_Flag);
void TIM_FlagClear(TIM_TypeDef *TIMx, u16 TIM_Flag);
u16 TIM_GetPWMIPulse(TIM_TypeDef  *TIMx);
u16 TIM_GetPWMIPeriod(TIM_TypeDef  *TIMx);
void TIM_ITConfig(TIM_TypeDef *TIMx, u16 TIM_IT, FunctionalState NewState);
void TIM_DMAConfig(TIM_TypeDef *TIMx, u16 TIM_DMA_Sources);
void TIM_DMACmd(TIM_TypeDef *TIMx, FunctionalState Newstate);

#endif	/*__TIM_H*/

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

