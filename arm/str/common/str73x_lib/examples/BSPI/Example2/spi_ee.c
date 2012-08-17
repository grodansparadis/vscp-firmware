/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : spi_ee.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides a set of functions needed to manage 
*                      the SPI M95080 EEPROM, mounted on the STR73x-EVAL board. 
*                      This driver can be tailored to manage other SPI EEPROM
*                      devices which supports 2-bytes address length.
*                      To do this, user has to change the "SPI_PageSize" constant  
*                      value with the page size of the EEPROM to be used.
*                      To use this driver in the target system the user needs to 
*                      do the following :
*                        1. Initialize the driver using "SPI_EE_Init" function.
*                        2. He can choose to erase the memory using "SPI_EE_Erase" 
*                           function.
*                        3. Then he can start writing to/reading from the EEPROM.
********************************************************************************
* History:
* 09/27/2005 :  V1.0
********************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Standard include ----------------------------------------------------------*/
#include "spi_ee.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BSPI2_RXR_BaseAddr  0xFFFFE000
#define SPI_PageSize 32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
BSPI_InitTypeDef BSPI_InitStructure;
GPIO_InitTypeDef GPIOs_InitStructure;
DMA_InitTypeDef  DMA_InitStructure;

/* Private function prototypes -----------------------------------------------*/
static u8 SPI_EE_ReadStatus(void);
static void BSPI_EE_WaitForLastTask(void); 
static void BSPI_EE_WriteEnable(void);
static u8 Send_Byte(u8 byte);
static void BSPI_EE_ChipSelect(u8 State);

/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : SPI_EE_Init
* Description    : Initializes the peripherals used by the SPI EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_Init()
{
  /* enable GPIO5 clock */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO5, ENABLE);
  
  /* enable BSPI2 clock */
  CFG_PeripheralClockConfig(CFG_CLK_BSPI2, ENABLE);
  
  /* enable DMA0 clock */
  CFG_PeripheralClockConfig(CFG_CLK_DMA0, ENABLE);
	
  /* BSPI2 pins configuration */
  GPIOs_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIOs_InitStructure.GPIO_Pins = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_Init(GPIO5, &GPIOs_InitStructure);
  
  /* configure P5.3 as output Push-Pull to drive the EEPROM ChipSelect pin */
  GPIOs_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIOs_InitStructure.GPIO_Pins = GPIO_PIN_3;  
  GPIO_Init(GPIO5, &GPIOs_InitStructure);
  
  /* deselect the EEPROM */
  BSPI_EE_ChipSelect(High);

  /* BSPI2 configuration */
  BSPI_InitStructure.BSPI_RxFifoSize = 1;
  BSPI_InitStructure.BSPI_TxFifoSize = 1;
  BSPI_InitStructure.BSPI_SSPin = BSPI_SSPin_Masked;
  BSPI_InitStructure.BSPI_CLKDivider = 6;
  BSPI_InitStructure.BSPI_CPOL = BSPI_CPOL_High;
  BSPI_InitStructure.BSPI_CPHA = BSPI_CPHA_1Edge;
  BSPI_InitStructure.BSPI_WordLength = BSPI_WordLength_8b;
  BSPI_InitStructure.BSPI_Mode = BSPI_Mode_Master;

  BSPI_DeInit(BSPI2);
  BSPI_Init(BSPI2, &BSPI_InitStructure);
  
  /* BSPI2 DMA interface configuration */
  BSPI_DMAConfig(BSPI2, BSPI_DMAReq_Rx, ENABLE);
  BSPI_DMARxBurstConfig(BSPI2, BSPI_DMA_RxBurst_1Word);
  
  BSPI_Cmd(BSPI2, ENABLE);
  
  /* DMA0 stream0 configuration */
  DMA_StructInit(&DMA_InitStructure);

  DMA_InitStructure.DMA_Stream = DMA_Stream0;
  DMA_InitStructure.DMA_SRC = DMA_SRC_NOT_INCR;
  DMA_InitStructure.DMA_DST = DMA_DST_INCR;
  DMA_InitStructure.DMA_SRCSize = DMA_SRCSize_Byte;
  DMA_InitStructure.DMA_SRCBurst = DMA_SRCBurst_1Word;
  DMA_InitStructure.DMA_DSTSize = DMA_DSTSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStructure.DMA_Dir = DMA_Dir_PeriphSRC;
  DMA_InitStructure.DMA_SRCBaseAddr = BSPI2_RXR_BaseAddr;
  
  DMA_DeInit(DMA0, DMA_Stream0);
  DMA_Init(DMA0, &DMA_InitStructure);
 
  /* set the DMA priority to highest on AHB bus */
  DMA_AHBArbitrationConfig(DMA_Priority_High); 
}

