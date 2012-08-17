/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_uart.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the UART software functions.
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
#include "73x_uart.h"
#include "73x_prccu.h"
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
* Function Name  : UART_DeInit
* Description    : Deinitializes the UARTx peripheral registers to their default
*                : reset values.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void UART_DeInit(UART_TypeDef *UARTx)
{

  if( UARTx == UART0)
  {
   CFG_PeripheralClockConfig(CFG_CLK_UART0,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_UART0,ENABLE);
  }
  else if  ( UARTx == UART1)
  {
  
   CFG_PeripheralClockConfig(CFG_CLK_UART1,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_UART1,ENABLE);
  }
  else if  ( UARTx == UART2)
  {
   CFG_PeripheralClockConfig(CFG_CLK_UART2,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_UART2,ENABLE);
  }
  else if  ( UARTx == UART3)
  {
   CFG_PeripheralClockConfig(CFG_CLK_UART3,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_UART3,ENABLE);
  }

}
/******************************************************************************
* Function Name  : UART_StructInit
* Description    : Fills in a UART_InitTypeDef structure with the reset value of
*                : each parameter.
* Input          : - UART_InitStruct : pointer to a UART_InitTypeDef structure
                   which will be initialized.
* Output         : None
* Return         : None
******************************************************************************/
void UART_StructInit(UART_InitTypeDef* UART_InitStruct)
{
  /*Initializes BaudRate*/
  UART_InitStruct->UART_BaudRate = 9600;

  /*Initializes Mode*/
  UART_InitStruct->UART_Mode = 0x000;

  /*Initializes Stop Bits*/
  UART_InitStruct->UART_StopBits = UART_StopBits_0_5;

  /*Initializes Parity*/
  UART_InitStruct->UART_Parity = UART_Parity_Even;

  /*Enable standard transmit/recieve mode*/
  UART_InitStruct->UART_Loop_Standard = UART_Standard;

  /*Disable yhe Receive mode*/
  UART_InitStruct->UART_Rx = UART_Rx_Disable;

  /*Disable the FOFO mode*/
  UART_InitStruct->UART_FIFO = UART_FIFO_Disable;
}
/******************************************************************************
* Function Name  : UART_Init
* Description    : Initializes the UARTx  peripheral according to the specified
*                  parameters in the UART_InitTypeDef structure.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - UART_InitStruct: pointer to a UART_InitTypeDef structure that
                   contains the configuration information for the specified UART
                   peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef* UART_InitStruct)
{
  u32 tmpBaudRate = 0;

  /*Initialize struct*/
  UARTx->BR = UART_BR_MASK;
  UARTx->CR &= UART_CR_MASK;

  /*Configure BaudRate*/
  tmpBaudRate = (u32)((PRCCU_GetFrequencyValue(PRCCU_CLOCK_MCLK) * 10) / (16*UART_InitStruct->UART_BaudRate));
  
  /*Search the reload value (Integer)*/
  if(tmpBaudRate -((tmpBaudRate/10)*10) < 5)
  {
    UARTx->BR = tmpBaudRate /10;
  }
  else
  {
    UARTx->BR = (tmpBaudRate /10)+1;
  }

  /*Choose Mode*/
  UARTx->CR = (UARTx->CR & 0xFFF8) | (u16)UART_InitStruct->UART_Mode;

  /*Choose Stop Bits*/
  UARTx->CR = (UARTx->CR & 0xFFE7) | (u16)UART_InitStruct->UART_StopBits;

  /*Choose Parity*/
  UARTx->CR |= UART_InitStruct->UART_Parity;

  /*Choose loopback or standard transmit/recieve mode*/
  UARTx->CR |= UART_InitStruct->UART_Loop_Standard;

  /*Enable or disable receive mode*/
  UARTx->CR |=  UART_InitStruct->UART_Rx;

  /*Enable or disable FIFO mode*/
  UARTx->CR |= UART_InitStruct->UART_FIFO;
}

