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
EXTIT_InitTypeDef EXTIT_InitStructure;
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

  /* Configure the system clocks */
  MRCC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Clear EXTIT line 7 pending bit  */
  EXTIT_ClearITPendingBit(EXTIT_ITLine7);

  /* Enable EXTIT line 7 on falling edge */
  EXTIT_InitStructure.EXTIT_ITLine = EXTIT_ITLine7;
  EXTIT_InitStructure.EXTIT_ITTrigger = EXTIT_ITTrigger_Falling;
  EXTIT_InitStructure.EXTIT_ITLineCmd = ENABLE;
  EXTIT_Init(&EXTIT_InitStructure);

  /* Configure and enable the interrupt controller */
  EIC_Configuration();

  /* Generate software interrupt: simulate a falling edge applied on EXTIT line 7 */
  EXTIT_GenerateSWInterrupt(EXTIT_ITLine7);

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
    /* Set HCLK to 30 MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div2);

    /* Set CKTIM to 30 MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div1);

    /* Set PCLK to 15 MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div2);

    /* Set CKSYS to 60 MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3); 
  
  /* Enable EXTIT and GPIO clock */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_EXTIT | MRCC_Peripheral_GPIO, ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure GPIO1 pin 5 in input mode */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIO1, &GPIO_InitStructure);

  /* Configure GPIO1 pin 1 in Output push-pull mode */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIO1, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : EIC_Configuration
* Description    : Configures and enables the interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_Configuration(void)
{
  EIC_IRQInitTypeDef EIC_IRQInitStructure;

  /* Enable and configure the priority of the EXTIT IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = EXTIT_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
 
  /* Enable the interrupt controller */
  EIC_IRQCmd(ENABLE);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
