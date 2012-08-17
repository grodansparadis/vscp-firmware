/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_rtc.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the RTC software functions headers
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

#ifndef __720_RTC_H
#define __720_RTC_H

/* Includes ------------------------------------------------------------------*/

#include "720_lib.h"

// Configuration Flag Mask

#define CNF_Mask  0x0010 
#define GI_Mask   0x0800

// Operation OFF flag

#define RTOFF_Mask   0x0020

/* Exported types ------------------------------------------------------------*/

typedef struct
{
  u16 RegH;
  u16 RegM;
  u16 RegL;
} u48;

typedef enum
{
  RTC_PERIR = 0x0004,
  NONE      = 0
} RTC_FLAGS ;

typedef enum
{
  RTC_PERIT = 0x0400,
  RTC_WRIT  = 0x0040
} RTC_IT ;

typedef enum
{
  RTC_OD = 0x0080,
  RTC_PP = 0xFF7F
} RTC_Mode ;

/*******************************************************************************
* Function Name  : RTC_PeriodicValue
* Description    : This routine is used to get the RTC periodic value
* Input          : None
* Return         : an u32 value that holds the periodic value
*******************************************************************************/
u32 RTC_PeriodicValue(void);
/*******************************************************************************
* Function Name  : RTC_PeriodicValueConfig
* Description    : This routine is used to set the RTC periodic Value
* Input          : an u32 value that holds the Real Time clock period time
* Return         : None
*******************************************************************************/
void RTC_PeriodicValueConfig(vu32 XPeriod);
/*******************************************************************************
* Function Name  : RTC_CounterValue
* Description    : This routine is used to get the RTC counter value
* Input          : None
* Return         : an u48 struct of value that holds the current counter value
*******************************************************************************/
u48 RTC_CounterValue(void);
/*******************************************************************************
* Function Name  : RTC_CounterConfig
* Description    : This routine is used to configure the RTC counter value
* Input          : the RTC New Counter Value
* Return         : None
*******************************************************************************/
void RTC_CounterConfig(u48 NewTime);
/*******************************************************************************
* Function Name  : RTC_AlarmValue
* Description    : This routine is used to get the RTC alarm Value
* Input          : None
* Return         : An u48 struct of value that holds the RTC alarm time
*******************************************************************************/
u48 RTC_AlarmValue(void);
/*******************************************************************************
* Function Name  : RTC_AlarmConfig
* Description    : This routine is used to set the RTC alarm Value
* Input          : an u48 value that holds the Real Time clock alarm time
* Return         : None
*******************************************************************************/
void RTC_AlarmConfig(u48 Xalarm);
/*******************************************************************************
* Function Name  : RTC_FlagStatus
* Description    : This routine is used to chek the RTC flag status
* Input          : an RTC flag
* Return         : Set or RESET
*******************************************************************************/
FlagStatus RTC_FlagStatus(RTC_FLAGS Xflag);
/*******************************************************************************
* Function Name  : RTC_FlagClear
* Description    : This routine is used to clear the RTC flags
* Input          : The flag to be cleared
* Return         : None
*******************************************************************************/
void RTC_FlagClear(RTC_FLAGS Xflag);
/*******************************************************************************
* Function Name  : RTC_ITConfig
* Description    : This routine is used to configure the RTC interrupts
* Input 1        : An RTC interrupt
* Input 2        : Enable or Disable
* Return         : None
*******************************************************************************/
void RTC_ITConfig(RTC_IT Xrtcit, FunctionalState NewState);
/*******************************************************************************
* Function Name  : RTC_ITStatus
* Description    : This routine is used to get the RTC interrupts status
* Input          : an RTC interrupt
* Return         : Enable or Disable
*******************************************************************************/
FunctionalState RTC_ITStatus(RTC_IT Xrtcit);
/*******************************************************************************
* Function Name  : RTC_ITClear
* Description    : This routine is used to clear the RTC interrupts
* Input          : an RTC interrupt
* Return         : None
*******************************************************************************/
void RTC_ITClear(RTC_IT Xrtcit);
/*******************************************************************************
* Function Name  : RTC_WakeUpMode
* Description    : This routine is used  to configure the opertaing mode of the
*                  wake-up pad
* Input          : Xrtcmode designates the mode of the wake-up pad and it can be:
*		     RTC_OD: The wake-up request pin operates in open-drain mode
*		     RTC_PP: The wake-up request pin operates in push-pull mode
* Return         : None
*******************************************************************************/
void RTC_WakeUpMode(RTC_Mode Xrtcmode);
/*******************************************************************************
* Function Name  : RTC_EnterCfgMode
* Description    : This routine is used to enter in the Configuration Mode
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_EnterCfgMode(void);
/*******************************************************************************
* Function Name  : RTC_ExitCfgMode
* Description    : This routine is used to exit from the Configuration Mode
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_ExitCfgMode(void);
/*******************************************************************************
* Function Name  : RTC_WaitForLastTask
* Description    : This routine is waits for the last task completetion
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_WaitForLastTask(void);


#endif

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/

