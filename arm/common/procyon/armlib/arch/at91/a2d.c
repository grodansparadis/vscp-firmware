
// system includes
#include "global.h"
#include "AT91SAM7S64.h"

// local includes
#include "rprintf.h"
#include "a2d.h"

void a2dInit(void)
{
	// turn on clock to ADC (note: unnecessary)
	// AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_ADC);

	// activate PA17-20 as inputs
	AT91C_BASE_PIOA->PIO_PER =   AT91C_PIO_PA17|AT91C_PIO_PA18|AT91C_PIO_PA19|AT91C_PIO_PA20;
	AT91C_BASE_PIOA->PIO_ODR =   AT91C_PIO_PA17|AT91C_PIO_PA18|AT91C_PIO_PA19|AT91C_PIO_PA20;
	AT91C_BASE_PIOA->PIO_PPUDR = AT91C_PIO_PA17|AT91C_PIO_PA18|AT91C_PIO_PA19|AT91C_PIO_PA20;

	// set ADC mode
	AT91C_BASE_ADC->ADC_MR =	AT91C_ADC_TRGEN_DIS |
								AT91C_ADC_LOWRES_10_BIT |
								AT91C_ADC_SLEEP_NORMAL_MODE |
								(0x3F<<8) |		// prescale setting
								(0x08<<16) |	// startup setting
								(0x0F<<24);		// sample-and-hold time

	// enable all channels
/*	AT91C_BASE_ADC->ADC_CHER =	AT91C_ADC_CH0 |
								AT91C_ADC_CH1 |
								AT91C_ADC_CH2 |
								AT91C_ADC_CH3 |
								AT91C_ADC_CH4 |
								AT91C_ADC_CH5 |
								AT91C_ADC_CH6 |
								AT91C_ADC_CH7;
*/
}

int a2dConvert(int channel)
{
	// enable the requested channel (and only the requested channel)
	AT91C_BASE_ADC->ADC_CHDR = 0xFF;
	AT91C_BASE_ADC->ADC_CHER = 1<<channel;
	
	// start conversion
	AT91C_BASE_ADC->ADC_CR = AT91C_ADC_START;

	// wait for end-of-conversion
	// actually, we wait for data ready
	while(!(AT91C_BASE_ADC->ADC_SR & AT91C_ADC_DRDY));

	// return data
	return AT91C_BASE_ADC->ADC_LCDR;
}
