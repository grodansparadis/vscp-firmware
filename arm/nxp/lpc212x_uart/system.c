//system.c
#include <iolpc2129.h>
#include "system.h"

#define VIC_TIMER0_bit (1 << VIC_TIMER0)

//oscillator frequency
//IMPORTANT - if you use oscillator with different frequency,
//please change this value, becàuse timer not work correctly
#define OSCILLATOR_CLOCK_FREQUENCY  14745600      //in MHz


unsigned int GetCclk(void)
{
  //return real processor clock speed
  return OSCILLATOR_CLOCK_FREQUENCY * (PLLCON & 1 ? (PLLCFG & 0xF) + 1 : 1);
}

unsigned int GetPclk(void)
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
  return GetCclk() / divider;
}


void FrecInit(void)
{
  //devide or multiplier
  //here is calculate frecuence
  PLLCFG_bit.MSEL = 0x2;  //M - multiplier
  PLLCFG_bit.PSEL = 0x1;  //P - devider
  //set changes (require from architecture)
  PLLFEED_bit.FEED = 0xAA;
  PLLFEED_bit.FEED = 0x55;


  //enable or connect PLL
  //enable PLL
  PLLCON_bit.PLLE = 1;
  //set changes (require from architecture)
  PLLFEED_bit.FEED = 0xAA;
  PLLFEED_bit.FEED = 0x55;

  //wait for PLOK (correct freq)
  while(PLLSTAT_bit.PLOCK == 0);

  //connect PLL
  PLLCON_bit.PLLC = 1;
  //set changes (require from architecture)
  PLLFEED_bit.FEED = 0xAA;
  PLLFEED_bit.FEED = 0x55;
}





