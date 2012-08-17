/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_ide.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file contains all the functions prototypes for the
*                      ATAPI-IDE software library.
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

#ifndef __720_IDE_H
#define __720_IDE_H
/* Includes ------------------------------------------------------------------*/
/* Standard include ----------------------------------------------------------*/
#include "720_lib.h"
/* Private variables ---------------------------------------------------------*/
// Enumiration of the identification register in the Configuration registers.
typedef enum
{
  IDE_VENID    = 0x100,
  IDE_DEVID    = 0x102,
  IDE_REVID    = 0x108,
  IDE_CCPROG   = 0x109,
  IDE_CLASSCODE= 0x10A,
  IDE_HEADTYPE = 0x10E
} IDE_IDentifier;
// Enumeration of the IOport registers
typedef enum
{
  IDE_DATA      = 0x10,
  IDE_ERROR     = 0x11,
  IDE_SECCOUNT  = 0x12,
  IDE_SECNUMB   = 0x13,
  IDE_CYLLOW    = 0x14,
  IDE_CYLHIGH   = 0x15,
  IDE_DEVHEAD   = 0x16,
  IDE_STATUS    = 0x17,
  IDE_ALTSTATUS = 0x26
} IDE_IOPortRegisters;
// Enumeration of the IDE needed flags
typedef enum
{
  //Status register (configuration register)
  IDE_MAS   = 0x0D,
  IDE_RTA   = 0x0C,
  IDE_STAS  = 0x0B,
  //Error register (IOport map register)
  IDE_ABRT  = 0x22,
  //Status regsiter (IOport map register)
  IDE_ERR   = 0x40,
  IDE_DRQ   = 0x43,
  IDE_DRDY  = 0x46,
  IDE_BUSY  = 0x47
} IDE_Flags;
// Enumeration of the IDE mode
typedef enum
{
  IDE_FastTimSelect = 0x01,
  IDE_IORDY         = 0x02,
  IDE_PREFETCH      = 0x04,
  IDE_DMATim        = 0x08
} IDE_Modes;
/* Private define ------------------------------------------------------------*/
//Define of mask and register base.
#define Error_Register_Base      0x011
#define StatusCmd_Register_Base  0x017
#define Status_Register_Base     0x106
#define SAPB_SGCR1Add            0xF0000C00

#define IDE_IOMask               0x01
#define IDE_P7ASMask             0x02
#define IDE_MasterMask           0x04
#define IDE_SwResetMask          0x04
#define IDE_ItMask               0x02

 /*******************************************************************************
* Function Name  : IDE_GetID
* Description    : it reads the register( Identifiaction regsieter) passed as parameter.
* Input          : RegID : one paramter of the "IDE_IDentifier" enumeration
* Return         : an u16 register value
*******************************************************************************/
static inline u16 IDE_GetID (IDE_IDentifier RegID)
{
  return (*(u16*)((IDE_BASE) + RegID));
}
/*******************************************************************************
* Function Name  : IDE_IOConfig
* Description    : Depending on the status passed as parameter this function is
*                  used to enable or disable the IO used. It also enable
*                  the ATPI to use the Port 7 pin
* Input          : NewState: ENABLE or DISABLE
* Return         : None
*******************************************************************************/
void IDE_IOConfig (FunctionalState NewState);
/*******************************************************************************
* Function Name  : IDE_MasterSelect
* Description    : It is used to select primary drive in master mode
* Input          : NewState: ENABLE or DISABLE
* Return         : None
*******************************************************************************/
static inline void IDE_MasterSelect (FunctionalState NewState )
{
  //Enable or Disable the master function Enable
  IDE->COMMAND = (NewState == ENABLE) ? (IDE->COMMAND | IDE_MasterMask) :\
                                        (IDE->COMMAND & ~IDE_MasterMask);
}

/*******************************************************************************
* Function Name  : IDE_PrimaryTiming
* Description    : It is used to configure the needed timing for the primary IDE
* Input          : RCT:Recovery timing it can be 0..3
*                  ISP:IORDY sample point it can be 0..3
* Return         : None
*******************************************************************************/
static inline void IDE_PrimaryTiming (vu8 RCT, vu8 ISP)
{
  // configure the RCT and the ISP in the primary timing register
  IDE->PIDETIM |= 0x8000 |((RCT<<8)|(ISP<<12));
}

/*******************************************************************************
* Function Name  : IDE_SlaveTiming
* Description    : It is used to configure the needed timing for the primary IDE
*                  in slave mode.
* Input          : RCT:Recovery timing it can be 0..3
*                  ISP:IORDY sample point it can be 0..3
* Return         : None
*******************************************************************************/
static inline void IDE_SlaveTiming (vu8 RCT, vu8 ISP)
{
  // configure the RCT and the ISP in the slave timing register
  IDE->SLIDETIM = RCT|(ISP<<2);
}

