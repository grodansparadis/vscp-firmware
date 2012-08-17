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
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RTCClockSource_OSC32K  /* Use the 32 KHz oscillator as RTC clock source */
//#define RTCClockSource_OSC4M /* Use OSC4M/128 as RTC clock source */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 THH = 0, TMM = 0, TSS = 0;
u32 Tmp = 0;
bool TimeDisplay = FALSE;
ErrorStatus OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void MRCC_Configuration(void);
void GPIO_Configuration(void);
void EIC_Configuration(void);
void UART_Configuration(void);

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

  /* Configure UART */
  UART_Configuration();

/* RTC configuration ---------------------------------------------------------*/
  /* Enable RTC Clock */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_RTC, ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Enable the RTC Second interrupt */
  RTC_ITConfig(RTC_IT_Second, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
 /* Set RTC prescaler: set RTC period to 1sec */
#ifdef RTCClockSource_OSC32K
  RTC_SetPrescaler(32767); /* RTC period = CK_RTC/RTC_PR = (32.768 KHz)/(32767+1) */
#else  /* RTCClockSource_OSC4M */
  RTC_SetPrescaler(31249); /* RTC period = CK_RTC/RTC_PR = (4MHz/128)/(31249+1) */
#endif

  /* Adjust hours */
  THH = 00;

  /* Adjust minutes */
  TMM = 00;

  /* Adjust seconds */
  TSS = 00;

  /* Calculate the counter value */
  Tmp = THH * 3600 + TMM * 60 + TSS;

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Adjust the counter value */
  RTC_SetCounter(Tmp);

  /* Configure and enable the interrupt controller */
  EIC_Configuration();

  while(1)
  {
    if(TimeDisplay == TRUE)
    { /* Update and display time each second */

      /* Load the Counter value */
      Tmp = RTC_GetCounter();

      /* Compute  hours */
      THH = Tmp/3600;
      /* Compute minutes */
      TMM = (Tmp % 3600)/60;
      /* Compute seconds */
      TSS = (Tmp % 3600)% 60;

      printf("Time :%0.2d:%0.2d:%0.2d\r",THH, TMM, TSS);

      TimeDisplay = FALSE;
    }
  }
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
    /* Set HCLK to 30MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div2);

    /* Set CKTIM to 30MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div1);

    /* Set PCLK to 30MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div1);

    /* Set CK_SYS to 60 MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);
  
  /* Enable GPIO and UART0 clock */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_UART0, ENABLE);

/* CK_RTC clock selection ----------------------------------------------------*/
#ifdef RTCClockSource_OSC32K
  /* Enable OSC32K */
  MRCC_OSC32KConfig(MRCC_OSC32K_Enable, MRCC_OSC32KBypass_Disable);
  while(MRCC_GetFlagStatus(MRCC_FLAG_OSC32KRDY) == RESET);
  /* Select OSC32K as CK_RTC clock source */
  MRCC_CKRTCConfig(MRCC_CKRTC_OSC32K);
#else /* RTCClockSource_OSC4M */
  /* Select OSC4M divided by 128 as CK_RTC clock source */
  MRCC_CKRTCConfig(MRCC_CKRTC_OSC4M_Div128);
#endif

  /* Wait for RTC clock to be ready */
  while(MRCC_GetFlagStatus(MRCC_FLAG_CKRTCOK) == RESET);
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

  /* Configure the UART0_Rx pin as Input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* Configure the UART0_Tx as Alternate function Push-Pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* Configure P2.4 as Output Push-Pull */
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

  /* Configure and enable RTC_IRQChannel */
  EIC_IRQInitStructure.EIC_IRQChannel = RTC_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd= ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable the interrupt controller */
  EIC_IRQCmd(ENABLE);

}
/*******************************************************************************
* Function Name  : UART_Configuration
* Description    : Configures UART0.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART_Configuration(void)
{
  UART_InitTypeDef UART_InitStructure;

/* UART0 configuration -------------------------------------------------------*/
  /* UART0 configured as follow:
        - Word Length = 8 Bits
        - One Stop Bit
        - Odd parity
        - BaudRate = 115200 baud
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - Receive and transmit FIFOs are disabled
  */
  UART_InitStructure.UART_WordLength = UART_WordLength_8D;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_Odd ;
  UART_InitStructure.UART_BaudRate = 115200;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
  UART_InitStructure.UART_Mode = UART_Mode_Tx_Rx;
  UART_InitStructure.UART_FIFO = UART_FIFO_Disable;

  UART_Init(UART0, &UART_InitStructure);

  /* Enable UART0 */
  UART_Cmd(UART0, ENABLE);
}


/*******************************************************************************
* Function Name  : fputc
* Description    : Retargets the C library printf function to the UART0.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the UART0 */

    UART_SendData(UART0, (u8) ch);
    while(UART_GetFlagStatus(UART0, UART_FLAG_TxFIFOFull) != RESET);

    return ch;
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
