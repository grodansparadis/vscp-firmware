/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : spi_ee.c
* Author             : MCD Application Team
* Date First Issued  : 07/17/2006
* Description        : This file provides a set of functions needed to manage the
*                      communication between SSP peripheral and SPI M95640 EEPROM.
********************************************************************************
* History:
* 07/17/2006 : V1.0
*******************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "spi_ee.h"

/* Private typedef -----------------------------------------------------------*/
#define SPI_EE_PageSize 32

#define WRITE      0x02  /* Write to Memory instruction */
#define WRSR       0x01  /* Write Status Register instruction */ 
#define WREN       0x06  /* Write enable instruction */

#define READ       0x03  /* Read from Memory instruction */
#define RDSR       0x05  /* Read Status Register instruction  */

#define Low        0x00  /* ChipSelect line low */
#define High       0x01  /* ChipSelect line high */

#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte 0xA5

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SPI_EE_ChipSelect(u8 State);
static u8 SPI_EE_SendByte(u8 byte);
static void SPI_EE_WriteEnable(void);
static void SPI_EE_WaitForWriteEnd(void);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : SPI_EE_Init
* Description    : Initializes the peripherals used by the SPI EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_Init(void)
{
  SSP_InitTypeDef  SSP_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
   
  /* Enable SSP0 and GPIOs clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_SSP0, ENABLE); 
  
  /* SSP0 pins configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ; 
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* SSP0 configuration */ 
  SSP_InitStructure.SSP_FrameFormat = SSP_FrameFormat_Motorola;
  SSP_InitStructure.SSP_Mode = SSP_Mode_Master;
  SSP_InitStructure.SSP_CPOL = SSP_CPOL_High;
  SSP_InitStructure.SSP_CPHA = SSP_CPHA_2Edge;
  SSP_InitStructure.SSP_DataSize = SSP_DataSize_8b;
  SSP_InitStructure.SSP_NSS = SSP_NSS_Soft;
  SSP_InitStructure.SSP_ClockRate = 2;
  SSP_InitStructure.SSP_ClockPrescaler = 2; /* SSP baud rate : PCLK/2*(2+1) */
  SSP_Init(SSP0, &SSP_InitStructure);
  /* SSP0 enable */
  SSP_Cmd(SSP0, ENABLE);  
}

/*******************************************************************************
* Function Name  : SPI_EE_PageWrite
* Description    : Writes more than one byte to the EEPROM with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the EEPROM page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM,
*                    must be equal or less than "SPI_EE_PageSize" value. 
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite)
{
  /* Enable the write access to the EEPROM */
  SPI_EE_WriteEnable();		
  
  /* Select the EEPROM: Chip Select low */
  SPI_EE_ChipSelect(Low);	   
  /* Send "Write to Memory " instruction */    
  SPI_EE_SendByte(WRITE);	
  /* Send WriteAddr high nibble address byte to write to */
  SPI_EE_SendByte((WriteAddr & 0xFF00) >> 8);  
  /* Send WriteAddr low nibble address byte to write to */
  SPI_EE_SendByte(WriteAddr & 0xFF);             
  
  /* while there is data to be written on the EEPROM */
  while(NumByteToWrite--) 
  {
    /* Send the current byte */			
    SPI_EE_SendByte(*pBuffer); 	           
    
    /* Point on the next byte to be written */
    pBuffer++; 
  }		
  
  /* Deselect the EEPROM: Chip Select high */
  SPI_EE_ChipSelect(High);
  
  /* Wait the end of EEPROM writing */
  SPI_EE_WaitForWriteEnd();	
}

