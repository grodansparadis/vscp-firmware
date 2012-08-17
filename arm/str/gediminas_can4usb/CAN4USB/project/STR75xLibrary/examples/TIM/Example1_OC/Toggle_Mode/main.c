/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Main program body.
********************************************************************************
* History:
* 07/17/2006 : V1.0
* 03/10/2006 : V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "75x_lib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_InitTypeDef     TIM_InitStructure;
u32 OC1Value = 15000;
u32 OC2Value = 6000;
ErrorStatus OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void MRCC_Configuration(void);
void GPIO_Configuration(void);
void EIC_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main()
{
  #ifdef DEBUG
    debug();
  #endif
  
  /* MRCC Configuration */
  MRCC_Configuration();
  
  /* GPIO Configuration */
  GPIO_Configuration();
    
  /* EIC Configuration */
  EIC_Configuration();
  
/* TIM Configuration in Output Compare Toggle Mode ---------------------------*/ 
  /* CK_TIM = 60 MHz, Prescaler = 0x0, TIM0_CLK = CK_TIM/(Prescaler + 1) = 60 MHz */
  /* OC1 update rate = TIM0_CLK / TIM_Pulse1 = 60 MHz/ 15000 = 4000 Hz (250 us) */
  /* OC2 update rate = TIM0_CLK / TIM_Pulse2 = 60 MHz/ 6000 = 10000 Hz (100 us) */

  /* TIM0 Channel1 configuration */
  TIM_InitStructure.TIM_Mode = TIM_Mode_OCToggle;              
  TIM_InitStructure.TIM_Prescaler = 0x0;
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;       
  TIM_InitStructure.TIM_Period = 0xFFFF;
  TIM_InitStructure.TIM_Channel = TIM_Channel_1;                
  TIM_InitStructure.TIM_Pulse1 = OC1Value;
  TIM_InitStructure.TIM_Polarity1 = TIM_Polarity1_Low;
  TIM_Init(TIM0, &TIM_InitStructure);

  /* TIM0 Channel2 configuration */
  TIM_InitStructure.TIM_Channel = TIM_Channel_2;                
  TIM_InitStructure.TIM_Pulse2 = OC2Value;
  TIM_Init(TIM0, &TIM_InitStructure);

  /* Preload register on TIM0_OCR1 and TIM0_OCR2 disabled: TIM0_OCR1 and TIM0_OCR2 
     values are taken in account immediately */
  TIM_PreloadConfig(TIM0, TIM_Channel_ALL, DISABLE);

  /* Clear TIM0 flags */
  TIM_ClearFlag(TIM0, TIM_FLAG_OC1 | TIM_FLAG_OC2| TIM_FLAG_Update);

  /* Enable TIM0 Output Compare1&2 interrupt */
  TIM_ITConfig(TIM0, TIM_IT_OC1 | TIM_IT_OC2, ENABLE);

  /* Enable TIM0 counter */
  TIM_Cmd(TIM0, ENABLE);

  while(1);
}
  
/*******************************************************************************
* Function Name  : MRCC_Configuration
* Description    :Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MRCC_Configuration(void)
{
  /* MRCC system reset(for debug purpose) */
  MRCC_DeInit();

  /* Wait for OSC4M start-up */
  OSC4MStartUpStatus = MRCC_WaitForOSC4MStartUp();

  if(OSC4MStartUpStatus == SUCCESS)
  {
    /* Set HCLK to 60 MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div1);

    /* Set CKTIM to 60 MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div1);

    /* Set PCLK to 30 MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div2);

    /* Enable Flash burst mode */
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);

    /* Set CKSYS to 60 MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);
 
  /* Enable TIM0 and GPIO clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_TIM0 | MRCC_Peripheral_GPIO, ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the used GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM0_OC1 pin (P0.0) configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* TIM0_OC2 pin (P1.00) configuration  */
  GPIO_Init(GPIO1, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : EIC_Configuration
* Description    : Configures the interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_Configuration(void)
{  
  EIC_IRQInitTypeDef  EIC_IRQInitStructure;

  /* Enable and configure the priority of the TIM0 Output Compare2 IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = TIM0_OC2_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable and configure the priority of the TIM0 Output Compare1 IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = TIM0_OC1_IRQChannel;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable the Interrupt controller to manage IRQ channel*/
  EIC_IRQCmd(ENABLE);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

