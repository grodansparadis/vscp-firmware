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

void WAKUP_IRQHandler(void);
void TIM2_OC2_IRQHandler(void);
void TIM2_OC1_IRQHandler(void);
void TIM2_IC12_IRQHandler(void);
void TIM2_UP_IRQHandler(void);
void TIM1_OC2_IRQHandler(void);
void TIM1_OC1_IRQHandler(void);
void TIM1_IC12_IRQHandler(void);
void TIM1_UP_IRQHandler(void);
void TIM0_OC2_IRQHandler(void);
void TIM0_OC1_IRQHandler(void);
void TIM0_IC12_IRQHandler(void);
void TIM0_UP_IRQHandler(void);
void PWM_OC123_IRQHandler(void);
void PWM_EM_IRQHandler(void);
void PWM_UP_IRQHandler(void);
void I2C_IRQHandler(void);
void SSP1_IRQHandler(void);
void SSP0_IRQHandler(void);
void UART2_IRQHandler(void);
void UART1_IRQHandler(void);
void UART0_IRQHandler(void);
void CAN_IRQHandler(void);
void USB_LP_IRQHandler(void);
void USB_HP_IRQHandler(void);
void ADC_IRQHandler(void);
void DMA_IRQHandler(void);
void EXTIT_IRQHandler(void);
void MRCC_IRQHandler(void);
void FLASHSMI_IRQHandler(void);
void RTC_IRQHandler(void);
void TB_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif	//_VECTOR_H
