/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_rccu.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the Reset and Clock Control Unit
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
#include "720_rccu.h"
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
* Function Name  : RCCU_PllStableStatus
* Description    : This routine is used to test whether the PLL is stable or not
* Input          : NONE
* Return         : PLL Clock Status
*                  RESET: the Pll is not stable
*                  SET  : PLL is stable
*******************************************************************************/
FlagStatus RCCU_PllStableStatus(void)
{
  FlagStatus TmpResult;
  RCCU_ProtectionConfig(DISABLE);
  TmpResult = (RCCU->PLLCONF & 0x8000) == 0 ? RESET : SET;
  return TmpResult;
}
/*******************************************************************************
* Function Name  : RCCU_AutBypConfig
* Description    : This routine is used to enable or disable the auto-bypass
* Input          : the Status of the auto bypass mode
*                      ENABLE: the auto bypass mode is enabled
*                      DISABLE: the auto bypass mode is disabled.
* Return         : NONE
*******************************************************************************/
void RCCU_AutBypConfig(FunctionalState NewState)
{
  RCCU_ProtectionConfig(DISABLE);
  RCCU->PLLCONF = (NewState == ENABLE)? (RCCU->PLLCONF | 0x4000):\
                                        (RCCU->PLLCONF & 0xBFFF);
}
/*******************************************************************************
* Function Name  : RCCU_PllConfig
* Description    : This routine is used to swithch off or on the PLL
* Input          : the Status of the PLL
*                      ENABLE: swtich on the PLL
*                      DISABLE: swith off the PLL
* Return         : NONE
*******************************************************************************/
void RCCU_PllConfig(FunctionalState NewState)
{
  RCCU_ProtectionConfig(DISABLE);
  RCCU->PLLCONF = (NewState == DISABLE)? (RCCU->PLLCONF | 0x200):\
                                         (RCCU->PLLCONF & 0xDFF);
}
/*******************************************************************************
* Function Name  : RCCU_PllBypConfig
* Description    : This routine is used to bypass or not the PLL
* Input          : the Status of the PLL
*                      ENABLE: PLL bypassed
*                      DISABLE: PLL not bypassed
* Return         : NONE
*******************************************************************************/
void RCCU_PllBypConfig(FunctionalState NewState)
{
  RCCU_ProtectionConfig(DISABLE);
  RCCU->PLLCONF = (NewState == ENABLE)? (RCCU->PLLCONF | 0x100):\
                                        (RCCU->PLLCONF & 0xEFF);
}
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
void RCCU_PllClkConfig(vu16 DivMulFac)
{
  RCCU_ProtectionConfig(DISABLE);
  RCCU->PLLCONF = (RCCU->PLLCONF & 0xFF00) | (DivMulFac-1);
}
/*******************************************************************************
* Function Name  : RCCU_ATAPIClkConfig
* Description    : This routine is used to configure the ATAPI clock
* Input          : the AHB_CLK division by 2 Status
*                      ENABLE: Select AHB_CLK/2 as ATAPI clock
*                      DISABLE: Select AHB_CLK/3 as ATAPI clock
* Return         : NONE
*******************************************************************************/
void RCCU_ATAPIClkConfig(RCCU_ATAPIClk ATAPIClk)
{
  RCCU_ProtectionConfig(DISABLE);
  RCCU->DIVCONF = (RCCU->DIVCONF & 0x03) | ATAPIClk;
}
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
void RCCU_AAPBClkConfig(RCCU_AAPBClk AAPBClk)
{
  RCCU_ProtectionConfig(DISABLE);
  RCCU->DIVCONF = (RCCU->DIVCONF & 0x1C) | AAPBClk;
}
/*******************************************************************************
* Function Name  : RCCU_FlagStatus
* Description    : This routine returns the staus of the specified flag
* Input          : the RCCU flag
*                      RCCU_Lock:     Lock changing interrupt flag
*                      RCCU_Ulock:    Unlcok changing interrupt flag
*                      RCCU_Div2:     The division by 2 of the CKIN clock signal
*                      RCCU_PllByp:   PLL bypass flag
*                      RCCU_Idle:     Idle mode status flag
*                      RCCU_Slow:     Slow mode status Flag
*                      RCCU_WDGReset: watchdog reset status flag
*                      RCCU_SWReset:  Software Reset Flag Status
* Return         : the status of the specified flag
*                      RESET: the flag is reset
*                      SET:   the tested flags is set.
*******************************************************************************/
FlagStatus RCCU_FlagStatus(RCCU_Flags Xflag)
{
  FlagStatus TmpResult ;
  TmpResult = (RCCU->CLKFLAG & Xflag) == 0 ? RESET : SET;
  return TmpResult;
}
/*******************************************************************************
* Function Name  : RCCU_SWResetConfig
* Description    : This routine is enable or disable the software reset
* Input          : NewState of the SW reset
*                         ENABLE: SW reset enabled
*                         DISABLE: SW Reset disabled
* Return         : NONE
*******************************************************************************/
void RCCU_SWResetConfig(FunctionalState NewState)
{
  RCCU_ProtectionConfig(DISABLE);
  RCCU->CLKCTL = (NewState == ENABLE)? (RCCU->CLKCTL | 0x08):\
                                       (RCCU->CLKCTL & 0x07);
}
/*******************************************************************************
* Function Name  : RCCU_Div2Config
* Description    : This routine is enable or disable the division by 2 of CKIN
*                  clock signal.
* Input          : NewState of the division by 2
*                         ENABLE: Division by 2 enabled
*                         DISABLE: Division by 2 disabled
* Return         : NONE
*******************************************************************************/
void RCCU_Div2Config(FunctionalState NewState)
{
  RCCU_ProtectionConfig(DISABLE);
  RCCU->CLKCTL = (NewState == ENABLE)? (RCCU->CLKCTL | 0x04):\
                                       (RCCU->CLKCTL & 0x0B);
}
/*******************************************************************************
* Function Name  : RCCU_IdleConfig
* Description    : This routine is select or not the IDLE mode
* Input          : NewState of the IDLE mode
*                         ENABLE: IDLE mode selected
*                         DISABLE: IDLE mode not selected
* Return         : NONE
*******************************************************************************/
void RCCU_IdleConfig(FunctionalState NewState)
{
  RCCU_ProtectionConfig(DISABLE);
  RCCU->CLKCTL = (NewState == ENABLE)? (RCCU->CLKCTL | 0x02):\
                                       (RCCU->CLKCTL & 0x0D);
}
/*******************************************************************************
* Function Name  : RCCU_SlowConfig
* Description    : This routine is used to select or not the Slow mode
* Input          : NewState of the Slow mode
*                         ENABLE: Slow mode selected
*                         DISABLE: Slow mode not selected
* Return         : NONE
*******************************************************************************/
void RCCU_SlowConfig(FunctionalState NewState)
{
  RCCU_ProtectionConfig(DISABLE);
  RCCU->CLKCTL = (NewState == ENABLE)? (RCCU->CLKCTL | 0x01):\
                                       (RCCU->CLKCTL & 0x0E);
}
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
void RCCU_ITConfig(RCCU_Flags Xflag, FunctionalState NewState)
{
  RCCU->MSKCTL = (NewState == ENABLE)? (RCCU->MSKCTL | Xflag):\
                                       (RCCU->MSKCTL & ~Xflag);
}

