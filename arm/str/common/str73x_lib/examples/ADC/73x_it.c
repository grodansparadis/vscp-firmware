/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : 73x_it.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Main Interrupt Service Routines
**********************************************************************************
*    This file can be used to describe all the exceptions subroutines
*    that may occur within user application.
*    When an interrupt happens, the software will branch automatically
*    to the corresponding routine according to the interrupt vector
*    loaded in the PC register.
*    The following routines are all empty, user can write code for
*    exceptions handlers and peripherals IRQ interrupts.
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH  CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/
/* Standard include ------------------------------------------------------------*/
#include "73x_lib.h"
/* Include of other module interface headers -----------------------------------*/
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------*/
/* Interface functions ---------------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Undefined_Handler
* Description    : This function Undefined instruction exception.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void Undefined_Handler(void)
{
}
/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : This function handles FIQ exception.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void FIQ_Handler(void)
{
}
/*******************************************************************************
* Function Name  : SWI_Handler
* Description    : This function handles SW exception.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void SWI_Handler(void)
{
}
/*******************************************************************************
* Function Name  : Prefetch_Handler
* Description    : This function handles preftetch abort exception.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void Prefetch_Handler(void)
{
}
/*******************************************************************************
* Function Name  : Abort_Handler
* Description    : This function handles data abort exception.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void Abort_Handler(void)
{
}
/*******************************************************************************
* Function Name  : PRCCUCMU_IRQHandler
* Description    : This function handlesthe PRCCU and the CMU interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void PRCCUCMU_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTIT01_IRQHandler
* Description    : This function handles External line1 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT01_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT02_IRQHandler
* Description    : This function handles External line2 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT02_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT03_IRQHandler
* Description    : This function handles External line3 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT03_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT04_IRQHandler
* Description    : This function handles External line 4 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT04_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT05_IRQHandler
* Description    : This function handles External line 5 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT05_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT06_IRQHandler
* Description    : This function handles External line 6 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT06_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT07_IRQHandler
* Description    : This function handles External line7 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT07_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT08_IRQHandler
* Description    : This function handles External line8 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT08_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT09_IRQHandler
* Description    : This function handles External line9 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT09_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT10_IRQHandler
* Description    : This function handles External line10 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT10_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT11_IRQHandler
* Description    : This function handles External line11 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT11_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT12_IRQHandler
* Description    : This function handles External line12 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT12_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT13_IRQHandler
* Description    : This function handles External line13 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT13_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT14_IRQHandler
* Description    : This function handles External line14 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT14_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : EXTIT15_IRQHandler
* Description    : This function handles External line15 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void EXTIT15_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : DMATRERR_IRQHandler
* Description    : This function handles the DMA transfer error interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void DMATRERR_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TIM1_IRQHandler
* Description    : This function handles the TIM1 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TIM1_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles the TIM2 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TIM2_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles the TIM3 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TIM3_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles the TIM4 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TIM4_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TB0_IRQHandler
* Description    : This function handles the TB0 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TB0_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TB1_IRQHandler
* Description    : This function handles the TB1 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TB1_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TB2_IRQHandler
* Description    : This function handles the TB2 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TB2_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles the TIM5 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TIM5_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles the TIM6 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TIM6_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles the TIM7 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TIM7_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TIM8_IRQHandler
* Description    : This function handles the TIM8 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TIM8_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : TIM9_IRQHandler
* Description    : This function handles the TIM9 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void TIM9_IRQHandler (void)
{
}


/*******************************************************************************
* Function Name  : UART2_IRQHandler
* Description    : This function handles the UART2 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void UART2_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : UART3_IRQHandler
* Description    : This function handles the UART3 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void UART3_IRQHandler (void)
{
}


/*******************************************************************************
* Function Name  : FlashEOP_IRQHandler
* Description    : This function handles the FLASH end of program interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void FLASHEOP_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : PWM0_IRQHandler
* Description    : This function handles the PWM0 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void PWM0_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : PWM1_IRQHandler
* Description    : This function handles the PWM1 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void PWM1_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : PWM2_IRQHandler
* Description    : This function handles the PWM2 interrupt request.
* Input          : detail input  parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void PWM2_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : PWM3_IRQHandler
* Description    : This function handles the PWM3 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void PWM3_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : PWM4_IRQHandler
* Description    : This function handles the PWM4 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void PWM4_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : PWM5_IRQHandler
* Description    : This function handles the PWM5 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void PWM5_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : WIU_IRQHandler
* Description    : This function handles the wake-up system interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void WIU_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : WDGWUT_IRQHandler
* Description    : This function handles the watchdog and the wakeup timer interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void WDGWUT_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : BSPI0_IRQHandler
* Description    : This function handles the BSPI0 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void BSPI0_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  :  BSPI1_IRQHandler
* Description    : This function handles the BSPI1 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void BSPI1_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : BSPI2_IRQHandler
* Description    : This function handles the BSPI2 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void BSPI2_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : UART0_IRQHandler
* Description    : This function handles the UART0 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void UART0_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : UART1_IRQHandler
* Description    : This function handles the UART1 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void UART1_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : I2C0ITERR_IRQHandler
* Description    : This function handles the I2C0 and DMA Error Interrupt requests
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void I2C0ITERR_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : I2C1ITERR_IRQHandler
* Description    : This function handles the I2C1 and DMA Error Interrupt requests.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void I2C1ITERR_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : I2C0ITDDC_IRQHandler
* Description    : This function handles the I2C0 and the General Interrupt requests.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void I2C0ITDDC_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : I2C1ITDDC_IRQHandler
* Description    : This function handles the I2C1 and the General Interrupt requests
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void I2C1ITDDC_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : CAN0_IRQHandler
* Description    : This function handles the CAN0 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void CAN0_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : CAN1_IRQHandler
* Description    : This function handles the CAN1 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void CAN1_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : CAN2_IRQHandler
* Description    : This function handles the CAN2 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void CAN2_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : DMA0_IRQHandler
* Description    : This function handles the DMA0 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void DMA0_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : DMA1_IRQHandler
* Description    : This function handles the DMA1 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void DMA1_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : DMA2_IRQHandler
* Description    : This function handles the DMA2 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void DMA2_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : DMA3_IRQHandler
* Description    : This function handles the DMA3 interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void DMA3_IRQHandler (void)
{
}

/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles the ADC interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void ADC_IRQHandler (void)
{

  GPIO_WordWrite(GPIO4,(ADC_GetConversionValue(ADC_CHANNEL0)));
  /* Clear EOC Interrupt pending bit */
  ADC_FlagClear (ADC_FLAG_EOC ); 

  GPIO_WordWrite(GPIO2, ADC_GetAnalogWatchdogResult (ADC_AnalogWatchdog0)<<6);
  /* Clear the last Analog Watchdog0 comparison result */
  ADC_FlagClear (ADC_FLAG_AnalogWatchdog0_HighThresold | ADC_FLAG_AnalogWatchdog0_LowThresold);
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles the RTC interrupt request.
* Input          : detail input parameters
* Output         : detail output parameters
* Return         : detail return value
*******************************************************************************/
void RTC_IRQHandler (void)
{
}


/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

