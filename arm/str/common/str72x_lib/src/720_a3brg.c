/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_a3brg.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the ASynchronous AHB-APB bridge
*                      functions source code
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
#include "720_a3brg.h"
/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


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
void A3BRG_ClkConfig( A3BRG_Peripherals Periph, FunctionalState NewState )
{
    A3BRG->PCG0 = (NewState == ENABLE)?  (A3BRG->PCG0) | (Periph | 0x1) :\
                                         (A3BRG->PCG0) & (~Periph);
}

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
void A3BRG_ResetConfig( A3BRG_Peripherals Periph, FunctionalState NewState )
{
    A3BRG->PUR0 = (NewState == DISABLE)?  (A3BRG->PUR0) | (Periph | 0x1) :\
                                          (A3BRG->PUR0) & (~Periph) ;
}
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
*                  Xstatus: The reset line Status
* Return         : None
*******************************************************************************/
void A3BRG_ClkEmuConfig( A3BRG_Peripherals Periph, FunctionalState NewState )
{
    A3BRG->PUR0 = (NewState == DISABLE)?  (A3BRG->PUR0) | (Periph | 0x1) :\
                                          (A3BRG->PUR0) & (~Periph);
}
/*******************************************************************************
* Function Name  : A3BRG_AbortError
* Description    : This routine is used to indicate if a previous AHB access has
*                  been aborted because it generates an error.
* Input          : NONE
* Return         : status of the previous access
*                  if 0  : No error
*                     !0 : an error takes place during the pervious access.
*******************************************************************************/
u32 A3BRG_AbortError ( void )
{
    return (A3BRG->BSR & 0x1);
}

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
u32 A3BRG_ErrorSource ( void )
{
    return (A3BRG->BSR & 0x1F0);
}
/*******************************************************************************
* Function Name  : A3BRG_AccessType
* Description    : This routine is used to return the type of access that
*                  generated the error
* Input          : NONE
* Return         : access type
*                  if 0 : Read Accesss
*                     1 : Write access
*******************************************************************************/
u32 A3BRG_AccessType ( void )
{
    return (A3BRG->PAER & 0x01000000);
}

/*******************************************************************************
* Function Name  : A3BRG_AddressError
* Description    : This routine is used to return the address of the location
*                  whose previous access has generated an error.
* Input          : NONE
* Return         : Location address offset
*******************************************************************************/
u32 A3BRG_AddressError ( void )
{
    return ((A3BRG->PAER & 0x00FFFFFF)|0xF0000000);
}
/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
