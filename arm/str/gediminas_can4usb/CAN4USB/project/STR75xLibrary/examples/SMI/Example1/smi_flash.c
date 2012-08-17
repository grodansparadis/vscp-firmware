/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : smi_flash.c
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : This file provides a set of function used to communicate
*                      with an SPI compatible FLASH(M25P64), connected to SMI
*                      peripheral, mounted on STR75x-EVAL.
********************************************************************************
* History:
* 07/17/2006 : V1.0
* 03/10/2006 : V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "smi_flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_CMD_WEN     0x06
#define FLASH_CMD_SE      0xD8
#define FLASH_CMD_BE      0xC7
#define FLASH_CMD_RDID    0x9F

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/  
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : SMI_FLASH_Init
* Description    : Configures SMI and GPIOs. This function must be called before
*                  any write/read operation on the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_Init(void)
{
  SMI_InitTypeDef  SMI_InitStructure;

  SMI_InitStructure.SMI_ClockHold = 0;
  SMI_InitStructure.SMI_Prescaler = 2;  
  SMI_InitStructure.SMI_DeselectTime = 1;
  SMI_Init(&SMI_InitStructure);

  /* Enable SMI Alternate Functions */
  GPIO_PinRemapConfig(GPIO_Remap_SMI_EN, ENABLE);

  /* Enable Bank 0 */
  SMI_BankCmd(SMI_Bank_0, ENABLE);

  /* Select Bank 0 */
  SMI_SelectBank(SMI_Bank_0);

  /* Enable SMI Flash Fast Read mode */
  SMI_FastReadConfig(SMI_FastRead_Enable);
}
  
/*******************************************************************************
* Function Name  : SMI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : - SectorAddr: FLASH's sector address to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_SectorErase(u32 SectorAddr)
{
  u32 Temp = 0, Cmd = 0;
 
  /* Clear ERF1 and ERF2 flags */
  SMI_ClearFlag(SMI_FLAG_ERF2 | SMI_FLAG_ERF1);

/* SMI in SW mode ------------------------------------------------------------*/
  SMI_ModeConfig(SMI_Mode_SW);

  /* Transmit one byte to external memory */
  SMI_TxRxLengthConfig(SMI_TxLength_1Byte, SMI_RxLength_0Bytes);

  /* Send Write Enable command */ 
  SMI_SendCmd(FLASH_CMD_WEN); 
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);

  /* Send Read Status Register command */
  SMI_SendRSRCmd();
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);

  /* Transmit four bytes to external memory */
  SMI_TxRxLengthConfig(SMI_TxLength_4Bytes, SMI_RxLength_0Bytes);
  
  /* Send Sector Erase command followed by three address bytes */
  Temp = SectorAddr >> 16;
  Cmd = (Temp << 8) | FLASH_CMD_SE;
  SMI_SendCmd(Cmd);
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);
  
  /* Wait while the FLASH is busy with Erase cycle */  
  do
  {
    SMI_SendRSRCmd();
    while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
    SMI_ClearFlag(SMI_FLAG_TF);
  } while((SMI_ReadMemoryStatusRegister() & 0x01) != RESET);

/* SMI in HW mode ------------------------------------------------------------*/
  SMI_ModeConfig(SMI_Mode_HW);
}

/*******************************************************************************
* Function Name  : SMI_FLASH_BulkErase
* Description    : Performs a bulk erase (erase the entire FLASH).
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_BulkErase(void)
{
  /* Clear ERF1 and ERF2 flags */
  SMI_ClearFlag(SMI_FLAG_ERF2 | SMI_FLAG_ERF1);

  /* Send Write Enable command */
  SMI_SendWENCmd();
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);

  /* Send Read Status Register command */
  SMI_SendRSRCmd();
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);
  
/* SMI in SW mode ------------------------------------------------------------*/
  SMI_ModeConfig(SMI_Mode_SW);
  
  /* Transmit one byte to external memory */
  SMI_TxRxLengthConfig(SMI_TxLength_1Byte, SMI_RxLength_0Bytes);

  /* Send Bulk Erase command */ 
  SMI_SendCmd(FLASH_CMD_BE); 
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);
  
  /* Wait while the FLASH is busy with Erase cycle */   
  do
  {
    SMI_SendRSRCmd();
    while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
    SMI_ClearFlag(SMI_FLAG_TF);
  }while((SMI_ReadMemoryStatusRegister() & 0x01) != RESET);