/*******************************************************************************
* Function Name  : RCCU_StopConfig
* Description    : This routine is used to select or not the STOP mode
* Input          : NewState of the Stop mode
*                         ENABLE: Stop mode selected
*                         DISABLE: Stop mode not selected
* Return         : NONE
*******************************************************************************/
void RCCU_StopConfig(FunctionalState NewState)
{
  RCCU->MSKCTL = (NewState == ENABLE)? (RCCU->MSKCTL | 0x01) :\
                                       (RCCU->MSKCTL & 0xC000);
}

/*******************************************************************************
* Function Name  : RCCU_ProtectionConfig
* Description    : This routine is used to write protect or not the registers
*                  PLLCONF, DIVCONF and CLKCTL.
* Input          : NewState of the write protection
*                         ENABLE: Write protection enabled
*                         DISABLE: Write protection disabled
* Return         : NONE
*******************************************************************************/
void RCCU_ProtectionConfig(FunctionalState NewState)
{
  RCCU->SYSPROT = (NewState == DISABLE)? 0x01 : 0x0;
}

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
u32 RCCU_GetFrequency(RCCU_Clocks RccuClk)
{
u32 tmp;

  if (RCCU->MSKCTL & 0x01)
    tmp = 32768;
  else
    {
    // PLLCLKREF frequency value
    //tmp = CKINVal >> ((RCCU->CLKCTL & 0x04)>>2);
    tmp = RCCU_Main_Osc >> ((RCCU->CLKCTL & 0x04)>>2);

    //The AHB clock frequency value
    if (RCCU->CLKCTL & 0x2)
      tmp = tmp >> 0x5;
    else
      {
       if (!(RCCU->PLLCONF & 0x100))
         tmp= (tmp>>((RCCU->PLLCONF & 0xC0)>>0x06))*((RCCU->PLLCONF & 0x3F)+1);
      }
    }

  if (RccuClk == RCCU_AHBClock)
    return tmp;
  if (RccuClk == RCCU_ATAPIClock)
    return (tmp/(2+((RCCU->DIVCONF & 0x04)>>2)));
  else
    return (tmp>>(1+(RCCU->DIVCONF & 0x3)));
}







/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
