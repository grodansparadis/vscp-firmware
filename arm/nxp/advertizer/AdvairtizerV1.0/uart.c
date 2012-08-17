/******************************************************************************
 *
 * $RCSfile: $
 * $Revision: $
 *
 * This module provides interface routines to the LPC ARM UARTs.
 * Copyright 2004, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 * uses VIC-Vector 0 if UART0 is in "INT_MODE"
 * uses VIC-Vector 1 if UART1 is in "INT_MODE"
 *
 *****************************************************************************/
#include <global.h>

void enable_RS232(void){
 F_IO1CLR = RX232_ENABLE_BIT;
}
void disable_RS232(void){
 F_IO1SET = RX232_ENABLE_BIT;
}
void enable_RS485(void){
 F_IO1CLR = RX485_ENABLE_BIT;
}
void disable_RS485(void){
  F_IO1SET = RX485_ENABLE_BIT;
}

void init_uart(void){
  uart0Init(B9600, UART_8N1, UART_FIFO_8); // setup the UART , ist fest auf xxx
  disable_RS232();
  disable_RS485();

}

#if defined(UART0_TX_INT_MODE) || defined(UART0_RX_INT_MODE) || \
    defined(UART1_TX_INT_MODE) || defined(UART1_RX_INT_MODE)
#endif

#if UART0_SUPPORT
 #ifdef UART0_RX_INT_MODE
  char  uart0_rx_buffer[UART0_RX_BUFFER_SIZE];
  U16 uart0_rx_insert_idx, uart0_rx_extract_idx;
 #endif

 #ifdef UART0_TX_INT_MODE
  U8  uart0_tx_buffer[UART0_TX_BUFFER_SIZE];
  U16 uart0_tx_insert_idx, uart0_tx_extract_idx;
  int uart0_tx_running;
 #endif
#endif

#if UART1_SUPPORT
#ifdef UART1_RX_INT_MODE
U8  uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
U16 uart1_rx_insert_idx, uart1_rx_extract_idx;
#endif

#ifdef UART1_TX_INT_MODE
U8  uart1_tx_buffer[UART1_TX_BUFFER_SIZE];
U16 uart1_tx_insert_idx, uart1_tx_extract_idx;
int uart1_tx_running;
#endif
#endif

#if UART0_SUPPORT

/******************************************************************************
 *
 * Function Name: uart0Init()
 *
 * Description:  
 *    This function initializes the UART for async mode
 *
 * Calling Sequence: 
 *    baudrate divisor - use UART_BAUD macro
 *    mode - see typical modes (uart.h)
 *    fmode - see typical fmodes (uart.h)
 *
 * Returns:
 *    void
 *
 * NOTE: uart0Init(UART_BAUD(9600), UART_8N1, UART_FIFO_8);
 *
 *****************************************************************************/
void uart0Init(U16 baud, U8 mode, U8 fmode)
{
  // set port pins for UART0
  PINSEL0 = (PINSEL0 & ~U0_PINMASK) | U0_PINSEL;

  UART0_IER = 0x00;                         // disable all interrupts
  UART0_IIR;                                // clear interrupt ID
  UART0_RBR;                                // clear receive register
  UART0_LSR;                                // clear line status register

UART0_LCR = ULCR_DLAB_ENABLE;        // select divisor latches 
UART0_DLL = 24;//6;
UART0_DLM = 0;
// This Register controls the clock pre-scaler for the baud rate generation. The reset value of
// the register keeps the fractional capabilities of UART1 disabled making sure that UART1 is
// fully software and hardware compatible with UARTs not equipped with this feature.
UART0_FDR = 14<<4|5;//7<<4|12; // MulVal[7..4] DivAddVal[3..0]

  // set the number of characters and other
  // user specified operating parameters
  UART0_LCR = (mode & ~ULCR_DLAB_ENABLE);
  UART0_FCR = fmode;

  UART0_ACR = 0x00;//Autobaud off

#if defined(UART0_TX_INT_MODE) || defined(UART0_RX_INT_MODE)
    ctl_set_isr(SIR_UART0, PRIO_UART0, CTL_ISR_TRIGGER_FIXED, uart0ISR, 0);
    ctl_unmask_isr(SIR_UART0);

#ifdef UART0_TX_INT_MODE
  // initialize the transmit data queue
  uart0_tx_extract_idx = uart0_tx_insert_idx = 0;
  uart0_tx_running = 0;
#endif

#ifdef UART0_RX_INT_MODE
  // initialize the receive data queue
  uart0_rx_extract_idx = uart0_rx_insert_idx = 0;

  // enable receiver interrupts
  UART0_IER = UIER_ERBFI;
#endif
#endif

}
/*
/**********************************************************************
 * uart0 Interrupt
 * The Interrupt Service Routine (ISR)
***********************************************************************/
static void uart0ISR(void){
  ctl_global_interrupts_re_enable_from_isr();

if (UART0_LSR & ULSR_RDR){ // Receive Data Ready
// case ULSR_RDR interrupt - Receive Data Available interrupt
// Data is available, go read it.
        do{
          U16 temp;
          // calc next insert index & store character
          temp = (uart0_rx_insert_idx + 1) % UART0_RX_BUFFER_SIZE;
          uart0_rx_buffer[uart0_rx_insert_idx] = UART0_RBR;
          if (UART0_LOCAL_ECHO){
            uart0Putch(uart0_rx_buffer[uart0_rx_insert_idx]);//echo
          }
          // check for more room in queue
          if (temp != uart0_rx_extract_idx)
            uart0_rx_insert_idx = temp; // update insert index
          }
        while (UART0_LSR & ULSR_RDR);
}

  VICSoftIntClear = SIR_UART0; //clears the uart0 interrupt.
  ctl_global_interrupts_un_re_enable_from_isr();
}
/******************************************************************************
 *
 * Function Name: uart0Putch()
 *
 * Description:  
 *    This function puts a character into the UART output queue for
 *    transmission.
 *
 * Calling Sequence: 
 *    character to be transmitted
 *
 * Returns:
 *    ch on success, -1 on error (queue full)
 *
 *****************************************************************************/
