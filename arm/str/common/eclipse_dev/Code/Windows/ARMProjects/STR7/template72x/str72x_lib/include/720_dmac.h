/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_DMAC.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file contains all the functions prototypes for the
*                      DMAC software library.
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

#ifndef __720_DMAC_H
#define __720_DMAC_H

#include "720_lib.h"

//____________________________DMAC_Data_Streams________________________________

#define DMAC_DataStream0     0x00
#define DMAC_DataStream1     0x40
#define DMAC_DataStream2     0x80
#define DMAC_DataStream3     0xC0


//____________________________DMAC_Registers___________________________________


 #define DMASoLo         0x0                  //DMAC source base address low 2
                                               // register
 #define DMASoHi         0x4                  //DMAC source base address high 2 
                                              //  register
 #define DMADeLo         0x8                  //DMAC destination base address 
                                               // low 2 register
 #define DMADeHi         0xC                  //DMAC destination base address 
                                              // high 2 register
 #define DMAMax          0x10                 //DMAC Maximum count register 
 #define DMACtrl         0x14                 //DMAC Control register 
 #define DMASoCurrHi     0x18                 //DMAC Current Source address 
                                               // high 2 register 
 #define DMASoCurrLo     0x1C                 //DMAC Current Source address 
                                               // low 2 register 
 #define DMADeCurrHi     0x20                 //DMAC Current Destination address 
                                              //  high 2 register 
 #define DMADeCurrLo     0x24                 //DMAC Current Destination address 
                                               // low 2 register 
 #define DMATCnt         0x28                 //DMAC Terminal Counter Register
 #define DMALUBuff       0x2C                 //DMAC Last Used Buffer location 
                                               // register
  
//______________________________DMAC_Configuration______________________

#define DMAC_ENABLE                               0x1
#define DMAC_DISABLE                              0x0
#define DMAC_SourceIncrement                      0x2
#define DMAC_SourceUnchanged                      0x0
#define DMAC_DestinationIncrement                 0x4
#define DMAC_DestinationUnchanged                 0x0
#define DMAC_SourceSizeByte                       0x0
#define DMAC_SourceSizeHalfWord                   0x8
#define DMAC_SourceSizeWord                       0x10
#define DMAC_PeripheralBurstSize1word             0x0
#define DMAC_PeripheralBurstSize4words            0x20
#define DMAC_PeripheralBurstSize8words            0x40
#define DMAC_PeripheralBurstSize16words           0x60
#define DMAC_DestinationSizeByte                  0x0
#define DMAC_DetinationSizeHalfWord               0x80
#define DMAC_DestinationSizeWord                  0x100
#define DMAC_NormalMode                           0x0
#define DMAC_CircularMode                         0x200
#define DMAC_PeripheralSource                     0x0
#define DMAC_PeripheralDestination                0x2000
#define DMAC_TransferMem2Mem                      0x800


//_______________DMAC_Interupt and Mask Flags___________________________



#define DMAC_Last2                               0x4
#define DMAC_Last3                               0x8
#define DMAC_SIM2                                0x4
#define DMAC_SIM3                                0x8
#define DMAC_SEM2                                0x40
#define DMAC_SEM3                                0x80
#define DMAC_ACT2                                0x400
#define DMAC_ACT3                                0x800



/*******************************************************************************
* Function Name  : DMAC_Init
* Description    : Initialize the DMA Controller.
* Input          : DMAC_DataStream: select which Data Stream 
                  ( X can be 2  or 3) to configure
* Return         : None

*******************************************************************************/
void DMAC_Init(vu8 DMAC_DataStreamX);

/*******************************************************************************
* Function Name  : DMAC_SourceBaseAddressConfig
* Description    : This function is used to configures the source base address
*                  for the specified stream X source DMA buffer.

* Input1         : Data Stream (u8): select which Data Stream (2 or 3) 
                   to configure

* Input2         : DMAC_SAdd (u32):Source address  value to be written 
                   to the source base addr register  
* Return         : None
*******************************************************************************/

void DMAC_SourceBaseAddressConfig(vu8 DMAC_DataStreamX,vu32 DMAC_SAdd);

/******************************************************************************
* Function Name  : DMAC_DestinationBaseAddressConfig
* Description    : This function is used to configures the destination 
                   base address
*                  for the specified stream X source DMA buffer.

* Input1         : Data Stream (u8): select which Data Stream (2 or 3) 
                   to configure

* Input2         : DMAC_DAdd (u32):Source address  value to be written 
                   to the destination base addr register  
* Return         : None
*******************************************************************************/

