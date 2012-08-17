/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*   based on work copyrighted 2006-2009 by Christian Olesch, Lab-Control      *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	generic AVR ADC functions
*******************************************************************************/

#ifndef _AVR_ADC_H_
#define _AVR_ADC_H_

	// reference selection
	#define ADCREF_OFF		0
	#define ADCREF_AVCC		64
	//#define ADCREF_11V		128 //not available in CANXX
	#define ADCREF_256V		192

	// Prescaler is to be set such that ADC works with 50...200 kHz
	#define ADCPS_1			0
	#define ADCPS_2			1
	#define ADCPS_4			2
	#define ADCPS_8			3
	#define ADCPS_16		4
	#define ADCPS_32		5
	#define ADCPS_64		6
	#define ADCPS_128		7

	// set reference, prescaler and switch on
	void adc_init(uint8_t ref, uint8_t prescaler);

	// disable adc and turn power off
	void adc_exit(void);

	// start a conversion and read it
	int16_t adc_read(uint8_t channel);

#endif
