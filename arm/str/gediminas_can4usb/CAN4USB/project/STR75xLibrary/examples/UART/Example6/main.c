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
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
  UART_InitTypeDef UART_InitStructure;
  ErrorStatus OSC4MStartUpStatus;
  
/* Private function prototypes -----------------------------------------------*/
  void MRCC_Configuration(void);
  void GPIO_Configuration(void);

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

/* UARTx configuration -------------------------------------------------------*/
  /* UARTx configured as follow:
        - Word Length = 8 Bits
        - One Stop Bit
        - Odd parity
        - BaudRate = 115200 baud
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - Receive and transmit FIFOs are enabled
        - Transmit FIFO level have 8 bytes depth 
        - Receive FIFO level have 8 bytes depth 
  */
  UART_InitStructure.UART_WordLength = UART_WordLength_8D;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_Odd ;
  UART_InitStructure.UART_BaudRate = 115200;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
  UART_InitStructure.UART_Mode = UART_Mode_Tx_Rx;
  UART_InitStructure.UART_FIFO = UART_FIFO_Enable;
  UART_InitStructure.UART_TxFIFOLevel = UART_FIFOLevel_1_2; /* FIFO size 16 bytes, FIFO level 8 bytes */
  UART_InitStructure.UART_RxFIFOLevel = UART_FIFOLevel_1_2; /* FIFO size 16 bytes, FIFO level 8 bytes */

  UART_Init(UARTx, &UART_InitStructure);
  
  /* Enable UARTx */
  UART_Cmd(UARTx, ENABLE);
  /* Output a message on Hyperterminal using printf function */
  printf("UART Example6 :retarget the C library printf function to the UART\n\r"); 

  while (1);
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
  
  /* Enable UARTx and GPIO Clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_UARTx | MRCC_Peripheral_GPIO, ENABLE);
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

  /* Configure the UARTx_Rx pin as Input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin =  GPIO_RxPin;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* Configure the UARTx_Tx as Alternate function Push-Pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_TxPin;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

}

/*******************************************************************************
* Function Name  : fputc
* Description    : Retargets the C library printf function to the UART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the UART */

    UART_SendData(UARTx, (u8) ch);
    while(UART_GetFlagStatus(UARTx, UART_FLAG_TxFIFOFull) != RESET);

    return ch;
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
