/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_tim.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the TIM software functions.
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
#include "73x_tim.h"
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
/*******************************************************************************
* Function Name  : TIM_DeInit
* Description    : Initializes TIM peripheral control and registers to their
*                : default reset values.
* Input          : TIMx: where x can be from 0 to 9 to select the TIM peripheral.
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_DeInit(TIM_TypeDef *TIMx)
{ 
  
  if( TIMx == TIM0)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TIM0,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TIM0,ENABLE);
  }
  else if  ( TIMx == TIM1)
  {
  
   CFG_PeripheralClockConfig(CFG_CLK_TIM1,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TIM1,ENABLE);
  }
  else if  ( TIMx == TIM2)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TIM2,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TIM2,ENABLE);
  }
  else if  ( TIMx == TIM3)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TIM3,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TIM3,ENABLE);
  }
  else if  ( TIMx == TIM4)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TIM4,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TIM4,ENABLE);
  }
  else if  ( TIMx == TIM5)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TIM5,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TIM5,ENABLE);
  }
  else if  ( TIMx == TIM6)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TIM6,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TIM6,ENABLE);
  }
  else if  ( TIMx == TIM7)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TIM7,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TIM7,ENABLE);
  }
  else if  ( TIMx == TIM8)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TIM8,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TIM8,ENABLE);
  }

  else if  ( TIMx == TIM9)
  {
   CFG_PeripheralClockConfig(CFG_CLK_TIM9,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_TIM9,ENABLE);
  }


}

/******************************************************************************
* Function Name  : TIM_StructInit
* Description    : Fills in a TIM_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : TIM_InitStruct : pointer to a TIM_InitTypeDef structure
                   which will be initialized.
* Output         : None                        
* Return         : None.
******************************************************************************/
void TIM_StructInit(TIM_InitTypeDef *TIM_InitStruct)
{
  TIM_InitStruct->TIM_Mode           = 0x0000;
  TIM_InitStruct->TIM_OCA_Modes      = 0x0000;
  TIM_InitStruct->TIM_OCB_Modes      = 0x0000;
  TIM_InitStruct->TIM_Clock_Source   = 0x0000;
  TIM_InitStruct->TIM_Clock_Edge     = 0x0000;
  TIM_InitStruct->TIM_INPUT_Edge     = 0x0000;
  TIM_InitStruct->TIM_ICAPA_Modes    = 0x0000;
  TIM_InitStruct->TIM_ICAPB_Modes    = 0x0000;
  TIM_InitStruct->TIM_Prescaler      = 0x0000;
  TIM_InitStruct->TIM_Pulse_Level_A  = 0x0000;
  TIM_InitStruct->TIM_Pulse_Level_B  = 0x0000;
  TIM_InitStruct->TIM_Period_Level   = 0x0000;
  TIM_InitStruct->TIM_Pulse_Length_A = 0x0000;
  TIM_InitStruct->TIM_Pulse_Length_B = 0x0000;
  TIM_InitStruct->TIM_Full_Period    = 0x0000;
}

/******************************************************************************
* Function Name  : TIM_Init
* Description    : Initializes TIM  peripheral according to the specified
*                  parameters in the TIM_InitTypeDef structure.
* Input          : - TIMx: where x can be 0, 1 or 9 to select the TIM peripheral.
*                  - TIM_InitStruct: pointer to a TIM_InitTypeDef structure that
                   contains the configuration information for the specified TIM
                   peripheral.
* Output         : None
* Return         : None
******************************************************************************/

