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
int main()
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
 
/* TIM0 and TIM1 configuration -----------------------------------------------*/ 
  /* CK_TIM = 30 MHz, Prescaler = 0x0, TIM0_CLK =  TIM1_CLK = CK_TIM/(Prescaler + 1) = 30 MHz */
  
  /* TIM0 generate a PWM signal with frequency = 1 KHz and a duty_cycle = 75% */
  TIM_InitStructure.TIM_Mode = TIM_Mode_PWM;              
  TIM_InitStructure.TIM_Prescaler = 0x0;                     
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal; 
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_InitStructure.TIM_Channel = TIM_Channel_2;       
  TIM_InitStructure.TIM_Period = 0x7530;
  TIM_InitStructure.TIM_Pulse2 = 0x57E4;                                                               
  TIM_InitStructure.TIM_Polarity2 = TIM_Polarity2_High;
  TIM_Init(TIM0, &TIM_InitStructure);

  /* Configure TIM1 in PWM Input Mode */ 
  TIM_InitStructure.TIM_Mode = TIM_Mode_PWMI;                                
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal;                   
  TIM_InitStructure.TIM_PWMI_ICSelection = TIM_PWMI_ICSelection_TI1;
  TIM_InitStructure.TIM_PWMI_ICPolarity = TIM_PWMI_ICPolarity_Falling;        
  TIM_Init(TIM1, &TIM_InitStructure);

  /* Clear TIM0 and TIM1 ISR registers */
  TIM_ClearFlag(TIM0, TIM_FLAG_OC1| TIM_FLAG_OC2| TIM_FLAG_Update|TIM_FLAG_IC1|TIM_FLAG_IC2);
  TIM_ClearFlag(TIM1, TIM_FLAG_OC1| TIM_FLAG_OC2| TIM_FLAG_Update|TIM_FLAG_IC1|TIM_FLAG_IC2);

  /* Enable TIM1 IC1 interrupt */
  TIM_ITConfig(TIM1, TIM_IT_IC1, ENABLE);

  /* Enable TIM0 and TIM1 Counter */
  TIM_Cmd(TIM0, ENABLE);
  TIM_Cmd(TIM1, ENABLE);

  while(1);
}
  
/*******************************************************************************
* Function Name  : MRCC_Configuration
* Description    : Configures the different system clocks.
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
    /* Set HCLK to 30 MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div2);

    /* Set CKTIM to 30 MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div1);

    /* Set PCLK to 30 MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div1);

    /* Set CKSYS to 60 MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3); 

  /* Enable GPIO, TIM0 and TIM1 clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_TIM0 |
                             MRCC_Peripheral_TIM1, ENABLE);
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

  /* TIM0_OC2 pin(P1.00) configuration  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;              
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIO1, &GPIO_InitStructure);

  /* TIM1_TI1 pin(P0.29) configuration  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;              
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_29;
  GPIO_Init(GPIO0, &GPIO_InitStructure);
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

  /* Enable and configure the priority of the TIM1 Input Capture 1&2 IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = TIM1_IC12_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable the Interrupt controller to manage IRQ channel*/
  EIC_IRQCmd(ENABLE);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


