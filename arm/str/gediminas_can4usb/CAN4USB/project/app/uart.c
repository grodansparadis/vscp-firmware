/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : uart.c
 *    Description : UARTs module
 *
 *    History :
 *    1. Date        : August 11, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
**************************************************************************/
#define UART_GLOBAL
#include "uart.h"

pUartFifo_t pUart0RxFifo; // Pointer to a FIFO Buffer of the UART0 Receive
pUartFifo_t pUart0TxFifo; // Pointer to a FIFO Buffer of the UART0 Transmit
pUartFifo_t pUart1RxFifo; // Pointer to a FIFO Buffer of the UART1 Receive
pUartFifo_t pUart1TxFifo; // Pointer to a FIFO Buffer of the UART1 Transmit
pUartFifo_t pUart2RxFifo; // Pointer to a FIFO Buffer of the UART2 Receive
pUartFifo_t pUart2TxFifo; // Pointer to a FIFO Buffer of the UART2 Transmit

// Hold UART0 Evens (PE, BI, FE, OE)
UartLineEvents_t Uart0LineEvents;
// Hold UART1 Evens (PE, BI, FE, OE)
UartLineEvents_t Uart1LineEvents;
// Hold UART2 Evens (PE, BI, FE, OE)
UartLineEvents_t Uart2LineEvents;

/*************************************************************************
 * Function Name: FifoPush
 * Parameters: pUartFifo_t Fifo, Int8U Data
 *
 * Return: Boolean
 *
 * Description: Push a char in a FIFO. Return TRUE when push is successful
 *  or FALSE when the FIFO is full.
 *
 *************************************************************************/
static Boolean FifoPush(pUartFifo_t Fifo, Int8U Data)
{
Int32U IndxTmp;

  // calculate next push index
  IndxTmp = Fifo->PushIndx + 1;
  IndxTmp = IndxTmp % UART_FIFO_SIZE;

  // Check FIFO state
  if (IndxTmp == Fifo->PopIndx)
  {
    // The FIFO is full
    return(FALSE);
  }
  // Push the data
  Fifo->Buffer[Fifo->PushIndx] = Data;
  // Updating the push's index
  Fifo->PushIndx = IndxTmp;
  return(TRUE);
}

/*************************************************************************
 * Function Name: FifoPop
 * Parameters: pUartFifo_t Fifo, Int8U Data
 *
 * Return: Boolean
 *
 * Description: Pop a char from a FIFO. Return TRUE when pop is successful
 *  or FALSE when the FIFO is empty.
 *
 *************************************************************************/
static Boolean FifoPop(pUartFifo_t Fifo, pInt8U pData)
{
Int32U IndxTmp;

  // Check FIFO state
  if (Fifo->PushIndx == Fifo->PopIndx)
  {
    // The FIFO is empty
    return(FALSE);
  }
  // Calculate the next pop index
  IndxTmp = Fifo->PopIndx + 1;
  IndxTmp = IndxTmp % UART_FIFO_SIZE;
  // Pop the data
  *pData = Fifo->Buffer[Fifo->PopIndx];
  // Updating of the pop's index
  Fifo->PopIndx = IndxTmp;
  return(TRUE);
}


/*************************************************************************
 * Function Name: Uart0Isr
 * Parameters: none
 *
 * Return: none
 *
 * Description: UART 0 interrupt routine
 *
 *************************************************************************/
void Uart0Isr(void)
{
Int32U  UartInt, LineStatus, Counter;
Int8U Data;
  // Recognize the interrupt event
  UartInt = UART0->MIS;
  if(UartInt & 0x07D0)
  {
    UART_ClearITPendingBit(UART0,(UartInt & 0x07D0));
    // Read the line state of the UART
    LineStatus = UART0->RSR;
    if(LineStatus & RLS_OverrunError)
    {
      // Overrun Error
      Uart0LineEvents.bOE = TRUE;
    }
    Data = UART0->DR;
    if (LineStatus & RLS_BreakInterruptr)
    {
      // Break Indicator
      Uart0LineEvents.bBI = TRUE;
    }
    else if (LineStatus & RLS_FramingError)
    {
      // Framing Error
      Uart0LineEvents.bFE = TRUE;
    }
    else if (LineStatus & RLS_ParityError)
    {
      // Parity Error
      Uart0LineEvents.bPE = TRUE;
    }
    // Push a new data into the receiver buffer
    if(!FifoPush(pUart0RxFifo,Data))
    {
      // the FIFO is full
      Uart0LineEvents.bOE = TRUE;
    }
    // Read the line state of the UART
    LineStatus = UART0->RSR;
  }
  if(UartInt & 0x0020)
  {
    UART_ClearITPendingBit(UART0,0x0020);
    Counter = 1;
    if(UART0->LCR & 1UL << 4)
    {
      switch (UART0->IFLS & 0x7)
      {
      case 0:
        Counter = 7;
        break;
      case 1:
        Counter = 5;
        break;
      case 2:
        Counter = 3;
        break;
      }
    }
    for( ;Counter; --Counter)
    {
      // Pop a data from the transmit buffer
      if(!FifoPop(pUart0TxFifo,&Data))
      {
        // The tx software FIFO is empty
        break;
      }
      UART0->DR = Data;
    }
  }
}

