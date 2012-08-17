/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_cfg.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      CFG software library.
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
#ifndef __73x_CFG_H
#define __73x_CFG_H

/* Includes ------------------------------------------------------------------*/
#include "73x_map.h"

/* Exported types ------------------------------------------------------------*/
/* The memory to be remapped */
typedef enum
{
  CFG_MEM_RAM = 0x01,
  CFG_MEM_FLASH
} CFG_MEM_TypeDef;

/* Exported constants --------------------------------------------------------*/
#define CFG_CLK_RAM              0
#define CFG_CLK_I2C0             1
#define CFG_CLK_WIU              2
#define CFG_CLK_UART0            4
#define CFG_CLK_UART1            5
#define CFG_CLK_TIM0             6
#define CFG_CLK_TIM1             7
#define CFG_CLK_TB0              8
#define CFG_CLK_CAN0             10
#define CFG_CLK_CAN1             11
#define CFG_CLK_PWM0             12
#define CFG_CLK_PWM1             13
#define CFG_CLK_PWM2             14
#define CFG_CLK_PWM3             15
#define CFG_CLK_PWM4             16
#define CFG_CLK_PWM5             17
#define CFG_CLK_GPIO0            18
#define CFG_CLK_GPIO1            19
#define CFG_CLK_GPIO2            20
#define CFG_CLK_GPIO3            21
#define CFG_CLK_GPIO4            22
#define CFG_CLK_GPIO5            23
#define CFG_CLK_GPIO6            24
#define CFG_CLK_BSPI0            25
#define CFG_CLK_BSPI1            26
#define CFG_CLK_BSPI2            27
#define CFG_CLK_ADC              28
#define CFG_CLK_EIC              29
#define CFG_CLK_WUT              30
#define CFG_CLK_I2C1             32
#define CFG_CLK_TIM5             35
#define CFG_CLK_TIM6             36
#define CFG_CLK_TIM7             37
#define CFG_CLK_TIM8             38
#define CFG_CLK_TIM9             39
#define CFG_CLK_UART2            40
#define CFG_CLK_UART3            41
#define CFG_CLK_TB1              45
#define CFG_CLK_TB2              46
#define CFG_CLK_CAN2             47
#define CFG_CLK_TIM2             48
#define CFG_CLK_TIM3             49
#define CFG_CLK_TIM4             50
#define CFG_CLK_RTC              51
#define CFG_CLK_DMA0             52
#define CFG_CLK_DMA1             53
#define CFG_CLK_DMA2             54
#define CFG_CLK_DMA3             55
#define CFG_CLK_ARB              61
#define CFG_CLK_AHB              62

/* CFG flags */
#define CFG_FLAG_SYS     0x800
#define CFG_FLAG_BOOT    0x400
#define CFG_FLAG_USER1   0x200
#define CFG_FLAG_USER2   0x100
#define CFG_FLAG_JTBT    0x80

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void CFG_RemapConfig(CFG_MEM_TypeDef CFG_Mem );
void CFG_PeripheralClockConfig(u8 CFG_CLK_Periph, FunctionalState NewStatus);
void CFG_EmulationPeripheralClockConfig(u8 CFG_CLK_Periph, FunctionalState NewStatus);
void CFG_PeripheralClockStop(u8 CFG_CLK_Periph);
void CFG_PeripheralClockStart(u8 CFG_CLK_Periph);
u32  CFG_DeviceID(void);
void CFG_FlashPowerOnDelay(u8 Delay);
FlagStatus CFG_FlagStatus(u16 CFG_Flag);

#endif /* __73x_CFG_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
