/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 07/17/2006
* Description        : Main program body
********************************************************************************
* History:
* 07/17/2006 : V1.0
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
PWM_InitTypeDef PWM_InitStructure;
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
  
  /* GPIO Pins Configuration */
  GPIO_Configuration();

/* TIM0 synchronized with PWM  -----------------------------------------------*/
  /* TIM0 Configuration in PWM mode*/
  TIM_InitStructure.TIM_Mode = TIM_Mode_PWM;              
  TIM_InitStructure.TIM_Prescaler = 0x02;                     
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal; 
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM_InitStructure.TIM_Channel = TIM_Channel_1;   
  TIM_InitStructure.TIM_Period = 0xF;
  TIM_InitStructure.TIM_Pulse1 = 0x7;                                                               
  TIM_InitStructure.TIM_Polarity1 = TIM_Polarity1_Low;         
  
  TIM_Init(TIM0, &TIM_InitStructure);

  /* PWM Channel 1 configuration */
  PWM_InitStructure.PWM_Mode = PWM_Mode_PWM;        
  PWM_InitStructure.PWM_Prescaler = 0x2;   
  PWM_InitStructure.PWM_CounterMode = PWM_CounterMode_Up; 
  PWM_InitStructure.PWM_Period = 0xBF;      
  PWM_InitStructure.PWM_Complementary = PWM_Complementary_Enable;            
  PWM_InitStructure.PWM_Channel = PWM_Channel_1;        
  PWM_InitStructure.PWM_Pulse1 = 0x3F;
  PWM_InitStructure.PWM_DTRAccess = PWM_DTRAccess_Disable;           
  PWM_InitStructure.PWM_Polarity1 = PWM_Polarity1_Low;   
  PWM_InitStructure.PWM_Polarity1N = PWM_Polarity1N_Low;     
  PWM_InitStructure.PWM_RepetitionCounter = 0x0;

  PWM_Init(&PWM_InitStructure);

  /* TIM0 configured to clock the PWM counter */ 
  TIM_SynchroConfig(TIM0_Master, PWM_Slave, TIM_SynchroAction_OC, TIM_SynchroMode_External);

  /* TIM0 enable */
  TIM_Cmd(TIM0, ENABLE);

  /* PWM enable */
  PWM_Cmd(ENABLE);

  /* PWM Main Outputs enable */
  PWM_CtrlPWMOutputs(ENABLE);

  while(1);
}
  
/*******************************************************************************
* Function Name  : MRCC_Configuration
* Description    : Configures the system clocks.
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

  /* GPIO, TIM0 and PWM clock source config */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_TIM0 |
                             MRCC_Peripheral_PWM, ENABLE);
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
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* TIM0_OC1 pin Pin configuration  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;              
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* PWM1 and PWM1N pins configuration */           
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
  GPIO_Init(GPIO1, &GPIO_InitStructure);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
