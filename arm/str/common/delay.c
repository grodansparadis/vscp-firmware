/*
   Precise Delay Functions for LPC2100
   Martin Thomas, 11/2004
*/

#include "systime.h"
#include "delay.h"


void delay_us_( unsigned long delayval )
{
  // Not implemented yet
}


void delay_ms( unsigned long delayval )
{
  unsigned long d = (delayval * 1e-3 * sysTICSperSEC ) + .5;
  pause( d );
}
