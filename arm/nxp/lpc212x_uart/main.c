// This program is created for LPC-E2129 board
// Return echo from rs232

#include "system.h"
#include "rs232.h"

int main()
{
  /**** VARIABLES ****/
  unsigned char ch = 0x0;

  /**** INITIALIZATION ****/
  // Frequency initialization
  FrecInit();
  // UART initialization
  UART0Initialize(9600);



  // loop forever
  while(1) {
    ch = UART0ReadChar();
    UART0WriteChar('*');
    UART0WriteChar(ch);
  }

}
