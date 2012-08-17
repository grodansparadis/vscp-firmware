/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_wut.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the WUT software functions.
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
#include "73x_wut.h"
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
/******************************************************************************
* Function Name  : WUT_DeInit
* Description    : Deinitializes the WUT peripheral registers to their default
*                  reset values.
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void WUT_DeInit(void)
{
  /* initialize the WUT registers to their reset values */
  CFG_PeripheralClockConfig(CFG_CLK_WUT,DISABLE);
  CFG_PeripheralClockConfig(CFG_CLK_WUT,ENABLE);
}

/******************************************************************************
* Function Name  : WUT_Init
* Description    : Initializes the WUT peripheral according to the specified
*                  parameters in the WUT_InitTypeDef structure.
* Input          : WUT_InitStruct: pointer to a WUT_InitTypeDef structure that
*                  contains the configuration information for the WUT peripheral.                  
* Output         : None
* Return         : None
******************************************************************************/
void WUT_Init(WUT_InitTypeDef* WUT_InitStruct)
{  
  /* select WUT mode */
  if(WUT_InitStruct->WUT_Mode == WUT_Mode_WakeUp)
  {
    /* WUT End of Count event is connected to WIU input Line 0 */
    CFG->R1 |= WUT_Mode_WakeUp;
  }
  else
  {
    /* WUT used in Timer mode */
    CFG->R1 &= WUT_Mode_Timer;
  }
  
 /* select clock source*/
  if(WUT_InitStruct->WUT_CLK_Source == WUT_CLK_EXTERNAL)
  {
    WUT->CR |= WUT_CLK_EXTERNAL;
  }
  else
  {
    WUT->CR &= WUT_CLK_INTERNAL;
  }

  /* configure the prescaler register */
  WUT->PR = WUT_InitStruct->WUT_Prescaler;
  
  /* configure the preload register */
  WUT->VR = WUT_InitStruct->WUT_Preload;
}

/*******************************************************************************
* Function Name  : WUT_Cmd
* Description    : Starts or stops the WUT counter.
* Input          : NewState: new state of the WUT peripheral counter. 
*                  This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void WUT_Cmd(FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    WUT->CR |= WUT_COUNTER_Start;
  }
  else
  {
    WUT->CR &= WUT_COUNTER_Stop;
  }
}

/******************************************************************************
* Function Name  : WUT_StructInit
* Description    : Fills in a WUT_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : WUT_InitStruct : pointer to a WUT_InitTypeDef structure which
*                  will be initialized.
* Output         : None
* Return         : None
******************************************************************************/
void WUT_StructInit(WUT_InitTypeDef* WUT_InitStruct)
{
  WUT_InitStruct->WUT_Mode = WUT_Mode_Timer;
  WUT_InitStruct->WUT_CLK_Source = WUT_CLK_INTERNAL;
  WUT_InitStruct->WUT_Prescaler  = 0x00FF;
  WUT_InitStruct->WUT_Preload    = 0xFFFF;
}

/*******************************************************************************
* Function Name  : WUT_ITConfig
* Description    : Enables or disables WUT End of Count interrupt.
* Input          : NewState: new state WUT End of Count interrupt
*                  This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void WUT_ITConfig(FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    /* set the ECM bit in the MR register */
    WUT->MR |= WUT_IT_ENABLE;
  }
  else
  {
    /* reset the ECM bit in the MR register */
    WUT->MR &= WUT_IT_DISABLE;
  }
}

/*******************************************************************************
* Function Name  : WUT_FlagStatus
* Description    : Checks whether the specified WUT flag is set or not.
* Input          : WUT_Flag: flag to check. This parameter can be one of the
*                  following values:
*                      - WUT_FLAG_BSYVR
*                      - WUT_FLAG_BSYPR
*                      - WUT_FLAG_BSYCR
*                      - WUT_FLAG_EC
* Output         : None
* Return         : The new state of the WUT_Flag (SET or RESET).
*******************************************************************************/
FlagStatus WUT_FlagStatus(u8 WUT_Flag)
{
  if ((WUT->SR & WUT_Flag) != RESET)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/*******************************************************************************
* Function Name  : WUT_FlagClear
* Description    : Clears WUT End of Count flag.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WUT_FlagClear(void)
{
  /* clear the EC bit */
  WUT->SR &= ~WUT_FLAG_EC;
}

/*******************************************************************************
* Function Name  : WUT_BusySignalConfig
* Description    : Enables or disables the read of the busy bits in the WUT_SR
*                  register.
* Input          : NewState: new state of the busy bits. This parameter can
*                  be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void WUT_BusySignalConfig(FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    /* set the BSYMSK bit in the MR register */
    WUT->MR |= WUT_BSYMSK_SET;
  }
  else
  {
    /* reset the BSYMSK bit in the MR register */
    WUT->MR &= WUT_BSYMSK_RESET;
  }
}

/*******************************************************************************
* Function Name  : WUT_GetCounterValue
* Description    : Returns WUT counter value.
* Input          : None
* Output         : None
* Return         : The WUT counter register value
*******************************************************************************/
u16 WUT_GetCounterValue(void)
{
  return (WUT->CNT);
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
