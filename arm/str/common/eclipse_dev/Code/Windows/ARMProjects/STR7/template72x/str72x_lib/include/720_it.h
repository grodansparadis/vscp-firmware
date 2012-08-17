/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_it.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : contains the headers of the interrupt handlers'routines
*
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
#ifndef _720_IT_H
#define _720_IT_H

#include "720_lib.h"

void Undefined_Handler      (void);
void FIQ_Handler            (void);
void SWI_Handler            (void);
void Prefetch_Handler       (void);
void Abort_Handler          (void);
void EXT03IT_IRQHandler     (void);
void EXT4IT_IRQHandler      (void);
void WIUIT_IRQHandler       (void);
void EFT1IT_IRQHandler      (void);
void EFT2IT_IRQHandler      (void);
void UART1IT_IRQHandler     (void);
void UART2IT_IRQHandler     (void);
void DMAIT_IRQHandler       (void);
void DMAS0IT_IRQHandler     (void);
void DMAS1IT_IRQHandler     (void);
void USBHPIT_IRQHandler     (void);
void USBLPIT_IRQHandler     (void);
void CANIT_IRQHandler       (void);
void BSPI1IT_IRQHandler     (void);
void BSPI2IT_IRQHandler     (void);
void IDEPIT_IRQHandler      (void);
void RTCIT_IRQHandler       (void);
void ADCIT_IRQHandler       (void);
void EFT2OCA_IRQHandler     (void);
void EFT2OCB_IRQHandler     (void);
void WDGIT_IRQHandler       (void);

#endif /* _720_IT_H */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
