/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_flash.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the Flash software functions.
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

/* Standard include ----------------------------------------------------------*/
#include "73x_flash.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void FLASH_WaitForLastTask(void);

/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : FLASH_DeInit
* Description    : Deinitializes Flash module registers to their default reset
*                  values.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_DeInit(void)
{
  /* reset Flash control registers */
  FLASHR->CR0 = 0x00000000;
  FLASHR->CR1 = 0x00000000;

  /* reset Flash data registers */
  FLASHR->DR0 = 0xFFFFFFFF;
  FLASHR->DR1 = 0xFFFFFFFF;

  /* reset Flash address register */
  FLASHR->AR = 0x00000000;

  /* reset Flash error register */
  FLASHR->ER  = 0x00000000;
}

/*******************************************************************************
* Function Name  : FLASH_ITConfig
* Description    : Enables or disables the end of write interrupt(INTM).
* Input          : NewState: new state of end of write interrupt. 
*                  This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_ITConfig(FunctionalState NewState)
{
  if(NewState == ENABLE)
  { /* enable INTM interrupt */
    FLASHR->CR0  |= FLASH_IT_INTM;
  }
  else
  { /* disable INTM interrupt */
    FLASHR->CR0  &= ~FLASH_IT_INTM;
  }
}

/*******************************************************************************
* Function Name  : FLASH_WordWrite
* Description    : Writes a word at the specified address in the Flash.
* Input          : - DestAddr: address of the Flash to be programmed.
*                  - Data: word to be written.
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_WordWrite(u32 DestAddr, u32 Data)
{
  /* wait for the end of last task */
  FLASH_WaitForLastTask();

  /* set the WPG bit in CR0 register */
  FLASHR->CR0 |= FLASH_WPG;

  /* load the destination address in AR register */
  FLASHR->AR = DestAddr;

  /* load the data to be programmed in DR0 register */
  FLASHR->DR0 = Data;

  /* set the start bit WMS in CR0 register to start Write operation */
  FLASHR->CR0 |= FLASH_WMS;
}

/*******************************************************************************
* Function Name  : FLASH_DWordWrite
* Description    : Writes a double word at the specified address in the Flash.
* Input          : - DestAddr: address of the Flash to be programmed, should be
*                    aligned on a double word boundary.
*                  - Data0: first word to be written.
*                  - Data1: second word to be written.
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_DWordWrite(u32 DestAddr, u32 Data0, u32 Data1)
{
  /* wait for the end of last task */
  FLASH_WaitForLastTask();

  /* set the DWPG bit in CR0 register */
  FLASHR->CR0 |= FLASH_DWPG;

  /* load the destination address in AR register */
  FLASHR->AR = DestAddr;

  /* load the first word to be programmed in DR0 register */
  FLASHR->DR0 = Data0;

  /* load the second word to be programmed in DR1 register */
  FLASHR->DR1 = Data1;

  /* set the start bit WMS in CR0 register to start Write operation */
  FLASHR->CR0 |= FLASH_WMS;
}

