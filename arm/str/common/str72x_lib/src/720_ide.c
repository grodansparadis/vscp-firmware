/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_ide.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the ATAPI-IDE software functions
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
#include "720_ide.h"

/*******************************************************************************
* Function Name  :  IDE_IOConfig
* Description    :  Depending on the status passed as parameter this function is
*                  used to enable or disable the IO used. It also enable
*                  the ATPI to use the Port 7 pin
* Input          : NewState: ENABLE or DISABLE
* Return         : None
*******************************************************************************/
void IDE_IOConfig (FunctionalState NewState)
{
  if ( NewState==ENABLE)
  {
    // Enable I/O in the Command register and Enable the ATAPI-IDE to use port7
    IDE->COMMAND|= IDE_IOMask;
    *((u16 *)(SAPB_SGCR1Add ))|=IDE_P7ASMask   ;
  }
  else
  {
    // Disable I/O in the Command register and Disable the ATAPI-IDE to use port7
    IDE->COMMAND&= ~IDE_IOMask;
    *((u16 *)(SAPB_SGCR1Add ))&= ~IDE_P7ASMask ;
  }
}



/*******************************************************************************
* Function Name  : IDE_CylinderConfig
* Description    : It configure the starting cylinder address for any disk access.
* Input          : cyladd define the value to be written in Cylinder High & low registers
* Return         : None
*******************************************************************************/
void IDE_CylinderConfig (vu16 cyladd)
{
   //write in the Cylinder low register the 8 LSB of the cyladd
  IDE->CYLLOW  = (u8) cyladd;
  //write in the Cylinder high register the 8 MSB of the cyladd
  IDE->CYLHIGH = (u8) (cyladd>>8);
}

/*******************************************************************************
* Function Name  :  IDE_SectorConfig
* Description    :It defines the number of the sectors of data to be transferred
*                 and the Identification number of the first of the first sector
*                 to be accessed.
* Input          : sectorcount: the number of sectors of data
*                  firstsector: the first sector number
* Return         : None
*******************************************************************************/
void IDE_SectorConfig (vu8 sectorcount,vu8 firstsector)
{
  //Write in the Secor Count register the number of sectors of data to be transferred
  IDE->SECCOUNT = sectorcount;
  //Write in the Secor Number register  number of the first sector to be accessed
  IDE->SECNUMB =  firstsector;
}

/*******************************************************************************
* Function Name  :  IDE_Flagstatus
* Description    :  Return the status of the flag passed as parameter
* Input          : flag :it can be one parameter of "IDE_flags" Enumeration
* Return         : None
*******************************************************************************/
FlagStatus IDE_FlagStatus (IDE_Flags flag)
{
    u32 Tmp;
    //test on the flag source register
  switch (flag>>5)
  {
   case 0x0 : Tmp = Status_Register_Base     ;  break;
   case 0x1 : Tmp = Error_Register_Base      ;  break;
   case 0x2 : Tmp = StatusCmd_Register_Base  ;  break;
  }
  //test on the status of the flag passed as paramter
  if ( ((*(u8*)((IDE_BASE) + Tmp))>>((flag&0x1F)) & 0x01) == 1 )
     return SET ;
  else
     return RESET ;
}

/*******************************************************************************
* Function Name  : IDE_FlagClear
* Description    : clear the flag passed as parameter
* Input          : flag :it can be  IDE_MAS, IDE_RTA,IDE_STAS
* Return         : None
*******************************************************************************/
void IDE_FlagClear (IDE_Flags flag)
{

    // test if it is one of the flag that can be cleared
  if ( flag>>5 == 0)
  {
    IDE->STATUS &= ~(0x01<<flag);
  }
}

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/

