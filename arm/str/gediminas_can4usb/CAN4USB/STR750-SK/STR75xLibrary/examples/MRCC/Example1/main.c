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
ErrorStatus OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void MRCC_Configuration(void);
void GPIO_Configuration(void);
void RTC_Configuration(void);
void EXTIT_Configuration(void);
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

  /* EXTIT configuration */
  EXTIT_Configuration();

  /* RTC configuration */
  RTC_Configuration();

  /* EIC configuration */
  EIC_Configuration();

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
    /* Set HCLK to 60MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div1);

    /* Set CKTIM to 30MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div2);

    /* Set PCLK to 30MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div1);

    /* Enable Flash burst mode */
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);

    /* Set CK_SYS to 60 MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }
  
  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);
  
  /* Enable EXTIT and GPIO clock */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_EXTIT | MRCC_Peripheral_GPIO, ENABLE);
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

  /* Configure P1.05 as input (EXTIT_Line7) */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIO1, &GPIO_InitStructure);

  /* Configure P0.01 as alternate function (MCO) */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* Output HCLK clock on P0.01 pin */
  MRCC_MCOConfig(MRCC_MCO_HCLK, MRCC_MCOPrescaler_1);
}

/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures RTC clock source and prescaler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
  /* If the system has resumed from STANDBY mode, there is no need to configure
     the RTC clock source */
  if(MRCC_GetFlagStatus(MRCC_FLAG_STDB) != RESET)
  {
    /* Clear the STDB flag */
    MRCC_ClearFlag(MRCC_FLAG_STDB);
  }
  else /* Configure the RTC clock source */
  {
    /* RTC clock source configuration ----------------------------------------*/
    /* Enable OSC32K*/
    MRCC_OSC32KConfig(MRCC_OSC32K_Enable, MRCC_OSC32KBypass_Disable);
    while(MRCC_GetFlagStatus(MRCC_FLAG_OSC32KRDY) == RESET);

    /* Select OSC32K as CK_RTC clock source */
    MRCC_CKRTCConfig(MRCC_CKRTC_OSC32K);
    /* Wait for RTC clock to be ready */
    while(MRCC_GetFlagStatus(MRCC_FLAG_CKRTCOK) == RESET);
  }

  /* RTC configuration -------------------------------------------------------*/
  /* Enable RTC Clock */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_RTC, ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();
 
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Set RTC prescaler: set RTC period to 1 second */
  RTC_SetPrescaler(32767); /* RTC period = CK_RTC/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();  
  /* Enable the RTC Second interrupt */
  RTC_ITConfig(RTC_IT_Second, ENABLE);
}

/*******************************************************************************
* Function Name  : EXTIT_Configuration
* Description    : Configures EXTIT line7.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT_Configuration(void)
{
  EXTIT_InitTypeDef  EXTIT_InitStructure;

  /* Configure EXTIT line7(P1.05) to generate an interrupt on falling edge */
  EXTIT_ClearITPendingBit(EXTIT_ITLine7);

  EXTIT_InitStructure.EXTIT_ITLine = EXTIT_ITLine7;
  EXTIT_InitStructure.EXTIT_ITTrigger = EXTIT_ITTrigger_Falling;
  EXTIT_InitStructure.EXTIT_ITLineCmd = ENABLE;
  EXTIT_Init(&EXTIT_InitStructure);
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

  /* Configure and enable EXTIT_IRQChannel */
  EIC_IRQInitStructure.EIC_IRQChannel = EXTIT_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 2;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Configure and enable RTC_IRQChannel */
  EIC_IRQInitStructure.EIC_IRQChannel = RTC_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable the interrupt controller */
  EIC_IRQCmd(ENABLE);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


