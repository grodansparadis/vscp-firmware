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
PWM_InitTypeDef PWM_InitStructure;     
GPIO_InitTypeDef GPIO_InitStructure;                                                 
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
  
  /* GPIO Pins Configuration */
  GPIO_Configuration();

/* PWM configuration in Three complementary PWM mode -------------------------*/
  /* PWM is configured to generate an update every 220ms with CK_TIM = 60MHz:
   update frequency = CK_TIM/(Prescaler+1)(Period+1) = 4.57 Hz */
  /* PWM Channel 1 configuration */
  PWM_InitStructure.PWM_Mode = PWM_Mode_PWM;        
  PWM_InitStructure.PWM_Prescaler = 0xC7;   
  PWM_InitStructure.PWM_CounterMode = PWM_CounterMode_Up; 
  PWM_InitStructure.PWM_Period = 0xFFFF;      
  PWM_InitStructure.PWM_Complementary = PWM_Complementary_Enable;            
  PWM_InitStructure.PWM_Channel = PWM_Channel_1;        
  PWM_InitStructure.PWM_Pulse1 = 0x3FFF;
  PWM_InitStructure.PWM_DTRAccess = PWM_DTRAccess_Disable;           
  PWM_InitStructure.PWM_Polarity1 = PWM_Polarity1_High;   
  PWM_InitStructure.PWM_Polarity1N = PWM_Polarity1N_High;     
  PWM_InitStructure.PWM_RepetitionCounter = 0x5;
  PWM_Init(&PWM_InitStructure);

  /* PWM Channel 2 configuration */
  PWM_InitStructure.PWM_Channel = PWM_Channel_2;        
  PWM_InitStructure.PWM_Pulse2 = 0x7FFF;  
  PWM_InitStructure.PWM_Polarity2 = PWM_Polarity2_High;   
  PWM_InitStructure.PWM_Polarity2N = PWM_Polarity2N_High;
  PWM_Init(&PWM_InitStructure);

/* PWM Channel 3 configuration */
  PWM_InitStructure.PWM_Channel = PWM_Channel_3;        
  PWM_InitStructure.PWM_Pulse3 = 0xBFFF;  
  PWM_InitStructure.PWM_Polarity3 = PWM_Polarity3_High;   
  PWM_InitStructure.PWM_Polarity3N = PWM_Polarity3N_High;
  PWM_InitStructure.PWM_DTRAccess = PWM_DTRAccess_Enable;
  PWM_InitStructure.PWM_Emergency = PWM_Emergency_Disable;
  PWM_InitStructure.PWM_DeadTime = 0xFF;
  PWM_InitStructure.PWM_LOCKLevel = PWM_LOCKLevel_3;
  PWM_InitStructure.PWM_OSSIState = PWM_OSSIState_Enable; 
  PWM_Init(&PWM_InitStructure);


  /* PWM Main Outputs enable */
  PWM_CtrlPWMOutputs(ENABLE);

  /* PWM counter enable */
  PWM_Cmd(ENABLE);

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

  /* Enable PWM and GPIO clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_PWM | MRCC_Peripheral_GPIO, ENABLE);
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
  /* PWM pins configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;              
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 |
                                GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4;

  GPIO_Init(GPIO1, &GPIO_InitStructure);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