/*******************************************************************************
* Function Name  : UART_ByteBufferSend
* Description    : Transmits data from a user defined buffer and returns the
*                  status of transmission.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - PtrToBuffer : A pointer to the data to send
*                  - NbOfBytes: The data length in bytes
* Output         : None
* Return         : None
*******************************************************************************/
void UART_ByteBufferSend(UART_TypeDef *UARTx, u8* PtrToBuffer, u8 NbOfBytes)
{
  if ((UARTx->CR &  UART_FIFO_Enable) == UART_FIFO_Enable)
  {
    /*FIFO is enabled*/
    while(NbOfBytes!=0)    
    {
      NbOfBytes--;
      /* while the UART_TxFIFO contain 16 characters.*/
      while((UARTx->SR & UART_Flag_TxFull) == UART_Flag_TxFull);
      UARTx->TxBUFR = *PtrToBuffer;
      PtrToBuffer++;
    }
  }
  else
  {
    /*FIFO is disabled*/
    while(NbOfBytes!=0 )   
    {
      NbOfBytes--;
      /* while the UART_TxFIFO contain one character.*/
      while ((UARTx->SR & UART_Flag_TxEmpty) != UART_Flag_TxEmpty);
      UARTx->TxBUFR = *PtrToBuffer;
      PtrToBuffer++;
    }
  }
}

/*******************************************************************************
* Function Name  : UART_9BitByteBufferSend
* Description    : Transmits 9 bits data from a user defined buffer and returns the 
*                  status of transmission.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - PtrToBuffer : A pointer to the data to send
*                  - NbOfBytes: The data length in bytes
* Output         : None
* Return         : None
*******************************************************************************/
void UART_9BitByteBufferSend(UART_TypeDef *UARTx, u16* PtrToBuffer, u8 NbOfBytes)
{
  if ((UARTx->CR &  UART_FIFO_Enable) == UART_FIFO_Enable)
  {
    /*FIFO is enable*/
    while(NbOfBytes!=0)    
    {
      NbOfBytes--;
      /* while the UART_TxFIFO contain 16 characters.*/
      while((UARTx->SR & UART_Flag_TxFull) == UART_Flag_TxFull)
      UARTx->TxBUFR = *PtrToBuffer;
      PtrToBuffer++;
    }
  }
  else
  {
    /*FIFO is disable*/
    while(NbOfBytes!=0 )   
    {
      NbOfBytes--;
      /* while the UART_TxFIFO contain one character.*/
      while((UARTx->SR & UART_Flag_TxEmpty) != UART_Flag_TxEmpty);
      UARTx->TxBUFR = *PtrToBuffer;
      PtrToBuffer++;
    }
  }
}

/*******************************************************************************
* Function Name  : UART_StringSend
* Description    : Transmits a string from a user defined string and returns the 
*                  status of transmission.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - PtrToString : A pointer to the sting to send
* Output         : None
* Return         : None
*******************************************************************************/
void UART_StringSend(UART_TypeDef *UARTx, u8 *PtrToString)
{
  if((UARTx->CR &  UART_FIFO_Enable)==UART_FIFO_Enable)
  {
    /*FIFO is enable*/
    while(*PtrToString != '\0')
    {
      /*while the UART_TxFIFO contains 16 characters */
      while((UARTx->SR & UART_Flag_TxFull) == UART_Flag_TxFull);
      UARTx->TxBUFR = *PtrToString;
      PtrToString++;
    }
    while((UARTx->SR & UART_Flag_TxFull) == UART_Flag_TxFull);
  }
  else
  {
    /*FIFO is disable*/
    while(*PtrToString != '\0')
    {
      /*while the UART_TxFIFO contain one character.*/
      while ((UARTx->SR & UART_Flag_TxEmpty) != UART_Flag_TxEmpty);
      UARTx->TxBUFR = *PtrToString;
      PtrToString++;
    }
    while ((UARTx->SR & UART_Flag_TxEmpty) != UART_Flag_TxEmpty);
  }
  
  UARTx->TxBUFR = '\0';
}

/*******************************************************************************
* Function Name  : UART_ByteBufferReceive
* Description    : Receives number of data words (Byte), stores them in user defined
*                  area and returns the status of the reception.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - PtrToBuffer : A pointer to the buffer where the data will be stored.
*                  - NbOfBytes : The data length.
* Output         : None
* Return         : An ErrorStatus enumuration value:
*                  - SUCCESS: reception done without error
*                  - ERROR: an error detected during reception
*******************************************************************************/
ErrorStatus UART_ByteBufferReceive(UART_TypeDef *UARTx, u8 *PtrToBuffer, u8 NbOfBytes)
{
  u16 BufFull, TimeOut =0;

  while(NbOfBytes!=0)
  {
    NbOfBytes--;
    /* while the UART_RxFIFO is empty and no Timeoutidle*/
    
    do
    {
      BufFull = UARTx->SR & UART_Flag_RxBufFull;
      TimeOut = UARTx->SR & UART_Flag_TimeOutIdle;
    } while ((BufFull == 0x0)& (TimeOut ==0x0));

    if((UARTx->SR & UART_Flag_TimeOutIdle) == UART_Flag_TimeOutIdle)
    {
      /*TimeOutIdle flag is set*/
      return ERROR;
    }
    else
    {
      /*Read the Receive Buffer Register*/
      *(PtrToBuffer)++ = (u8)UARTx->RxBUFR; 
    }
  }
  return SUCCESS;
}

