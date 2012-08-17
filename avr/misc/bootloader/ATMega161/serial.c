#include "defines.h"


void initbootuart()
{
  BAUD_RATE_LOW_REG = 11;                           // 19200bps @ 3.69 MHz
  UART_CONTROL_REG = (1 << ENABLE_RECEIVER_BIT) |
                     (1 << ENABLE_TRANSMITTER_BIT); // enable receive and transmit 
}


void sendchar(char c)
{
  UART_DATA_REG = c;                                   // prepare transmission
  while (!(UART_STATUS_REG & (1 << TRANSMIT_COMPLETE_BIT)));// wait until byte sendt
  UART_STATUS_REG |= (1 << TRANSMIT_COMPLETE_BIT);          // delete TXCflag
}


char recchar(void)
{
  while(!(UART_STATUS_REG & (1 << RECEIVE_COMPLETE_BIT)));  // wait for data
  return UART_DATA_REG;
}