/*************************************************************************
 * Function Name: Uart1Isr
 * Parameters: none
 *
 * Return: none
 *
 * Description: UART 1 interrupt routine
 *
 *************************************************************************/
void Uart1Isr(void)
{
Int32U  UartInt, LineStatus, Counter;
Int8U Data;
  // Recognize the interrupt event
  UartInt = UART1->MIS;
  if(UartInt & 0x07D0)
  {
    UART_ClearITPendingBit(UART1,(UartInt & 0x07D0));
    // Read the line state of the UART
    LineStatus = UART1->RSR;
    if(LineStatus & RLS_OverrunError)
    {
      // Overrun Error
      Uart1LineEvents.bOE = TRUE;
    }
    Data = UART1->DR;
    if (LineStatus & RLS_BreakInterruptr)
    {
      // Break Indicator
      Uart1LineEvents.bBI = TRUE;
    }
    else if (LineStatus & RLS_FramingError)
    {
      // Framing Error
      Uart1LineEvents.bFE = TRUE;
    }
    else if (LineStatus & RLS_ParityError)
    {
      // Parity Error
      Uart1LineEvents.bPE = TRUE;
    }
    // Push a new data into the receiver buffer
    if(!FifoPush(pUart1RxFifo,Data))
    {
      // the FIFO is full
      Uart1LineEvents.bOE = TRUE;
    }
    // Read the line state of the UART
    LineStatus = UART1->RSR;
  }
  if(UartInt & 0x0020)
  {
    UART_ClearITPendingBit(UART1,0x0020);
    Counter = 1;
    if(UART1->LCR & 1UL << 4)
    {
      switch (UART1->IFLS & 0x7)
      {
      case 0:
        Counter = 7;
        break;
      case 1:
        Counter = 5;
        break;
      case 2:
        Counter = 3;
        break;
      }
    }
    for( ;Counter; --Counter)
    {
      // Pop a data from the transmit buffer
      if(!FifoPop(pUart1TxFifo,&Data))
      {
        // The tx software FIFO is empty
        break;
      }
      UART1->DR = Data;
    }
  }
}

/*************************************************************************
 * Function Name: Uart2Isr
 * Parameters: none
 *
 * Return: none
 *
 * Description: UART 2 interrupt routine
 *
 *************************************************************************/
void Uart2Isr(void)
{
Int32U  UartInt, LineStatus, Counter;
Int8U Data;
  // Recognize the interrupt event
  UartInt = UART2->MIS;
  if(UartInt & 0x07D0)
  {
    UART_ClearITPendingBit(UART2,(UartInt & 0x07D0));
    // Read the line state of the UART
    LineStatus = UART2->RSR;
    if(LineStatus & RLS_OverrunError)
    {
      // Overrun Error
      Uart2LineEvents.bOE = TRUE;
    }
    Data = UART2->DR;
    if (LineStatus & RLS_BreakInterruptr)
    {
      // Break Indicator
      Uart2LineEvents.bBI = TRUE;
    }
    else if (LineStatus & RLS_FramingError)
    {
      // Framing Error
      Uart2LineEvents.bFE = TRUE;
    }
    else if (LineStatus & RLS_ParityError)
    {
      // Parity Error
      Uart2LineEvents.bPE = TRUE;
    }
    // Push a new data into the receiver buffer
    if(!FifoPush(pUart2RxFifo,Data))
    {
      // the FIFO is full
      Uart2LineEvents.bOE = TRUE;
    }
    // Read the line state of the UART
    LineStatus = UART2->RSR;
  }
  if(UartInt & 0x0020)
  {
    UART_ClearITPendingBit(UART2,0x0020);
    Counter = 1;
    if(UART2->LCR & 1UL << 4)
    {
      switch (UART2->IFLS & 0x7)
      {
      case 0:
        Counter = 7;
        break;
      case 1:
        Counter = 5;
        break;
      case 2:
        Counter = 3;
        break;
      }
    }
    for( ;Counter; --Counter)
    {
      // Pop a data from the transmit buffer
      if(!FifoPop(pUart2TxFifo,&Data))
      {
        // The tx software FIFO is empty
        break;
      }
      UART2->DR = Data;
    }
  }
}