/*******************************************************************************
* Function Name  : UART_9BitBufferReceive
* Description    : Receives number of data words (Byte), stores them in user defined
*                  area and returns the status of the reception.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - PtrToBuffer : A pointer to the buffer where the data will be stored.
*                  - NbOfBytes : The data length.
* Output         : None
* Return         : An ErrorStatus enumuration value:
*                  - SUCCESS: reception done without error
*                  - ERROR: an error detected during reception
*******************************************************************************/
ErrorStatus UART_9BitBufferReceive(UART_TypeDef *UARTx, u16 *PtrToBuffer, u8 NbOfBytes)
{
  u16 BufFull, TimeOut =0;

   while(NbOfBytes!=0)
  {
    NbOfBytes--;
    /* while the UART_RxFIFO is empty and no Timeoutidle*/
    do
    {
      BufFull = UARTx->SR & UART_Flag_RxBufFull;
      TimeOut = UARTx->SR & UART_Flag_TimeOutIdle;
    } while ((BufFull == 0x0)& (TimeOut ==0x0));

    if((UARTx->SR & UART_Flag_TimeOutIdle) == UART_Flag_TimeOutIdle)
    {
      /*TimeOutIdle flag is set*/
      return ERROR;
    }
    else
    {
      /*Read the Receive Buffer Register*/
      *(PtrToBuffer)++ = (u8)UARTx->RxBUFR; 
    }
  }
  
  return SUCCESS;
}

/*******************************************************************************
* Function Name  : UART_StringReceive
* Description    : Receives a string, stores it in a user defined
*                : area and returns the status of the reception.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - PtrToString : A pointer to the buffer where the string will
*                    be stored.
* Output         : None
* Return         : None
*******************************************************************************/
void UART_StringReceive(UART_TypeDef *UARTx, u8 *PtrToString)
{
  do
  {
    /* while the UART_RxFIFO is empty and no Timeoutidle*/
    while ((UARTx->SR & (UART_Flag_RxBufFull |UART_Flag_RxHalfFull)) == 0x0);    
    /* then read the RxBUFR*/
    *(PtrToString++) = (u8)UARTx->RxBUFR; 
   }while((*(PtrToString - 1)!=0x0D)&(*(PtrToString - 1)!='\0'));
}

/*******************************************************************************
* Function Name  : UART_SetTimeOutValue
* Description    : Configures the Time Out value.
* Input 1        : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - UART_TimeOut : The time-out period value.
* Output         : None
* Return         : None
*******************************************************************************/
void UART_SetTimeOutValue(UART_TypeDef *UARTx, u16 UART_TimeOut)
{
  UARTx->TOR = UART_TimeOut;
}

/*******************************************************************************
* Function Name  : UART_ByteReceive
* Description    : Returns the most recent received Byte by the UARTx peripheral.
* Input          : UARTx: where x can be 0..3 to select the UARTx peripheral.
* Output         : The received data
* Return         : The UARTx.SR register contents
*******************************************************************************/
u8 UART_ByteReceive(UART_TypeDef *UARTx)
{
  /*read the Receive Buffer Register*/
   return (u8)UARTx->RxBUFR; 
}

/*******************************************************************************
* Function Name  : UART_9BitReceive
* Description    : Returns the most recent received 9 bits by the UARTx peripheral.
* Input          : UARTx: where x can be 0..3 to select the UARTx peripheral.
* Output         : The received data
* Return         : The UARTx.SR register contents
*******************************************************************************/
u16 UART_9BitReceive(UART_TypeDef *UARTx)
{
  /*read the Receive Buffer Register*/
  return (u16)UARTx->RxBUFR; 
}

