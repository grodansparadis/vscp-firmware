//rs232.c
#include <iolpc2129.h>
#include "rs232.h"

#define OSCILLATOR_CLOCK_FREQUENCY  14745600      //in MHz

unsigned int processorClockFrequency(void)
{
  //return real processor clock speed
  return OSCILLATOR_CLOCK_FREQUENCY * (PLLCON & 1 ? (PLLCFG & 0xF) + 1 : 1);
}

unsigned int peripheralClockFrequency(void)
{
  //VPBDIV - determines the relationship between the processor clock (cclk)
  //and the clock used by peripheral devices (pclk).
  unsigned int divider;
  switch (VPBDIV & 3)
  {
    case 0: divider = 4;  break;
    case 1: divider = 1;  break;
    case 2: divider = 2;  break;
  }
  return processorClockFrequency() / divider;
}

/**** UART0 ****/
void UART0Initialize(unsigned int baud)
{
  unsigned int divisor = peripheralClockFrequency() / (16 * baud);

  //set Line Control Register (8 bit, 1 stop bit, no parity, enable DLAB)
  U0LCR_bit.WLS   = 0x3;    //8 bit
  U0LCR_bit.SBS   = 0x0;    //1 stop bit
  U0LCR_bit.PE    = 0x0;    //no parity
  U0LCR_bit.DLAB  = 0x1;    //enable DLAB
  //with one row
  // U0LCR = 0x83;


  //devisor
  U0DLL = divisor & 0xFF;
  U0DLM = (divisor >> 8) & 0xFF;
  U0LCR &= ~0x80;

  //set functionalite to pins:  port0.0 -> TX0,  port0.1 -> RXD0
  PINSEL0_bit.P0_0 = 0x1;
  PINSEL0_bit.P0_1 = 0x1;
  //with one row
  //PINSEL0 = PINSEL0 & ~0xF | 0x5;
}

void UART0WriteChar(unsigned char ch0)
{
  //when U0LSR_bit.THRE is 0 - U0THR contains valid data.
  while (U0LSR_bit.THRE == 0);
  U0THR = ch0;
}

unsigned char UART0ReadChar(void)
{
  //when U0LSR_bit.DR is 1 - U0RBR contains valid data
  while (U0LSR_bit.DR == 0);
  return U0RBR;
}


unsigned char UART0ReadChar_nostop(void)
{
  //when U0LSR_bit.DR is 1 - U0RBR contains valid data
  if(U0LSR_bit.DR == 1) return U0RBR;
  else return 0;
}

void UART0WriteChar_nostop(unsigned char ch0)
{
  //when U0LSR_bit.THRE is 0 - U0THR contains valid data.
  if(U0LSR_bit.THRE == 1) U0THR = ch0;
}



/**** UART1 ****/
void UART1Initialize(unsigned int baud)
{
  unsigned int divisor = peripheralClockFrequency() / (16 * baud);

  //set Line Control Register (8 bit, 1 stop bit, no parity, enable DLAB)
  U1LCR_bit.WLS   = 0x3;    //8 bit
  U1LCR_bit.SBS   = 0x0;    //1 stop bit
  U1LCR_bit.PE    = 0x0;    //no parity
  U1LCR_bit.DLAB  = 0x1;    //enable DLAB
  //with one row
  // U0LCR = 0x83;


  //devisor
  U1DLL = divisor & 0xFF;
  U1DLM = (divisor >> 8) & 0xFF;
  U1LCR &= ~0x80;

  //set functionalite to pins:  port0.8 -> TX1,  port0.9 -> RXD1
  PINSEL0_bit.P0_8 = 0x1;
  PINSEL0_bit.P0_9 = 0x1;
  //with one row
  //PINSEL0 = PINSEL0 & ~0xF | 0x5;
}

void UART1WriteChar(unsigned char ch0)
{
  //when U0LSR_bit.THRE is 0 - U0THR contains valid data.
  while (U1LSR_bit.THRE == 0);
  U1THR = ch0;
}

unsigned char UART1ReadChar(void)
{
  //when U0LSR_bit.DR is 1 - U0RBR contains valid data
  while (U1LSR_bit.DR == 0);
  return U1RBR;
}


unsigned char UART1ReadChar_nostop(void)
{
  //when U0LSR_bit.DR is 1 - U0RBR contains valid data
  if(U1LSR_bit.DR == 1) return U1RBR;
  else return 0;
}

void UART1WriteChar_nostop(unsigned char ch0)
{
  //when U0LSR_bit.THRE is 0 - U0THR contains valid data.
  if(U1LSR_bit.THRE == 1) U1THR = ch0;
}



