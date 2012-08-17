/*! \file spi.c \brief SPI Interface Driver for AT91SAM7S. */
//*****************************************************************************
//
// File Name	: 'spi.c'
// Title		: SPI Interface Driver for AT91SAM7S
// Author		: Pascal Stang - Copyright (C) 2006
// Created		: 2006.02.01
// Revised		: 2006.02.01
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

#include "at91sam7s64.h"
#include "global.h"
#include "spi.h"

#ifndef SPI_SCKDIV
#define SPI_SCKDIV		2
#endif

void spiInit(void)
{
	// enable clock to SPI interface
	AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_SPI);

    // setup PIO pins for SPI bus
    *AT91C_PIOA_ASR   = AT91C_PA12_MISO|AT91C_PA13_MOSI|AT91C_PA14_SPCK;	// assign pins to SPI interface
    *AT91C_PIOA_PDR   = AT91C_PA12_MISO|AT91C_PA13_MOSI|AT91C_PA14_SPCK;
    *AT91C_PIOA_PPUER = AT91C_PA12_MISO|AT91C_PA13_MOSI|AT91C_PA14_SPCK;	// set pullups
	// setup PIO pins for SPI chip selects
	//AT91C_BASE_PIOA->PIO_ASR = AT91C_PA11_NPCS0|AT91C_PA31_NPCS1;
	//AT91C_BASE_PIOA->PIO_PDR = AT91C_PA11_NPCS0|AT91C_PA31_NPCS1;
	//AT91C_BASE_PIOA->PIO_OER = AT91C_PA11_NPCS0|AT91C_PA31_NPCS1;

	// reset and enable SPI
    *AT91C_SPI_CR = AT91C_SPI_SPIEN | AT91C_SPI_SWRST;
	*AT91C_SPI_CR = AT91C_SPI_SPIEN;

	// set master mode with:
	//	- SPI master
	//	- no mode fault
	//	- variable peripheral chip select
	*AT91C_SPI_MR = AT91C_SPI_MODFDIS | AT91C_SPI_PS_VARIABLE | AT91C_SPI_MSTR;
//	*AT91C_SPI_MR = AT91C_SPI_MODFDIS | AT91C_SPI_PS_FIXED | AT91C_SPI_MSTR | (0x0E<<16);

	// setup data transfer format and rate for device 0-3 => 8bits, CPOL=0, NCPHA=1
	AT91C_SPI_CSR[0] = AT91C_SPI_NCPHA|AT91C_SPI_BITS_8|(SPI_SCKDIV<<8);
	AT91C_SPI_CSR[1] = AT91C_SPI_NCPHA|AT91C_SPI_BITS_8|(SPI_SCKDIV<<8);
	AT91C_SPI_CSR[2] = AT91C_SPI_NCPHA|AT91C_SPI_BITS_8|(SPI_SCKDIV<<8);
	AT91C_SPI_CSR[3] = AT91C_SPI_NCPHA|AT91C_SPI_BITS_8|(SPI_SCKDIV<<8);
}

u08 spiTransferByte(u08 data)
{
	// wait for transmit completion/ready
	while(!(*AT91C_SPI_SR & AT91C_SPI_TDRE));
	// write data to be transmitted
	*AT91C_SPI_TDR = data;
	// wait for completion
	while(!(*AT91C_SPI_SR & AT91C_SPI_RDRF));
	// return received data
	return *AT91C_SPI_RDR;
}

u16 spiTransfer16(u16 data)
{
	// wait for transmit completion/ready
	while(!(*AT91C_SPI_SR & AT91C_SPI_TDRE));
	// write data to be transmitted
	*AT91C_SPI_TDR = data;
	// wait for completion
	while(!(*AT91C_SPI_SR & AT91C_SPI_RDRF));
	// return received data
	return *AT91C_SPI_RDR;
}

u32 spiTransfer32(u32 data)
{
	u32 rxdata;

	// wait for transmit completion/ready
	while(!(*AT91C_SPI_SR & AT91C_SPI_TDRE));
	// write data to be transmitted
	*AT91C_SPI_TDR = data>>16;
	// wait for completion
	while(!(*AT91C_SPI_SR & AT91C_SPI_RDRF));
	// get received data
	rxdata = (*AT91C_SPI_RDR&0xFFFF)<<16;
	// write data to be transmitted
	*AT91C_SPI_TDR = data;
	// wait for completion
	while(!(*AT91C_SPI_SR & AT91C_SPI_RDRF));
	// get received data
	rxdata |= (*AT91C_SPI_RDR&0xFFFF);
	// return received data
	return rxdata;
}

/*
void spiSetClockDiv(u08 clockdiv)
{
	//SPCCR = clockdiv;
}

void spiSend(u16 data)
{
	// write data to be transmitted
	*AT91C_SPI_TDR = data;
}
*/
