/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_bspi.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the BSPI software functions.
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
#include "73x_bspi.h"
#include "73x_cfg.h"


/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : BSPI_DeInit
* Description    : Deinitializes the BSPIx peripheral registers to their default
*                  reset values.
* Input          : BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_DeInit(BSPI_TypeDef* BSPIx)
{
 
  if( BSPIx == BSPI0)
  {
   CFG_PeripheralClockConfig(CFG_CLK_BSPI0,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_BSPI0,ENABLE);
  }
  else if  ( BSPIx == BSPI1)
  {
  
   CFG_PeripheralClockConfig(CFG_CLK_BSPI1,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_BSPI1,ENABLE);
  }
  else if  ( BSPIx == BSPI2)
  {
   CFG_PeripheralClockConfig(CFG_CLK_BSPI2,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_BSPI2,ENABLE);
  }
  
}

/*******************************************************************************
* Function Name  : BSPI_Init
* Description    : Initializes the BSPIx  peripheral according to the specified
*                  parameters in the BSPI_InitTypeDef structure.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - BSPI_InitStruct: pointer to a BSPI_InitTypeDef structure that
*                    contains the configuration information for the specified BSPI
*                    peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void BSPI_Init(BSPI_TypeDef* BSPIx, BSPI_InitTypeDef* BSPI_InitStruct)
{
  u8 FIFO_Size = 0;

  BSPIx->CSR1 &= BSPI_CSR1_Mask;
  BSPIx->CSR2 &= BSPI_CSR2_Mask;
  BSPIx->CSR3 &= BSPI_CSR3_Mask;

  if(BSPI_InitStruct->BSPI_Mode == BSPI_Mode_Master)
  {
    /* configure the baud rate */
    BSPIx->CLK = BSPI_InitStruct->BSPI_CLKDivider;
    
    /* configure the status of the Slave Select pin */
    BSPIx->CSR3 |= BSPI_InitStruct->BSPI_SSPin;
  }
  
  /* configure the Clock Polarity */
  BSPIx->CSR1 |= BSPI_InitStruct->BSPI_CPOL;

  /* configure the Clock Phase */
  BSPIx->CSR1 |= BSPI_InitStruct->BSPI_CPHA;

  /* configure the BSPI mode of operation */
  BSPIx->CSR1 |= BSPI_InitStruct->BSPI_Mode;

  /* configure the word length */
  BSPIx->CSR1 |= BSPI_InitStruct->BSPI_WordLength;

  /* configure the Receive FIFO size */
  if(BSPI_InitStruct->BSPI_RxFifoSize > 0 && BSPI_InitStruct->BSPI_RxFifoSize < 17)
  {
    FIFO_Size = BSPI_InitStruct->BSPI_RxFifoSize - 1;
    /* clear the RFS[3:0] bits */
    BSPIx->CSR1 &=  BSPI_RxFifoSize_Mask;
    /* set the RFS[3:0] bits according to FIFO_Size value */
    BSPIx->CSR1 |= FIFO_Size << 12;
  }
  else
  {
    BSPIx->CSR1 &= BSPI_RxFifoSize_Mask;
  }

  /* configure the Transmit FIFO size */
  if(BSPI_InitStruct->BSPI_TxFifoSize > 0 && BSPI_InitStruct->BSPI_TxFifoSize < 17)
  {
    FIFO_Size = BSPI_InitStruct->BSPI_TxFifoSize -1;
    /* clear the TFS[3:0] bits */
    BSPIx->CSR2 &=  BSPI_TxFifoSize_Mask;
    /* set the TFS[3:0] bits according to FIFO_Size value */
    BSPIx->CSR2 |= FIFO_Size << 10;
  }
  else
  {
    BSPIx->CSR2 &= BSPI_TxFifoSize_Mask;
  }
}

