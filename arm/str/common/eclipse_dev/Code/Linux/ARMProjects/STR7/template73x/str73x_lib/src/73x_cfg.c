/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_cfg.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the CFG software functions.
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
* Function Name  : CFG_PeripheralClockConfig
* Description    : Switch on/off separately the clock of each module of the device.
*                  in case of the switch off the peripheral is kept under reset
* Input          : - CFG_CLK_Periph : the peripheral to enable or disable its clock.
*                  - NewStatus : the new status it can be ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void CFG_PeripheralClockConfig(u8 CFG_CLK_Periph, FunctionalState NewStatus)
{
  u8 delay = 0 ; 
  if (NewStatus == ENABLE)
  {
   /* Enable the clock of the selected module */
    if(CFG_CLK_Periph < 32)
    {
      CFG->PCGR0 |= 0x0001 << CFG_CLK_Periph;
    }
    else
    {
      CFG->PCGR1 |= 0x0001 << (CFG_CLK_Periph-32);
    }
  }
  else
  {
   /* Disable the clock of the selected module */
    if(CFG_CLK_Periph < 32)
    {
      CFG->PCGR0 &= ~(0x0001 << CFG_CLK_Periph);
    }
    else
    {
      CFG->PCGR1 &= ~(0x0001 << (CFG_CLK_Periph-32));
    }
  }

  for (delay = 8 ; delay != 0 ; delay--); /* Peripheral Stabilization */
}

/*******************************************************************************
* Function Name  : CFG_EmulationPeripheralClockConfig
* Description    : Switch on/off separately the clock of each module of the device
*                  in case of the emulation mode.
* Input          : - CFG_CLK_Periph : the peripheral to enable or disable its clock.
*                  - NewStatus : the new status, it can be ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void CFG_EmulationPeripheralClockConfig(u8 CFG_CLK_Periph, FunctionalState NewStatus)
{
   u8 delay =0 ; 

  if (NewStatus == ENABLE)
  {
   /* Enable the clock of the selected module */
    if(CFG_CLK_Periph < 32)
    {
      CFG->PECGR0 |= 0x0001 << CFG_CLK_Periph;
    }
    else
    {
      CFG->PECGR1 |= 0x0001 << (CFG_CLK_Periph-32);
    }
  }
  else
  {
   /* Disable the clock of the selected module */
    if(CFG_CLK_Periph < 32)
    {
      CFG->PECGR0 &= ~(0x0001 << CFG_CLK_Periph);
    }
    else
    {
      CFG->PECGR1 &= ~(0x0001 << (CFG_CLK_Periph-32));
    }
  }

  for (delay = 8 ; delay !=0 ; delay--); /* Peripheral Stabilization */
}

/*******************************************************************************
* Function Name  : CFG_RemapConfig
* Description    : Remaps RAM or FLASH to address 0h.
* Input          : - CFG_Mem:the memory to be remapped to address 0h. it can be
*                    one of the following value: CFG_MEM_RAM or CFG_MEM_FLASH
* Output         : None
* Return         : None
*******************************************************************************/
void CFG_RemapConfig(CFG_MEM_TypeDef CFG_Mem )
{
  /* Depending on the memory to be remapped to address 0h*/
  if (CFG_Mem == CFG_MEM_RAM)
  {
    /* Set the REMAP bit in the Configuration Register 0*/
    CFG->R0 |= CFG_MEM_RAM;
  }
  else
  {
    /* Reset the REMAP bit in the Configuration Register 0*/
    CFG->R0 &= ~CFG_MEM_RAM;
  }
}

/*******************************************************************************
* Function Name  : CFG_PeripheralClockStop
* Description    : Switch off separately the clock of each module of the device.
*                  in this case of the switch off the pripheral configuration
*                  is conserved
* Input          : - CFG_CLK_Periph : the peripheral to stop the clock.
* Output         : None
* Return         : None
*******************************************************************************/
void CFG_PeripheralClockStop(u8 CFG_CLK_Periph)
{
   
  /* STOP the clock of the selected module without resetting its configuration */
  if (CFG_CLK_Periph < 32 )
  {
    CFG->PCGRB0 |= 0x0001 << CFG_CLK_Periph;
  }
  else
  {
    CFG->PCGRB1 |= 0x0001 << (CFG_CLK_Periph-32);
  }

}

/*******************************************************************************
* Function Name  : CFG_PeripheralClockStart
* Description    : Switch On separately the clock of each module of the device.
*                  in this case of the switch off the pripheral configuration
*                  is conserved
* Input          : - CFG_CLK_Periph : the peripheral to start the clock.
* Output         : None
* Return         : None
*******************************************************************************/
void CFG_PeripheralClockStart(u8 CFG_CLK_Periph)
{
   u8 delay =0 ; 

  /* Start the clock of the selected module */
  if (CFG_CLK_Periph < 32 )
  {
    CFG->PCGRB0 &= ~(0x0001 << CFG_CLK_Periph);
  }
  else
  {
    CFG->PCGRB1 &= ~(0x0001 << (CFG_CLK_Periph-32));
  }

  for (delay = 8 ; delay != 0 ; delay--); /* Peripheral Stabilization */
}

/*******************************************************************************
* Function Name  : CFG_FlagStatus
* Description    : Checks whether the specified CFG flag is set or not.
* Input          : - CFG_Flag: flag to check. This parameter can be one of the
*                    following values:
*                        - CFG_FLAG_SYS
*                        - CFG_FLAG_BOOT
*                        - CFG_FLAG_USER1
*                        - CFG_FLAG_USER2
*                        - CFG_FLAG_JTBT
* Output         : None
* Return         : The new state of the CFG_Flag (SET or RESET).
*******************************************************************************/
FlagStatus CFG_FlagStatus(u16 CFG_Flag)
{
  /* Check whether CFG_Flag is set or not */
  if((CFG->R0 & CFG_Flag) != 0)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/*******************************************************************************
* Function Name  : CFG_DeviceID
* Description    : Returns the device IDentification number.
* Input          : None
* Output         : None
* Return         : The DeviceID
*******************************************************************************/
u32 CFG_DeviceID(void)
{
 
    return CFG->DIDR ;
  
}

/*******************************************************************************
* Function Name  : CFG_FlashPowerOnDelay
* Description    : Sets the delay on Flash PowerOn when the device exits from 
*                  Low Power WFI mode.
* Input          : Delay (0x0..0xF)
* Output         : None
* Return         : None
*******************************************************************************/
void CFG_FlashPowerOnDelay(u8 Delay)
{
 
    u32 tmp = CFG->R1 & 0xFFFFFF0F;
    
    CFG->R1 = tmp | ((Delay & 0xF)<< 4) ;
  
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
