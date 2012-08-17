/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 07/17/2006
* Description        : Main program body
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
SMI_InitTypeDef SMI_InitStructure;
ErrorStatus OSC4MStartUpStatus;
  
/* Private function prototypes -----------------------------------------------*/
void MRCC_Configuration(void);
void GPIO_Configuration(void);
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
 
  /* Enable SMI Fast Read mode */
  SMI_FastReadConfig(SMI_FastRead_Enable);

  /* SMI clock configuration */
  SMI_InitStructure.SMI_ClockHold = 0;
  SMI_InitStructure.SMI_Prescaler = 1;  
  SMI_InitStructure.SMI_DeselectTime = 1;
  SMI_Init(&SMI_InitStructure);

  /* Configure the system clocks: HCLK = SMI_CK = 48MHz */
  MRCC_Configuration();
  
  /* Configure the used GPIO pins */
  GPIO_Configuration();
  
/* Toggle leds connected to P1.00, P1.01, P1.02 and P1.03 pins ---------------*/
  while(1)
  {
/* Turn on led connected to P1.0..3 and P1.14 */
    GPIO_Write(GPIO2, 0x00);
    GPIO_Write(GPIO1, 0xFFFFF);
    GPIO_Write(GPIO0, 0x00);

    /* Insert delay */
    Delay(0xFFFF);
        
    /* Turn on leds connected to P2.3 and P2.4 pins */
    GPIO_Write(GPIO2, 0xFFFFF);
    GPIO_Write(GPIO1, 0x00);
    GPIO_Write(GPIO0, 0x00);

    /* Insert delay */
    Delay(0xFFFF);

    
    /* Turn on led connected to P0.0,P0.1 and P0.23..31 */
    GPIO_Write(GPIO2, 0x00);
    GPIO_Write(GPIO1, 0x00);
    GPIO_Write(GPIO0, 0xFFFFFFFF);

    /* Insert delay */
    Delay(0xFFFF);
  }
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
    /* Set HCLK to 48MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div1);

    /* Set CKTIM to 24MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div2);

    /* Set PCLK to 24MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div1);

    /* Set CKSYS to 48MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_12);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3); 
   
  /* Enable GPIOs clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO, ENABLE);
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

  /* GPIO Configuration --------------------------------------------------------*/
  /* Configure P2.3 and P2.4 as output push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_Init(GPIO2, &GPIO_InitStructure);

  /* Configure P1.0..P1.3 and P1.14 as output push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_14;
  GPIO_Init(GPIO1, &GPIO_InitStructure);

  /* Configure P0.0,P0.1,P0.23...P.31 as output push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_23|GPIO_Pin_24|GPIO_Pin_25|
                                 GPIO_Pin_26|GPIO_Pin_27|GPIO_Pin_28|GPIO_Pin_29|GPIO_Pin_30|
                                 GPIO_Pin_31;
  GPIO_Init(GPIO0, &GPIO_InitStructure);
  
  /* Output HCLK clock on MCO pin (P0.01) */  
  MRCC_MCOConfig(MRCC_MCO_HCLK, MRCC_MCOPrescaler_1);  
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
 for(; nCount != 0; nCount--);
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
