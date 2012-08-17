/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_eic.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the EIC software functions.
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

/* Standard include -----------------------------------------------------------*/
#include "73x_eic.h"
#include "73x_cfg.h"


/* Include of other module interface headers ----------------------------------*/
/* Local includes -------------------------------------------------------------*/
/* Private typedef ------------------------------------------------------------*/
/* Private define -------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------*/
/* Interface functions --------------------------------------------------------*/

extern void ARMIRQ_Enable (void);   /* Imported from 73x_init.s Assembler file */
extern void ARMFIQ_Enable (void);   /* Imported from 73x_init.s Assembler file */
extern void ARMIRQ_Disable (void);  /* Imported from 73x_init.s Assembler file */
extern void ARMFIQ_Disable (void);  /* Imported from 73x_init.s Assembler file */

/* Private functions ---------------------------------------------------------*/

/******************************************************************************
* Function Name  : EIC_Init
* Description    : Initializes the EIC  peripheral according to the specified
*                  parameters in the EIC_InitTypeDef structure.
* Input          : EIC_InitStruct: pointer to a EIC_InitTypeDef structure that
*                  contains the configuration information for the EIC peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void EIC_Init(EIC_InitTypeDef* EIC_InitStruct)
{
  
   extern u32 PRCCUCMU_Addr;
   u8 Counter=0;
   u32 Offset = (u32)&PRCCUCMU_Addr;
   u32 Tmp=0;


  /* Initialize the EIC IVR and SIRn registers to be able                 */
  /* to recognize Interrupts    ------------------------------------------*/

  /* IVR = high half of load PC instruction (LDR PC,) */
  EIC->IVR  = 0xE59F0000;

  /* Read the offset of the interrupt vectors table address */
  Offset = (Offset+0x3E0)<<16;

  /* Initialize SIRn registers with the equivalent low half of load PC instruction */
  for(Counter=64; Counter!=0; Counter--)
  {
    EIC->SIRn[64-Counter] = Offset|0xF0000000;
    Offset += 0x00000004 << 16;
  }


  /* Enable or disable the select IRQ channel -------------------------*/
  if(EIC_InitStruct->EIC_IRQCmd == ENABLE)
  {
   /* Enable the selected IRQ channel*/
    if(EIC_InitStruct->EIC_IRQChannel<32)
    {
      EIC->IER0 |= 0x0001 << EIC_InitStruct->EIC_IRQChannel;
    }
    else
    {
      EIC->IER1 |= 0x0001 << (EIC_InitStruct->EIC_IRQChannel-32);
    }
  }
  else
  {
    /* Disable the select IRQ channel */
    if(EIC_InitStruct->EIC_IRQChannel<32)
    {
      EIC->IER0 &= ~(0x0001 << EIC_InitStruct->EIC_IRQChannel);
    }
    else
    {
      EIC->IER1 &= ~(0x0001 << (EIC_InitStruct->EIC_IRQChannel-32));
    }
  }

  /* Enable or disable the selected FIQ channel -------------------------*/
  if(EIC_InitStruct->EIC_FIQCmd == ENABLE)
  {
    /* Enable the selected FIQ channel*/
    EIC->FIR |= EIC_InitStruct->EIC_FIQChannel ;
  }
  else
  {
    /* Disable the selected FIQ channel */
    EIC->FIR &= ~EIC_InitStruct->EIC_FIQChannel;
  }

  /* Configure the selected IRQ channel priority */
  Tmp = (EIC->SIRn[EIC_InitStruct->EIC_IRQChannel]&0xFFFF0000) | ((u16)EIC_InitStruct->EIC_IRQChannelPriority & 0x000F);

  EIC->SIRn[EIC_InitStruct->EIC_IRQChannel] = Tmp;
}

/******************************************************************************
* Function Name  : EIC_DeInit
* Description    : Initializes the Interrupt Controller to be able with
*                  the str73x library architecture to handle th interrupt request.
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void EIC_DeInit(void)
{

  /*Reset EIC peripheral */
  CFG_PeripheralClockConfig(CFG_CLK_EIC,DISABLE);
  CFG_PeripheralClockConfig(CFG_CLK_EIC,ENABLE); 
}

/******************************************************************************
* Function Name  : EIC_StructInit
* Description    : Fills in a EIC_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : EIC_InitStruct : pointer to a EIC_InitTypeDef structure
                   which will be initialized.
* Output         : None
* Return         : None
******************************************************************************/
void EIC_StructInit(EIC_InitTypeDef* EIC_InitStruct)
{
  /* Initialize all the EIC_InitStruct members to 0*/
  EIC_InitStruct->EIC_IRQChannel = 0;
  EIC_InitStruct->EIC_FIQChannel = 0;
  EIC_InitStruct->EIC_IRQCmd = DISABLE;
  EIC_InitStruct->EIC_FIQCmd = DISABLE;
  EIC_InitStruct->EIC_IRQChannelPriority = 0;
}