int uart0Putch(int ch)
{
#ifdef UART0_TX_INT_MODE
  U16 temp;
  unsigned cpsr;

  temp = (uart0_tx_insert_idx + 1) % UART0_TX_BUFFER_SIZE;

  if (temp == uart0_tx_extract_idx)
    return -1;                          // no room

  cpsr = disableIRQ();                  // disable global interrupts
  UART0_IER &= ~UIER_ETBEI;             // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts

  // check if in process of sending data
  if (uart0_tx_running)
    {
    // add to queue
    uart0_tx_buffer[uart0_tx_insert_idx] = (U8)ch;
    uart0_tx_insert_idx = temp;
    }
  else
    {
    // set running flag and write to output register
    uart0_tx_running = 1;
    UART0_THR = (U8)ch;
    }

  cpsr = disableIRQ();                  // disable global interrupts
  UART0_IER |= UIER_ETBEI;              // enable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
#else
  while (!(UART0_LSR & ULSR_THRE))      // wait for TX buffer to empty
    continue;                           // also either WDOG() or swap()

  UART0_THR = (U8)ch;
#endif
  return (U8)ch;
}

/******************************************************************************
 *
 * Function Name: uart0Space()
 *
 * Description:  
 *    This function gets the available space in the transmit queue
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    available space in the transmit queue
 *
 *****************************************************************************/
U16 uart0Space(void)
{
#ifdef UART0_TX_INT_MODE
  int space;

  if ((space = (uart0_tx_extract_idx - uart0_tx_insert_idx)) <= 0)
    space += UART0_TX_BUFFER_SIZE;

  return (U16)(space - 1);
#else
  return USHRT_MAX;
#endif
}

/******************************************************************************
 *
 * Function Name: uart0Puts()
 *
 * Description:  
 *    This function writes a NULL terminated 'string' to the UART output
 *    queue, returning a pointer to the next character to be written.
 *
 * Calling Sequence: 
 *    address of the string
 *
 * Returns:
 *    a pointer to the next character to be written
 *    (\0 if full string is written)
 *
 *****************************************************************************/
const char *uart0Puts(const char *string)
{
  register char ch;

  while ((ch = *string) && (uart0Putch(ch) >= 0))
    string++;

  return string;
}

/******************************************************************************
 *
 * Function Name: uart0Write()
 *
 * Description:  
 *    This function writes 'count' characters from 'buffer' to the UART
 *    output queue.
 *
 * Calling Sequence: 
 *    
 *
 * Returns:
 *    0 on success, -1 if insufficient room, -2 on error
 *    NOTE: if insufficient room, no characters are written.
 *
 *****************************************************************************/
int uart0Write(const char *buffer, U16 count)
{
#ifdef UART0_TX_INT_MODE
  if (count > uart0Space())
    return -1;
#endif
  while (count && (uart0Putch(*buffer++) >= 0))
    count--;

  return (count ? -2 : 0);
}

