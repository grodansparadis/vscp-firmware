/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_prccu.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      PRCCU software library.
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

#ifndef __73x_PRCCU_H
#define __73x_PRCCU_H

/* Includes ------------------------------------------------------------------*/
#include "73x_map.h"

/* Exported types ------------------------------------------------------------*/
/* MCLK clock sources */
typedef enum
{
  PRCCU_MCLKSRC_CLOCK2 ,       /* Select CLOCK2 output     */
  PRCCU_MCLKSRC_CLOCK2_16,     /* Select CLOCK2/16 output  */
  PRCCU_MCLKSRC_PLL            /* Select the PLL output    */
}PRCCU_MCLKSRC;

/* Low power modes */
typedef enum
{
  PRCCU_LPM_HALT,
  PRCCU_LPM_WFI,
  PRCCU_LPM_LPWFI
}PRCCU_LPM;

/* PRCCU Clock output sources */
typedef enum
{
  PRCCU_CLOCK_EXT,
  PRCCU_CLOCK_MCLK
}PRCCU_OUTPUT;

/* PRCCU Init structure define */
typedef struct
{
  FunctionalState   PRCCU_DIV2;             /* Enable or disable the DIV2              */
  PRCCU_MCLKSRC     PRCCU_MCLKSRC_SRC;      /* Select the MCLK source clock            */
  u8                PRCCU_PLLDIV;           /* Configure the PLL divider factor        */
  u8                PRCCU_PLLMUL;           /* Configure the PLL Multiplication factor */
  FunctionalState   PRCCU_FREEN;            /* Enable or disable PLL free running mode */
}PRCCU_InitTypeDef;

/* PRCCU Interrupts Masks */
#define PRCCU_IT_LOCK        0x080
#define PRCCU_IT_STOP        0x400
#define PRCCU_IT_CLK2_16     0x200

/* PRCCU Flags */
#define PRCCU_FLAG_LOCK      0x0002
#define PRCCU_FLAG_SOFTRES   0x0020
#define PRCCU_FLAG_WDGRES    0x0040
#define PRCCU_FLAG_LVD_INT   0x0100
#define PRCCU_FLAG_LOCK_I    0x0800
#define PRCCU_FLAG_CK2_16_I  0x2000
#define PRCCU_FLAG_STOP_I    0x4000
#define PRCCU_FLAG_VROK      0x0004

/* Voltage regulator */
#define PRCCU_VR_LPWFI  0x10
#define PRCCU_VR_Run    0xF7

/* Low Power Voltage Regulator Current Capability values */
#define PRCCU_LPVR_Current_2  0x02
#define PRCCU_LPVR_Current_4  0x04
#define PRCCU_LPVR_Current_6  0x06

/* PLL Multiplication Factor */
#define PRCCU_PLLMUL_12  0x10
#define PRCCU_PLLMUL_16  0x30
#define PRCCU_PLLMUL_20  0x00
#define PRCCU_PLLMUL_28  0x20

/* PLL Divider Factor */
#define PRCCU_PLLDIV_1  0x01
#define PRCCU_PLLDIV_2  0x02
#define PRCCU_PLLDIV_3  0x03
#define PRCCU_PLLDIV_4  0x04
#define PRCCU_PLLDIV_5  0x05
#define PRCCU_PLLDIV_6  0x06
#define PRCCU_PLLDIV_7  0x07

/* DIV2 selection */
#define PRCCU_DIV2_Enable  0x8000
#define PRCCU_DIV2_Disable 0x7FFF

/* CSU clock selection */
#define PRCCU_CSU_CKSEL_Enable  0x01
#define PRCCU_CSU_CKSEL_Disable 0xFFFFFFFE

/* CK2_16 selection */
#define PRCCU_CK2_16_Enable   0xFFFFFFF7
#define PRCCU_CK2_16_Disable  0x08

/* PLL turn off */
#define PRCCU_PLL_SwitchOff 0x07

/* Free running mode for PLL selection */
#define PRCCU_FREEN_Enable  0x00000080
#define PRCCU_FREEN_Disable 0xFFFFFF7F

/* PLL input clock range selection */
#define PRCCU_FREF_RANGE_High 0x40
#define PRCCU_FREF_RANGE_Low  0xFFFFFFBF

/* PRCCU registers masks */
#define PRCCU_CFR_Mask    0x00008008 
#define PRCCU_CFR_Mask1   0xFFFFFFFE 
#define PRCCU_PLLCR_Mask  0x0 

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void PRCCU_DeInit(void);
void PRCCU_Init(PRCCU_InitTypeDef* PRCCU_InitStruct);
void PRCCU_StructInit(PRCCU_InitTypeDef* PRCCU_InitStruct);
void PRCCU_EnterLPM (PRCCU_LPM NewLPM);
u32 PRCCU_GetFrequencyValue (PRCCU_OUTPUT PRCCU_CLOCK_Out);
void  PRCCU_SetExtClkDiv(u16 ExtClkDiv);
void PRCCU_LPVRCurrentConfig (u8 Current_Capability);
void PRCCU_ITConfig(u16 PRCCU_IT, FunctionalState NewState);
FlagStatus PRCCU_FlagStatus(u16 PRCCU_Flag);
void PRCCU_FlagClear (u16 PRCCU_Flag);
void PRCCU_SwResetGenerate(void);
void PRCCU_VRCmd (u8 V_Regulator, FunctionalState NewState);


#endif /* __73x_PRCCU_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