void TIM_Init(TIM_TypeDef *TIMx, TIM_InitTypeDef  *TIM_InitStruct)
{
   if (TIM_InitStruct->TIM_Clock_Source == TIM_CLK_INTERNAL)
  {
   TIMx->CR1 &= TIM_ECK_DISABLE_Mask;
  }
  else
  {
   if (TIM_InitStruct->TIM_Clock_Source & 0x8000)
   {
   TIMx->CR1 |= TIM_ECK_ENABLE_Mask;
   CFG->TIMSR = (CFG->TIMSR & 0x0000)|TIM_ICAP_NONE;
   } 
   else
   {
   TIMx->CR1 |= TIM_ECK_ENABLE_Mask;
   CFG->TIMSR = (CFG->TIMSR & 0x0000)|TIM_InitStruct->TIM_Clock_Source;
   }
     if (TIM_InitStruct->TIM_Clock_Edge == TIM_Rising)
    {
      TIMx->CR1 |= TIM_EXEDG_ENABLE_Mask;
    }
    else
    {
      TIMx->CR1 &= TIM_EXEDG_DISABLE_Mask;
    }
  }
    
  TIMx->CR2 =( TIMx->CR2 & 0xFF00 )|TIM_InitStruct->TIM_Prescaler ;

 switch ( TIM_InitStruct->TIM_Mode)
 {
/*********************************PWM Input mode *************************/
   case TIM_PWMI:
   {  
    /* Set the PWMI Bit */
    TIMx->CR1 |= TIM_PWMI_Mask;

    /* Set the first edge Level */
    if ( TIM_InitStruct->TIM_ICAPA_Modes == TIM_Rising)
    {
      TIMx->CR1 |= TIM_IEDGA_ENABLE_Mask;
    }
    else
    {
      TIMx->CR1 &= TIM_IEDGA_DISABLE_Mask;
    }
	    
    /* Set the Second edge Level ( Opposit of the first level )  */
    if ( TIM_InitStruct->TIM_ICAPA_Modes == TIM_Falling)
    {        
      TIMx->CR1 |= TIM_IEDGB_ENABLE_Mask;
    }
    else
    {
      TIMx->CR1 &= TIM_IEDGB_DISABLE_Mask;
    }
   }
   break;

 /*********************************Output compare channel A **************/
   case TIM_OCM_CHANNELA:
   {       
    if (TIM_InitStruct->TIM_Pulse_Level_A == TIM_High)
    {
      TIMx->CR1 |= TIM_OLVLA_Set_Mask;
    }
    else
    {
      TIMx->CR1 &= TIM_OLVLA_Reset_Mask;
    }

    TIMx->OCAR = TIM_InitStruct->TIM_Pulse_Length_A;
    
    if (TIM_InitStruct->TIM_OCA_Modes == TIM_Timing)
    {
      TIMx->CR1 &= TIM_OCA_DISABLE_Mask;
    }
    else
    {
      TIMx->CR1 |= TIM_OCA_ENABLE_Mask;
    }

   }
   break;

/*********************************Output compare channel B***************/
   case TIM_OCM_CHANNELB:
   {       
    if (TIM_InitStruct->TIM_Pulse_Level_B == TIM_High)
    {
      TIMx->CR1 |= TIM_OLVLB_Set_Mask;
    }
    else
    {
      TIMx->CR1 &= TIM_OLVLB_Reset_Mask;
    }

    TIMx->OCBR = TIM_InitStruct->TIM_Pulse_Length_B;

    if (TIM_InitStruct->TIM_OCB_Modes == TIM_Timing)
    {
      TIMx->CR1 &= TIM_OCB_DISABLE_Mask;
    }
    else
    {
      TIMx->CR1 |= TIM_OCB_ENABLE_Mask;
    }
   }
   break;

/*********************************Output compare channel A et B************/
   case TIM_OCM_CHANNELAB:
   {
    TIMx->OCBR = TIM_InitStruct->TIM_Pulse_Length_B;
    TIMx->OCAR = TIM_InitStruct->TIM_Pulse_Length_A;

    if (TIM_InitStruct->TIM_OCB_Modes == TIM_Timing)
    {
      TIMx->CR1 &= TIM_OCB_DISABLE_Mask;
    }
    else
    {
      TIMx->CR1 |= TIM_OCB_ENABLE_Mask;
    }
    if (TIM_InitStruct->TIM_OCA_Modes == TIM_Timing)
    {
      TIMx->CR1 &= TIM_OCA_DISABLE_Mask;
    }
    else
    {
      TIMx->CR1 |= TIM_OCA_ENABLE_Mask;
    }
    if (TIM_InitStruct->TIM_Pulse_Level_A== TIM_High)
    {
      TIMx->CR1 |= TIM_OLVLA_Set_Mask;
    }
    else
    {
      TIMx->CR1 &= TIM_OLVLA_Reset_Mask;
    }
    if (TIM_InitStruct->TIM_Pulse_Level_B== TIM_High)
    {
      TIMx->CR1 |= TIM_OLVLB_Set_Mask;
    }
    else
    {
      TIMx->CR1 &= TIM_OLVLB_Reset_Mask;
    }
   }
   break;

 /*********************************PWM mode*************************/
   case TIM_PWM:
   {
    /* Set the Level During the pulse */
    if ( TIM_InitStruct->TIM_Pulse_Level_A == TIM_High)
    {
      TIMx->CR1|= TIM_OLVLB_Set_Mask;
    }
    else
    {    
      TIMx->CR1 &= TIM_OLVLB_Reset_Mask;
    }

    /* Set the Level after After the pulse */
    if (TIM_InitStruct->TIM_Period_Level == TIM_High)
    {
      TIMx->CR1 |= TIM_OLVLA_Set_Mask;
    }
    else
    {
      TIMx->CR1 &= TIM_OLVLA_Reset_Mask;
    }
    /* Set the OCAE */
    TIMx->CR1 |= TIM_OCA_ENABLE_Mask;

    /* Set the PWM Bit */
    TIMx->CR1 |= TIM_PWM_Mask;
         
    /* Set the Duty Cycle value */
    if ( TIM_InitStruct->TIM_Pulse_Length_A < 5 ) 
    { 
      TIM_InitStruct->TIM_Pulse_Length_A = 4;
    }
    TIMx->OCAR = TIM_InitStruct->TIM_Pulse_Length_A - 4;
             
    /* Set the Full Period */
    TIMx->OCBR = TIM_InitStruct->TIM_Full_Period - 4;
   }
   break;

 /*************************************One pulse mode******************/
   case TIM_OPM:
   {
    /* Set the Level During the pulse */
    if (TIM_InitStruct->TIM_Pulse_Level_A == TIM_High)
     TIMx->CR1 |= TIM_OLVLB_Set_Mask;
         
    /* Set the Level after the pulse  */
    if (TIM_InitStruct->TIM_Period_Level == TIM_High)
     TIMx->CR1 |= TIM_OLVLA_Set_Mask;
         
    /* Set the Activation Edge on the INCAP 1 */
    if (TIM_InitStruct->TIM_INPUT_Edge == TIM_Rising)
     TIMx->CR1 |= TIM_IEDGA_ENABLE_Mask;
         
    /* Set the Output Compare Function  */
    TIMx->CR1 |= TIM_OCA_ENABLE_Mask;
         
    /* Set the One pulse mode */
    TIMx->CR1 |= TIM_OMP_Mask;
         
    /* Set the Pulse length  */
    TIMx->OCAR = TIM_InitStruct->TIM_Pulse_Length_A;
   }
   break;

 /*********************************Input capture channel A*************/
   case TIM_ICAP_CHANNELA:
   {
    if (TIM_InitStruct->TIM_ICAPA_Modes == TIM_Rising)
    {
      TIMx->CR1 |= TIM_IEDGA_ENABLE_Mask;
    }
    else 
    {
      TIMx->CR1 &= TIM_IEDGA_DISABLE_Mask;
    }
   }
   break;

/*********************************Input capture channel B**************/
   case TIM_ICAP_CHANNELB:
   {
    if (TIM_InitStruct->TIM_ICAPB_Modes == TIM_Rising)
    {
      TIMx->CR1 |= TIM_IEDGB_ENABLE_Mask;
    }
    else 
    {
      TIMx->CR1 &= TIM_IEDGB_DISABLE_Mask;
    }
   }
   break;

/*********************************Input capture channel A & B *********/
   case TIM_ICAP_CHANNELAB:
   {
   if (TIM_InitStruct->TIM_ICAPB_Modes == TIM_Rising)
   {
     TIMx->CR1 |= TIM_IEDGB_ENABLE_Mask;
   }
   else
   {
     TIMx->CR1 &= TIM_IEDGB_DISABLE_Mask;
   }
   if (TIM_InitStruct->TIM_ICAPA_Modes == TIM_Rising)
   {
     TIMx->CR1 |= TIM_IEDGA_ENABLE_Mask;
   }
   else
   { 
     TIMx->CR1 &= TIM_IEDGA_DISABLE_Mask;
   }
  }
  break;
  }
}

