/******************** (C) COPYRIGHT 2004 STMicroelectronics *******************
* File Name          : 720_eic.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the Enhanced Interrupt Controller
*                      functions source code
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

/* Standard include ----------------------------------------------------------*/
#include "720_eic.h"
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
* Function Name  : EIC_Init
* Description    : Initialise the EIC using the load PC instruction
*                 (PC = PC +offset)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_Init(void)
{
  extern u32 EXT03IT_Addr;
  u8 bCounter;
  u32 dOffset=(u32)&EXT03IT_Addr;

  EIC->ICR=0x00000000;              // Disable FIQ and IRQ interrupts

  EIC->IER=0x00000000;              // Disable all channels interrupts

  EIC->IPR=0xFFFFFFFF;              // Clear all pending bits

  EIC->FIR=0x00000038;              // Disable all FIQ channels' interrupts and
                                    // clear FIQ channels pending bits

  EIC->CIPR=0x00000000;             // Set the current priority level to zero

  EIC->IVR=0xE59F0000;              // IVR = high half of load pc,pc,#offset
                                    // instruction

  dOffset = (dOffset+0x000003E0)<<16;

                                    // Read the interrupt vectors table
                                    // addresses

  for(bCounter=0;bCounter<32;bCounter++)
  {
    EIC->SIR[bCounter]=dOffset|0xF0000000;
                                    // Initialize SIRn registers
    dOffset+=0x00000004 << 16;
  }
}


/*******************************************************************************
* Function Name  : EIC_CurrentPriorityLevelConfig
* Description    : Change the current priority level of the served IRQ routine
* Input          : NewPriorityLevel: the new priority value
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_CurrentPriorityLevelConfig (vu8 NewPriorityLevel)
{
  u32 dVal;
  dVal=EIC->ICR;                // Save ICR content
  EIC->ICR&=0xFFFE;             // Disable IRQ interrupts.
  EIC->CIPR=NewPriorityLevel;   // Change the current priority
  EIC->ICR=dVal;                // Restore ICR content
}
/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
