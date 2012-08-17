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
#define SSP0_DR_Address   0xFFFFB808

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DMA_InitTypeDef   DMA_InitStructure;
SSP_InitTypeDef   SSP_InitStructure;
u16 SRC_Buffer[64] = {0x5100,0x5200,0x5300,0x5400,0x5500,0x5600,0x5700,0x5800,
                      0x5900,0x5A00,0x5B00,0x5C00,0x5D00,0x5E00,0x5F00,0x6000,
                      0x6100,0x6200,0x6300,0x6400,0x6500,0x6600,0x6700,0x6800,
                      0x6900,0x6A00,0x6B00,0x6C00,0x6D00,0x6E00,0x6F00,0x7000,
                      0x7100,0x7200,0x7300,0x7400,0x7500,0x7600,0x7700,0x7800,
                      0x7900,0x7A00,0x7B00,0x7C00,0x7D00,0x7E00,0x7F00,0x8000,
                      0x8100,0x8200,0x8300,0x8400,0x8500,0x8600,0x8700,0x8800,
                      0x8900,0x8A00,0x8B00,0x8C00,0x8D00,0x8E00,0x8F00,0x9000};
u16 SSP1_Buffer_Rx[64];
vu8 Rx_Idx=0;
TestStatus TransferStatus;
ErrorStatus OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void MRCC_Configuration(void);
void EIC_Configuration(void);
TestStatus Buffercmp(u16* pBuffer1, u16* pBuffer2, u16 BufferLength);

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

  /* MRCC configuration */
  MRCC_Configuration();
  
  /* GPIO pins configuration */
  GPIO_Configuration();

  /* EIC configuration */
  EIC_Configuration(); 
    
/* SSP configuration ---------------------------------------------------------*/ 
  /* SSP0 configuration */ 
  SSP_StructInit(&SSP_InitStructure);
  SSP_InitStructure.SSP_FrameFormat = SSP_FrameFormat_Motorola;
  SSP_InitStructure.SSP_Mode = SSP_Mode_Master;
  SSP_InitStructure.SSP_CPOL = SSP_CPOL_High;
  SSP_InitStructure.SSP_CPHA = SSP_CPHA_2Edge;
  SSP_InitStructure.SSP_DataSize = SSP_DataSize_16b;
  SSP_InitStructure.SSP_NSS = SSP_NSS_Soft;
  SSP_InitStructure.SSP_ClockRate = 5;
  SSP_InitStructure.SSP_ClockPrescaler = 3; /* SSP baud rate : 30 MHz/2*(5+1) = 2.5MHz */
  SSP_Init(SSP0, &SSP_InitStructure);
  
  /* SSP1 configuration */ 
  SSP_InitStructure.SSP_Mode = SSP_Mode_Slave;
  SSP_InitStructure.SSP_SlaveOutput = SSP_SlaveOutput_Enable;
  SSP_Init(SSP1, &SSP_InitStructure);

  /* Configure the SSP0 DMA receive transfer as burst */
  SSP_DMATxConfig(SSP0_DMATxReq_Burst);

  /* Enable SSP DMA transmit */
  SSP_DMACmd(SSP0_DMA_Transmit, ENABLE);

  /* Enable SSP1 receive interrupt: this interrupt is generated when the SSP1 
     receive FIFO hold four data */
  SSP_ITConfig(SSP1, SSP_IT_RxFifo, ENABLE);

  /* SSP1 enable */
  SSP_Cmd(SSP1, ENABLE);

  /* SSP0 enable */
  SSP_Cmd(SSP0, ENABLE);

/* DMA configuration ---------------------------------------------------------*/ 
  DMA_InitStructure.DMA_BufferSize = 64;
  DMA_InitStructure.DMA_SRCBaseAddr = (u32)SRC_Buffer;
  DMA_InitStructure.DMA_DSTBaseAddr = SSP0_DR_Address;
  DMA_InitStructure.DMA_SRC = DMA_SRC_INCR;
  DMA_InitStructure.DMA_DST = DMA_DST_NOT_INCR;
  DMA_InitStructure.DMA_SRCSize = DMA_SRCSize_HalfWord;
  DMA_InitStructure.DMA_SRCBurst = DMA_SRCBurst_4Data;
  DMA_InitStructure.DMA_DSTSize = DMA_DSTSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeriphDST;
  DMA_Init(DMA_Stream1, &DMA_InitStructure);

  /* DMA end of stream1 transfer interrupt enable */
  DMA_ITConfig(DMA_IT_SI1, ENABLE);

  /* Enable DMA Stream1 transfer Memory->SSP0->SSP1 */
  DMA_Cmd(DMA_Stream1, ENABLE);

  /* Wait till all data transmitted by SSP0 are received by SSP1 */
  while(Rx_Idx<64);

  /* Check the received data with the send ones */
  TransferStatus = Buffercmp(SRC_Buffer, SSP1_Buffer_Rx, 64);
  /* TransferStatus = PASSED, if the transmitted from SSP0 and the 
     received data by SSP1 are the same */
  /* TransferStatus = FAILED, if the transmitted from SSP0 and the 
     received data by SSP are different */

  /* Wait for the end of Stream1 transfer */
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
    /* Set HCLK to 60MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div1);

    /* Set CKTIM to 30MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div2);

    /* Set PCLK to 30MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div1);

    /* Enable Flash burst mode */
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);
  
    /* Set CKSYS to 60MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);  

  /* GPIO, SSP0 and SSP1 clock source enable */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_SSP0 |
                             MRCC_Peripheral_SSP1, ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the used I/O ports pins
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
 
  /* SSP0 and SSP1 pins Config */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
                                GPIO_Pin_16 | GPIO_Pin_17 | GPIO_Pin_18 ;
  GPIO_Init(GPIO0, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : EIC_Configuration
* Description    : Configures the used EIC interrupts requests
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_Configuration(void)
{
  EIC_IRQInitTypeDef  EIC_IRQInitStructure;

  /* DMA IRQ Channel Config */
  EIC_IRQInitStructure.EIC_IRQChannel = DMA_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  EIC_IRQInitStructure.EIC_IRQChannel = SSP1_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 2;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable the IRQ Interrupt controller */
  EIC_IRQCmd(ENABLE);
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
TestStatus Buffercmp(u16* pBuffer1, u16* pBuffer2, u16 BufferLength)
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
