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

/* Private variables ---------------------------------------------------------*/
/* Define Init Structure for each peripheral*/
  GPIO_InitTypeDef  GPIO_InitStructure;
  WDG_InitTypeDef   WDG_InitStructure;

/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void main()
{

#ifdef DEBUG
    debug();
#endif
  
  /* GPIO4 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4 , ENABLE);

  /* EIC Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_EIC , ENABLE);


//-----------------------------------------------------------------------------
// Use WDG with End Of Count Interrupt 
//-----------------------------------------------------------------------------

/* EIC Configuration */
  EIC_IRQChannelConfig(WDGWUT_IRQChannel, ENABLE );
  EIC_IRQChannelPriorityConfig(WDGWUT_IRQChannel,1);

  /* GPIO4 Configuration in Output Mode */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_ALL;
  GPIO_Init (GPIO4, &GPIO_InitStructure);

  /* WDG Structure Initialization*/
  WDG_StructInit( & WDG_InitStructure);


  /*Configure end of counter = 1s with internal RC (MCLK ~ 1.17MHz)*/
  WDG_InitStructure.WDG_Preload = 0x1200 ;

  WDG_Init(& WDG_InitStructure);
  EIC_IRQCmd( ENABLE);

  WDG_ITConfig (ENABLE);

  WDG_Cmd( ENABLE);

  GPIO_WordWrite(GPIO4,0xFFFF);

  while (1) ; 

 }

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/



