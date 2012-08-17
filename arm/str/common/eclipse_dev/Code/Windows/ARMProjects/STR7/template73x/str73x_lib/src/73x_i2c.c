/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_i2c.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the I2C software functions.
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

/* Standard includes -------------------------------------------------------- */
#include "73x_i2c.h"
#include "73x_cfg.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************************************************************************
* Function Name  : I2C_DeInit                                                
* Description    : Deinitializes the I2C peripheral registers to their default
*                  reset values.                 
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.               
* Output         : None                                                      
* Return         : None                                                      
******************************************************************************/
void I2C_DeInit (I2C_TypeDef* I2Cx)
{
  /* Initialize all the registers of the specified I2C */
  
  if( I2Cx == I2C0)
  {
   CFG_PeripheralClockConfig(CFG_CLK_I2C0,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_I2C0,ENABLE);
  }
  else if  ( I2Cx == I2C1)
  {
  
   CFG_PeripheralClockConfig(CFG_CLK_I2C1,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_I2C1,ENABLE);
  }

}

/******************************************************************************
* Function Name  : I2C_Init                                                  
* Description    : Initializes the I2Cx  peripheral according to the specified
*                  parameters in the I2C_InitTypeDef structure.
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.
*                  - I2C_InitStruct: pointer to an I2C_InitTypeDef structure that
*                  contains the configuration information for the specified I2C
*                  peripheral.               
* Output         : None                                                      
* Return         : None                                                      
******************************************************************************/
void I2C_Init (I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct)
{
  u16 Result=0x0F;
  u32 MCLK=8000000;

  /* Initialize all the register of the specified I2C passed as parameter */
  I2Cx->CR &= I2C_CR_Mask;
  I2Cx->CCR=0x0;
  I2Cx->ECCR=0x0;
  I2Cx->OAR1=0x0;
  I2Cx->OAR2=0xE0;
  (void)I2Cx->SR1;
  (void)I2Cx->SR2;
  I2Cx->DR=0x0;

  /* FR2-FR0 frequency bits Config referring to MCLK */
  /* read the MCLK frequency */
  MCLK = PRCCU_GetFrequencyValue(PRCCU_CLOCK_MCLK);
  I2C_Cmd (I2Cx, DISABLE);
  if (MCLK <10000000)
    I2Cx->OAR2 = (I2Cx->OAR2 & 0x1F);
  else if (MCLK <16670000)
    I2Cx->OAR2 = (I2Cx->OAR2 & 0x1F) | 0x20;
  else if (MCLK < 26670000)
    I2Cx->OAR2 = (I2Cx->OAR2 & 0x1F) | 0x40;
  else if (MCLK <40000000)
    I2Cx->OAR2 = (I2Cx->OAR2 & 0x1F) | 0x60;
  else if (MCLK < 53330000)
    I2Cx->OAR2 = (I2Cx->OAR2 & 0x1F) | 0x80;
  else if (MCLK < 66000000)
    I2Cx->OAR2 = (I2Cx->OAR2 & 0x1F) | 0xA0;
  else if (MCLK <80000000)
    I2Cx->OAR2 = (I2Cx->OAR2 & 0x1F) | 0xC0;
  else if (MCLK <100000000)
    I2Cx->OAR2 = (I2Cx->OAR2 & 0x1F) | 0xE0;
  I2C_Cmd (I2Cx, ENABLE);

  /* General Call */
  I2Cx->CR |= I2C_InitStruct->I2C_GeneralCall;

  /* Acknowledgement */
  I2Cx->CR |= I2C_InitStruct->I2C_Ack;

  /* Own Address */
  I2Cx->OAR1 = I2C_InitStruct->I2C_OwnAddress;
  I2Cx->OAR2 |= (I2C_InitStruct->I2C_OwnAddress & 0x0300)>>7;

  /* Speed and Fast/Standard Mode */
  if (I2C_InitStruct->I2C_CLKSpeed <=100000)
  {
    /* Standard mode selected */
    Result = ((MCLK/I2C_InitStruct->I2C_CLKSpeed)-7)/2;
    /* Clear FM/SM bit */
    I2Cx->CCR = Result &0x7f;
  }
  else if (I2C_InitStruct->I2C_CLKSpeed <=400000)
  {
    /* Fast mode selected */
    Result = ((MCLK/I2C_InitStruct->I2C_CLKSpeed)-9)/3;
    /* Set FM/SM bit */
    I2Cx->CCR = Result |0x80;
  }
  I2Cx->ECCR = Result >>7;
}

