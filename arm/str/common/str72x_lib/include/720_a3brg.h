/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_a3brg.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the Asynchronous AHB-APB bridge
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

#ifndef _720_A3BRG_H
#define _720_A3BRG_H

#include "720_lib.h"
/* define the A3BRG Peripherals' clock and reset line masks*/

typedef enum 
  {
  GPIO2_RC = 0x04,
  BSPI1_RC = 0x08,
  BSPI2_RC = 0x10,
  UART1_RC = 0x20,
  UART2_RC = 0x40,
  EFT1_RC  = 0x80,
  EFT2_RC  = 0x100,
  ADC_RC   = 0x200,
  CAN_RC   = 0x400,
  USB_RC   = 0x800
  }A3BRG_Peripherals;
/*******************************************************************************
* Function Name  : A3BRG_ClkConfig
* Description    : This routine is used to enable or disable the clock signal
*                  for the specified peripheral.
* Input          : Periph: the peripheral to enable or disable the clock
*                          signal.
*                          refer to the values defines above for more details on
*                          the allowed values of this parameter.
*
*                  NewState: The reset line Status
* Return         : None
*******************************************************************************/
void A3BRG_ClkConfig( A3BRG_Peripherals Periph, FunctionalState NewState );
/*******************************************************************************
* Function Name  : A3BRG_ResetConfig
* Description    : This routine is used to activate or deactivate the reset
*                  line for the specified peripheral.
* Input          : Periph: the peripheral to activate or deactivate the
*                          reset line.
*                          refer to the values defines above for more details on
*                          the allowed values of this parameter.
*
*                  NewState: The reset line Status
* Return         : None
*******************************************************************************/
void A3BRG_ResetConfig( A3BRG_Peripherals Periph, FunctionalState NewState );
/*******************************************************************************
* Function Name  : A3BRG_ClkEmuConfig
* Description    : This routine is used to configure the status of the clock for
*                  the peripheral controlled by the bridge while the application
*                  is stopped due to a debug request.
* Input          : Periph: the peripheral to activate or deactivate the
*                          reset line.
*                          refer to the values defines above for more details on
*                          the allowed values of this parameter.
*
*                  NewState: The reset line Status
* Return         : None
*******************************************************************************/
void A3BRG_ClkEmuConfig( A3BRG_Peripherals Periph, FunctionalState NewState );
/*******************************************************************************
* Function Name  : A3BRG_AbortError
* Description    : This routine is used to indicate if a previous AHB access has
*                  been aborted because it generates an error.
* Input          : NONE
* Return         : status of the previous access
*                  if 0  : No error
*                     !0 : an error takes place during the pervious access.
*******************************************************************************/
u32 A3BRG_AbortError ( void );
/*******************************************************************************
* Function Name  : A3BRG_ErrorSource
* Description    : This routine is used to return the error source of the
*                  previous access if it has been aborted otherwise it retunrs 0
* Input          : NONE
* Return         : source of the error
*                  if  00 : No error
*                      10 : An access out of memory
*                      40 : an access to a protected peripheral
*                      80 : an access to a non clocked peripheral
*                     100 : an access to a peripheral under reset
*******************************************************************************/
u32 A3BRG_ErrorSource ( void );
/*******************************************************************************
* Function Name  : A3BRG_AccessType
* Description    : This routine is used to return the type of access that
*                  generated the error
* Input          : NONE
* Return         : access type
*                  if 0 : Read Accesss
*                     1 : Write access
*******************************************************************************/
u32 A3BRG_AccessType ( void );
/*******************************************************************************
* Function Name  : A3BRG_AddressError
* Description    : This routine is used to return the address of the location
*                  whose previous access has generated an error.
* Input          : NONE
* Return         : Location address offset
*******************************************************************************/
u32 A3BRG_AddressError ( void );

#endif /* _720_A3BRG_H*/


/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
