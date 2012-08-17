/******************************************************************************
 *
 * $Workfile: uart.h $
 *
 * This module provides the interface definitions for uart.c
 * Copyright 2001, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 * $History: uart.h $
 * 
 *****************************************************************************/
#ifndef INC_UART_H
#define INC_UART_H

#include <stddef.h>
#include <inttypes.h>

#define CPU_CLOCK (7.3720e6)

///////////////////////////////////////////////////////////////////////////////
#define UART0_SUPPORT       (1)         // non-zero to enable uart0 code
#define UART1_SUPPORT       (0)         // non-zero to enable uart1 code

///////////////////////////////////////////////////////////////////////////////
// code is optimized for power of 2 buffer sizes (16, 32, 64, 128, ...)
// NOTE: the buffers are only used if the respective interrupt mode is
// enabled.
#define UART0_RX_BUFFER_SIZE 128        // uart0 receive buffer size
#define UART0_TX_BUFFER_SIZE 128        // uart0 transmit buffer size
#define UART1_RX_BUFFER_SIZE 64         // uart1 receive buffer size
#define UART1_TX_BUFFER_SIZE 128        // uart1 transmit buffer size

///////////////////////////////////////////////////////////////////////////////
// uncomment the following to use various interrupt modes
#define UART0_INT_MODE
//#define UART1_INT_MODE
// or
//#define UART0_TX_INT_MODE
//#define UART0_RX_INT_MODE
//#define UART1_TX_INT_MODE
//#define UART1_RX_INT_MODE

///////////////////////////////////////////////////////////////////////////////
// use the following macros to determine the 'baud' parameter values
// for uartInit()
// CAUTION - 'baud' SHOULD ALWAYS BE A CONSTANT or a lot of code
// will be generated.
#define UART_BAUD(baud) ((uint16_t)((CPU_CLOCK / (16.0 * (baud))) + 0.5) - 1)

///////////////////////////////////////////////////////////////////////////////
// some typical baud rates
// confirm the cpu clock rate will support the desired baud rate
#define B115200 (UART_BAUD(115200L))
#define B76800  (UART_BAUD(76800UL))
#define B57600  (UART_BAUD(57600UL))
#define B38400  (UART_BAUD(38400UL))
#define B28800  (UART_BAUD(28800U))
#define B19200  (UART_BAUD(19200U))
#define B14400  (UART_BAUD(14400))
#define B9600   (UART_BAUD(9600))
#define B4800   (UART_BAUD(4800))
#define B2400   (UART_BAUD(2400))

///////////////////////////////////////////////////////////////////////////////
// Double Speed operation for the ATmega161 & ATmega163
// use the following macros to determine the 'baud' parameter values
// for uartInit() for Double Speed operation.  The macros above will
// also work just fine.
// CAUTION - 'baud' SHOULD ALWAYS BE A CONSTANT or a lot of code
// will be generated.
#if defined(__AVR_ATmega161__) || defined(__AVR_ATmega163__)
#define DOUBLE_SPEED_BIT _BV(15)
#define UART_2x_BAUD(baud) \
   (((uint16_t)((CPU_CLOCK / (8.0 * (baud))) + 0.5) - 1) | DOUBLE_SPEED_BIT)

///////////////////////////////////////////////////////////////////////////////
// some typical baud rates
// confirm the cpu clock rate will support the desired baud rate
#define B2x115200 (UART_2x_BAUD(115200L))
#define B2x76800  (UART_2x_BAUD(76800UL))
#define B2x57600  (UART_2x_BAUD(57600UL))
#define B2x38400  (UART_2x_BAUD(38400UL))
#define B2x28800  (UART_2x_BAUD(28800U))
#define B2x19200  (UART_2x_BAUD(19200U))
#define B2x14400  (UART_2x_BAUD(14400))
#define B2x9600   (UART_2x_BAUD(9600))
#define B2x4800   (UART_2x_BAUD(4800))
#define B2x2400   (UART_2x_BAUD(2400))

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
void uart0Init( uint16_t baud );

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
int16_t uart0Putch( char ch );

/******************************************************************************
 *
 * Function Name: uart0BlockingPutch()
 *
 * Description:  
 *    This function puts a character into the UART output queue for
 *    transmission after firts waiting for buffer room.
 *
 * Calling Sequence: 
 *    character to be transmitted
 *
 * Returns:
 *    ch on success, -1 on error
 *
 *****************************************************************************/
int16_t uart0BlockingPutch( char ch );

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
size_t uart0Space( void );

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
char *uart0Puts( char *string);

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
char *uart0BlockingPuts( char *string );


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
int uart0Write(char *buffer, size_t count);

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
int uart0TxEmpty(void);

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
void uart0TxFlush(void);

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
int16_t uart0Getch(void);
#endif

#if UART1_SUPPORT && defined(__AVR_ATmega161__)
/******************************************************************************
 *
 * Function Name: uart1Init()
 *
 * Description:  
 *    This function initializes the UART1
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
void uart1Init( uint16_t baud );

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
int16_t uart1Putch( char ch );

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
size_t uart1Space( void );

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
char *uart1Puts( char *string );

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
int uart1Write( char *buffer, size_t count );

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
int uart1TxEmpty( void );

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
void uart1TxFlush( void );

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
int16_t uart1Getch(void);
#endif                                  // UART1_SUPPORT && defined(__AVR_ATmega161__)
#endif








