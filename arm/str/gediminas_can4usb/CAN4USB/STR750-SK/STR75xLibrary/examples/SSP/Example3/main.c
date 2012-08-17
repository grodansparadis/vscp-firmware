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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u16 SSP0_Buffer_Tx[32] = {0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000,
                          0x8000, 0x9000, 0xA000, 0xB000, 0xC000, 0xD000, 0xE000,
                          0xF000, 0x1000, 0x1100, 0x1200, 0x1300, 0x1400, 0x1500,
                          0x1600, 0x1700, 0x1800, 0x1900, 0x1A00, 0x1B00, 0x1C00,
                          0x1D00, 0x1E00, 0x1F00, 0x2000};
u16 SSP1_Buffer_Tx[32] = {0x5100, 0x5200, 0x5300, 0x5400, 0x5500, 0x5600, 0x5700,
                          0x5800, 0x5900, 0x5A00, 0x5B00, 0x5C00, 0x5D00, 0x5E00,
                          0x5F00, 0x6000, 0x6100, 0x6200, 0x6300, 0x6400, 0x6500,
                          0x6600, 0x6700, 0x6800, 0x6900, 0x6A00, 0x6B00, 0x6C00,
                          0x6D00, 0x6E00, 0x6F00, 0x7000};
u16 SSP0_Buffer_Rx[32], SSP1_Buffer_Rx[32];
u8 Tx_Idx=0, Rx_Idx=0;
u8 k=0;
SSP_InitTypeDef   SSP_InitStructure;
TestStatus TransferStatus1, TransferStatus2;
ErrorStatus OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void MRCC_Configuration(void);
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

  /* SSP0 Master configuration */
  SSP_InitStructure.SSP_FrameFormat = SSP_FrameFormat_Motorola;
  SSP_InitStructure.SSP_Mode = SSP_Mode_Master;
  SSP_InitStructure.SSP_CPOL = SSP_CPOL_High;
  SSP_InitStructure.SSP_CPHA = SSP_CPHA_2Edge;
  SSP_InitStructure.SSP_DataSize = SSP_DataSize_16b;
  SSP_InitStructure.SSP_NSS = SSP_NSS_Soft;
  SSP_InitStructure.SSP_ClockRate = 5;
  SSP_InitStructure.SSP_ClockPrescaler = 2; /* SSP baud rate : 30 MHz/2*(5+1) = 2.5MHz */
  SSP_Init(SSP0, &SSP_InitStructure);

  /* SSP1 Slave configuration */
  SSP_InitStructure.SSP_Mode = SSP_Mode_Slave;
  SSP_InitStructure.SSP_SlaveOutput = SSP_SlaveOutput_Enable;
  SSP_Init(SSP1, &SSP_InitStructure);

  /* SSP0 enable */
  SSP_Cmd(SSP0, ENABLE);
  
  /* SSP1 enable */
  SSP_Cmd(SSP1, ENABLE);

  /* Transfer procedure */
  while(Tx_Idx<32)
  {
    SSP_SendData(SSP0, SSP0_Buffer_Tx[Tx_Idx++]);
    while(SSP_GetFlagStatus(SSP1, SSP_FLAG_RxFifoNotEmpty)==RESET);
    SSP1_Buffer_Rx[Rx_Idx++] = SSP_ReceiveData(SSP1);
  }

  /* Check the received data with the send ones */
  TransferStatus1 = Buffercmp(SSP0_Buffer_Tx, SSP1_Buffer_Rx, 32);
  /* TransferStatus = PASSED, if the data transmitted from SSP0 and
     received by SSP1 are the same */
  /* TransferStatus = FAILED, if the data transmitted from SSP0 and
     received by SSP1 are different */

  /* Clear SSP0 receive Fifo */
  for(k=0; k<8; k++) SSP0_Buffer_Rx[k] = SSP_ReceiveData(SSP0);

  /* SSP1 Master configuration */
  SSP_InitStructure.SSP_Mode = SSP_Mode_Master;
  SSP_Init(SSP1, &SSP_InitStructure);

  /* SSP0 Slave configuration */
  SSP_InitStructure.SSP_Mode = SSP_Mode_Slave;
  SSP_InitStructure.SSP_SlaveOutput = SSP_SlaveOutput_Enable;
  SSP_Init(SSP0, &SSP_InitStructure);

  /* Reset counters */
  Tx_Idx=Rx_Idx=0;

  /* Transfer procedure */
  while(Tx_Idx<32)
  {
    SSP_SendData(SSP1, SSP1_Buffer_Tx[Tx_Idx++]);
    while(SSP_GetFlagStatus(SSP0, SSP_FLAG_RxFifoNotEmpty)==RESET);
    SSP0_Buffer_Rx[Rx_Idx++] = SSP_ReceiveData(SSP0);
  }

  /* Check the received data with the send ones */
  TransferStatus2 = Buffercmp(SSP1_Buffer_Tx, SSP0_Buffer_Rx, 32);
  /* TransferStatus = PASSED, if the transmitted data from SSP1 and
     received by SSP0 are the same */
  /* TransferStatus = FAILED, if the transmitted data from SSP1 and
     received by SSP0 are different */

  while(1);
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
* Description    : Configures the used I/O ports pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* SSP0 and SSP1 pins Config */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7 |
                                GPIO_Pin_16 | GPIO_Pin_17 | GPIO_Pin_18 ;
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
