/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : bspi.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file contains all the functions prototypes for the
*                      BSPI software library.
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

#ifndef __720_BSPI_H
#define __720_BSPI_H

#include "720_lib.h"

typedef enum {
  BSPI_MASTER = 0x0002,
  BSPI_SLAVE
} BSPIMode_TypeDef;

typedef enum {
  BSPI_16b = 0x0400,
  BSPI_8b  = 0x0C00
} BSPIWord_TypeDef;

typedef enum {
  BSPI_FirstEdge = 0xFDFF,
  BSPI_SecondEdge=0x0200
} BSPICLKPhase_TypeDef;

typedef enum {
  BSPI_LOW = 0x0100,
  BSPI_HIGH
} BSPICLKPolarity_TypeDef;

typedef enum {
  BSPI_BERIT = 0x0080,
  BSPI_RxIT  = 0x0010,
  BSPI_ALL   = 0x0090
} BSPIITConfig_TypeDef;

typedef enum {
  BSPI_BERR  = 0x004,
  BSPI_RxFNE = 0x008,
  BSPI_RxFF  = 0x010,
  BSPI_RxOFL = 0x020,
  BSPI_TxFE  = 0x040,
  BSPI_TxUFL = 0x080,
  BSPI_TxFF  = 0x100,
  BSPI_TxFNE = 0x200
} BSPIFlags_TypeDef ;

typedef enum {
  BSPI_TxITFE  = 0x4000 ,
  BSPI_TxITUFL = 0x8000,
  BSPI_TxITFF  = 0xC000,
  BSPI_TxITDIS
} BSPITxIT_TypeDef;

typedef enum {
  BSPI_RxITFNE = 0x0004,
  BSPI_RxITFF  = 0x000C,
  BSPI_RxITDIS
} BSPIRxIT_TypeDef;

typedef enum {
 BSPI_Tx1Word  = 0x00F3,
 BSPI_Tx4Word  = 0x0004,
 BSPI_Tx8Word  = 0x0008,
 BSPI_Tx16Word = 0x000C
} BSPITxBurst_TypeDef;

typedef enum {
 BSPI_Rx1Word  = 0x00CF,
 BSPI_Rx4Word  = 0x0010,
 BSPI_Rx8Word  = 0x0020,
 BSPI_Rx16Word = 0x0030
} BSPIRxBurst_TypeDef;

#define BSPI_BSPE 0x01
#define BSPI_DFIFO 0x0001
#define BSPI_MaskSlave 0x0001
#define BSPI_MaskDMAConfig 0x0002
#define BSPI_TREQ 0x0040
#define BSPI_RREQ 0x0080

