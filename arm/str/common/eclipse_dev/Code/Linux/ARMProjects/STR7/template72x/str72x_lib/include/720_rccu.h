/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_rccu.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the Reset Clock Control Unit
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
#ifndef __720_RCCU_H
#define __720_RCCU_H
/* Includes ------------------------------------------------------------------*/

#include "720_lib.h"

// PLL Divider Factors

#define RCCU_PLLDiv1  0x00         // The divider factor is 1
#define RCCU_PLLDiv2  0x40         // The divider factor is 2
#define RCCU_PLLDiv4  0x80         // The divider factor is 4
#define RCCU_PLLDiv8  0xC0         // The divider factor is 8

// The ATAPI divider factors

typedef enum
{
  RCCU_ATAPIDiv2 = 0x00,       // AHB_CLK / 2
  RCCU_ATAPIDiv3 = 0x01        // AHB_CLK / 3
}RCCU_ATAPIClk;

// The AAPB output clock divider factor

typedef enum
{
  RCCU_AAPBDiv2  = 0x00,          // AHB_CLK / 2
  RCCU_AAPBDiv4  = 0x01,          // AHB_CLK / 4
  RCCU_AAPBDiv8  = 0x02,          // AHB_CLK / 8
  RCCU_AAPBDiv16 = 0x03           // AHB_CLK / 16
}RCCU_AAPBClk;

// the RCCU Flags definition

typedef enum
{
  RCCU_Lock     = 0x8000,     // Lock changing interrupt flag
  RCCU_Ulock    = 0x4000,     // Unlock changing interrupt flag
  RCCU_Div2     = 0x40,       // division by 2 of CKIN clock signal
  RCCU_PLLByp   = 0x20,       // PLL bypass status flag
  RCCU_Idle     = 0x10,       // IDLE mode status flag
  RCCU_Slow    = 0x8,        // Slow mode status flag
  RCCU_WDGReset = 0x2,        // Watchdog Reset staus flag
  RCCU_SWReset  = 0x1         // Software reset flag status
}RCCU_Flags;

// The RCCU clocks

typedef enum
{
  RCCU_ATAPIClock = 0x0,       // The ATAPI clock
  RCCU_AHBClock   = 0x1,       // The AHB clock
  RCCU_AAPBClock  = 0x2        // The APB clock
}RCCU_Clocks;



