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

#include "73x_lib.h"
#include "vectors.h"

extern u16 Conversion_Value;
u16 AnalogWatchdog_Result;

/*******************************************************************************
* Function Name  : Undefined_Handler
* Description    : This function Undefined instruction exception.
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
* Description    : This function handles SW exception.
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
* Description    : This function handles preftetch abort exception.
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
* Description    : This function handles data abort exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Abort_Handler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : PRCCUCMU_IRQHandler
* Description    : This function handlesthe PRCCU and the CMU interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PRCCUCMU_IRQHandler(void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT01_IRQHandler
* Description    : This function handles External line1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT01_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT02_IRQHandler
* Description    : This function handles External line2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT02_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT03_IRQHandler
* Description    : This function handles External line3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT03_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT04_IRQHandler
* Description    : This function handles External line 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT04_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT05_IRQHandler
* Description    : This function handles External line 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT05_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT06_IRQHandler
* Description    : This function handles External line 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT06_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT07_IRQHandler
* Description    : This function handles External line7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT07_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT08_IRQHandler
* Description    : This function handles External line8 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT08_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT09_IRQHandler
* Description    : This function handles External line9 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT09_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT10_IRQHandler
* Description    : This function handles External line10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT10_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT11_IRQHandler
* Description    : This function handles External line11 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT11_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT12_IRQHandler
* Description    : This function handles External line12 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT12_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT13_IRQHandler
* Description    : This function handles External line13 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT13_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT14_IRQHandler
* Description    : This function handles External line14 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT14_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : EXTIT15_IRQHandler
* Description    : This function handles External line15 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTIT15_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : DMATRERR_IRQHandler
* Description    : This function handles the DMA transfer error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMATRERR_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM1_IRQHandler
* Description    : This function handles the TIM1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles the TIM2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles the TIM3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles the TIM4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TB0_IRQHandler
* Description    : This function handles the TB0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TB0_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TB1_IRQHandler
* Description    : This function handles the TB1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TB1_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TB2_IRQHandler
* Description    : This function handles the TB2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TB2_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles the TIM5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles the TIM6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM6_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles the TIM7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM8_IRQHandler
* Description    : This function handles the TIM8 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : TIM9_IRQHandler
* Description    : This function handles the TIM9 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM9_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART2_IRQHandler
* Description    : This function handles the UART2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART2_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART3_IRQHandler
* Description    : This function handles the UART3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART3_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : FlashEOP_IRQHandler
* Description    : This function handles the FLASH end of program interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASHEOP_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : PWM0_IRQHandler
* Description    : This function handles the PWM0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWM0_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : PWM1_IRQHandler
* Description    : This function handles the PWM1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWM1_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : PWM2_IRQHandler
* Description    : This function handles the PWM2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWM2_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : PWM3_IRQHandler
* Description    : This function handles the PWM3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWM3_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : PWM4_IRQHandler
* Description    : This function handles the PWM4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWM4_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : PWM5_IRQHandler
* Description    : This function handles the PWM5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWM5_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : WIU_IRQHandler
* Description    : This function handles the wake-up system interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WIU_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : WDGWUT_IRQHandler
* Description    : This function handles the watchdog and the wakeup timer interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WDGWUT_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : BSPI0_IRQHandler
* Description    : This function handles the BSPI0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI0_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  :  BSPI1_IRQHandler
* Description    : This function handles the BSPI1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI1_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : BSPI2_IRQHandler
* Description    : This function handles the BSPI2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSPI2_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART0_IRQHandler
* Description    : This function handles the UART0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART0_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : UART1_IRQHandler
* Description    : This function handles the UART1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART1_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : I2C0ITERR_IRQHandler
* Description    : This function handles the I2C0 and DMA Error Interrupt requests
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C0ITERR_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : I2C1ITERR_IRQHandler
* Description    : This function handles the I2C1 and DMA Error Interrupt requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1ITERR_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : I2C0ITDDC_IRQHandler
* Description    : This function handles the I2C0 and the General Interrupt requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C0ITDDC_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : I2C1ITDDC_IRQHandler
* Description    : This function handles the I2C1 and the General Interrupt requests
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1ITDDC_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : CAN0_IRQHandler
* Description    : This function handles the CAN0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN0_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : CAN1_IRQHandler
* Description    : This function handles the CAN1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : CAN2_IRQHandler
* Description    : This function handles the CAN2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN2_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : DMA0_IRQHandler
* Description    : This function handles the DMA0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA0_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : DMA1_IRQHandler
* Description    : This function handles the DMA1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : DMA2_IRQHandler
* Description    : This function handles the DMA2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : DMA3_IRQHandler
* Description    : This function handles the DMA3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA3_IRQHandler (void)
{
	while(1);
}

/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles the ADC interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler (void)
{
  Conversion_Value = ADC_GetConversionValue(ADC_CHANNEL0);
  /* Clear EOC Interrupt pending bit */
  ADC_FlagClear (ADC_FLAG_EOC );
  /* Clear the last Analog Watchdog0 comparison result */
  ADC_FlagClear (ADC_FLAG_AnalogWatchdog0_HighThresold | ADC_FLAG_AnalogWatchdog0_LowThresold);

  ADC_ConversionCmd (ADC_ConversionStop);
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles the RTC interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler (void)
{
	while(1);
}
