/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_dma.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the DMA software functions.
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
#include "73x_dma.h"
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
* Function Name  : DMA_DeInit
* Description    : Deinitializes the DMAx stream registers to their default reset
*                  values.
* Input          : - DMAx: where x can be 0, 1, 2  or 3 to select the DMA controller.
*                  - DMA_Stream: DMA stream to be deinitialized.
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_DeInit(DMA_TypeDef* DMAx, u8 DMA_Stream)
{
  u32 Stream_BaseAddr = 0;
  
  /* get the base address of the stream to be used:
  Stream_BaseAddr := DMAx_Base + Stream offset */
  Stream_BaseAddr = (*(u32*)&(DMAx)) + DMA_Stream ;

  *(u16 *)(Stream_BaseAddr + DMA_SOURCEL) = 0;
  *(u16 *)(Stream_BaseAddr + DMA_SourceH) = 0;

  *(u16 *)(Stream_BaseAddr + DMA_DESTL) = 0;
  *(u16 *)(Stream_BaseAddr + DMA_DESTH) = 0;

  *(u16 *)(Stream_BaseAddr + DMA_MAX)  = 0;
  *(u16 *)(Stream_BaseAddr + DMA_CTRL) = 0;
  *(u16 *)(Stream_BaseAddr + DMA_LUBUFF) = 0;

  DMAx->MASK = 0;
  DMAx->CLR = 0;
  DMAx->Last = 0;
}

/*******************************************************************************
* Function Name  : DMA_Init
* Description    : Initializes the DMAx stream according to the specified
*                  parameters in the DMA_InitStruct structure.
* Input          : - DMAx: where x can be 0, 1, 2  or 3 to select the DMA controller.
*                  - DMA_InitStruct:  pointer to a DMA_InitTypeDef structure that
*                    contains the configuration information for the specified
*                    DMA stream.
* Output         : None
* Return         : None
******************************************************************************/
void DMA_Init(DMA_TypeDef* DMAx, DMA_InitTypeDef* DMA_InitStruct)
{
  u32 Stream_BaseAddr = 0;

  /* get the base address of the stream to be used:
     Stream_BaseAddr := DMAx_Base + Stream offset */
  Stream_BaseAddr = (*(u32*)&(DMAx)) + DMA_InitStruct->DMA_Stream ;

  *(u16 *)(Stream_BaseAddr + DMA_SOURCEL) = 0;
  *(u16 *)(Stream_BaseAddr + DMA_SourceH) = 0;

  *(u16 *)(Stream_BaseAddr + DMA_DESTL) = 0;
  *(u16 *)(Stream_BaseAddr + DMA_DESTH) = 0;

  *(u16 *)(Stream_BaseAddr + DMA_MAX)  = 0;
  *(u16 *)(Stream_BaseAddr + DMA_CTRL) = 0;

  /* set the buffer Size */
  *(u16 *)(Stream_BaseAddr + DMA_MAX) = DMA_InitStruct->DMA_BufferSize ;

  /* Fill the DMA Control Register */
  *(u16 *)(Stream_BaseAddr + DMA_CTRL) = DMA_InitStruct->DMA_SRC;
  *(u16 *)(Stream_BaseAddr + DMA_CTRL) |= DMA_InitStruct->DMA_DST;
  *(u16 *)(Stream_BaseAddr + DMA_CTRL) |= DMA_InitStruct->DMA_SRCSize;
  *(u16 *)(Stream_BaseAddr + DMA_CTRL) |= DMA_InitStruct->DMA_SRCBurst;
  *(u16 *)(Stream_BaseAddr + DMA_CTRL) |= DMA_InitStruct->DMA_DSTSize;
  *(u16 *)(Stream_BaseAddr + DMA_CTRL) |= DMA_InitStruct->DMA_Mode;
  *(u16 *)(Stream_BaseAddr + DMA_CTRL) |= DMA_InitStruct->DMA_Dir;
  
  if(DMA_InitStruct->DMA_Stream == DMA_Stream3)
  {
    *(u16 *)(Stream_BaseAddr + DMA_CTRL) |= DMA_InitStruct->DMA_M2M;
  }

  /* configure the Source base address */
  *(u16 *)(Stream_BaseAddr + DMA_SOURCEL) = DMA_InitStruct->DMA_SRCBaseAddr;
  *(u16 *)(Stream_BaseAddr + DMA_SourceH) = DMA_InitStruct->DMA_SRCBaseAddr >> 16;

  /* configure the Destination base address */
  *(u16 *)(Stream_BaseAddr + DMA_DESTL) = DMA_InitStruct->DMA_DSTBaseAddr;
  *(u16 *)(Stream_BaseAddr + DMA_DESTH) = DMA_InitStruct->DMA_DSTBaseAddr >> 16;
  
  switch(DMA_InitStruct->DMA_TriggeringSource)
  {
    case DMA_TriggeringSource_BSPI0 :
    CFG->R0 |= DMA_TriggeringSource_BSPI0;
    break;

    case DMA_TriggeringSource_BSPI1 :
    CFG->R0 |= DMA_TriggeringSource_BSPI1;
    break;

    case DMA_TriggeringSource_TIM8_TIM9:
    CFG->R0 &= DMA_TriggeringSource_TIM8_TIM9;
    break;
  }
}

