#include <interrupt.h>
#include <pgmspace.h>
#include <wdt.h>
#include "uart.h"

int main(void)
{
  int ch;
  char buffer[20];

  uart0Init(B9600);
//  uart1Init(B2x9600);
  sei();
  uart0Puts("\n\rHello World\n\r");
  strncpy_P(buffer, PSTR("\n\rMe too!\n\r"), sizeof (buffer));
  uart0Puts(buffer);

  for (;;)
    {
    if ((ch = uart0Getch()) >= 0)
      uart0Putch(ch);

//    if ((ch = uart1Getch()) >= 0)
//      uart1Putch(ch);

    wdt_reset();
    }

  return 0;                             // should NEVER get here
}
