/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_cgc.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the Clock Gataing Control block
*                      software functions headers
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

/* Includes ------------------------------------------------------------------*/
#ifndef __720_CGC_H
#define __720_CGC_H

#include "720_lib.h"
/* define the Peripherals clcok and reset line masks*/
/*AHB/S-APB peripherals*/

typedef enum
  {
  EMI_RC     =  0x01,          // External Memory interface
  DRAMC_RC   =  0x02,          // SDRAM Controller
  DMAC_RC    =  0x04,          // DMA controller
  ATAPI_RC   =  0x08,          // ATAPI-AHB
  EIC_RC     =  0x20,          // Enhanced Interrupt Controller
  GPIO3_RC   =  0x40,          // GPIO3
  GPIO4_RC   =  0x80,          // GPIO4
  WIU_RC     =  0x400,         // WakeUp Interrupt Unit
  RTC_RC     =  0x800,         // RTC-APB
  ATAPIK_RC  =  0x20000,       // ATAPI-Kernel
  USBK_RC    =  0x40000        // USB-Kernel
  }CGC_Peripherals;


/*******************************************************************************
* Function Name  : CGC_ClkConfig
* Description    : This routine is used to enbales or disables the clock signal 
*                :  for the specified AHB/SAPB or dedicated peripheral.
* Input          : the peripheral to enable the clock signal for it.
*                : the New State of the peripheral clock
* Return         : None
*******************************************************************************/
void CGC_ClkConfig( CGC_Peripherals Periph, FunctionalState NewState );
/*******************************************************************************
* Function Name  : CGC_ResetConfig
* Description    : This routine is used to activate or deactivate the reset
*                : line for the specified AHB/SAPB or the dedicated peripheral.
* Input          : the peripheral to activate or deactivate the reset line.
*                : The reset line New State
* Return         : None
*******************************************************************************/
void CGC_ResetConfig( CGC_Peripherals Periph, FunctionalState NewState );
/*******************************************************************************
* Function Name  : CGC_ClkEmuConfig
* Description    : This routine is used to configure the status of the clock
*                : for the specified AHB/SAPB or the dedicated peripheral while
*                : the application program execution is stopped due to a debug
*                : request.
* Input          : the peripheral to enable or disable the clock during the
*                : debug mode.
* Return         : None
*******************************************************************************/
void CGC_ClkEmuConfig( CGC_Peripherals Periph, FunctionalState NewState );

#endif /*__720_CGC_H*/


/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
