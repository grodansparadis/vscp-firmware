/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_agcr.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the AGCR software functions headers
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

#ifndef _720_AGCR_H
#define _720_AGCR_H
/* Includes ------------------------------------------------------------------*/
#include "720_lib.h"
/* Exported types ------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : AGCR_Init
* Description    : Init AGCR register
* Input 1        : None
* Input 2        : None
* Output         : None
* Return         : None
*******************************************************************************/
void AGCR_Init(void);
/*******************************************************************************
* Function Name  : AGCR_ADCStopConfig
* Description    : Stop/Run ADC Operations
* Input 1        : ENABLE/DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void AGCR_ADCStopConfig(FunctionalState Xstatus);
/*******************************************************************************
* Function Name  : AGCR_ADCOffConfig
* Description    : Switch off/on ADC Analog section
* Input 1        : ENABLE/DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void AGCR_ADCOffConfig(FunctionalState Xstatus);

#endif /* _720_AGCR_H */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