/*******************************************************************************
* Function Name  : BSPI_Init
* Description    : Initializes BSPI peripheral control and registers to their
*                : default reset values.
* Input          : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Output         : BSPI peripheral registers and BSPI global variables are
*                : initialized.
* Return         : None.
*******************************************************************************/
void BSPI_Init( BSPI_TypeDef *BSPIx);
/*******************************************************************************
* Function Name  : BSPI_Config
* Description    : Enables/disables the specified BSPI peripheral.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral to
*                : be enabled or disabled.
* Input 2        : NewState: specifies the status of the BSPI
*                : New State value must be either ENABLE or DISABLE:
*                : ENABLE: the specified BSPI peripheral will be enabled.
*                : DISABLE: the specified BSPI peripheral will be disabled.
* Output         : BSPE bit  in BSPCSR1.
* Return         : None.
*******************************************************************************/
static inline void BSPI_Config( BSPI_TypeDef *BSPIx, FunctionalState NewState)
{
  if (NewState == ENABLE) BSPIx->CSR1 |= BSPI_BSPE;
  else BSPIx->CSR1 &= ~BSPI_BSPE;
}
/*******************************************************************************
* Function Name  : BSPI_ModeConfig
* Description    : Configures the BSPI as a master or a slave.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : ModeConfig: specifies the BSPI mode.
*                : Refer to the section ‘Mode Configuration’ for more details
*                : on the allowed values of this parameter.
* Output         : MSTR bit in BSPCSR1 is modified register.
* Return         : None.
*******************************************************************************/
static inline void BSPI_ModeConfig(BSPI_TypeDef *BSPIx,BSPIMode_TypeDef Mode)
{
  if (Mode == BSPI_MASTER) BSPIx->CSR1 |= BSPI_MASTER;
  else BSPIx->CSR1 &= ~BSPI_MASTER;
}
/*******************************************************************************
* Function Name  : WordConfig
* Description    : Configure the word length of the receive FIFO and transmit data
*                : registers to either 8 or 16 bits.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI.
* Input 2        : WordConfig: specifies the word length.
*                : Refer to the section ‘Word Configuration’ for more details 
*                : on the allowed values of this parameter.
* Output         : WL bits in BSPCSR1 register.
* Return         : None.
*******************************************************************************/
static inline void BSPI_WordConfig(BSPI_TypeDef *BSPIx,BSPIWord_TypeDef Word)
{
  if( Word == BSPI_16b) BSPIx->CSR1 |= BSPI_16b; else BSPIx->CSR1 &= ~BSPI_8b;
}
/*******************************************************************************
* Function Name  : BSPI_CLKPhaseConfig
* Description    : Configure capturing the first data sample on the first edge
*                : of clock or on the second edge.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : CLKPhase: specifies the mode of capturing the first data.
*                : Refer to the section ‘Clock Phase Configuration’ for more
*                : details on the allowed values of this parameter.
* Output         : CPHA bits in BSPCSR1 register.
* Return         : None.
*******************************************************************************/
static inline void BSPI_CLKPhaseConfig(BSPI_TypeDef *BSPIx, BSPICLKPhase_TypeDef CLKPhase)
{
  if (CLKPhase == BSPI_FirstEdge) BSPIx->CSR1&=BSPI_FirstEdge; 
  else BSPIx->CSR1 |=BSPI_SecondEdge;
}
/*******************************************************************************
* Function Name  : BSPI_CLKPolarity
* Description    : Configures the clock to be active high or low.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : CLKPolarity: specifies the clock as a active high or a low.
*                : Refer to the section ‘Clock Polarity Config’ for more details
*                : on the allowed values of this parameter.
* Output         : CPOL bits in BSPCSR1 register.
* Return         : None.
*******************************************************************************/
static inline void BSPI_CLKPolarityConfig(BSPI_TypeDef *BSPIx, BSPICLKPolarity_TypeDef CLKPolarity)
{
  if (CLKPolarity == BSPI_LOW) BSPIx->CSR1 |= BSPI_LOW;
  else BSPIx->CSR1 &= ~BSPI_LOW;
}
/*******************************************************************************
* Function Name  : BSPI_CLKDividerConfig
* Description    : Configure BSPI clock divider
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI.
* Input 2        : Div: holds the value of the clock divider.
* Output         : Div [7:0] bits in BSPCLK register.
* Return         : None.
*******************************************************************************/
static inline void BSPI_CLKDividerConfig(BSPI_TypeDef *BSPIx, vu8 CLKDivider)
{
  BSPIx->CLK = CLKDivider;
}
/*******************************************************************************
* Function Name  : BSPI_WordSend.
* Description    : Transmit a single Word.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : holds the word which will be transmitted.
* Output         : BSPTXR register.
* Return         : None.
*******************************************************************************/
static inline void BSPI_WordSend(BSPI_TypeDef *BSPIx, vu16 Data)
{
  if ((BSPIx->CSR1 & 0x0400) == 0) Data <<= 8;
  BSPIx->TXR = Data;
}
/*******************************************************************************
* Function Name  : BSPI_WordReceive.
* Description    : Returns the recent received word.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Output         : None.
* Return         : The value of the received word.
*******************************************************************************/
static inline u16 BSPI_WordReceive(BSPI_TypeDef *BSPIx)
{
  return (BSPIx->CSR1 & 0x0400) == 0 ? BSPIx->RXR >> 8 : BSPIx->RXR;
}
/*******************************************************************************
* Function Name  : BSPI_ITConfig.
* Description    : Enable or disable interrupt.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : ITConfig: specifies the BSPI interrupt.
*                : Refer to the section ‘Interrupt Configuration’ for more details on
*                : the allowed values of this parameter..
* Input 3        : NewState: specified whether the BSPI interrupt is enabled or
*                : disabled.
*		 : New State value must be either ENABLE or DISABLE.
*		 : ENABLE: to enable interrupt
*		 : DISABLE: to disable interrupt.
* Output         : BSPCSR1 register.
* Return         : None.
*******************************************************************************/
static inline void BSPI_ITConfig(BSPI_TypeDef *BSPIx, BSPIITConfig_TypeDef ITConfig, FunctionalState NewState)
{
  if (NewState == ENABLE) BSPIx->CSR1 |= ITConfig; else BSPIx->CSR1 &= ~ITConfig;
}
/*******************************************************************************
* Function Name  : BSPI_FifoDisable
* Description    : Configure the BSPI FIFO.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Output         : DFIFO bit in BSPCSR2 register.
* Return         : None.
*******************************************************************************/
static inline void BSPI_FifoDisable(BSPI_TypeDef *BSPIx)
{
  BSPIx->CSR2 |= BSPI_DFIFO;
}
/*******************************************************************************
* Function Name  : BSPI_FlagStatus.
* Description    : Check whether the specified BSPI Flag is set or not.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : Flag: specifies the flag to see the status.
*                : Refer to the section ‘BSPI Flags’ for more details on the
*                : allowed values of this parameter.
* Output         : None.
* Return         : FlagStatus type
*		 : SET: if the tested flag is set.
*                : RESET: if the corresponding flag is reset.
*******************************************************************************/
static inline FlagStatus BSPI_FlagStatus(BSPI_TypeDef *BSPIx, BSPIFlags_TypeDef Flag)
{
  return (BSPIx->CSR2 &= Flag) != 0 ? SET : RESET;
}
/*******************************************************************************
* Function Name  : BSPI_MaskSlaveConfig
* Description    : Enable/Disable mask of salve select pin.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : NewState: specfied whether the BSPI mask slave select is enabled
*                : or disabled.
*		 : New State value must be ENABLE or DISABLE.
*		 : ENABLE: to enable mask slave select.
*		 : DISABLE: to disable mask slave select.
* Output         : MASK_SS bit in BSPCSR3 register.
* Return         : None.
*******************************************************************************/
static inline void BSPI_MaskSlaveConfig(BSPI_TypeDef *BSPIx, FunctionalState NewState )
{
  if ( NewState == ENABLE) BSPIx->CSR3 |= BSPI_MaskSlave;
  else BSPIx->CSR3 &= ~BSPI_MaskSlave;
}
/*******************************************************************************
* Function Name  : BSPI_DMAConfig
* Description    : Enable/disable  DMA interface.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : NewState: specfied whether the BSPI DMA interface is enabled
*                : or disabled.
*		 : New State value must be ENABLE or DISABLE.
*		 : ENABLE: to enable DMA interface.
*		 : DISABLE: to disable DMA interface.
* Output         : DMA_EN bit in BSPCSR3 register.
* Return         : None.
*******************************************************************************/
static inline void BSPI_DMAConfig(BSPI_TypeDef *BSPIx, FunctionalState NewState )
{
  if ( NewState == ENABLE) BSPIx->CSR3 |= BSPI_MaskDMAConfig;
  else BSPIx->CSR1 &= ~BSPI_MaskDMAConfig;
}
/*******************************************************************************
* Function Name  : BSPI_TxReqConfig
* Description    : Enable/disable the transmission DMA request.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : NewState: specfied whether the BSPI transmit request is enabled
*                : or disabled.
*		 : New State value must be ENABLE or DISABLE.
*		 : ENABLE:  to enable transmit request.
*		 : DISABLE: to disable transmit request.
* Output         : TREQ_EN bit in BSPCSR3 register.
* Return         : None.
*******************************************************************************/
static inline void  BSPI_TxReqConfig(BSPI_TypeDef *BSPIx,FunctionalState NewState)
{
  if ( NewState == ENABLE) BSPIx->CSR3 |= BSPI_TREQ;
  else BSPIx->CSR3 &= ~BSPI_TREQ;
}
/*******************************************************************************
* Function Name  : BSPI_RxReqConfig
* Description    : Enable/disable the reception DMA request.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : NewState: specfied whether the BSPI transmit request is enabled
*                : or disabled.
*		 : New State value must be ENABLE or DISABLE.
*		 : ENABLE:  to enable transmit request.
*		 : DISABLE: to disable transmit request.
* Output         : RREQ_EN bit in BSPCSR3 register.
* Return         : None.
*******************************************************************************/
static inline void  BSPI_RxReqConfig(BSPI_TypeDef *BSPIx,FunctionalState NewState)
{
  if ( NewState == ENABLE) BSPIx->CSR3 |= BSPI_RREQ;
  else BSPIx->CSR3 &= ~BSPI_RREQ;
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
void BSPI_TxITConfig(BSPI_TypeDef *BSPIx, BSPITxIT_TypeDef TxIT);
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
void BSPI_RxITConfig(BSPI_TypeDef *BSPIx, BSPIRxIT_TypeDef RxIT);
/*******************************************************************************
* Function Name  : BSPI_TxFifoConfig
* Description    : Configures BSPI transmission FIFO number of words.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI.
* Input 2        : TxDepth:specifies the depth of the transmit FIFO.
* Output         : TFE bit in BSPCSR2 register.
* Return         : None.
*******************************************************************************/
void BSPI_TxFifoConfig(BSPI_TypeDef *BSPIx, vu8 TxDepth);
/*******************************************************************************
* Function Name  : BSPI_RxFifoDepth
* Description    : Configures BSPI reception FIFO number of words.
* Input 1        : BSPIx where x can be 1 or 2 to select the BSPI peripheral.
* Input 2        : RxDepth:specifies the depth of the receive FIFO.
* Output         : RFE bits [3:0] in BSPCSR1 register.
* Return         : None.
*******************************************************************************/
void BSPI_RxFifoConfig(BSPI_TypeDef *BSPIx, vu8 RxDepth);
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
void  BSPI_BufferSend(BSPI_TypeDef *BSPIx, vu16 *PtrToBuffer, vu8 NbOfWords);
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
void BSPI_BufferReceive(BSPI_TypeDef *BSPIx, vu16 *PtrToBuffer, vu8 NbOfWords);
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
void BSPI_TxBurstConfig(BSPI_TypeDef *BSPIx, BSPITxBurst_TypeDef TxBurst);
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
void BSPI_RxBurstConfig(BSPI_TypeDef *BSPIx, BSPIRxBurst_TypeDef RxBurst);
#endif /* __720_BSPI_H */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/