/******************************************************************************
 *
 * Function Name: uart0TxEmpty()
 *
 * Description:
 *    This function returns the status of the UART transmit data
 *    registers.
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    FALSE - either the tx holding or shift register is not empty
 *   !FALSE - if both the tx holding & shift registers are empty
 *
 *****************************************************************************/
int uart0TxEmpty(void)
{
  return (UART0_LSR & (ULSR_THRE | ULSR_TEMT)) == (ULSR_THRE | ULSR_TEMT);
}

/******************************************************************************
 *
 * Function Name: uart0TxFlush()
 *
 * Description:  
 *    This function removes all characters from the UART transmit queue
 *    (without transmitting them).
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
void uart0TxFlush(void)
{
#ifdef UART0_TX_INT_MODE
  unsigned cpsr;

  UART0_FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo

  // "Empty" the transmit buffer.
//  int en=ctl_global_interrupts_disable();
  cpsr = disableIRQ();                  // disable global interrupts
  UART0_IER &= ~UIER_ETBEI;                 // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
//  ctl_set_interrupts(en);

  uart0_tx_insert_idx = uart0_tx_extract_idx = 0;
#else
  UART0_FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo
#endif
}

/******************************************************************************
 *
 * Function Name: uart0RxFlush()
 *
 * Description:  
 *    This function removes all characters from the UART receive queue
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
void uart0RxFlush(void){

  UART0_FCR |= UFCR_RX_FIFO_RESET;          // clear the RX fifo
  U8 i=0;
  // "Empty" the receive buffer.
  while(i < UART0_RX_BUFFER_SIZE){
    uart0_rx_buffer[i]=0xff;
    i++;
  }
//UART0_RX_BUFFER_SIZE
  uart0_rx_insert_idx = uart0_rx_extract_idx = 0;
}

#endif


#if UART1_SUPPORT
/******************************************************************************
 *
 * Function Name: uart1Init()
 *
 * Description:  
 *    This function initializes the UART for async mode
 *
 * Calling Sequence: 
 *    baudrate divisor - use UART_BAUD macro
 *    mode - see typical modes (uart.h)
 *    fmode - see typical fmodes (uart.h)
 *
 * Returns:
 *    void
 *
 * NOTE: uart1Init(UART_BAUD(9600), UART_8N1, UART_FIFO_8);
 *
 *****************************************************************************/
void uart1Init(U16 baud, U8 mode, U8 fmode)
{
  // set port pins for UART1
  PINSEL0 = (PINSEL0 & ~U1_PINMASK) | U1_PINSEL;

  U1IER = 0x00;                         // disable all interrupts
  U1IIR;                                // clear interrupt ID
  U1RBR;                                // clear receive register
  U1LSR;                                // clear line status register

  // set the baudrate
  U1LCR = ULCR_DLAB_ENABLE;             // select divisor latches 
  U1DLL = (U8)baud;                // set for baud low byte
  U1DLM = (U8)(baud >> 8);         // set for baud high byte

  // set the number of characters and other
  // user specified operating parameters
  U1LCR = (mode & ~ULCR_DLAB_ENABLE);
  U1FCR = fmode;

#if defined(UART1_TX_INT_MODE) || defined(UART1_RX_INT_MODE)
  // initialize the interrupt vector
  VICIntSelect &= ~VIC_BIT(VIC_UART1);  // UART1 selected as IRQ
  VICIntEnable = VIC_BIT(VIC_UART1);    // UART1 interrupt enabled
  VICVectCntl1 = VIC_ENABLE | VIC_UART1;
  VICVectAddr1 = (U32)uart1ISR;    // address of the ISR

#ifdef UART1_TX_INT_MODE
  uart1_tx_extract_idx = uart1_tx_insert_idx = 0;
  uart1_tx_running = 0;
#endif

#ifdef UART1_RX_INT_MODE
  // initialize data queues
  uart1_rx_extract_idx = uart1_rx_insert_idx = 0;

  // enable receiver interrupts
  U1IER |= UIER_ERBFI;
#endif
#endif
}

/******************************************************************************
 *
 * Function Name: uart1Putch()
 *
 * Description:  
 *    This function puts a character into the UART output queue for
 *    transmission.
 *
 * Calling Sequence: 
 *    ch - character to be transmitted
 *
 * Returns:
 *    ch on success, -1 on error (queue full)
 *
 *****************************************************************************/
