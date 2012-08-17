/*************************************************************
* ADC interface for the internal PIC16F ADC units
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#ifndef _ADC_H_
#define _ADC_H_

#include <system.h>

volatile bit adc_go @ ADCON0 . GO;
volatile bit adc_on @ ADCON0 . ADON;
 
void adc_init(void);
short adc_measure(char channel);

#endif  // _ADC_H_