/*******************************************************************************
* Function Name  : SPI_EE_DeInit
* Description    : Deinitializes the peripherals used by the SPI EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_DeInit()
{
  /* disable DMA0 stream0 */
  DMA_DeInit(DMA0, DMA_Stream0);
  
  /* set the pins used in this driver to their reset value */
  GPIOs_InitStructure.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
  GPIOs_InitStructure.GPIO_Pins = GPIO_PIN_3 | GPIO_PIN_5 |  
                                  GPIO_PIN_6 | GPIO_PIN_7 ;
  GPIO_Init(GPIO5, &GPIOs_InitStructure);
  
  /* disable BSPI2 clock and reset it */
  CFG_PeripheralClockConfig(CFG_CLK_BSPI2, DISABLE);
}

/*******************************************************************************
* Function Name  : SPI_EE_BlockWrite
* Description    : Writes block of data to the EEPROM. In this function, each 
*                  byte is written using Byte WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_BlockWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
{
  while(NumByteToWrite--) /* while there is data to be written */
  {
    /* enable the write access to the EEPROM */
    BSPI_EE_WriteEnable();		
    
    /* select the EEPROM */
    BSPI_EE_ChipSelect(Low);	   

    /* send "Write to Memory " instruction */    
    Send_Byte(WRITE);	
    
    /* send the high byte of the EEPROM's internal address to write to */
    Send_Byte(WriteAddr >> 8);  
    
    /* send the low byte of the EEPROM's internal address to write to */
    Send_Byte(WriteAddr & 0xFF);   
    
    /* send the current byte  */			
    Send_Byte(*pBuffer); 	        
    
    /* deselect the EEPROM */
    BSPI_EE_ChipSelect(High);	
    
    /* point to the next byte to be written */
    pBuffer++; 
    
    /* increment the EEPROM's internal address to write to */   
    WriteAddr++;
    	
    /* waiting loop until write opertaion has completed */
    BSPI_EE_WaitForLastTask(); 
  }		
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

  Addr = WriteAddr % SPI_PageSize;
  count = SPI_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_PageSize;
  NumOfSingle = NumByteToWrite % SPI_PageSize;
  
  if(Addr == 0) /* WriteAddr is SPI_PageSize aligned  */
  {
    if(NumOfPage == 0) /* NumByteToWrite < SPI_PageSize */
    {
      SPI_EE_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_PageSize */ 
    {
      while(NumOfPage--)
      {
        SPI_EE_PageWrite(pBuffer, WriteAddr, SPI_PageSize);
        WriteAddr +=  SPI_PageSize;
        pBuffer += SPI_PageSize;  
      }    
     
      SPI_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);      
   }
  }
  else /* WriteAddr is not SPI_PageSize aligned  */
  {
    if(NumOfPage== 0) /* NumByteToWrite < SPI_PageSize */
    {
      if(NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_PageSize */
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
    else /* NumByteToWrite > SPI_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_PageSize;
      NumOfSingle = NumByteToWrite % SPI_PageSize;	
      
      SPI_EE_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;  
     
      while(NumOfPage--)
      {
        SPI_EE_PageWrite(pBuffer, WriteAddr, SPI_PageSize);
        WriteAddr +=  SPI_PageSize;
        pBuffer += SPI_PageSize;  
      }
      
      if(NumOfSingle != 0)
      {
        SPI_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }      	
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
*                    must be equal or less than "SPI_PageSize" value. 
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite)
{
  /* enable the write access to the EEPROM */
  BSPI_EE_WriteEnable();		
    
  /* select the EEPROM */
  BSPI_EE_ChipSelect(Low);	   

  /* send "Write to Memory " instruction */    
  Send_Byte(WRITE);	
    
  /* send the high byte of the EEPROM's internal address to write to */
  Send_Byte(WriteAddr >> 8);  
    
  /* send the low byte of the EEPROM's internal address to write to */
  Send_Byte(WriteAddr & 0xFF);             
	               	    
  while(NumByteToWrite--) /* while there is data to be written */
  {
    /* send the current byte */			
    Send_Byte(*pBuffer); 	           
    
    /* point to the next byte to be written */
    pBuffer++; 
  }		
 
  /* deselect the EEPROM */
  BSPI_EE_ChipSelect(High);
  
  /* waiting loop until write opertaion has completed */
  BSPI_EE_WaitForLastTask();
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
  /* select the EEPROM */
  BSPI_EE_ChipSelect(Low);	   
  
  /* send "Read from Memory " instruction */
  Send_Byte(READ);	 
  
  /* send the high byte of the EEPROM's internal address to read from */	
  Send_Byte(ReadAddr >> 8); 

  /* send the low byte of the EEPROM's internal address to read from */   
  Send_Byte(ReadAddr & 0xFF);     
    	
  while(NumByteToRead--) /* while there is data to be read */
  {
    /* read a byte from the EEPROM */			
    *pBuffer = Send_Byte(Dummy_Byte);
    
    /* point to the next location where the byte read will be saved */
    pBuffer++;
  }	
  
  /* deselect the EEPROM */
  BSPI_EE_ChipSelect(High);		
}

/*******************************************************************************
* Function Name  : SPI_EE_DMABufferRead
* Description    : Reads a block of data from the EEPROM. This function use the 
*                  DMA0 stream0 to store the data received by the BSPI2.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the EEPROM.
*                  - ReadAddr : EEPROM's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_DMABufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead)
{
  DMA_InitStructure.DMA_BufferSize = NumByteToRead;
  DMA_InitStructure.DMA_DSTBaseAddr = (u32)pBuffer;

  DMA_Init(DMA0, &DMA_InitStructure);

  /* select the EEPROM */
  BSPI_EE_ChipSelect(Low);	   
  
  /* send "Read from Memory " instruction */
  Send_Byte(READ);	 
  
  /* send the high byte of the EEPROM's internal address to read from */	
  Send_Byte( ReadAddr >> 8); 

  /* send the low byte of the EEPROM's internal address to read from */   
  Send_Byte(ReadAddr & 0xFF);     
  
  /* purge the receive FIFO */
  BSPI_RxFifoDisable(BSPI2);

  /* enable DMA0 stream0 */
  DMA_Cmd(DMA0, DMA_Stream0, ENABLE);
  
  /* enable BSPI2 DMA interface */
  BSPI_DMACmd(BSPI2, ENABLE);
  	
  while(NumByteToRead--) /* while there is data to be read */
  {
    /* send byte through the BSPI2 peripheral */	
    BSPI_WordSend(BSPI2, Dummy_Byte); 	
  
    /* loop while Transmit FIFO is full */
    while(BSPI_FlagStatus(BSPI2, BSPI_FLAG_TFF) == SET);  
  }	
  
  /* deselect the EEPROM */
  BSPI_EE_ChipSelect(High);	
    
  /* disable BSPI2 DMA interface */
  BSPI_DMACmd(BSPI2, DISABLE);		
}
 
/*******************************************************************************
* Function Name  : SPI_EE_Erase
* Description    : Fills the whole memory with value 0xFF. This will set the  
*                  memory to its initial delivery state.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_Erase()
{
  u8 PageIndex = 0, Counter = 0; 
  u16 PageAddr = 0;
  
  for(PageIndex = 32; PageIndex != 0; PageIndex--)
  {        	
    /* enable the write access to the EEPROM */
    BSPI_EE_WriteEnable();		
  
    /* select the EEPROM */
    BSPI_EE_ChipSelect(Low);
    
    /* send "Write to Memory " instruction */	   
    Send_Byte(WRITE);	
    
    /* send the high byte of the EEPROM's internal page address to write to */	
    Send_Byte(PageAddr >> 8); 
 
    /* send the low byte of the EEPROM's internal page address to write to */	
    Send_Byte(PageAddr &0xFF);   
      
    for(Counter = 32; Counter != 0; Counter--) /* Page write */
    { 				
      Send_Byte(0xFF); 	        
    }
    
    /* deselect the EEPROM */
    BSPI_EE_ChipSelect(High);	

    /* waiting loop until write opertaion has completed */
    BSPI_EE_WaitForLastTask(); 
    
    /* select the next page address to write to */
    PageAddr += 32;    	    
  }		
}