/*******************************************************************************
* Function Name  : TIM_CounterCmd
* Description    : Enables or disables TIMx Counter peripheral.
* Input          : - TIMx: where x can be 0, 1 or 9 to select the TIM peripheral.
*                  - TIM_operation: new state of the  TIMx Counter peripheral 
*                    (TIM_operation can be TIM_START, TIM_STOP or TIM_CLEAR)
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_CounterCmd(TIM_TypeDef  *TIMx, TIM_CounterOperations TIM_operation)
{
  switch ( TIM_operation )
  {
    case TIM_START :
     TIMx->CR1 |= TIM_ENABLE_Mask;
     break;
      
    case TIM_STOP :
      TIMx->CR1 &= TIM_DISABLE_Mask;
      break;
      
    case TIM_CLEAR :
      TIMx->CNTR = 0x1234;
      break;
  }
}

/*******************************************************************************
* Function Name  : TIM_ClockSourceConfig
* Description    : This routine is used to configure the TIM clock source
* Input          : - TIMx: where x can be 0, 1..9 to select the TIM peripheral
*                : - TIM_Clock : Specifies the TIM source clock
*                    - TIM_CLK_INTERNAL : The TIM is clocked by the APB frequency
*                                         divided by the prescaler value.
*                    - TIM_CLK_EXTERNAL : The TIM is clocked by an external Clock
                     - TIM_ICAP_x : where x can be 0,1..9 to select the ICAPx 
                       input pin that recieves the external clock.
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_ClockSourceConfig ( TIM_TypeDef *TIMx, u16 TIM_Clock )
{
if (TIM_Clock == TIM_CLK_INTERNAL)
  {
   TIMx->CR1 &= TIM_ECK_DISABLE_Mask;
  }
  else
  {
   if (TIM_Clock & 0x8000)
   {
   TIMx->CR1 |= TIM_ECK_ENABLE_Mask;
   CFG->TIMSR = (CFG->TIMSR & 0x0000)|TIM_ICAP_NONE;
   } 
   else
   {
   TIMx->CR1 |= TIM_ECK_ENABLE_Mask;
   CFG->TIMSR = (CFG->TIMSR & 0x0000)|TIM_Clock;
   }
/**/
}
}

