/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_eic.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the Enhanced Interrupt Controller
*                      software functions headers
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

#ifndef __720_EIC_H
#define __720_EIC_H

/* Standard include ----------------------------------------------------------*/
/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "720_lib.h"

// IRQ Channels

typedef enum
  {
   EINT03_IRQChannel   =  0 ,    // External Interrupts EINT0-EINT3
   EINT4_IRQChannel    =  1 ,    // External Interrupt EINT4
   WIU_IRQChannel      =  2 ,    // External Interrupt management IRQ
   EFT1_IRQChannel     =  3 ,    // EFT1 globlal interrupt
   EFT2_IRQChannel     =  4 ,    // EFT2 global interrupt
   UART1_IRQChannel    =  5 ,    // UART 1 global interrupt
   UART2_IRQChannel    =  6 ,    // UART 2 global interrupt
   DMA_IRQChannel      =  7 ,    // DMA event global interrupt
   DMAS0_IRQChannel    =  8 ,    // DMA stream 0 end of transfer interrupt
   DMAS1_IRQChannel    =  9 ,    // DMA Stream 1 end of transfer interrupt
   USBHp_IRQChannel    =  10 ,   // USB High priority event interrupt
   USBLp_IRQChannel    =  11 ,   // USB Low priority event interrupt
   CAN_IRQChannel      =  12 ,   // CAN module general interrupt
   BSPI1_IRQChannel    =  13 ,   // BSPI 1 global interrupt
   BSPI2_IRQChannel    =  14 ,   // BSPI 2 Global interrupt
   IDE_IRQChannel      =  15 ,   // IDE Primary channel interrupt
   RTC_IRQChannel      =  16 ,   // RTC periodic interrupt
   ADC_IRQChannel      =  23 ,   // ADC sample ready interrupt
   EFT2OCA_IRQChannel  =  29 ,   // EFT 2 Output Compare A interrupt
   EFT2OCB_IRQChannel  =  30 ,   // EFT 2 Ouptut Compare B interrupt
   WDG_IRQChannel      =  31     // WatchDog Timer interrupt
   }EIC_IRQChannel;

// FIQ Channels

typedef enum
  {
  EINT_FIQChannel     = 0x1,          // External Interrupt FIQ
  EFT2OCB_FIQChannel  = 0x2,          // EFT 2 Ouptut Compare B interrupt
  AHBError_FIQChannel = 0x4          // AHB Error Detection interrupt
  }EIC_FIQChannel;



/*******************************************************************************
* Function Name  : EIC_Init
* Description    : Initialise the EIC using the load PC instruction
*                 (PC = PC +offset)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_Init(void);
/*******************************************************************************
* Function Name  : EIC_IRQConfig
* Description    : Enable or Disable IRQ interrupts
* Input          : NewState: specifies the status of the IRQ interrupts
*                      ENABLE:  IRQ interrupts enabled
*                      DISABLE: IRQ interrupts disabled
* Output         : None
* Return         : None
*******************************************************************************/
static inline void EIC_IRQConfig (FunctionalState NewState)
{
  EIC->ICR = (NewState==ENABLE)? (EIC->ICR|0x0001):(EIC->ICR&~0x0001);
}

/*******************************************************************************
* Function Name  : EIC_FIQConfig
* Description    : Enable or disable the FIQ interrupts
* Input 1        : NewState: specifies the status of the FIQ interrupts
*                        ENABLE:  to enable FIQ inerrupts
*                        DISABLE: to disable FIQ inerrupts
* Output         : None
* Return         : None
*******************************************************************************/
static inline void EIC_FIQConfig (FunctionalState NewState)
{
  EIC->ICR= (NewState==ENABLE)? (EIC->ICR|0x0002):(EIC->ICR&~0x0002);
}
/*******************************************************************************
* Function Name  : EIC_IRQChannelConfig
* Description    : Enables or disables the IRQ of the specified channel.
* Input 1        : IRQChannel: channel (refer to the masks of the
*                              IRQ Channel defined above
*                              for details on its values).
*                  More than one channel can be specified
* Input 2        : Channel IRQ New State
*                      ENABLE:  to enable the IRQ on the specified channel
*                      DISABLE: to disable the IRQ on the specified channel
* Output         : None
* Return         : None
*******************************************************************************/
static inline void EIC_IRQChannelConfig (EIC_IRQChannel IRQChannel, FunctionalState NewState)
{
  EIC->IER = (NewState==ENABLE) ?  (EIC->IER | 0x0001 << IRQChannel):\
                                   (EIC->IER & ~(0x0001 << IRQChannel));
}

