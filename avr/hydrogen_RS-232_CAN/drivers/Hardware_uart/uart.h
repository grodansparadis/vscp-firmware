
/****************************************************************************
 Title  :   C  include file for the UART FUNCTIONS library (uart.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      30/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/


#ifndef UART_H
#define UART_H

/*##############################################################################################*/
/*                             START OF CONFIGURATION BLOCK                                     */
/*##############################################################################################*/

#ifndef F_CPU
#define F_CPU                         3686400   /* CPU clock frequency for STK500 */
#endif
#define UARTS_NEEDED                  2         /* how many uart you need */
#define UART_RX_BUFFER_SIZE           20        /* Receive buffer size  */
#define UART0_RX_TERMINATION_CHAR     '\r'
#define UART1_RX_TERMINATION_CHAR     '\r'

/* Ways to reduce code size */
#define UART_DYNAMIC_BAUD_CHANGE     1          /* 1 if you will change baudrate during run time */
#define UART0_BAUDRATE               115200     /* Valid only if SUART_DYNAMIC_BAUD_CHANGE == 0 */
#define UART1_BAUDRATE               115200     /* Valid only if SUART_DYNAMIC_BAUD_CHANGE == 0 */

#define UART0_ERROR_CHECK_SUPPORT    1
#define UART0_AT_COMMAND_SUPPORT     1        
#define UART0_RAW_DATA_SUPPORT       1
#define UART1_ERROR_CHECK_SUPPORT    1
#define UART1_AT_COMMAND_SUPPORT     1        
#define UART1_RAW_DATA_SUPPORT       1

#define UART_MULTI_COMMAND_SUPPORT   1

/*##############################################################################################*/
/*                              END OF CONFIGURATION BLOCK                                      */
/*##############################################################################################*/
/*
   ***********************  UART COMMAND DEFINITIONS  ******************************
   When you issue a "uart_command(SELECT_UARTx)" (x=0 or 1) all the commands
   and functions that follow refer to that particular UART.
   To change UART issue another "uart_command(SELECT_UARTx)" command.

   The "uart_command(UART_GET_RX_COUNT)" returns the number of received chars.
   When in "UART_GET_RAW_DATA" mode it returns the number of received chars IN REAL TIME.
   When in "UART_GET_AT_CMD" mode it returns the number of received chars after and only after
   a valid "AT" command is received. Untill then it returns zero.
   When in "UART_GET_STRING" mode it returns the number of received chars after and only after
   a valid string is received. Untill then it returns zero.
   Use "uart_command(UART_GET_RX_COUNT)" to test if something is received.
   If something is received you can read it using the "uart_get_buffer()" macro
   or the "uart_get_buf()"command.

   When issuing UART COMMANDS Precedence is from LSB to MSB.
   That means that if you give multiple relevant commands the higher number command
   is executed last thus superseding the lower number commands.
   EXAMPLE: if you give the "suart_command(SELECT_UART0|UART_GET_AT_CMD | UART_RX_OFF)" by mistake
   the uart receiving will be disabled.
*/
#define SELECT_UART1              (1<<0)
#define SELECT_UART0              (1<<1)
#define UART_TX_ON                (1<<2)
#define UART_GET_STRING           (1<<3)        /* Get a string               */
#define UART_GET_AT_CMD           (1<<4)        /* waits for a new AT command */
#define UART_GET_RAW_DATA         (1<<5)        /* Enables RAW data reception */
#define UART_GET_ERROR            (1<<6)        /* Returns the error number if any */
#define UART_GET_RX_COUNT         (1<<7)        /* Returns the number of received chars */
#define UART_STRING_RECEIVED      (1<<8)        /* Returns the number of received chars in a string */
#define UART_AT_CMD_RECEIVED      (1<<9)        /* Returns the number of received chars in a string */
#define UART_ECHO_ON              (1<<10)
#define UART_ECHO_OFF             (1<<11)
#define UART_RX_OFF               (1<<12)
#define UART_TX_OFF               (1<<13)

#define OVERUN_ERROR               1
#define FRAMING_ERROR              2
#define BUFFER_OVERUN              3
#define UART_BAUDRATE_TOO_LOW      4
#define UART_BAUDRATE_TOO_HIGH     5
#define UART_HIGH_BAUDRATE_ERROR   6

