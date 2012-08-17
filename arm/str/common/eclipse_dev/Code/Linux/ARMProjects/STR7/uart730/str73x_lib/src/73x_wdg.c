/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_wdg.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the WDG software functions.
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
#include "73x_wdg.h"
#include "73x_prccu.h"

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
* Function Name  : WDG_DeInit
* Description    : Deinitializes the WDG peripheral registers to their default
*                  reset values.
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void WDG_DeInit(void)
{
  /* Deinitialize Control register */
  WDG->CR = 0x0000;
  
  /* Deinitialize Prescaler register */
  WDG->PR = 0x00FF;
  
  /* Deinitialize Preload register */
  WDG->VR = 0xFFFF;
  
  /* Deinitialize Status register */
  WDG->SR = 0x0;
  
  /* Deinitialize Mask register */
  WDG->MR = 0x0;
  
  /* Deinitialize key register */
  WDG->KR = 0x0000;
   
}

/******************************************************************************
* Function Name  : WDG_StructInit
* Description    : Fills in a WDG_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : WD_InitStruct : pointer to a WDG_InitTypeDef structure which
*                  will be initialized.
* Output         : None                        
* Return         : None
******************************************************************************/
void WDG_StructInit(WDG_InitTypeDef *WDG_InitStruct)
{
  /* Initialize mode */
  WDG_InitStruct->WDG_Mode = WDG_Mode_Timer;

  /* Initialize CLK source */
  WDG_InitStruct->WDG_CLK_Source = WDG_CLK_INTERNAL;
  
  /* Initialize Prescaler */
  WDG_InitStruct->WDG_Prescaler = 0x00FF;
  
  /* Initialize Preload */
  WDG_InitStruct->WDG_Preload = 0xFFFF ;
}

/******************************************************************************
* Function Name  : WDG_Init
* Description    : Initializes WDG  peripheral according to the specified
*                  parameters in the WDG_InitTypeDef structure.
* Input          : WDG_InitStruct: pointer to a WDG_InitTypeDef structure that
*                  contains the configuration information for the WDG peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void WDG_Init( WDG_InitTypeDef *WDG_InitStruct)
{
  /*Initialize register*/
  WDG->CR &= WDG_CR_MASK;

  /* Select WDG Mode */
  WDG->CR |= WDG_InitStruct->WDG_Mode;
   
  /* Select WDG Clock source */
  WDG->CR |= WDG_InitStruct->WDG_CLK_Source;

  /* Configure WDG Clock Prescaler */
  WDG->PR = WDG_InitStruct->WDG_Prescaler;

  /* Configure WDG Clock Counter */
  WDG->VR = WDG_InitStruct->WDG_Preload ;
}

/*******************************************************************************
* Function Name  : WDG_Cmd
* Description    : Enables or disables WDG peripheral.
* Input          : NewState: new state of the WDG peripheral. This parameter can
*                  be ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void WDG_Cmd(FunctionalState NewState)
{
  if((WDG->CR & 0x0001) == WDG_Mode_Timer)
  {
    /* Timer mode */
    if(NewState == ENABLE)
    {
      /* Start timer by setting SC bit in Control register */
      WDG->CR |= WDG_Counter_Start;
    }
    else
    {
      /* Stop timer by clearning SC bit in Control register */
      WDG->CR &= WDG_Counter_Stop;
    }
  }
  else
  { 
    /* Watchdog mode */
    if(NewState == ENABLE)
    {
      WDG->KR = WDG_KeyValue1;
      WDG->KR = WDG_KeyValue2;
    }
  }
}

/*******************************************************************************
* Function Name  : WDG_FlagClear
* Description    : Clears the end of count flag.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
 void WDG_FlagClear(void)
{
  WDG->SR &= 0xFFFE;
}

/*******************************************************************************
* Function Name  : WDG_ITConfig
* Description    : Enables or disables the end count interrupt.
* Input          : NewState: new state of the WDG peripheral. This parameter can
*                  be ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
 void WDG_ITConfig(FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    /*Enable the end count interrupt*/
    WDG->MR |= WDG_ECIT_Enable;
  }
  else
  {
    /*Disable the end count interrupt*/
    WDG->MR &= WDG_ECIT_Disable;
  }
}
/*******************************************************************************
* Function Name  : WDG_FlagStatus
* Description    : Checks whether the end of counter flag is set or not.
* Input          : None
* Output         : None
* Return         : The new state of the end of counter (SET or RESET).
*******************************************************************************/
FlagStatus WDG_FlagStatus (void)
{
  /* Checks whether end of count flag is set or not */
  if((WDG->SR & 0x1) != RESET)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/*******************************************************************************
* Function Name  : WDG_GetCounter
* Description    : Gets the current counter value.
* Input          : None
* Output         : None
* Return         : The value of CNT register: counter value
*******************************************************************************/
u16 WDG_GetCounter (void)
{
  return WDG->CNT;
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
