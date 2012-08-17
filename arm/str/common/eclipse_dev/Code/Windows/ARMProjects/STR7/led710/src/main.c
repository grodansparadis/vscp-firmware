/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 18/06/2003
* Description        : GPIO library example
********************************************************************************
* History:
*  18/06/2003 : creation
*******************************************************************************/

#include "71x_lib.h"

void Delay(u32 Xtime);

int main(void)
{
#ifdef DEBUG
    libdebug();
#endif  
u16 var;
GPIO_Config( GPIO1, 0xFFFF, GPIO_OUT_PP );
GPIO_BitWrite(GPIO1, 15, 0x00);
GPIO_BitWrite(GPIO1, 6, 0x00);
GPIO_BitWrite(GPIO1, 5, 0x00);
GPIO_BitWrite(GPIO1, 4, 0x00);
  // infinite loop
  while (1)
  {
	GPIO_BitWrite(GPIO1, 6, 0x01);
	Delay(0x2FFFF);
	GPIO_BitWrite(GPIO1, 5, 0x01);
	Delay(0x2FFFF);
	GPIO_BitWrite(GPIO1, 4, 0x01);
	Delay(0x2FFFF);
	GPIO_BitWrite(GPIO1, 4, 0x00);
	Delay(0x2FFFF);
	GPIO_BitWrite(GPIO1, 5, 0x00);
	Delay(0x2FFFF);
	GPIO_BitWrite(GPIO1, 6, 0x00);
	Delay(0x2FFFF);
  }
}

void Delay(u32 Xtime)
{
  u32 j;

  for(j=Xtime;j!=0;j--);
}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
