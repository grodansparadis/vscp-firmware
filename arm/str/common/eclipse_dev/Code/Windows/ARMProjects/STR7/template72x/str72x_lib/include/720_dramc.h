/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : DRAMC.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file contains all the functions prototypes for the
*                      DRAMC software library .
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

#ifndef __720_DRAMC_H
#define __720_DRAMC_H

#include "720_lib.h"

#ifndef _RCCU
  #error _RCCU Must be Defined in "720_conf.h"
#endif

/* Includes -------------------------------------------------------------------*/

/* Exported types -------------------------------------------------------------*/
#define DRAMC_Bank1	 1
#define DRAMC_Bank2	 2
#define DRAMC_Bank3	 3
#define DRAMC_Bank4	 4
/* Exported constants ---------------------------------------------------------*/

//DRAMC_BankConfig defined constant
#define  DRAMC_8bitsColWidth   0x000
#define  DRAMC_9bitsColWidth   0x001
#define  DRAMC_10bitsColWidth  0x002
#define  DRAMC_ByteDataWidth   0x000
#define  DRAMC_H_WordDataWidth 0x400
#define  DRAMC_WordDataWidth   0x800
#define  DRAMC_WAITSTATE(n)   ( (n) << 5 ) // n valid between 0 and 7
#define  DRAMC_DataLat(n)     ( (n) << 8 ) // n valid between 0 and 3
#define  DRAMC_IdleTime(n)    ( (n) << 2 ) // n valid between 0 and 7

//DRAMC_ModeRegisterSet defined constant
#define  DRAMC_BLSeq1      0x0
#define  DRAMC_BLSeq2      0x1
#define  DRAMC_BLSeq4      0x2
#define  DRAMC_BLSeq8      0x3
#define  DRAMC_BLInter1    0x8
#define  DRAMC_BLInter2    0x9
#define  DRAMC_BLInter4    0xA
#define  DRAMC_BLInter8    0xB
#define  DRAMC_BLFull      0x7
#define  DRAMC_CASLat2     0x20
#define  DRAMC_CASLat3     0x30
#define  DRAMC_PMode       0x0
#define  DRAMC_Single      0x200

//DRAMC_MemoryConfig defined constant
#define  DRAMC_PowerSave_Enter   0x2000
#define  DRAMC_PowerSave_Exit    0x0
#define  DRAMC_Type_SDRAM        0x1000
#define  DRAMC_Type_EDO          0x0
#define  DRAMC_B1_Enable         0x100
#define  DRAMC_B2_Enable         0x200
#define  DRAMC_B3_Enable         0x400
#define  DRAMC_B4_Enable         0x800

//DRAMC_CommandSend defined constant
#define  DRAMC_MRS_Command       0x7
#define  DRAMC_CBR_Command       0x3
#define  DRAMC_PRECH_Command     0x5
#define  DRAMC_PALL_Command      0x5
#define  DRAMC_ACT_Command       0x1
#define  DRAMC_WRIT_Command      0x6
#define  DRAMC_WRITA_Command     0x6
#define  DRAMC_READ_Command      0x2
#define  DRAMC_READA_Command     0x2
#define  DRAMC_BST_Command       0x4
#define  DRAMC_NOP_Command       0x0

/*****************************************************************************/
typedef enum {
               MRS               ,       //Mode register set
               CBR               ,       //Auto refresh.
               PRECH             ,       //Precharge selected bank.
               PALL              ,       //Precharge all bank.
               ACT               ,       //Bank active.
               WRIT              ,       //Write.
               WRITA             ,       //Write with auto precharge.
               READ              ,       // Read.
               READA             ,       // Read with auto precharge.
               BST               ,       // Burst stop.
               NOP               ,       // No operation.
             }  DRAMC_Command;

/******************************************************************************
* Function Name  : DRAMC_BankConfig
* Description    : This routine is used to configure a specific parameters for
                   the specified external bank (data width,data latency,wait
                   state, idle time and column width,Bank Size).
* Input          : Bankx  (u32)
*                : Config (u16)
*                : Size   (u32)
* Return         : None
*******************************************************************************/
void DRAMC_BankConfig ( vu32 Bankx, vu16 Config , vu32 Size);
/******************************************************************************
* Function Name  : DRAMC_ModeRegisterSet
* Description    : This routine is used to configure the internal
                   mode register of the  specified memory bank.
* Input          : Bankx (u32)
*                : Config (u16)
* Return         : None
**********************************************************************************/
void DRAMC_ModeRegisterSet ( vu32 Bankx, vu16 Config );
/**********************************************************************************
* Function Name  : DRAMC_MemoryConfig
* Description    : This routine is used to configure parameters that are common
                   for all banks wich are Power save mode, memory type , Bank
                   enable and refresh period .
* Input          : Config (u16)
*                : T_Refresh (u32)
* Return         : None
************************************************************************************/
void DRAMC_MemoryConfig (vu16 Config , vu32 T_Refresh);
/***********************************************************************************
* Function Name  : DRAMC_CommandSend
* Description    : this routine is used to send specified command to the external memory.
* Input          : Cmd(DRAMC_Command)
* Return         : None
*************************************************************************************/
void DRAMC_CommandSend (  vu32 Bankx , DRAMC_Command Cmd );

#endif  //__720_DRAMC_H

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
