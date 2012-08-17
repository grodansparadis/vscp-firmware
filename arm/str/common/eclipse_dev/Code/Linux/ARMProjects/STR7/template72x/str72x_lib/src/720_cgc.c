/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_cgc.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the Clock Gating Clock Block
*                      functions source code
**********************************************************************************
* History:
* 12/20/2004 :  V1.1
* 08/02/2004 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH  CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

/* Standard include ----------------------------------------------------------*/
#include "720_cgc.h"
/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : CGC_ClkConfig
* Description    : This routine is used to enbales or disables the clock signal
*                  for the specified AHB/SAPB or dedicated peripheral.
* Input          : the peripheral to enable the clock signal for it.
*                : the status of the peripheral clock
* Return         : None
*******************************************************************************/
void CGC_ClkConfig( CGC_Peripherals Periph, FunctionalState NewState )
{
  if (NewState==ENABLE)
  {
  CGC->PCG1 |= (u16)Periph;
  CGC->PCG2 |= (u16)(Periph >> 16);
  }
  else
  {
  CGC->PCG1 &= ~(u16)Periph;
  CGC->PCG2 &= ~((u16)(Periph >> 16));
  }
}

/*******************************************************************************
* Function Name  : CGC_ResetConfig
* Description    : This routine is used to activate or deactivate the reset
*                  line for the specified AHB/SAPB or the dedicated peripheral.
* Input          : the peripheral to activate or deactivate the reset line.
*                  The reset line Status
* Return         : None
*******************************************************************************/
void CGC_ResetConfig( CGC_Peripherals Periph, FunctionalState NewState )
{
  if (NewState==DISABLE)
  {
  CGC->PUR1 |= (u16)Periph;
  CGC->PUR2 |= (u16)(Periph >> 16);
  }
  else
  {
  CGC->PUR1 &= ~(u16)Periph;
  CGC->PUR2 &= ~((u16)(Periph >> 16));
  }
}
/*******************************************************************************
* Function Name  : CGC_ClkEmuConfig
* Description    : This routine is used to configure the status of the clock
*                  for the specified AHB/SAPB or the dedicated peripheral while
*                  the application program execution is stopped due to a debug
*                  request.
* Input          : the peripheral to enable or disable the clock during the
*                  debug mode.
* Return         : None
*******************************************************************************/
void CGC_ClkEmuConfig( CGC_Peripherals Periph, FunctionalState NewState )
{
  if (NewState==ENABLE)
  {
  CGC->EMU1 |= (u16)Periph;
  CGC->EMU2 |= (u16)(Periph >> 16);
  }
  else
  {
  CGC->EMU1 &= ~(u16)Periph;
  CGC->EMU2 &= ~((u16)(Periph >> 16));
  }
}

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
