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

// modified by Martin Thomas: UART-echo, Timer3 & callback

#include <stdint.h>
#include "vectors.h"
#include "71x_lib.h"

// mthomas: added
volatile Callback_Function pTimer3_callback;

void Timer3_set_callback(Callback_Function pFunc)
{
	pTimer3_callback = pFunc;
}

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
void SWI_Handler(int swi_num, int *regs)
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
* Function Name  : T0TIMI_IRQHandler
* Description    : This function handles the Timer0 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void T0TIMI_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles the FLASH global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : RCCU_IRQHandler
* Description    : This function handles the RCCU global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCCU_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles the RTC global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : WDG_IRQHandler
* Description    : This function handles the Watchdog interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WDG_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : XTI_IRQHandler
* Description    : This function handles the External interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void XTI_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : USBHP_IRQHandler
* Description    : This function handles USB high priority interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBHP_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : I2C0ITERR_IRQHandler
* Description    : This function handles the I2C0 error interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C0ITERR_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : I2C1ITERR_IRQHandler
* Description    : This function handles the I2C1 error interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1ITERR_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART0_IRQHandler
* Description    : This function handles the UART0 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART0_IRQHandler(void)
{
	uint8_t bRByte;
	uint8_t UARTStatus;
	/*  Get the received byte, set the guard time to 0xFF */
	UARTStatus = UART_ByteReceive(UART0, &bRByte, 0xFF);
	
	/*  Send the received byte */
	UART_ByteSend(UART0, &bRByte);
	// while(1);
}

/*******************************************************************************
* Function Name  : UART1_IRQHandler
* Description    : This function handles the UART1 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART1_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART2_IRQHandler
* Description    : This function handles the UART2 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART2_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART3_IRQHandler
* Description    : This function handles the UART3 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART3_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : BSPI0_IRQHandler
* Description    : This function handles the BSPI0 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI0_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : BSPI1_IRQHandler
* Description    : This function handles the BSPI1 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI1_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : I2C0_IRQHandler
* Description    : This function handles the I2C0 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C0_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : I2C1_IRQHandler
* Description    : This function handles the I2C1 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : CAN_IRQHandler
* Description    : This function handles the CAN module global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : ADC12_IRQHandler
* Description    : This function handles the ADC sample ready interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC12_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : T1TIMI_IRQHandler
* Description    : This function handles Timer1 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void T1TIMI_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : T2TIMI_IRQHandler
* Description    : This function handles Timer2 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void T2TIMI_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : T3TIMI_IRQHandler
* Description    : This function handles Timer3 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void T3TIMI_IRQHandler(void)
{
#if 0
	static int cnt = 0;
	
	if ( cnt++ >= 1000 ) {
		GPIO_BitWrite( GPIO0, 2, !GPIO_BitRead( GPIO0, 2 ));
		cnt = 0;
	}
#endif
	
#if 0
	TIM3->SR  &= ~0x4000;           //-- Clear Timer1 interrupt pending bit
	TIM3->OCAR = TIMER3_LOAD_VAL;   //-- Reload Timer1 period
	TIM3->CNTR = 0xFFFC;            //-- Start Timer1 again
#else
	TIM_FlagClear ( TIM3 , TIM_OCA_IT );
	TIM3->OCAR += TIMER3_LOAD_VAL;  // mt: instead: just increment the match-value
#endif

	if ( pTimer3_callback ) {
		pTimer3_callback();
	}
}

/*******************************************************************************
* Function Name  : HDLC_IRQHandler
* Description    : This function handles HDLC global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HDLC_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : USBLP_IRQHandler
* Description    : This function handles USB low priority event interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBLP_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : T0TOI_IRQHandler
* Description    : This function handles the Timer0 overflow interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void T0TOI_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : T0OC1_IRQHandler
* Description    : This function handles the Timer0 Output compare 1 interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void T0OC1_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : T0OC2_IRQHandler
* Description    : This function handles the Timer0 Output compare 2 interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void T0OC2_IRQHandler(void)
{
	while(1);
}
