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
MRCC_ClocksTypeDef  MRCC_Clocks;
ErrorStatus OSC4MStartUpStatus01;

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void EIC_Configuration(void);
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
    
/* Configures the different system clocks ------------------------------------*/
  /* MRCC system reset(for debug purpose) */
  MRCC_DeInit();

  /* Wait for OSC4M start-up */
  OSC4MStartUpStatus01 = MRCC_WaitForOSC4MStartUp();

  if(OSC4MStartUpStatus01 == SUCCESS)
  {
    /* Set HCLK to 60MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div1);

    /* Set CKTIM to 30MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div2);

    /* Set PCLK to 30MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div1);

    /* Enable Flash burst mode */
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);

    /* Set CK_SYS to 60 MHz */
    if(MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15) == ERROR)
    {
      /* Add here some code to deal with this error */
    }
  }
  else
  {
    /* Add here some code to deal with this error */        
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);

/* Configure RTC clock source ------------------------------------------------*/
  /* Enable OSC32K */
  if(MRCC_OSC32KConfig(MRCC_OSC32K_Enable, MRCC_OSC32KBypass_Disable) == SUCCESS)
  {
    /* Wait for OSC32K clock to be stable */
    while(MRCC_GetFlagStatus(MRCC_FLAG_OSC32KRDY) == RESET);

    /* Select OSC32K as RTC clock source */
    if(MRCC_CKRTCConfig(MRCC_CKRTC_OSC32K)== SUCCESS)
    {
      /* Wait for RTC clock to be ready for use */
      while(MRCC_GetFlagStatus(MRCC_FLAG_CKRTCOK) == RESET);
    }
    else
    {
      /* Add here some code to deal with this error */
    }
  }
  else
  {
    /* Add here some code to deal with this error */
  }

  /* This function fills a MRCC_ClocksTypeDef structure with the current status
     and frequencies of different on chip clocks (for debug purpose) */
   MRCC_GetClocksStatus(&MRCC_Clocks);

  /* The  MRCC_Clocks members should be as following:
      MRCC_Clocks.CKSYS_Source    = OSC4MPLL
      MRCC_Clocks.CKRTC_Source    = OSC32K
      MRCC_Clocks.CKUSB_Source    = Disabled
      MRCC_Clocks.PLL_Status      = ON
      MRCC_Clocks.OSC4M_Status    = ON
      MRCC_Clocks.LPOSC_Status    = OFF
      MRCC_Clocks.OSC32K_Status   = ON
      MRCC_Clocks.CKSYS_Frequency = 60000000
      MRCC_Clocks.HCLK_Frequency  = 60000000
      MRCC_Clocks.CKTIM_Frequency = 30000000
      MRCC_Clocks.PCLK_Frequency  = 30000000 */

  /* Enable GPIO clock */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO, ENABLE);

  /* Configure the used I/O ports pins */
  GPIO_Configuration();

  /* Enable No Clock Detected interrupt */
  MRCC_ITConfig(MRCC_IT_NCKD, ENABLE);
  
  /* Configure the interrupt controller */
  EIC_Configuration();


  while(1)
  {
    /* Turn on led conencted to P2.3 pin */
    GPIO_WriteBit(GPIO2, GPIO_Pin_3, Bit_RESET);

    Delay(0x7FFFF);

    /* Turn off conencted to P2.3 pin */
    GPIO_WriteBit(GPIO2, GPIO_Pin_3, Bit_SET);

    Delay(0x7FFFF);
  }
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the used I/O ports pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Configure P2.3 as Output push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIO2, &GPIO_InitStructure);
  
  /* Configure P0.01 as alternate function (MCO) */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* Output HCLK clock on MCO pin (P0.01) */  
  MRCC_MCOConfig(MRCC_MCO_HCLK, MRCC_MCOPrescaler_1);
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
 
  /* Configure and enable MRCC IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = MRCC_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
 
  /* Enable EIC IRQ output request to CPU */
  EIC_IRQCmd(ENABLE); 
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
  for(; nCount!= 0;nCount--);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


