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
#define RxBufferSize   0xFF

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
  UART_InitTypeDef UART_InitStructure;
  u8 TxBuffer[] = "UART Example1: UART - Hyperterminal communication using hardware flow control\n\r";
  u8 RxBuffer[RxBufferSize];
  u8 NbrOfDataToTransfer = TxBufferSize;
  u8 TxCounter = 0;
  u8 RxCounter = 0;
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
  #ifdef LIBDEBUG
    libdebug();
  #endif
  /* Configure the system clocks */
  MRCC_Configuration();
  
  /* Configure the GPIO ports */
  GPIO_Configuration();
  
/* UART0 configuration -------------------------------------------------------*/
  /* UART0 configured as follow:
        - Word Length = 7 Bits
        - Two Stop Bit
        - No parity
        - BaudRate = 115200 baud
        - Hardware flow control enabled (RTS and CTS signals)
        - Receive and transmit enabled
        - Receive and transmit FIFOs are enabled
        - Transmit and Receive FIFOs levels have 8 bytes depth 
  */
  UART_InitStructure.UART_WordLength = UART_WordLength_7D;
  UART_InitStructure.UART_StopBits = UART_StopBits_2;
  UART_InitStructure.UART_Parity = UART_Parity_No ;
  UART_InitStructure.UART_BaudRate = 115200;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_RTS_CTS;
  UART_InitStructure.UART_Mode = UART_Mode_Tx_Rx;
  UART_InitStructure.UART_FIFO = UART_FIFO_Enable;
  UART_InitStructure.UART_TxFIFOLevel = UART_FIFOLevel_1_2; /* FIFO size 16 bytes, FIFO level 8 bytes */
  UART_InitStructure.UART_RxFIFOLevel = UART_FIFOLevel_1_2; /* FIFO size 16 bytes, FIFO level 8 bytes */

  UART_Init(UART0, &UART_InitStructure);
  
  /* Enable the UART0 */
  UART_Cmd(UART0, ENABLE);

  
/* Communication hyperterminal-UART0 using hardware flow control -------------*/
  /* Send a buffer from UART to hyperterminal */
  while(NbrOfDataToTransfer--)
  {
    UART_SendData(UART0, TxBuffer[TxCounter++]);
    while(UART_GetFlagStatus(UART0, UART_FLAG_TxFIFOFull) != RESET);          
  }
  
  /* Receive a string (Max RxBufferSize bytes) from the Hyperterminal ended by '\r' (Enter key) */
  do
  { 
    if((UART_GetFlagStatus(UART0, UART_FLAG_RxFIFOEmpty) != SET)&&(RxCounter < RxBufferSize))
    {
       RxBuffer[RxCounter] = UART0->DR;
       UART_SendData(UART0, RxBuffer[RxCounter++]);
    }   
 
  }while((RxBuffer[RxCounter - 1] != '\r')&&(RxCounter != RxBufferSize));

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
  
  /* Configure the UART0_RTS pin as Alternate function Push-Pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
  GPIO_Init(GPIO1, &GPIO_InitStructure);

  /* Configure the UART0_Rx and UART0_CTS as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10  | GPIO_Pin_12;
  GPIO_Init(GPIO0, &GPIO_InitStructure); 
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
