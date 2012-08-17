/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_emi.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the EMI software functions
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

#include "720_emi.h"

/*******************************************************************************
* Function Name  : EMI_Init
* Description    : This routine is used to initiate all the EMI registers
*                : to their reset Value.
* Input          : None
* Return         : None
*******************************************************************************/
void EMI_Init (void)
{
  // Initiate EMI register to their reset value
    EMI->BCON0 = EMI->BCON1 = 0x01;
    EMI->BBASE1 = 0x0;
}

/*******************************************************************************
* Function Name  : EMI_BankLengthConfig
* Description    : This routine is used, for the selected bank, to configure
*                : the bank length and to define when Bank 1 is selected the bank base
* Input          : Bank (vu8) : 0 or 1
*                : Bank_length (vu16) : 64K < Bank_length < 8192K
*                input 3: an (vu32) defining the base start address (0x4xxxxxxx)
* Return         : None
*******************************************************************************/
void EMI_BankLengthConfig (vu8 Bank, vu16 Bank_length,...)
{  u32 Tmp;
  // Configure the selected bank length
  *(vu16 *)(EMI_BASE + (4*Bank)) |= ((Bank_length/64)-1)<<8;

  // Configure the Bank1 base when bank is passed as parameter
  if ( Bank == 1)
  {
    Tmp = (vu32)*((vu32 *)&Bank_length + sizeof(Bank_length));
    EMI->BBASE1 = Tmp>>11;
  }
}

/*******************************************************************************
* Function Name  : EMI_BankEn
* Description    : This routine is used to enable the EMI on the selected
*                : the bank and to enable the EMI to use the Port 7.
* Input          : Bank (vu8)
*                : NewState
* Return         : None
*******************************************************************************/
void EMI_BankConfig (vu8 Bank, FunctionalState NewState)
{
  if (NewState == ENABLE )
  {
     //Enable the EMI interface to use the Port7 pin
     *(vu16*)SGCR1_Base &= ~EMI_Port7_Mask;
          //Enable the selected bank
     *(vu16 *)(EMI_BASE + (4*Bank)) |= EMI_En_Mask;
  }
  else
  {
     //Disable the EMI interface to use the Port7 pin
     *(vu16*)SGCR1_Base |= EMI_Port7_Mask;
     //Disable the selected bank
     *(vu16 *)(EMI_BASE + (4*Bank)) &= ~EMI_En_Mask;
  }
}

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