/******************************************************************************
* Function Name  : I2C_StructInit		         		                      
* Description    : Initialize the I2C Init Structure parameters                    
* Input          : - I2C_InitStruct: pointer to an I2C_InitTypeDef structure
                     which will be initialized.  
* Output         : None	              
* Return         : None.						                             
******************************************************************************/
void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct)
{
  /* Reset I2C init structure parameters values */
  I2C_InitStruct->I2C_GeneralCall = I2C_GeneralCallDisable;
  I2C_InitStruct->I2C_OwnAddress = 0x0;
  I2C_InitStruct->I2C_CLKSpeed = 0x0;
  I2C_InitStruct->I2C_Ack = 0x0;
}

/******************************************************************************
* Function Name  : I2C_Cmd                                                    
* Description    : Enables or disables the specified I2C peripheral.                        
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.
*                  - NewState: new state of the I2Cx peripheral. This parameter
*                    can be: ENABLE or DISABLE.
* Output         : None                      
* Return         : None.                                                      
*******************************************************************************/
void I2C_Cmd (I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    /* Enable the I2C selected by setting twice the PE bit on the CR register */
    I2Cx->CR |= I2C_PESET_Mask;
    I2Cx->CR |= I2C_PESET_Mask;
  }
  else
  {
    /* Disable the selected I2C */
    I2Cx->CR &= ~I2C_PESET_Mask;
  }
}

/*******************************************************************************
* Function Name  : I2C_STARTGenerate                                          
* Description    : Generates I2C communication START condition.               
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.
*                  - NewState: new state of the Start condition. This parameter
*                    can be: ENABLE or DISABLE.                     
* Output         : None
* Return         : None.                                                      
*******************************************************************************/
void I2C_STARTGenerate (I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    I2Cx->CR |= I2C_START_Mask;
  }
  else
  {
    I2Cx->CR &= ~I2C_START_Mask;
  }
}

/******************************************************************************
* Function Name  : I2C_STOPGenerate                                           
* Description    : Generates I2C communication STOP condition.                
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.
*                  - NewState: new state of the Stop condition. This parameter
*                    can be: ENABLE or DISABLE.       
* Output         : None                
* Return         : None.                                                      
*******************************************************************************/
void I2C_STOPGenerate (I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    I2Cx->CR |= I2C_STOP_Mask;
  }
  else
  {
    I2Cx->CR &= ~I2C_STOP_Mask;
  }
}

/******************************************************************************
* Function Name  : I2C_AcknowledgeConfig                                      
* Description    : Enables or disables I2C acknowledge feature.               
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.
*                  - NewState: new state of the Acknowledgement. This parameter
*                    can be: ENABLE or DISABLE. 
* Output         : None                     
* Return         : None.                                                      
*******************************************************************************/
void I2C_AcknowledgeConfig (I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    I2Cx->CR |= I2C_ACK_Mask;
  }
  else
  {
    I2Cx->CR &= ~I2C_ACK_Mask;
  }
}

/*******************************************************************************
* Function Name  : I2C_ITConfig                                               
* Description    : Enables or disables I2C interrupt feature.                 
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.
*                  - NewState: new state of the specified I2C interrupt.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None                      
* Return         : None.                                                      
*******************************************************************************/
void I2C_ITConfig (I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    I2Cx->CR |= I2C_ITEnable;
  }
  else
  {
    I2Cx->CR &= I2C_ITDisable;
  }
}

