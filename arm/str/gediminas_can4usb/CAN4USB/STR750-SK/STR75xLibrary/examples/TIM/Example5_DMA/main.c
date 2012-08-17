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
#define TIM0_DMAB_Address 0xFFFF8C60

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u16 SRC_Buffer[13] = {0x0, 0x1F, 0x3F, 0x7F, 0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF,
                      0x1FFF, 0x3FFF, 0x7FFF, 0x9FFF} ;
TIM_InitTypeDef TIM_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
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
    
/* TIM0 configuration in PWM mode (CK_TIM = 30 MHz)---------------------------*/
  /* TIM0 signal frequency: CK_TIM / (TIM_Prescaler + 1) * (TIM_Period + 1) = 13.8 Hz */  
  TIM_InitStructure.TIM_Mode = TIM_Mode_PWM;              
  TIM_InitStructure.TIM_Prescaler = 0x20;                     
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal; 
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;       
  TIM_InitStructure.TIM_Period = 0xFFFF;                        
  TIM_InitStructure.TIM_Channel = TIM_Channel_2;                
  TIM_InitStructure.TIM_Pulse2 = 0x3FFF;                        
  TIM_InitStructure.TIM_Polarity2 = TIM_Polarity2_Low;         
  
  TIM_Init(TIM0, &TIM_InitStructure); 

/* DMA Configuration ---------------------------------------------------------*/ 
  DMA_InitStructure.DMA_BufferSize = 13;
  DMA_InitStructure.DMA_SRCBaseAddr = (u32)SRC_Buffer;
  DMA_InitStructure.DMA_DSTBaseAddr = TIM0_DMAB_Address;
  DMA_InitStructure.DMA_SRC = DMA_SRC_INCR;
  DMA_InitStructure.DMA_DST = DMA_DST_NOT_INCR;
  DMA_InitStructure.DMA_SRCSize = DMA_SRCSize_HalfWord;
  DMA_InitStructure.DMA_SRCBurst = DMA_SRCBurst_1Data;
  DMA_InitStructure.DMA_DSTSize = DMA_DSTSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeriphDST;
  DMA_Init(DMA_Stream0, &DMA_InitStructure);

  /* Enable DMA Stream0 transfer */
  DMA_Cmd(DMA_Stream0, ENABLE);
  
  /* DMA transfer on update event, TIM0_OCR2 is the destination register */
  TIM_DMAConfig(TIM_DMASource_Update, TIM_OCRMState_Disable, TIM_DMABase_OCR2);

  /* Enable TIM0 DMA transfer on update event */
  TIM_DMACmd(TIM_DMASource_Update, ENABLE);
 
  /* Enable TIM0 counter */
  TIM_Cmd(TIM0, ENABLE); 

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

  /* TIM0_OC2 pin configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;              
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIO1, &GPIO_InitStructure);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

