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
ADC_InitTypeDef ADC_InitStructure;
PWM_InitTypeDef PWM_InitStructure;
ErrorStatus OSC4MStartUpStatus;
u16 Channel14Value; 

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void MRCC_Configuration(void);
void Delay(u32 Xtime);

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

/* ADC configuration ---------------------------------------------------------*/ 
  ADC_StructInit(&ADC_InitStructure);  
  ADC_InitStructure.ADC_ConversionMode = ADC_ConversionMode_Scan;
  ADC_InitStructure.ADC_SamplingPrescaler = 2;
  ADC_InitStructure.ADC_ConversionPrescaler = 0;
  ADC_InitStructure.ADC_FirstChannel = ADC_CHANNEL11;
  ADC_InitStructure.ADC_ChannelNumber = 3;
  ADC_Init(&ADC_InitStructure);

  /* Injected conversion configuration */
  ADC_InjectedConversionConfig(ADC_Injec_ExtTrigger_RisingEdge, ADC_CHANNEL14, 1);

  /* Enable ADC */
  ADC_Cmd(ENABLE);

  /* Start calibration */
  ADC_StartCalibration(ADC_CalibAverage_Enable);

  /* Start conversion */
  ADC_ConversionCmd(ADC_Conversion_Start);

/* PWM configuration ---------------------------------------------------------*/ 
  PWM_InitStructure.PWM_Mode = PWM_Mode_PWM;        
  PWM_InitStructure.PWM_Prescaler = 0x63;   
  PWM_InitStructure.PWM_CounterMode = PWM_CounterMode_Down; 
  PWM_InitStructure.PWM_Period = 0x7530;      
  PWM_InitStructure.PWM_Complementary = PWM_Complementary_Disable;  
  PWM_InitStructure.PWM_OCState = PWM_OCState_Enable;         
  PWM_InitStructure.PWM_Channel = PWM_Channel_1;        
  PWM_InitStructure.PWM_Pulse1 = 0x4E20;
  PWM_InitStructure.PWM_DTRAccess = PWM_DTRAccess_Disable;           
  PWM_InitStructure.PWM_Polarity1 = PWM_Polarity1_Low;        
  PWM_InitStructure.PWM_RepetitionCounter = 0x0;
  PWM_Init(&PWM_InitStructure);

  /* Enable main PWM outputs */
  PWM_CtrlPWMOutputs(ENABLE);

  /* Select TRGO source signal */
  PWM_TRGOSelection(PWM_TRGOMode_OC);

  /* Start injected conversion using the external PWM TRGO */
  PWM_Cmd(ENABLE);

 /* Wait for the JECH event --------------------------------------------------*/
  while(1)
  {
    if(ADC_GetFlagStatus(ADC_FLAG_JECH) == SET)
    {
      /* Get ADC channel14 converted value */
      Channel14Value = ADC_GetConversionValue(ADC_CHANNEL14);

      /* Toggle the led connected to P2.3 pin */
      GPIO_WriteBit(GPIO2, GPIO_Pin_3, Bit_RESET); 
      Delay(0xFFFF); 
      GPIO_WriteBit(GPIO2, GPIO_Pin_3, Bit_SET);

      /* Clear end of injected chain conversion flag */
      ADC_ClearFlag(ADC_FLAG_JECH);
    }
  }
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

    /* Set CKTIM to 30MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div1);

    /* Set PCLK to 15MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div2);

    /* Set CK_SYS to 60 MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);
  
  /* ADC, PWM and GPIO clock source enable */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_ADC | MRCC_Peripheral_PWM |
                             MRCC_Peripheral_GPIO, ENABLE); 
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

  /* P2.3 Output push-pull pin configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIO2, &GPIO_InitStructure);

  /* ADC Channels[11..14] pins configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_Init(GPIO1, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(u32 Xtime)
{
  u32 j;

  for(j= Xtime; j>0; j--);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


