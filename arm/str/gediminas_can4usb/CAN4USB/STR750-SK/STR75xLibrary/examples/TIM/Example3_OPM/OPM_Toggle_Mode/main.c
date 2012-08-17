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
void GPIO_Configuration(void);
void MRCC_Configuration(void);

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
      
  /* OPM configuration: External pulse received on TIM0_TI1 pin, OPM in toggle 
     mode is used, the pulse is delayed from the external signal edge with 2.73 ms */
  TIM_InitStructure.TIM_Mode = TIM_Mode_OPM_Toggle;           
  TIM_InitStructure.TIM_Prescaler = 0x3;                     
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal; 
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;       
  TIM_InitStructure.TIM_Period = 0xFFFF;                        
  TIM_InitStructure.TIM_Channel = TIM_Channel_1;                
  TIM_InitStructure.TIM_Pulse1 = 0x4FFF;                        
  TIM_InitStructure.TIM_Polarity1 = TIM_Polarity1_High; 
  TIM_InitStructure.TIM_IC1Selection = TIM_IC1Selection_TI1;
  TIM_InitStructure.TIM_IC1Polarity = TIM_IC1Polarity_Falling; 
  TIM_InitStructure.TIM_RepetitivePulse = TIM_RepetitivePulse_Disable;       
  
  TIM_Init(TIM0, &TIM_InitStructure);

  /* TIM1 configured to generate the external pulse for the TIM0 */
  TIM_InitStructure.TIM_Mode = TIM_Mode_OCToggle;           
  TIM_InitStructure.TIM_Prescaler = 0x4;                     
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal; 
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;       
  TIM_InitStructure.TIM_Period = 0xFFFF;                        
  TIM_InitStructure.TIM_Channel = TIM_Channel_1;                                        
  TIM_InitStructure.TIM_Polarity1 = TIM_Polarity1_Low; 
 
  TIM_Init(TIM1, &TIM_InitStructure);

  /* Reset TIM0 counter */
  TIM_ResetCounter(TIM0);

  /* Enable TIM1 counter */
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

  /* TIM0_TI1 pin configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;              
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* TIM0_OC1 and TIM1_OC1 pins configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;              
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_28;
  GPIO_Init(GPIO0, &GPIO_InitStructure);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


