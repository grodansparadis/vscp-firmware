/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_lib.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : Peripherals pointers initialization
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
#define EXT

#include "720_map.h"

#ifdef LIBDEBUG

/*******************************************************************************
* Function Name  : libdebug
* Description    : this function initialize peripherals pointers
* Input          : no one
* Output         : no one
* Return         : no one
*******************************************************************************/
void libdebug()
{
 
 #ifdef _IDE
   IDE   = (IDE_TypeDef  *)IDE_BASE;
 #endif  /* _IDE */

 /*------------------ Asynchronous Peripherals --------------------------------*/
 #ifdef _A3BRG
  A3BRG    = (A3BRG_TypeDef  *) A3BRG_BASE ;
 #endif /* _A3BRG */

 #ifdef _AGCR
   AGCR   = (AGCR_TypeDef  *)AGCR_BASE;
 #endif  /* _AGCR */

 #ifdef _GPIO2
  GPIO2    = (GPIO_TypeDef  *) GPIO2_BASE ;
 #endif /* _GPIO2 */

 #ifdef _BSPI1
  BSPI1    = (BSPI_TypeDef  *) BSPI1_BASE ;
 #endif /* _BSPI1 */

 #ifdef _BSPI2
  BSPI2    = (BSPI_TypeDef  *) BSPI2_BASE ;
 #endif /* _BSPI2 */

 #ifdef _UART1
  UART1    = (UART_TypeDef  *) UART1_BASE ;
 #endif /* _UART1 */

 #ifdef _UART2
  UART2    = (UART_TypeDef  *) UART2_BASE ;
 #endif /* _UART2 */

 #ifdef _EFT1
  EFT1    = (EFT_TypeDef  *) EFT1_BASE ;
 #endif /* _EFT1 */

 #ifdef _EFT2
  EFT2    = (EFT_TypeDef  *) EFT2_BASE ;
 #endif /* _EFT2 */

 #ifdef _ADC
  ADC    = (ADC_TypeDef  *) ADC_BASE ;
 #endif /* _ADC */

 #ifdef _CAN
  CAN    = (CAN_TypeDef  *) CAN_BASE ;
 #endif /* _CAN */

 #ifdef _USB
  USB    = (USB_TypeDef  *) USB_BASE ;
 #endif /* _USB */

 #ifdef _WDG
  WDG    = (WDG_TypeDef  *) WDG_BASE ;
 #endif /* _WDG */

/*------------------ Synchronous Peripherals ---------------------------------*/

 #ifdef _DRAMC
   DRAMC   = (DRAMC_TypeDef  *)DRAMC_BASE;
 #endif  /* _DRAMC */

 #ifdef _EMI
   EMI   = (EMI_TypeDef  *)EMI_BASE;
 #endif  /* _EMI */

 #ifdef _DMAC
  DMAC   = (DMAC_TypeDef  *)DMAC_BASE;
 #endif  /* _DMAC */

 #ifdef _SAPB
  SAPB    = (SAPB_TypeDef  *) SAPB_BASE ;
 #endif /* _SAPB */  

 #ifdef _RCCU
  RCCU    = (RCCU_TypeDef  *) RCCU_BASE ;
 #endif /* _RCCU */

 #ifdef _GPIO3
  GPIO3    = (GPIO_TypeDef  *) GPIO3_BASE ;
 #endif /* _GPIO3 */

 #ifdef _GPIO4
  GPIO4    = (GPIO_TypeDef  *) GPIO4_BASE ;
 #endif /* _GPIO4 */

 #ifdef _WIU
  WIU    = (WIU_TypeDef  *) WIU_BASE ;
 #endif /* _WIU */
 
 #ifdef _RTC
  RTC    = (RTC_TypeDef  *) RTC_BASE ;
 #endif /* _RTC */
                                                     
 #ifdef _CGC
  CGC    = (CGC_TypeDef  *) CGC_BASE ;
 #endif /* _CGC */
 
 #ifdef _AHB
  AHB    = (AHB_TypeDef  *) AHB_BASE ;
 #endif /* _AHB */

 #ifdef _EIC
   EIC   = (EIC_TypeDef  *)EIC_BASE;
 #endif  /* _EIC */

}

#endif  /* LIBDEBUG */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