/*******************************************************************************
* Function Name  : SPI_EE_ReadStatus
* Description    : Returns the EEPROM's status register.
* Input          : None
* Output         : None
* Return         : EEPROM's status register value .
*******************************************************************************/
static u8 SPI_EE_ReadStatus()
{
  u8 EE_Status = 0;
	
  /* select the EEPROM */
  BSPI_EE_ChipSelect(Low);
	
  /* send "Read Status Register" instruction */
  Send_Byte(RDSR);
		
  /* send a dummy byte to generate the clock needed by the EEPROM 
  and put the value of the status register in EE_Status variable */
  EE_Status = Send_Byte(Dummy_Byte);

  /* deselect the EEPROM */
  BSPI_EE_ChipSelect(High);	
  
  /* return the status register value */		
  return (EE_Status);   
}

/*******************************************************************************
* Function Name  : BSPI_EE_ChipSelect
* Description    : Selects and deselects the EEPROM.
* Input          : State : level to be applied on the EEPROM's ChipSelect pin.
* Output         : None
* Return         : None
*******************************************************************************/
static void BSPI_EE_ChipSelect(u8 State)
{
  GPIO_BitWrite(GPIO5, GPIO_PIN_3, (BitAction)State);
}

/*******************************************************************************
* Function Name  : Send_Byte
* Description    : Sends a byte through the BSPI interface and return the  byte 
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
static u8 Send_Byte(u8 byte)
{
  /* send byte through the BSPI2 peripheral */	
  BSPI_WordSend(BSPI2, byte); 	
  
  /* loop while Transmit FIFO is full */
  while(BSPI_FlagStatus(BSPI2, BSPI_FLAG_TFF) == SET);   

  /* loop while Receive FIFO is empty */
  while(BSPI_FlagStatus(BSPI2, BSPI_FLAG_RFNE) == RESET);	
  
  /* return the byte read from the SPI bus */
  return(BSPI_WordReceive(BSPI2)); 	  
}

/*******************************************************************************
* Function Name  : BSPI_EE_WriteEnable
* Description    : Enables the write access to the EEPROM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void BSPI_EE_WriteEnable()  
{
  /* select the EEPROM */	
  BSPI_EE_ChipSelect(Low);	
  
  /* send "Write Enable" instruction */
  Send_Byte(WREN);
  
  /* deselect the EEPROM */
  BSPI_EE_ChipSelect(High);	
}

/*******************************************************************************
* Function Name  : BSPI_EE_WaitForLastTask
* Description    : Polls the status of the Write In Progress (WIP) flag in the  
*                  EEPROM's status  register  and  loop  until write  opertaion
*                  has completed.  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void BSPI_EE_WaitForLastTask() 
{
  u8 EE_Status = 0;
  
  /* loop as long as the memory is busy with a write cycle */ 		
  do
  { 			 
    EE_Status = SPI_EE_ReadStatus();    																				
  } while((EE_Status & WIP_Flag) == Write_In_Progress);   	
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
