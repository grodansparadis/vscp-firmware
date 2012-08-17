/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : 71x_it.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Main Interrupt Service Routines
********************************************************************************
* History:
* 10/01/2006 : V1.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_pwr.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
extern void USB_Istr(void);
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Undefined_Handler
* Description    : This function handles Undefined instruction exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Undefined_Handler(void)
{
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
	/*XTI_PendingBitClear(XTI_InterruptLineValue());
	Resume(RESUME_INTERNAL);*/
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
   CTR_HP();
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
	USB_Istr();
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
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
