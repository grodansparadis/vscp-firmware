/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_conf.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/04 
* Description        : Library configuration
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
#ifndef __720_CONF_H
#define __720_CONF_H


/* Comment the line below to put the library in release mode */
//#define LIBDEBUG

//#ifndef inline
//  #define inline __inline
//#endif


#define _IDE

/*----------------------- Asynchronous Peripherals ---------------------------*/

#define _A3BRG
#define _AGCR
#define _GPIO
#define _GPIO2
#define _BSPI
#define _BSPI1
#define _BSPI2
#define _UART
#define _UART1
#define _UART2
#define _EFT
#define _EFT1
#define _EFT2
#define _ADC
#define _CAN
#define _USB
#define _WDG

/*----------------------- Synchronous Peripherals ----------------------------*/

#define _DRAMC
#define _EMI
#define _DMAC
#define _SAPB
#define _RCCU
#define _GPIO3
#define _GPIO4
#define _WIU
#define _RTC
#define _CGC
#define _AHB
#define _AERR
#define _EIC

/*----------------------- CKIN frequency value ----------------------------*/

//#define CKINVal 16000000

/*----------------------------------------------------------------------------*/


#endif /* __720_CONF_H */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