/*******************************************************************************
* Function Name  : FLASH_BlockWrite
* Description    : Writes a block of data from the RAM to the Flash.
* Input          : - SourceAddr: specifies the RAM address of the block of data
*                    to be programmed in the Flash.
*                  - DestAddr: specifies the address in the Flash where the
*                    block of data will be programmed.
*                  - NbrWordToWrite: specifies the number of data to be 
*                    programmed expressed in terms of words.
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_BlockWrite(u32 SourceAddr, u32 DestAddr, u32 NbrWordToWrite)
{
  u32 NbrWord = 0, NbrDWord = 0;
  u32 TmpData0 = 0, TmpData1 = 0;

  /* get the number of word that can be programmed using Double Word Program operation */
  NbrDWord = NbrWordToWrite >> 1;

  /* get the number of word that can be programmed using Word Program operation */
  NbrWord = NbrWordToWrite & 0x00000001;

  if((DestAddr % 8) == 0) /* Flash destination address is Double Word aligned */
  {
    if(NbrWord == 0) /* 'NbrWordToWrite' is an even number */
    { /* all data will be programmed using Double Word Program operation */
      while(NbrDWord--)
      {
        TmpData0 = *(u32 *)SourceAddr; /* get the first word to be programmed */
        SourceAddr += 4; /* increment the RAM source address by 4 */
        TmpData1 = *(u32 *)SourceAddr; /* get the second word to be programmed */
        SourceAddr += 4; /* increment the RAM source address by 4 */
        FLASH_DWordWrite(DestAddr, TmpData0, TmpData1);
        DestAddr += 8; /* increment the Flash destination address by 8 */
      }
    }
    else /* 'NbrWordToWrite' is an odd number  */
    { /* 'NbrWordToWrite-1' words will be programmed using Double Word Program operation */      
      while(NbrDWord--)
      {
        TmpData0 = *(u32 *)SourceAddr; /* get the first word to be programmed */
        SourceAddr += 4; /* increment the RAM source address by 4 */
        TmpData1 = *(u32 *)SourceAddr; /* get the second word to be programmed */
        SourceAddr += 4; /* increment the RAM source address by 4 */
        FLASH_DWordWrite(DestAddr, TmpData0, TmpData1);
        DestAddr += 8; /* increment the Flash destination address by 8 */
      }

      /* the last word is programmed using Word Program operation */
      TmpData0 = *(u32 *)SourceAddr;
      FLASH_WordWrite(DestAddr, TmpData0);
    }
  }
  else /* Flash destination address is Word aligned */
  {
    if(NbrWord == 0) /* 'NbrWordToWrite' is an even number */
    { /* the first word is programmed using Word Program operation */
      TmpData0 = *(u32 *)SourceAddr; /* get the first word to be programmed */
      SourceAddr += 4; /* increment the RAM source address by 4 */
      FLASH_WordWrite(DestAddr, TmpData0);
      DestAddr += 4; /* increment the Flash destination address by 4 */
      NbrDWord -= 1; /* decrease 'NbrDWord' by 1 */

      /* 'NbrWordToWrite-2' words will be programmed using Double Word Program operation */
      while(NbrDWord--)
      {
        TmpData0 = *(u32 *)SourceAddr; /* get the first word to be programmed */
        SourceAddr += 4; /* increment the RAM source address by 4 */
        TmpData1 = *(u32 *)SourceAddr; /* get the second word to be programmed */
        SourceAddr += 4; /* increment the RAM source address by 4 */
        FLASH_DWordWrite(DestAddr, TmpData0, TmpData1);
        DestAddr += 8; /* increment the Flash destination address by 8 */
      }

      /* the last word is programmed using Word Program operation */
      TmpData0 = *(u32 *)SourceAddr; /* get the first word to be programmed */
      FLASH_WordWrite(DestAddr, TmpData0);
    }
    else /* 'NbrWordToWrite' is an odd number  */
    {
      /* the first word is programmed using Word Program operation */
      TmpData0 = *(u32 *)SourceAddr; /* get the first word to be programmed */
      SourceAddr += 4; /* increment the RAM source address by 4 */
      FLASH_WordWrite(DestAddr, TmpData0);
      DestAddr += 4; /* increment the Flash destination address by 4 */

      /* the remaining words will be programmed using Double Word Program operation */
      while(NbrDWord--)
      {
        TmpData0 = *(u32 *)SourceAddr; /* get the first word to be programmed */
        SourceAddr += 4; /* increment the RAM source address by 4 */
        TmpData1 = *(u32 *)SourceAddr; /* get the second word to be programmed */
        SourceAddr += 4; /* increment the RAM source address by 4 */
        FLASH_DWordWrite(DestAddr, TmpData0, TmpData1);
        DestAddr += 8; /* increment the Flash destination address by 8 */
      }
    }
  }
}

