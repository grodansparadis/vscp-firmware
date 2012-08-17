/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_wiu.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the WIU software functions headers
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

#ifndef _720_WIU_H
#define _720_WIU_H
/* Includes ------------------------------------------------------------------*/
#include "720_lib.h"
/* Exported types ------------------------------------------------------------*/
typedef enum
{
  WIU_WakeUp           = 1,
  WIU_Interrupt        = 2,
  WIU_WakeUpInterrupt  = 3
} WIUMode_TypeDef;

typedef enum
{
  WIU_FallingEdge,
  WIU_RisingEdge
} WIUTriggerEdge_TypeDef;

#define WIU_Line0  0x0001
#define WIU_Line1  (WIU_Line0<<1)
#define WIU_Line2  (WIU_Line1<<1)
#define WIU_Line3  (WIU_Line2<<1)
#define WIU_Line4  (WIU_Line3<<1)
#define WIU_Line5  (WIU_Line4<<1)
#define WIU_Line6  (WIU_Line5<<1)
#define WIU_Line7  (WIU_Line6<<1)
#define WIU_Line8  (WIU_Line7<<1)
#define WIU_Line9  (WIU_Line8<<1)
#define WIU_Line10 (WIU_Line9<<1)
#define WIU_Line11 (WIU_Line10<<1)
#define WIU_Line12 (WIU_Line11<<1)
#define WIU_Line13 (WIU_Line12<<1)
#define WIU_Line14 (WIU_Line13<<1)
#define WIU_Line15 (WIU_Line14<<1)

/*******************************************************************************
* Function Name  : WIU_Init
* Description    : Configure the WIU Mede
* Input 1        : Mode can be WakeUp, Interrupt or WakeUpInterrupt.
* Input 2        : NewStatus can be ENABLE ro DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_Init(void);
/*******************************************************************************
* Function Name  : WIU_EnterStopMode
* Description    : Enter the stop mode
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_EnterStopMode(void);
/*******************************************************************************
* Function Name  : WIU_ModeConfig
* Description    : Configure the WIU Mede
* Input 1        : Mode can be WakeUp, Interrupt or WakeUpInterrupt.
* Input 2        : NewState can be ENABLE ro DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_ModeConfig(WIUMode_TypeDef Mode,FunctionalState NewState);
/*******************************************************************************
* Function Name  : WIU_LineModeConfig
* Description    : Configure the trigger edge.
* Input 1        : Lines to be configured.
* Input 2        : The trigger edge can be FallingEdge or RisingEdge.
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_LineModeConfig(vu16 Lines, WIUTriggerEdge_TypeDef TriggerEdge);

/*******************************************************************************
* Function Name  : WIU_LineConfig
* Description    : Enable and disable lines interrupts
* Input 1        : Lines to be configured.
* Input 2        : NewState can be ENABLE ro DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_LineConfig(vu16 Lines, FunctionalState NewState);

/*******************************************************************************
* Function Name  : WIU_InterruptLineValue
* Description    : Get the interrupt lines
* Input 1        : None.
* Output         : The interrupts lines numbers
* Return         : None
*******************************************************************************/
u16 WIU_InterruptLineValue(void);
/*******************************************************************************
* Function Name  : WIU_PendingBitClear
* Description    : Clear the pending bits
* Input 1        : Bits to be cleared.
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_PendingBitClear(vu16 Lines);


#endif /* _720_WIU_H */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/


