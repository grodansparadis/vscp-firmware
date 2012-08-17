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
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define TxBufferSize   (countof(TxBuffer) - 1)

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/  
  UART_InitTypeDef UART_InitStructure;
  u8 TxBuffer[] = "Buffer Send from UART0 to UART2 using Flags";
  u8 RxBuffer[TxBufferSize];
  u8 TxCounter = 0, RxCounter = 0;  
  TestStatus TransferStatus;
  ErrorStatus OSC4MStartUpStatus;
  
/* Private function prototypes -----------------------------------------------*/
  void MRCC_Configuration(void);
  void GPIO_Configuration(void);
  TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength);
  
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

/* UART0 and UART2 configuration ---------------------------------------------*/  
  /* UART0 and UART2 configured as follow:
        - Word Length = 8 Bits
        - One Stop Bit
        - Even parity
        - BaudRate = 230400 baud
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - Receive and transmit FIFOs are enabled
        - Transmit and Receive FIFOs levels have 8 bytes depth 
  */

  UART_InitStructure.UART_WordLength = UART_WordLength_8D;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_Even;
  UART_InitStructure.UART_BaudRate = 230400;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
  UART_InitStructure.UART_Mode = UART_Mode_Tx_Rx;
  UART_InitStructure.UART_FIFO = UART_FIFO_Enable;
  UART_InitStructure.UART_TxFIFOLevel = UART_FIFOLevel_1_2; /* FIFO size 16 bytes, FIFO level 8 bytes */
  UART_InitStructure.UART_RxFIFOLevel = UART_FIFOLevel_1_2; /* FIFO size 16 bytes, FIFO level 8 bytes */

  /* Configure UART0 */
  UART_Init(UART0, &UART_InitStructure);

  /* Configure UART2 */
  UART_Init(UART2, &UART_InitStructure);
  
  /* Enable UART0 */
  UART_Cmd(UART0, ENABLE);

  /* Enable UART2 */
  UART_Cmd(UART2, ENABLE);

  
  while(TxCounter < TxBufferSize)
  {   
    /* Send one byte from UART0 to UART2 */
    UART_SendData(UART0, TxBuffer[TxCounter++]);
    
    /* Loop until the end of transmit */ 
    while(UART_GetFlagStatus(UART0,UART_FLAG_Busy)!= RESET);
    
    /* Read when the Rx FIFO is not empty */
    while(UART_GetFlagStatus(UART2,UART_FLAG_RxFIFOEmpty) == RESET)
    {
      /* Store the received byte in RxBuffer */
      RxBuffer[RxCounter++] = UART_ReceiveData(UART2);  
    }
  } 

  /* Check the received data with the send ones */
  TransferStatus = Buffercmp(TxBuffer, RxBuffer, TxBufferSize);
  /* TransferStatus = PASSED, if the data transmitted from UART0 and  
     received by UART2 are the same */
  /* TransferStatus = FAILED, if the data transmitted from UART0 and 
     received by UART2 are different */
    
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
  
  /* Enable GPIO, UART0 and UART2 Clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_UART0 |
                             MRCC_Peripheral_UART2, ENABLE);
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
  
  /* Configure UART0_Tx and UART2_Tx pins as Alternate function Push-Pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_25;
  GPIO_Init(GPIO0, &GPIO_InitStructure);
  
  /* Configure UART0_Rx and UART2_Rx pins as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_24;
  GPIO_Init(GPIO0, &GPIO_InitStructure); 
}

/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers.
* Input          : - pBuffer1, pBuffer2: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : PASSED: pBuffer1 identical to pBuffer2
*                  FAILED: pBuffer1 differs from pBuffer2
*******************************************************************************/
TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;  
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
