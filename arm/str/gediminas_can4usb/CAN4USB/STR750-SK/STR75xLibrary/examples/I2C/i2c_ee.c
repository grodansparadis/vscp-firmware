/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : i2c_ee.c
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006 
* Description        : This file provides a set of functions needed to manage the
*                      communication between I2C peripheral and I2C M24C08 EEPROM.
********************************************************************************
* History:
* 07/17/2006 : V1.0
* 03/10/2006 : V0.1  
*******************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "i2c_ee.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_Speed    400000
#define I2C_OwnAddr  0xA0
#define I2C_PageSize 16

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u16 EEPROM_ADDRESS;
vu16 SR2_Tmp = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  /* SCL and SDA I2C pins configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 ;
  GPIO_Init(GPIO0, &GPIO_InitStructure);

  /* Configure P2.0 as E2 EEPROM select pin drivers */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
  GPIO_Init (GPIO2, &GPIO_InitStructure); 
}

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : I2C Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_Configuration(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 

  /* I2C configuration */
  I2C_InitStructure.I2C_GeneralCall = I2C_GeneralCall_Disable;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_CLKSpeed = I2C_Speed;
  I2C_InitStructure.I2C_OwnAddress = I2C_OwnAddr;
  
  /* I2C Peripheral Enable */
  I2C_Cmd (ENABLE);
  /* Apply I2C configuration after enabling it */
  I2C_Init(&I2C_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_EE_Init
* Description    : Initializes peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_Init()
{
  /* GPIO configuration */
  GPIO_Configuration();

  /* I2C configuration */
  I2C_Configuration();

  /* depending on the EEPROM Address selected in the i2c_ee.h file */
#ifdef EEPROM_Block0_ADDRESS
  /* Set the pin connected to E2 chip enable */
  GPIO_WriteBit(GPIO2, GPIO_Pin_0, Bit_SET);
  /* Select the EEPROM Block0 to write on */
  EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
#endif
#ifdef EEPROM_Block1_ADDRESS
  /* Set the pin connected to E2 chip enable */
  GPIO_WriteBit(GPIO2, GPIO_Pin_0, Bit_SET);
  /* Select the EEPROM Block1 to write on */
  EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
#endif
#ifdef EEPROM_Block2_ADDRESS
  /* Set the pin connected to E2 chip enable */
  GPIO_WriteBit(GPIO2, GPIO_Pin_0, Bit_SET);
  /* Select the EEPROM Block2 to write on */
  EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
#endif
#ifdef EEPROM_Block3_ADDRESS
  /* Set the pin connected to E2 chip enable */
  GPIO_WriteBit(GPIO2, GPIO_Pin_0, Bit_SET);
  /* Select the EEPROM Block3 to write on */
  EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
#endif
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage, NumOfSingle, Addr, count;

  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitForWriteEnd();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
    	I2C_EE_WaitForWriteEnd();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    	I2C_EE_WaitForWriteEnd();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitForWriteEnd();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitForWriteEnd();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitForWriteEnd();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitForWriteEnd();
      }
    }
  }  	
}

/*******************************************************************************
* Function Name  : I2C_EE_ByteWrite
* Description    : Writes one byte to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr)
{
  /* Send STRAT condition */
  I2C_GenerateSTART(ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));  

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_MODE_TRANSMITTER);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECTED));

  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(ENABLE);
      
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(WriteAddr);
  
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send the byte to be written */
  I2C_SendData(*pBuffer); 
   
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STOP condition */
  I2C_GenerateSTOP(ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_PageWrite
* Description    : Writes more than one byte to the EEPROM with a single WRITE
*                  cycle. The number of byte can't exceed the EEPROM page size.
* Input          : - pBuffer : pointer to the buffer containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
  /* Send START condition */
  I2C_GenerateSTART(ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_MODE_TRANSMITTER);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECTED));  

  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(ENABLE);

  /* Send the EEPROM's internal address to write to */    
  I2C_SendData(WriteAddr);  

  /* Test on EV8 and clear it */
  while(! I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(*pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferRead
* Description    : Reads a block of data from the EEPROM.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the EEPROM.
*                  - ReadAddr : EEPROM's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{  
  /* Send START condition */
  I2C_GenerateSTART(ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  
  /* In the case of a single data transfer disable ACK before reading the data */
  if(NumByteToRead==1) 
  {
    I2C_AcknowledgeConfig(DISABLE);
  }
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_MODE_TRANSMITTER);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECTED));
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(ReadAddr);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_MODE_RECEIVER);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECTED));
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(ENABLE);

  /* While there is data to be read */
  while(NumByteToRead)  
  {
    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {
      if(NumByteToRead == 2)
      {
        /* Disable Acknowledgement */
        I2C_AcknowledgeConfig(DISABLE);
      }

      if(NumByteToRead == 1)
      {
        /* Send STOP Condition */
        I2C_GenerateSTOP(ENABLE);
      }
      
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData();

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;    
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_WaitForWriteEnd
* Description    : Wait for EEPROM to terminate the write operation task
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_WaitForWriteEnd(void)
{
  do
  {
    /* Send START condition */
    I2C_GenerateSTART(ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_MODE_TRANSMITTER);
    
    /* Wait the set of EVF flag */
    while(!(I2C_ReadRegister(I2C_SR1) & I2C_FLAG_EVF));

    /* Store the AF flag status */
    SR2_Tmp=(I2C_ReadRegister(I2C_SR2)<<8);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECTED));
  
    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(ENABLE);

  }while(SR2_Tmp & I2C_FLAG_AF);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
