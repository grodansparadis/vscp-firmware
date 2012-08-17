/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : 73x_tb.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the TB software functions.
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

/* Standard include ----------------------------------------------------------*/
#include "73x_tb.h"
#include "73x_prccu.h"
#include "73x_cfg.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Define to prevent recursive inclusion -------------------------------------*/
 /******************************************************************************
* Function Name  : TB_DeInit
* Description    : Deinitializes TB peripheral registers to their default reset
*                  values.
* Input          : TBx: where x can be 0, 1 or 2 to select the TB peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void TB_DeInit(TB_TypeDef *TBx)
{
  if( TBx == TB0)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TB0,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TB0,ENABLE);
  }
  else if  ( TBx == TB1)
  {
  
   CFG_PeripheralClockConfig(CFG_CLK_TB1,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TB1,ENABLE);
  }
  else if  ( TBx == TB2)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TB2,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TB2,ENABLE);
  }

}

/******************************************************************************
* Function Name  : TB_StructInit
* Description    : Fills in a TB_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : TB_InitStruct : pointer to a TB_InitTypeDef structure
*                  which will be initialized.
* Output         : None                        
* Return         : None.
******************************************************************************/
void TB_StructInit(TB_InitTypeDef *TB_InitStruct)
{
  TB_InitStruct->TB_CLK_Source = 0x0000;
  TB_InitStruct->TB_Prescaler = 0x00FF;
  TB_InitStruct->TB_Preload = 0xFFFF;
}

/******************************************************************************
* Function Name  : TB_Init
* Description    : Initializes TB  peripheral according to the specified
*                  parameters in the TB_InitTypeDef structure.
* Input          : - TBx: where x can be 0, 1 or 2 to select the TB peripheral.
*                  - TB_InitStruct: pointer to a TB_InitTypeDef structure that
*                  contains the configuration information for the specified TB
*                  peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void TB_Init( TB_TypeDef* TBx, TB_InitTypeDef* TB_InitStruct)
{
  TBx->CR &= TB_CR_Mask;
  TBx->CR |= TB_InitStruct->TB_CLK_Source;
  TBx->PR = TB_InitStruct->TB_Prescaler;
  TBx->VR = TB_InitStruct->TB_Preload;
}

/*******************************************************************************
* Function Name  : TB_Cmd
* Description    : Enables or disables TBx peripheral.
* Input          : - TBx: where x can be 0, 1 or 2 to select the TB peripheral.
*                  - Newstate: new state of the TBx peripheral (Newstate can be
                   ENABLE or DISABLE)
* Output         : None
* Return         : None
*******************************************************************************/
void TB_Cmd(TB_TypeDef *TBx, FunctionalState Newstate )
{
  if(Newstate == ENABLE) 
  { 
    TBx->CR |= TB_COUNTER_Start;
  }
  else
  {
    TBx->CR &= TB_COUNTER_Stop;
  }
}

/*******************************************************************************
* Function Name  : TB_FlagStatus
* Description    : Checks whether the TB End of Count (EC) flag is set or not.
* Input          : TBx: where x can be 0, 1 or 2 to select the TB peripheral.
* Output         : None
* Return         : The new state of the EC flag (SET or RESET).
*******************************************************************************/
FlagStatus TB_FlagStatus(TB_TypeDef *TBx)
{
  if((TBx->SR & TB_ECIT_Enable) != RESET )
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/*******************************************************************************
* Function Name  : TB_GetCounter
* Description    : Returns TB counter value.
* Input          : None
* Output         : None
* Return         : The TB counter register value
*******************************************************************************/
u16 TB_GetCounter(TB_TypeDef *TBx)
{
  return TBx->CNT;
}

/*******************************************************************************
* Function Name  : TB_FlagClear
* Description    : Clears the TB Flag. 
* Input          : TBx: where x can be 0, 1 or 2 to select the TB peripheral.                 
* Output         : None
* Return         : None
*******************************************************************************/
void TB_FlagClear(TB_TypeDef *TBx)
{
 /* reset the TB End of Count (EC) flag */
  TBx->SR &= TB_ECIT_Disable;
}


/*******************************************************************************
* Function Name  : TB_ITConfig
* Description    : Enables or disables the TB End of Count interrupt.
* Input          : - TBx: where x can be 0, 1 or 2 to select the TB peripheral.
*                  - Newstate:  new state of TB End of Count interrupt.
*                  This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void TB_ITConfig(TB_TypeDef *TBx, FunctionalState Newstate)
{
  if(Newstate == ENABLE)
  {
    TBx->MR |= TB_ECIT_Enable;
  }
  else
  {
    TBx->MR &= TB_ECIT_Disable;
  }
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