/*******************************************************************************
* Function Name  : TIM_GetClockStatus
* Description    : This routine is used to get the TIM clock source status
* Input          : TIMx: where x can be 0, 1 or 9 to select the TIM peripheral.
* Output         : None
* Return         : The TIM source clock
*                    - TIM_CLK_INTERNAL : The TIM is clocked by the APB frequency
*                                         divided by the prescaler value.
*                    - TIM_CLK_EXTERNAL : The TIM is clocked by an external Clock
*******************************************************************************/
u16 TIM_GetClockStatus(TIM_TypeDef *TIMx)
{
  if  ((TIMx->CR1 & TIM_ECK_ENABLE_Mask) == 0)
  {
    return TIM_CLK_INTERNAL;
  }
  else
  {
    return TIM_CLK_EXTERNAL;
  }	
}

/*******************************************************************************
* Function Name  : TIM_PrescalerConfig
* Description    : This routine is used to configure the TIM prescaler value
*                  (when using an internal clock).
* Input          : - TIMx: where x can be 0, 1 or 9 to select the TIM peripheral
*                : - TIM_Prescaler: Prescaler division factor
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_PrescalerConfig(TIM_TypeDef *TIMx, u8 TIM_Prescaler)
{
  TIMx->CR2 = TIMx->CR2 & 0xFF00;
  TIMx->CR2 |= TIM_Prescaler;
}

/*******************************************************************************
* Function Name  : TIM_GetPrescalerValue
* Description    : This routine is used to get the TIM prescaler value
*                  ( when using an internal clock )
* Input          : TIMx: where x can be 0, 1 or 9 to select the TIM peripheral.
* Output         : None
* Return         : Prescaler Register
*******************************************************************************/
u8 TIM_GetPrescalerValue(TIM_TypeDef *TIMx)
{
  return TIMx->CR2 & 0x00FF;
}

/*******************************************************************************
* Function Name  : TIM_GetICAPAValue
* Description    : This routine is used to get the Input Capture A value.
* Input          : TIMx: where x can be 0, 1 or 9 to select the TIM peripheral
* Output         : None
* Return         : Input Capture A Register
*******************************************************************************/
u16 TIM_GetICAPAValue(TIM_TypeDef *TIMx)
{
  return TIMx->ICAR;
}

/*******************************************************************************
* Function Name  : TIM_GetICAPBValue
* Description    : This routine is used to get the Input Capture B value.
* Input          : TIMx: where x can be 0, 1 or 9 to select the TIM peripheral
* Output         : None
* Return         : Input Capture B Register 
*******************************************************************************/
u16 TIM_GetICAPBValue ( TIM_TypeDef *TIMx)
{
  return TIMx->ICBR;
}

/*******************************************************************************
* Function Name  : TIM_FlagStatus
* Description    : Checks whether the specified TIM flag is set or not.
* Input          : - TIMx: where x can be 0, 1 or 9 to select the TIM peripheral.
*                  - TIM_Flag: flag to check(TIM_Flag can be TIM_FLAG_ICA, TIM_FLAG_ICB
*                    TIM_FLAG_OCA, TIM_FLAG_OCB, TIM_FLAG_TO)
* Output         : None
* Return         : The NewState of the TIM_Flag (SET or RESET).
*******************************************************************************/
FlagStatus TIM_FlagStatus ( TIM_TypeDef *TIMx, u16 TIM_Flag )
{
  if((TIMx->SR & TIM_Flag) == RESET)
  {
    return RESET;
  }
  else
  {
    return SET;
  }
}

