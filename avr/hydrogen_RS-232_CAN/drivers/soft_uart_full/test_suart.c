/****************************************************************************
 Title  :   C Test program for the Software UART FUNCTIONS library (test_uart.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      10/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  Based on application note AVR 305 
            This software is FREE.

*****************************************************************************/

 
#include <io.h>
#include "suart.h"




void main(void)
{

unsigned char buffer[20];
unsigned char string[]="\r\nSoftware UART testing...";
static unsigned char eeprom_string[]__attribute__((section(".eeprom")))="suart_puts_e Testing...";


/* Initialize the software uart to 115200 bps and enable receiving. */
   
#if SUART_DYNAMIC_BAUD_CHANGE == 1  

unsigned long baudrate=115200;
unsigned char error=0;

/* If there is an error go to a lower BAUDRATE */
while( (error=suart_init(baudrate)) == SUART_BAUDRATE_TOO_HIGH )
    {
      baudrate/=2;     
      if(baudrate==28800) { baudrate=38400; }
    } 

#elif SUART_DYNAMIC_BAUD_CHANGE == 0

suart_init();

#endif

#if SUART_DYNAMIC_IO_SETTINGS == 1
suart_set_io(PORTD,4,5);
#endif

while(1)
    {

      suart_puts(string);
      suart_puts_P("\r\n");
/*********************************************************************************************************/
      suart_puts_P("Type something + ENTER");
/*
   In interrupt mode check the "suart_string_received" var for an indication that a string is received
   or x chars are received. 
   In non interrupt mode checking is not needed as the suart_gets(x) does not return until a string is
   received or x chars are received or timeout occured (If enabled). If a timeout occured,
   the "suart_string_received" and "suart_rx_count" vars will have a value of 255 (0xFF)
   if nothing was received prior to timeout or a number representing the received chars
   before the timeout occured.
   In suart_gets(x) the argument passed is the maximum chars wanted. If x=255 then there is no limit.
   IN INTERRUPT MODE THERE IS NO TIMEOUT. IT IS NOT NEEDED ANYWAY.
*/
      suart_gets(SUART_NO_RX_LIMIT);                                  

#if SUART_WILL_USE_INTERRUPT == 1 
      while(suart_string_received == 0);
#endif

#if   SUART_TIMEOUT_ENABLED == 1
      if(suart_timeout_flag) { suart_puts_P(" TIMEOUT!\r\n"); }
#endif
      suart_puts_P("\r\n");
      suart_puts_P("sizeof(received_string)= ");
      suart_puti(suart_string_received, 0);
      suart_puts_P("\r\n1) ACTUAL BUFFER CONTENTS: ");
      suart_puts((unsigned char*)suart_rx_buffer);
      suart_puts_P("\r\n2) RETRIEVED BUFFER DATA: ");
/*
      "suart_get_buffer(buffer);" gets as may as the destination string can hold.
      After the buffer has been read the used chars are removed from the suart_rx_buffer
      and their space is reclaimed.  You can also get the actual suart_rx_buffer as demonstrated. 
      This way the suart_rx_buffer is left intact. 
*/ 
      suart_get_buf(buffer, suart_string_received);
      suart_puts(buffer);
      suart_puts_P("\r\n");

/*********************************************************************************************************/

      suart_puts_P("Type at least 10 chars");
      suart_gets(10);                                 
#if SUART_WILL_USE_INTERRUPT == 1 
      while(suart_string_received == 0);
#endif

#if   SUART_TIMEOUT_ENABLED == 1
      if(suart_timeout_flag) { suart_puts_P(" TIMEOUT!\r\n"); }
#endif
      suart_puts_P("\r\n");
      suart_puts_P("sizeof(received_string)= ");
      suart_puti(suart_string_received, 0);
      suart_puts_P("\r\n1) ACTUAL BUFFER CONTENTS: ");
      suart_puts((unsigned char*)suart_rx_buffer);
      suart_puts_P("\r\n2) RETRIEVED BUFFER DATA: ");
      suart_get_buf(buffer, suart_rx_count);
      suart_puts(buffer);
      suart_puts_P("\r\n");

/*********************************************************************************************************/      
/*
   In suart_raw_rx(x) the argument passed is the maximum chars wanted. If x=0 then there is no limit.
   In interrupt mode suart_raw_rx(x) receives chars of any kind until x chars are received
   or you disable receiving using suart_rx_off(). 
   In non interrupt mode suart_raw_rx(x) does not return until x chars of any kind are received
   or timeout occurs if enabled. 
   MAKE SURE THAT TIMEOUT IS ENABLED IF YOU USE THE suart_raw_rx(x) FUNCTION IN THE NON INTERRUPT MODE
   OTHERWISE YOU WILL BE WAITING FOREVER.
*/
      suart_puts_P("Type at least 5 chars");
      suart_raw_rx(5);
#if SUART_WILL_USE_INTERRUPT == 1 
      while(suart_rx_count < 5); 
#endif
#if   SUART_TIMEOUT_ENABLED == 1
      if(suart_timeout_flag) { suart_puts_P(" TIMEOUT!\r\n"); }
#endif
      suart_get_buf(buffer, suart_rx_count);        /* Read the buffer */
      suart_puts_P("\r\n");
      suart_puts(buffer);                           /* After the buffer has been read the chars are */
      suart_puts_P("\r\n");                         /* no longer in the buffer and more space is usable */

/*********************************************************************************************************/
      
      suart_puts_P("Type 5 more chars");
      suart_raw_rx(SUART_NO_RX_LIMIT);              /* ENABLE RAW DATA RECEIVING. (All chars are accepted) */
#if SUART_WILL_USE_INTERRUPT == 1 
      while(suart_rx_count < 5); 
#endif
#if   SUART_TIMEOUT_ENABLED == 1
      if(suart_timeout_flag) { suart_puts_P(" TIMEOUT!\r\n"); }
#endif
      suart_get_buf(buffer, suart_rx_count);        /* Read the buffer */
      suart_puts_P("\r\n");
      suart_puts(buffer);                           /* After the buffer has been read the chars are */
      suart_puts_P("\r\n");                         /* no longer in the buffer and more space is usable */

/*********************************************************************************************************/

      suart_puts_e(eeprom_string);
      suart_puts_P("\r\nsuart_puti Testing...   ");
      suart_puti(1963,1);
      suart_putc('\r');
      suart_putc('\n');
      


      suart_puts_P("Press ENTER to repeat");
      while(suart_getc()!='\r');
#if   SUART_TIMEOUT_ENABLED == 1
      if(suart_timeout_flag) { suart_puts_P(" TIMEOUT!\r\n"); }
#endif
      suart_putc('\r');
      suart_putc('\n');
      suart_puts_P("\r\n");

#if SUART_WILL_USE_INTERRUPT == 1
      suart_rx_off();
#endif
      
    }
        
}
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/


