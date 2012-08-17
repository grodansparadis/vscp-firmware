/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_DMAC.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file contains source code of  DMAC software library.
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

#include "720_dmac.h"

/*******************************************************************************
* Function Name  : DMAC_Init
* Description    : Initialize the DMA Controller.
* Input          : DMAC_DataStream: select which Data Stream 
                  ( X can be 2  or 3) to configure
* Return         : None

*******************************************************************************/

void DMAC_Init(vu8 DMAC_DataStreamX)
{
 *(vu16 *)(DMAC_BASE+DMAC_DataStreamX + DMAMax)=0x00;
 *(vu16 *)(DMAC_BASE+DMAC_DataStreamX + DMACtrl)=0x00;
 *(vu16 *)(DMAC_BASE+DMAC_DataStreamX + DMATCnt)=0x00;
 *(vu16 *)(DMAC_BASE+DMAC_DataStreamX + DMALUBuff)=0x00;
 DMAC->DMAMask=0x00;
 DMAC->DMAClr=0x00;
 DMAC->DMALast=0x00;
}  

/*******************************************************************************
* Function Name  : DMAC_SourceBaseAddressConfig
* Description    : This function is used to configures the source base address
*                  for the specified stream X source DMA buffer.

* Input1         : Data Stream (vu8): select which Data Stream (2 or 3)
                   to configure

* Input2         : DMAC_SAdd (vu32):Source address  value to be written
                   to the source base addr register  
* Return         : None
*******************************************************************************/

void DMAC_SourceBaseAddressConfig(vu8 DMAC_DataStreamX,vu32 DMAC_SAdd)
{
 *(vu16 *)(DMAC_BASE+DMAC_DataStreamX + DMASoLo)=DMAC_SAdd;
 *(vu16 *) (DMAC_BASE+DMAC_DataStreamX+DMASoHi)=(DMAC_SAdd>>16);
}

/******************************************************************************
* Function Name  : DMAC_DestinationBaseAddressConfig
* Description    : This function is used to configures the destination 
                   base address
*                  for the specified stream X source DMA buffer.

* Input1         : Data Stream (vu8): select which Data Stream (2 or 3)
                   to configure

* Input2         : DMAC_DAdd (vu32):Source address  value to be written
                   to the destination base addr register  
* Return         : None
*******************************************************************************/

void DMAC_DestinationBaseAddressConfig(vu8 DMAC_DataStreamX,vu32 DMAC_DAdd)
{
 *(vu16 *)(DMAC_BASE+DMAC_DataStreamX+DMADeLo)=DMAC_DAdd;
 *(vu16 *)(DMAC_BASE+DMAC_DataStreamX+DMADeHi)=(DMAC_DAdd>>16);
}

/*******************************************************************************
* Function Name  : DMAC_BufferSize
* Description    : This function is used to  set the maximum data unit count 
                   for the specified stream X which correspond to 
                   the buffer size.
* Input1         : Data Stream (vu8): select which Data Stream (2 or 3)
                   to configure

* Input2         : DMAC_BufferSize: StreamX maximum data unit count defining 
                   the buffer size and must be written to the  maximum count 
                   registerX  
* Return         : None
*******************************************************************************/

void DMAC_BufferSize(vu8 DMAC_DataStreamX,vu16 DMAC_BufferSize)
{
 *(vu16 *)(DMAC_BASE+DMAC_DataStreamX+DMAMax)=DMAC_BufferSize;
}

/*******************************************************************************
* Function Name  : DMAC_DMAConfig
* Description    : For each data streams (2 or 3), it configures enabling,
                   incrementation of the current source register ,
                   incrementation of the current destination register ,
                   data width for source to DMA,number of words in the 
                   peripheral burst,data width for DMA to the destination,
                   the DMA mode(normal/circular),enables/disables the memory
                   to memory transfer for the stream 3 and indicates if the 
                   peripheral is the source or the destination.
* Input1         : Data Stream (vu8): select which Data Stream (2 or 3)
                   to configure

* Input2         : Config: value to be loaded in the appropiateDMAC register 
                   in order to configure the selected data stream.
* Return         : None

*******************************************************************************/

void DMAC_DMAConfig(vu8 DMAC_DataStreamX, vu16 Config)
{
 *(vu16 *)(DMAC_BASE+DMAC_DataStreamX+DMACtrl)=Config;
}

/******************************************************************************
* Function Name  : DMAC_CurrentSourceAddress
* Description    : For each data streams (2 or 3), it returns the current value 
                   of the source address 
* Input          : Data Stream (vu8): select which Data Stream (2 or 3)
                   to configure


* Return         : The current value of the source address pointer
                   related to streamX.

********************************************************************************/

u32 DMAC_CurrentSourceAddress(vu8 DMAC_DataStreamX)
{
return(*(u32*)(DMAC_BASE+DMAC_DataStreamX+DMASoCurrLo))|((*(u32*)(DMAC_BASE+DMAC_DataStreamX+DMASoCurrHi))<<16);
}

/********************************************************************************
* Function Name  : DMAC_CurrentDestinationAddress
* Description    : For each data streams (2 or 3), it returns the current value 
                   of the destination address 
* Input          : Data Stream (vu8): select which Data Stream (2 or 3)
                   to configure


* Return         : The current value of the destination address pointer related 
                   to streamX.

********************************************************************************/

u32 DMAC_CurrentDestinationAddress(vu8 DMAC_DataStreamX)
{
return(*(u32*)(DMAC_BASE+DMAC_DataStreamX+DMADeCurrLo))|((*(u32*)(DMAC_BASE+DMAC_DataStreamX+DMADeCurrHi))<<16);

}

