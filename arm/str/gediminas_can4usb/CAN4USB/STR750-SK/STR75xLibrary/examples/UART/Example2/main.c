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
#define TxBufferSize   (countof(TxBuffer) - 1)
#define RxBufferSize   0x20

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
  UART_InitTypeDef UART_InitStructure;
  u8 TxBuffer[] = "UART Example2: UART-Hyperterminal communication using Interrupt\n\r";
  u8 RxBuffer[RxBufferSize];
  u8 TxCounter = 0;
  u8 NbrOfDataToTransfer = TxBufferSize;
  u8 NbrOfDataToRead = RxBufferSize;
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

/* UART0 configuration -------------------------------------------------------*/
  /* UART0 configured as follow:
        - Word Length = 8 Bits
        - One Stop Bit
        - Odd parity
        - BaudRate = 9600 baud
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - Receive and transmit FIFOs are enabled
        - Transmit FIFO level have 8 bytes depth 
        - Receive FIFO level have 2 bytes depth 
  */
  UART_InitStructure.UART_WordLength = UART_WordLength_8D;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_Odd ;
  UART_InitStructure.UART_BaudRate = 9600;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
  UART_InitStructure.UART_Mode = UART_Mode_Tx_Rx;
  UART_InitStructure.UART_FIFO = UART_FIFO_Enable;
  UART_InitStructure.UART_TxFIFOLevel = UART_FIFOLevel_1_2; /* FIFO size 16 bytes, FIFO level 8 bytes */
  UART_InitStructure.UART_RxFIFOLevel = UART_FIFOLevel_1_8; /* FIFO size 16 bytes, FIFO level 2 bytes */

  UART_Init(UART0, &UART_InitStructure);


  /* Fill the UART0 transmit FIFO */
  while(TxCounter < 0x10)
  {
    UART_SendData(UART0, TxBuffer[TxCounter++]);
  }

  /* Enable the UART Receive interrupt: this interrupt is generated when the UART0 
     receive FIFO hold two data */
  UART_ITConfig(UART0, UART_IT_Receive, ENABLE);

  /* Enable the UART Transmit interrupt: this interrupt is generated when the UART0 
     transmit FIFO hold less than eight data */
  UART_ITConfig(UART0, UART_IT_Transmit, ENABLE);

  /* Configure and enable the interrupt controller */
  EIC_Configuration();

  /* Enable the UART0: start sending the transmit FIFO content to the hyperterminal  */
  UART_Cmd(UART0, ENABLE);

  while (1);
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

  /* Enable UART0 and GPIO Clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_UART0 | MRCC_Peripheral_GPIO, ENABLE);
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

  /* Configure the UART0_Tx as Alternate function Push-Pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* Configure the UART0_Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIO0, &GPIO_InitStructure);
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

  /* Configure the interrupt controller */
  EIC_IRQInitStructure.EIC_IRQChannel = UART0_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
  /* Enable the interrupt controller */
  EIC_IRQCmd(ENABLE);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
