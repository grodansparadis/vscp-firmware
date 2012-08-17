/*! \file a2d.c \brief Analog-to-Digital Converter Driver for LPC2000. */
//*****************************************************************************
//
// File Name	: 'a2d.c'
// Title		: Analog-to-Digital Converter Driver for LPC2000
// Author		: Pascal Stang - Copyright (C) 2006
// Created		: 2006.04.09
// Revised		: 2006.04.15
// Version		: 0.1
// Target MCU	: LPC2000 ARM processors
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

// system includes
#include "lpc2000.h"
#include "global.h"

// local includes
#include "a2d.h"

void a2dInit(void)
{
	// set ADC to:
	// - CLKDIV as appropriate
	// - single conversion mode
	// - 10-bit accuracy
	// - powered-up
	ADCR = (((PCLK/4500000))<<8) | ADCR_CLKS_10BIT | ADCR_PDN;
}

int a2dConvert(int channel)
{
	uint32_t result=0;

	// start conversion on requested channel
	ADCR &= ~(ADCR_SEL_MASK|ADCR_START_NOW);	// clear previous channel and conversion
	ADCR |= (1<<channel);	// select new channel
	ADCR |= ADCR_START_NOW;	// start conversion

	// wait for end-of-conversion
	while(!(result & ADDR_DONE)) result = ADDR;

	// return data
	return (result & ADDR_VALUE_MASK)>>6;
}