/*******************************************************************************
* Function Name  : UART_ByteSend
* Description    : Transmits signle Byte of data through the UARTx peripheral.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - data: The data byte to send
* Output         : None
* Return         : None
*******************************************************************************/
void UART_ByteSend(UART_TypeDef *UARTx, u8 data)
{
  UARTx->TxBUFR = data;
}

/*******************************************************************************
* Function Name  : UART_9BitSend
* Description    : Transmits 9 Bit of data through the UARTx peripheral.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - data :A pointer to the data to send
* Output         : None
* Return         : None
*******************************************************************************/
void UART_9BitSend(UART_TypeDef *UARTx, u16 data)
{
  UARTx->TxBUFR = data;
}

/*******************************************************************************
* Function Name  : UART_Cmd
* Description    : Enables or disables UART peripheral.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - NewState: specifies the new state of the UART peripheral.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void UART_Cmd(UART_TypeDef *UARTx, FunctionalState NewState)
{
 if (NewState == ENABLE)
  {
    /* Enable the selected UART by setting the RUN bit in the CR register*/
    UARTx->CR |= UART_RUN_Enable_Mask;
  }
  else
  {
    /* Disable the selected UART by clearing the RUN bit in the CR register*/
    UARTx->CR &= UART_RUN_Disable_Mask;
  }
}

/*******************************************************************************
* Function Name  : UART_FifoReset
* Description    : Resets the Rx and the Tx FIFOs of the selected UART.
* Input          : - UARTx: where x can be 0..3 to select the UARTx peripheral.
*                : - FIFO_Reset : clear the FIFo UART_RxFIFO or UART_TxFIFO
* Output         : None
* Return         : None
*******************************************************************************/
void UART_FifoReset(UART_TypeDef *UARTx, u16 FIFO_Reset)
{
 if (FIFO_Reset == UART_RxFIFO) 
  {
    /*Empties the RxFIFO*/  
    UARTx->RxRSTR = 0x5555; 
  } 
  else
  {
    /*Empties the TxFIFO*/  
    UARTx->TxRSTR = 0x5555;
  }
}
 
/*******************************************************************************
* Function Name  : UART_ITConfig 
* Description    : Enables or disables the specified UART interrupts.
* Input          : - UARTx: where x can be 0..3 to select the UARTx peripheral.
*                  - UART_IT: specifies the UART interrupts sources to be enabled
*                    or disabled. This parameter can be any combination of the
*                    following values:
*                         - UART_IT_TxFull 
*                         - UART_IT_RxHalfFull 
*                         - UART_IT_TimeOutIdle     
*                         - UART_IT_TimeOutNotEmpty 
*                         - UART_IT_OverrunError    
*                         - UART_IT_FrameError      
*                         - UART_IT_ParityError    
*                         - UART_IT_TxHalfEmpty    
*                         - UART_IT_TxEmpty         
*                         - UART_IT_RxBufFull   
*                  - NewState: new state of the specified UART interrupts.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/

void UART_ITConfig(UART_TypeDef *UARTx,u16 UART_IT, FunctionalState NewState)
{
  if(NewState == ENABLE)
  {
    UARTx->IER |= UART_IT;
  }
  else
  { 
    UARTx->IER &= ~UART_IT;
  }
}
/*******************************************************************************
* Function Name  : UART_FlagStatus
* Description    : Checks whether the specified UART flag is set or not.
* Input          : - UARTx: where x can be 0..3  to select the UARTx peripheral.
*                  - UART_Flag: flag to check. This parameter can be one of the
*                    following values:
*                         - UART_Flag_TxFull 
*                         - UART_Flag_RxHalfFull 
*                         - UART_Flag_TimeOutIdle     
*                         - UART_Flag_TimeOutNotEmpty 
*                         - UART_Flag_OverrunError    
*                         - UART_Flag_FrameError      
*                         - UART_Flag_ParityError    
*                         - UART_Flag_TxHalfEmpty    
*                         - UART_Flag_TxEmpty         
*                         - UART_Flag_RxBufFull   
* Output         : None
* Return         : the new state of the UART_Flag (SET or RESET).
*******************************************************************************/
FlagStatus UART_FlagStatus(UART_TypeDef *UARTx, u16 UART_Flag)
{
  /* Checks whether UART_Flag is set or not */
  if((UARTx->SR & UART_Flag) != RESET)
  { 
    return SET;
  }
  else
  {
    return RESET;
  } 
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
