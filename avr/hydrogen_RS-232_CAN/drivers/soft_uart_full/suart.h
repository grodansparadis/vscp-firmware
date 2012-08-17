/****************************************************************************
 Title  :   C  include for the SOFTWARE UART FUNCTIONS library (suart.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      10/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  Based on application note AVR 305
            This software is FREE.
*****************************************************************************/

/*################### DO NOT CHANGE ANYTHING BELOW THIS LINE !!! #####################################*/
/*############## ALL CONFIGURATION IS DONE IN "softuart.cfg" file !!! ################################*/

#ifndef SUART_H
#define SUART_H

#define SUART_NO_RX_LIMIT         0xFF

/* ERROR CODES */
#define SUART_BAUDRATE_TOO_LOW    1
#define SUART_BAUDRATE_TOO_HIGH   2 

#include "softuart.cfg"

/* 
   GLOBAL CONTROL VARIABLES VISIBLE TO ALL. USE THEM TO CONTROL THE SUART FUNCTIONS. 
   I have selected variables instead of function calls due to lower overhead and time is not enough.
   For usage of the below variables see the "test_suart.c" file. 
*/
volatile unsigned char  suart_rx_buffer[SUART_BUF_SIZE+1];
volatile unsigned char  suart_rx_count;
volatile unsigned char  suart_string_received;
#if  SUART_TIMEOUT_ENABLED == 1    
volatile unsigned char  suart_timeout_flag;
#endif



/* FUNCTION PROTOTYPES  */
#if SUART_DYNAMIC_BAUD_CHANGE == 1 && SUART_WILL_USE_A_TIMER == 0
extern unsigned char    suart_initialize(unsigned int baudrate);
#elif SUART_DYNAMIC_BAUD_CHANGE == 1 && SUART_WILL_USE_A_TIMER == 1
extern unsigned char    suart_initialize(unsigned long baudrate);
#elif SUART_DYNAMIC_BAUD_CHANGE == 0
extern unsigned char    suart_init(void);
#endif
#if SUART_DYNAMIC_IO_SETTINGS == 1
extern void             suart_set_io_(unsigned char port, unsigned char rx_pin, unsigned char tx_pin);
#endif

extern void             suart_raw_rx(unsigned char number_of_chars);
extern void             suart_rx_off(void);
extern void             suart_putc(unsigned char data);
extern void             suart_puts(unsigned char *tx_data);
extern void             suart_puts_p(const char *progmem_tx_data);
extern void             suart_puts_e(unsigned char *eeprom_tx_data);
extern void             suart_puti(int value, unsigned char dot_position);
extern unsigned char    suart_getc(void);
extern void             suart_gets(unsigned char number_of_chars);
extern void             suart_get_buf(unsigned char *data, unsigned char size);

/* MACRO DEFINITIONS */



#ifndef P
#define P(s) ({static const char c[] __attribute__ ((progmem)) = s;c;})
#endif
#define suart_puts_P(__s)         suart_puts_p(P(__s))

#define suart_get_buffer(x)       suart_get_buf(x, sizeof(x)-1)

#if SUART_DYNAMIC_BAUD_CHANGE == 1 && SUART_WILL_USE_A_TIMER == 0
#define suart_init(baud)          suart_initialize(baud/10)
#elif SUART_DYNAMIC_BAUD_CHANGE == 1 && SUART_WILL_USE_A_TIMER == 1
#define suart_init(baud)          suart_initialize(baud)
#endif

#if  defined(_IO_REG_MACRO_MODE_) || defined(_SFR_ASM_COMPAT) || defined(__ASSEMBLER__)  \
     || !defined(_SFR_DEFS_H_) 

#if SREG > 0X3F
#define suart_set_io(port,rx_pin,tx_pin) suart_set_io_(port,rx_pin,tx_pin)
#elif #if SREG <= 0X3F
#define suart_set_io(port,rx_pin,tx_pin) suart_set_io_((port+0x20),rx_pin,tx_pin)
#endif

#else

#define suart_set_io(port,rx_pin,tx_pin) suart_set_io_(_SFR_MEM_ADDR(port),rx_pin,tx_pin)

#endif

#endif
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/

