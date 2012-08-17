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
ADC_InitTypeDef  ADC_InitStructure;
ErrorStatus  OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void MRCC_Configuration(void);
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

  /* MRCC configuration */
  MRCC_Configuration();
  
  /* GPIO pins configuration */
  GPIO_Configuration();
    
  /* EIC configuration */
  EIC_Configuration();

/* ADC configuration ---------------------------------------------------------*/
  ADC_InitStructure.ADC_ConversionMode = ADC_ConversionMode_Scan;
  ADC_InitStructure.ADC_ExtTrigger = ADC_ExtTrigger_Disable;
  ADC_InitStructure.ADC_AutoClockOff = ADC_AutoClockOff_Disable;
  ADC_InitStructure.ADC_SamplingPrescaler = 5;
  ADC_InitStructure.ADC_ConversionPrescaler = 1;
  ADC_InitStructure.ADC_FirstChannel = ADC_CHANNEL14;
  ADC_InitStructure.ADC_ChannelNumber = 1;
  ADC_Init(&ADC_InitStructure);

  /* Analog watchdog0 configuration */
  ADC_AnalogWatchdogConfig(ADC_AnalogWatchdog0, ADC_CHANNEL14, 0x100, 0x200); 
  
  /* Enable EOC interrupt */
  ADC_ITConfig(ADC_IT_EOC, ENABLE);
  
  /* Enable ADC */
  ADC_Cmd(ENABLE);
  
  /* Start calibration */
  ADC_StartCalibration(ADC_CalibAverage_Enable);
  
  /* Start conversion */
  ADC_ConversionCmd(ADC_Conversion_Start);
  
  /* Enable Analog watchdog0 */
  ADC_AnalogWatchdogCmd(ADC_AnalogWatchdog0, ENABLE);  

  /* Wait for each EOC interrupt */
  while(1);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the used I/O ports pins 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  /* P1.0 and P1.0 Output push-pull pins configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIO1, &GPIO_InitStructure);

  /* ADC Channel14 pin configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;
  GPIO_Init(GPIO1, &GPIO_InitStructure);
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
    /* Set HCLK to 30MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div2);

    /* Set CKTIM to 15MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div2);

    /* Set PCLK to 15MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div1);

    /* Set CK_SYS to 60 MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3); 

  /* ADC and GPIO clock source enable */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_ADC | MRCC_Peripheral_GPIO, ENABLE);
}

/*******************************************************************************
* Function Name  : EIC_Configuration
* Description    : Configures the used EIC interrupts requests
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_Configuration(void)
{
  EIC_IRQInitTypeDef  EIC_IRQInitStructure;

  /* ADC IRQ Channel configuration */
  EIC_IRQInitStructure.EIC_IRQChannel = ADC_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable the IRQ interrupt controller */
  EIC_IRQCmd(ENABLE);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