/* SMI in HW mode ------------------------------------------------------------*/
  SMI_ModeConfig(SMI_Mode_HW);
}

/*******************************************************************************
* Function Name  : SMI_FLASH_WritebBuffer
* Description    : Writes a byte buffer to the selected memory Bank. The size of
*                  the buffer must not exceed the FLASH page size.
* Input          : - bBuffer : pointer to buffer. 
*                  - WriteAddr : external memory address from which the data will
*                    be written.
*                  - NumByteToWrite : number of bytes to write. 
*                    
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_WritebBuffer(u8* bBuffer, u32 WriteAddr, u16 NumByteToWrite)
{   	 
  /* Clear ERF1 and ERF2 flags */
  SMI_ClearFlag(SMI_FLAG_ERF2 | SMI_FLAG_ERF1);

  /* Send Write Enable command*/
  SMI_SendWENCmd();
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);

  /* Enable Burst Mode */
  SMI_WriteBurstConfig(SMI_WriteBurst_Enable);

  for(; NumByteToWrite != 0; NumByteToWrite--) /* while there is data to write */
  {
    /* Transfer data to the memory */
    *(u8 *) WriteAddr = *bBuffer++; 
    
    /* Increment the address*/  
    WriteAddr += 1;
  }	  

  /* Disable Burst Mode */
  SMI_WriteBurstConfig(SMI_WriteBurst_Disable);

  /* Wait while the FLASH is busy with Write cycle */
  while((SMI_GetFlagStatus(SMI_FLAG_WC)) == RESET);
  SMI_ClearFlag(SMI_FLAG_WC);
}

/*******************************************************************************
* Function Name  : SMI_FLASH_WritehBuffer
* Description    : Writes a half-word buffer to the selected memory Bank. 
*                  The size of the buffer must not exceed the FLASH page size. 
* Input          : - bBuffer : pointer to buffer. 
*                  - WriteAddr : external memory address from which the data will
*                    be written.
*                  - NumHalfWordToWrite : number of half-word to write.
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_WritehBuffer(u16* hBuffer, u32 WriteAddr, u16 NumHalfWordToWrite)
{   	 
  /* Clear ERF1 and ERF2 flags */
  SMI_ClearFlag(SMI_FLAG_ERF2 | SMI_FLAG_ERF1);

  /* Send Write Enable command*/
  SMI_SendWENCmd();
  SMI_ClearFlag(SMI_FLAG_TF);

  /* Enable Burst Mode */
  SMI_WriteBurstConfig(SMI_WriteBurst_Enable);

  for(; NumHalfWordToWrite != 0; NumHalfWordToWrite--) /* while there is data to write */
  {
    /* Transfer data to the memory */
    *(u16 *) WriteAddr = *hBuffer++; 
    
    /* Increment the address*/  
    WriteAddr += 2;
  }	  

  /* Disable Burst Mode */
  SMI_WriteBurstConfig(SMI_WriteBurst_Disable);

  /* Wait while the FLASH is busy with Write cycle */
  while((SMI_GetFlagStatus(SMI_FLAG_WC)) == RESET);
  SMI_ClearFlag(SMI_FLAG_WC);  
}

