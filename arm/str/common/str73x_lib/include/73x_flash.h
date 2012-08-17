/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_flash.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      Flash software library.
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
* CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __73x_FLASH_H
#define __73x_FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "73x_map.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Bank0 Flash sectors */
#define FLASH_Sector0    0x00000001
#define FLASH_Sector1    0x00000002
#define FLASH_Sector2    0x00000004
#define FLASH_Sector3    0x00000008
#define FLASH_Sector4    0x00000010
#define FLASH_Sector5    0x00000020
#define FLASH_Sector6    0x00000040
#define FLASH_Sector7    0x00000080
#define FLASH_Module     0x000000FF

/* Flash operation enable bit definition */
#define FLASH_PWD     0x00008000
#define FLASH_SPR     0x01000000
#define FLASH_SER     0x08000000
#define FLASH_DWPG    0x10000000
#define FLASH_WPG     0x20000000
#define FLASH_SUSP    0x40000000
#define FLASH_WMS     0x80000000

/* Flash Interrupt */
#define FLASH_IT_INTM   0x00200000

/* Flash Flags */
#define FLASH_FLAG_BSY0     0x21   
#define FLASH_FLAG_LOCK     0x24  
#define FLASH_FLAG_JBL      0x33   
#define FLASH_FLAG_INTP     0x34   
#define FLASH_FLAG_BSM      0x36   
#define FLASH_FLAG_ERR      0x40  
#define FLASH_FLAG_ERER     0x41   
#define FLASH_FLAG_PGER     0x42   
#define FLASH_FLAG_10ER     0x43  
#define FLASH_FLAG_SEQER    0x46   
#define FLASH_FLAG_RESER    0x47   
#define FLASH_FLAG_WPF      0x48   

#define FLASH_FLAG_LOCKBSY0    0x00000012

/* Flash base address */
#define FLASH_Base    0x80000000

#define FLASH_Flag_Mask  0x1F

/* Module private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void FLASH_DeInit(void);
void FLASH_ITConfig (FunctionalState NewState);
void FLASH_WordWrite(u32 DestAddr, u32 Data);
void FLASH_DWordWrite(u32 DestAddr, u32 Data0, u32 Data1);
void FLASH_BlockWrite(u32 SourceAddr, u32 DestAddr, u32 NbrWordToWrite);
u32 FLASH_WordRead(u32 SourceAddr);
void FLASH_BlockRead(u32 SourceAddr, u32 DestAddr, u32 NbrWordToRead);
void FLASH_SectorErase(u8 FLASH_Sector);
void FLASH_PowerDownConfig(FunctionalState NewState);
void FLASH_Suspend(void);
void FLASH_Resume(u32 OperToResume);
FlagStatus FLASH_FlagStatus (u8 FLASH_Flag);
void FLASH_FlagClear(u8 FLASH_Flag);


#endif /* __73x_FLASH_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
