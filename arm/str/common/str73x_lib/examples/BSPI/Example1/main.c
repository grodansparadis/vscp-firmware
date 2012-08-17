/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This program provides a basic example of how to use the
*                      BSPI software library.
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH  CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

/* Standard include ------------------------------------------------------------*/
#include "main.h"

/* Include of other module interface headers -----------------------------------*/
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
#define BSPI0_RXR_Base  0xFFFFD800
#define BufferSize   (countof(Tx_Buffer)-1)

/* Private macro ---------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables -----------------------------------------------------------*/
GPIO_InitTypeDef   GPIO_InitStructure;
BSPI_InitTypeDef   BSPI_InitStructure;
PRCCU_InitTypeDef  PRCCU_InitStructure;
CMU_InitTypeDef    CMU_InitStructure;
DMA_InitTypeDef    DMA_InitStructure;

u8 Tx_Buffer[ ] = "/* STR73x BSPI Software Library Example... */";
u8 Rx_Buffer[BufferSize];
u8 NbrDataToTransfer = BufferSize;

Status  TransferStatus;

/* Private function prototypes -------------------------------------------------*/
/* Interface functions ---------------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main
* Description    : main function, where program start. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main()
{

#ifdef DEBUG
    debug();
#endif

/* set the system clock to 32MHz, using external 8MHz quartz -----------------*/
  /* select quartz oscillator as PRCCU clock input */
  CMU_StructInit(&CMU_InitStructure);
  CMU_InitStructure.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;  
  CMU_Init(&CMU_InitStructure);
  
  /* set MCLK clock to 32 MHz */
  PRCCU_InitStructure.PRCCU_DIV2 = ENABLE;
  PRCCU_InitStructure.PRCCU_MCLKSRC_SRC = PRCCU_MCLKSRC_PLL ;
  PRCCU_InitStructure.PRCCU_PLLDIV = 2 ;
  PRCCU_InitStructure.PRCCU_PLLMUL = PRCCU_PLLMUL_16;
  PRCCU_Init(&PRCCU_InitStructure);

/* Peripheral clock configuration --------------------------------------------*/
  /* enable GPIO 4, 5 and 6 clock */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4, ENABLE);
  CFG_PeripheralClockConfig(CFG_CLK_GPIO5, ENABLE);
  CFG_PeripheralClockConfig(CFG_CLK_GPIO6, ENABLE);

  /* enable BSPI 0, 1 clock */
  CFG_PeripheralClockConfig(CFG_CLK_BSPI0, ENABLE);
  CFG_PeripheralClockConfig(CFG_CLK_BSPI1, ENABLE);

  /* enable DMA0 clock */
  CFG_PeripheralClockConfig(CFG_CLK_DMA0, ENABLE);

  /* enable EIC clock */
  CFG_PeripheralClockConfig(CFG_CLK_EIC, ENABLE);

/* GPIO pins configuration ---------------------------------------------------*/
  /* BSPI0 pins configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
                                 GPIO_PIN_14;
  GPIO_Init(GPIO6, &GPIO_InitStructure);

  /* BSPI1 pins configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_Init(GPIO5, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_15;
  GPIO_Init(GPIO4, &GPIO_InitStructure);

  
/* BSPI1 configuration -------------------------------------------------------*/
  BSPI_InitStructure.BSPI_RxFifoSize = 16;
  BSPI_InitStructure.BSPI_TxFifoSize = 16;
  BSPI_InitStructure.BSPI_SSPin = BSPI_SSPin_Masked;
  BSPI_InitStructure.BSPI_CLKDivider = 6; /* BSPI clock frequency : 5.3Mb/s (32MHz/6) */
  BSPI_InitStructure.BSPI_CPOL = BSPI_CPOL_High;
  BSPI_InitStructure.BSPI_CPHA = BSPI_CPHA_1Edge;
  BSPI_InitStructure.BSPI_WordLength = BSPI_WordLength_8b;
  BSPI_InitStructure.BSPI_Mode = BSPI_Mode_Master;

  BSPI_Init(BSPI1, &BSPI_InitStructure);

  /* enable BSPI1 Transmit FIFO Empty(TFE) interrupt*/
  BSPI_TxITConfig(BSPI1, BSPI_TxIT_TFE);

/* BSPI0 configuration -------------------------------------------------------*/ 
  BSPI_InitStructure.BSPI_Mode = BSPI_Mode_Slave;
  BSPI_Init(BSPI0, &BSPI_InitStructure);
  
  /* BSPI0 DMA interface configuration */
  BSPI_DMAConfig(BSPI0, BSPI_DMAReq_Rx, ENABLE);
  BSPI_DMARxBurstConfig(BSPI0, BSPI_DMA_RxBurst_1Word);
  BSPI_DMACmd(BSPI0, ENABLE);
   
/* DMA0 stream2 configuration ------------------------------------------------*/
  DMA_InitStructure.DMA_Stream = DMA_Stream2;
  DMA_InitStructure.DMA_BufferSize = NbrDataToTransfer;
  DMA_InitStructure.DMA_SRC = DMA_SRC_NOT_INCR;
  DMA_InitStructure.DMA_DST = DMA_DST_INCR;
  DMA_InitStructure.DMA_SRCSize = DMA_SRCSize_Byte;
  DMA_InitStructure.DMA_SRCBurst = DMA_SRCBurst_1Word;
  DMA_InitStructure.DMA_DSTSize = DMA_DSTSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStructure.DMA_Dir = DMA_Dir_PeriphSRC;
  DMA_InitStructure.DMA_SRCBaseAddr = BSPI0_RXR_Base;
  DMA_InitStructure.DMA_DSTBaseAddr = (u32)&Rx_Buffer;
  DMA_InitStructure.DMA_TriggeringSource = DMA_TriggeringSource_BSPI0;

  DMA_Init(DMA0, &DMA_InitStructure);

  /* enable DMA0 Stream2 transfer end interrupt(SI2) */
  DMA_ITConfig(DMA0, DMA_IT_SI2, ENABLE);
 
  /* set the DMA priority to highest on AHB bus */
  DMA_AHBArbitrationConfig(DMA_Priority_High);
  
/* EIC configuration ----------------------------------------------------------*/
  /* enable and configure the priority of the BSPI1 IRQ Channel*/
  EIC_IRQChannelConfig(BSPI1_IRQChannel, ENABLE);
  EIC_IRQChannelPriorityConfig(BSPI1_IRQChannel, 1);

  /* enable and configure the priority of the DMA0 IRQ Channel*/
  EIC_IRQChannelConfig(DMA0_IRQChannel, ENABLE);
  EIC_IRQChannelPriorityConfig(DMA0_IRQChannel, 2);

  /* enable the Interrupt controller to manage IRQ channel*/
  EIC_IRQCmd(ENABLE);

/* Start data transfer ---------------------------------------------------------*/
  /* enable BSPI0 */
  BSPI_Cmd(BSPI0, ENABLE);

  /* enable DMA0 stream2 */
  DMA_Cmd(DMA0, DMA_Stream2, ENABLE);

  /* enable BSPI1 */
  BSPI_Cmd(BSPI1, ENABLE);


  while(1);
}

/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers.
* Input          : - pBuffer, pBuffer1: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : PASS: pBuffer identical to pBuffer1
*                  FAIL: pBuffer differs from pBuffer1
*******************************************************************************/
Status Buffercmp(u8* pBuffer, u8* pBuffer1, u16 BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer != *pBuffer1)
    {
      return FAIL;
    }
    
    pBuffer++;
    pBuffer1++;
  }
  return PASS;  
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/