/******************************************************************************************
* Function Name  : I2C_GetStatus                                                         
* Description    : Get the flags status in a 16 bit register.                            
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral. 
* Output         : None	                           
* Return         : A 16 bit register flag status.                                    
*        Reg bits: Ack-ENDAD-AF-STOPF-ARLO-BERR-GCAL-EVF-ADD10-TRA-BUSY-BTF-ADSL-M/SL-SB 
******************************************************************************************/
u16 I2C_GetStatus(I2C_TypeDef* I2Cx)
{
  u16 Flag1=0, Flag2=0;

  Flag1 = I2Cx->SR2;
  Flag1 = Flag1<<8;
  Flag2 = I2Cx->CR&0x04;
  return (((I2Cx->SR1|(Flag1)) & 0x3FFF) | (Flag2<<12));
}

/*****************************************************************************************
* Function Name  : I2C_GetLastEvent                                                     
* Description    : Get the Last happened I2C Event.                                              
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.  
* Output         : None	                          
* Return         : The Last happened Event.                                                    
*****************************************************************************************/
u16 I2C_GetLastEvent(I2C_TypeDef* I2Cx)
{
  u16 Flag1=0, LastEvent=0;

  Flag1 = I2Cx->SR2;
  Flag1 = Flag1<<8;
  LastEvent = (((I2Cx->SR1|(Flag1)) & 0x3FFF));
  return LastEvent;
}

/*****************************************************************************************
* Function Name  : I2C_EventCheck                                                       
* Description    : Get the Last I2C Event.                                              
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.                            
*                : - I2C_Event: the event to check. This parameter can be one of the following
*                    values:
*                         - I2C_EVENT_SLAVE_ADDRESS_MATCHED
*                         - I2C_EVENT_SLAVE_BYTE_RECEIVED
*                         - I2C_EVENT_SLAVE_BYTE_TRANSMITTED
*                         - I2C_EVENT_MASTER_MODE_SELECT
*                         - I2C_EVENT_MASTER_MODE_SELECTED
*                         - I2C_EVENT_MASTER_BYTE_RECEIVED
*                         - I2C_EVENT_MASTER_BYTE_TRANSMITTED
*                         - I2C_EVENT_MASTER_MODE_ADDRESS10
*                         - I2C_EVENT_SLAVE_STOP_DETECTED
*                         - I2C_EVENT_SLAVE_ACK_FAILURE  
* Output         : None	                                        
* Return         : An ErrorStatus enumuration value:
*                         - SUCCESS: Last event is equal to the I2C_Event
*                         - ERROR: Last event is different from the I2C_Event        
*****************************************************************************************/
ErrorStatus I2C_EventCheck(I2C_TypeDef* I2Cx, u16 I2C_Event )
{
  u16  LastEvent = I2C_GetLastEvent(I2Cx);

  if (LastEvent == I2C_Event)
  {
    return SUCCESS;
  }
  else
  {
    return ERROR;
  }
}

