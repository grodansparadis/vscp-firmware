/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_ADC.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the ADC software functions
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
#include "720_adc.h"

/*******************************************************************************
* Function Name      : ADC_Init
* Description        : Initialize the ADC
* Input              : None.
* Return             : None.
*******************************************************************************/
void ADC_Init(void)
{
  //Initiate ADC registers to their reset values
  AGCR->CR1  = 0x04 ;
  ADC->CSR   = 0x00;
  ADC->CPR   = 0x06;
  
}

/*******************************************************************************
* Function Name      : ADC_PrescalerConfig
* Description        : Configure the prescaler
* Input              : None.
* Return             : None.
*******************************************************************************/
void ADC_PrescalerConfig(vu32 ADC_clk)
{
  //Update the Prescaler Register

   ADC->CPR = ((ADC->CSR & ADC_DEC) == 1)?
   RCCU_GetFrequency(RCCU_AAPBClock)/(ADC_clk*5120*8):
   RCCU_GetFrequency(RCCU_AAPBClock)/(ADC_clk*512*8);
}

/*******************************************************************************
* Function Name      : ADC_ITConfig
* Description        : enable or disable the interrupt
* Input              : NewState=ENABLE=>enable interrupt
*                      NewState=DISABLE=>disable interrupt
* Return             : None
*******************************************************************************/
void ADC_ITConfig(FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    // Test the conversion mode
    if (ADC->CSR & 0x0040)
    {
      // Set interrupt bit equivalent to the channel selected
      switch (ADC->CSR & 0x30)
      {
        case 0x00 : ADC->CSR |= ADC_IT0_Mask;  break;
        case 0x10 : ADC->CSR |= ADC_IT1_Mask;  break;
        case 0x20 : ADC->CSR |= ADC_IT2_Mask;  break;
        case 0x30 : ADC->CSR |= ADC_IT3_Mask;  break;
      }
    }
    else
      // Set all interrupt bits in case of round robin mode
      ADC->CSR |= ADC_IT_Mask;
  }
  else
    // Clear all interrupt bits
    ADC->CSR &= ~ADC_IT_Mask;
}
/*********************(c) 2004  STMicroelectronics********************* END OF FILE **/



