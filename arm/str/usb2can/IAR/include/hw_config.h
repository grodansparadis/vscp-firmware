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
#ifndef __HW_Config_H
#define __HW_Config_H
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

enum {
  CAN_TX_MSGOBJ = 0,
  CAN_RX_MSGOBJ = 1
};
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

#define BEGIN_RX_STD_OBJ   0
#define END_RX_STD_OBJ    13
#define BEGIN_RX_EXT_OBJ  14
#define END_RX_EXT_OBJ    27
#define GREEN             0
#define RED               1

/*
extern ErrorStatus         OSC4MStartUpStatus;
extern GPIO_InitTypeDef    GPIO_InitStructure;
extern UART_InitTypeDef    UART_InitStructure;
extern EIC_IRQInitTypeDef  EIC_IRQInitStructure;
extern GPIO_InitTypeDef    CAN_RX;
extern GPIO_InitTypeDef    CAN_TX;
extern CAN_InitTypeDef     CAN_Struct;
extern TB_InitTypeDef      TB_InitStructure; 
*/

/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void UART0_Config_Default(void);
bool UART0_Config(void);
void USB_To_UART_Send_Data(u8* data_buffer, u8 Nb_bytes);
void UART_To_USB_Send_Data(void);
extern void Set_TB(void);
extern void CAN_Config(void);
extern void Led_OFF(u8 LED);
extern void Led_ON(u8 LED);
extern void Reset_Device(void);
extern void CAN_ENABLE(void);
extern void CAN_DISABLE(void);
#endif /* __HW_CONFIG_H */
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

