/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_lib.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This module is used to include the peripherals header files
*                      in the user application.
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

#ifndef __720_LIB_H
#define __720_LIB_H

#include "720_map.h"
#include "720_conf.h"


#ifdef _IDE
#include "720_ide.h"
#endif /*_IDE*/

/*----------------------- Asynchronous Peripherals ---------------------------*/

#ifdef _A3BRG
#include "720_a3brg.h"
#endif /*_A3BRG */

#ifdef _AGCR
#include "720_agcr.h"
#endif /*_AGCR */

#ifdef _GPIO
#include "720_gpio.h"
#endif /*_GPIO */

#ifdef _BSPI
#include "720_bspi.h"
#endif /*_BSPI */

#ifdef _UART
#include "720_uart.h"
#endif /*_UART */

#ifdef _EFT
#include "720_eft.h"
#endif /*_EFT */

#ifdef _ADC
#include "720_adc.h"
#endif /*_ADC */

#ifdef _CAN
#include "720_can.h"
#endif /*_CAN */

#ifdef _USB
#endif /*_USB */

#ifdef _WDG
#include "720_wdg.h"
#endif /*_WDG */


/*----------------------- Synchronous Peripherals ----------------------------*/

#ifdef _DRAMC
#include "720_dramc.h"
#endif /*_DRAMC */

#ifdef _EMI
#include "720_emi.h"
#endif /*_EMI*/

#ifdef _DMAC
#include "720_dmac.h"
#endif /*_DMAC */

#ifdef _SAPB
#include "720_sapb.h"
#endif /*_SAPB */

#ifdef _RCCU
#include "720_rccu.h"
#endif /*_RCCU */

#ifdef _WIU
#include "720_wiu.h"
#endif /*_WIU */

#ifdef _RTC
#include "720_rtc.h"
#endif /*_RTC */

#ifdef _CGC
#include "720_cgc.h"
#endif /*_CGC */

#ifdef _AHB
#include "720_ahb.h"
#endif /*_AHB */


#ifdef _EIC
  #include "720_eic.h"
#endif /* _EIC */


void  libdebug( void );


#endif /* __720_LIB_H */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
