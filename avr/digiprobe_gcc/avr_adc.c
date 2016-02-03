/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*   based on work copyrighted 2006-2009 by Christian Olesch, Lab-Control      *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	generic AVR ADC functions
*******************************************************************************/

#include "digiprobe_gcc.inc"

/******************************************************************************
	Set reference and prescaler and enable ADC
 ******************************************************************************/
void adc_init(uint8_t refbits, uint8_t prescaler_div)
{
	#ifdef PRADC
		power_adc_enable();
	#endif

	// Referenz einstellen
	ADMUX = refbits;

	// Prescaler einstellen und AD-Wandler einschalten
	ADCSRA = _BV(ADEN) | prescaler_div;
};

/******************************************************************************
	Disable ADC and switch off if possible
 ******************************************************************************/
void adc_exit(void)
{
	#ifdef PRADC
		power_adc_disable();
	#endif

	// switch it off by setting ADEN to zero
	ADCSRA &= ~_BV(ADEN);
};

/******************************************************************************
	Start conversion and read it out
 ******************************************************************************/
int16_t adc_read(uint8_t channel)
{
	uint8_t lobyte, hibyte;
	int16_t result;

	//Warten bis evtl. vorhergehende Wandlung beendet.
	while(ADCSRA & _BV(ADSC)) {};

	//Kanal auswählen, die CANXX haben nur 8 kanäle, daher MUX auf 0...7 begrenzen
	channel &= 7;
	ADMUX &= ~7;
	ADMUX |= channel;

	//Wandlungs starten
	ADCSRA |= _BV(ADSC);

	//Warten bis Wandlung beendet.
	while(ADCSRA & _BV(ADSC)) {};

	//Ergebnis auslesen
	lobyte=ADCL;
	hibyte=ADCH;

	result = hibyte;
	result <<= 8;
	result += lobyte;

	return result;
};