/*******************************************************************************
* Function Name  : FLASH_WordRead
* Description    : Reads a word from the specified address in the Flash.
* Input          : SourceAddr: address of the Flash to be read.
* Output         : None
* Return         : The data value at the specified address.
*******************************************************************************/
u32 FLASH_WordRead(u32 SourceAddr)
{
  /* wait for the end of last task */
  FLASH_WaitForLastTask();

  /* add the Flash offset(0x80000000) to SourceAddr and return the word value
   pointed by this absolute address */
  return(*(u32 *)(FLASH_Base + SourceAddr ));
}

/*******************************************************************************
* Function Name  : FLASH_BlockRead
* Description    : Reads a block of data from a specified address in the Flash
*                  and store it in the RAM.
* Input          : - SourceAddr: specifies the address, in the Flash, of the
*                    block of data to be read.
*                  - DestAddr: specifies the destination address in the RAM
*                    where that data will be copied.
*                  - NbrWordToRead: specifies the number of words to be read
*                    from the Flash.
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_BlockRead(u32 SourceAddr, u32 DestAddr, u32 NbrWordToRead)
{
  /* wait for the end of last task */
  FLASH_WaitForLastTask();

  while(NbrWordToRead--) /* while the whole data are not yet read */
  {
    *(u32 *)DestAddr = *(u32 *)(FLASH_Base + SourceAddr);

    /* increase the source address */
    SourceAddr += 4;

    /* increase the destination address */
    DestAddr += 4;
  }
}

/*******************************************************************************
* Function Name  : FLASH_SectorErase
* Description    : Erases the specified Flash sectors.
* Input          : FLASH_Sector: sectors to be erased. This parameter can be
*                  any combination of the following values:
*                         - FLASH_SectorX: Bank0 Flash SectorX will be erased
*                           (X can be [0..7])
*                         - FLASH_Module: All Bank0 Sectors will be erased
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_SectorErase(u8 FLASH_Sector)
{
  /* wait for the end of last task */
  FLASH_WaitForLastTask();

  /* set the SER bit in CR0 register */
  FLASHR->CR0 |= FLASH_SER;

  /* select the sectors to be erased in the CR1 register */
  FLASHR->CR1 |= FLASH_Sector;

  /* set the start bit WMS in CR0 register */
  FLASHR->CR0 |= FLASH_WMS;
}

/*******************************************************************************
* Function Name  : FLASH_Suspend
* Description    : Suspends an on-going operation on the Flash.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_Suspend(void)
{
  /* set the SUSP bit in CR0 register */
  FLASHR->CR0 |= FLASH_SUSP;

  /* wait for the end of last task */
  FLASH_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : FLASH_Resume
* Description    : Resumes a suspended operation on the Flash.
* Input          : OperToResume: specifies the operation that has been
*                  suspended and needs to be resumed. This parameter can be
*                  one of the following values:
*                         - FLASH_SER: Sector Erase operation will be resumed
*                         - FLASH_WPG: Word Program operation will be resumed
*                         - FLASH_DWPG: Double Word Program operation will be resumed
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_Resume(u32 OperToResume)
{
  /* clear the SUSP bit in CR0 register */
  FLASHR->CR0 &= ~FLASH_SUSP;

  /* configure CR0 register with the operation that needs to be resumed */
  FLASHR->CR0 |= OperToResume;

  /* set the start bit WMS in CR0 register */
  FLASHR->CR0 |= FLASH_WMS;
}