#define UART0                      0
#define UART1                      1


/* REGISTER & DEVICE CAPABILITY LIBRARY */
#if defined (__AVR_ATmega128__)

#define UART0_UDR                  UDR0
#define UART0_UCR                  UCSR0B
#define UART0_USR                  UCSR0A
#define UART0_UCSRC                0
#define UART0_UBRRL                UBRR0L
#define UART0_UBRRH                UBRR0H
#define UART0_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART0_TRANS)
#define UART0_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART0_RECV)

#define UART1_UDR                  UDR1
#define UART1_UCR                  UCSR1B
#define UART1_USR                  UCSR1A
#define UART1_UCSRC                0
#define UART1_UBRRL                UBRR1L
#define UART1_UBRRH                UBRR1H
#define UART1_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART1_TRANS)
#define UART1_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART1_RECV)

#define NUMBER_OF_UARTS            2
#define UART_DOUBLE_SPEED_CAPABLE  1
#define USART_CAPABLE              1
/*------------------------------------------------------------------------------------------------*/
#elif defined (__AVR_ATmega161__)

#define UART0_UDR                  UDR0
#define UART0_UCR                  UCSR0B
#define UART0_USR                  UCSR0A
#define UART0_UCSRC                0
#define UART0_UBRRL                UBRR0
#define UART0_UBRRH                0
#define UART0_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART0_TRANS)
#define UART0_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART0_RECV)

#define UART1_UDR                  UDR1
#define UART1_UCR                  UCSR1B
#define UART1_USR                  UCSR1A
#define UART1_UCSRC                0
#define UART1_UBRRL                UBRR1
#define UART1_UBRRH                0
#define UART1_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART1_TRANS)
#define UART1_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART1_RECV)

#define NUMBER_OF_UARTS            2
#define UART_DOUBLE_SPEED_CAPABLE  1
#define USART_CAPABLE              0

/*------------------------------------------------------------------------------------------------*/
#elif defined(__AVR_ATmega163__) || defined(__AVR_ATmega323__)

#define UART0_UDR                  UDR
#define UART0_UCR                  UCSRB
#define UART0_USR                  UCSRA
#define UART0_UCSRC                0
#define UART0_UBRRL                UBRR
#define UART0_UBRRH                UBRRH
#define UART0_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART_TRANS)
#define UART0_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART_RECV)

#define NUMBER_OF_UARTS            1
#define UART_DOUBLE_SPEED_CAPABLE  1
#define USART_CAPABLE              0
#undef  UART1_RX_TERMINATION_CHAR
/*------------------------------------------------------------------------------------------------*/
#elif defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || defined(__AVR_ATmega64__) || \
      defined(__AVR_ATmega8535__) || defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8__)


#define UART0_UDR                  UDR
#define UART0_UCR                  UCSRB
#define UART0_USR                  UCSRA
#define UART0_UCSRC                0
#define UART0_UBRRL                UBRRL
#define UART0_UBRRH                UBRRH
#define UART0_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART_TRANS)
#define UART0_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART_RECV)

#define NUMBER_OF_UARTS            1
#define UART_DOUBLE_SPEED_CAPABLE  1
#define USART_CAPABLE              1
#undef  UART1_RX_TERMINATION_CHAR
/*------------------------------------------------------------------------------------------------*/
#else

#define UART0_UDR                  UDR
#define UART0_UCR                  UCR
#define UART0_USR                  USR
#define UART0_UCSRC                0
#define UART0_UBRRL                UBRR
#define UART0_UBRRH                0
#define UART0_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART_TRANS)
#define UART0_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART_RECV)

#define NUMBER_OF_UARTS            1
#define UART_DOUBLE_SPEED_CAPABLE  0
#define USART_CAPABLE              0
#undef  UART1_RX_TERMINATION_CHAR

#endif
/*------------------------------------------------------------------------------------------------*/
/* DEFINITION OF UART BIT POSITIONS  */
/* UART_USR */
#define UART_RXC                   7
#define UART_TXC                   6
#define UART_UDRE                  5
#define UART_FE                    4
#define UART_OVR                   3
#define UART_U2X                   1