/*****************************************************************************************
* Function Name  : I2C_FlagStatus  
* Description    : Checks whether the specified I2C flag is set or not.
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral. 
*                  - I2C_Flag: flag to check. This parameter can be one of the
*                    following values:
*                         - I2C_FLAG_SB: Start bit flag    
*                         - I2C_FLAG_M_SL: Master/Slave flag   
*                         - I2C_FLAG_ADSL: Adress matched flag    
*                         - I2C_FLAG_BTF: Byte transfer finished flag    
*                         - I2C_FLAG_BUSY: Bus busy flag    
*                         - I2C_FLAG_TRA: Transmitter/Receiver flag    
*                         - I2C_FLAG_ADD10: 10-bit addressing in Master mode flag   
*                         - I2C_FLAG_EVF: Event flag     
*                         - I2C_FLAG_GCAL: General call flag    
*                         - I2C_FLAG_BERR: Bus error flag    
*                         - I2C_FLAG_ARLO: Arbitration lost flag    
*                         - I2C_FLAG_STOPF: Stop detection flag   
*                         - I2C_FLAG_AF: Acknowledge failure flag      
*                         - I2C_FLAG_ENDAD: End of address transmission flag   
*                         - I2C_FLAG_ACK: Acknowledge enable flag     
* Output         : None	                                                                                 
* Return         : The NewState of the I2C_Flag (SET or RESET).                              
*****************************************************************************************/
FlagStatus I2C_FlagStatus (I2C_TypeDef *I2Cx, u16 I2C_Flag)
{
  u16 Tmp= I2C_GetStatus(I2Cx) & I2C_Flag;

  if (Tmp != 0)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/*****************************************************************************************
* Function Name  : I2C_FlagClear                                                        
* Description    : Clears the I2C Flag passed as a parameter                            
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral. 
*                  - I2C_Flag: flag to check. This parameter can be one of the
*                    following values:
*                         - I2C_FLAG_SB: Start bit flag    
*                         - I2C_FLAG_M_SL: Master/Slave flag   
*                         - I2C_FLAG_ADSL: Adress matched flag    
*                         - I2C_FLAG_BTF: Byte transfer finished flag    
*                         - I2C_FLAG_BUSY: Bus busy flag    
*                         - I2C_FLAG_TRA: Transmitter/Receiver flag    
*                         - I2C_FLAG_ADD10: 10-bit addressing in Master mode flag   
*                         - I2C_FLAG_EVF: Event flag     
*                         - I2C_FLAG_GCAL: General call flag    
*                         - I2C_FLAG_BERR: Bus error flag    
*                         - I2C_FLAG_ARLO: Arbitration lost flag    
*                         - I2C_FLAG_STOPF: Stop detection flag   
*                         - I2C_FLAG_AF: Acknowledge failure flag      
*                         - I2C_FLAG_ENDAD: End of address transmission flag   
*                         - I2C_FLAG_ACK: Acknowledge enable flag                                      
*                  - parameter needed in the case that the flag to be cleared need   
*                  a write in one register  
* Output         : None	                                             
* Return         : None.                                                                
*****************************************************************************************/
void I2C_FlagClear (I2C_TypeDef *I2Cx, u16 I2C_Flag, ...)
{
  u8 Tmp = (u8)*((u32 *) & I2C_Flag + sizeof(I2C_Flag));

  /* flags that need a read of the SR2 register to be cleared */
  if (I2C_Flag==I2C_FLAG_ADD10 || I2C_Flag==I2C_FLAG_EVF || I2C_Flag==I2C_FLAG_BERR || I2C_Flag==I2C_FLAG_ARLO ||
      I2C_Flag==I2C_FLAG_STOPF || I2C_Flag==I2C_FLAG_AF  || I2C_Flag==I2C_FLAG_ENDAD)
  {
    /* Read the SR2 register */
    (void)I2Cx->SR2;

    /* Two flags need a second step to be cleared */
    switch (I2C_Flag)
    {
       case  I2C_FLAG_ADD10: I2Cx->DR = Tmp; break;
       case  I2C_FLAG_ENDAD: I2Cx->CR|=0x20; break;
    }
  }

  /* flags that need a read of the SR1 register to be cleared */
  else if (I2C_Flag==I2C_FLAG_SB || I2C_Flag==I2C_FLAG_ADSL || I2C_Flag==I2C_FLAG_BTF || I2C_Flag==I2C_FLAG_TRA)
       {
         /* Read the SR1 register */
         (void)I2Cx->SR1;

         /* three flags need a second step to be cleared */
         if (I2C_Flag == I2C_FLAG_SB)
           I2Cx->DR=Tmp;
         else
           if (I2C_Flag==I2C_FLAG_BTF || I2C_Flag==I2C_FLAG_TRA)
             Tmp=I2Cx->DR;
       }

       /* flags that need the PE bit to be cleared */
       else if ( I2C_Flag==I2C_FLAG_M_SL || I2C_Flag==I2C_FLAG_GCAL)
            {
              I2C_Cmd(I2Cx, DISABLE);
              I2C_Cmd(I2Cx, ENABLE);
            }
}

/********************************************************************************
* Function Name  : I2C_ByteSend                                                
* Description    : Send a data byte.                                 
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.                  
*                  - I2C_Data : the byte to be sent
* Output         : None	             
* Return         : None.                                                       
********************************************************************************/
void I2C_ByteSend (I2C_TypeDef* I2Cx, u8 I2C_Data)
{
  /* Write in the DR register the byte to be sent */
  I2Cx->DR = I2C_Data;
}

/********************************************************************************
* Function Name  : I2C_ByteReceive                                             
* Description    : Read the received byte. 
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.  
* Output         : None	                                                                  
* Return         : The received byte                                      
********************************************************************************/
u8 I2C_ByteReceive (I2C_TypeDef* I2Cx)
{
  /* Return the byte in the DR register */
  return I2Cx->DR;
}

/********************************************************************************
* Function Name  : I2C_AddressSend                                             
* Description    : Transmits the address byte to select the slave device.      
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.                   
*                  - I2C_Address: the slave address                                     
*                  - I2C_AddMode: the addressing mode. This parameter can be one 
*                    of the following values
*                         - I2C_MODE_ADDRESS7: 7-bit addressing mode 
*                         - I2C_MODE_ADDRESS10: 10-bit addressing mode         
*                  - I2C_Direction: the transmission mode. This parameter can be one 
*                    of the following values
*                         - I2C_MODE_TRANSMITTER: Transmitter mode 
*                         - I2C_MODE_RECEIVER: Receiver mode  
* Output         : None	
* Return         : None.                                                       
********************************************************************************/
void I2C_AddressSend (I2C_TypeDef* I2Cx, u16 I2C_Address, u8 I2C_AddMode, u8 I2C_Direction)
{
  if (I2C_AddMode == I2C_MODE_ADDRESS10 )
  /* 10 bit addressing mode */
  {
    /* Send the Header with bit0 Reset for write */
    I2Cx->DR = ((I2C_Address>>7) | 0xf0) & 0xfe;
    /* Test on EV9 and clear it */
    while( ! I2C_EventCheck(I2Cx, I2C_EVENT_MASTER_MODE_ADDRESS10) );  /* EV9*/
    /* Send the low byte of the slave address */
    I2C_ByteSend(I2Cx, (u8)I2C_Address);

    /* Test on the direction to define the read/write bit */
    if (I2C_Direction == I2C_MODE_RECEIVER)
    {
      /* Test on EV6 and clear it */
      while( ! I2C_EventCheck(I2Cx, I2C_EVENT_MASTER_MODE_SELECTED) );  /* EV6 */
      /* Clear EV6 by set again the PE bit */
      I2C_Cmd (I2Cx, ENABLE);
      /* Repeated START Generate */
      I2C_STARTGenerate (I2Cx, ENABLE);
      /* Test on EV5 and clear it */
      while( ! I2C_EventCheck(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) );  /* EV5 */
      /* Send the Header with bit0 Set for read */
      I2Cx->DR = ((I2C_Address>>7) | 0xf1);
    }
  }
  else
  /* 7 bit addressing mode */
  {
    if (I2C_Direction == I2C_MODE_RECEIVER)
      I2C_Address|=0x01;
    else
      I2C_Address&=~0x01;

    I2Cx->DR=(u8)I2C_Address;
  }
}

/********************************************************************************
* Function Name  : I2C_BufferSend                                              
* Description    : Transmits data from buffer. This function will return an error
*                  in case of a transmission failure.      
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.                 
*                  - PtrToBuffer: ‘u8’ pointer to the buffer that contains the
*                    data to be transmitted.     
*                  - NbOfBytes: specifies the number of bytes to be transferred
*                    from the buffer.   
* Output         : None	            
* Return         : An ErrorStatus enumuration value:
*                         - SUCCESS: transmission done without error
*                         - ERROR: an error detected during transmission       
********************************************************************************/
ErrorStatus I2C_BufferSend (I2C_TypeDef* I2Cx, u8* PtrToBuffer, u8 NbOfBytes)
{
  ErrorStatus Status=SUCCESS;
  u16 Event=0, ErrorFlag=0;

  while (NbOfBytes--)
  {
    I2Cx->DR= *PtrToBuffer;
    PtrToBuffer++;
    do
    {
      /* Wait till EV8 happen or error detected */
      Event = I2C_GetLastEvent(I2Cx);
      ErrorFlag = Event & 0x1600;
    }
    while ((Event != I2C_EVENT_MASTER_BYTE_TRANSMITTED) && ErrorFlag==0);

    /* In case of Error exit */
    if (ErrorFlag)
      {
        Status = ERROR;
        break;
      }
  }
  return Status;
}

/********************************************************************************
* Function Name  : I2C_BufferReceive                                           
* Description    : Receive a data buffer. This function will return an error
*                  in case of a transmission failure.         
* Input          : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.                 
*                  - PtrToBuffer: ‘u8’ pointer to the buffer where the received
*                    data will be stored.          
*                  - NbOfBytes: specifies the number of bytes to be received
*                    in the buffer.   
* Output         : None	            
* Return         : An ErrorStatus enumuration value:
*                         - SUCCESS: transmission done without error
*                         - ERROR: an error detected during transmission       
********************************************************************************/
ErrorStatus I2C_BufferReceive (I2C_TypeDef* I2Cx, u8* PtrToBuffer, u8 NbOfBytes)
{
  ErrorStatus Status = SUCCESS;
  u16 Event=0, ErrorFlag=0;

  while (NbOfBytes--)
  {
    do
    {
      Event = I2C_GetLastEvent(I2Cx);
      ErrorFlag = Event & 0x1600;
    }
    while ((Event != I2C_EVENT_MASTER_BYTE_RECEIVED) && ErrorFlag==0);

    /* In case of no Error */
    if (ErrorFlag==0)    
    {
      if ( NbOfBytes == 2 )
      {
        I2C_AcknowledgeConfig (I2Cx, DISABLE);
      }
      *PtrToBuffer = I2Cx->DR;
      PtrToBuffer++;
    }
    else break;
  }

  /* In case of Error exit */
  if (ErrorFlag)
    Status = ERROR;

  return Status;
}

/*******************************************************************************
* Function Name  : I2C_RegisterRead                                                 
* Description    : Reads any I2C register and returns its value.                    
* Input1         : - I2Cx: where x can be 0 or 1 to select the I2C peripheral.                       
*                : - I2C_Register: the I2Cx register to be read. This parameter 
*                    can be one of the following values:                      
*                         - I2C_CR:   CR register. 
*                         - I2C_SR1:  SR1 register.
*                         - I2C_SR2:  SR2 register.
*                         - I2C_CCR:  CCR register.
*                         - I2C_OAR1: OAR1 register.
*                         - I2C_OAR2: OAR2 register.
*                         - I2C_DR:   DR register.
*                         - I2C_ECCR: ECCR register.
* Output         : None
* Return         : The value of the register passed as parameter               
********************************************************************************/
u8 I2C_RegisterRead (I2C_TypeDef* I2Cx, u16 I2C_Register)
{
  return (*(u8 *)(*((u32 *)&I2Cx) + I2C_Register));
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
