/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_eft.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the EFT software functions
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

#include "720_eft.h"

/*******************************************************************************
* Function Name  : EFT_Init
* Description    : This routine is used to Initialize the EFT peripheral
* Input          : EFT Timer to Initialize
* Return         : None
*******************************************************************************/
void EFT_Init(EFT_TypeDef *EFTx)
{
  EFTx->CR1  = 0x0000;
  EFTx->CR2  = 0x0000;
  EFTx->SR   = 0x0000;
}
/*******************************************************************************
* Function Name  : EFT_ClockSourceConfig
* Description    : This routine is used to configure the EFT clock source
* Input          : (1) EFT Timer
*                : (2) EFT_Clocks : Specifies the EFT source clock
*                :    - INTERNAL : The EFT is clocked by the asynchronous APB
*		 :	           frequency divided by the prescaler value.
*                :    - EXTERNAL : The EFT is clocked by an external Clock
* Return         : None
*******************************************************************************/
void EFT_ClockSourceConfig(EFT_TypeDef *EFTx, EFT_Clocks Xclock)
{
  EFTx->CR1 = (Xclock==EXTERNAL)? (EFTx->CR1 | ECKEN_Mask) :\
                                  (EFTx->CR1 & ~ECKEN_Mask);
}
/*******************************************************************************
* Function Name  : EFT_ClockSourceValue
* Description    : This routine is used to get the EFT clock source
* Input          : EFT Timer
* Return         : EFT_Clocks : Specifies the EFT source clock
*                :    - INTERNAL : The EFT is clocked by the asynchronous APB
*                :                 frequency divided by the prescaler value.
*                :    - EXTERNAL : The EFT is clocked by an external Clock
*******************************************************************************/
EFT_Clocks EFT_ClockSourceValue(EFT_TypeDef *EFTx)
{
  EFT_Clocks TmpResult ;
  TmpResult = ((EFTx->CR1 & ECKEN_Mask) == 0)? INTERNAL : EXTERNAL;
  return (TmpResult);
}
/*******************************************************************************
* Function Name  : EFT_PrescalerConfig
* Description    : This routine is used to configure the EFT prescaler value
*                : (using an internal clock)
* Input          : (1) EFT Timer
*                : (2) Prescaler (vu8)
* Return         : None
*******************************************************************************/
void EFT_PrescalerConfig(EFT_TypeDef *EFTx, vu8 Xprescaler)
{
  EFTx->CR2 = ((EFTx->CR2 & 0xFF00) | Xprescaler);
}
/*******************************************************************************
* Function Name  : EFT_PrescalerValue
* Description    : This routine is used to get the EFT prescaler value
*                : (when using using an internal clock)
* Input          : EFT Timer
* Return         : Prescaler (u8)
*******************************************************************************/
u8 EFT_PrescalerValue(EFT_TypeDef *EFTx)
{
  return (EFTx->CR2 & 0x00FF);
}
/*******************************************************************************
* Function Name  : EFT_ClockLevelConfig
* Description    : This routine is used to configure the EFT clock level
*                :  (using an external clock)
* Input          : EFT Timer
*                : Clock_Edges : Specifies the active adge of the external clock
*                :  - RISING  : The rising  edge
*                :  - FALLING : The falling edge
* Return         : None
*******************************************************************************/
void EFT_ClockLevelConfig(EFT_TypeDef *EFTx, Clock_Edges Xedge)
{
  EFTx->CR1 = (Xedge == RISING) ? (EFTx->CR1 | EXEDG_Mask) :\
                                  (EFTx->CR1 & ~EXEDG_Mask);
}
/*******************************************************************************
* Function Name  : EFT_ClockLevelValue
* Description    : This routine is used to get the EFT clock level
* Input          : EFT Timer
* Output         : Clock_Edges : Specifies the active adge of the external clock
*                :  - RISING  : The rising  edge
*                :  - FALLING : The falling edge
*******************************************************************************/
Clock_Edges EFT_ClockLevelValue(EFT_TypeDef *EFTx)
{
  Clock_Edges TmpResult;
  TmpResult = ((EFTx->CR1 & EXEDG_Mask) == 0)? FALLING : RISING;
  return TmpResult ;
}
/*******************************************************************************
* Function Name  : EFT_ICAPModeConfig
* Description    : This routine is used to configure the input capture feature
* Input          : (1) EFT Timer
*                : (2) Input Capture Channel (Channel_A or Channel_B)
*                : (3) Active Edge : Rising edge or Falling edge.
* Output         : None
*******************************************************************************/
void EFT_ICAPModeConfig(EFT_TypeDef  *EFTx,
                        EFT_Channels Xchannel,
                        Clock_Edges  Xedge)
{
  vu16 Tmp ;
  Tmp = EFTx->CR1 ;
  switch (Xchannel)
  {
    case CHANNEL_A:
    {
      EFTx->CR1 = (Xedge == RISING)? (EFTx->CR1 | IEDGA_Mask) :\
                                     (EFTx->CR1 & ~IEDGA_Mask);
      break ;
    }
    case CHANNEL_B:
    {
      EFTx->CR1 = (Xedge == RISING)? (EFTx->CR1 | IEDGB_Mask) :\
                                     (EFTx->CR1 & ~IEDGB_Mask);
      break ;
    }
  }
}
/*******************************************************************************
* Function Name  : EFT_ICAPValue
* Description    : This routine is used to get the Input Capture value
* Input          : (1) EFT Timer
*                : (2) Input Capture Channel (Channel_A or Channel_B)
* Output         : None
*******************************************************************************/
u16 EFT_ICAPValue(EFT_TypeDef *EFTx, EFT_Channels Xchannel)
{
  u16 Tmp;
  Tmp = (Xchannel == CHANNEL_A)? EFTx->ICAR : EFTx->ICBR;
  return Tmp;
}
/*******************************************************************************
* Function Name  : EFT_OCMPModeConfig
* Description    : This routine is used to configure the out put compare feature
* Input          : (1) EFT Timer
*                : (2) OCMP Channel (Channel_A or Channel_B)
*                : (3) Pulse Length
*                : (4) OC_Mode : output wave, or only timing .
*                : (5) Level   : Rising edge or Falling edge after the ==
* Output         : None
*******************************************************************************/
void EFT_OCMPModeConfig(EFT_TypeDef  *EFTx,
                        EFT_Channels Xchannel,
                        vu16         XpulseLength,
                        OC_Modes     Xmode,
                        Logic_Levels Xlevel)
{
  vu16 Tmp1 = 0x0000;
  // Start The EFT Counter
  EFTx->CR1  = EN_Mask;

  switch (Xmode)
  {
    case TIMING:
    {
      // Output Compare Used only for Internal Timing Operation
      Tmp1 = (Xchannel == CHANNEL_A)? Tmp1 & ~OCAE_Mask : Tmp1 & ~OCBE_Mask;
      break;
    }
    case WAVE:
    {
      // Output Compare Used for external wave generation
      Tmp1 = (Xchannel == CHANNEL_A)? OCAE_Mask : OCBE_Mask;
      if (Xlevel == HIGH)
        Tmp1 = (Xchannel == CHANNEL_A)? (Tmp1 | OLVLA_Mask) : (Tmp1 | OLVLB_Mask);
      else
        Tmp1 = (Xchannel == CHANNEL_A)? (Tmp1 & ~OLVLA_Mask) : (Tmp1 & ~OLVLB_Mask);
      break;
    }
  }
  if (Xchannel == CHANNEL_A)
    //EFTx->OCAR = (XpulseLength - 5);
    EFTx->OCAR = XpulseLength;
  else
    //EFTx->OCBR = (XpulseLength - 5);
    EFTx->OCBR = XpulseLength;
    EFTx->CNTR = 0x0000;
    EFTx->CR1 |= Tmp1;
}
/*******************************************************************************
* Function Name  : EFT_CounterConfig
* Description    : This routine is used to configure the the EFT counter
* Input          : (1) EFT Timer
*                : (2) specifies the operation of the counter
* Output         : None
*******************************************************************************/
void EFT_CounterConfig(EFT_TypeDef *EFTx, EFT_CounterOperations Xoperation)
{
  switch (Xoperation)
  {
    case EFT_START:
    {
      EFTx->CR1 |= EN_Mask;
      break;
    }
    case EFT_STOP:
    {
      EFTx->CR1 &= ~EN_Mask;
      break;
    }
    case EFT_CLEAR:
    {
      EFTx->CNTR = 0x1234;
      break;
    }
  }
}
/*******************************************************************************
* Function Name  : EFT_ITConfig
* Description    : This routine is used to configure the EFT IT
* Input          : (1) EFT Timer
*                : (2) EFT interrupt
*                : (2) ENABLE / DISABLE
* Output         : None
*******************************************************************************/
void EFT_ITConfig(EFT_TypeDef *EFTx, vu16 New_IT, FunctionalState NewState)
{
  vu16 Tmp;
  Tmp = EFTx->CR2;
  Tmp = (NewState == ENABLE)? (Tmp | New_IT) : (Tmp & ~New_IT);
  EFTx->CR2 = Tmp;
}
/*******************************************************************************
* Function Name  : EFT_FlagStatus
* Description    : This routine is used to check whether a Flag is Set.
* Input          : (1) EFT Timer
*                : (2) The EFT FLag
* Output         : Flag Status
*******************************************************************************/
FlagStatus EFT_FlagStatus(EFT_TypeDef *EFTx, EFT_Flags Xflag)
{
  FlagStatus TmpResult ;
  TmpResult = ((EFTx->SR & Xflag) == 0)? RESET : SET;
  return TmpResult;
}
/*******************************************************************************
* Function Name  : EFT_FlagClear
* Description    : This routine is used to clear Flags.
* Input          : (1) EFT Timer
*                : (2) The EFT FLag
* Output         : None
*******************************************************************************/
void EFT_FlagClear(EFT_TypeDef *EFTx, EFT_Flags Xflag)
{
  EFTx->SR &= ~Xflag;
}

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/

