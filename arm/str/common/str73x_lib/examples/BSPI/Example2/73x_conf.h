/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_conf.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Library configuration
********************************************************************************
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

#ifndef __73x_CONF_H
#define __73x_CONF_H


/* Comment the line below to compile the library in release mode */
#define DEBUG


/*************************FLASHR*************************/
//#define _FLASHR
/*************************FLASHPR************************/
//#define _FLASHPR
/*************************PRCCU*************************/
#define _PRCCU
/*************************ARB***************************/
#define _ARB
/*************************CFG*************************/
#define _CFG
/*************************I2C*************************/
//#define _I2C
//#define _I2C0
//#define _I2C1
/*************************TB*************************/
//#define _TB
//#define _TB0
//#define _TB1
//#define _TB2
/*************************UART*************************/
//#define _UART
//#define _UART0
//#define _UART1
//#define _UART2
//#define _UART3
/*************************WDG*************************/
//#define _WDG
/*************************WUT*************************/
//#define _WUT
/*************************TIM*************************/
//#define _TIM
//#define _TIM0
//#define _TIM1
//#define _TIM2
//#define _TIM3
//#define _TIM4
//#define _TIM5
//#define _TIM6
//#define _TIM7
//#define _TIM8
//#define _TIM9
/*************************WIU*************************/
//#define _WIU
/*************************CAN*************************/
//#define _CAN
//#define _CAN0
//#define _CAN1
//#define _CAN2
/*************************PWM*************************/
//#define _PWM
//#define _PWM0
//#define _PWM1
//#define _PWM2
//#define _PWM3
//#define _PWM4
//#define _PWM5
/*************************GPIO*************************/
#define _GPIO
//#define _GPIO0
//#define _GPIO1
//#define _GPIO2
//#define _GPIO3
//#define _GPIO4
#define _GPIO5
//#define _GPIO6
/*************************BSPI*************************/
#define _BSPI
//#define _BSPI0
//#define _BSPI1
#define _BSPI2
/*************************DMAC*************************/
#define _DMA
#define _DMA0
//#define _DMA1
//#define _DMA2
//#define _DMA3
/*************************RTC*************************/
//#define _RTC
/*************************CMU*************************/
#define _CMU
/*************************ADC*************************/
//#define _ADC
/*************************EIC*************************/
#define _EIC



#define Main_Oscillator  8000000   /* Value of the Main Quartz in Hz*/

#define RC_oscillator    2340000   /* Typical Reset Value of the Internal RC in Hz */

#endif /* __73x_CONF_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