/* UART_UCR */
#define UART_RXCIE                 7
#define UART_TXCIE                 6
#define UART_UDRIE                 5
#define UART_RXEN                  4
#define UART_TXEN                  3


#define RAM                        1
#define EEPROM                     2
#define FLASH                      3


/* DECISION OF HOW MANY UARTS TO USE */
#if      NUMBER_OF_UARTS > UARTS_NEEDED
#undef   NUMBER_OF_UARTS
#define  NUMBER_OF_UARTS   UARTS_NEEDED

#elif    NUMBER_OF_UARTS == 2 && UARTS_NEEDED > 2
#warning YOU DONT HAVE THAT MANY UARTS! CONFIGURATION DONE USING 2 UART!

#elif    NUMBER_OF_UARTS == 1 && UARTS_NEEDED >= 2
#warning YOU DONT HAVE 2 UARTS! CONFIGURATION DONE USING 1 UART!

#endif

/*##############################################################################################*/
/* UART CONTROL FUNCTIONS */
/*##############################################################################################*/
/* 
   Initialize UART. The function sets baud rate and gives varius uart commands.
   The uart commands can be ORed.
*/
#if    UART_DYNAMIC_BAUD_CHANGE == 1
extern unsigned char  uart_init(unsigned long int baud_rate, unsigned int command);
#elif  UART_DYNAMIC_BAUD_CHANGE == 0
extern void           uart_init(void);
#endif

/*
   Send commands to UART. See the #define section of this file for the commands.
   The uart commands can be ORed.
*/
extern unsigned char  uart_command(unsigned int command);

/*##############################################################################################*/
/* UART RECEIVED DATA FUNCTIONS */
/*##############################################################################################*/

/*
   Copies rx buffer to data and flushes the rx buffer if in UART_AT_RX mode.
   In UART_GET_RAW_DATA mode you get a precise copy of the specified size.
   Data in the rx buffer that weren't copied are realigned towards the start of the buffer
   and the new found space is reclaimed.
   The function performs destination array size check so it will return only as many chars
   as the destination array can hold. That way you can use the uart_get_buffer(*array) macro. 
   In UART_GET_AT_CMD mode you get the "AT" command without the "AT" prefix.
   After succesfull reception of an "AT" command receiving is disabled! so if you want to receive
   further "AT" commands reenable receiving issuing a "uart_command(UART_GET_AT_CMD)" command.
   In UART_GET_STRING mode you get the received string.
   After succesfull reception of a string receiving is disabled! so if you want to receive
   another string reenable receiving issuing a "uart_command(UART_GET_STRING)" command.
*/
extern void           uart_get_buf(unsigned char *data, unsigned char size);

/* get a char (interupt mode). Chars are written in the rx buffer also. */
extern unsigned char  uart_getc(void);

/*##############################################################################################*/
/* UART DATA TRANSMISSION  FUNCTIONS */
/*##############################################################################################*/

/* send a string (interrupt mode) and also checks for a NULL pointer or EMPTY string */
extern void           uart_puts(unsigned char *tx_data);

/* send a program memory string (interrupt mode). Use the MACRO uart_puts_P for convinience
   if a constant string is to be used  since it automatically creates and stores the string
   to program memory.                                                                    */
extern void           uart_puts_p(const char *progmem_tx_data);

/* send a string located at EEPROM memory (interrupt mode) */
extern void           uart_puts_e(unsigned char *eeprom_tx_data);

/* send a signed integer (interrupt mode) */
extern void           uart_puti(int value, unsigned char dot_position);

/* send a char (interrupt mode) */
extern  void          uart_putc(unsigned char tx_data);


/* MACRO's DEFINITIONS */
/* Macro to automatically store string to program memory */
#ifndef P
#define P(s) ({static const char c[] __attribute__ ((progmem)) = s;c;})
#endif
#define uart_puts_P(__s)         uart_puts_p(P(__s))
/* Macro to use uart_gets with one argument */
#define uart_getS(x)             uart_gets(x,sizeof(x))
#define uart_get_buffer(x)       uart_get_buf(x, sizeof(x)-1)


#endif  /* ifndef UART_H */
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/

