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
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define UART0_Rx_Pin (0x0001<<8) /*  TQFP 64: pin N° 63 , TQFP 144 pin N° 143 */
#define UART0_Tx_Pin (0x0001<<9) /*  TQFP 64: pin N° 64 , TQFP 144 pin N° 144 */
/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void UART0_Config_Default(void);
bool UART0_Config(void);
void USB_To_UART_Send_Data(u8* data_buffer, u8 Nb_bytes);
void UART_To_USB_Send_Data(void);
#endif /* __HW_CONFIG_H */

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

