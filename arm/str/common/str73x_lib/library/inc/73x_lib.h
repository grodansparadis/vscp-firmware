/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_lib.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file includes the peripherals header files in the
*                      user application.
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

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __73x_LIB_H
#define __73x_LIB_H

/* Includes ------------------------------------------------------------------*/
#include "73x_map.h"
#include "73x_conf.h"


#ifdef _FLASHR
#include "73x_flash.h"
#endif /*_FLASHR */

#ifdef _PRCCU
#include "73x_prccu.h"
#endif /*_PRCCU */

#ifdef _CFG
#include "73x_cfg.h"
#endif /*CFG */

#ifdef _I2C
#include "73x_i2c.h"
#endif /*_I2C */

#ifdef _TB
#include "73x_tb.h"
#endif /*_TB */

#ifdef _UART
#include "73x_uart.h"
#endif /*_UART */

#ifdef _WDG
#include "73x_wdg.h"
#endif /*_WDG */

#ifdef _WUT
#include "73x_wut.h"
#endif /*_WUT */

#ifdef _TIM
#include "73x_tim.h"
#endif /*_TIM */

#ifdef _WIU
#include "73x_wiu.h"
#endif /*_WIU */

#ifdef _CAN
#include "73x_can.h"
#endif /*CAN */

#ifdef _PWM
#include "73x_pwm.h"
#endif /*_PWM */

#ifdef _GPIO
#include "73x_gpio.h"
#endif /*_GPIO*/

#ifdef _BSPI
#include "73x_bspi.h"
#endif /*_BSPI */

#ifdef _DMA
#include "73x_dma.h"
#endif /*_DMA */

#ifdef _RTC
#include "73x_rtc.h"
#endif /*_RTC */

#ifdef _CMU
#include "73x_cmu.h"
#endif /*_CMU */

#ifdef _ADC
#include "73x_adc.h"
#endif /*_ADC */

#ifdef _EIC
#include "73x_eic.h"
#endif /*_EIC */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void  debug(void);

#endif /* __73x_LIB_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
