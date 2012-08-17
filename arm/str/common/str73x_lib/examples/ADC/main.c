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
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/ 

/* Peripherals Init Structures */
GPIO_InitTypeDef GPIOx_InitStructure;
ADC_InitTypeDef  ADC_InitStructure;
CMU_InitTypeDef  CMU_InitStructure;
PRCCU_InitTypeDef  PRCCU_InitStructure;


/* Private function prototypes -------------------------------------------------*/  
/* Interface functions ---------------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/

int main(void)
{

#ifdef DEBUG
    debug();
#endif

/*-------------------------------------GPIO4 --------------------------------*/
  /* GPIO4 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4, ENABLE);
  /* GPIO4 Configuration */
  GPIOx_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIOx_InitStructure.GPIO_Pins = GPIO_PIN_ALL ;
  GPIO_Init (GPIO4, &GPIOx_InitStructure);

/*-----------------------------------GPIO3 ----------------------------------*/
  /* GPIO3 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO3, ENABLE);
  /* GPIO3 Configuration */
  GPIOx_InitStructure.GPIO_Mode = GPIO_Mode_HI_AIN_TRI;
  GPIOx_InitStructure.GPIO_Pins = GPIO_PIN_0 ;
  GPIO_Init (GPIO3, &GPIOx_InitStructure);

/*-------------------------------GPIO2 --------------------------------------*/
  /* GPIO2 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO2, ENABLE);
  /* GPIO2 Configuration */
  GPIOx_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIOx_InitStructure.GPIO_Pins = GPIO_PIN_ALL ;
  GPIO_Init (GPIO2, &GPIOx_InitStructure);

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
  /* Enable the external Quartz clock source */
  CMU_InitStructure.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;
  CMU_Init(&CMU_InitStructure);
  
/*-------------------------------------PRCCU---------------------------------*/
  /* PRCCU Initialization */
  PRCCU_DeInit();
  PRCCU_StructInit(&PRCCU_InitStructure);  
  /* Set MCLK = 8MHz */
  PRCCU_InitStructure.PRCCU_DIV2 = DISABLE; 
  PRCCU_Init(&PRCCU_InitStructure);
 

/*---------------------------------ADC---------------------------------------*/
  /* ADC Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_ADC, ENABLE);
  /* ADC Configuration */
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Calibration = ADC_Calibration_ON;
  ADC_InitStructure.ADC_CalibAverage = ADC_CalibAverage_Enable;
  ADC_InitStructure.ADC_AutoClockOff = ADC_AutoClockOff_Disable ;
  ADC_InitStructure.ADC_ConversionMode = ADC_ConversionMode_Scan ;
  ADC_InitStructure.ADC_SamplingPrescaler = 0x7;
  ADC_InitStructure.ADC_ConversionPrescaler = 0x0;
  ADC_InitStructure.ADC_FirstChannel = ADC_CHANNEL0 ;
  ADC_InitStructure.ADC_ChannelNumber = 1;
  ADC_Init (&ADC_InitStructure);

  /* ADC Analog Watchdog Configuration */
  ADC_AnalogWatchdogConfig(ADC_AnalogWatchdog0, ADC_CHANNEL0, 0x0100, 0x0250);
  /* Analog watchdog0 Enable */
  ADC_AnalogWatchdogCmd (ADC_AnalogWatchdog0, ENABLE); 
  /* EOC interrupt Enable */
  ADC_ITConfig(ADC_IT_EOC, ENABLE);
  /* Enable ADC */
  ADC_Cmd(ENABLE);
  /* Start ADC conversion */
  ADC_ConversionCmd (ADC_ConversionStart);
  /* Wait for ADC interrupt */
  while(1);
 

}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
