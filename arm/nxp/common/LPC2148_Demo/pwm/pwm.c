//
//  $Id: pwm.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/pwm/pwm.c $
//

#include "FreeRTOS.h"

#include "pwm.h"

//
//
//
static int pwmPulseWidth = 20000;
static int pwmPercentage = 50;

//
//
//
int pwmDutyCycle (int percentage)
{
  if ((percentage < 0) || (percentage > 100))
    return -1;

  PWM_MR5 = ((configCPU_CLOCK_HZ / pwmPulseWidth) * (pwmPercentage = percentage)) / 100;
  PWM_LER = PWM_LER_M5L;

  return 0;
}

//
//
//
int pwmFrequency (int frequency)
{
  if ((frequency < 1) || (frequency >= (int) configCPU_CLOCK_HZ))
    return -1;

  PWM_MR0 = (configCPU_CLOCK_HZ / (pwmPulseWidth = frequency));
  PWM_MR5 = ((configCPU_CLOCK_HZ / pwmPulseWidth) * pwmPercentage) / 100;
  PWM_LER = PWM_LER_M5L | PWM_LER_M0L;

  return 0;
}

//
//
//
void pwmInit (void)
{
  SCB_PCONP |= SCB_PCONP_PCPWM0;
  PCB_PINSEL1 |= PCB_PINSEL1_P021_PWM5;

  PWM_TCR  = PWM_TCR_CR;
  PWM_PR   = 0;
  PWM_MR0  = (configCPU_CLOCK_HZ / pwmPulseWidth);
  PWM_MCR |= PWM_MCR_MR0R;
  PWM_PCR |= PWM_PCR_ENA5;
  PWM_TCR  = (PWM_TCR_CE | PWM_TCR_PWME);

  pwmDutyCycle (50);
}
