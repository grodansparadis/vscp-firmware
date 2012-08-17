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
#define TxBufferSize0   (countof(TxBuffer0) - 1)
#define TxBufferSize2   (countof(TxBuffer2) - 1)
#define RxBufferSize0   TxBufferSize2
#define RxBufferSize2   TxBufferSize0

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
  UART_InitTypeDef UART_InitStructure;
  u8 TxBuffer0[] = "UART Example 4: UART0 -> UART2 using Interrupt";
  u8 TxBuffer2[] = "UART Example 4: UART2 -> UART0 using Interrupt";
  u8 RxBuffer0[RxBufferSize0];
  u8 RxBuffer2[RxBufferSize2];
  u8 TxCounter0 = 0;
  u8 TxCounter2 = 0;
  u8 RxCounter0 = 0; 
  u8 RxCounter2 = 0;
  u8 NbrOfDataToTransfer0 = TxBufferSize0;
  u8 NbrOfDataToTransfer2 = TxBufferSize2;
  u8 NbrOfDataToRead0 = RxBufferSize0;
  u8 NbrOfDataToRead2 = RxBufferSize2;
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
  
/* UART0 and UART2 configuration ---------------------------------------------*/  
  /* UART0 and UART2 configured as follow:
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

  /* Configure UART0 */
  UART_Init(UART0, &UART_InitStructure);

  /* Configure UART2 */
  UART_Init(UART2, &UART_InitStructure);

       
  /* Fill the Transmit FIFOs of UART0 and UART2 */
  while(TxCounter0 < 0x10)
  {
    UART_SendData(UART2, TxBuffer2[TxCounter2++]);
    
    UART_SendData(UART0, TxBuffer0[TxCounter0++]);        
  }
  
  /* Enable UART0 Receive and Transmit interrupt */
  UART_ITConfig(UART0, UART_IT_Receive | UART_IT_Transmit, ENABLE);

  /* Enable UART2 Receive and Transmit interrupt */
  UART_ITConfig(UART2, UART_IT_Receive | UART_IT_Transmit, ENABLE);

  /* Configure and enable the interrupt controller */
  EIC_Configuration();
  
  /* Enable UART2 */
  UART_Cmd(UART2, ENABLE);

  /* Enable UART0 */
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
* Function Name  : EIC_Configuration
* Description    : Configures the interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_Configuration(void)
{
  EIC_IRQInitTypeDef EIC_IRQInitStructure;

  /* Configure and enable UART0_IRQChannel */  
  EIC_IRQInitStructure.EIC_IRQChannel = UART0_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
  
  /* Configure and enable UART2_IRQChannel */
  EIC_IRQInitStructure.EIC_IRQChannel = UART2_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 2;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
  
  /* Enable the interrupt controller */
  EIC_IRQCmd(ENABLE);
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