/*******************************************************************************
* Function Name  : IDE_ModeSelect
* Description    : It is used to configure the needed timing for the primary IDE
*                  in slave mode.
* Input          : device:it can be 0 or 1
*                  mode:it can be one parameter "IDE_Modes" Enumeration
* Return         : None
*******************************************************************************/
static inline void IDE_ModeSelect (vu8 device , IDE_Modes mode)
{
  // configure the RCT and the ISP in the slave timing register
  IDE->PIDETIM = mode<<(device<<3) ;
}

/*******************************************************************************
* Function Name  : IDE_DataSend
* Description    : It writes in the data register (IDE Port Map) the u16 data to be sent
* Input          : data ; the u16 data to be sent.
* Return         : None
*******************************************************************************/
static inline void IDE_DataSend (vu16 data)
{
  // write in the Data register the data to be sent
  IDE->DATA = data ;
}

/*******************************************************************************
* Function Name  : IDE_DataReceive
* Description    : Read and return the data register (IDE Port Map).
* Input          : None
* Return         : u16 the data receiced
*******************************************************************************/
static inline u16 IDE_DataReceive (void)
{
  // return the data register value
  return IDE->DATA;
}

/*******************************************************************************
* Function Name  : IDE_CylinderConfig
* Description    : It configure the starting cylinder address for any disk access.
* Input          : cyladd define the value to be written in Cylinder High & low registers
* Return         : None
*******************************************************************************/
void IDE_CylinderConfig (vu16 cyladd);

/*******************************************************************************
* Function Name  : IDE_SectorConfig
* Description    : It defines the number of the sectors of data to be transferred
*                  and the Identification number of the first of the first sector
*                  to be accessed.
* Input          : sectorcount: the number of sectors of data
*                  firstsector: the first sector number
* Return         : None
*******************************************************************************/
void IDE_SectorConfig (vu8 sectorcount,vu8 firstsector);

/*******************************************************************************
* Function Name  : IDE_GetStatus
* Description    : Return the status register (IDE Port Map)
* Input          : None
* Return         : the status register.
*******************************************************************************/
static inline u8 IDE_GetStatus(void)
{
  // return the status register.
  return IDE->STATUSCMD;
}

/*******************************************************************************
* Function Name  : IDE_ReadIOPortRegister
* Description    : Return the value of any register passed as parameter always
*                  u8 all IDE Port Map register are u8
* Input          : RegIO it can be one parameter of "IDE_IOPortRegisters" Enumeration
* Return         : None
*******************************************************************************/
static inline u8 IDE_ReadIOPortRegisters (IDE_IOPortRegisters RegIO)
{
   // return the register passed as a paramter
   return (*(u8*)((IDE_BASE) + RegIO));
}

/*******************************************************************************
* Function Name  : IDE_FlagStatus
* Description    : Return the status of the flag passed as parameter
* Input          : flag :it can be one parameter of "IDE_flags" Enumeration
* Return         : None
*******************************************************************************/
FlagStatus IDE_FlagStatus (IDE_Flags flag);

/*******************************************************************************
* Function Name  : IDE_FlagClear
* Description    : Clear the flag passed as parameter
* Input          : flag :it can be  IDE_MAS, IDE_RTA,IDE_STAS
* Return         : None
*******************************************************************************/
void IDE_FlagClear (IDE_Flags flags);

/*******************************************************************************
* Function Name  :  IDE_SWReset
* Description    :  Reset the IDE Software Reset bit. When this bit is set
*                   the drive is held reset
* Input          :  NewState: ENABLE or DISABLE
* Return         :  None
*******************************************************************************/
static inline void IDE_SWReset (FunctionalState NewState)
{
  //Enable or Disable the Software reset option
  IDE->DEVCTRL = (NewState == ENABLE) ? (IDE->DEVCTRL | IDE_SwResetMask) :\
                                        (IDE->COMMAND & ~IDE_SwResetMask);
}

/*******************************************************************************
* Function Name  : IDE_ITConfig
* Description    : Enable or disable the interrupt option of the IDE
* Input          : NewState: ENABLE or DISABLE
* Return         : None
*******************************************************************************/
static inline void IDE_ITConfig (FunctionalState NewState)
{
  //Enable or Disable the nIRQ line assertion
  IDE->DEVCTRL = (NewState == DISABLE) ? (IDE->DEVCTRL | IDE_ItMask) :\
                                         (IDE->COMMAND & ~IDE_ItMask);
}

/*******************************************************************************
* Function Name  : IDE_CommandSend
* Description    : Send a command to the IDE.
* Input          : command: refer to the documentation for more detail of the available command
* Return         : None
*******************************************************************************/
static inline void IDE_Command_Send (vu8 command)
{
  //send command
  IDE->STATUSCMD = command;
}
#endif // __720_IDE_H

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
