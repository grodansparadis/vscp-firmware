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
TB_InitTypeDef TB_InitStructure;     
  
u32 LPOSCFreq = 250000, TBICValue; 
u8 StartUpCounter = 0;
ErrorStatus OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void MRCC_Configuration(void);
void GPIO_Configuration(void);
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

  /* MRCC Configuration */
  MRCC_Configuration();
  
  /* GPIO Pins Configuration */
  GPIO_Configuration();

/* TB configuration in Input Capture mode ------------------------------------*/
  TB_DeInit();
  TB_InitStructure.TB_Mode = TB_Mode_IC; 
  TB_InitStructure.TB_ClockSource = TB_ClockSource_CKTIM;
  TB_InitStructure.TB_CounterMode = TB_CounterMode_Up ;  
  TB_InitStructure.TB_ICAPolarity = TB_ICAPolarity_Rising; 
  TB_InitStructure.TB_Prescaler = 0x2;    
  TB_InitStructure.TB_AutoReload = 0xFFFF;    
  TB_Init(&TB_InitStructure);

  /* Enable RTC clock measurement: CK_RTC is connected to TB IC1 */
  MRCC_RTCMConfig(MRCC_RTCM_Enable);

  /* Clear Input Capture flag */
  TB_ClearFlag(TB_FLAG_IC);

  /* Enable TB counter */
  TB_Cmd(ENABLE);

  /* Wait untill Input Capture flag is set */
  while(TB_GetFlagStatus(TB_FLAG_IC) == RESET);
  /* Clear Input Capture flag */
  TB_ClearFlag(TB_FLAG_IC);
  /* Wait untill Input Capture flag is set */
  while(TB_GetFlagStatus(TB_FLAG_IC) == RESET);

  /* Get the TB Capture value */
  TBICValue = TB_GetICAP1();

  /* Clear Input Capture flag */
  TB_ClearFlag(TB_FLAG_IC);

  /* Disable TB counter */
  TB_Cmd(DISABLE);

  /* Compute LPOSC frequency value */
  if(TBICValue != 0)
  {
    LPOSCFreq = 20000000/TBICValue;
  }

/* RTC configuration ---------------------------------------------------------*/
  /* Adjust RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(LPOSCFreq-1); 
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second interrupt */
  RTC_ITConfig(RTC_IT_Second, ENABLE);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  
  /* Configure and enable the interrupt controller */
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
    /* Set HCLK to 60 MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div1);

    /* Set CKTIM to 60 MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div1);

    /* Set PCLK to 30 MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div2);

    /* Enable Flash burst mode */
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);

    /* Set CK_SYS to 60 MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }
  
  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);
  
  /* Enable TB and GPIO clock */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_TB | MRCC_Peripheral_GPIO, ENABLE);

/* RTC clock source configuration --------------------------------------------*/  
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
    /* Enable LPOSC */
    MRCC_LPOSCConfig(MRCC_LPOSC_Enable);

    /* Select LPOSC as RTC clock source */
    MRCC_CKRTCConfig(MRCC_CKRTC_LPOSC);
    while(MRCC_GetFlagStatus(MRCC_FLAG_CKRTCOK) == RESET);
  }  

  /* Enable RTC clock */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_RTC, ENABLE);
  
  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();
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
  GPIO_InitTypeDef GPIO_InitStructure;  

  /* Configure P2.4 Output Push-Pull */ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;              
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIO2, &GPIO_InitStructure);

  /* Configure P0.01 as alternate function (MCO) */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* Output HCLK clock on P0.01 pin */
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
  EIC_IRQInitTypeDef EIC_IRQInitStructure;

  /* Configure and enable RTC IRQ Channel */
  EIC_IRQInitStructure.EIC_IRQChannel = RTC_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd= ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable the interrupt controller */
  EIC_IRQCmd(ENABLE);
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

