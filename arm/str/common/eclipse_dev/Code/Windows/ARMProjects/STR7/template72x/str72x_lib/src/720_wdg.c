/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_wdg.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the WDG software functions 
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
#include "720_wdg.h"
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


/*******************************************************************************
* Function Name  : FindFactors
* Description    : Search for the best (a,b) values that fit n = a*b
*                  with the following constraints: 1<=a<=256, 1<=b<=65536
* Input 1        : n: the number to decompose
* Input/Output 2 : a: a pointer to the first factor
* Input/Output 3 : b: a pointer to the second factor
* Return         : None
*******************************************************************************/
static void FindFactors(u32 n, u32 *a, u32 *b)
{
  s32 b0;  s32 a0;  s32 err, err_min=0xFFFF;
 *a = a0 = ((n-1)/65536) + 1;
 *b = b0 = n / *a;

 for (; *a <= 256; (*a)++)
 {
  *b = n / *a;
  err = *a * *b - n;

  if (abs(err) > (*a / 2))
  {
   (*b)++;
   err = *a * *b - n;
  }

  if (abs(err) < abs(err_min))
  {
   err_min = err;
   a0 = *a;
   b0 = *b;
   if (err == 0) break;
  }
 }
 *a = a0;
 *b = b0;
}
/*******************************************************************************
* Function Name  : WDG_PeriodValueConfig
* Description    : Set the prescaler and counter reload value
* Input          : Amount of time (us) needed
* Return         : None
*******************************************************************************/
void WDG_PeriodValueConfig(vu32 Time)
{
  u32 a;
  u32 n,b;

  n = Time * (RCCU_GetFrequency(RCCU_AAPBClock) / 1000000);

  FindFactors(n, &a, &b);
  WDG->PR = a - 1;
  WDG->VR = b - 1;
}
/*******************************************************************************
* Function Name  : WDG_Enable
* Description    : Enable the Watchdog mode or the Timer mode.
*                :
* Input          : ENABLE: to enable the watchdog mode
*		 : DISABLE: to enable the timer mode.
*                :
* Return         : None
*******************************************************************************/
void WDG_Enable(FunctionalState NewState)
{
 // enable watch dog or timer depending of Xstatus (ENABLE or DISABLE)
  WDG->CR = (NewState==ENABLE)? WDG->CR|0x0001 : WDG->CR &~0x0001;
}
/*******************************************************************************
* Function Name  : WDG_CntRefresh
* Description    : Refresh and update the WDG counter to avoid a system reset.
* Input          : None
* Return         : None
*******************************************************************************/
void WDG_CntRefresh(void)
{
//write the first value in the key register
  WDG->KR = 0xA55A;
//write the second value
  WDG->KR = 0x5AA5;
}

/*******************************************************************************
* Function Name  : WDG_PrescalerConfig
* Description    : Set the counter prescaler value.
* Input          : 8 bit data
* Return         : None
*******************************************************************************/
void WDG_PrescalerConfig(vu8 Xprescaler)
{
  WDG->PR = Xprescaler;
// Divide the counter clock by (Prescaler + 1)
}
/*******************************************************************************
* Function Name  : WDG_CntReloadUpdate
* Description    : Update the counter pre-load value.
* Input          : 16 bit data
* Return         : None
*******************************************************************************/
void WDG_CntReloadUpdate(vu16 PreLoadValue)
{
  WDG->VR = PreLoadValue ;
// Reload sequence
 }
/*******************************************************************************
* Function Name  : WDG_CntOnOffConfig
* Description    : Start or stop the free auto-reload timer to countdown.
*                :
* Input          : START to start the counter
*                : STOP to stop the counter
*                :
* Return         : None
*******************************************************************************/
void WDG_CntOnOffConfig(WDG_CounterOperations Xoperation)
{
//write the start or stop counting bit
  WDG->CR = (Xoperation==WDG_START)? ( WDG->CR |0x0002 ) : (WDG->CR &~0x0002);
}
/*******************************************************************************
* Function Name  : WDG_ECITConfig
* Description    : Enable or disable the end of count interrupt
*                :
* Input          : ENABLE to enable the end of count interrupt
*                : DISABLE to disable the end of count interrupt
*                :
* Return         : None
*******************************************************************************/
void WDG_ECITConfig(FunctionalState NewState)
{
  WDG->MR = (NewState==ENABLE)? (WDG->MR |0x0001) : (WDG->MR &~0x0001);
}
/*******************************************************************************
* Function Name  : WDG_ECFlagClear
* Description    : Clear the end of count flag 
* Input          : None
* Return         : None
*******************************************************************************/
void WDG_ECFlagClear(void)
{
  WDG->SR = 0x0000;
//Reset the WDG Status Register bit
}
/*******************************************************************************
* Function Name  : WDG_ECStatus
* Description    : Return the end of count status 
* Input          : None
* Return         : u16
*******************************************************************************/
FlagStatus WDG_ECStatus(void)
{
  FlagStatus TmpResult ;
  TmpResult = (WDG->SR & 0x0001) == 0 ? RESET : SET;
  return TmpResult;
}
/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/









