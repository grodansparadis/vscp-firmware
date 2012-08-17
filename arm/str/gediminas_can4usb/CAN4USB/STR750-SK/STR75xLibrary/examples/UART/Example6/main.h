/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.h
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Header for main.c module
********************************************************************************
* History:
* 07/17/2006 : V1.0
* 03/10/2006 : V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "75x_lib.h"
#include "stdio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define USE_UART0
//#define USE_UART1
//#define USE_UART2

#ifdef USE_UART0
  #define  UARTx  UART0
  #define  MRCC_Peripheral_UARTx MRCC_Peripheral_UART0
  #define  GPIO_RxPin GPIO_Pin_10
  #define  GPIO_TxPin GPIO_Pin_11
#endif /* Use_UART0 */

#ifdef USE_UART1
  #define  UARTx  UART1
  #define  MRCC_Peripheral_UARTx MRCC_Peripheral_UART1
  #define  GPIO_RxPin GPIO_Pin_20
  #define  GPIO_TxPin GPIO_Pin_21
#endif /* Use_UART1 */

#ifdef USE_UART2
  #define  UARTx  UART2
  #define  MRCC_Peripheral_UARTx MRCC_Peripheral_UART2
  #define  GPIO_RxPin GPIO_Pin_24
  #define  GPIO_TxPin GPIO_Pin_25
#endif /* Use_UART2 */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