/*******************************************************************************
* Function Name  : EIC_CurrentPriorityLevelConfig
* Description    : Change the current priority level of the served IRQ routine
* Input          : NewPriorityLevel: New current priority.
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_CurrentPriorityLevelConfig(u8 NewPriorityLevel)
{
  u32 Val = EIC->ICR;  /* Save ICR content */
  
  /* Disable IRQ interrupts */
  ARMIRQ_Disable();
  EIC->ICR &= ~0x0001;
  ARMIRQ_Enable();

  /* Change the current priority */
  EIC->CIPR = NewPriorityLevel;

  /* Restore ICR content */
  EIC->ICR = Val;
}

/*******************************************************************************
* Function Name  : EIC_IRQChannelConfig
* Description    : Enable or Disable the selected IRQ Channel
* Input          : - IRQChannel: the selected IRQ channel.
*                  - NewState: New state of the selected IRQ channel.
*                  This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_IRQChannelConfig(u8 IRQChannel, FunctionalState NewStatus)
{
  /* Enable or disable the select IRQ channel */
  if (NewStatus == ENABLE)
  {
   /* Enable the selected IRQ channel*/
    if(IRQChannel<32)
    {
      EIC->IER0 |= 0x0001 << IRQChannel;
    }
    else
    {
      EIC->IER1 |= 0x0001 << (IRQChannel-32);
    }
  }
  else
  {
    /* Disable the selected IRQ channel */
    if(IRQChannel<32)
    {
      EIC->IER0 &= ~(0x0001 << IRQChannel);
    }
    else
    {
      EIC->IER1 &= ~(0x0001 << (IRQChannel-32));
    }
  }
}

/*******************************************************************************
* Function Name  : EIC_ExternalITTriggerConfig
* Description    : Select the edge or the level in which the external interrupt
*                  is issued
* Input          : - EXTERNAL_IT: The selected External IRQ channel.
*                  - EXTIT_Trigger : the selected triggering edge or level
*                    of the external interrupt it can be one of the following value:
*                         - EXTIT_TRIGGER_Rising_Falling
*                         - EXTIT_TRIGGER_Rising
*                         - EXTIT_TRIGGER_Falling
*                         - EXTIT_TRIGGER_HIGH_Level
*                         - EXTIT_TRIGGER_LOW_Level
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_ExternalITTriggerConfig(u16 EXTERNAL_IT, EXTIT_TRIGGER_TypeDef EXTIT_Trigger)
{
  /*  Depending on the selected Edge or level and according to the External
   *  Interrupt Configuration Table Configure the External Interrupt Trigger
   *  Event Registers */
  switch (EXTIT_Trigger)
  {
    case EXTIT_TRIGGER_Rising_Falling:
      CFG->EITE0 &= ~ EXTERNAL_IT;
      CFG->EITE1 |=   EXTERNAL_IT;
      CFG->EITE2 |=   EXTERNAL_IT;
      break;

    case EXTIT_TRIGGER_Rising:
      CFG->EITE0 |=  EXTERNAL_IT;
      CFG->EITE1 &=~ EXTERNAL_IT;
      CFG->EITE2 |=  EXTERNAL_IT;
      break;

    case EXTIT_TRIGGER_Falling:
      CFG->EITE0 &=~ EXTERNAL_IT;
      CFG->EITE1 &=~ EXTERNAL_IT;
      CFG->EITE2 |=  EXTERNAL_IT;
      break;

    case EXTIT_TRIGGER_HIGH_Level:
      CFG->EITE0 |=  EXTERNAL_IT;
      CFG->EITE1 |=  EXTERNAL_IT;
      CFG->EITE2 &=~ EXTERNAL_IT;
      break;

    case EXTIT_TRIGGER_LOW_Level:
      CFG->EITE0 &=~ EXTERNAL_IT;
      CFG->EITE1 |=  EXTERNAL_IT;
      CFG->EITE2 &=~ EXTERNAL_IT;
      break;
  }
}


