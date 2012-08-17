/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_type.h
* Author             : MCD Application Team
* Date First Issued  : 10/27/2003 : V1.0
* Description        : Type definitions used by the STR USB Library
********************************************************************************
* History:
* 09/18/2006 : V3.0
* 09/01/2006 : V2.0
* 10/27/2003 : V1.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_TYPE_H
#define __USB_TYPE_H
/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#ifndef NULL
#define NULL	((void *)0)
#endif

#ifdef STR71x /*STR71x family*/
#ifndef _71x_type_H 
typedef unsigned long   u32;
typedef unsigned short  u16;
typedef unsigned char   u8;

typedef signed long   s32;
typedef signed short  s16;
typedef signed char   s8;

typedef volatile unsigned long   vu32;
typedef volatile unsigned short  vu16;
typedef volatile unsigned char   vu8;

typedef volatile signed long   vs32;
typedef volatile signed short  vs16;
typedef volatile signed char   vs8;

typedef enum { FALSE = 0, TRUE  = !FALSE } bool;
typedef enum { RESET = 0, SET   = !RESET } FlagStatus;
typedef enum { DISABLE = 0, ENABLE  = !DISABLE} FunctionalState;
typedef enum { INDIRECT = 0, DIRECT  = !INDIRECT} RegisterAccess;

#endif
#endif /* End of STR71x family*/
#ifdef STR91x /*STR91x family*/
#ifndef __91x_type_H

typedef long long                u64;
typedef unsigned long            u32;
typedef unsigned short           u16;
typedef unsigned char            u8;

typedef signed long              s32;
typedef signed short             s16;
typedef signed char              s8;

typedef volatile unsigned long   vu32;
typedef volatile unsigned short  vu16;
typedef volatile unsigned char   vu8;

typedef volatile signed long     vs32;
typedef volatile signed short    vs16;
typedef volatile signed char     vs8;

typedef enum { FALSE = 0, TRUE  = !FALSE } bool;
typedef enum { RESET = 0, SET   = !RESET } FlagStatus, ITStatus;
typedef enum { DISABLE = 0, ENABLE  = !DISABLE} FunctionalState;
typedef enum { ERROR = 0, SUCCESS  = !ERROR} ErrorStatus;
#endif 
#endif /* End of STR91x family*/

#ifdef STR75x /*STR75x family*/
#ifndef __75x_TYPE_H

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


typedef enum { FALSE = 0, TRUE  = !FALSE } bool;

typedef enum { RESET = 0, SET   = !RESET } FlagStatus, ITStatus;

typedef enum { DISABLE = 0, ENABLE  = !DISABLE} FunctionalState;

typedef enum { ERROR = 0, SUCCESS  = !ERROR} ErrorStatus;
#endif 
#endif /* End of STR75x family*/


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/

#endif /* __USB_TYPE_H */
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
