/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : hw_config.h
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Hardware Configuration & Setup
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H
/* Includes ------------------------------------------------------------------*/
#include "usb_type.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define ApplicationAddress 0x00000000UL
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void USB_Cable_Config (FunctionalState NewState);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);

void DFU_Button_Config(void);
u32  DFU_Button_Read(void);

void Reset_Device(void);


void Internal_FLASH_SectorErase(u32 Address);
void Internal_FLASH_WordWrite(u32 Address,u32 Data);
void Internal_FLASH_HalfWordWrite(u32 Address,u16 Data);
/* External variables --------------------------------------------------------*/

#endif  /*__HW_CONFIG_H*/

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