/*******************************************************************************
* Function Name  : EIC_IRQCmd
* Description    : Enables or disables EIC IRQ output request to CPU.
* Input          : NewState: new state of the EIC IRQ output request to CPU.
*                  This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_IRQCmd( FunctionalState NewState)
{
  
   ARMIRQ_Disable ();


  if (NewState == ENABLE)
  {
    /* Enable EIC IRQ output request to CPU*/
    EIC->ICR  |= EIC_IRQEnable_Mask;
  }
  else
  {
    /* Disable EIC IRQ output request to CPU*/
    EIC->ICR  &= EIC_IRQDisable_Mask;
  }

   ARMIRQ_Enable();

}

/*******************************************************************************
* Function Name  : EIC_FIQCmd
* Description    : Enables or disables EIC FIQ output request to CPU.
* Input          : NewState: new state of the EIC FIQ output request to CPU.
*                  This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_FIQCmd( FunctionalState NewState)
{
   ARMFIQ_Disable();
  

  if (NewState == ENABLE)
  {
    /* Enable EIC FIQ output request to CPU*/
    EIC->ICR  |= EIC_FIQEnable_Mask;
  }
  else
  {
    /* Disable EIC FIQ output request to CPU*/
    EIC->ICR  &= EIC_FIQDisable_Mask;
  }

  ARMFIQ_Enable();
}

/*******************************************************************************
* Function Name  : EIC_CurrentPriorityLevelValue
* Description    : Returns the current priority level of the current served IRQ
*                  routine
* Input          : None
* Output         : None
* Return         : The current priority level
*******************************************************************************/
u8 EIC_CurrentPriorityLevelValue(void)
{
  return EIC->CIPR;
}

/*******************************************************************************
* Function Name  : EIC_CurrentIRQChannelValue
* Description    : Returns the current served IRQ channel number
* Input          : None
* Output         : None
* Return         : The current served IRQ channel number
*******************************************************************************/
u8 EIC_CurrentIRQChannelValue(void)
{
  /* Reads and return the 6 LSB of the CICR register*/
  return (u8) (EIC->CICR);
}

/*******************************************************************************
* Function Name  : EIC_CurrentFIQChannelValue
* Description    : Returns the current served FIQ channel number.
* Input          : None
* Output         : None
* Return         : The current served FIQ channel number
*******************************************************************************/
u8 EIC_CurrentFIQChannelValue(void)
{
   /* Reads and return the FIP[1:0] bits */
   return (u8) (EIC->FIPR);
}

/*******************************************************************************
* Function Name  : EIC_FIQPendingBitClear
* Description    : Clears the FIQ pending bit of the selected FIQ Channel.
* Input          : FIQ channel : the selected FIQ channel to clear its
                   Correspondent pending bit
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_FIQPendingBitClear(u8 FIQChannel)
{
  /* Clear the correspondent FIP[1:0] bit of the FIQChannel */
  EIC->FIPR   = FIQChannel ;
}

/*******************************************************************************
* Function Name  : EIC_FIQChannelConfig
* Description    : Configures the FIQ Channel.
* Input          : - FIQChannel: the selected channel to change its status
                   - NewStatus : the new status of FIQ channel it can be
                     ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_FIQChannelConfig(u8 FIQChannel,FunctionalState NewStatus)
{
  /* Enable or disable the selected FIQ channel */
  if (NewStatus == ENABLE)
  {
    /* Enable the selected FIQ channel*/
    EIC->FIR |= FIQChannel ;
  }
  else
  {
    /* Disable the selected FIQ channel */
    EIC->FIR &= ~FIQChannel;
  }
}

/*******************************************************************************
* Function Name  : EIC_IRQChannelPriorityConfig
* Description    : Configures the selected IRQ channel priority.
* Input          : - IRQChannel: the selected IRQ channel.
*                  - Priority:  the priority to be configured for the IRQchannel
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_IRQChannelPriorityConfig(u8 IRQChannel, u8 Priority)
{
  /* Configure the priority by setting the SIPL[3:0] of the correspondent SIRn register*/
  EIC->SIRn[IRQChannel] = (EIC->SIRn[IRQChannel]&0xFFFF0000) | (u16)Priority & 0x000F;
}

/*******************************************************************************
* Function Name  : EIC_ExternalITFilterConfig
* Description    : Enables or disables a digital filter on external interrupt.
*                 channels INT[15:0]
* Input          : - NewStatus : the new status of the digital filter  it can be
                     ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_ExternalITFilterConfig(FunctionalState NewStatus)
{  
  if (NewStatus == ENABLE)
  {
    /* Enable the digital filter */
    CFG->R1 |= EIC_EXTIT_FILTER_Mask;
  }
  else
  {
    /* Disable the digital filter */
    CFG->R1 &= ~EIC_EXTIT_FILTER_Mask;
  }
}


/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/


