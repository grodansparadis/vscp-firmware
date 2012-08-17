/*! \file d2a.c \brief Digital-to-Analog Converter Driver for ADuC7000. */
//*****************************************************************************
//
// File Name	: 'd2a.c'
// Title		: Digital-to-Analog Converter Driver for ADuC7000
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
#include "d2a.h"

// functions
void d2aInit(void)
{
	#define DACBYP				(1<<6)
	#define DACCLK				(1<<5)
	#define DACCLR				(1<<4)
	#define DACRANGE_PWRDOWN	(0x00)
	#define DACRANGE_DACREF		(0x01)
	#define DACRANGE_AREF		(0x02)
	#define DACRANGE_AVDD		(0x03)

	// set DAC to ON, range to 0-AVdd
	DAC0CON |= DACCLR | DACRANGE_AVDD;
	DAC1CON |= DACCLR | DACRANGE_AVDD;
	DAC2CON |= DACCLR | DACRANGE_AVDD;
	DAC3CON |= DACCLR | DACRANGE_AVDD;
}

void d2aSet(int channel, int level)
{
	DAC->Channel[channel].Data = level<<16;
}
