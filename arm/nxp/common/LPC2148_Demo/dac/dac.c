//
//  $Id: dac.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/dac/dac.c $
//

#include "FreeRTOS.h"

#include "dac.h"

//
//
//
void dacInit (void)
{
  PCB_PINSEL1 |= PCB_PINSEL1_P025_AOUT;

  DAC_CR = 0;
}

unsigned int dacSet (unsigned int newValue)
{
  unsigned int dacCR;
  unsigned int dacCurrentValue;

  dacCR = DAC_CR;
  dacCurrentValue = (dacCR & DAC_CR_VALUEMASK) >> DAC_CR_VALUESHIFT;
  dacCR = (dacCR & ~DAC_CR_VALUEMASK) | ((newValue << DAC_CR_VALUESHIFT) & DAC_CR_VALUEMASK);
  DAC_CR = dacCR;

  return dacCurrentValue;
}
