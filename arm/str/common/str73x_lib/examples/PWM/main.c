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

/* includes-------------------------------------------------------------------*/
#include "73x_lib.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/  
/*-------------- Define Init Structure for each peripheral--------------------*/
  
  GPIO_InitTypeDef GPIO_InitStructure;
  PWM_InitTypeDef PWM_InitStructure1;

/* Private function prototypes -----------------------------------------------*/  
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/  
void main()
{
#ifdef DEBUG
    debug();
#endif

/* CFG Initialization */
  
 /* GPIO4 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4 , ENABLE);

 /* GPIO2 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO2 , ENABLE);

 /* PWM1 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_PWM1 , ENABLE);

/* GPIO4 Configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_ALL ;
  GPIO_Init (GPIO4, &GPIO_InitStructure);

//-----------------------------------------------------------------------------
//  Use PWM with Compare Period Interrupt 
//-----------------------------------------------------------------------------

/* EIC Configuration */
  EIC_IRQChannelConfig(PWM1_IRQChannel, ENABLE );
  EIC_IRQChannelPriorityConfig(PWM1_IRQChannel,3);

/* GPIO2 Configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_3;
  GPIO_Init (GPIO2, &GPIO_InitStructure);

/* PWM structure Initialization */
  PWM_DeInit (PWM1);
  PWM_StructInit(&PWM_InitStructure1);

/* PWM1 Configuration */
  PWM_InitStructure1.PWM_Prescaler0 = 0x2 ;
  PWM_InitStructure1.PWM_Prescaler1 = 0x1;
  PWM_InitStructure1.PWM_DutyCycle = 0x3FFF;
  PWM_InitStructure1.PWM_Period = 0xFFFF;
  PWM_InitStructure1.PWM_Polarity_Level = PWM_POLARITY_Low;

  PWM_Init (PWM1, &PWM_InitStructure1);
   
  GPIO_WordWrite(GPIO4, 0xFFFF);

  EIC_IRQCmd( ENABLE);

  PWM_ITConfig(PWM1, ENABLE);

  PWM_Cmd(PWM1, ENABLE);
  

 while (1)
  {
  }
}
/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
