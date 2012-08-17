/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_wdg.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the WDG software functions headers
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

#ifndef __720_WDG_H
#define __720_WDG_H
/* Includes ------------------------------------------------------------------*/
#include "720_lib.h"
#include "stdlib.h"
#ifndef _RCCU
  #error _RCCU Must be Defined in "720_conf.h"
#endif
/* Exported types ------------------------------------------------------------*/
typedef enum 
{ 
  WDG_START,
  WDG_STOP
}WDG_CounterOperations;

/*******************************************************************************
* Function Name  : WDG_Enable
* Description    : Enable the Watchdog mode or the Timer mode.
*                : 
* Input          : ENABLE: to enable the watchdog mode
*		 : DISABLE: to enable the timer mode.
*                :
* Return         : None
*******************************************************************************/
void WDG_Enable ( FunctionalState NewState );
/*******************************************************************************
* Function Name  : WDG_CntRefresh
* Description    : Refresh and update the WDG counter to avoid a system reset.
* Input          : None
* Return         : None
*******************************************************************************/
void WDG_CntRefresh ( void );
/*******************************************************************************
* Function Name  : WDG_PrescalerConfig
* Description    : Set the counter prescaler value.
* Input          : 8 bit data
* Return         : None
*******************************************************************************/
void WDG_PrescalerConfig ( vu8 Xprescaler );
/*******************************************************************************
* Function Name  : WDG_CntReloadUpdate
* Description    : Update the counter pre-load value.
* Input          : 16 bit data
* Return         : None
*******************************************************************************/
void WDG_CntReloadUpdate ( vu16 PreLoadValue );
/*******************************************************************************
* Function Name  : WDG_PeriodValueConfig
* Description    : Set the prescaler and counter reload value based on a given time.
* Input          : Amount of time (us) needed
* Return         : None
*******************************************************************************/
void WDG_PeriodValueConfig ( vu32 Time );
/*******************************************************************************
* Function Name  : WDG_CntOnOffConfig
* Description    : Start or stop the free auto-reload timer to count down.
*                :
* Input          : START to start the counter
*                : STOP to stop the counter
*                :
* Return         : None
*******************************************************************************/
void WDG_CntOnOffConfig ( WDG_CounterOperations Xoperation );
/*******************************************************************************
* Function Name  : WDG_ECITConfig
* Description    : Enable or disable the end of count interrupt
*                :
* Input          : ENABLE to enable the end of count interrupt
*                : DISABLE to disable the end of count interrupt
*                :
* Return         : None
*******************************************************************************/
void WDG_ECITConfig ( FunctionalState NewState );
/*******************************************************************************
* Function Name  : WDG_ECFlagClear
* Description    : Clear the end of count flag 
* Input          : None
* Return         : None
*******************************************************************************/
void WDG_ECFlagClear ( void );
/*******************************************************************************
* Function Name  : WDG_ECStatus
* Description    : Return the end of count status 
* Input          : None
* Return         : u16
*******************************************************************************/
FlagStatus WDG_ECStatus ( void );

#endif /* __720_WDG_H */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