/*************************************************************************
 * Function Name: UartInit
 * Parameters: Int32U IrqSlot
 *
 * Return: Boolean
 *
 * Description: Init UART
 *
 *************************************************************************/
Boolean UartInit(UartNum_t Uart,Int32U IrqSlot)
{
GPIO_InitTypeDef  GPIO_InitStructure;
UART_InitTypeDef  UART_InitStructure;
EIC_IRQInitTypeDef EIC_IRQInitStructure;

volatile Int8U Tmp;

  // Enable GPIO clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO, ENABLE);
  // Release GPIO reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_GPIO,DISABLE);

  UART_StructInit(&UART_InitStructure);
  UART_InitStructure.UART_Parity = UART_Parity_No;

  switch(Uart)
  {
  case UART_0:
    pUart0RxFifo = (pUartFifo_t)malloc(sizeof(UartFifo_t));
    if(pUart0RxFifo == NULL)
    {
      return(FALSE);
    }
    pUart0TxFifo = (pUartFifo_t)malloc(sizeof(UartFifo_t));
    if(pUart0TxFifo == NULL)
    {
      free(pUart0RxFifo);
      return(FALSE);
    }
    // Init receive and transmit FIFOs
    pUart0RxFifo->PopIndx = pUart0RxFifo->PushIndx = \
    pUart0TxFifo->PopIndx = pUart0TxFifo->PushIndx = 0;

    // Release reset and enable clock
    UART_DeInit(UART0);
    MRCC_PeripheralClockConfig(MRCC_Peripheral_UART0, ENABLE);

    // Assign GPIO 0.11 to UART0 (Tx)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = 1UL<<11;
    GPIO_Init(GPIO0, &GPIO_InitStructure);
    // Assign GPIO 0.10 to UART0 (Rx)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = 1UL<<10;
    GPIO_Init(GPIO0, &GPIO_InitStructure);

    // Init UART0
    UART_Init(UART0,&UART_InitStructure);

    // Enable and configure the priority of the UART0 Update IRQ Channel
    EIC_IRQInitStructure.EIC_IRQChannel = UART0_IRQChannel;
    EIC_IRQInitStructure.EIC_IRQChannelPriority = IrqSlot;
    EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
    EIC_IRQInit(&EIC_IRQInitStructure);

    // Enable UART0 interrupts
    UART_ITConfig(UART0, UART_IT_OverrunError   |
                         UART_IT_BreakError     |
                         UART_IT_ParityError    |
                         UART_IT_FrameError     |
                         UART_IT_ReceiveTimeOut |
                         UART_IT_Transmit       |
                         UART_IT_Receive        , ENABLE);

    // Enable the UART0
    UART_Cmd(UART0, ENABLE);
    break;
  case UART_1:
    pUart1RxFifo = (pUartFifo_t)malloc(sizeof(UartFifo_t));
    if(pUart1RxFifo == NULL)
    {
      return(FALSE);
    }
    pUart1TxFifo = (pUartFifo_t)malloc(sizeof(UartFifo_t));
    if(pUart1TxFifo == NULL)
    {
      free(pUart1RxFifo);
      return(FALSE);
    }
    // Init receive and transmit FIFOs
    pUart1RxFifo->PopIndx = pUart1RxFifo->PushIndx = \
    pUart1TxFifo->PopIndx = pUart1TxFifo->PushIndx = 0;

    // Release reset and enable clock
    UART_DeInit(UART1);
    MRCC_PeripheralClockConfig(MRCC_Peripheral_UART1, ENABLE);

    // Assign GPIO 0.21 to UART1 (Tx)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = 1UL<<21;
    GPIO_Init(GPIO0, &GPIO_InitStructure);
    // Assign GPIO 0.20 to UART1 (Rx)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = 1UL<<20;
    GPIO_Init(GPIO0, &GPIO_InitStructure);

    // Init UART1
    UART_Init(UART1,&UART_InitStructure);

    // Enable and configure the priority of the UART1 Update IRQ Channel
    EIC_IRQInitStructure.EIC_IRQChannel = UART1_IRQChannel;
    EIC_IRQInitStructure.EIC_IRQChannelPriority = IrqSlot;
    EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
    EIC_IRQInit(&EIC_IRQInitStructure);

    // Enable UART1 interrupts
    UART_ITConfig(UART1, UART_IT_OverrunError   |
                         UART_IT_BreakError     |
                         UART_IT_ParityError    |
                         UART_IT_FrameError     |
                         UART_IT_ReceiveTimeOut |
                         UART_IT_Transmit       |
                         UART_IT_Receive        , ENABLE);

    // Enable the UART1
    UART_Cmd(UART1, ENABLE);
    break;
  case UART_2:
    pUart2RxFifo = (pUartFifo_t)malloc(sizeof(UartFifo_t));
    if(pUart2RxFifo == NULL)
    {
      return(FALSE);
    }
    pUart2TxFifo = (pUartFifo_t)malloc(sizeof(UartFifo_t));
    if(pUart2TxFifo == NULL)
    {
      free(pUart2RxFifo);
      return(FALSE);
    }
    // Init receive and transmit FIFOs
    pUart2RxFifo->PopIndx = pUart2RxFifo->PushIndx = \
    pUart2TxFifo->PopIndx = pUart2TxFifo->PushIndx = 0;

    // Release reset and enable clock
    UART_DeInit(UART2);
    MRCC_PeripheralClockConfig(MRCC_Peripheral_UART2, ENABLE);

    // Assign GPIO 0.25 to UART2 (Tx)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = 1UL<<25;
    GPIO_Init(GPIO0, &GPIO_InitStructure);
    // Assign GPIO 0.24 to UART2 (Rx)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = 1UL<<24;
    GPIO_Init(GPIO0, &GPIO_InitStructure);

    // Init UART2
    UART_Init(UART2,&UART_InitStructure);

    // Enable and configure the priority of the UART2 Update IRQ Channel
    EIC_IRQInitStructure.EIC_IRQChannel = UART2_IRQChannel;
    EIC_IRQInitStructure.EIC_IRQChannelPriority = IrqSlot;
    EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
    EIC_IRQInit(&EIC_IRQInitStructure);

    // Enable UART2 interrupts
    UART_ITConfig(UART2, UART_IT_OverrunError   |
                         UART_IT_BreakError     |
                         UART_IT_ParityError    |
                         UART_IT_FrameError     |
                         UART_IT_ReceiveTimeOut |
                         UART_IT_Transmit       |
                         UART_IT_Receive        , ENABLE);

    // Enable the UART2/
    UART_Cmd(UART2, ENABLE);
  default:
    return(FALSE);
  }
  return(TRUE);
}

