/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_cmu.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      CMU software library.
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
#ifndef __73x_CMU_H
#define __73x_CMU_H

/* Includes ------------------------------------------------------------------*/
#include "73x_map.h"

/* Exported types ------------------------------------------------------------*/
typedef struct{
  u8 CMU_RCOscControl;  /*Adjusts the frequency of Rc oscillator */
  u8 CMU_EndCountValue; /*End of count value */
  u16 CMU_FreqRef_High; /*Frequency reference high bits */
  u16 CMU_FreqRef_Low;  /*Frequency reference low bits */
  u16 CMU_CKSEL0;       /*RC-Oscillator selector bit */
  u16 CMU_CKSEL1;       /*Oscillator-PLL selector bit */
  u16 CMU_CKSEL2;       /*CK selector bit */
}CMU_InitTypeDef;

/* Exported constants --------------------------------------------------------*/
/*CMU CKOUT = CKRC or CKOSC*/
#define CMU_CKSEL0_CKOSC  0x0001
#define CMU_CKSEL0_CKRC   0x0000

/*CMU Frequency Meter clock = CKPLL or CKOSC*/
#define CMU_CKSEL1_CKPLL 0x0002
#define CMU_CKSEL1_CKOSC 0x0000

/*CMU Digital logic clock = CKRC or CKOSC*/
#define CMU_CKSEL2_CKOSC 0x0004
#define CMU_CKSEL2_CKRC  0x0000

/*CMU Reset Enable/Disable*/
#define CMU_Reset_Enable  0x0008 
#define CMU_Reset_Disable 0xFFF7

/*CMU Start Frequency Measure*/
#define CMU_StartFreqMeas 0x0010

/* ReadyFrequencybit*/
#define CMU_SFMbit 0x0010

/*Select CMU_Mode*/
#define CMU_Stop_High   0xFF7F
#define CMU_Stop_Low    0x0080 
#define CMU_Run_Low     0x0100   
#define CMU_Run_High    0xFEFF   

/*Select CMU_IT */
#define CMU_IT_OSCLessRC      0x01       
#define CMU_IT_EndCounter     0x02
#define CMU_IT_FreqLessLowRef 0x04
#define CMU_IT_Reset          0x08

/*Select Stop oscillator*/
#define CMU_RCoscSoftStop 0x0020
#define CMU_RCoscHardStop 0x0040
#define CMU_MainoscStop   0x0400

/*Flag Status*/
#define CMU_FLAG_CKON0bit 0x01
#define CMU_FLAG_CKON1bit 0x02
#define CMU_FLAG_CKON2bit 0x04
#define CMU_FLAG_RONbit   0x08
#define CMU_FLAG_CRFbit   0x10


/*CMU Clear Flag Reset */
#define CMU_ClearCRFbit 0x0200

/*Sequence*/
#define CMU_KeyValue1 0x50FA
#define CMU_KeyValue2 0xAF05

/*Initialize register*/
#define CMU_RCCTL_MASK 0x0008 
#define CMU_FDISP_MASK 0x0000 
#define CMU_FRH_MASK   0x0FFF
#define CMU_FRL_MASK   0x0000
#define CMU_CTRL_MASK  0xFFF8

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void CMU_DeInit(void);
void CMU_StructInit(CMU_InitTypeDef *CMU_InitStruct);
void CMU_Init(CMU_InitTypeDef *Init_Structure);
void CMU_ITConfig(u8 CMU_IT, FunctionalState NewState);
void CMU_StopOscConfig(u16 CMU_StopOsc);
void CMU_ResetConfig(FunctionalState NewState);
u16 CMU_GetOSCFrequency(void);
void  CMU_Lock(FunctionalState NewState);
void CMU_ModeOscConfig(u16 CMU_OSCMode);
void CMU_ITClear(u16 CMU_ITClear);
void CMU_FlagClear(void);
FlagStatus CMU_FlagStatus (u8 CMU_Flag);

#endif /* __73x_CMU_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

