/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : The program presents an example of using Timer Base with End 
                       Of Count Interrupt 
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
/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Define Init Structure for each peripheral*/
  GPIO_InitTypeDef  GPIO_InitStructure;
  TB_InitTypeDef    TB_InitStructure;
  PRCCU_InitTypeDef PRCCU_InitStructure;
  CMU_InitTypeDef   CMU_InitStructure;

/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int main()
{

 #ifdef DEBUG
    debug();
#endif

/* CFG Initialization */
  
 /* GPIO4 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4 , ENABLE);

 /* TB2 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_TB2 , ENABLE);

 /* EIC Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_EIC , ENABLE);


  CMU_StructInit(&CMU_InitStructure);
  CMU_InitStructure.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;  
  CMU_Init(&CMU_InitStructure);
  
  /* set MCLK clock to 32 MHz */
  PRCCU_InitStructure.PRCCU_DIV2 = ENABLE ;   
  PRCCU_InitStructure.PRCCU_MCLKSRC_SRC = PRCCU_MCLKSRC_PLL ;
  PRCCU_InitStructure.PRCCU_PLLDIV = 2 ;
  PRCCU_InitStructure.PRCCU_PLLMUL = PRCCU_PLLMUL_16;
  PRCCU_Init(&PRCCU_InitStructure); 

/*-----------------------------------------------------------------------------
  Use Timer Base with End Of Count Interrupt 
------------------------------------------------------------------------------*/

/* EIC Configuration */
  EIC_IRQChannelConfig(TB2_IRQChannel, ENABLE );
  EIC_IRQChannelPriorityConfig(TB2_IRQChannel,1);

 /* GPIO4 Configuration in Output Mode */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_ALL;
  GPIO_Init (GPIO4, &GPIO_InitStructure);
  
 /*TB2 Configuration to toggle Leds each 250ms */ 

   TB_InitStructure.TB_CLK_Source = TB_CLK_INTERNAL  ;
   TB_InitStructure.TB_Prescaler = 0xC7;  /* The internal TB Frequency = 160KHz   */
   TB_InitStructure.TB_Preload = 0x9C3F ; /* 160000Hz * 0.25 ms  = 40000 = 0x9C40 */
  
  TB_Init (TB2, &TB_InitStructure);

  EIC_IRQCmd(ENABLE);

  TB_ITConfig (TB2,ENABLE);

  TB_Cmd(TB2, ENABLE);

  GPIO_WordWrite(GPIO4,0xFFFF);
 

  while (1)  
  {
  }
 }

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/


