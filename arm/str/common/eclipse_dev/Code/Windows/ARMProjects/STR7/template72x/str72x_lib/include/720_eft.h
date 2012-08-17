/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_eft.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the EFT software functions headers
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

/* Standard include ----------------------------------------------------------*/
/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __720_EFT_H
#define __720_EFT_H
/* Includes ------------------------------------------------------------------*/
#include "720_lib.h"
/* Exported types ------------------------------------------------------------*/
typedef enum { EXTERNAL,
               INTERNAL
             } EFT_Clocks ;

typedef enum { RISING,
               FALLING
             } Clock_Edges ;

typedef enum { CHANNEL_A,
               CHANNEL_B
             } EFT_Channels ;

typedef enum { TIMING,
               WAVE
             }OC_Modes ;

typedef enum { HIGH,
               LOW
             }Logic_Levels ;

typedef enum { EFT_START,
               EFT_STOP,
               EFT_CLEAR
             }EFT_CounterOperations ;

typedef enum { EFT_ICFA = 0x8000 ,
               EFT_OCFA = 0x4000 ,
               EFT_TOF  = 0x2000 ,
               EFT_ICFB = 0x1000 ,
               EFT_OCFB = 0x0800
             }EFT_Flags ;

/* Exported constants --------------------------------------------------------*/

#define ECKEN_Mask  0x0001
#define EXEDG_Mask  0x0002
#define IEDGA_Mask  0x0004
#define IEDGB_Mask  0x0008
#define PWM_Mask    0x0010
#define OMP_Mask    0x0020
#define OCAE_Mask   0x0040
#define OCBE_Mask   0x0080
#define OLVLA_Mask  0x0100
#define OLVLB_Mask  0x0200
#define FOLVA_Mask  0x0400
#define FOLVB_Mask  0x0800
#define PWMI_Mask   0x4000
#define EN_Mask     0x8000
#define OCBIE_Mask  0x0080
#define ICBIE_Mask  0x1000
#define TOE_Mask    0x2000
#define ICAIE_Mask  0x8000
#define OCAIE_Mask  0x4000


#define ICA_IT   0x8000             // Input Capture Channel A
#define OCA_IT   0x4000             // Output Compare Channel A
#define TO_IT    0x2000             // Timer OverFlow
#define ICB_IT   0x1000             // Input Capture Channel B
#define OCB_IT   0x0800             // Input Capture Channel A


/*******************************************************************************
* Function Name  : EFT_Init
* Description    : This routine is used to Initialize the EFT peripheral
* Input          : EFT Timer to Initialize
* Return         : None
*******************************************************************************/
void EFT_Init(EFT_TypeDef *EFTx);
/*******************************************************************************
* Function Name  : EFT_ClockSourceConfig
* Description    : This routine is used to configure the EFT clock source
* Input          : (1) EFT Timer
*                : (2) EFT_Clocks : Specifies the EFT source clock
*                :    - INTERNAL : The EFT is clocked by the asynchronous APB
*		 :	           frequency divided by the prescaler value.
*                :    - EXTERNAL : The EFT is clocked by an external Clock
* Return         : None
*******************************************************************************/
void EFT_ClockSourceConfig(EFT_TypeDef *EFTx, EFT_Clocks Xclock);
/*******************************************************************************
* Function Name  : EFT_ClockSourceValue
* Description    : This routine is used to get the EFT clock source
* Input          : EFT Timer
* Return         : EFT_Clocks : Specifies the EFT source clock
*                :    - INTERNAL : The EFT is clocked by the asynchronous APB
*                :                 frequency divided by the prescaler value.
*                :    - EXTERNAL : The EFT is clocked by an external Clock
*******************************************************************************/
EFT_Clocks EFT_ClockSourceValue(EFT_TypeDef *EFTx);
/*******************************************************************************
* Function Name  : EFT_PrescalerConfig
* Description    : This routine is used to configure the EFT prescaler value
*                : (using an internal clock)
* Input          : (1) EFT Timer
*                : (2) Prescaler (u8)
* Return         : None
*******************************************************************************/
void EFT_PrescalerConfig(EFT_TypeDef *EFTx, vu8 Xprescaler);
/*******************************************************************************
* Function Name  : EFT_PrescalerValue
* Description    : This routine is used to get the EFT prescaler value
*                : (when using using an internal clock)
* Input          : EFT Timer
* Return         : Prescaler (u8)
*******************************************************************************/
u8 EFT_PrescalerValue(EFT_TypeDef *EFTx);
/*******************************************************************************
* Function Name  : EFT_ClockLevelConfig
* Description    : This routine is used to configure the EFT clock level
*                :  (using an external clock)
* Input          : EFT Timer
*                : Clock_Edges : Specifies the active adge of the external clock
*                :  - RISING  : The rising  edge
*                :  - FALLING : The falling edge
* Return         : None
*******************************************************************************/
void EFT_ClockLevelConfig(EFT_TypeDef *EFTx, Clock_Edges Xedge);
/*******************************************************************************
* Function Name  : EFT_ClockLevelValue
* Description    : This routine is used to get the EFT clock level
* Input          : EFT Timer
* Output         : Clock_Edges : Specifies the active adge of the external clock
*                :  - RISING  : The rising  edge
*                :  - FALLING : The falling edge
*******************************************************************************/
Clock_Edges EFT_ClockLevelValue(EFT_TypeDef *EFTx);
/*******************************************************************************
* Function Name  : EFT_ICAPModeConfig
* Description    : This routine is used to configure the input capture feature
* Input          : (1) EFT Timer
*                : (2) Input Capture Channel (Channel_A or Channel_B)
*                : (3) Active Edge : Rising edge or Falling edge.
* Output         : None
*******************************************************************************/
void EFT_ICAPModeConfig(EFT_TypeDef  *EFTx,
                        EFT_Channels Xchannel,
                        Clock_Edges  Xedge);
