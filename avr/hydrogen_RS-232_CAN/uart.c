/******************************************************************************
 *
 * $Workfile: uart.c $
 *
 * This module provides interface routines to the AVR uart
 * Copyright 2001, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 * $History: uart.c $
 * 
 *****************************************************************************/
#include <stddef.h>
#include <limits.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <compat/deprecated.h>
#include "uart.h"

#ifdef UART0_INT_MODE
#ifndef UART0_TX_INT_MODE
#define UART0_TX_INT_MODE
#endif
#ifndef UART0_RX_INT_MODE
#define UART0_RX_INT_MODE
#endif
#endif

#ifdef UART1_INT_MODE
#ifndef UART1_TX_INT_MODE
#define UART1_TX_INT_MODE
#endif
#ifndef UART1_RX_INT_MODE
#define UART1_RX_INT_MODE
#endif
#endif

/*****************************************************************************/
// various processors have different register names
// we'll use the most encompassing
#if !defined(__AVR_ATmega161__)

#if defined(__AVR_AT90S2333__) || \
    defined(__AVR_AT90S4433__) || \
    defined(__AVR_ATmega32__)  || \
    defined(__AVR_ATmega83__)  || \
    defined(__AVR_ATmega163__)
    
#define UCSR0A  UCSRA
#define UCSR0B  UCSRB

#else

#define UCSR0A  USR
#define UCSR0B  UCR

#endif

#define UDR0    UDR
#define UBRR0   UBRR

#define RXC0    RXC
#define TXC0    TXC
#define UDRE0   UDRE
#define FE0     FE
#define OVR0    OVR
#define U2X0    U2X

#define RXCIE0  RXCIE
#define TXCIE0  TXCIE
#define UDRIE0  UDRIE
#define RXEN0   RXEN
#define TXEN0   TXEN

#else
// 161

#endif


/***** local vars ************************************************************/
#if UART0_SUPPORT
#ifdef UART0_RX_INT_MODE
static uint8_t  uart0_rx_buffer[UART0_RX_BUFFER_SIZE];
static uint16_t uart0_rx_insert_idx, uart0_rx_extract_idx;
#endif

#ifdef UART0_TX_INT_MODE
static uint8_t  uart0_tx_buffer[UART0_TX_BUFFER_SIZE];
static uint16_t uart0_tx_insert_idx, uart0_tx_extract_idx;
#endif
#endif

#if UART1_SUPPORT
#ifdef UART1_RX_INT_MODE
static uint8_t  uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
static uint16_t uart1_rx_insert_idx, uart1_rx_extract_idx;
#endif

#ifdef UART1_TX_INT_MODE
static uint8_t  uart1_tx_buffer[UART1_TX_BUFFER_SIZE];
static uint16_t uart1_tx_insert_idx, uart1_tx_extract_idx;
#endif
#endif

#if UART0_SUPPORT
/******************************************************************************
 *
 * Function Name: uart0Init()
 *
 * Description:  
 *    This function initializes the UART
 *
 * Calling Sequence: 
 *    baud - baudrate divisor
 *
 * Returns:
 *    void
 *
 * Usage: uart0Init(B9600);
 *    or  uart0Init(UART_BAUD(9600));
 *
 *****************************************************************************/
