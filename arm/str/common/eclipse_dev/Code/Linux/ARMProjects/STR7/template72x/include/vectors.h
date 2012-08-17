/***********************************************************************************
*	Copyright 2004 Anglia Design
*	This demo code and associated components are provided as is and has no warranty,
*	implied or otherwise.  You are free to use/modify any of the provided
*	code at your own risk in your applications with the expressed limitation
*	of liability (see below)
* 
*	LIMITATION OF LIABILITY:   ANGLIA SHALL NOT BE LIABLE FOR ANY LOSS OF PROFITS,
*	LOSS OF USE, LOSS OF DATA, INTERRUPTION OF BUSINESS, NOR FOR INDIRECT, SPECIAL,
*	INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND WHETHER UNDER THIS AGREEMENT OR
*	OTHERWISE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
*	Web     		: www.anglia-designs.com
*
***********************************************************************************/

#ifndef _VECTOR_H
#define _VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

void Undefined_Handler(void);
void FIQ_Handler(void);
void SWI_Handler(void);
void Prefetch_Handler(void);
void Abort_Handler(void);
void EXT03IT_IRQHandler(void);
void EXT4IT_IRQHandler(void);
void WIUIT_IRQHandler(void);
void EFT1IT_IRQHandler(void);
void EFT2IT_IRQHandler(void);
void UART1IT_IRQHandler(void);
void UART2IT_IRQHandler(void);
void DMAIT_IRQHandler(void);
void DMAS0IT_IRQHandler(void);
void DMAS1IT_IRQHandler(void);
void USBHPIT_IRQHandler(void);
void USBLPIT_IRQHandler(void);
void CANIT_IRQHandler(void);
void BSPI1IT_IRQHandler(void);
void BSPI2IT_IRQHandler(void);
void IDEPIT_IRQHandler(void);
void RTCIT_IRQHandler(void);
void ADCIT_IRQHandler(void);
void EFT2OCA_IRQHandler(void);
void EFT2OCB_IRQHandler(void);
void WDGIT_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif	//_VECTOR_H
