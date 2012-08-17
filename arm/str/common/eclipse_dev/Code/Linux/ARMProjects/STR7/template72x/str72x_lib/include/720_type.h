/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
********************************************************************************
* File Name          : 720_type.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : It contains common types and constants used in all the
*                      peripherals' drivers
**********************************************************************************
* History:
* 12/20/2004 :  V1.1
* 08/02/2004 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH  CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/
#ifndef __720_type_H
#define __720_type_H

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

  /*===================================================================*/
  typedef enum { FALSE = 0, TRUE  = !FALSE } bool;
  /*===================================================================*/
  typedef enum { RESET = 0, SET   = !RESET } FlagStatus;
  /*===================================================================*/
  typedef enum { DISABLE = 0, ENABLE  = !DISABLE} FunctionalState;
  /*===================================================================*/

#endif /* __720_type_H */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
