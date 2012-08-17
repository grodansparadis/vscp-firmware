/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_emi.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file contains all the functions prototypes for the
*                      EMI software library.
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

#ifndef __720_EMI_H
#define __720_EMI_H

#include "720_lib.h"
// EMI External ready Control parameters
// S-APB Global Configuration Register 1 (SGCR1) base address
#define SGCR1_Base     0xF0000C00

// Mask for enablinf each bank
#define EMI_En_Mask    0x20

// Mask for enabling the EMI interface to use the Por7
#define EMI_Port7_Mask 0x02

//Mask for the internal synchronisation option
#define EMI_Sync   0x01
//Mask for the external ready option
#define EMI_Eready 0x02



/*******************************************************************************
* Function Name  : EMI_Init
* Description    : This routine is used to initiate all the EMI registers
*                : to their reset Value.
* Input          : None
* Return         : None
*******************************************************************************/
void EMI_Init (void);

/*******************************************************************************
* Function Name  : EMI_CycleLengthConfig
* Description    : This routine is used, for the selected bank, to configure
*                : the number of wait state
* Input          : Bank (u8) : 0 or 1
*                : Waitstate (u8):   0 < Waitstate < 7
* Return         : None
*******************************************************************************/
static inline void EMI_CycleLengthConfig (vu8 Bank, vu8 Waitstate)
{
  // Configure the number of the wait state
  *(u16 *)(EMI_BASE + (4*Bank)) |= (Waitstate<<2);
}

/*******************************************************************************
* Function Name  : EMI_BankLengthConfig
* Description    : This routine is used, for the selected bank, to configure
*                : the bank length and to define when Bank 1 is selected the bank base
* Input          : Bank (u8) : 0 or 1
*                : Bank_length (u16) : 64K < Bank_length < 8192K
*                input 3: an (u32) defining the base start address (0x4xxxxxxx)
* Return         : None
*******************************************************************************/
void EMI_BankLengthConfig (vu8 Bank, vu16 Bank_length,...);

/*******************************************************************************
* Function Name  : EMI_ExternalReadyConfig
* Description    : This routine is used, for the selectzed bank, to enable
*                : the external ready Control with or without internal synchronization.
* Input          : Bank (u8)
*                : Eready can be EMI_Eready or/and EMI_Sync
* Return         : None
*******************************************************************************/
static inline void EMI_ExternalReadyConfig (vu8 Bank, vu8 Eready)
{
  //Enable the External ready Control with or without the internal synchronization
  *(u16 *)(EMI_BASE + (4*Bank)) |= Eready<<6;
}

/*******************************************************************************
* Function Name  : EMI_BankConfig
* Description    : This routine is used to enable the EMI on the selected
*                : the bank and to enable the EMI to use the Port 7.
* Input          : Bank (vu8)
*                : NewState
* Return         : None
*******************************************************************************/
void EMI_BankConfig (vu8 Bank, FunctionalState NewState);
#endif // __720_EMI_H

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