/*******************************************************************************
* Function Name  : EIC_FIQChannelConfig
* Description    : Enable or Disable the FIQ interrupt of the specified
*                  channel.
* Input 1        : FIQChannel: the FIQ channel (refer to the values of the
*                                               FIQ Channel defined above
*                                               for details on its values)
*                  More than one channel can be specified.
* Input 2        : Channel FIQ New State
*                       ENABLE:  to enable the FIQ of the specified channel.
*                       DISABLE: to disable the FIQ if the specified channel.
* Output         : None
* Return         : None
*******************************************************************************/
static inline void EIC_FIQChannelConfig (EIC_FIQChannel FIQChannel, FunctionalState NewState)
{
	EIC->FIR=(NewState==ENABLE)?(EIC->FIR|FIQChannel):(EIC->FIR&~FIQChannel);
}

/*******************************************************************************
* Function Name  : EIC_IRQChannelPriorityConfig
* Description    : Configure the selected IRQ channel priority
* Input 1        : IRQChannel: the IRQ channel to set the priority
*                              Refer to the values of the IRQ channels defined
*                              above
* Input 2        : Priority: the specified IRQ channel priority
* Output         : None
* Return         : None
*******************************************************************************/
static inline void EIC_IRQChannelPriorityConfig (EIC_IRQChannel IRQChannel, vu8 Priority)
{
	EIC->SIR[IRQChannel]=(EIC->SIR[IRQChannel]&0xFFFF0000)|(u16)(Priority & 0x000F);
}

/*******************************************************************************
* Function Name  : EIC_CurrentPriorityLevelValue
* Description    : Return the current priority level of the current served IRQ
*                  routine
* Input          : None
* Output         : None
* Return         : The current priority level
*******************************************************************************/
static  inline u8 EIC_CurrentPriorityLevelValue (void)
{
  return EIC->CIPR;
}
/*******************************************************************************
* Function Name  : EIC_CurrentPriorityLevelConfig
* Description    : Change the current priority level of the served IRQ routine
* Input          : NewPriorityLevel: the new priority value
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_CurrentPriorityLevelConfig (vu8 NewPriorityLevel);

/*******************************************************************************
* Function Name  : EIC_CurrentIRQChannelValue
* Description    : return the current served IRQ channel number
* Input          : None
* Output         : None
* Return         : The current served IRQ channel number
*******************************************************************************/
static inline u8 EIC_CurrentIRQChannelValue (void)
{
  return EIC->CICR;
}

/*******************************************************************************
* Function Name  : EIC_CurrentFIQChannelValue
* Description    : return the current served FIQ channel number
* Input 1        : None
* Output         : None
* Return         : The current served FIQ channel number
*******************************************************************************/
static inline u8 EIC_CurrentFIQChannelValue (void)
{
   return (EIC->FIR>>3)&0x0007;
}

/*******************************************************************************
* Function Name  : EIC_FIQPendingBitClear
* Description    : Clear the FIQ pending bit
* Input 1        : FIQ channel (Refer to the FIQ channels definition above for
*                               for more details on the allowed values of this
*                               parameter)
*                  More than one channed can be specified
* Output         : None
* Return         : None
*******************************************************************************/
static inline void EIC_FIQPendingBitClear (EIC_FIQChannel FIQChannel)
{
  EIC->FIR=(EIC->FIR&0x0007)|(FIQChannel<<3);
}
#endif /* __720_EIC_H */
/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