/*******************************************************************************
* Function Name  : SPI_EE_BufferWrite
* Description    : Writes block of data to the EEPROM. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_EE_PageSize;
  count = SPI_EE_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_EE_PageSize;
  NumOfSingle = NumByteToWrite % SPI_EE_PageSize;
  
  if(Addr == 0) /* WriteAddr is SPI_EE_PageSize aligned  */
  {
    if(NumOfPage == 0) /* NumByteToWrite < SPI_EE_PageSize */
    {
      SPI_EE_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_EE_PageSize */ 
    {
      while(NumOfPage--)
      {
        SPI_EE_PageWrite(pBuffer, WriteAddr, SPI_EE_PageSize);
        WriteAddr +=  SPI_EE_PageSize;
        pBuffer += SPI_EE_PageSize;  
      }    
     
      SPI_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);      
   }
  }
  else /* WriteAddr is not SPI_EE_PageSize aligned  */
  {
    if(NumOfPage== 0) /* NumByteToWrite < SPI_EE_PageSize */
    {
      if(NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_EE_PageSize */
      {
      	temp = NumOfSingle - count;
      	
      	SPI_EE_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count; 
        
        SPI_EE_PageWrite(pBuffer, WriteAddr, temp);       	
      }
      else
      {
      	SPI_EE_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_EE_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_EE_PageSize;
      NumOfSingle = NumByteToWrite % SPI_EE_PageSize;	
      
      SPI_EE_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;  
     
      while(NumOfPage--)
      {
        SPI_EE_PageWrite(pBuffer, WriteAddr, SPI_EE_PageSize);
        WriteAddr +=  SPI_EE_PageSize;
        pBuffer += SPI_EE_PageSize;  
      }
      
      if(NumOfSingle != 0)
      {
        SPI_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }      	
}

/*******************************************************************************
* Function Name  : SPI_EE_BufferRead
* Description    : Reads a block of data from the EEPROM.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the EEPROM.
*                  - ReadAddr : EEPROM's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead)
{
  /* Select the EEPROM: Chip Select low */
  SPI_EE_ChipSelect(Low);	   
  
  /* Send "Read from Memory " instruction */
  SPI_EE_SendByte(READ);	 
  
  /* Send ReadAddr high nibble address byte to read from */
  SPI_EE_SendByte((ReadAddr & 0xFF00) >> 8);  
  /* Send ReadAddr low nibble address byte to read from */
  SPI_EE_SendByte(ReadAddr & 0xFF);     
   	
  while(NumByteToRead--) /* while there is data to be read */
  {
    /* Read a byte from the EEPROM */			
    *pBuffer = SPI_EE_SendByte(Dummy_Byte);
   
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }	
  
  /* Deselect the EEPROM: Chip Select high */
  SPI_EE_ChipSelect(High);		
}

/*******************************************************************************
* Function Name  : SPI_EE_ChipSelect
* Description    : Selects or deselects the EEPROM.
* Input          : State : level to be applied on the EEPROM's ChipSelect pin.
* Output         : None
* Return         : None
*******************************************************************************/
static void SPI_EE_ChipSelect(u8 State)
{
  /* Set High or low the chip select line on P04 pin */
  GPIO_WriteBit(GPIO0, GPIO_Pin_4, (BitAction)State);
}

/*******************************************************************************
* Function Name  : SPI_EE_SendByte
* Description    : Sends a byte through the SSP interface and return the  byte 
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
static u8 SPI_EE_SendByte(u8 byte)
{
  /* Send byte through the SSP0 peripheral */	
  SSP_SendData(SSP0, byte);	
  
  /* Loop while Transmit FIFO is full */
  while(SSP_GetFlagStatus(SSP0, SSP_FLAG_TxFifoEmpty) == RESET);   
  
  /* Loop while Receive FIFO is empty */
  while(SSP_GetFlagStatus(SSP0, SSP_FLAG_RxFifoNotEmpty) == RESET);	
  
  /* Return the byte read from the SPI bus */
  return SSP_ReceiveData(SSP0); 	  
}

/*******************************************************************************
* Function Name  : SPI_EE_WriteEnable
* Description    : Enables the write access to the EEPROM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void SPI_EE_WriteEnable(void)  
{
  /* Select the EEPROM: Chip Select low */
  SPI_EE_ChipSelect(Low);	
  
  /* Send "Write Enable" instruction */
  SPI_EE_SendByte(WREN);
  
  /* Deselect the EEPROM: Chip Select high */
  SPI_EE_ChipSelect(High);	
}

/*******************************************************************************
* Function Name  : SPI_EE_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the  
*                  EEPROM's status  register  and  loop  until write  opertaion
*                  has completed.  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void SPI_EE_WaitForWriteEnd(void) 
{
  u8 EE_Status = 0;
  
  /* Select the EEPROM: Chip Select low */
  SPI_EE_ChipSelect(Low);
  	
  /* Send "Read Status Register" instruction */
  SPI_EE_SendByte(RDSR);
  
  /* Loop as long as the memory is busy with a write cycle */ 		
  do
  { 	
    /* Send a dummy byte to generate the clock needed by the EEPROM 
    and put the value of the status register in EE_Status variable */
    EE_Status = SPI_EE_SendByte(Dummy_Byte);  
															
  } while((EE_Status & WIP_Flag) == SET); /* Write in progress */  

  /* Deselect the EEPROM: Chip Select high */
  SPI_EE_ChipSelect(High);	 	
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
