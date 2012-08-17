/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_pwm.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      PWM software library.
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
#ifndef __73x_PWM_H
#define __73x_PWM_H

/* Includes ----------------------------------------------------------------- */
#include "73x_map.h"

/* Exported types ----------------------------------------------------------- */
/* PWM Init structure define */
typedef struct
{             
 u8  PWM_Prescaler0;		/* Prescaler0 value */  
 u8  PWM_Prescaler1;		/* Prescaler1 value */
 u16 PWM_DutyCycle;		/* Duty Cycle value */
 u16 PWM_Period;		/* Period value */
 u8  PWM_Polarity_Level;	/* Polarity level of the PWM signal */
} PWM_InitTypeDef;

/* Exported constants --------------------------------------------------------*/

/*PWM interrupts */
#define PWM_ITCPI_Enable   0x0001
#define PWM_ITCPI_Disable  0xFFFE

/*Polarity level*/
#define PWM_POLARITY_Low   0x1
#define PWM_POLARITY_High  0x0

/* PWM Enable/Disable */
#define PWM_Enable_Mask 0x0001
#define PWM_Disable_Mask  0xFFFE

#define PWM_POLARITY_Set 0x0001
#define PWM_POLARITY_Reset 0xFFFE

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void PWM_Init(PWM_TypeDef *PWMx, PWM_InitTypeDef  *PWM_InitStruct);
void PWM_DeInit(PWM_TypeDef *PWMx);
void PWM_StructInit(PWM_InitTypeDef *PWM_InitStruct);
void PWM_Cmd(PWM_TypeDef* PWMx, FunctionalState NewState);
void PWM_SetPeriod(PWM_TypeDef *PWMx, u16 PWM_Period);
u16 PWM_GetPeriodValue(PWM_TypeDef *PWMx);
void PWM_SetDutyCycle(PWM_TypeDef *PWMx, u16 PWM_Duty);
void PWM_PolarityConfig(PWM_TypeDef *PWMx, u16 PWM_Polarity_Mode);
u16 PWM_GetDutyCycleValue(PWM_TypeDef *PWMx);
void PWM_ITConfig(PWM_TypeDef *PWMx, FunctionalState Newstate);
FlagStatus PWM_FlagStatus (PWM_TypeDef *PWMx);
void PWM_FlagClear(PWM_TypeDef *PWMx);
 
#endif /*__73x_PWM_H*/

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

