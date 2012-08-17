/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_tb.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      TB software library.
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
#ifndef __73x_TB_H
#define __73x_TB_H

/* Includes ----------------------------------------------------------------- */
#include "73x_map.h"

/* Exported types ----------------------------------------------------------- */
/* TB Init structure define */
typedef struct
{
    u16 TB_CLK_Source; /* Clock Source             */
    u16 TB_Prescaler;  /* Prescaler Register value */
    u16 TB_Preload;    /* Preload Register value   */
}TB_InitTypeDef;

/* Exported constants --------------------------------------------------------*/
/*CLOCK SOURCE*/
#define TB_CLK_INTERNAL     0x0000
#define TB_CLK_EXTERNAL     0x0004

/*TB Start/Stop counter*/
#define TB_COUNTER_Start 0x0002
#define TB_COUNTER_Stop  0xFFFD

/*TB Enable/Disable end of count interrupt request*/
#define TB_ECIT_Enable 0x0001
#define TB_ECIT_Disable 0xFFFE

#define TB_CR_Mask 0xFFFB

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TB_Init(TB_TypeDef *TBx, TB_InitTypeDef  *TB_InitStruct);
void TB_DeInit(TB_TypeDef *TBx);
void TB_StructInit(TB_InitTypeDef *TB_InitStruct);
void TB_Cmd(TB_TypeDef *TBx, FunctionalState Newstate );
FlagStatus TB_FlagStatus(TB_TypeDef *TBx);
u16 TB_GetCounter(TB_TypeDef *TBx);
void TB_FlagClear(TB_TypeDef *TBx);
void TB_ITConfig(TB_TypeDef *TBx, FunctionalState Newstate);

#endif /* __73x_TB_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/