/********************************************************************************
* Function Name  : DMAC_RemainingUnitNumber
* Description    : For each data streams (2 or 3), It returns the number of the 
                   data units remaining in the current DMA transfer.
* Input          : Data Stream (vu8): select which Data Stream (2 or 3)
                   to configure

* Return         : It returns the number of the data units remaining 
                   in the current DMA transfer.

********************************************************************************/

u16 DMAC_RemainingUnitNumber(vu8 DMAC_DataStreamX)
{
return (DMAC_BASE+DMAC_DataStreamX+DMATCnt);
}

/*******************************************************************************
* Function Name  : DMAC_LastBufferAddressConfig
* Description    : This function is used to configure the circular
                   buffer position where the last data to be used by the
                   specified stream X is located.
* Input1         : Data Stream (vu8): select which Data Stream (2 or 3)
                   to configure.

* Input2         : DMAC_LastBufferAddress: Last buffer position addres to be written to
                   the Last Used Buffer location register.

* Return         : None
*******************************************************************************/

void DMAC_LastBufferAddressConfig(vu8 DMAC_DataStreamX, vu16 DMAC_LastBufferAddress)
{
 *(u16*)(DMAC_BASE+DMAC_DataStreamX+DMALUBuff)=DMAC_LastBufferAddress;
}


/*******************************************************************************
* Function Name  : DMAC_ITConfig
* Description    : This function  configures the status Flag that can generate 
                   an interrupt.
* Input1         : Flag: this parameter specifies DMAC interrupts sources
                   to be configured.Flag can be 
                                - DMAC_SIM2 : Enable the Stream 2 end Interrupt
                                - DMAC_SIM3:  Enable the Stream 3 end Interrupt
                                - DMAC_SEM2 : Enable the Stream 2 error Interrupt
                                - DMAC_SEM3 : Enable the Stream 3 error Interrupt
                   The user can select more than one interrupt source, 
                   by OR’ing them.
* Input2         : NewState: ENABLE or DISABLE.
 
* Return         : None
*******************************************************************************/

void DMAC_ITConfig(vu16 DMAC_Flag ,FunctionalState NewState)
{
if(NewState==ENABLE){
   DMAC->DMAMask|=DMAC_Flag;
     } else
       DMAC->DMAMask&=~DMAC_Flag;
}


/*******************************************************************************
* Function Name  : DMAC_FlagClear
* Description    : This function is used to clear the status flags.
* Input1         : DMAC_Flag: this parameter specifies DMAC flags
                   ( interrupts or error) to be cleared. Flag can be
                                - DMAC_SIM2  : Clear the Stream 2 end Interrupt
                                - DMAC_SIM3  : Clear the Stream 3 end Interrupt
                                - DMAC_SEM2 : Clear the Stream 2 error Interrupt
                                - DMAC_SEM3 : Clear the Stream 3 error Interrupt
                    The user can select more than one flags,by OR’ing them.

* Return         : None
*******************************************************************************/

void DMAC_FlagClear(vu16 DMAC_Flag )
{ 

   DMAC->DMAClr|=DMAC_Flag;
     
}

/*******************************************************************************
* Function Name  : DMAC_FlagStatus
* Description    : This function  provides status information regarding 
                   the DMA Controller.
* Input          : DMAC_Flag:The flag to be read.Flag can be:
                                 - DMAC_SIM2 : Stream 2 end Interrupt
                                 - DMAC_SIM3 : Stream 3 end Interrupt
                                 - DMAC_SEM2 : Stream 2 error Interrupt
                                 - DMAC_SEM3 : Stream 3 error Interrupt
                                 - DMAC_ACT2 : Stream 2 error Interrupt
                                 - DMAC_ACT3 : Stream 3 error Interrupt
                The user can’t select more than one flag  at the same moment

* Return         : The status of the selected  flag (SET or RESET)

*******************************************************************************/

FlagStatus DMAC_FlagStatus(vu16 DMAC_Flag)
{
  // Test on the flag status and return set or RESET
  return DMAC->DMAStatus & DMAC_Flag ? SET : RESET;
}

/*******************************************************************************
* Function Name  : DMAC_GetStatus
* Description    : This function  provides status information regarding 
                   the DMA Controller.
* Input          : DMAC_Flag:The flag to be read.Flag can be:
                                - DMAC_SIM2  : Stream 2 end Interrupt
                                - DMAC_SIM3  : Stream 3 end Interrupt
                                - DMAC_SEM2 : Stream 2 error Interrupt
                                - DMAC_SEM3 : Stream 3 error Interrupt
                                - DMAC_ACT2 : Stream 2 error Interrupt
                                - DMAC_ACT3 : Stream 3 error Interrupt
                 The user can select more than one flags,by OR’ing them.

* Return         : The DMAC status information(the interrupt status register value)

*******************************************************************************/
u16 DMAC_GetStatus(vu16 DMAC_Flag)
{

  return (DMAC->DMAStatus & DMAC_Flag) ;
}


/*******************************************************************************
* Function Name  : DMAC_LastBufferSweep
* Description    : It allows the activation of last buffer sweep mode
                   for the stream configured in circular buffer mode.
* Input1         : DMAC_LASTX notify the DMA that the last circular buffer sweep
                   started for the corresponding StreamX,
                   (X can take the following values:2,3).
                     The user can’t select more than one flag  at the same moment
* Return         : None
********************************************************************************/

void DMAC_LastBufferSweep(vu16 DMAC_LASTX)
{ 

      DMAC->DMALast|=DMAC_LASTX;
}

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
