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
void PRCCUCMU_IRQHandler(void);
void EXTIT01_IRQHandler(void);
void EXTIT02_IRQHandler(void);
void EXTIT03_IRQHandler(void);
void EXTIT04_IRQHandler(void);
void EXTIT05_IRQHandler(void);
void EXTIT06_IRQHandler(void);
void EXTIT07_IRQHandler(void);
void EXTIT08_IRQHandler(void);
void EXTIT09_IRQHandler(void);
void EXTIT10_IRQHandler(void);
void EXTIT11_IRQHandler(void);
void EXTIT12_IRQHandler(void);
void EXTIT13_IRQHandler(void);
void EXTIT14_IRQHandler(void);
void EXTIT15_IRQHandler(void);
void DMATRERR_IRQHandler(void);
void TIM1_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void TB0_IRQHandler(void);
void TB1_IRQHandler(void);
void TB2_IRQHandler(void);
void TIM5_IRQHandler(void);
void TIM6_IRQHandler(void);
void TIM7_IRQHandler(void);
void TIM8_IRQHandler(void);
void TIM9_IRQHandler(void);
void UART2_IRQHandler(void);
void UART3_IRQHandler(void);
void FLASHEOP_IRQHandler(void);
void PWM0_IRQHandler(void);
void PWM1_IRQHandler(void);
void PWM2_IRQHandler(void);
void PWM3_IRQHandler(void);
void PWM4_IRQHandler(void);
void PWM5_IRQHandler(void);
void WIU_IRQHandler(void);
void WDGWUT_IRQHandler(void);
void BSPI0_IRQHandler(void);
void BSPI1_IRQHandler(void);
void BSPI2_IRQHandler(void);
void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void I2C0ITERR_IRQHandler(void);
void I2C1ITERR_IRQHandler(void);
void I2C0ITDDC_IRQHandler(void);
void I2C1ITDDC_IRQHandler(void);
void CAN0_IRQHandler(void);
void CAN1_IRQHandler(void);
void CAN2_IRQHandler(void);
void DMA0_IRQHandler(void);
void DMA1_IRQHandler(void);
void DMA2_IRQHandler(void);
void DMA3_IRQHandler(void);
void ADC_IRQHandler(void);
void RTC_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif	//_VECTOR_H
