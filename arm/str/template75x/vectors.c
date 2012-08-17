/***********************************************************************************
*	Copyright 2005 Anglia Design
*	This demo code and associated components are provided as is and has no warranty,
*	implied or otherwise.  You are free to use/modify any of the provided
*	code at your own risk in your applications with the expressed limitation
*	of liability (see below)
* 
*	LIMITATION OF LIABILITY:   ANGLIA OR ANGLIA DESIGNS SHALL NOT BE LIABLE FOR ANY
*	LOSS OF PROFITS, LOSS OF USE, LOSS OF DATA, INTERRUPTION OF BUSINESS, NOR FOR
*	INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND WHETHER UNDER
*	THIS AGREEMENT OR OTHERWISE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
*	Author			: Spencer Oliver
*	Web     		: www.anglia-designs.com
*
***********************************************************************************/

#include "vectors.h"

/*******************************************************************************
* Function Name  : Undefined_Handler
* Description    : This function handles Undefined instruction exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Undefined_Handler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : This function handles FIQ exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FIQ_Handler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : SWI_Handler
* Description    : This function handles SWI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SWI_Handler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : Prefetch_Handler
* Description    : This function handles Prefetch Abort exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Prefetch_Handler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : Abort_Handler
* Description    : This function handles Data Abort exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Abort_Handler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : WAKUP_IRQHandler
* Description    : This function handles External line 15(WAKUP) interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WAKUP_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM2_OC2_IRQHandler
* Description    : This function handles TIM2 Output Compare 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_OC2_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM2_OC1_IRQHandler
* Description    : This function handles TIM2 Output Compare 1 interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_OC1_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM2_IC12_IRQHandler
* Description    : This function handles TIM2 Input Capture 1 & 2 interrupt  
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IC12_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM2_UP_IRQHandler
* Description    : This function handles TIM2 Update interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_UP_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM1_OC2_IRQHandler
* Description    : This function handles TIM1 Output Compare 2 interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_OC2_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM1_OC1_IRQHandler
* Description    : This function handles TIM1 Output Compare 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_OC1_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM1_IC12_IRQHandler
* Description    : This function handles TIM1 Input Capture 1 & 2 interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_IC12_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 Update interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM0_OC2_IRQHandler
* Description    : This function handles TIM0 Output Compare 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM0_OC2_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM0_OC1_IRQHandler
* Description    : This function handles TIM0 Output Compare 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM0_OC1_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM0_IC12_IRQHandler
* Description    : This function handles TIM0 Input Capture 1 & 2 interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM0_IC12_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM0_UP_IRQHandler
* Description    : This function handles TIM0 Update interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM0_UP_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : PWM_OC123_IRQHandler
* Description    : This function handles PWM Output Compare 1,2&3 interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWM_OC123_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : PWM_EM_IRQHandler
* Description    : This function handles PWM Emergency interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWM_EM_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : PWM_UP_IRQHandler
* Description    : This function handles PWM Update interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWM_UP_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : I2C_IRQHandler
* Description    : This function handles I2C global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : SSP1_IRQHandler
* Description    : This function handles SSP1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SSP1_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : SSP0_IRQHandler
* Description    : This function handles SSP0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SSP0_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART2_IRQHandler
* Description    : This function handles UART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART2_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART1_IRQHandler
* Description    : This function handles UART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART1_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART0_IRQHandler
* Description    : This function handles UART0 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART0_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : CAN_IRQHandler
* Description    : This function handles CAN global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : USBLP_IRQHandler
* Description    : This function handles USB Low Priority event interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : USBHP_IRQHandler
* Description    : This function handles USB High Priority event interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : DMA_IRQHandler
* Description    : This function handles DMA global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT_IRQHandler
* Description    : This function handles External lines 14 to 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : MRCC_IRQHandler
* Description    : This function handles MRCC interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MRCC_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : FLASHSMI_IRQHandler
* Description    : This function handles Flash and SMI global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASHSMI_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TB_IRQHandler
* Description    : This function handles TB global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TB_IRQHandler(void)
{
	while(1);
}