/*******************************************************************************
* Function Name  : BSPI_Cmd
* Description    : Enables or disables the specified BSPI peripheral.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - NewState: new state of the  BSPIx peripheral. This parameter
*                    can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_Cmd(BSPI_TypeDef* BSPIx, FunctionalState NewState)
{
  if(NewState == ENABLE)
  {
    BSPIx->CSR1 |= BSPI_Enable;
  }
  else
  {
    BSPIx->CSR1 &= BSPI_Disable;
  }
}

/*******************************************************************************
* Function Name  : BSPI_StructInit
* Description    : Fills in a BSPI_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : BSPI_InitStruct : pointer to a BSPI_InitTypeDef structure
                   which will be initialized.
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_StructInit(BSPI_InitTypeDef* BSPI_InitStruct)
{
  /* initialize the BSPI_SSPin member */
  BSPI_InitStruct->BSPI_SSPin = BSPI_SSPin_Used;

  /* initialize the BSPI_CLKDivider member */
  BSPI_InitStruct->BSPI_CLKDivider = 6;

  /* initialize the BSPI_CPOL member */
  BSPI_InitStruct->BSPI_CPOL = BSPI_CPOL_High;

  /* initialize the BSPI_CPHA member */
  BSPI_InitStruct->BSPI_CPHA = BSPI_CPHA_1Edge;

  /* initialize the BSPI_Mode member */
  BSPI_InitStruct->BSPI_Mode = BSPI_Mode_Slave;

  /* initialize the BSPI_WordLength member */
  BSPI_InitStruct->BSPI_WordLength = BSPI_WordLength_8b;
  
  /* initialize the BSPI_RxFifoSize member */
  BSPI_InitStruct->BSPI_RxFifoSize = 0;

  /* initialize the BSPI_TxFifoSize member */
  BSPI_InitStruct->BSPI_TxFifoSize = 0;
}

/*******************************************************************************
* Function Name  : BSPI_ITConfig
* Description    : Enables or disables the specified BSPI interrupts.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - BSPI_IT: specifies the BSPI interrupts sources to be enabled
*                    or disabled. This parameter can be any combination of the
*                    following values:
*                         - BSPI_IT_REI: Receive error interrupt mask
*                         - BSPI_IT_BEI: Bus error interrupt mask
*                  - NewState: new state of the specified BSPI interrupts.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_ITConfig(BSPI_TypeDef* BSPIx, u16 BSPI_IT, FunctionalState NewState)
{
  if(NewState == ENABLE)
  {
    BSPIx->CSR1 |= BSPI_IT;
  }
  else
  {
    BSPIx->CSR1 &= ~BSPI_IT;
  }
}

/*******************************************************************************
* Function Name  : BSPI_TxITConfig
* Description    : Configures the transmit interrupt source.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - BSPI_TxIT: specifies the BSPI transmit interrupt source to
*                    be enabled. This parameter can be one of the following values:
*                         - BSPI_TxIT_NONE: No interrupt
*                         - BSPI_TxIT_TFE: Transmit FIFO empty interrupt mask
*                         - BSPI_TxIT_TUFL: Transmit underflow interrupt mask
*                         - BSPI_TxIT_TFF: Transmit FIFO full interrupt mask
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_TxITConfig(BSPI_TypeDef* BSPIx, u16 BSPI_TxIT)
{
  /* clear the TIE[1:0] bits in CSR2 register */
  BSPIx->CSR2 &= BSPI_TxIT_Mask;

  /* set the TIE[1:0] bits according to BSPI_TxIT value */
  BSPIx->CSR2 |= BSPI_TxIT;
}

/*******************************************************************************
* Function Name  : BSPI_RxITConfig
* Description    : Configures the receive interrupt source.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - BSPI_RxIT: specifies the BSPI receive interrupt source to
*                    be enabled. This parameter can be one of the following values:
*                         - BSPI_RxIT_NONE: No interrupt
*                         - BSPI_RxIT_RFNE: Receive FIFO not empty interrupt mask
*                         - BSPI_RxIT_RFF: Receive FIFO full interrupt mask
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_RxITConfig(BSPI_TypeDef* BSPIx, u16 BSPI_RxIT)
{
  /* clear the RIE[1:0] bits in CSR1 register */
  BSPIx->CSR1 &= BSPI_RxIT_Mask;

  /* set the RIE[1:0] bits according to BSPI_RxIT value */
  BSPIx->CSR1 |= BSPI_RxIT;
}

