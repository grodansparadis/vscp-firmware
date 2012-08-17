/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This program provides a basic example of how to use the
*                      DMA software library.
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
* CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

/* Standard include ----------------------------------------------------------*/
#include "main.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BufferSize  512

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DMA_InitTypeDef DMA_InitStructure;
u16 Index;
u16 NbrDataToTransfer = BufferSize;
u32 SRC_Buffer[BufferSize];
u32 DST_Buffer[BufferSize];
Status  TransferStatus;

/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
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

  /* fill in the SRC_Buffer */
  for(Index = 0; Index < NbrDataToTransfer; Index++)
  {
    SRC_Buffer[Index] = Index ; 
  }

/* Peripheral clock configuration --------------------------------------------*/
  /* enable DMA3 clock */
  CFG_PeripheralClockConfig(CFG_CLK_DMA3, ENABLE);

  /* enable EIC clock */
  CFG_PeripheralClockConfig(CFG_CLK_EIC, ENABLE);

/* DMA controller onfiguration -----------------------------------------------*/
  DMA_InitStructure.DMA_Stream = DMA_Stream3;
  DMA_InitStructure.DMA_BufferSize = NbrDataToTransfer;
  DMA_InitStructure.DMA_SRC = DMA_SRC_INCR;   
  DMA_InitStructure.DMA_DST = DMA_DST_INCR; 
  DMA_InitStructure.DMA_SRCSize = DMA_SRCSize_Word;
  DMA_InitStructure.DMA_SRCBurst = DMA_SRCBurst_1Word;
  DMA_InitStructure.DMA_DSTSize = DMA_DSTSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable; 
  DMA_InitStructure.DMA_Dir = DMA_Dir_PeriphSRC; 
  DMA_InitStructure.DMA_SRCBaseAddr = (u32)SRC_Buffer;
  DMA_InitStructure.DMA_DSTBaseAddr = (u32)DST_Buffer;

  DMA_Init(DMA3, &DMA_InitStructure);

  /* enable DMA3 stream3 transfer end interrupt(SI3) */
  DMA_ITConfig(DMA3, DMA_IT_SI3, ENABLE);

  /* set the DMA priority to highest on AHB bus */ 
  DMA_AHBArbitrationConfig(DMA_Priority_High);

  /* enable and configure the priority of the DMA3 Channel*/
  EIC_IRQChannelConfig(DMA3_IRQChannel, ENABLE);
  EIC_IRQChannelPriorityConfig(DMA3_IRQChannel, 1);

  /* enable the Interrupt controller to manage IRQ channel*/
  EIC_IRQCmd(ENABLE);

  /* enable DMA3 stream3 */
  DMA_Cmd(DMA3, DMA_Stream3, ENABLE);

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
Status Buffercmp(u32* pBuffer, u32* pBuffer1, u16 BufferLength)
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
