/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_prop.h
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : All processing related to Microphone Demo (Endp=0)
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
#ifndef __USB_PROP_H
#define __USB_PROP_H
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Micro_init(void);
void Micro_Reset(void);
void Micro_Status_In (void);
void Micro_Status_Out (void);
RESULT Micro_Data_Setup(u8);
RESULT Micro_NoData_Setup(u8);
RESULT Micro_Get_Interface_Setting(u8 Interface,u8 AlternateSetting);
u8 *Micro_GetDeviceDescriptor(u16 );
u8 *Micro_GetConfigDescriptor(u16);
u8 *Micro_GetStringDescriptor(u16);
/* Exported define -----------------------------------------------------------*/
#define Micro_GetConfiguration          NOP_Process
#define Micro_SetConfiguration          NOP_Process
#define Micro_GetInterface              NOP_Process
#define Micro_SetInterface              NOP_Process
#define Micro_GetStatus                 NOP_Process
#define Micro_ClearFeature              NOP_Process
#define Micro_SetEndPointFeature        NOP_Process
#define Micro_SetDeviceFeature          NOP_Process
#define Micro_SetDeviceAddress          NOP_Process

#endif /* __USB_PROP_H */
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