/*******************************************************************************
* Function Name  : DMA_StructInit
* Description    : Fills in a DMA_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : DMA_InitStruct : pointer to a DMA_InitTypeDef structure
*                  which will be initialized.
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct)
{
  /* initialize the DMA_BufferSize member */
  DMA_InitStruct->DMA_BufferSize = 0;

  /* initialize the DMA_SRCBaseAddr member */
  DMA_InitStruct->DMA_SRCBaseAddr = 0;

  /* initialize the DMA_DSTBaseAddr member */
  DMA_InitStruct ->DMA_DSTBaseAddr = 0;
  
  /* initialize the DMA_SRC member */
  DMA_InitStruct->DMA_SRC = DMA_SRC_NOT_INCR;
  
  /* initialize the DMA_DST member */
  DMA_InitStruct->DMA_DST = DMA_DST_NOT_INCR;
  
  /* initialize the DMA_SRCSize member */
  DMA_InitStruct->DMA_SRCSize = DMA_SRCSize_Byte;
  
  /* initialize the DMA_SRCBurst member */
  DMA_InitStruct->DMA_SRCBurst = DMA_SRCBurst_1Word;
  
  /* initialize the DMA_DSTSize member */
  DMA_InitStruct->DMA_DSTSize = DMA_DSTSize_Byte;
  
  /* initialize the DMA_Mode member */
  DMA_InitStruct->DMA_Mode = DMA_Mode_Normal;
  
  /* initialize the DMA_M2M member */
  DMA_InitStruct->DMA_M2M =  DMA_M2M_Disable;

  /* initialize the DMA_Dir member */
  DMA_InitStruct->DMA_Dir = DMA_Dir_PeriphSRC;
}

/*******************************************************************************
* Function Name  : DMA_AHBArbitrationConfig
* Description    : Handles the arbitration between CPU and  DMA on AHB bus.
* Input          : DMA_Priority : specifies the DMA priority on the AHB bus.
*                  This parameter can be one of the following values:
*                         - DMA_Priority_High: DMA has the highest priority.
*                         - DMA_Priority_Low: only CPU can access the bus.
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_AHBArbitrationConfig(u8 DMA_Priority)
{
  if(DMA_Priority == DMA_Priority_High)
  {
    /* Switch-on Native Arbiter and AHB Arbiter clocks */
    CFG_PeripheralClockConfig(CFG_CLK_ARB, ENABLE);
    CFG_PeripheralClockConfig(CFG_CLK_AHB, ENABLE);

    /* set the DMA priority to the highest */
    ARB->PRIOR = DMA_Priority_High;
  }
  else
  {
    /* Switch-off the Native Arbiter and AHB Arbiter clocks */
    CFG_PeripheralClockConfig(CFG_CLK_ARB, DISABLE);
    CFG_PeripheralClockConfig(CFG_CLK_AHB, DISABLE);
  }
}