/*******************************************************************************
* Function Name  : BSPI_DMAConfig
* Description    : Enables or disables the transmit and/or receive DMA request.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - BSPI_DMAReq : specifies the DMA request to be enabled or 
*                    disabled. This parameter can be any combination of the
*                    following values:
*                         - BSPI_DMAReq_Tx: DMA transmit request mask
*                         - BSPI_DMAReq_Rx: DMA receive request mask
*                  - NewState: new state of the specified DMA request.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_DMAConfig(BSPI_TypeDef* BSPIx, u8 BSPI_DMAReq, FunctionalState NewState)
{
  if(NewState == ENABLE)
  {
    BSPIx->CSR3 |= BSPI_DMAReq;
  }
  else
  {
    BSPIx->CSR3 &= ~BSPI_DMAReq;
  }
}

/*******************************************************************************
* Function Name  : BSPI_DMATxBurstConfig
* Description    : Configures the transmit burst length of the BSPIx’s DMA interface.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - BSPI_DMA_TxBurst: specifies the transmit burst length.
*                    This parameter can be one of the following values:
*                         - BSPI_DMA_TxBurst_1Word: 1 word transferred
*                         - BSPI_DMA_TxBurst_4Word: 4 words transferred
*                         - BSPI_DMA_TxBurst_8Word: 8 words transferred
*                         - BSPI_DMA_TxBurst_16Word: 16 words transferred
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_DMATxBurstConfig(BSPI_TypeDef* BSPIx, u8 BSPI_DMA_TxBurst)
{
  /* clear the TBURST_LEN[1:0] bits in CSR3 register */  
  BSPIx->CSR3 &= BSPI_DMA_TxBurst_Mask;

  /* set the TBURST_LEN[1:0] bits according to BSPI_DMA_TxBurst value */
  BSPIx->CSR3 |= BSPI_DMA_TxBurst;
}

/*******************************************************************************
* Function Name  : BSPI_DMARxBurstConfig
* Description    : Configures the receive burst length of the BSPIx’s DMA interface.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - BSPI_DMA_RxBurst: specifies the receive burst length.
*                    This parameter can be one of the following values:
*                         - BSPI_DMA_RxBurst_1Word: 1 word transferred
*                         - BSPI_DMA_RxBurst_4Word: 4 words transferred
*                         - BSPI_DMA_RxBurst_8Word: 8 words transferred
*                         - BSPI_DMA_RxBurst_16Word: 16 words transferred
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_DMARxBurstConfig(BSPI_TypeDef* BSPIx, u8 BSPI_DMA_RxBurst)
{
  /* clear the RBURST_LEN[1:0] bits in CSR3 register */
  BSPIx->CSR3 &= BSPI_DMA_RxBurst_Mask;

  /* set the RBURST_LEN[1:0] bits according to BSPI_DMA_RxBurst value */
  BSPIx->CSR3 |= BSPI_DMA_RxBurst;
}

/*******************************************************************************
* Function Name  : BSPI_DMACmd
* Description    : Enables or disables the BSPIx DMA interface.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - NewState: new state of the DMA interface. This parameter
*                    can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_DMACmd(BSPI_TypeDef* BSPIx, FunctionalState NewState)
{
  if(NewState == ENABLE)
  {
    BSPIx->CSR3 |= BSPI_DMA_Enable;
  }
  else
  {
    BSPIx->CSR3 &= BSPI_DMA_Disable;
  }
}

/*******************************************************************************
* Function Name  : BSPI_WordSend.
* Description    : Transmits a single word through the BSPI peripheral.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - Data : word to be transmitted.
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_WordSend(BSPI_TypeDef* BSPIx, u16 Data)
{
  if ((BSPIx->CSR1 & BSPI_WordLength_16b) == FALSE)
  {
    Data <<= 8;
  }

  BSPIx->TXR = Data;
}

/*******************************************************************************
* Function Name  : BSPI_BufferSend.
* Description    : Transmits data from a user defined buffer. This function will
*                  fail if an error occurs.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - PtrToBuffer : ‘u8’ pointer to the buffer that contains the
*                    data to be transmitted.
*                  - NbOfBytes : specifies the number of bytes to be transferred
*                    from the buffer.
* Output         : None
* Return         : An ErrorStatus enumuration value:
*                         - SUCCESS: transmission done without error
*                         - ERROR: an error detected during transmission
*******************************************************************************/
ErrorStatus BSPI_BufferSend(BSPI_TypeDef* BSPIx, u8* PtrToBuffer, u8 NbOfBytes)
{
  while(NbOfBytes--) /* while there is data to be transmitted */
  {
    BSPIx->TXR = (*PtrToBuffer) << 8; /* send the current byte  */
    PtrToBuffer++;  /* point to the next byte to be transferred */
 
    if(BSPI_FlagStatus(BSPIx, BSPI_FLAG_BERR) == RESET) /* if no bus error detected */
    {
      while(BSPI_FlagStatus(BSPIx, BSPI_FLAG_TFF) == SET); /* loop while Transmit
                                                             FIFO is full */
    }
    else /* if bus error detected, exit */
    {
      return ERROR;
    }
  }

  return SUCCESS;  /* all data are transmitted successfully */
}

