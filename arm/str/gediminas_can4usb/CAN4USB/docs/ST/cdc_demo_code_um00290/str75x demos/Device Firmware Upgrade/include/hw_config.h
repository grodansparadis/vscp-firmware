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
/* Exported types ------------------------------------------------------------*/
typedef struct
{
  vu32 CR0;
  vu32 CR1;
  vu32 DR0;
  vu32 DR1;
  vu32 AR;
  vu32 ER;
} FLASHR_TypeDef;

typedef struct
{
  vu32 NVWPAR;
  vu32 EMPTY;
  vu32 NVAPR0;
  vu32 NVAPR1;
} FLASHPR_TypeDef;

/* Exported constants --------------------------------------------------------*/
#define FLASHR_BASE        0x20100000
#define FLASHPR_BASE       0x2010DFB0
#define ApplicationAddress 0x20004000


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Cable_Config (FunctionalState NewState);
void USB_Interrupts_Config(void);

void DFU_Button_Config(void);
u8  DFU_Button_Read(void);

void Reset_Device(void);

void Internal_FLASH_SectorErase(u32 Sectors);
void Internal_FLASH_WordWrite(u32 Address, u32 Data);
void Internal_FLASH_WritePrConfig(u32 Xsectors, FunctionalState NewState);
u32  Internal_FLASH_SectorMask(u32 Address);

void SMI_FLASH_Init(void);
void SMI_FLASH_SectorErase(u32 Address);
void SMI_FLASH_WordWrite(u32 Address,u32 Data);
void SMI_FLASH_PageWrite(u32 Address,u32* wBuffer);
/* External variables --------------------------------------------------------*/

#endif  /*__HW_CONFIG_H*/

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