/*******************************************************************************
* Function Name  : DMA_Cmd
* Description    : Enables or disables the specified DMA stream.
* Input          : - DMAx: where x can be 0, 1, 2  or 3 to select the DMA controller.
*                  - DMA_Stream: specifies the DMA Stream to be enabled or
*                    disabled. This parameter can be one of the following values:
*                         - DMA_Stream0: use DMAx stream0.
*                         - DMA_Stream1: use DMAx stream1.
*                         - DMA_Stream2: use DMAx stream2.
*                         - DMA_Stream3: use DMAx stream3.
*                  - NewState: new state of the DMAx stream. This parameter can
*                  be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_Cmd(DMA_TypeDef* DMAx, u8 DMA_Stream, FunctionalState NewState)
{
  u32 Stream_BaseAddr = 0;

 /* set the base address of the stream to be used:
    Stream_BaseAddr := DMAx_Base + Stream offset */
  Stream_BaseAddr = (*(u32*)&(DMAx)) + DMA_Stream ;

  if(NewState == ENABLE)
  {
    *(u16 *)(Stream_BaseAddr + DMA_CTRL) |= DMA_Enable;
  }
  else
  {
    *(u16 *)(Stream_BaseAddr + DMA_CTRL) &= DMA_Disable;
  }
}

/*******************************************************************************
* Function Name  : DMA_ITConfig
* Description    : Enables or disables the specified DMA interrupts.
* Input          : - DMAx: where x can be 0, 1, 2 or 3 to select the DMA controller.
*                  - DMA_IT: specifies the DMA interrupts sources to be enabled
*                    or disabled. This parameter can be any combination of the
*                    following values:
*                         - DMA_IT_SI0: Stream0 transfer end interrupt mask
*                         - DMA_IT_SI1: Stream1 transfer end interrupt mask
*                         - DMA_IT_SI2: Stream2 transfer end interrupt mask
*                         - DMA_IT_SI3: Stream3 transfer end interrupt mask
*                         - DMA_IT_SE0: Stream0 transfer error interrupt mask
*                         - DMA_IT_SE1: Stream1 transfer error interrupt mask
*                         - DMA_IT_SE2: Stream2 transfer error interrupt mask
*                         - DMA_IT_SE3: Stream3 transfer error interrupt mask
*                         - DMA_IT_ALL: ALL DMA interrupts mask
*                  - NewState: new state of the specified DMA interrupts.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_ITConfig(DMA_TypeDef* DMAx, u16 DMA_IT, FunctionalState NewState)
{
  if(NewState == ENABLE)
  {
    DMAx->MASK |= DMA_IT;
  }
  else
  {
    DMAx->MASK &= ~DMA_IT;
  }
}

/*******************************************************************************
* Function Name  : DMA_GetCurrDSTAddr
* Description    : Returns the current value of the destination address pointer
*                  related to the specified DMA stream.
* Input          : - DMAx: where x can be 0, 1, 2 or 3 to select the DMA controller.
*                  - DMA_Stream: specifies the DMA stream to get its destination
*                    address pointer current value.
*                    This parameter can be one of the following values:
*                         - DMA_Stream0
*                         - DMA_Stream1
*                         - DMA_Stream2
*                         - DMA_Stream3
* Output         : None
* Return         : The current value of the destination address pointer related
*                  to the specified DMA stream.
*******************************************************************************/
u32 DMA_GetCurrDSTAddr(DMA_TypeDef* DMAx, u8 DMA_Stream)
{
  u32 Stream_BaseAddr = 0;

 /* get the base address of the stream to be used:
    Stream_BaseAddr := DMAx_Base + Stream offset */
  Stream_BaseAddr = (*(u32*)&(DMAx)) + DMA_Stream ;

  return(*(u32*)(Stream_BaseAddr+DMA_DECURRL)|(*(u32*)(Stream_BaseAddr+DMA_DECURRH)<<16));
}