/*******************************************************************************
* Function Name  : BSPI_WordReceive.
* Description    : Returns the most recent received word by the BSPI peripheral.
* Input          : BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
* Output         : None
* Return         : The value of the received word.
*******************************************************************************/
u16 BSPI_WordReceive(BSPI_TypeDef* BSPIx)
{
  if((BSPIx->CSR1 & BSPI_WordLength_16b) == FALSE)
  {
    return ( BSPIx->RXR >> 8 );
  }
  else
  {
   return ( BSPIx->RXR );
  }
}

/*******************************************************************************
* Function Name  : BSPI_BufferReceive.
* Description    : Receives number of data bytes and stores them in user defined
*                  buffer. This function will fail if an error occurs.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - PtrToBuffer : ‘u8’ pointer to the buffer that receives the
*                    data to be read.
*                  - NbOfBytes : specifies the number of bytes to be read.
* Output         : None
* Return         : An ErrorStatus enumuration value:
*                         - SUCCESS: reception done without error
*                         - ERROR: an error detected during reception
*******************************************************************************/
ErrorStatus BSPI_BufferReceive(BSPI_TypeDef* BSPIx, u8* PtrToBuffer, u8 NbOfBytes)
{
  while(NbOfBytes--) /* while the whole data are not yet received */
  {
    while(BSPI_FlagStatus(BSPIx, BSPI_FLAG_RFNE) == RESET); /* loop while Receive
                                                              FIFO is empty */
    *PtrToBuffer = BSPIx->RXR >> 8; /* read the current byte  */
    PtrToBuffer++;  /* point to the next location to write to */

    /* if bus error detected, exit */
    if(BSPI_FlagStatus(BSPIx, BSPI_FLAG_BERR) == SET)
    {
      return ERROR;
    }
  }

  return SUCCESS;  /* all data are received successfully */
}

/*******************************************************************************
* Function Name  : BSPI_RxFifoDisable
* Description    : Disables the BSPIx receive FIFO.
* Input          : BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI_RxFifoDisable(BSPI_TypeDef* BSPIx)
{
  BSPIx->CSR2 |= BSPI_DFIFO;
}

/*******************************************************************************
* Function Name  : BSPI_FlagStatus
* Description    : Checks whether the specified BSPI flag is set or not.
* Input          : - BSPIx: where x can be 0, 1 or 2 to select the BSPI peripheral.
*                  - BSPI_Flag: flag to check. This parameter can be one of the
*                    following values:
*                         - BSPI_FLAG_BERR: Bus error flag
*                         - BSPI_FLAG_RFNE: Receive FIFO not empty flag
*                         - BSPI_FLAG_RFF: Receive FIFO full flag
*                         - BSPI_FLAG_ROFL: Receive FIFO overflow flag
*                         - BSPI_FLAG_TFE: Transmit FIFO empty flag
*                         - BSPI_FLAG_TUFL: Transmit underflow flag
*                         - BSPI_FLAG_TFF: Transmit FIFO full flag
*                         - BSPI_FLAG_TFNE: Transmit FIFO not empty flag
* Output         : None
* Return         : The new state of BSPI_Flag (SET or RESET).
*******************************************************************************/
FlagStatus BSPI_FlagStatus(BSPI_TypeDef* BSPIx, u16 BSPI_Flag)
{
  if((BSPIx->CSR2 & BSPI_Flag) != RESET)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
