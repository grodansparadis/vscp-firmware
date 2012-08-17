/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_rtc.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      RTC software library.
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
#ifndef __73x_RTC_H
#define __73x_RTC_H

/* Includes ----------------------------------------------------------------- */
#include "73x_map.h"

/* Exported types ----------------------------------------------------------- */
/* RTC Init structure define */
typedef struct
{
  u32 RTC_Alarm;
  u32 RTC_Counter;
  u32 RTC_Prescaler;
} RTC_InitTypeDef;

/* Exported constants --------------------------------------------------------*/
/* RTC Interrupt defines */
#define RTC_IT_GL     0x08
#define RTC_IT_OV     0x04
#define RTC_IT_ALA    0x02
#define RTC_IT_SEC    0x01
#define RTC_IT_ALL    0x0F

/* RTC Flags defines */
#define RTC_FLAG_GL     0x08
#define RTC_FLAG_OV     0x04
#define RTC_FLAG_ALA    0x02
#define RTC_FLAG_SEC    0x01

/* Configuration Flag Mask */
#define RTC_CNF_Mask  0x0010

/* Last task finished Mask */
#define RTC_RTOFF_Mask   0x0020

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void RTC_Init(RTC_InitTypeDef* RTC_InitStruct);
void RTC_DeInit(void);
void RTC_StructInit(RTC_InitTypeDef* RTC_InitStruct);
void RTC_SetCounter(u32 RTC_Counter);
u32 RTC_GetCounterValue(void);
void RTC_SetPrescaler(u32 RTC_prescaler);
u32 RTC_GetPrescalerValue(void);
void RTC_SetAlarm(u32 RTC_alarm);
u32 RTC_GetAlarmValue(void);
void RTC_FlagClear(u16 RTC_Flag);
FlagStatus RTC_FlagStatus (u16 RTC_Flag);
void RTC_ITConfig(u16 RTC_It, FunctionalState NewState);
FunctionalState RTC_ITStatus(u16 RTC_It);
void RTC_WaitForLastTask(void);
void RTC_EnterCfgMode(void);
void RTC_ExitCfgMode(void);

#endif /* __73x_RTC_H  */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