/*******************************************************************************
* Function Name  : RCCU_PllStableStatus
* Description    : This routine is used to test whether the PLL is stable or not
* Input          : NONE
* Return         : PLL Clock Status
*                  RESET: the Pll is not stable
*                  SET  : PLL is stable
*******************************************************************************/
FlagStatus RCCU_PllStableStatus(void);
/*******************************************************************************
* Function Name  : RCCU_AutBypConfig
* Description    : This routine is used to enable or disable the auto-bypass
* Input          : the Status of the auto bypass mode
*                      ENABLE: the auto bypass mode is enabled
*                      DISABLE: the auto bypass mode is disabled.
* Return         : NONE
*******************************************************************************/
void RCCU_AutBypConfig(FunctionalState NewState);
/*******************************************************************************
* Function Name  : RCCU_PllConfig
* Description    : This routine is used to swithch off or on the PLL
* Input          : the Status of the PLL
*                      ENABLE: swtich on the PLL
*                      DISABLE: swith off the PLL
* Return         : NONE
*******************************************************************************/
void RCCU_PllConfig(FunctionalState NewState);
/*******************************************************************************
* Function Name  : RCCU_PllBypConfig
* Description    : This routine is used to bypass or not the PLL
* Input          : the Status of the PLL
*                      ENABLE: PLL bypassed
*                      DISABLE: PLL not bypassed
* Return         : NONE
*******************************************************************************/
void RCCU_PllBypConfig(FunctionalState NewState);
/*******************************************************************************
* Function Name  : RCCU_PllClkConfig
* Description    : This routine is used to configure the multiplication and
*                  divider factors for the PLL output clock
* Input          : DivMul factor: the new clock multiplication and divider
*                  factors.
*                  DivMulFac[7:6] == the division factor
*                                    00: the divder factor is 1
*                                    01: the divder factor is 2
*                                    10: the divder factor is 4
*                                    11: the divder factor is 8
*                  DivMulFac[5:0] == the mulitiplicatin factor - 1
* Return         : NONE
*******************************************************************************/
void RCCU_PllClkConfig(vu16 DivMulFac);
/*******************************************************************************
* Function Name  : RCCU_ATAPIClkConfig
* Description    : This routine is used to configure the ATAPI clock
* Input          : the AHB_CLK division by 2 Status
*                      ENABLE: Select AHB_CLK/2 as ATAPI clock
*                      DISABLE: Select AHB_CLK/3 as ATAPI clock
* Return         : NONE
*******************************************************************************/
void RCCU_ATAPIClkConfig(RCCU_ATAPIClk ATAPIClk);
/*******************************************************************************
* Function Name  : RCCU_AAPBClkConfig
* Description    : This routine is used to configure the APB output clock
* Input          : the APB output clock divider factor
*                      RCCU_AAPBDiv2:  APB CLK is AHB_CLK/2
*                      RCCU_AAPBDiv4:  APB CLK is AHB_CLK/4
*                      RCCU_AAPBDiv8:  APB CLK is AHB_CLK/8
*                      RCCU_AAPBDiv16: APB CLK is AHB_CLK/16
* Return         : NONE
*******************************************************************************/
void RCCU_AAPBClkConfig(RCCU_AAPBClk AAPBClk);
/*******************************************************************************
* Function Name  : RCCU_FlagStatus
* Description    : This routine returns the staus of the specified flag
* Input          : the RCCU flag
*                      RCCU_Lock:     Lock changing interrupt flag
*                      RCCU_Ulock:    Unlcok changing interrupt flag
*                      RCCU_Div2:     The division by 2 of the CKIN clock signal
*                      RCCU_PllByp:   PLL bypass flag
*                      RCCU_Idle:     Idle mode status flag
*                      RCCU_Slow:    Slow mode status Flag
*                      RCCU_WDGReset: watchdog reset status flag
*                      RCCU_SWReset:  Software Reset Flag Status
* Return         : the status of the specified flag
*                      RESET: the flag is reset
*                      SET:   the tested flags is set.
*******************************************************************************/
FlagStatus RCCU_FlagStatus(RCCU_Flags Xflag);
/*******************************************************************************
* Function Name  : RCCU_SWResetConfig
* Description    : This routine is enable or disable the software reset
* Input          : New State of the SW reset
*                         ENABLE: SW reset enabled
*                         DISABLE: SW Reset disabled
* Return         : NONE
*******************************************************************************/
void RCCU_SWResetConfig(FunctionalState NewState);
/*******************************************************************************
* Function Name  : RCCU_Div2Config
* Description    : This routine is enable or disable the division by 2 of CKIN
*                  clock signal.
* Input          : New State of the division by 2
*                         ENABLE: Division by 2 enabled
*                         DISABLE: Division by 2 disabled
* Return         : NONE
*******************************************************************************/
void RCCU_Div2Config(FunctionalState NewState);
/*******************************************************************************
* Function Name  : RCCU_IdleConfig
* Description    : This routine is select or not the IDLE mode
* Input          : New State of the IDLE mode
*                         ENABLE: IDLE mode selected
*                         DISABLE: IDLE mode not selected
* Return         : NONE
*******************************************************************************/
void RCCU_IdleConfig(FunctionalState NewState);
/*******************************************************************************
* Function Name  : RCCU_SlowConfig
* Description    : This routine is used to select or not the Slow mode
* Input          : New State of the Slow mode
*                         ENABLE: Slow mode selected
*                         DISABLE: Slow mode not selected
* Return         : NONE
*******************************************************************************/
void RCCU_SlowConfig(FunctionalState NewState);
/*******************************************************************************
* Function Name  : RCCU_ITConfig
* Description    : This routine is used to configure the specified RCCU
*                  interrupt(s).
* Input          : - RccuIT: The interrupt to enable or disable
*                       RCCU_Lock  : RCCU lock interrupt
*                       RCCU_Ulock : RCCU unlock interrupt
*                  - NewState: Interrupt Status
*                         ENABLE: the specified interrupt is enabled
*                         DISABLE: the specified interrupt is disabled
* Return         : NONE
*******************************************************************************/
void RCCU_ITConfig(RCCU_Flags Xflag, FunctionalState NewState);
/*******************************************************************************
* Function Name  : RCCU_StopConfig
* Description    : This routine is used to select or not the STOP mode
* Input          : New State of the Stop mode
*                         ENABLE: Stop mode selected
*                         DISABLE: Stop mode not selected
* Return         : NONE
*******************************************************************************/
void RCCU_StopConfig(FunctionalState NewState);
/*******************************************************************************
* Function Name  : RCCU_ProtectionConfig
* Description    : This routine is used to write protect or not the registers
*                  PLLCONF, DIVCONF and CLKCTL.
* Input          : New State of the write protection
*                         ENABLE: Write protection enabled
*                         DISABLE: Write protection disabled
* Return         : NONE
*******************************************************************************/
void RCCU_ProtectionConfig(FunctionalState NewState);
/*******************************************************************************
* Function Name  : RCCU_FreqVal
* Description    : This routine is used to compute the frequency (in Hz) of
*                  the specified clock
* Input          : - CkinVal: The CKIN input clock frequency value (in Hz)
*                  - RccuClk: The specified clock to compute the frequency:
*                             + RCCU_ATAPIClock: The ATAPI clock
*                             + RCCU_AHBClock  : The AHB clock
*                             + RCCU_AAPBClock : The APB clock
* Return         : the frequency of the specified clock (in Hz)
*******************************************************************************/
u32 RCCU_GetFrequency(RCCU_Clocks RccuClk);

extern const u32 RCCU_Main_Osc;

#endif	// __720_RCCU_H

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
