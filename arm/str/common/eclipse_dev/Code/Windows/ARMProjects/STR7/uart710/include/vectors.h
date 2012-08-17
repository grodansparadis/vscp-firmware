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
*	Author			: Spencer Oliver
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
void T0TIMI_IRQHandler(void);
void FLASH_IRQHandler(void);
void RCCU_IRQHandler(void);
void RTC_IRQHandler(void);
void WDG_IRQHandler(void);
void XTI_IRQHandler(void);
void USBHP_IRQHandler(void);
void I2C0ITERR_IRQHandler(void);
void I2C1ITERR_IRQHandler(void);
void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void UART2_IRQHandler(void);
void UART3_IRQHandler(void);
void BSPI0_IRQHandler(void);
void BSPI1_IRQHandler(void);
void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);
void CAN_IRQHandler(void);
void ADC12_IRQHandler(void);
void T1TIMI_IRQHandler(void);
void T2TIMI_IRQHandler(void);
void T3TIMI_IRQHandler(void);
void HDLC_IRQHandler(void);
void USBLP_IRQHandler(void);
void T0TOI_IRQHandler(void);
void T0OC1_IRQHandler(void);
void T0OC2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif	//_VECTOR_H
