/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_rtc.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the RTC software functions
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
#include "720_rtc.h"
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
* Function Name  : RTC_PeriodicValue
* Description    : This routine is used to get the RTC periodic value
* Input          : None
* Return         : an u32 value that holds the periodic value
*******************************************************************************/
u32 RTC_PeriodicValue(void)
{
  // Return the periodic Value
  return ((RTC->PH & 0x000F) << 16) | RTC->PL;
}
/*******************************************************************************
* Function Name  : RTC_PeriodicValueConfig
* Description    : This routine is used to set the RTC periodic Value
* Input          : an vu32 value that holds the Real Time clock period time
* Return         : None
*******************************************************************************/
void RTC_PeriodicValueConfig(vu32 XPeriod)
{
  // Enter In Configuration Mode
  RTC_EnterCfgMode();
  // Set The MSB part of the periodic Time
  RTC->PH = ((XPeriod & 0x000F0000) >> 16);
  // Set The LSB part of the periodic Time
  RTC->PL = ((XPeriod & 0x0000FFFF));
  // Exit From Configuration Mode
  RTC_ExitCfgMode();
}
/*******************************************************************************
* Function Name  : RTC_CounterValue
* Description    : This routine is used to get the RTC counter value
* Input          : None
* Return         : an u48 struct of value that holds the current counter value
*******************************************************************************/
u48 RTC_CounterValue(void)
{
  u48 counter ;
  // Get the counter Value
  counter.RegH = RTC->CNTH;
  counter.RegM = RTC->CNTM;
  counter.RegL = RTC->CNTL;
  // Retuen the counter Value
  return (counter);
}
/*******************************************************************************
* Function Name  : RTC_CounterConfig
* Description    : This routine is used to configure the RTC counter value
* Input          : the RTC New Counter Value
* Return         : None
*******************************************************************************/
void RTC_CounterConfig(u48 NewTime)
{
  // Enter In Configuration Mode
  RTC_EnterCfgMode();

  // Store the New Time parameters 
  RTC->CNTH = NewTime.RegH;
  RTC->CNTM = NewTime.RegM;
  RTC->CNTL = NewTime.RegL;

  // Exit From Configuration Mode
  RTC_ExitCfgMode();
}
/*******************************************************************************
* Function Name  : RTC_AlarmValue
* Description    : This routine is used to get the RTC alarm Value
* Input          : None
* Return         : An u48 struct of value that holds the RTC alarm time
*******************************************************************************/
u48 RTC_AlarmValue(void)
{
  u48 Alarm ;
  // Get The Alarm time Value
  Alarm.RegH = RTC->AH;
  Alarm.RegM = RTC->AM;
  Alarm.RegL = RTC->AL;
  // Return The Alarm time Value
  return (Alarm);
}
/*******************************************************************************
* Function Name  : RTC_AlarmConfig
* Description    : This routine is used to set the RTC alarm Value
* Input          : an u48 value that holds the Real Time clock alarm time
* Return         : None
*******************************************************************************/
void RTC_AlarmConfig(u48 Xalarm)
{
  // Enter In Configuration Mode
  RTC_EnterCfgMode();

  // Set The MSB part of the Alarm Time
  RTC->AH = Xalarm.RegH;
  // Set The Middle part of the Alarm Time
  RTC->AM = Xalarm.RegM;
  // Set The LSB part of the Alarm Time
  RTC->AL = Xalarm.RegL;

  // Exit From Configuration Mode
  RTC_ExitCfgMode();
}

/*******************************************************************************
* Function Name  : RTC_FlagStatus
* Description    : This routine is used to chek the RTC flag status
* Input          : an RTC flag
* Return         : Set or RESET
*******************************************************************************/
FlagStatus RTC_FlagStatus(RTC_FLAGS Xflag)
{
  // Return The Flag Status
  return ((( RTC->CR & Xflag) == 0)?  (RESET) : (SET));
}
/*******************************************************************************
* Function Name  : RTC_FlagClear
* Description    : This routine is used to clear the RTC flags
* Input          : The flag to be cleared
* Return         : None
*******************************************************************************/
void RTC_FlagClear(RTC_FLAGS Xflag)
{
  // Enter In Configuration Mode
  RTC_EnterCfgMode();

  // Clear an RTC flag
  RTC->CR &= ~Xflag;

  // Exit From Configuration Mode
  RTC_ExitCfgMode();
}
/*******************************************************************************
* Function Name  : RTC_ITConfig
* Description    : This routine is used to configure the RTC interrupts
* Input 1        : An RTC interrupt
* Input 2        : Enable or Disable
* Return         : None
*******************************************************************************/
void RTC_ITConfig(RTC_IT Xrtcit, FunctionalState NewState)
{
  // Enter In Configuration Mode
  RTC_EnterCfgMode();

  // Configures The RTC interrupts.
  RTC->CR =(NewState == ENABLE)? (RTC->CR | Xrtcit) : (RTC->CR & (~Xrtcit));

  // Exit From Configuration Mode
  RTC_ExitCfgMode();
}
/*******************************************************************************
* Function Name  : RTC_ITStatus
* Description    : This routine is used to get the RTC interrupts status
* Input          : an RTC interrupt
* Return         : Enable or Disable
*******************************************************************************/
FunctionalState RTC_ITStatus(RTC_IT Xrtcit)
{
  // Return The RTC interrupt status
  return ((RTC->CR & Xrtcit) == 0)? (DISABLE) : (ENABLE);
}
/*******************************************************************************
* Function Name  : RTC_ITClear
* Description    : This routine is used to clear the RTC interrupts
* Input          : an RTC interrupt
* Return         : None
*******************************************************************************/
void RTC_ITClear(RTC_IT Xrtcit)
{
  // Enter In Configuration Mode
  RTC_EnterCfgMode();

  // Clears an RTC interrupt
  RTC->CR &= ~Xrtcit;

  // Exit From Configuration Mode
  RTC_ExitCfgMode();
}
/*******************************************************************************
* Function Name  : RTC_WakeUpMode
* Description    : This routine is used  to configure the opertaing mode of the
*                  wake-up pad
* Input          : Xrtcmode designates the mode of the wake-up pad and it can be:
*		     RTC_OD: The wake-up request pin operates in open-drain mode
*		     RTC_PP: The wake-up request pin operates in push-pull mode
* Return         : None
*******************************************************************************/
void RTC_WakeUpMode(RTC_Mode Xrtcmode)
{
  // Enter In Configuration Mode
  RTC_EnterCfgMode();

  RTC->CR = (Xrtcmode == RTC_OD)? (RTC->CR | Xrtcmode) : (RTC->CR & Xrtcmode);

  // Exit From Configuration Mode
  RTC_ExitCfgMode();
}
/*******************************************************************************
* Function Name  : RTC_EnterCfgMode
* Description    : This routine is used to enter in the Configuration Mode
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_EnterCfgMode(void)
{
  // Set the CNF flag to enter in the Configuration Mode
  RTC->CR |= CNF_Mask;
}
/*******************************************************************************
* Function Name  : RTC_ExitCfgMode
* Description    : This routine is used to exit from the Configuration Mode
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_ExitCfgMode(void)
{
  // Reset the CNF flag to exit from the Configuration Mode
  RTC->CR &= ~CNF_Mask;

   // Wait For Last Task Completition
  RTC_WaitForLastTask();
}
/*******************************************************************************
* Function Name  : RTC_WaitForLastTask
* Description    : This routine is waits for the last task completetion
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_WaitForLastTask(void)
{
  // Loop until the Last operation completition
  while ((RTC->CR & RTOFF_Mask) == 0);
}


/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
