/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : i2c_ee.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides a set of functions needed to manage the
*                      I2C M24C08 EEPROM, mounted on the STR73x-EVAL board.
********************************************************************************
* History:
* 09/27/2005 :  V1.0
*******************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Standard include ----------------------------------------------------------*/
#include "i2c_ee.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_Speed    10000
#define I2C_OwnAddr  0xA0
#define I2C_PageSize 16

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
I2C_InitTypeDef  I2C_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : I2C_EE_Init
* Description    : Initializes the peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_Init()
{
  /* enable GPIO2 clock */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO2, ENABLE);
  
  /* enable I2C0 clock */
  CFG_PeripheralClockConfig(CFG_CLK_I2C0, ENABLE);
  
  /* I2C0 pins Configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_14 | GPIO_PIN_15 ;
  GPIO_Init (GPIO2, &GPIO_InitStructure);

  /* configure P2.12 and P2.13 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_12 | GPIO_PIN_13 ;
  GPIO_Init (GPIO2, &GPIO_InitStructure); 

  GPIO_BitWrite(GPIO2, GPIO_PIN_12, Bit_RESET);
  GPIO_BitWrite(GPIO2, GPIO_PIN_13, Bit_RESET);

  /* I2C initialization */
  I2C_InitStructure.I2C_GeneralCall = I2C_GeneralCallDisable;
  I2C_InitStructure.I2C_Ack = I2C_AckEnable;
  I2C_InitStructure.I2C_CLKSpeed = I2C_Speed;
  I2C_InitStructure.I2C_OwnAddress = I2C_OwnAddr;
  
  I2C_Cmd (I2C0, ENABLE);
  I2C_Init(I2C0, &I2C_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_EE_DeInit
* Description    : Deinitializes the peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_DeInit()
{
  /* set the pins used in this driver to their reset value */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_12 | GPIO_PIN_13 | 
                                  GPIO_PIN_14 | GPIO_PIN_15 ;
  GPIO_Init (GPIO2, &GPIO_InitStructure);
  
  /* disable I2C0 clock and reset it */
  CFG_PeripheralClockConfig(CFG_CLK_I2C0, DISABLE);  
}

/*******************************************************************************
* Function Name  : I2C_EE_BlockWrite
* Description    : Writes block of data to the I2C EEPROM.
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
    
  if(Addr == 0) /* WriteAddr is I2C_PageSize aligned  */
  {
    if(NumOfPage == 0) /* NumByteToWrite < I2C_PageSize */
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      I2C_EE_WaitForLastTask(0xFFFF);
    }
    else /* NumByteToWrite > I2C_PageSize */ 
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitForLastTask(0xFFFF);
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }    
     
      while(NumOfSingle--)
      {
        I2C_EE_ByteWrite(pBuffer, WriteAddr);
        I2C_EE_WaitForLastTask(0xFFFF);
        WriteAddr += 1;
        pBuffer += 1;   
     }
   }
  }
  else /* WriteAddr is not I2C_PageSize aligned  */
  {
    if(NumOfPage== 0) /* NumByteToWrite < I2C_PageSize */
    {
      while(NumOfSingle--)
      {
        I2C_EE_ByteWrite(pBuffer, WriteAddr);
        I2C_EE_WaitForLastTask(0xFFFF);
        WriteAddr += 1;
        pBuffer += 1;   
      }
    }
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      while(count--)
      {
        I2C_EE_ByteWrite(pBuffer, WriteAddr);
        I2C_EE_WaitForLastTask(0xFFFF);
        WriteAddr += 1;
        pBuffer += 1;   
      } 

      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitForLastTask(0xFFFF);
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitForLastTask(0xFFFF); 
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
  /* send STRAT condition */
  I2C_STARTGenerate(I2C0, ENABLE);

  /* test on EV5 and clear it */
  while(! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_MODE_SELECT));  

  /* send EEPROM address for write */
  I2C_AddressSend(I2C0, EEPROM_Block0_ADDRESS, I2C_MODE_ADDRESS7, I2C_MODE_TRANSMITTER);
  
  /* test on EV6 and clear it */
  while(! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_MODE_SELECTED));

  /* clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C0, ENABLE);
      
  /* send the EEPROM's internal address to write to */
  I2C_ByteSend(I2C0, WriteAddr);
  
  /* test on EV8 and clear it */
  while(! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* send the byte to be written */
   I2C_ByteSend(I2C0, *pBuffer); 
   
  /* test on EV8 and clear it */
  while(! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* send STOP condition */
  I2C_STOPGenerate (I2C0, ENABLE);
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
  /* send STRAT condition */
  I2C_STARTGenerate(I2C0, ENABLE);
  
  /* test on EV5 and clear it */
  while(!I2C_EventCheck(I2C0, I2C_EVENT_MASTER_MODE_SELECT)); 
  
  /* send EEPROM address for write */
  I2C_AddressSend(I2C0, EEPROM_Block0_ADDRESS, I2C_MODE_ADDRESS7, I2C_MODE_TRANSMITTER);

  /* test on EV6 and clear it */
  while(!I2C_EventCheck(I2C0, I2C_EVENT_MASTER_MODE_SELECTED));  

  /* clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C0, ENABLE);

  /* send the EEPROM's internal address to write to */    
  I2C_ByteSend(I2C0, WriteAddr);  

  /* test on EV8 and clear it */
  while(! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  while(NumByteToWrite--)  /* while there is data to be written */
  {
    /* send the current byte */
    I2C_ByteSend(I2C0, *pBuffer); 
    
    /* point to the next byte to be written */
    pBuffer++; 
  
    /* test on EV8 and clear it */
    while (! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }
  
  /* send STOP condition */
  I2C_STOPGenerate (I2C0, ENABLE);
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
  /* send START condition */
  I2C_STARTGenerate(I2C0, ENABLE);
  
  /* test on EV5 and clear it */
  while(! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_MODE_SELECT));
  
  /* send EEPROM address for write */
  I2C_AddressSend(I2C0, EEPROM_Block0_ADDRESS, I2C_MODE_ADDRESS7, I2C_MODE_TRANSMITTER);
  
  /* test on EV6 and clear it */
  while(! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_MODE_SELECTED));
  
  /* clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C0, ENABLE);

  /* send the EEPROM's internal address to write to */
  I2C_ByteSend(I2C0, ReadAddr);  
  
  /* test on EV8 and clear it */
  while(! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* send STRAT condition a second time */  
  I2C_STARTGenerate(I2C0, ENABLE);
  
  /* test on EV5 and clear it */
  while(! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_MODE_SELECT));
  
  /* send EEPROM address for read */
  I2C_AddressSend(I2C0, EEPROM_Block0_ADDRESS, I2C_MODE_ADDRESS7, I2C_MODE_RECEIVER);
  
  /* test on EV6 and clear it */
  while(! I2C_EventCheck(I2C0, I2C_EVENT_MASTER_MODE_SELECTED));
  
  /* clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C0, ENABLE);

  while(NumByteToRead)  /* while there is data to be read */
  {
    /* test on EV7 and clear it */
    if(I2C_EventCheck(I2C0, I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {
      if(NumByteToRead == 2)
      {
        I2C_AcknowledgeConfig(I2C0, DISABLE);
      }

      if(NumByteToRead == 1)
      {
        /* send STOP Condition */
        I2C_STOPGenerate(I2C0, ENABLE);
      }
      
      /* read a byte from the EEPROM */
      *pBuffer = I2C_ByteReceive(I2C0);

      /* point to the next location where the byte read will be saved */
      pBuffer++; 
      
      NumByteToRead--;    
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C0, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_WaitForLastTask
* Description    : Waits until write operation has completed.  
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_WaitForLastTask(u32 nCount)
{
  u32 j;

  for(j = 0; j < nCount; j++);
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
