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
u8 SSP0_Buffer_Tx[32] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                         0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
                         0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
                         0x1C, 0x1D, 0x1E, 0x1F, 0x20};
u8 SSP1_Buffer_Tx[32] = {0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
                         0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62,
                         0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
                         0x6C, 0x6D, 0x6E, 0x6F, 0x70};

u8 SSP0_Buffer_Rx[32], SSP1_Buffer_Rx[32];
u8 Tx_Idx = 0, Rx_Idx = 0, k = 0;
SSP_InitTypeDef   SSP_InitStructure;
TestStatus TransferStatus1, TransferStatus2;
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

  /* MRCC configuration */
  MRCC_Configuration();
  
  /* GPIO configuration */
  GPIO_Configuration();

  /* SSP0 configuration */ 
  SSP_InitStructure.SSP_FrameFormat = SSP_FrameFormat_Motorola;
  SSP_InitStructure.SSP_Mode = SSP_Mode_Master;
  SSP_InitStructure.SSP_CPOL = SSP_CPOL_High;
  SSP_InitStructure.SSP_CPHA = SSP_CPHA_2Edge;
  SSP_InitStructure.SSP_DataSize = SSP_DataSize_8b;
  SSP_InitStructure.SSP_NSS = SSP_NSS_Soft;
  SSP_InitStructure.SSP_ClockRate = 2;
  SSP_InitStructure.SSP_ClockPrescaler = 4; /* SSP baud rate : 30 MHz/(4*(2+1))= 2.5MHz */
  SSP_Init(SSP0, &SSP_InitStructure);

  /* SSP1 configuration */ 
  SSP_InitStructure.SSP_Mode = SSP_Mode_Slave;
  SSP_InitStructure.SSP_SlaveOutput = SSP_SlaveOutput_Enable;
  SSP_Init(SSP1, &SSP_InitStructure);

  /* SSP0 enable */
  SSP_Cmd(SSP0, ENABLE);
  
  /* SSP1 enable */
  SSP_Cmd(SSP1, ENABLE);

  /* Master to slave transfer procedure */
  while(Tx_Idx < 32)
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
  for(k = 0; k < 8; k++) SSP0_Buffer_Rx[k] = SSP_ReceiveData(SSP0);

  /* Reset counters */
  Tx_Idx = Rx_Idx = 0;

  /* Slave to master transfer procedure */       
  while(Tx_Idx < 32)
  {   
    SSP_SendData(SSP1, SSP1_Buffer_Tx[Tx_Idx]);
    
    /* send a dummy bit to generate the clock */
    SSP_SendData(SSP0, SSP0_Buffer_Tx[Tx_Idx++]); 
    
    while(SSP_GetFlagStatus(SSP0, SSP_FLAG_RxFifoNotEmpty)==RESET);
    SSP0_Buffer_Rx[Rx_Idx++] = SSP_ReceiveData(SSP0);
  }

  /* Check the received data with the send ones */
  TransferStatus2 = Buffercmp(SSP1_Buffer_Tx, SSP0_Buffer_Rx, 32);
  /* TransferStatus = PASSED, if the data transmitted from SSP1 and  
     received by SSP0 are the same */
  /* TransferStatus = FAILED, if the data transmitted from SSP1 and 
     received by SSP0 are different */

  while(1);
}

/*******************************************************************************
* Function Name  : MRCC_Configuration
* Description    : Configure the different system clocks.
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
* Description    : Configure the used I/O ports pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
 
  /* SSP0 and SSP1 pins Config */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
                                 GPIO_Pin_16 | GPIO_Pin_17 | GPIO_Pin_18;

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