void uart0Init(uint16_t baud)
{
  // disable the UART
  outp(0x00, UCSR0B);
  outp(0x00, UCSR0A);

#if defined(__AVR_ATmega161__) || defined(__AVR_ATmega163__)
  if ( baud & DOUBLE_SPEED_BIT)
	{
//    sbi( UCSR0A, U2X0 );
	 sbi( UCSR0A, U2X );
	 }
#endif

  // load the baudrate divisor register
  outp( (uint8_t)baud, UBRR0 );

#if defined(__AVR_ATmega161__) || defined(__AVR_ATmega32__) || \
    defined(__AVR_ATmega163__) || defined(__AVR_ATmega83__) || \
    defined(__AVR_AT90S4433__) || defined(__AVR_AT90S2333__)
  // output the upper four bits of the baudrate divisor
  outp((inp(UBRRH) & 0xF0) | ((baud >> 8) & 0x0F), UBRRH);
#endif

  // enable the UART0 transmitter & receiver
//  outp(BV(TXEN0) + BV(RXEN0), UCSR0B);
   outp(_BV(TXEN) + _BV(RXEN), UCSR0B);  

#ifdef UART0_TX_INT_MODE
  uart0_tx_extract_idx = uart0_tx_insert_idx = 0;
#endif                                  // UART0_TX_INT_MODE

#ifdef UART0_RX_INT_MODE
  uart0_rx_extract_idx = uart0_rx_insert_idx = 0;

  // enable receiver interrupts
// sbi(UCSR0B, RXCIE0);
  sbi(UCSR0B, RXCIE);   
#endif                                  // UART0_RX_INT_MODE
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
int16_t uart0Putch(char ch)
{
#ifdef UART0_TX_INT_MODE
  uint16_t temp;

  temp = (uart0_tx_insert_idx + 1) % UART0_TX_BUFFER_SIZE;

  if ( temp == uart0_tx_extract_idx ) {
    return -1;                          // no room
  }

  // add to queue
  uart0_tx_buffer[ uart0_tx_insert_idx ] = (uint8_t)ch;
  uart0_tx_insert_idx = temp;
//  sbi( UCSR0B, UDRIE0 );                // enable TX interrupts
  sbi( UCSR0B, UDRIE );                // enable TX interrupts

#else

  while ( bit_is_clear( UCSR0A, UDRE0 ) ) { // wait for TX buffer to empty
    wdt_reset();
  }

  outp(ch, UDR0);

#endif                                  // UART0_TX_INT_MODE

  return (uint8_t)ch;
}

/******************************************************************************
 *
 * Function Name: uart0BlockingPutch()
 *
 * Description:  
 *    This function puts a character into the UART output queue for
 *    transmission and blocks if there is no room.
 *
 * Calling Sequence: 
 *    character to be transmitted
 *
 * Returns:
 *    ch on success, -1 on error
 *
 *****************************************************************************/
int16_t uart0BlockingPutch( char ch )
{
  while( 0 == uart0Space() );
  return uart0Putch( ch );
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
size_t uart0Space(void)
{
#ifdef UART0_TX_INT_MODE
  int space;

  if ((space = (uart0_tx_extract_idx - uart0_tx_insert_idx)) <= 0)
    space += UART0_TX_BUFFER_SIZE;

  return (size_t)(space - 1);
#else
  return USHRT_MAX;
#endif                                  // UART0_TX_INT_MODE
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
char *uart0Puts( char *string )
{
  register char ch;

  while ((ch = *string) && (uart0Putch(ch) >= 0))
    string++;

  return string;
}

/******************************************************************************
 *
 * Function Name: uart0BlockingPuts()
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
char *uart0BlockingPuts( char *string )
{
  register char ch;

  while ((ch = *string) && (uart0BlockingPutch(ch) >= 0))
    string++;

  return string;
}

/******************************************************************************
 *
 * Function Name: uart0Write()
 *
 * Description:  
 *    This function writes 'count' characters from 'buffer' to the UART0
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
int uart0Write( char *buffer, size_t count)
{
#ifdef UART0_TX_INT_MODE
  if (count > uart0Space())
    return -1;
#endif                                  // UART0_TX_INT_MODE
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
 *    !0 - if both the tx holding & shift registers are empty
 *    0 - either the tx holding or shift register is not empty
 *
 *****************************************************************************/
int uart0TxEmpty(void)
{
//#  return bit_is_set(UCSR0A, TXC0);
   return bit_is_set(UCSR0A, TXC );  
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
  /* "Empty" the transmit buffer. */
//#  cbi(UCSR0B, UDRIE0);                  // disable TX interrupts
  cbi(UCSR0B, UDRIE);                  // disable TX interrupts
  uart0_tx_insert_idx = uart0_tx_extract_idx = 0;
//#  sbi(UCSR0B, UDRIE0);                  // enable TX interrupts
  sbi(UCSR0B, UDRIE);                  // enable TX interrupts
#endif                                  // UART0_TX_INT_MODE
}

/******************************************************************************
 *
 * Function Name: uart0Getch()
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
int16_t uart0Getch(void)
{
  uint8_t ch;

#ifdef UART0_RX_INT_MODE
  if (uart0_rx_insert_idx == uart0_rx_extract_idx) // check if character is available
    return -1;

  ch = uart0_rx_buffer[uart0_rx_extract_idx++]; // get character, bump pointer
  uart0_rx_extract_idx %= UART0_RX_BUFFER_SIZE; // limit the pointer
  return ch;
#else
  if ( bit_is_set(UCSR0A, RXC0 ) ) {    // check if character is available
    ch = inp(UDR0);                     // get the character

    if (!(inp(UCSR0A) & (_BV(FE0) | _BV(OVR0)))) // check for errors
      return ch;                        // none, return character
    }

  return -1;                            // no valid cahracter
#endif                                  // UART0_RX_INT_MODE
}

#ifdef UART0_RX_INT_MODE
/******************************************************************************
 *
 * Function Name: SIGNAL(SIG_UART_RECV))
 *
 * Description:  
 *    uart receive isr
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
SIGNAL( SIG_UART0_RECV )
{
  uint16_t temp;

  // check status register for receive errors
//#  if ( inp( UCSR0A ) & ( BV( FE0 ) | BV( OVR0 ))) {
   if ( inp( UCSR0A ) & ( _BV( FE ) | _BV( DOR ))) {
    // this character is no good
    inp( UDR0 );  // dummy read
    return;
  }
  
  temp = ( uart0_rx_insert_idx + 1 ) % UART0_RX_BUFFER_SIZE;
  uart0_rx_buffer[ uart0_rx_insert_idx ] = inp( UDR0 ); // read the character
  
  if ( temp != uart0_rx_extract_idx ) {
    uart0_rx_insert_idx = temp;
  }
}
#endif                                  // UART0_RX_INT_MODE

#ifdef UART0_TX_INT_MODE
/******************************************************************************
 *
 * Function Name: SIGNAL(SIG_UART_DATA)
 *
 * Description:  
 *    uart transmit isr
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
SIGNAL( SIG_UART0_DATA )
{
  if ( uart0_tx_insert_idx != uart0_tx_extract_idx ) {
    outp( uart0_tx_buffer[ uart0_tx_extract_idx++ ], UDR0 );
    uart0_tx_extract_idx %= UART0_TX_BUFFER_SIZE;
  }
  else {
//    cbi( UCSR0B, UDRIE0 );              // disable TX interrupts
     cbi( UCSR0B, UDRIE );              // disable TX interrupts	
  }
}
#endif                                  // UART0_TX_INT_MODE
#endif                                  // UART0_SUPPORT


#if UART1_SUPPORT && defined(__AVR_ATmega161__)
/******************************************************************************
 *
 * Function Name: uart1Init()
 *
 * Description:  
 *    This function initializes the UART
 *
 * Calling Sequence: 
 *    baud - baudrate divisor
 *
 * Returns:
 *    void
 *
 * Usage: uart1Init(B9600);
 *    or  uart1Init(UART_BAUD(9600));
 *
 *****************************************************************************/
void uart1Init( uint16_t baud )
{
  // disable the UART
  outp(0x00, UCSR1B);
  outp(0x00, UCSR1A);

  // check if double speed operation is requested
  if ( baud & DOUBLE_SPEED_BIT ) {
    sbi( UCSR1A, U2X1 );
  }

  // load the baudrate divisor register
  outp((uint8_t)baud, UBRR1);

  // output the upper four bits of the baudrate divisor
  outp((inp(UBRRH) & 0x0F) | ((uint8_t)(baud >> 4) & 0xF0), UBRRH);

  // enable the UART1 transmitter & receiver
  outp( _BV(TXEN1) + _BV(RXEN1), UCSR1B );

#ifdef UART1_TX_INT_MODE
  uart1_tx_extract_idx = uart1_tx_insert_idx = 0;
#endif                                  // UART1_TX_INT_MODE

#ifdef UART1_RX_INT_MODE
  uart1_rx_extract_idx = uart1_rx_insert_idx = 0;

  // enable receiver interrupts
  sbi(UCSR1B, RXCIE1);
#endif                                  // UART1_RX_INT_MODE
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
 *    character to be transmitted
 *
 * Returns:
 *    ch on success, -1 on error (queue full)
 *
 *****************************************************************************/
int16_t uart1Putch( char ch )
{
#ifdef UART1_TX_INT_MODE
  uint16_t temp;

  temp = (uart1_tx_insert_idx + 1) % UART1_TX_BUFFER_SIZE;

  if (temp == uart1_tx_extract_idx)
    return -1;                          // no room

  // add to queue
  uart1_tx_buffer[uart1_tx_insert_idx] = (uint8_t)ch;
  uart1_tx_insert_idx = temp;
  sbi(UCSR1B, UDRIE1);                  // enable TX interrupts

#else

  while ( bit_is_clear(UCSR1A, UDRE1) ) {  // wait for TX buffer to empty
    wdt_reset();
  }

  outp( ch, UDR1 );

#endif                                  // UART1_TX_INT_MODE

  return (uint8_t)ch;
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
size_t uart1Space( void )
{
#ifdef UART1_TX_INT_MODE
  int space;

  if ((space = (uart1_tx_extract_idx - uart1_tx_insert_idx)) <= 0)
    space += UART1_TX_BUFFER_SIZE;

  return (size_t)(space - 1);
#else
  return USHRT_MAX;
#endif                                  // UART1_TX_INT_MODE
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
char *uart1Puts( char *string )
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
 *    This function writes 'count' characters from 'buffer' to the UART1
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
int uart1Write( char *buffer, size_t count )
{
#ifdef UART1_TX_INT_MODE
  if (count > uart1Space())
    return -1;
#endif                                  // UART1_TX_INT_MODE
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
 *    !0 - if both the tx holding & shift registers are empty
 *    0 - either the tx holding or shift register is not empty
 *
 *****************************************************************************/
int uart1TxEmpty(void)
{
  return bit_is_set(UCSR1A, TXC1);
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
void uart1TxFlush( void )
{
#ifdef UART1_TX_INT_MODE
  /* "Empty" the transmit buffer. */
  cbi(UCSR1B, UDRIE1);                  // disable TX interrupts
  uart1_tx_insert_idx = uart1_tx_extract_idx = 0;
  sbi(UCSR1B, UDRIE1);                  // enable TX interrupts
#endif                                  // UART1_TX_INT_MODE
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
int16_t uart1Getch( void )
{
  uint8_t ch;

#ifdef UART1_RX_INT_MODE
  if (uart1_rx_insert_idx == uart1_rx_extract_idx) // check if character is available
    return -1;

  ch = uart1_rx_buffer[uart1_rx_extract_idx++]; // get character, bump pointer
  uart1_rx_extract_idx %= UART1_RX_BUFFER_SIZE; // limit the pointer
  return ch;
#else
  if (bit_is_set(UCSR1A, RXC1))         // check if character is available
    {
    ch = inp(UDR1);                     // get the character

    if (!(inp(UCSR1A) & (_BV(FE1) | _BV(OVR1)))) // check for errors
      return ch;                        // none, return character
    }

  return -1;                            // no valid cahracter
#endif                                  // UART1_RX_INT_MODE
}

#ifdef UART1_RX_INT_MODE
/******************************************************************************
 *
 * Function Name: SIGNAL(SIG_UART1_RECV))
 *
 * Description:  
 *    uart receive isr
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
SIGNAL(SIG_UART1_RECV)
{
  uint16_t temp;

  // check status register for receive errors
  if (inp(UCSR1A) & (_BV(FE1) | _BV(OVR1)))
    {
    // this character is no good
    inp(UDR1);                           // dummy read
    return;
    }

  temp = (uart1_rx_insert_idx + 1) % UART1_RX_BUFFER_SIZE;
  uart1_rx_buffer[uart1_rx_insert_idx] = inp(UDR1); // read the character

  if (temp != uart1_rx_extract_idx)
    uart1_rx_insert_idx = temp;
}
#endif                                  // UART1_RX_INT_MODE

#ifdef UART1_TX_INT_MODE
/******************************************************************************
 *
 * Function Name: SIGNAL(SIG_UART1_DATA)
 *
 * Description:  
 *    uart transmit isr
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
SIGNAL(SIG_UART1_DATA)
{
  if (uart1_tx_insert_idx != uart1_tx_extract_idx)
    {
    outp(uart1_tx_buffer[uart1_tx_extract_idx++], UDR1);
    uart1_tx_extract_idx %= UART1_TX_BUFFER_SIZE;
    }
  else
    cbi(UCSR1B, UDRIE1);                // disable TX interrupts
}
#endif                                  // UART1_TX_INT_MODE
#endif                                  // UART1_SUPPORT && defined(__AVR_ATmega161__)