/*******************************************************************************
* Function Name  : FLASH_FlagStatus
* Description    : Checks whether the specified Flash flag is set or not.
* Input          : FLASH_Flag: flag to check. This parameter can be one of the
*                  following values:
*                         - FLASH_FLAG_BSY0: Bank0 busy flag
*                         - FLASH_FLAG_LOCK: Flash register access locked flag
*                         - FLASH_FLAG_JBL: JTAG boot mode latched flag
*                         - FLASH_FLAG_INTP: End of write interrupt pending flag
*                         - FLASH_FLAG_BSM: Boot from system memory flag
*                         - FLASH_FLAG_ERR: Write error flag
*                         - FLASH_FLAG_ERER: Erase error flag
*                         - FLASH_FLAG_PGER: Program error flag
*                         - FLASH_FLAG_10ER: 1 over 0 error flag
*                         - FLASH_FLAG_SEQER: Sequence error flag
*                         - FLASH_FLAG_RESER: Resume error flag
*                         - FLASH_FLAG_WPF: Write protection flag
* Output         : None
* Return         : The new state of FLASH_Flag (SET or RESET).
*******************************************************************************/
FlagStatus FLASH_FlagStatus(u8 FLASH_Flag)
{
  u8 FlashReg = 0, FlagPos = 0;

  /* get the Flash register index */
  FlashReg = FLASH_Flag >> 5;

  /* get the flag position */
  FlagPos = FLASH_Flag & FLASH_Flag_Mask;

  if(FlashReg == 1) /* the flag to check is in CR0 register */
  {  
    if((FLASHR->CR0 & (1<<FlagPos))!= RESET)
    {
      return SET;
    }
    else
    {
      return RESET;
    }
  } 
  else /* (FlashReg == 2 )  The flag to check is in ER register */
  {   
    if((FLASHR->ER & (1<<FlagPos)) != RESET)
    {
      return SET;
    }
    else
    {
      return RESET;
    }
  }
}

/*******************************************************************************
* Function Name  : FLASH_FlagClear
* Description    : Clears the specified Flash flag.
* Input          : FLASH_Flag: flag to clear. This parameter can be
*                  one of the following values:
*                         - FLASH_FLAG_JBL: JTAG boot mode latched flag
*                         - FLASH_FLAG_INTP: End of write interrupt pending flag
*                         - FLASH_FLAG_BSM: Boot from system memory flag
*                         - FLASH_FLAG_ERR: Write error flag
*                         - FLASH_FLAG_ERER: Erase error flag
*                         - FLASH_FLAG_PGER: Program error flag
*                         - FLASH_FLAG_10ER: 1 over 0 error flag
*                         - FLASH_FLAG_SEQER: Sequence error flag
*                         - FLASH_FLAG_RESER: Resume error flag
*                         - FLASH_FLAG_WPF: Write protection flag
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_FlagClear(u8 FLASH_Flag)
{
  u8 FlashReg = 0, FlagPos = 0;

  /* get the Flash register index */
  FlashReg = FLASH_Flag >> 5;

  /* get the flag position */
  FlagPos = FLASH_Flag & FLASH_Flag_Mask;

  switch(FlashReg)
  {
    case 1 : /* the flag to clear is in CR0 register */
      FLASHR->CR0 &= ~(1<<FlagPos);
    break;

    case 2 : /* the flag to clear is in ER register */
      FLASHR->ER &= ~(1<<FlagPos);
    break;
  }
}

/*******************************************************************************
* Function Name  : FLASH_WaitForLastTask
* Description    : Waits for the end of last task.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void FLASH_WaitForLastTask(void)
{
  /* wait until the Flash controller acknowledges the end of the last
    operation resetting the BSY0 and LOCK bits in the CR0 register */
  while((FLASHR->CR0 & FLASH_FLAG_LOCKBSY0) != RESET);
}

/*******************************************************************************
* Function Name  : FLASH_PowerDownConfig
* Description    : Enables or disables the Flash power down mode.
* Input          : NewState : new state of the Flash power down mode.
*                  This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_PowerDownConfig(FunctionalState NewState)
{
  if(NewState == ENABLE)
  { /* enable Flash power down mode */
    FLASHR->CR0  |= FLASH_PWD;
  }
  else
  { /* disable Flash power down mode */
    FLASHR->CR0  &= ~FLASH_PWD;
  }
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
