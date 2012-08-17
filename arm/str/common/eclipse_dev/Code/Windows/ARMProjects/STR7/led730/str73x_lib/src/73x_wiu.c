/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_wiu.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the WIU software functions.
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
#include "73x_wiu.h"
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
* Function Name  : WIU_Init
* Description    : Initializes the WIU unit according to the specified parameters
*                  in the WIU_InitTypeDef structure.
* Input          : WIU_InitStruct: pointer to a WIU_InitTypeDef structure that
*                  contains the configuration information for the WIU peripheral.                   
* Output         : None
* Return         : None
******************************************************************************/
void WIU_Init(WIU_InitTypeDef* WIU_InitStruct)
{
  /* select the Wake-up line to be used */
  WIU->MR |= WIU_InitStruct->WIU_Line;

  /* configure the triggering edge */
  if(WIU_InitStruct->WIU_TriggerEdge == WIU_RisingEdge)
  {
    /* trigger on rising edge */
    WIU->TR |= WIU_InitStruct->WIU_Line;
  }
  else
  {
    /* trigger on falling edge */
    WIU->TR &= ~WIU_InitStruct->WIU_Line;
  }

  if(WIU_InitStruct->WIU_Mode == WIU_Mode_SWInterrupt)
  {
    /* enable interrupt mode */
    WIU->CTRL |= WIU_Mode_Interrupt;
    
    /* set the corresponding WUINT bit*/
    WIU->INTR |= WIU_InitStruct->WIU_Line;
  }
  else
  {
    /* configure the WIU mode */
    WIU->CTRL |= WIU_InitStruct->WIU_Mode;
  }
}

/******************************************************************************
* Function Name  : WIU_DeInit
* Description    : Deinitializes the WIU registers to their default reset values.
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void WIU_DeInit(void)
{
  /* initialize the WIU registers to their reset value */
  CFG_PeripheralClockConfig(CFG_CLK_WIU, DISABLE);
  CFG_PeripheralClockConfig(CFG_CLK_WIU, ENABLE);
}

/******************************************************************************
* Function Name  : WIU_StructInit
* Description    : Fills in a WIU_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : WIU_InitStruct : pointer to a WIU_InitTypeDef structure
*                  which will be initialized.
* Output         : None
* Return         : None
******************************************************************************/
void WIU_StructInit(WIU_InitTypeDef* WIU_InitStruct)
{
  /* initialize the WIU_InitStruct fields to their reset values */
  WIU_InitStruct->WIU_Mode = 0x0 ;
  WIU_InitStruct->WIU_Line = 0x0 ;
  WIU_InitStruct->WIU_TriggerEdge = WIU_FallingEdge ;
}

/*******************************************************************************
* Function Name  : WIU_EnterStopMode
* Description    : Executes STOP mode entering sequence.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_EnterStopMode( void)
{
  u8 delay = 8;

  /* write '1' to STOP bit */
  WIU->CTRL |= 0x04;

  /* write '0' to STOP bit */
  WIU->CTRL &= 0xFFFFFFFB;

  /* write '1' to STOP bit */
  WIU->CTRL |= 0x04;

  /* add some No operations  */
  for (delay = 8; delay != 0 ; delay --);
}

/*******************************************************************************
* Function Name  : WIU_GetITLineValue
* Description    : Returns the value of the WIU line generating an interrupt.
* Input          : None
* Output         : None
* Return         : The line generating interrupt
*******************************************************************************/
u32 WIU_GetITLineValue(void)
{
  return (WIU->PR);
}

/*******************************************************************************
* Function Name  : WIU_PendingBitClear
* Description    : Clears the pending bit of the selected WIU line.
* Input          : WIU_Line : Wake-up line to clear its pending bit.
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_PendingBitClear(u32 WIU_Line)
{
  /* clear the pending bit of the selected WIU line */
  WIU->PR = WIU_Line;
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
