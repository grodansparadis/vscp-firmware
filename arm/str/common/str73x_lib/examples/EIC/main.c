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

/* Standard include ------------------------------------------------------------*/
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

 

 /* Enable Clocks*/
  
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4 , ENABLE); 
  CFG_PeripheralClockConfig(CFG_CLK_EIC   , ENABLE); 
  CFG_PeripheralClockConfig(CFG_CLK_TIM0  , ENABLE);
  CFG_PeripheralClockConfig(CFG_CLK_TIM1  , ENABLE);
  CFG_PeripheralClockConfig(CFG_CLK_TIM2  , ENABLE);
  CFG_PeripheralClockConfig(CFG_CLK_TIM3  , ENABLE); 
  CFG_PeripheralClockConfig(CFG_CLK_TIM4  , ENABLE);
  CFG_PeripheralClockConfig(CFG_CLK_TIM5  , ENABLE);
  CFG_PeripheralClockConfig(CFG_CLK_TIM6  , ENABLE);
  CFG_PeripheralClockConfig(CFG_CLK_TIM7  , ENABLE);
  

 /* EIC Configuration */
  
  EIC_DeInit();  /* This function may be used if the EIC
                    is not configured in the 73x_init file */
  EIC_StructInit(&EIC_InitStructure);
  EIC_Init (&EIC_InitStructure);
            
  EIC_FIQChannelConfig(TIM0_FIQChannel, ENABLE );
  EIC_IRQChannelConfig(TIM1_IRQChannel, ENABLE );
  EIC_IRQChannelConfig(TIM2_IRQChannel, ENABLE );
  EIC_IRQChannelConfig(TIM3_IRQChannel, ENABLE );
  EIC_IRQChannelConfig(TIM4_IRQChannel, ENABLE );
  EIC_IRQChannelConfig(TIM5_IRQChannel, ENABLE );
  EIC_IRQChannelConfig(TIM6_IRQChannel, ENABLE );
  EIC_IRQChannelConfig(TIM7_IRQChannel, ENABLE );
  
  EIC_IRQChannelPriorityConfig(TIM1_IRQChannel,7);
  EIC_IRQChannelPriorityConfig(TIM2_IRQChannel,6);
  EIC_IRQChannelPriorityConfig(TIM3_IRQChannel,5);
  EIC_IRQChannelPriorityConfig(TIM4_IRQChannel,4);
  EIC_IRQChannelPriorityConfig(TIM5_IRQChannel,3);
  EIC_IRQChannelPriorityConfig(TIM6_IRQChannel,2);
  EIC_IRQChannelPriorityConfig(TIM7_IRQChannel,1);
  
  EIC_IRQCmd(ENABLE);
  EIC_FIQCmd(ENABLE);



 /* GPIO4 Configuration */  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_ALL;
  GPIO_Init (GPIO4, &GPIO_InitStructure);

  /* Timers  Configuration */
  
  TIM_StructInit(&TIM_InitStructure);
  TIM_InitStructure.TIM_Clock_Source = TIM_CLK_INTERNAL;
  TIM_InitStructure.TIM_Prescaler = 0x0;
  TIM_Init (TIM0, &TIM_InitStructure);
  TIM_InitStructure.TIM_Prescaler = 0x1;
  TIM_Init (TIM1, &TIM_InitStructure);
  TIM_InitStructure.TIM_Prescaler = 0x2;
  TIM_Init (TIM2, &TIM_InitStructure);
  TIM_InitStructure.TIM_Prescaler = 0x3;
  TIM_Init (TIM3, &TIM_InitStructure);
  TIM_InitStructure.TIM_Prescaler = 0x4;
  TIM_Init (TIM4, &TIM_InitStructure);
  TIM_InitStructure.TIM_Prescaler = 0x5;
  TIM_Init (TIM5, &TIM_InitStructure);
  TIM_InitStructure.TIM_Prescaler = 0x6;
  TIM_Init (TIM6, &TIM_InitStructure);
  TIM_InitStructure.TIM_Prescaler = 0x7;
  TIM_Init (TIM7, &TIM_InitStructure);


  TIM_ITConfig (TIM0,TIM_IT_TO ,ENABLE );
  TIM_ITConfig (TIM1,TIM_IT_TO ,ENABLE );
  TIM_ITConfig (TIM2,TIM_IT_TO ,ENABLE );
  TIM_ITConfig (TIM3,TIM_IT_TO ,ENABLE );
  TIM_ITConfig (TIM4,TIM_IT_TO ,ENABLE );
  TIM_ITConfig (TIM5,TIM_IT_TO ,ENABLE );
  TIM_ITConfig (TIM6,TIM_IT_TO ,ENABLE );
  TIM_ITConfig (TIM7,TIM_IT_TO ,ENABLE );


  TIM_CounterCmd(TIM0, TIM_START);
  TIM_CounterCmd(TIM1, TIM_START);
  TIM_CounterCmd(TIM2, TIM_START);
  TIM_CounterCmd(TIM3, TIM_START);
  TIM_CounterCmd(TIM4, TIM_START);
  TIM_CounterCmd(TIM5, TIM_START);
  TIM_CounterCmd(TIM6, TIM_START);
  TIM_CounterCmd(TIM7, TIM_START);


  
while (1);

}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/


