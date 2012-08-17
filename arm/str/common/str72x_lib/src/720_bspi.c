/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_bspi.c
* Author             : MCD Application Team
* Date First Issued  : 06/14/2004
* Description        : This file provides all the BSPI software functions
**********************************************************************************
* History:
* 12/20/2004 :  V1.1
* 08/02/2004 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH  CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

#include "720_bspi.h"

/*******************************************************************************
* Function Name  : BSPI_Init
* Description    : Initializes BSPI peripheral control and registers to their 
*                : default reset values.
* Input          : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Output         : BSPI peripheral registers and BSPI global variables are 
*                : initialized.
* Return         : None.
*******************************************************************************/
void BSPI_Init(BSPI_TypeDef *BSPIx)
{
  BSPIx->CSR2 = 0x41;
  BSPIx->CSR1 = 0x00;
  BSPIx->CLK  = 0x06;
  BSPIx->CSR3 = 0x00;
}
/*******************************************************************************
* Function Name  : BSPI_TxITConfig.
* Description    : Configures the transmit interrupt source.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : TxIT: specifies the transmit interrupt source.
*		 : Refer to the section ‘Transmit Interrupt Configuration’ for 
*                : more details on the allowed values of this parameter.
* Output         : TIE bit in BSPCSR2 register.
* Return         : None.
*******************************************************************************/
void BSPI_TxITConfig(BSPI_TypeDef *BSPIx, BSPITxIT_TypeDef TxIT)
{
  switch (TxIT)
    {
      case BSPI_TxITDIS:
        BSPIx->CSR2 &=~BSPI_TxITFF;
      break;
      case BSPI_TxITFE:
	BSPIx->CSR2 &=~BSPI_TxITUFL;
	BSPIx->CSR2 |= BSPI_TxITFE;
      break;
      case BSPI_TxITUFL:
	BSPIx->CSR2 &=~BSPI_TxITFE;
	BSPIx->CSR2 |= BSPI_TxITUFL;
      break;
      case BSPI_TxITFF:
        BSPIx->CSR2 |= BSPI_TxITFF;
      break;
   }
}
/*******************************************************************************
* Function Name  : BSPI_RxITConfig.
* Description    : Configures the receive interrupt source.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : RxIT: specifies the source for the receive interrupt.
*		 : Refer to the section ‘Receive Interrupt Configuration’ for
*                : more details on the allowed values of this parameter.
* Output         : RIE bit in BSPCSR1 is register.
* Return         : None.
*******************************************************************************/
void BSPI_RxITConfig(BSPI_TypeDef *BSPIx, BSPIRxIT_TypeDef RxIT)
{
  switch (RxIT)
    {
       case BSPI_RxITDIS:
         BSPIx->CSR1 &= ~BSPI_RxITFF;
       break;
       case BSPI_RxITFNE:
	 BSPIx->CSR1 &= ~0x0008;
	 BSPIx->CSR1 |= BSPI_RxITFNE;
       break;
       case BSPI_RxITFF:
         BSPIx->CSR1 |= BSPI_RxITFF;
       break;
    }
}

/*******************************************************************************
* Function Name  : BSPI_TxFifoConfig
* Description    : Configures BSPI transmission FIFO number of words.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI.
* Input 2        : TxDepth:specifies the depth of the transmit FIFO.
* Output         : TFE bit in BSPCSR2 register.
* Return         : None.
*******************************************************************************/
void BSPI_TxFifoConfig(BSPI_TypeDef *BSPIx, vu8 TxDepth)
{
  if (TxDepth > 0 && TxDepth < 17)
    {
      TxDepth--;
      BSPIx->CSR2 &= (TxDepth*0x400) | ~0x3C00;
      BSPIx->CSR2 |= TxDepth*0x400;
    }
  else BSPIx->CSR2 &= ~0x3C00;
}

