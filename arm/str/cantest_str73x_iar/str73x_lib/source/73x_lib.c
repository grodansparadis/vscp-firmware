/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_lib.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all peripherals pointers initialization.
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
* CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/
#define EXT

/* Standard include ----------------------------------------------------------*/
#include "73x_lib.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#ifdef DEBUG

/*******************************************************************************
* Function Name  : debug
* Description    : this function initialize peripherals pointers
* Input          : no one
* Output         : no one
* Return         : no one
*******************************************************************************/
void debug(void)
{

/*************************FLASHR*************************/

#ifdef _FLASHR
  FLASHR   = (FLASHR_TypeDef *) FLASHR_BASE;
#endif /*_FLASHR */

/*************************FLASHPR************************/

#ifdef _FLASHPR
  FLASHPR   = (FLASHPR_TypeDef *) FLASHPR_BASE;
#endif /*_FLASHPR */

/*************************PRCCU**************************/

#ifdef _PRCCU
  PRCCU   = (PRCCU_TypeDef  *)PRCCU_BASE;
#endif /*_PRCCU */

/*************************ARB****************************/

#ifdef _ARB
  ARB = (ARB_TypeDef *) ARB_BASE  ;
#endif /*_ARB */

/*************************CFG*************************/

#ifdef _CFG
 CFG   = (CFG_TypeDef  *)CFG_BASE;
#endif /*CFG */

/*************************I2C*************************/

#ifdef _I2C0
I2C0   = (I2C_TypeDef  *)I2C0_BASE;
#endif /*_I2C */

#ifdef _I2C1
I2C1   = (I2C_TypeDef  *)I2C1_BASE;
#endif /*_I2C */

/*************************TB*************************/

#ifdef _TB0
TB0   = (TB_TypeDef  *)TB0_BASE;
#endif /*_TB0 */

#ifdef _TB1
TB1   = (TB_TypeDef  *)TB1_BASE;
#endif /*_TB1 */

#ifdef _TB2
TB2   = (TB_TypeDef  *)TB2_BASE;
#endif /*_TB2 */

/*************************UART*************************/

#ifdef _UART0
UART0   = (UART_TypeDef  *)UART0_BASE;
#endif /*_UART0 */

#ifdef _UART1
UART1   = (UART_TypeDef  *)UART1_BASE;
#endif /*_UART1 */

#ifdef _UART2
UART2   = (UART_TypeDef  *)UART2_BASE;
#endif /*_UART2 */

#ifdef _UART3
UART3   = (UART_TypeDef  *)UART3_BASE;
#endif /*_UART3 */

/*************************WDG*************************/
#ifdef _WDG
WDG   = (WDG_TypeDef  *)WDG_BASE;
#endif /*_WDG */
/*************************WUT*************************/
#ifdef _WUT
WUT   = (WUT_TypeDef  *)WUT_BASE;
#endif /*_WUT */
/*************************TIM*************************/

#ifdef _TIM0
TIM0   = (TIM_TypeDef  *)TIM0_BASE;
#endif /*_TIM0 */

#ifdef _TIM1
TIM1   = (TIM_TypeDef  *)TIM1_BASE;
#endif /*_TIM1 */

#ifdef _TIM2
TIM2   = (TIM_TypeDef  *)TIM2_BASE;
#endif /*_TIM2 */

#ifdef _TIM3
TIM3   = (TIM_TypeDef  *)TIM3_BASE;
#endif /*_TIM3 */

#ifdef _TIM4
TIM4   = (TIM_TypeDef  *)TIM4_BASE;
#endif /*_TIM4 */

#ifdef _TIM5
TIM5   = (TIM_TypeDef  *)TIM5_BASE;
#endif /*_TIM5 */

#ifdef _TIM6
TIM6   = (TIM_TypeDef  *)TIM6_BASE;
#endif /*_TIM6 */

#ifdef _TIM7
TIM7   = (TIM_TypeDef  *)TIM7_BASE;
#endif /*_TIM7 */

#ifdef _TIM8
TIM8   = (TIM_TypeDef  *)TIM8_BASE;
#endif /*_TIM8 */

#ifdef _TIM9
TIM9   = (TIM_TypeDef  *)TIM9_BASE;
#endif /*_TIM9 */

/*************************WIU*************************/

#ifdef _WIU
WIU   = (WIU_TypeDef  *)WIU_BASE;
#endif /*_WIU */

/*************************CAN*************************/

#ifdef _CAN0
CAN0   = (CAN_TypeDef  *)CAN0_BASE;
#endif /*CAN0 */

#ifdef _CAN1
CAN1   = (CAN_TypeDef  *)CAN1_BASE;
#endif /*CAN1 */

#ifdef _CAN2
CAN2   = (CAN_TypeDef  *)CAN2_BASE;
#endif /*CAN2 */

/*************************PWM*************************/

#ifdef _PWM0
PWM0   = (PWM_TypeDef  *)PWM0_BASE;
#endif /*_PWM0 */

#ifdef _PWM1
PWM1   = (PWM_TypeDef  *)PWM1_BASE;
#endif /*_PWM1 */

#ifdef _PWM2
PWM2   = (PWM_TypeDef  *)PWM2_BASE;
#endif /*_PWM2 */

#ifdef _PWM3
PWM3   = (PWM_TypeDef  *)PWM3_BASE;
#endif /*_PWM3 */

#ifdef _PWM4
PWM4   = (PWM_TypeDef  *)PWM4_BASE;
#endif /*_PWM4 */

#ifdef _PWM5
PWM5   = (PWM_TypeDef  *)PWM5_BASE;
#endif /*_PWM5 */

/*************************GPIO*************************/

#ifdef _GPIO0
GPIO0   = (GPIO_TypeDef  *)GPIO0_BASE;
#endif /*_GPIO0*/

#ifdef _GPIO1
GPIO1   = (GPIO_TypeDef  *)GPIO1_BASE;
#endif /*_GPIO1*/

#ifdef _GPIO2
GPIO2   = (GPIO_TypeDef  *)GPIO2_BASE;
#endif /*_GPIO2*/

#ifdef _GPIO3
GPIO3   = (GPIO_TypeDef  *)GPIO3_BASE;
#endif /*_GPIO3*/

#ifdef _GPIO4
GPIO4   = (GPIO_TypeDef  *)GPIO4_BASE;
#endif /*_GPIO4*/

#ifdef _GPIO5
GPIO5   = (GPIO_TypeDef  *)GPIO5_BASE;
#endif /*_GPIO5*/

#ifdef _GPIO6
GPIO6   = (GPIO_TypeDef  *)GPIO6_BASE;
#endif /*_GPIO6*/

/*************************BSPI*************************/

#ifdef _BSPI0
BSPI0   = (BSPI_TypeDef  *)BSPI0_BASE;
#endif /*_BSPI0 */

#ifdef _BSPI1
BSPI1   = (BSPI_TypeDef  *)BSPI1_BASE;
#endif /*_BSPI1 */

#ifdef _BSPI2
BSPI2   = (BSPI_TypeDef  *)BSPI2_BASE;
#endif /*_BSPI2 */

/*************************DMA*************************/

#ifdef _DMA0
DMA0  = (DMA_TypeDef  *)DMA0_BASE;
#endif /*_DMA0 */

#ifdef _DMA1
DMA1  = (DMA_TypeDef  *)DMA1_BASE;
#endif /*_DMA1 */

#ifdef _DMA2
DMA2  = (DMA_TypeDef  *)DMA2_BASE;
#endif /*_DMA2 */

#ifdef _DMA3
DMA3  = (DMA_TypeDef  *)DMA3_BASE;
#endif /*_DMA3 */

/*************************RTC*************************/

#ifdef _RTC
RTC   = (RTC_TypeDef  *)RTC_BASE;
#endif /*_RTC */

/*************************CMU*************************/

#ifdef _CMU
CMU   = (CMU_TypeDef  *)CMU_BASE;
#endif /*_CMU */

/*************************ADC*************************/

#ifdef _ADC
ADC   = (ADC_TypeDef  *)ADC_BASE;
#endif /*_ADC */

/*************************EIC*************************/

#ifdef _EIC
EIC   = (EIC_TypeDef  *)EIC_BASE;
#endif /*_EIC */

}
#endif  /* DEBUG */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
