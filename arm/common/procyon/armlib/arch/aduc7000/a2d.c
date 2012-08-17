/*! \file a2d.c \brief Analog-to-digital Converter Driver for ADuC7000. */
//*****************************************************************************
//
// File Name	: 'a2d.c'
// Title		: Analog-to-digital Converter Driver for ADuC7000
// Author		: Pascal Stang - Copyright (C) 2005
// Created		: 2/5/2005
// Revised		: 2/5/2005
// Version		: 0.1
// Target MCU	: Analog Devices ADuC7000 ARM Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "aduc7026.h"

#include "global.h"
#include "a2d.h"

// functions
void a2dInit(void)
{
	#define ADCENCONV			(1<<7)
	#define ADCBUSY				(1<<6)
	#define ADCPWR				(1<<5)
	#define ADCMODE_SE			(0x00<<3)
	#define ADCMODE_DIFF		(0x01<<3)
	#define ADCMODE_PDIFF		(0x02<<3)
	#define ADCMODE_MASK		(0x03<<3)
	#define ADCCONV_SINGLE		(0x03)
	#define ADCCONV_CONT		(0x04)

	// set ADC to ON
	ADCCON |= ADCPWR;

	// select reference
	REFCON = 0x01;	// internal 2.5V reference
}

int a2dConvert(int channel)
{
	// set conversion mode to single-ended
	ADCCON &= ~ADCMODE_MASK;
	ADCCON |= ADCMODE_SE;
	// set channel for conversion
	ADCCP = channel;
	// start conversion
	ADCCON |= ADCENCONV | ADCCONV_SINGLE;
	// wait until complete
	while(!ADCSTA);
	// return result
	return ADCDAT>>16;
}
