/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 07/17/2006
* Description        : Main program body.
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
TB_InitTypeDef TB_InitStructure;        
ErrorStatus OSC4MStartUpStatus;

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

  /* MRCC Configuration */
  MRCC_Configuration();
  
  /* GPIO Pins Configuration */
  GPIO_Configuration();

  /* EIC Configuration */
  EIC_Configuration();

  /* RTC clock source configuration ----------------------------------------*/
  /* Enable OSC32K*/
  MRCC_OSC32KConfig(MRCC_OSC32K_Enable, MRCC_OSC32KBypass_Disable);
  /* Wait for OSC32K to be ready */
  while(MRCC_GetFlagStatus(MRCC_FLAG_OSC32KRDY) == RESET);

  /* Select OSC32K as CK_RTC clock source */
  MRCC_CKRTCConfig(MRCC_CKRTC_OSC32K);
  /* Wait for RTC clock to be ready */
  while(MRCC_GetFlagStatus(MRCC_FLAG_CKRTCOK) == RESET);
      
 /* TB configuration in Timing mode ------------------------------------------*/
  /* TB is configured to generate an update every 1s using the RTC as clock source
     update period = RTC_clock/(Prescaler+1)(ARR+1) */
  TB_InitStructure.TB_Mode = TB_Mode_Timing;  
  TB_InitStructure.TB_ClockSource = TB_ClockSource_CKRTC;      
  TB_InitStructure.TB_Prescaler = 7;
  TB_InitStructure.TB_CounterMode = TB_CounterMode_Down;   
  TB_InitStructure.TB_AutoReload = 4095;

  TB_Init(&TB_InitStructure);

  /* Enable TB update interrupt */
  TB_ITConfig(TB_IT_GlobalUpdate, ENABLE);

  /* Enable RTC clock: CK_RTC is connected to TB IC1 */
  MRCC_RTCMConfig(MRCC_RTCM_Enable);

  /* Enable TB counter */
  TB_Cmd(ENABLE);

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

  /* Enable TB and GPIO clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_TB | MRCC_Peripheral_GPIO, ENABLE);
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

  /* Configure P2.4 as output push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIO2, &GPIO_InitStructure);
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

  /* Enable and configure the priority of the TB IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = TB_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable the Interrupt controller to manage IRQ channel*/
  EIC_IRQCmd(ENABLE);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/