int uart1Putch(int ch)
{
#ifdef UART1_TX_INT_MODE
  U16 temp;
  unsigned cpsr;

  temp = (uart1_tx_insert_idx + 1) % UART1_TX_BUFFER_SIZE;

  if (temp == uart1_tx_extract_idx)
    return -1;                          // no room

  cpsr = disableIRQ();                  // disable global interrupts
  U1IER &= ~UIER_ETBEI;                 // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts

  // check if in process of sending data
  if (uart1_tx_running)
    {
    // add to queue
    uart1_tx_buffer[uart1_tx_insert_idx] = (U8)ch;
    uart1_tx_insert_idx = temp;
    }
  else
    {
    // set running flag and write to output register
    uart1_tx_running = 1;
    U1THR = (U8)ch;
    }

  cpsr = disableIRQ();                  // disable global interrupts
  U1IER |= UIER_ETBEI;                  // enable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
#else
  while (!(U1LSR & ULSR_THRE))          // wait for TX buffer to empty
    continue;                           // also either WDOG() or swap()

  U1THR = (U8)ch;
#endif
  return (U8)ch;
}

/******************************************************************************
 *
 * Function Name: uart1Space()
 *
 * Description:  
 *    This function gets the available space in the transmit queue
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    available space in the transmit queue
 *
 *****************************************************************************/
U16 uart1Space(void)
{
#ifdef UART1_TX_INT_MODE
  int space;

  if ((space = (uart1_tx_extract_idx - uart1_tx_insert_idx)) <= 0)
    space += UART1_TX_BUFFER_SIZE;

  return (U16)(space - 1);
#else
  return USHRT_MAX;
#endif
}

/******************************************************************************
 *
 * Function Name: uart1Puts()
 *
 * Description:  
 *    This function writes a NULL terminated 'string' to the UART output
 *    queue, returning a pointer to the next character to be written.
 *
 * Calling Sequence: 
 *    address of the string
 *
 * Returns:
 *    a pointer to the next character to be written
 *    (\0 if full string is written)
 *
 *****************************************************************************/
const char *uart1Puts(const char *string)
{
  register char ch;

  while ((ch = *string) && (uart1Putch(ch) >= 0))
    string++;

  return string;
}

/******************************************************************************
 *
 * Function Name: uart1Write()
 *
 * Description:  
 *    This function writes 'count' characters from 'buffer' to the UART
 *    output queue.
 *
 * Calling Sequence: 
 *    
 *
 * Returns:
 *    0 on success, -1 if insufficient room, -2 on error
 *    NOTE: if insufficient room, no characters are written.
 *
 *****************************************************************************/
int uart1Write(const char *buffer, U16 count)
{
#ifdef UART1_TX_INT_MODE
  if (count > uart1Space())
    return -1;
#endif
  while (count && (uart1Putch(*buffer++) >= 0))
    count--;

  return (count ? -2 : 0);
}

/******************************************************************************
 *
 * Function Name: uart1TxEmpty()
 *
 * Description:
 *    This function returns the status of the UART transmit data
 *    registers.
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    FALSE - either the tx holding or shift register is not empty
 *   !FALSE - if both the tx holding & shift registers are empty
 *
 *****************************************************************************/
int uart1TxEmpty(void)
{
  return (U1LSR & (ULSR_THRE | ULSR_TEMT)) == (ULSR_THRE | ULSR_TEMT);
}

/******************************************************************************
 *
 * Function Name: uart1TxFlush()
 *
 * Description:  
 *    This function removes all characters from the UART transmit queue
 *    (without transmitting them).
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
void uart1TxFlush(void)
{
#ifdef UART1_TX_INT_MODE
  unsigned cpsr;

  U1FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo

  cpsr = disableIRQ();                  // disable global interrupts
  U1IER &= ~UIER_ETBEI;                 // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
  uart1_tx_insert_idx = uart1_tx_extract_idx = 0;
#else
  U1FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo
#endif
}

/******************************************************************************
 *
 * Function Name: uart1Getch()
 *
 * Description:  
 *    This function gets a character from the UART receive queue
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    character on success, -1 if no character is available
 *
 *****************************************************************************/
int uart1Getch(void)
{
#ifdef UART1_RX_INT_MODE
  U8 ch;

  if (uart1_rx_insert_idx == uart1_rx_extract_idx) // check if character is available
    return -1;

  ch = uart1_rx_buffer[uart1_rx_extract_idx++]; // get character, bump pointer
  uart1_rx_extract_idx %= UART1_RX_BUFFER_SIZE; // limit the pointer
  return ch;
#else
  if (U1LSR & ULSR_RDR)                 // check if character is available
    return U1RBR;                       // return character
  return -1;
#endif
}

#endif
