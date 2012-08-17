/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Main program body
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
TIM_InitTypeDef TIM_InitStructure;
EIC_IRQInitTypeDef EIC_IRQInitStructure;
ErrorStatus OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void MRCC_Configuration(void);
void GPIO_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main()
{
  #ifdef DEBUG
    debug();
  #endif

  /* Configure the system clocks */
  MRCC_Configuration();

  /* Configure GPIOs */
  GPIO_Configuration();

/* EIC configuration ---------------------------------------------------------*/
  /* Enable and configure the priority of the TIM0 Update IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = TIM0_UP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable and configure the priority of the TIM1 Update IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = TIM1_UP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 2;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable and configure the priority of the TIM2 Update IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = TIM2_UP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 3;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable the Interrupt controller to manage IRQ channel*/
  EIC_IRQCmd(ENABLE);

/* TIM0, TIM1 and TIM2 Configuration in Output Compare Timing Mode -----------*/
  /* CK_TIM = 60 MHz, Prescaler = 5999,
     TIM0_CLK = TIM1_CLK = TIM2_CLK = CK_TIM/(Prescaler + 1) = 10 KHz */

  TIM_InitStructure.TIM_Mode = TIM_Mode_OCTiming;
  TIM_InitStructure.TIM_Prescaler = 5999;
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_InitStructure.TIM_Channel = TIM_Channel_1;
  TIM_InitStructure.TIM_Period = 9999; /* TIM0 Update event = TIM0_CLK/ (TIM_Period+1) = 1 Hz */
  TIM_Init(TIM0, &TIM_InitStructure);

  /* TIM1 configuration */
  TIM_InitStructure.TIM_Period = 19999; /* TIM1 Update event = TIM1_CLK/ (TIM_Period+1) = 0.5 Hz */
  TIM_Init(TIM1, &TIM_InitStructure);

  /* TIM2 configuration */
  TIM_InitStructure.TIM_Period = 29999; /* TIM2 Update event = TIM2_CLK/ (TIM_Period+1) = 0.33 Hz */
  TIM_Init(TIM2, &TIM_InitStructure);

  /* Clear TIM0 flags */
  TIM_ClearFlag(TIM0, TIM_FLAG_OC1| TIM_FLAG_OC2| TIM_FLAG_Update);

  /* Clear TIM1 flags */
  TIM_ClearFlag(TIM1, TIM_FLAG_OC1| TIM_FLAG_OC2| TIM_FLAG_Update);

  /* Clear TIM2 flags */
  TIM_ClearFlag(TIM2, TIM_FLAG_OC1| TIM_FLAG_OC2| TIM_FLAG_Update);

  /* Enable TIM0 Update interrupt */
  TIM_ITConfig(TIM0, TIM_IT_Update, ENABLE);

  /* Enable TIM1 Update interrupt */
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

  /* Enable TIM2 Update interrupt */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  /* Enable TIM0 counter */
  TIM_Cmd(TIM0, ENABLE);

  /* Enable TIM1 counter */
  TIM_Cmd(TIM1, ENABLE);

  /* Enable TIM2 counter */
  TIM_Cmd(TIM2, ENABLE);


  while(1);
}

/*******************************************************************************
* Function Name  : MRCC_Configuration
* Description    : Configures the different system clocks
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
 

  /* Enable TIM0, TIM1, TIM2 and GPIO clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_TIM0 | MRCC_Peripheral_TIM1 |
                             MRCC_Peripheral_TIM2 | MRCC_Peripheral_GPIO, ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the used GPIO pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure P1.0..P1.2 as output push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2;
  GPIO_Init(GPIO1, &GPIO_InitStructure);

}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
