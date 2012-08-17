/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : CMU Main program Example
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
#include <stdio.h>
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/ 


CMU_InitTypeDef   CMU_InitStruct;
GPIO_InitTypeDef  GPIO4_InitStructure;


int main(void)
{
  u16 Delay = 0x0;
  u8  Repeat = 0x0;
  
#ifdef DEBUG
    debug();
#endif

  CFG_PeripheralClockConfig(CFG_CLK_GPIO4 , ENABLE);
  /* GPIO4 Configuration */
  GPIO4_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO4_InitStructure.GPIO_Pins = GPIO_PIN_ALL ;
  GPIO_Init (GPIO4, &GPIO4_InitStructure);
  GPIO_WordWrite(GPIO4,0xFFFF);
  printf("\r CMU Example : Switch between external and internal oscillator\n");
  CMU_StructInit(&CMU_InitStruct);
  CMU_DeInit( );

 /*Infinite loop*/
  while(1)
  {
    /*Configure CMU to select the external Quartz as Main Clock (MCLK) */
    CMU_InitStruct.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;
    CMU_Init (&CMU_InitStruct);

    printf("\r Now the external Oscillator is selected \n");
    for ( Repeat =0x10 ;Repeat !=0   ; Repeat --)
    {
      for(Delay = 0; Delay < 0xFFFF; Delay++);
      GPIO_WordWrite(GPIO4,~GPIO_WordRead(GPIO4));
    }

    /*Configure CMU to select the internal RC oscillator as Main Clock (MCLK) */
    CMU_InitStruct.CMU_CKSEL0 = CMU_CKSEL0_CKRC;
    CMU_Init (&CMU_InitStruct);

    printf("\r Now the internal Oscillator is selected \n");
    for ( Repeat =0x10 ;Repeat !=0   ; Repeat --)

    {
      for(Delay = 0; Delay < 0xFFFF; Delay++);
      GPIO_WordWrite(GPIO4,~GPIO_WordRead(GPIO4));
       }
 
   }
}
/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

