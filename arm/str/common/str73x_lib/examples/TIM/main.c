/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Main program body
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
#include "73x_lib.h"
/* Include of other module interface headers -----------------------------------*/
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/ 

  GPIO_InitTypeDef 	GPIO_InitStructure;
  TIM_InitTypeDef 	TIM_InitStructure;
  EIC_InitTypeDef       EIC_InitStructure;

 

/* Private function prototypes -------------------------------------------------*/  
/* Interface functions ---------------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/ 

int main()
{

  #ifdef DEBUG
    debug();
  #endif

  

 /* CFG Initialization */
  
 /* GPIO4 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4 , ENABLE);

 /* GPIO1 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO1 , ENABLE);


 /* GPIO0 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO0 , ENABLE);

 /* TIM3 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_TIM3 , ENABLE);

 /* TIM4 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_TIM4 , ENABLE);

 /* EIC Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_EIC , ENABLE);

/*-----------------------------------------------------------------------------*/
/*  PWM Mode and OPM Mode Example                                              */
/*-----------------------------------------------------------------------------*/

 /* EIC Configuration */
  EIC_StructInit(&EIC_InitStructure);
  EIC_Init (&EIC_InitStructure);
  EIC_IRQChannelConfig(TIM3_IRQChannel, ENABLE );
  EIC_IRQChannelPriorityConfig(TIM3_IRQChannel,1);

 /* TIM3 OCMPA(P0.15) pin is configured in Alternate Function Mode */

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_15;
  GPIO_Init (GPIO0, &GPIO_InitStructure);

 /* TIM4 ICAPA(P1.3) pin is configured in Input Trigger TTL Mode and */
 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_3;
  GPIO_Init (GPIO1, &GPIO_InitStructure);
 
 /* TIM4 OCMPA(P1.0) is configured in AF_PP                          */

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_0;
  GPIO_Init (GPIO1, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_ALL;
  GPIO_Init (GPIO4, &GPIO_InitStructure);

  GPIO_WordWrite(GPIO4, 0xFFFF);

  /* TIM4 Configuration in ONE PULSE MODE*/
 
  TIM_InitStructure.TIM_Mode = TIM_OPM;
  TIM_InitStructure.TIM_Pulse_Level_A = TIM_High ;
  TIM_InitStructure.TIM_Clock_Source = TIM_CLK_INTERNAL;
  TIM_InitStructure.TIM_Period_Level = TIM_Low;
  TIM_InitStructure.TIM_Prescaler = 0x0;
  TIM_InitStructure.TIM_Pulse_Length_A = 0x1FF0;
  TIM_InitStructure.TIM_INPUT_Edge = TIM_Rising;

  TIM_Init (TIM4, &TIM_InitStructure);

  /* TIM3 Configuration in PULSE WIDTH MODULATION Mode*/
  
  TIM_InitStructure.TIM_Mode = TIM_PWM;
  TIM_InitStructure.TIM_Pulse_Level_A = TIM_High ;
  TIM_InitStructure.TIM_Clock_Source = TIM_CLK_INTERNAL;
  TIM_InitStructure.TIM_Period_Level = TIM_Low;
  TIM_InitStructure.TIM_Prescaler = 0x0;
  TIM_InitStructure.TIM_Pulse_Length_A = 0x3FFF;
  TIM_InitStructure.TIM_Full_Period = 0x7FFF;

  TIM_Init (TIM3, &TIM_InitStructure);
 
  TIM_ITConfig ( TIM3,TIM_IT_OCB ,ENABLE );

  TIM_CounterCmd(TIM4, TIM_START);

  TIM_CounterCmd(TIM3, TIM_START);

  EIC_IRQCmd( ENABLE);


while (1);

}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/


