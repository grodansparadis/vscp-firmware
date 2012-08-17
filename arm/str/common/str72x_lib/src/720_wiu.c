/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_wiu.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the WIU software functions
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

#include "720_wiu.h"

/*******************************************************************************
* Function Name  : WIU_Init
* Description    : Initialize the WIU
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_Init(void)
{
  WIU->MR = 0x0000;
  WIU->TR = 0x0000;
  WIU->PR = 0xFFFF;
  WIU->CTRL = 0x0000;
  WIU->CTRL = 0x0000;
}
/*******************************************************************************
* Function Name  : WIU_EnterStopMode
* Description    : Enter the stop mode
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_EnterStopMode(void)
{
  vu16 Tmp;
  Tmp = WIU->CTRL & 0x03;
  // Write '1' to Stop Bit
  WIU->CTRL = 0x04 + Tmp;
  // Write '0' to Stop Bit
  WIU->CTRL = Tmp;
  // Write '1' to Stop Bit
  WIU->CTRL = 0x04 + Tmp;
  // add Delay
  asm volatile("nop;nop;nop;nop;nop;nop");
  /*__asm
    {
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
    }*/

}
/*******************************************************************************
* Function Name  : WIU_ModeConfig
* Description    : Configure the WIU Mede
* Input 1        : Mode can be WakeUp, Interrupt or WakeUpInterrupt
* Input 2        : NewState can be ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_ModeConfig(WIUMode_TypeDef Mode, FunctionalState NewState)
{
  if (NewState == ENABLE) WIU->CTRL|=Mode;
  else WIU->CTRL &= ~Mode;
}
/*******************************************************************************
* Function Name  : WIU_LineModeConfig
* Description    : Configure the trigger edge
* Input 1        : Lines to be configured
* Input 2        : The trigger edge can be FallingEdge or RisingEdge
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_LineModeConfig(vu16 Lines, WIUTriggerEdge_TypeDef TriggerEdge)
{
  if (TriggerEdge == WIU_FallingEdge) WIU->TR &= ~Lines;
  else WIU->TR |= Lines;
}
/*******************************************************************************
* Function Name  : WIU_LineConfig
* Description    : Enable and disable lines interrupts
* Input 1        : Lines to be configured
* Input 2        : NewState can be ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_LineConfig(u16 Lines, FunctionalState NewState)
{
  if(NewState) WIU->MR |= Lines;
  else WIU->MR &= ~Lines;
}
/*******************************************************************************
* Function Name  : WIU_InterruptLineValue
* Description    : Get the interrupt lines
* Input 1        : None
* Output         : The interrupts lines numbers
* Return         : None
*******************************************************************************/
u16 WIU_InterruptLineValue(void)
{
  return (WIU->PR);
}
/*******************************************************************************
* Function Name  : WIU_PendingBitClear
* Description    : Clear the pending bits
* Input 1        : Bits to be cleared
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_PendingBitClear(vu16 Lines)
{
  WIU->PR |= Lines;
}


/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/


