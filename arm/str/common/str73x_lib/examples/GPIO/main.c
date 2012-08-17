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

/* Define GPIO Init Structure */
GPIO_InitTypeDef  GPIO4_InitStructure;

/* Functions Prototypes-------------------------------------------------------*/
void Delay(u32 Xtime);

/* Variables------------------------------------------------------------------*/
u32 i=2;
bool Direction = FALSE;

/*---------------------------------main---------------------------------------*/

void main()
{

#ifdef DEBUG
    debug();
#endif

  /* GPIO4 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4 , ENABLE);            
  /* GPIO4 Configuration */ 
  GPIO4_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO4_InitStructure.GPIO_Pins = GPIO_PIN_ALL ;
  GPIO_Init (GPIO4, &GPIO4_InitStructure);

  /* GPIO4 Write */
  GPIO_BitWrite(GPIO4, GPIO_PIN_0, Bit_SET);
  Delay(0x2FFFF);
  GPIO_BitWrite(GPIO4, GPIO_PIN_0, Bit_RESET);
  Delay(0x2FFFF);
  GPIO_ByteWrite(GPIO4, GPIO_LSB, 0xFF);
  Delay(0x2FFFF);
  GPIO_ByteWrite(GPIO4, GPIO_MSB, 0xFF);
  Delay(0x2FFFF);
  GPIO_WordWrite(GPIO4, 0x0);
  Delay(0x2FFFF);
  GPIO_WordWrite(GPIO4, 0xFFFF);
  Delay(0x2FFFF);

  /* Loop write on each GPIO4 pin */
  while (1)
  {
    if (! Direction)
    {
      GPIO_BitWrite(GPIO4, i, Bit_SET);
      Delay(0x9FFF);
      i=i*4;
      if(i==0x20000)
      {
        i=0x4000;
        Direction = TRUE;
      }
    }
    else
    {
      GPIO_BitWrite(GPIO4, i, Bit_SET);
      Delay(0x9FFF);
      i=i/4;
      if(i==0)
      {
        i=2;
        Direction = FALSE;
      }
    }
  GPIO_WordWrite(GPIO4, GPIO_PIN_NONE);
  }
}

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name : void Delay(u32 Xtime)
Description   : Add a dealy
Input param   : u32 Xtime
Output param  : None
*******************************************************************************/
void Delay(u32 Xtime)
{
  u32 j;

  for(j=Xtime;j!=0;j--);
}


/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
