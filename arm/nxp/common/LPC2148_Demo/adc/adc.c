//
//  $Id: adc.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/adc/adc.c $
//

#include "FreeRTOS.h"

#include "adc.h"

//
//
//
int adcRead0_3 (void)
{
  AD0_CR &= ~(AD_CR_START_MASK | AD_CR_SELMASK);
  AD0_CR |=  (AD_CR_START_NOW | AD_CR_SEL3);
  AD0_CR |=   AD_CR_START_NOW;

  while (!(AD0_DR3 & AD_DR_DONE))
    ;

  return ((AD0_DR3 & AD_DR_RESULTMASK) >> AD_DR_RESULTSHIFT);
}

//
//  Assumes PCLK == 48Mhz
//
void adcInit (void)
{
  SCB_PCONP |= SCB_PCONP_PCAD0;

  PCB_PINSEL1 |= PCB_PINSEL1_P030_AD03;

  AD0_CR = AD_CR_CLKS10 | AD_CR_PDN | ((11 - 1) << AD_CR_CLKDIVSHIFT) | AD_CR_SEL3;
}
