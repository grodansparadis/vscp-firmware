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
#include <stdio.h>
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/ 
/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/ 

/* Define Device Init Structure */
  GPIO_InitTypeDef     GPIO_InitStructure;
  CMU_InitTypeDef      CMU_InitStructure;
  PRCCU_InitTypeDef    PRCCU_InitStructure;

/* Private function prototypes -------------------------------------------------*/ 
void Delay(u32 Xtime);
void LedLoop(void);
 
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
  /* GPIO4 Configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_ALL ;
  GPIO_Init (GPIO4, &GPIO_InitStructure);

/*--------------------------------------GPIO5--------------------------------------------*/
  /* GPIO5 Clock Enable */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO5 , ENABLE);
  /* GPIO5 Configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_8 | GPIO_PIN_9 ;
  GPIO_Init (GPIO5, &GPIO_InitStructure);
 
/*---------------------------------------EIC---------------------------------------------*/
  /* EIC Clcok Enable */
  CFG_PeripheralClockConfig(CFG_CLK_EIC , ENABLE);
  /* EIC Config */
  EIC_ExternalITTriggerConfig(EXTERNAL_IT6, EXTIT_TRIGGER_Rising);
  EIC_IRQChannelPriorityConfig(EXTIT06_IRQChannel, 1);
  EIC_IRQChannelConfig(EXTIT06_IRQChannel, ENABLE);
  EIC_ExternalITTriggerConfig(EXTERNAL_IT7, EXTIT_TRIGGER_Rising);
  EIC_IRQChannelPriorityConfig(EXTIT07_IRQChannel, 2);
  EIC_IRQChannelConfig(EXTIT07_IRQChannel, ENABLE);
  EIC_IRQCmd(ENABLE);

/*---------------------------------------CMU---------------------------------------------*/
  /* CMU Initialization */
  CMU_DeInit( );
  CMU_StructInit(&CMU_InitStructure);
  /* Enable the internal RC oscillator */
  CMU_InitStructure.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;
  CMU_Init( &CMU_InitStructure);

/*-------------------------------------PRCCU---------------------------------------------*/
  /* PRCCU Initialization */
  PRCCU_DeInit(); 
  /* PRCCU Config */
  PRCCU_InitStructure.PRCCU_DIV2 = ENABLE;
  PRCCU_InitStructure.PRCCU_MCLKSRC_SRC = PRCCU_MCLKSRC_PLL ; 
  PRCCU_InitStructure.PRCCU_PLLDIV = PRCCU_PLLDIV_4 ;
  PRCCU_InitStructure.PRCCU_PLLMUL = PRCCU_PLLMUL_12;
  PRCCU_InitStructure.PRCCU_FREEN = DISABLE; 
  PRCCU_Init(&PRCCU_InitStructure);

  printf ("\nMCLK=12MHz before entering in LPWFI mode\n");
  printf ("\n-->To enter in LPWFI mode press on  the NEXT push-button mounted on STR73x-Eval board");
  printf ("\n-->To exit LPWFI mode press on PREV push-button mounted on STR73x-Eval board\n");

  /* Turn on leds successively */
  LedLoop();
}

/* Private functions -------------------------------------------------------- */

/*******************************************************************************
Function name : void Delay(u32 Xtime)
Description   : Add a delay
Input param   : u32 Xtime
Output param  : None
*******************************************************************************/
void Delay(u32 Xtime)
{
  u32 j;

  for(j=Xtime;j!=0;j--);
}

/*******************************************************************************
Function name : void LedLoop()
Description   : Turn led successively
Input param   : None
Output param  : None
*******************************************************************************/
void LedLoop(void)
{
 u32 i=2;
 bool Direction = FALSE;

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

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