/*************************************************************************
 * Function Name: UartSetLineCoding
 * Parameters:  UartNum_t Uart,UART_InitTypeDef pUartCoding
 *
 * Return: None
 *
 * Description: Init UART Baud rate, Word width, Stop bits, Parity type
 *
 *************************************************************************/
void UartSetLineCoding(UartNum_t Uart,UART_InitTypeDef UartCoding)
{
UART_InitTypeDef  UART_InitStructure;
UART_TypeDef * pUart;

  UART_StructInit(&UART_InitStructure);

  switch(Uart)
  {
  case UART_0:
    pUart = UART0;
    break;
  case UART_1:
    pUart = UART1;
    break;
  case UART_2:
    pUart = UART2;
    break;
  default:
    return;
  }

  UART_InitStructure.UART_WordLength = UartCoding.UART_WordLength;
  UART_InitStructure.UART_StopBits   = UartCoding.UART_StopBits;
  UART_InitStructure.UART_Parity     = UartCoding.UART_Parity;
  UART_InitStructure.UART_BaudRate   = UartCoding.UART_BaudRate;

  UART_Init(pUart,&UART_InitStructure);

}

/*************************************************************************
 * Function Name: UartRead
 * Parameters:  UartNum_t Uart, pInt8U pBuffer, Int32U BufferSize
 *
 * Return: Int32U
 *
 * Description: Read received data from UART.
 *              Return number of readied characters
 *
 *************************************************************************/
