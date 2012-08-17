/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_wdg.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      WDG software library.
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
#ifndef __73x_WDG_H
#define __73x_WDG_H

/* Includes ------------------------------------------------------------------*/
#include "73x_map.h"

/* Exported types ------------------------------------------------------------*/
 typedef struct
{
  u16 WDG_Mode;       /* Watchdog or Timer mode*/
  u16 WDG_CLK_Source; /* CLK Source */
  u16 WDG_Prescaler;  /* Prescaler register*/
  u16 WDG_Preload;    /* Preload register*/
}WDG_InitTypeDef;

/* Exported constants --------------------------------------------------------*/
/*WDG/Timer Select*/
#define WDG_Mode_WDG   0x0001
#define WDG_Mode_Timer 0x0000

/*WDG Start/Stop counter*/
#define WDG_Counter_Start 0x0002
#define WDG_Counter_Stop  0xFFFD

/*WDG EXT_CK/CK Select*/
#define WDG_CLK_EXTERNAL 0x0004
#define WDG_CLK_INTERNAL 0x0000

/*WDG Enable/Disable end of count interrupt request*/
#define WDG_ECIT_Enable 0x0001
#define WDG_ECIT_Disable 0xFFFE

/*WDG Sequence*/
#define WDG_KeyValue1 0xA55A
#define WDG_KeyValue2 0x5AA5

/*Initialize register*/
#define  WDG_CR_MASK 0xFFFA
#define  WDG_PR_MASK 0x00FF
#define  WDG_VR_MASK 0xFFFF

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void WDG_DeInit(void);
void WDG_StructInit(WDG_InitTypeDef *WDG_InitStruct);
void WDG_Init(WDG_InitTypeDef  *WDG_InitStruct);
void WDG_ITConfig(FunctionalState NewState);
u16 WDG_GetCounter(void);
void WDG_Cmd(FunctionalState NewState);
FlagStatus WDG_FlagStatus(void);
void WDG_FlagClear(void);


#endif /* __73x_WDG_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