/*******************************************************************************
* Function Name  : BSPI_RxFifoDepth
* Description    : Configures BSPI reception FIFO number of words.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : RxDepth:specifies the depth of the receive FIFO.
* Output         : RFE bits [3:0] in BSPCSR1 register.
* Return         : None.
*******************************************************************************/
void BSPI_RxFifoConfig(BSPI_TypeDef *BSPIx, vu8 RxDepth)
{
  if (RxDepth > 0 && RxDepth < 17)
    {
      RxDepth--;
      BSPIx->CSR1 &= (RxDepth*0x1000) | ~0xF000;
      BSPIx->CSR1 |= RxDepth*0x1000;
    }
  else BSPIx->CSR1 &= ~0xF000;
}
/*******************************************************************************
* Function Name  : BSPI_BufferSend.
* Description    : Transmits data from a buffer.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : PtrToBuffer is an ‘u16’ pointer to the first word of the
*                : buffer to be transmitted.
* Input 3        : NbOfWords parameter indicates the number of words saved in
*                : the buffer to be sent.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  BSPI_BufferSend(BSPI_TypeDef *BSPIx, vu16 *PtrToBuffer, vu8 NbOfWords)
{
  vu8 SendWord = 0;
  while (SendWord < NbOfWords)
    {
      BSPI_WordSend(BSPIx, *(PtrToBuffer+SendWord));
      SendWord++;
    }
}
/*******************************************************************************
* Function Name  : BSPI_BufferReceive.
* Description    : Receives number of data words and stores them in user defined
*                : area.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : PtrToBuffer is an ‘u16’ pointer to the first word of the
*                : defined area to save the received buffer.
* Input 3        : NbOfWords parameter indicates the number of words to be
*                : received in the buffer.
* Output         : None.
* Return         : None.
*******************************************************************************/
void BSPI_BufferReceive(BSPI_TypeDef *BSPIx, vu16 *PtrToBuffer, vu8 NbOfWords)
{
  vu16 ReceiveWord = 0;
  while (ReceiveWord < NbOfWords)
    {
      *(PtrToBuffer+ReceiveWord) = BSPI_WordReceive(BSPIx);
      ReceiveWord++;
    }
}
/*******************************************************************************
* Function Name  : BSPI_RxBurstConfig.
* Description    : Configure the burst length when the BSPI receive data.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : RxBurst: specifies the burst length.
*                : Refer to the section ‘Receive Burst Length Configuration’
*                : for more details on the allowed values of this parameter.
* Output         : RBUSRT_LEN [1:0 ] bits in BSPCSR3 register.
* Return         : None.
*******************************************************************************/
void BSPI_RxBurstConfig(BSPI_TypeDef *BSPIx, BSPIRxBurst_TypeDef RxBurst)
{
  switch (RxBurst)
    {
      case BSPI_Rx1Word:
        BSPIx->CSR3 &= BSPI_Rx1Word;
      break;
      case BSPI_Rx4Word:
      	BSPIx->CSR3 &=BSPI_Rx1Word;
	BSPIx->CSR3 |=BSPI_Rx4Word ;
      break;
      case BSPI_Rx8Word:
	BSPIx->CSR3 &=BSPI_Rx1Word;
	BSPIx->CSR3 |=BSPI_Rx8Word;
      break;
      case BSPI_Rx16Word:
        BSPIx->CSR3 |= BSPI_Rx16Word;
      break;
  }
}
/*******************************************************************************
* Function Name  : BSPI_TxBurstConfig.
* Description    : Configure the burst length when the BSPI transmit data.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : TxBurst: specifies the burst length.
*                : Refer to the section ‘Transmit Burst Length Configuration’
*                : for more details on the allowed values of this parameter.
* Output         : TBUSRT_LEN [1:0 ] bits in BSPCSR3 register.
* Return         : None.
*******************************************************************************/
void BSPI_TxBurstConfig(BSPI_TypeDef *BSPIx, BSPITxBurst_TypeDef TxBurst)
{
  switch (TxBurst)
    {
      case BSPI_Tx1Word:
        BSPIx->CSR3 &= BSPI_Tx1Word;
      break;
      case BSPI_Tx4Word:
        BSPIx->CSR3 &=BSPI_Tx1Word;
        BSPIx->CSR3 |=BSPI_Tx4Word ;
      break;
      case BSPI_Tx8Word:
        BSPIx->CSR3 &=BSPI_Tx1Word;
	BSPIx->CSR3 |=BSPI_Tx8Word ;
      break;
      case BSPI_Tx16Word:
        BSPIx->CSR3 |= BSPI_Tx16Word;
      break;
   }
}

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/



