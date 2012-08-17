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
* Function Name  : EXT03IT_IRQHandler
* Description    : This function handles the external interrupts EINT0-3
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXT03IT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXT4IT_IRQHandler
* Description    : This function handles the external interrupt EINT4
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXT4IT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : WIUIT_IRQHandler
* Description    : This function handles the WIU Wake-up event interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void WIUIT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EFT1IT_IRQHandler
* Description    : This function handles the EFT1 Global interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EFT1IT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EFT2IT_IRQHandler
* Description    : This function handles the EFT2 Global interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EFT2IT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART1IT_IRQHandler
* Description    : This function handles the UART 1 global interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void UART1IT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART2IT_IRQHandler
* Description    : This function handles the UART2 Global interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void UART2IT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : DMAIT_IRQHandler
* Description    : This function handles DMA event global interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void DMAIT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : DMAS0IT_IRQHandler
* Description    : This function handles DMA Stream 0 end of transfer interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void DMAS0IT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : DMAS1IT_IRQHandler
* Description    : This function handles DMA Stream 1 end of transfer interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void DMAS1IT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : USBHPIT_IRQHandler
* Description    : This function handles the USB high priority event interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void USBHPIT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : USBLPIT_IRQHandler
* Description    : This function handles the USB low priority event interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void USBLPIT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : CANIT_IRQHandler
* Description    : This function handles the CAN module general interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void CANIT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : BSPI1IT_IRQHandler
* Description    : This function handles the BSPI 1 global interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void BSPI1IT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : BSPI2IT_IRQHandler
* Description    : This function handles the BSPI 2 global interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void BSPI2IT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : IDEPIT_IRQHandler
* Description    : This function handles the IDE Primary Channel interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void IDEPIT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : RTCIT_IRQHandler
* Description    : This function handles the RTC periodic iterrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void RTCIT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : ADCIT_IRQHandler
* Description    : This function handles the ADC sample ready interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void ADCIT_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EFT2OCA_IRQHandler
* Description    : This function handles the EFT 2 Output Compare A interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EFT2OCA_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EFT2OCB_IRQHandler
* Description    : This function handles the EFT 2 Output Compare B interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EFT2OCB_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : WDGIT_IRQHandler
* Description    : This function handles the WDG Timer interrupt
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void WDGIT_IRQHandler(void)
{
	while(1);
}