/*******************************************************************************
* Function Name  : DMA_GetCurrSRCAddr
* Description    : Returns the current value of the source address pointer
*                  related to the specified DMA stream.
* Input          : - DMAx: where x can be 0, 1, 2 or 3 to select the DMA controller.
*                  - DMA_Stream: specifies the DMA stream to get its source
*                    address pointer current value.
*                    This parameter can be one of the following values:
*                         - DMA_Stream0
*                         - DMA_Stream1
*                         - DMA_Stream2
*                         - DMA_Stream3
* Output         : None
* Return         : The current value of the source address pointer related to
*                  the specified DMA stream.
*******************************************************************************/
u32 DMA_GetCurrSRCAddr(DMA_TypeDef* DMAx, u8 DMA_Stream)
{
  u32 Stream_BaseAddr = 0;

 /* get the base address of the stream to be used:
    Stream_BaseAddr := DMAx_Base + Stream offset */
  Stream_BaseAddr = (*(u32*)&(DMAx)) + DMA_Stream ;

  return(*(u32*)(Stream_BaseAddr+DMA_SOCURRL)|(*(u32*)(Stream_BaseAddr+DMA_SOCURRH)<<16));
}

/*******************************************************************************
* Function Name  : DMA_GetTerminalCounter
* Description    : Returns the number of data units remaining in the current
*                  DMAx stream transfer.
* Input          : - DMAx: where x can be 0, 1, 2 or 3 to select the DMA controller.
*                  - DMA_Stream: specifies the DMA stream to get its number of
*                    data units remaining in the current DMA transfer.
*                    This parameter can be one of the following values:
*                         - DMA_Stream0
*                         - DMA_Stream1
*                         - DMA_Stream2
*                         - DMA_Stream3
* Output         : None
* Return         : The number of data units remaining in the current DMAx stream
*                  transfer.
*******************************************************************************/
u16 DMA_GetTerminalCounter(DMA_TypeDef* DMAx, u8 DMA_Stream)
{
  u32 Stream_BaseAddr = 0;

 /* get the base address of the stream to be used:
    Stream_BaseAddr := DMAx_Base + Stream offset */
  Stream_BaseAddr = (*(u32*)&(DMAx)) + DMA_Stream ;

  return(*(u32*)(Stream_BaseAddr+DMA_TCNT));
}

/*******************************************************************************
* Function Name  : DMA_LastBufferSweepConfig
* Description    : Activates the last buffer sweep mode for the DMAx stream
*                  configured in circular buffer mode.
* Input          : - DMAx: where x can be 0, 1, 2 or 3 to select the DMA controller.
*                  - DMA_Stream: specifies the DMA stream to start its last
*                    circular buffer sweep.
*                    This parameter can be one of the following values:
*                         - DMA_Stream0
*                         - DMA_Stream1
*                         - DMA_Stream2
*                         - DMA_Stream3
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_LastBufferSweepConfig(DMA_TypeDef* DMAx, u8 DMA_Stream)
{
  switch(DMA_Stream)
  {
    case DMA_Stream0 :
    DMAx->Last |= DMA_LAST0;
    break;

    case DMA_Stream1 :
    DMAx->Last |= DMA_LAST1;
    break;

    case DMA_Stream2:
    DMAx->Last |= DMA_LAST2;
    break;

    case DMA_Stream3:
    DMAx->Last |= DMA_LAST3;
    break;
  }
}

/*******************************************************************************
* Function Name  : DMA_LastBufferAddrConfig
* Description    : Configures the circular buffer position where the last data 
*                  to be used by the specified DMA stream is located.
* Input          : - DMAx: where x can be 0, 1, 2 or 3 to select the DMA controller.
*                  - DMA_Stream: specifies the DMA stream to configure.
*                    This parameter can be one of the following values:
*                         - DMA_Stream0
*                         - DMA_Stream1
*                         - DMA_Stream2
*                         - DMA_Stream3
*                  - DMA_LastBufferAddr: specifies the circular buffer position
*                    where the last data to be used by the specified DMA stream
*                    is located.
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_LastBufferAddrConfig(DMA_TypeDef* DMAx, u8 DMA_Stream, u16 DMA_LastBufferAddr)
{
  u32 Stream_BaseAddr = 0;

 /* get the base address of the stream to be used:
    Stream_BaseAddr := DMAx_Base + Stream offset */
  Stream_BaseAddr = (*(u32*)&(DMAx)) + DMA_Stream ;

  *(u16 *)(Stream_BaseAddr + DMA_LUBUFF) = DMA_LastBufferAddr;
}