/*******************************************************************************
* Function Name  : EFT_ICAPValue
* Description    : This routine is used to get the Input Capture value
* Input          : (1) EFT Timer
*                : (2) Input Capture Channel (Channel_A or Channel_B)
* Output         : None
*******************************************************************************/
u16 EFT_ICAPValue(EFT_TypeDef *EFTx, EFT_Channels Xchannel);
/*******************************************************************************
* Function Name  : EFT_OCMPModeConfig
* Description    : This routine is used to configure the out put compare feature
* Input          : (1) EFT Timer
*                : (2) OCMP Channel (Channel_A or Channel_B)
*                : (3) Pulse Length
*                : (4) OC_Mode : output wave, or only timing .
*                : (5) Level   : Rising edge or Falling edge after the ==
* Output         : None
*******************************************************************************/
void EFT_OCMPModeConfig(EFT_TypeDef  *EFTx,
                        EFT_Channels Xchannel,
                        vu16         XpulseLength,
                        OC_Modes     Xmode,
                        Logic_Levels Xlevel);
/*******************************************************************************
* Function Name  : EFT_CounterConfig
* Description    : This routine is used to configure the the EFT counter
* Input          : (1) EFT Timer
*                : (2) specifies the operation of the counter
* Output         : None
*******************************************************************************/
void EFT_CounterConfig(EFT_TypeDef *EFTx, EFT_CounterOperations Xoperation);
/*******************************************************************************
* Function Name  : EFT_ITConfig
* Description    : This routine is used to configure the EFT IT
* Input          : (1) EFT Timer
*                : (2) EFT interrupt
*                : (2) ENABLE / DISABLE
* Output         : None
*******************************************************************************/
void EFT_ITConfig(EFT_TypeDef *EFTx, vu16 New_IT, FunctionalState NewState);
/*******************************************************************************
* Function Name  : EFT_FlagStatus
* Description    : This routine is used to check whether a Flag is Set.
* Input          : (1) EFT Timer
*                : (2) The EFT FLag
* Output         : Flag Status
*******************************************************************************/
FlagStatus EFT_FlagStatus(EFT_TypeDef *EFTx, EFT_Flags Xflag);
/*******************************************************************************
* Function Name  : EFT_FlagClear
* Description    : This routine is used to clear Flags.
* Input          : (1) EFT Timer
*                : (2) The EFT FLag
* Output         : None
*******************************************************************************/
void EFT_FlagClear(EFT_TypeDef *EFTx, EFT_Flags Xflag);

#endif //__720_EFT_H

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
