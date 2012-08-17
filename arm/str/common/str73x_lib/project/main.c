/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
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
#ifdef __GNUC__
#include "UART0_stdio.h"
#else
#include <stdio.h>
#endif
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/
  GPIO_InitTypeDef 	GPIO_InitStructure;
/* Private function prototypes -------------------------------------------------*/
/* Interface functions ---------------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/

int main()
{
  #ifdef DEBUG
    debug();
  #endif

  /* GPIO4 Configuration as Output Push-Pull */

  CFG_PeripheralClockConfig(CFG_CLK_GPIO4 , ENABLE); /* Enable  GPIO4 peripheral clock */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;   /* GPIO structure configuration   */
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_ALL;       /* All GPIO4 port is used         */
  GPIO_Init (GPIO4, &GPIO_InitStructure);            /* GPIO4 initialization           */

while (1)

 {
   printf ("Hello World \n");
   GPIO_WordWrite(GPIO4, ~(GPIO_WordRead(GPIO4)));
 }

}


/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