/*******************************************************************************
* Function Name  : DMA_TimeOutConfig
* Description    : Sets DMA transaction time out.
* Input          : TimeOut_Value: specifies the DMA Time Out value.
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_TimeOutConfig(u16 TimeOut_Value)
{
  /* set the DMA Time Out value */
  ARB->TOR = TimeOut_Value;
}

/*******************************************************************************
* Function Name  : DMA_FlagStatus
* Description    : Checks whether the specified DMA controller flag is set or not.
* Input          : - DMAx : where x can be 0, 1, 2 or 3 to select the DMA controller.
*                  - DMA_Flag: flag to check. This parameter can be one of the
*                    following values:
*                         - DMA_FLAG_INT0: Stream0 transfer end interrupt flag
*                         - DMA_FLAG_INT1: Stream1 transfer end interrupt flag
*                         - DMA_FLAG_INT2: Stream2 transfer end interrupt flag
*                         - DMA_FLAG_INT3: Stream3 transfer end interrupt flag
*                         - DMA_FLAG_ERR0: Stream0 transfer error interrupt flag
*                         - DMA_FLAG_ERR1: Stream1 transfer error interrupt flag
*                         - DMA_FLAG_ERR2: Stream2 transfer error interrupt flag
*                         - DMA_FLAG_ERR3: Stream3 transfer error interrupt flag
*                         - DMA_FLAG_ACT0: Stream0 status
*                         - DMA_FLAG_ACT1: Stream1 status
*                         - DMA_FLAG_ACT2: Stream2 status
*                         - DMA_FLAG_ACT3: Stream3 status

* Output         : None
* Return         : The new state of DMA_Flag (SET or RESET).
*******************************************************************************/
FlagStatus DMA_FlagStatus(DMA_TypeDef* DMAx, u16 DMA_Flag)
{
  if((DMAx->STATUS & DMA_Flag) != RESET)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/*******************************************************************************
* Function Name  : DMA_FlagClear
* Description    : Clears the pending interrupt flags corresponding to the
*                  specified DMA stream.
* Input          : - DMAx : where x can be 0, 1, 2 or 3 to select the DMA controller.
*                  - DMA_Flag: flags to clear. This parameter can be any
*                    combination of the following values:
*                         - DMA_FLAG_INT0: Clears the pending flag of Stream0
*                                          transfer end interrupt.
*                         - DMA_FLAG_INT1: Clears the pending flag of Stream1
*                                          transfer end interrupt.
*                         - DMA_FLAG_INT2: Clears the pending flag of Stream2
*                                          transfer end interrupt.
*                         - DMA_FLAG_INT3: Clears the pending flag of Stream3
*                                          transfer end interrupt.
*                         - DMA_FLAG_ERR0: Clears the pending flag of Stream0
*                                          transfer error interrupt.
*                         - DMA_FLAG_ERR1: Clears the pending flag of Stream1
*                                          transfer error interrupt.
*                         - DMA_FLAG_ERR2: Clears the pending flag of Stream2
*                                          transfer error interrupt.
*                         - DMA_FLAG_ERR3: Clears the pending flag of Stream3
*                                          transfer error interrupt.
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_FlagClear(DMA_TypeDef* DMAx, u16 DMA_Flag)
{
  /* Set the bit corresponding to the flags to clear */
  DMAx->CLR  =  DMA_Flag ;
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
