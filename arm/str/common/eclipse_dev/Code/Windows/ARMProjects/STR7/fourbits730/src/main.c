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
#include "73x_lcd.h"
/* Include of other module interface headers -----------------------------------*/
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/
u16 Conversion_Value = 0;
u8 buffer[11] = "ADC: ";

/* Peripherals Init Structures */
GPIO_InitTypeDef GPIO0_InitStructure;
GPIO_InitTypeDef GPIO3_InitStructure;
ADC_InitTypeDef  ADC_InitStructure;
CMU_InitTypeDef  CMU_InitStructure;

/* Private function prototypes -------------------------------------------------*/
unsigned int millier(unsigned int val){
 return((val/1000) + 0x30);
}
unsigned int centaine(unsigned int val){
  unsigned int valCopy;
 valCopy = val % 1000;
 return((valCopy/100) + 0x30);
}
unsigned int dixaine(unsigned int val){
 unsigned int valCopy;
 valCopy = val % 100;
 return((valCopy/10) + 0x30);
}		
unsigned int unite(unsigned int val){
 unsigned int valCopy;
 valCopy = val % 100;
 return((valCopy%10) + 0x30);
}
/* Interface functions ---------------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/


int main(void)
{
u32 CPIO_Value;
#ifdef LIBDEBUG
    libdebug();
#endif



/*-------------------------------------GPIO0 --------------------------------*/
  /* GPIO0 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO0, ENABLE);
  /* GPIO0 Configuration */
  GPIO0_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO0_InitStructure.GPIO_Pins = GPIO_PIN_ALL ;
  GPIO_Init (GPIO0, &GPIO0_InitStructure);

/*-----------------------------------GPIO3 ----------------------------------*/
  /* GPIO3 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO3, ENABLE);
  /* GPIO3 Configuration */
  GPIO3_InitStructure.GPIO_Mode = GPIO_Mode_HI_AIN_TRI;
  GPIO3_InitStructure.GPIO_Pins = GPIO_PIN_ALL ;
  GPIO_Init (GPIO3, &GPIO3_InitStructure);

/*-----------------------------------EIC-------------------------------------*/
  /* EIC Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_EIC, ENABLE);
  /* EIC Config */
  EIC_IRQChannelPriorityConfig( ADC_IRQChannel, 1);
  EIC_IRQChannelConfig( ADC_IRQChannel , ENABLE );
  EIC_IRQCmd(ENABLE);

/*---------------------------------CMU---------------------------------------*/
  /* CMU Initialization */
  CMU_DeInit( );
  CMU_StructInit(&CMU_InitStructure);
  /* Enable the external oscillator */
  CMU_InitStructure.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;
  CMU_Init(&CMU_InitStructure);
  /* Disable DIV2 */
  PRCCU->CFR &= 0x7FFF;

/*---------------------------------ADC---------------------------------------*/
  /* ADC Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_ADC, ENABLE);
  /* ADC Configuration */
  ADC_DeInit ();
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Calibration = ADC_Calibration_ON;
  ADC_InitStructure.ADC_CalibAverage = ADC_CalibAverage_Enable;
  ADC_InitStructure.ADC_AutoClockOff = ADC_AutoClockOff_Disable ;
  ADC_InitStructure.ADC_ConversionMode = ADC_ConversionMode_Scan ;
  ADC_InitStructure.ADC_SamplingPrescaler = 0x2;
  ADC_InitStructure.ADC_ConversionPrescaler = 0x4;
  ADC_InitStructure.ADC_FirstChannel = ADC_CHANNEL0 ;
  ADC_InitStructure.ADC_ChannelNumber = 1;
  ADC_Init (&ADC_InitStructure);


  /* EOC interrupt Enable */
  ADC_ITConfig(ADC_IT_EOC, ENABLE);
  /* Enable ADC */
  ADC_Cmd(ENABLE);
  /*---------------------------------LCD---------------------------------------*/

  /* LCD Configuration */
  LCD_Init();
  LCD_LineClear(1);
  LCD_LineClear(2);
  LCD_SetPosCur(1,1);
  /* Display "STR730 IAR BOARD" message on LCD: Line 1 */
  LCD_SendStringByStep("STR730 IAR Board", 1, 1);
  LCD_Wait (40000);
  LCD_SetPosCur(2,1);
  /* Display "ADC:" message on LCD: Line 2 */
  LCD_SendStringByStep(buffer,2, 1);

 while(1)
 {
  /* Start ADC conversion */
   ADC_ConversionCmd (ADC_ConversionStart);
   buffer[8] = unite(Conversion_Value);
   buffer[7] = dixaine(Conversion_Value);
   buffer[6] = centaine(Conversion_Value);
   buffer[5] = millier(Conversion_Value);
   buffer[9] = 0;
   /* Display ADC conversion Value on LCD */
   LCD_SendStringByStep(&buffer[5],2, 5);
   CPIO_Value = 0;

   for(u32 i = (u32)((float)Conversion_Value/60); i; i--)
   {
    CPIO_Value <<= 1;
    CPIO_Value |= 1;
   }
   GPIO_WordWrite(GPIO0,CPIO_Value);

  }
 }


/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

























































































