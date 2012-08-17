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
#define UART0_DR_Base   0xFFFFD400

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DMA_InitTypeDef   DMA_InitStructure;
UART_InitTypeDef  UART_InitStructure;
u8 DST_Buffer[4];
ErrorStatus OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void MRCC_Configuration(void);
void GPIO_Configuration(void);
void EIC_Configuration(void);
void PinToggle(void);

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

  /* Configure the different system clocks */
  MRCC_Configuration();

  /* Configure the used I/O ports pins */
  GPIO_Configuration();

  /* Configure the interrupt controller */
  EIC_Configuration();

/* UART and DMA configuration ------------------------------------------------*/
  /* UART0 configured as follow:
        - Word Length = 8 Bits
        - One Stop Bit
        - Odd parity
        - BaudRate = 9600 baud
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - Receive and transmit FIFOs are enabled
        - Transmit FIFO level have 8 bytes depth 
        - Receive FIFO level have 4 bytes depth 
  */
  UART_InitStructure.UART_WordLength = UART_WordLength_8D;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_Odd ;
  UART_InitStructure.UART_BaudRate = 9600;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
  UART_InitStructure.UART_Mode = UART_Mode_Tx_Rx;
  UART_InitStructure.UART_FIFO = UART_FIFO_Enable;
  UART_InitStructure.UART_TxFIFOLevel = UART_FIFOLevel_1_2; /* FIFO size 16 bytes, FIFO level 8 bytes */
  UART_InitStructure.UART_RxFIFOLevel = UART_FIFOLevel_1_4; /* FIFO size 16 bytes, FIFO level 4 bytes */

  UART_Init(UART0, &UART_InitStructure);

  /* Enable UART DMA burst transfer */
  UART_DMAConfig(UART0_DMATransfer_Burst, UART0_DMAOnError_Disable);

  /* Enable UART DMA Rx request */
  UART_DMACmd(UART0_DMAReq_Rx, ENABLE);

  /* Configure DMA stream 2 */
  DMA_InitStructure.DMA_BufferSize = 4;
  DMA_InitStructure.DMA_SRCBaseAddr = (u32) UART0_DR_Base;
  DMA_InitStructure.DMA_DSTBaseAddr = (u32) DST_Buffer;
  DMA_InitStructure.DMA_SRC = DMA_SRC_NOT_INCR;
  DMA_InitStructure.DMA_DST = DMA_DST_INCR;
  DMA_InitStructure.DMA_SRCSize = DMA_SRCSize_Byte;
  DMA_InitStructure.DMA_SRCBurst = DMA_SRCBurst_4Data;
  DMA_InitStructure.DMA_DSTSize = DMA_DSTSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeriphSRC;
  DMA_Init(DMA_Stream2, &DMA_InitStructure);

  /* Enable DMA Stream2 End of transfer interrupt */
  DMA_ITConfig(DMA_IT_SI2, ENABLE);

  /* Enable UART0 */
  UART_Cmd(UART0, ENABLE);

  /* Enable DMA Stream2 transfer */
  DMA_Cmd(DMA_Stream2, ENABLE);

  while(1)
  {
    /* Toggle led connected to P2.3 pin */
    PinToggle();

    /* Enter WFI mode with FLASH on(DMA allowed during WFI); the system will
        wake-up by the DMA Stream2 End of transfer interrupt */
    MRCC_EnterWFIMode(MRCC_WFIParam_FLASHOn);
    
    /* Verify that the system has resumed from WFI mode (LP_DONE set to '1') */
    if(MRCC_GetFlagStatus(MRCC_FLAG_LPDONE) != RESET)
    {/* The system entered and resumed from WFI mode */
              
      /* Turn off the led connected to P2.4 pin */
      GPIO_WriteBit(GPIO2, GPIO_Pin_4, Bit_RESET);

      /* Clear LP_DONE flag */
      MRCC_ClearFlag(MRCC_FLAG_LPDONE);               
    }
    else
    {/* The system fails to enter in WFI mode */
        
      /* Turn on the led connected to P2.4 pin */
      GPIO_WriteBit(GPIO2, GPIO_Pin_4, Bit_SET);        
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
    /* Set HCLK to 60MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div1);

    /* Set CKTIM to 30MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div2);

    /* Set PCLK to 30MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div1);

    /* Enable Flash burst mode */
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);

    /* Set CK_SYS to 60 MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);  
  
  /* Enable UART0 and GPIO clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_UART0 | MRCC_Peripheral_GPIO, ENABLE);
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
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Configure P2.3 and P2.4 as Output push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_Init(GPIO2, &GPIO_InitStructure);
  
  /* Turn off leds connected to P2.3 and P2.4 pins */
  GPIO_Write(GPIO2, 0x00);;

  /* Configure P0.01 as alternate function (MCO) */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* Output HCLK clock on MCO pin (P0.01) */
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
  EIC_IRQInitTypeDef  EIC_IRQInitStructure;

  /* Configure and enable DMA_IRQChannel */
  EIC_IRQInitStructure.EIC_IRQChannel = DMA_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable EIC IRQ output request to CPU */
  EIC_IRQCmd(ENABLE);
}

/*******************************************************************************
* Function Name  : PinToggle
* Description    : Toggles P2.3 pin.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PinToggle(void)
{
  u32 i = 0, j = 0;

  for(i = 10; i != 0; i--)
  {
    for(j = 0x5FFFFF; j != 0; j--);

    /* Toggle the led connected to P2.3 pin */
    GPIO_WriteBit(GPIO2, GPIO_Pin_3, (BitAction)(1-GPIO_ReadBit(GPIO2, GPIO_Pin_3)));
  }
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