void DMAC_DestinationBaseAddressConfig(vu8 DMAC_DataStreamX,vu32 DMAC_DAdd);

/*******************************************************************************
* Function Name  : DMAC_BufferSize
* Description    : This function is used to  set the maximum data unit count 
                   for the specified stream X which correspond to 
                   the buffer size.
* Input1         : Data Stream (u8): select which Data Stream (2 or 3)
                   to configure

* Input2         : DMAC_BufferSize: StreamX maximum data unit count defining 
                   the buffer size and must be written to the  maximum count 
                   registerX  
* Return         : None
*******************************************************************************/

void DMAC_BufferSize(vu8 DMAC_DataStreamX,vu16 DMAC_BufferSize) ;

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
* Input1         : Data Stream (u8): select which Data Stream (2 or 3) 
                   to configure

* Input2         : Config: value to be loaded in the appropiateDMAC register 
                   in order to configure the selected data stream.
* Return         : None

*******************************************************************************/

void DMAC_DMAConfig(vu8 DMAC_DataStreamX, vu16 Config);

/******************************************************************************
* Function Name  : DMAC_CurrentSourceAddress
* Description    : For each data streams (2 or 3), it returns the current value 
                   of the source address 
* Input          : Data Stream (u8): select which Data Stream (2 or 3)
                   to configure


* Return         : The current value of the source address pointer
                   related to streamX.

********************************************************************************/

u32 DMAC_CurrentSourceAddress(vu8 DMAC_DataStreamX);

/********************************************************************************
* Function Name  : DMAC_CurrentDestinationAddress
* Description    : For each data streams (2 or 3), it returns the current value 
                   of the destination address 
* Input          : Data Stream (u8): select which Data Stream (2 or 3) 
                   to configure


* Return         : The current value of the destination address pointer related 
                   to streamX.

********************************************************************************/

u32 DMAC_CurrentDestinationAddress(vu8 DMAC_DataStreamX);

/********************************************************************************
* Function Name  : DMAC_RemainingUnitNumber
* Description    : For each data streams (2 or 3), It returns the number of the 
                   data units remaining in the current DMA transfer.
* Input          : Data Stream (u8): select which Data Stream (2 or 3)
                   to configure

* Return         : It returns the number of the data units remaining 
                   in the current DMA transfer.

********************************************************************************/

u16 DMAC_RemainingUnitNumber(vu8 DMAC_DataStreamX);

/*******************************************************************************
* Function Name  : DMAC_LastBufferAddressConfig
* Description    : This function is used to configure the circular
                   buffer position where the last data to be used by the
                   specified stream X is located.
* Input1         : Data Stream (u8): select which Data Stream (2 or 3)
                   to configure.

* Input2         : DMAC_LastBufferAddress: Last buffer position addres to be written to
                   the Last Used Buffer location register.

* Return         : None
*******************************************************************************/

void DMAC_LastBufferAddressConfig(vu8 DMAC_DataStreamX, vu16 DMAC_LastBufferAddress);

/*******************************************************************************
* Function Name  : DMAC_ITConfig
* Description    : This function  configures the status flag that can generate 
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

void DMAC_ITConfig(  vu16 Flag ,FunctionalState NewState);


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

void DMAC_FlagClear(vu16 DMAC_Flag );

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

FlagStatus DMAC_FlagStatus(vu16 DMAC_Flag);

/*******************************************************************************
* Function Name  : DMAC_GetStatus
* Description    : This function  provides status information regarding 
                   the DMA Controller.
* Input          : DMAC_Flag:The flag to be read.Flag can be:
                                - DMAC_SIM2 : Stream 2 end Interrupt
                                - DMAC_SIM3 : Stream 3 end Interrupt
                                - DMAC_SEM2 : Stream 2 error Interrupt
                                - DMAC_SEM3 : Stream 3 error Interrupt
                                - DMAC_ACT2 : Stream 2 error Interrupt
                                - DMAC_ACT3 : Stream 3 error Interrupt
                 The user can select more than one flags,by OR’ing them.

* Return         : The DMAC status information

*******************************************************************************/
u16 DMAC_GetStatus(vu16 DMAC_Flag);

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

void DMAC_LastBufferSweep(vu16 DMAC_LASTX);

#endif // __720_DMAC_H

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
