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
#define ADC_D14_Base        0xFFFF8488
#define TIM0_OCR2_Address  0xFFFF8C38

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DMA_InitTypeDef   DMA_InitStructure;
ADC_InitTypeDef   ADC_InitStructure;
TIM_InitTypeDef   TIM_InitStructure;
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

  /* MRCC configuration */
  MRCC_Configuration();
  
  /* GPIO pins configuration */
  GPIO_Configuration();

  /* Reset DMA stream3 registers */
  DMA_DeInit(DMA_Stream3);

/* ADC configuration ---------------------------------------------------------*/ 
  ADC_InitStructure.ADC_ConversionMode = ADC_ConversionMode_Scan;
  ADC_InitStructure.ADC_ExtTrigger = ADC_ExtTrigger_Disable;
  ADC_InitStructure.ADC_AutoClockOff = ADC_AutoClockOff_Disable;
  ADC_InitStructure.ADC_SamplingPrescaler = 0;
  ADC_InitStructure.ADC_ConversionPrescaler = 0;
  ADC_InitStructure.ADC_FirstChannel = ADC_CHANNEL14;
  ADC_InitStructure.ADC_ChannelNumber = 1;
  ADC_Init(&ADC_InitStructure);

  /* Enable DMA transfer for channel14 */
  ADC_DMAConfig(ADC_DMA_CHANNEL14, ENABLE);

  /* Enable ADC */
  ADC_Cmd(ENABLE);

  /* Start calibration */
  ADC_StartCalibration(ADC_CalibAverage_Enable);

  /* Start conversion */
  ADC_ConversionCmd(ADC_Conversion_Start);

  /* Enable ADC DMA transfer through global enable bit */
  ADC_DMACmd(ADC_DMA_Enable);


/* TIM0 configuration --------------------------------------------------------*/
  TIM_InitStructure.TIM_Mode = TIM_Mode_PWM;              
  TIM_InitStructure.TIM_Prescaler = 0x00;                     
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal; 
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Down;       
  TIM_InitStructure.TIM_Period = 0x350;                        
  TIM_InitStructure.TIM_Channel = TIM_Channel_2;                
  TIM_InitStructure.TIM_Pulse2 = 0x350;                        
  TIM_InitStructure.TIM_Polarity2 = TIM_Polarity2_Low;         
  TIM_Init(TIM0, &TIM_InitStructure); 

  /* Start TIM0 */
  TIM_Cmd(TIM0, ENABLE);

/* DMA configuration ---------------------------------------------------------*/ 
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_SRCBaseAddr = ADC_D14_Base;
  DMA_InitStructure.DMA_DSTBaseAddr = TIM0_OCR2_Address;
  DMA_InitStructure.DMA_SRC = DMA_SRC_NOT_INCR;
  DMA_InitStructure.DMA_DST = DMA_DST_NOT_INCR;
  DMA_InitStructure.DMA_SRCSize = DMA_SRCSize_HalfWord;
  DMA_InitStructure.DMA_SRCBurst = DMA_SRCBurst_1Data;
  DMA_InitStructure.DMA_DSTSize = DMA_DSTSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeriphDST;
  DMA_Init(DMA_Stream3, &DMA_InitStructure);

  /* Enable DMA Stream3 transfer */
  DMA_Cmd(DMA_Stream3, ENABLE);

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
    /* Set HCLK to 60MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div1);

    /* Set CKTIM to 60MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div1);

    /* Set PCLK to 30MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div2);

    /* Enable Flash Burst mode */
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);

    /* Set CK_SYS to 60 MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);
 
  
  /* GPIO, ADC and TIM0 clock source enable */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_ADC |
                             MRCC_Peripheral_TIM0, ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
 
  /* ADC Channel14 pin configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;
  GPIO_Init(GPIO1, &GPIO_InitStructure);

  /* TIM0 OC2 pin configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIO1, &GPIO_InitStructure);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