Int32U UartRead(UartNum_t Uart, pInt8U pBuffer, Int32U BufferSize)
{
Int32U Count;
pUartFifo_t pUartFifo;

  switch(Uart)
  {
  case UART_0:
    pUartFifo = pUart0RxFifo;
    break;
  case UART_1:
    pUartFifo = pUart1RxFifo;
    break;
  case UART_2:
    pUartFifo = pUart2RxFifo;
    break;
  default:
    return(0);
  }

  for (Count = 0; Count < BufferSize; ++Count)
  {
    if(!FifoPop(pUartFifo,pBuffer+Count))
    {
      break;
    }
  }
  return(Count);
}

/*************************************************************************
 * Function Name: UartWrite
 * Parameters:  UartNum_t Uart, pInt8U pBuffer, Int32U BufferSize
 *
 * Return: Int32U
 *
 * Description: Write a data to UART. Return number of successful
 *  transmitted bytes
 *
 *************************************************************************/
Int32U UartWrite(UartNum_t Uart, pInt8U pBuffer, Int32U BufferSize)
{
Int32U Count = 0;
Int8U Data;
pUartFifo_t pUartFifo;
UART_TypeDef * pUart;
FunctionalState cpu_sr;

  switch(Uart)
  {
  case UART_0:
    pUart = UART0;
    pUartFifo = pUart0TxFifo;
    break;
  case UART_1:
    pUart = UART1;
    pUartFifo = pUart1TxFifo;
    break;
  case UART_2:
    pUart = UART2;
    pUartFifo = pUart2TxFifo;
    break;
  default:
    return(0);
  }

  if(BufferSize != 0)
  {
    for ( ; Count < BufferSize; ++Count)
    {
      if(!FifoPush(pUartFifo,*(pBuffer+Count)))
      {
        break;
      }
    }

    if(UART_GetFlagStatus(pUart,UART_FLAG_TxFIFOFull) != SET)
    {
      do
      {
        EIC_IRQDisCmd(cpu_sr);
        if(!FifoPop(pUartFifo,&Data))
        {
          EIC_IRQRestoreCmd(cpu_sr);
          // The tx software FIFO is empty
          break;
        }
        UART_SendData(pUart,Data);
        EIC_IRQRestoreCmd(cpu_sr);
      }
      while(UART_GetFlagStatus(pUart,UART_FLAG_TxFIFOFull) != SET);
    }

  }
  return(Count);
}

/*************************************************************************
 * Function Name: UartGetUartEvents
 * Parameters:  UartNum_t Uart
 *
 * Return: UartLineEvents_t
 *
 * Description: Get Uart Line events (PE,OE, FE, BI)
 *
 *************************************************************************/
UartLineEvents_t UartGetUartLineEvents (UartNum_t Uart)

{
UartLineEvents_t  LineEvents;
  LineEvents.Data = 0;
  switch(Uart)
  {
  case UART_0:
    LineEvents.Data = AtomicExchange(LineEvents.Data,
                                     &Uart0LineEvents.Data);
    break;
  case UART_1:
    LineEvents.Data = AtomicExchange(LineEvents.Data,
                                     &Uart1LineEvents.Data);
    break;
  case UART_2:
    LineEvents.Data = AtomicExchange(LineEvents.Data,
                                     &Uart2LineEvents.Data);
    break;
  }
  return(LineEvents);
}

/*************************************************************************
 * Function Name: UartSetUartLineState
 * Parameters:  UartNum_t Uart UartNum_t Uart, Boolean Break
 *
 * Return: none
 *
 * Description: Set Uart Break Event
 *
 *************************************************************************/
void UartSetUartLineState (UartNum_t Uart, Boolean Break)
{
UART_TypeDef * pUart;

  if(Break == FALSE)
  {
    return;
  }

  switch(Uart)
  {
  case UART_0:
    pUart = UART0;
    break;
  case UART_1:
    pUart = UART1;
    break;
  case UART_2:
    pUart = UART2;
    break;
  default:
    return;
  }
  UART_SendBreak(pUart);
}

