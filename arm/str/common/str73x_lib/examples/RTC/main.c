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
 
/*-------------------Define Peripherals Init Structure--------------------------*/
  GPIO_InitTypeDef  GPIO_InitStructure;
  CMU_InitTypeDef   CMU_InitStructure;
  PRCCU_InitTypeDef   PRCCU_InitStructure;
  RTC_InitTypeDef   RTC_InitStructure;

/* Private function prototypes -------------------------------------------------*/  
/* Interface functions ---------------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/ 

void main()
{

#ifdef DEBUG
    debug();
#endif

/*--------------------------------------GPIO4--------------------------------------------*/
  /* GPIO4 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4 , ENABLE);
  /* GPIO4 Config */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_ALL ;
  GPIO_Init (GPIO4, &GPIO_InitStructure);

/*---------------------------------------CMU---------------------------------------------*/
  /* CMU Initialization */
  CMU_DeInit( );
  CMU_StructInit(&CMU_InitStructure);
  /* Enable the external oscillator */
  CMU_InitStructure.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;
  CMU_Init( &CMU_InitStructure);

/*-------------------------------------PRCCU---------------------------------------------*/
  /* PRCCU Initialization */
  PRCCU_DeInit();
  PRCCU_StructInit(&PRCCU_InitStructure);  
  /* PRCCU Config */
  PRCCU_InitStructure.PRCCU_DIV2 = DISABLE;
  PRCCU_Init(&PRCCU_InitStructure);

  /* Set RTC clock------- */
  /* (clock:0-->9) fOSC=8Mhz/RTCP=(2,4,8,16,32,64,128,256,512,1024)
                           -->EXTCLK=(4M,2M,1M,500K,250K,125K,62.5K,31.25K,15.625K,7.812K) */
  PRCCU_SetExtClkDiv(8);

/*---------------------------------------EIC---------------------------------------------*/
  /* EIC Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_EIC , ENABLE);
  /* EIC Config */
  EIC_IRQChannelPriorityConfig( RTC_IRQChannel, 1);
  EIC_IRQChannelConfig( RTC_IRQChannel , ENABLE );
  EIC_IRQCmd(ENABLE);

/*---------------------------------------RTC---------------------------------------------*/
  /* RTC Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_RTC , ENABLE);

  /* RTC Interrupt Config----------------------------- */
  RTC_ITConfig (RTC_IT_SEC | RTC_IT_ALA | RTC_IT_OV, ENABLE);

  /* RTC Configuration------------------------------- */
  RTC_InitStructure.RTC_Alarm = 0xFFFFFFFF;
  RTC_InitStructure.RTC_Counter = 0xFFFFFFFA;
  RTC_InitStructure.RTC_Prescaler = 0x3D09; /* configure RTC =1S  (RCK=1000000/64=CKL32=15625Hz) */
  RTC_Init (&RTC_InitStructure);

  while(1);
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/