/*******************************************************************************
* Function Name  : TIM_FlagClear
* Description    : Clears the TIM Flag passed as a parameter
* Input          : - TIMx: where x can be 0, 1 or n to select the TIM peripheral.
*                  - TIM_Flag: flag to clear (TIM_Flag can be TIM_FLAG_ICA, TIM_FLAG_ICB
*                    TIM_FLAG_OCA, TIM_FLAG_OCB, TIM_FLAG_TO)
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_FlagClear ( TIM_TypeDef *TIMx, u16 TIM_Flag )
{
  /* Clear TIM_Flag */
  TIMx->SR &= ~TIM_Flag;
}

/*******************************************************************************
* Function Name  : TIM_GetPWMIPulse
* Description    : This routine is used to get the Pulse value in PWMI Mode.
* Input          : TIMx: where x can be 0, 1 or 9 to select the TIM peripheral
* Output         : None
* Return         : Pulse 
*******************************************************************************/
u16 TIM_GetPWMIPulse(TIM_TypeDef  *TIMx)
{
  return TIMx->ICBR;
}

/*******************************************************************************
* Function Name  : TIM_GetPWMIPeriod
* Description    : This routine is used to get the Period value in PWMI Mode.
* Input          : TIMx: where x can be 0, 1 or 9 to select the TIM peripheral
* Output         : None
* Return         : Period 
*******************************************************************************/
u16 TIM_GetPWMIPeriod(TIM_TypeDef  *TIMx)
{
  return TIMx->ICAR;
}

/*******************************************************************************
* Function Name  : TIM_ITConfig
* Description    : Configures the Timer interrupt source.
* Input          : - TIMx: where x can be 0, 1 or 9 to select the TIM peripheral.
*                  - TIM_IT: specifies the TIM interrupt source to be enabled.
*                    This parameter can be one of the following values:
*                         - TIM_IT_ICA: Input Capture A Interrupt source
*                         - TIM_IT_OCA: Output Compare A Interrupt source
*                         - TIM_IT_TO: Timer Overflow Interrupt source
*                         - TIM_IT_ICB: Input Capture B Interrupt source
*                         - TIM_IT_OCB: Output Compare B Interrupt source
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_ITConfig(TIM_TypeDef *TIMx, u16 TIM_IT, FunctionalState NewState)
{
  if(NewState == ENABLE) 
  {
    TIMx->CR2 = (TIMx->CR2 & 0x00FF) | TIM_IT; 
  }
  else
  {
    TIMx->CR2 &= ~TIM_IT;
  }
}

/*******************************************************************************
* Function Name  : TIM_DMAConfig
* Description    : Configures the Timer DMA source.
* Input          : - TIMx: where x can be 0, 1 or 9 to select the TIM peripheral.
*                  - TIM_DMA_Souces: specifies the TIM DMA source to be selected.
*                    This parameter can be one of the following values:
*                         - TIM_DMA_ICA: Input Capture A DMA source
*                         - TIM_DMA_OCA: Output Compare A DMA source
*                         - TIM_DMA_TO: Timer Overflow DMA source
*                         - TIM_DMA_ICB: Input Capture B DMA source
*                         - TIM_DMA_OCB: Output Compare B DMA source
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_DMAConfig(TIM_TypeDef *TIMx, u16 TIM_DMA_Sources)
{
  /* Reset the DMAS[1:0] bits */
  TIMx->CR1 &= 0xCFFF;
  /* Set the DMAS[1:0] bits according to TIM_DMA_Sources parameter */
  TIMx->CR1 |= TIM_DMA_Sources; 
}

/*******************************************************************************
* Function Name  : TIM_DMACmd
* Description    : Enables or disables TIMx DMA peripheral.
* Input          : - TIMx: where x can be 0, 1 or 9 to select the TIM peripheral.
*                  - Newstate: new state of the  TIMx DMA peripheral 
*                    (Newstate can be ENABLE or DISABLE) 
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_DMACmd(TIM_TypeDef *TIMx, FunctionalState Newstate )
{
  if (Newstate == ENABLE) 
  {
    TIMx->CR2 |= TIM_DMA_Enable;
  }
  else
  { 
    TIMx->CR2 &= TIM_DMA_Disable;
  }
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

