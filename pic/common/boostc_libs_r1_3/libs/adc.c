/*************************************************************
* ADC interface for the internal PIC16F ADC units
*
* Change the Tad setting in adc_init if you use another 
* crystal frequency.
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#include "adc.h"

////////////////////////////////////////////////////////////
// Hardware init code
////////////////////////////////////////////////////////////
void adc_init(void){
	set_bit(adcon1, ADFM);		// AD result needs to be right justified
	set_bit(adcon1, PCFG0);		// all analog inputs
	set_bit(adcon1, PCFG1); 	// Vref+ = Vdd
	set_bit(adcon1, PCFG2);		// Vref- = Vss
	
	set_bit(adcon0, ADCS1);  	// Select Tad = 32 * Tosc (this depends on the X-tal here 10 MHz, should work up to 20 MHz)
	clear_bit(adcon0, CHS0); 	// Channel 0
	clear_bit(adcon0, CHS1); 	//
	clear_bit(adcon0, CHS2);	//
	adc_on = 1;					// Activate AD module
}

short adc_measure(char channel){

	// Clear the channel selection in adcon0
	adcon0 &= 11000111b;
	
	// Shift the channel value to the appropriate position
	channel = (channel & 0x07) << 3;
	
	// And move it to the appropriate position in adcon0
	adcon0 |= channel;

	// Wait the required acquisition time (datasheet)
	delay_10us(2);
	
	// Start the conversion
	adc_go = 1;
	
	// Wait until it is done
	while (adc_go);
	
	short retval;
	retval = (short)adresh;
	retval = retval << 8;
	retval |= adresl;
	
	return retval;
	
}