/*******************************************************************************
* Function Name  : SMI_FLASH_WritewBuffer
* Description    : Writes a word buffer to the selected memory Bank. The size of
*                  the buffer must not exceed the FLASH page size.
* Input          : - bBuffer : pointer to buffer. 
*                  - WriteAddr : external memory address from which the data will
*                    be written.
*                  - NumByteToWrite : number of word to write.
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_WritewBuffer(u32* wBuffer, u32 WriteAddr, u16 NumWordToWrite)
{   	
  /* Clear ERF1 and ERF2 flags */
  SMI_ClearFlag(SMI_FLAG_ERF2 | SMI_FLAG_ERF1);

  /* Send Write Enable command*/
  SMI_SendWENCmd();
  SMI_ClearFlag(SMI_FLAG_TF);
  
  /* Enable Burst Mode */
  SMI_WriteBurstConfig(SMI_WriteBurst_Enable);

  for(; NumWordToWrite != 0; NumWordToWrite--) /* while there is data to write */
  {
    /* Transfer data to the memory */
    *(u32 *) WriteAddr = *wBuffer++; 
    
    /* Increment the address*/  
    WriteAddr += 4;
  }	  
  
  /* Disable Burst Mode */
  SMI_WriteBurstConfig(SMI_WriteBurst_Disable);

  /* Wait while the FLASH is busy with Write cycle */
  while((SMI_GetFlagStatus(SMI_FLAG_WC)) == RESET);
  SMI_ClearFlag(SMI_FLAG_WC);    
}

/*******************************************************************************
* Function Name  : SMI_FLASH_ReadbBuffer
* Description    : Reads a block of data from the external memory.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the external memory.
*                  - ReadAddr : external memory address to read from.
*                  - NumByteToRead : number of bytes to read.
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_ReadbBuffer(u8* bBuffer, u32 ReadAddr, u16 NumByteToRead)
{   	
  for(; NumByteToRead != 0; NumByteToRead--) /* while there is data to read */
  {
    /* Read a byte from the FLASH */			
    *bBuffer++ = *(u8*) ReadAddr++;
  }	  
}

/*******************************************************************************
* Function Name  : SMI_FLASH_ReadhBuffer
* Description    : Reads a block of data from the external memory.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the external memory.
*                  - ReadAddr : external memory address to read from.
*                  - NumHalfWordToRead : number of half words to read.
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_ReadhBuffer(u16* hBuffer, u32 ReadAddr, u16 NumHalfWordToRead)
{   	
  for(; NumHalfWordToRead != 0; NumHalfWordToRead--) /* while there is data to read */
  {
    /* Read a half word from the FLASH */			
    *hBuffer++ = *(u16*) ReadAddr;

    /* Increment the address*/ 
    ReadAddr += 2;
  }	  
}

/*******************************************************************************
* Function Name  : SMI_FLASH_ReadwBuffer
* Description    : Reads a block of data from the external memory.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the external memory.
*                  - ReadAddr : external memory address to read from.
*                  - NumWordToRead : number of words to read.
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_ReadwBuffer(u32* wBuffer, u32 ReadAddr, u16 NumWordToRead)
{   	
  for(; NumWordToRead != 0; NumWordToRead--) /* while there is data to read */
  {
    /* Read a word from the FLASH */			
    *wBuffer++ = *(u32*) ReadAddr;

    /* Increment the address*/ 
    ReadAddr += 4;
  }	  
}

/*******************************************************************************
* Function Name  : SMI_FLASH_ReadID
* Description    : Reads FLASH identification ID.
* Input          : None
* Output         : None
* Return         : FLASH identification ID
*******************************************************************************/
u32 SMI_FLASH_ReadID(void)
{   	 
  u32 Temp = 0, Temp0 = 0, Temp1 =0, Temp2 = 0;

  /* SMI in SW mode */
  SMI_ModeConfig(SMI_Mode_SW);

  /* Transmit one byte and receive three bytes to/from external memory */
  SMI_TxRxLengthConfig(SMI_TxLength_1Byte, SMI_RxLength_3Bytes);

  /* Clear ERF1 and ERF2 flags */
  SMI_ClearFlag(SMI_FLAG_ERF2 | SMI_FLAG_ERF1);
  
  /* Send Read Identification  command */ 
  SMI_SendCmd(FLASH_CMD_RDID);
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);
    
  /* Compute the FLASH ID*/
  Temp = SMI->RR; 
  Temp0 = Temp & 0xFF;  /* Manufacturer Identification */
  Temp1 = (Temp >> 8) & 0xFF;  /* Memory Type */
  Temp2 = (Temp >> 16) & 0xFF; /* Memory Capacity */
  
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  /* SMI in HW mode */	  
  SMI_ModeConfig(SMI_Mode_HW);

  return Temp;
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
