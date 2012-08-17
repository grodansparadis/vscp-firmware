/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : main.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Header for the main.c file.
********************************************************************************
* History:
* 09/27/2005 :  V1.0
*******************************************************************************/
/*******************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "73x_lib.h"

/* Exported types ------------------------------------------------------------*/
typedef enum { FAIL = 0, PASS = !FAIL } Status;
/* Exported constants --------------------------------------------------------*/
/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
Status Buffercmp(u32* pBuffer, u32* pBuffer1, u16 BufferLength);

#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
