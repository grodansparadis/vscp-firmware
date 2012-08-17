/*! \file spi.c \brief SPI Interface Driver for LPC2100. */
//*****************************************************************************
//
// File Name	: 'spi.c'
// Title		: SPI Interface Driver for LPC2100
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.05.05
// Revised		: 2004.07.12
// Version		: 0.1
// Target MCU	: ARM processors
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "lpc210x.h"
#include "config.h"
#include "global.h"
#include "spi.h"

void spiInit(void)
{
	// setup SCK pin P04
	PINSEL0 &= ~(3<<8);
	PINSEL0 |= 1<<8;
	// setup MISO pin P05
	PINSEL0 &= ~(3<<10);
	PINSEL0 |= 1<<10;
	// setup MOSI pin P06
	PINSEL0 &= ~(3<<12);
	PINSEL0 |= 1<<12;
	// setup SSEL pin P07
	PINSEL0 &= ~(3<<14);
	PINSEL0 |= 1<<14;


}

void spiSetClockDiv(u08 clockdiv)
{
	SPCCR = clockdiv;
}

void spiSendByte(u08 data)
{

}

u08 spiTransferByte(u08 data)
{
	return 0;
}
