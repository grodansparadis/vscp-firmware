/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_wut.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      WUT software library.
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
#ifndef __73x_WUT_H
#define __73x_WUT_H
/* Includes ------------------------------------------------------------------*/

#include "73x_map.h"

/* Exported types ------------------------------------------------------------*/
/* WUT Init structure define */
typedef struct
{ 
  u16 WUT_Mode;
  u16 WUT_CLK_Source;
  u16 WUT_Prescaler;
  u16 WUT_Preload;
}WUT_InitTypeDef;

/* Exported constants --------------------------------------------------------*/
/* WUT mode */
#define WUT_Mode_WakeUp    0x0008
#define WUT_Mode_Timer     0xFFF7

/* WUT clock selection */
#define WUT_CLK_EXTERNAL    0x0004
#define WUT_CLK_INTERNAL    0xFFFB

/* WUT Flags */
#define WUT_FLAG_BSYVR    0x08    /* Preload Value Register Busy flag */
#define WUT_FLAG_BSYPR    0x04    /* Prescaler Register Busy flag */
#define WUT_FLAG_BSYCR    0x02    /* Control Register Busy flag */
#define WUT_FLAG_EC       0x01    /* End of Count pending flag */

/* Enable and disable masks for the WUT counter */
#define WUT_COUNTER_Start    0x0002
#define WUT_COUNTER_Stop     0xFFFD

/* Enable and disable masks for the end of count WUT interrupt */
#define WUT_IT_ENABLE   0x01
#define WUT_IT_DISABLE  0xFFFE

/* Masks for the Busy Bit */
#define WUT_BSYMSK_SET    0x02
#define WUT_BSYMSK_RESET  0xFFFD

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void WUT_DeInit(void);
void WUT_StructInit(WUT_InitTypeDef* WUT_InitStruct);
void WUT_Init(WUT_InitTypeDef* WUT_InitStruct);
void WUT_Cmd(FunctionalState NewState);
void WUT_ITConfig(FunctionalState NewState);
void WUT_BusySignalConfig(FunctionalState NewState);
u16 WUT_GetCounterValue(void);
FlagStatus WUT_FlagStatus(u8 WUT_Flag);
void WUT_FlagClear(void);


#endif /* __73x_WUT_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
