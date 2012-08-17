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
u16 i = 0;
ErrorStatus OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void MRCC_Configuration(void);
void GPIO_Configuration(void);
void Delay(u32 nCount);

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

/* TIM0, TIM1 and TIM2 configuration -----------------------------------------*/
   /* CK_TIM = 30 MHz, Prescaler = 0x0, 
      TIM0_CLK = TIM1_CLK = TIM2_CLK = CK_TIM/(Prescaler + 1) = 30 MHz */

  /* TIM0 configuration in PWM mode */
  TIM_InitStructure.TIM_Mode = TIM_Mode_PWM;              
  TIM_InitStructure.TIM_Prescaler = 0x00;                     
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal; 
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Down;       
  TIM_InitStructure.TIM_Period = 0xFFFF;                        
  TIM_InitStructure.TIM_Channel = TIM_Channel_1;                
  TIM_InitStructure.TIM_Pulse1 = 0x3FFF;                        
  TIM_InitStructure.TIM_Polarity1 = TIM_Polarity1_Low;           
  TIM_Init(TIM0, &TIM_InitStructure);

  /* TIM1 configuration in PWM mode */
  TIM_Init(TIM1, &TIM_InitStructure);

  /* TIM2 configuration in PWM mode */
  TIM_Init(TIM2, &TIM_InitStructure);


  /* TIM0 and TIM1 are used as Master in Enable Mode
     TIM1 and TIM2 are used as Slaves in Gated Mode: 
     both the start and the stop are controlled by the Master */ 
  TIM_SynchroConfig(TIM0_Master, TIM1_Slave, TIM_SynchroAction_Enable, TIM_SynchroMode_Gated);
  TIM_SynchroConfig(TIM1_Master, TIM2_Slave, TIM_SynchroAction_Enable, TIM_SynchroMode_Gated);
  
  TIM_Cmd(TIM0, ENABLE);
  TIM_Cmd(TIM1, ENABLE);
  TIM_Cmd(TIM2, ENABLE);

  while(1)
  {
    if(i == 0)
    {
      i = 1;
      TIM_Cmd(TIM0, DISABLE);
      Delay(0xFFFF);
    }
    else
    {
      TIM_Cmd(TIM0, ENABLE);   
      i = 0; 
      Delay(0xFFFF);
    }
  }

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

  /* Enable GPIO, TIM0, TIM1 and TIM2 clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_TIM0 | 
                             MRCC_Peripheral_TIM1 | MRCC_Peripheral_TIM2, ENABLE);
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

  /* TIM0_OC1, TIM2_OC1 and TIM1_OC1 pins configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;              
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_28;
  GPIO_Init(GPIO0, &GPIO_InitStructure);  
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(u32 nCount)
{
  u32 j = 0;

  for(j = nCount; j != 0; j--);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

