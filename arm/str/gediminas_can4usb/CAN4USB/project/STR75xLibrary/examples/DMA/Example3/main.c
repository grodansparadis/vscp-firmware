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
#define PWM_DMAB_Address  0xFFFF9860

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u16 const SRC_Buffer[128] = {0x0200,0x0400,0x0600,0x0800,0x0A00,0x0C00,
                             0x0E00,0x1000,0x1200,0x1400,0x1600,0x1800, 
                             0x1A00,0x1C00,0x1E00,0x2000,0x2200,0x2400, 
                             0x2600,0x2800,0x2A00,0x2C00,0x2E00,0x3000,
                             0x3200,0x3400,0x3600,0x3800,0x3A00,0x3C00,
                             0x3E00,0x4000,0x4200,0x4400,0x4600,0x4800, 
                             0x4A00,0x4C00,0x4E00,0x5000,0x5200,0x5400, 
                             0x5600,0x5800,0x5A00,0x5C00,0x5E00,0x6000,
                             0x6200,0x6400,0x6600,0x6800,0x6A00,0x6C00,  
                             0x6E00,0x7000,0x7200,0x7400,0x7600,0x7800, 
                             0x7A00,0x7C00,0x7E00,0x8000,0x8200,0x8400, 
                             0x8600,0x8800,0x8A00,0x8C00,0x8E00,0x9000, 
                             0x9200,0x9400,0x9600,0x9800,0x9A00,0x9C00, 
                             0x9E00,0xA000,0xA200,0xA400,0xA600,0xA800,
                             0xAA00,0xAC00,0xAE00,0xB000,0xB200,0xB400, 
                             0xB600,0xB800,0xBA00,0xBC00,0xBE00,0xC000, 
                             0xC200,0xC400,0xC600,0xC800,0xCA00,0xCC00, 
                             0xCE00,0xD000,0xD200,0xD400,0xD600,0xD800, 
                             0xDA00,0xDC00,0xDE00,0xE000,0xE200,0xE400, 
                             0xE600,0xE800,0xEA00,0xEC00,0xEE00,0xF000,
                             0xF200,0xF400,0xF600,0xF800,0xFA00,0xFC00,
                             0xFE00,0xFFFE};
DMA_InitTypeDef   DMA_InitStructure;
PWM_InitTypeDef   PWM_InitStructure;
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

/* PWM configuration ---------------------------------------------------------*/
  PWM_InitStructure.PWM_Mode = PWM_Mode_PWM;        
  PWM_InitStructure.PWM_Prescaler = 99;   
  PWM_InitStructure.PWM_CounterMode = PWM_CounterMode_Down; 
  PWM_InitStructure.PWM_Period = 0xFFFF;       
  PWM_InitStructure.PWM_OCState = PWM_OCState_Enable;             
  PWM_InitStructure.PWM_Channel = PWM_Channel_1;       
  PWM_InitStructure.PWM_Pulse1 = 0x100;               
  PWM_InitStructure.PWM_Polarity1 = PWM_Polarity1_High;           
  PWM_Init(&PWM_InitStructure);

  /* Configure DMA transfer on each PWM update event */
  PWM_DMAConfig(PWM_DMASource_Update, PWM_OCRMState_Disable, PWM_DMABase_OCR1);

  /* Enable PWM update DMA transfer */
  PWM_DMACmd(PWM_DMASource_Update, ENABLE);

  /* Main Outputs enable */
  PWM_CtrlPWMOutputs(ENABLE);

/* DMA Configuration ---------------------------------------------------------*/ 
  DMA_InitStructure.DMA_BufferSize = 128;
  DMA_InitStructure.DMA_SRCBaseAddr = (u32)SRC_Buffer;
  DMA_InitStructure.DMA_DSTBaseAddr = PWM_DMAB_Address;
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

  /* Start PWM */
  PWM_Cmd(ENABLE);

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

    /* Enable Flash burst mode */
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);
  
    /* Set CKSYS to 60MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);  

  /* GPIO and PWM clock source enable */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_PWM, ENABLE); 
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

  /* PWM OC1 pin configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
  GPIO_Init(GPIO1, &GPIO_InitStructure);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
