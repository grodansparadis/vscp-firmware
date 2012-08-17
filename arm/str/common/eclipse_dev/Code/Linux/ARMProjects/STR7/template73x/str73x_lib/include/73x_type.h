/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_type.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the common data types used for the
                       STR73x software library
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
#ifndef __73x_TYPE_H
#define __73x_TYPE_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

typedef signed long      s32;
typedef signed short     s16;
typedef signed char      s8;

typedef volatile signed long      vs32;
typedef volatile signed short     vs16;
typedef volatile signed char      vs8;

typedef unsigned long       u32;
typedef unsigned short      u16;
typedef unsigned char       u8;

typedef volatile unsigned long      vu32;
typedef volatile unsigned short     vu16;
typedef volatile unsigned char      vu8;

typedef volatile unsigned long  const    vuc32;  /* Read Only */
typedef volatile unsigned short const    vuc16;  /* Read Only */ 
typedef volatile unsigned char  const    vuc8;   /* Read Only */

/*===================================================================*/
typedef enum { FALSE = 0, TRUE  = !FALSE } bool;
/*===================================================================*/
typedef enum { RESET = 0, SET   = !RESET } FlagStatus;
/*===================================================================*/
typedef enum { DISABLE = 0, ENABLE  = !DISABLE} FunctionalState;
/*===================================================================*/
typedef enum { ERROR = 0, SUCCESS  = !ERROR} ErrorStatus;
/*===================================================================*/

/* Exported constants --------------------------------------------------------*/
/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __73x_TYPE_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
