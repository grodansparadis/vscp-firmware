/*! \file spiflash.c \brief SPI Flash Memory Driver (M25Pxx/AT25Fxxx/etc). */
//*****************************************************************************
//
// File Name	: 'spiflash.c'
// Title		: SPI Flash Memory Driver (M25Pxx/AT25Fxxx/etc)
// Author		: Pascal Stang - Copyright (C) 2006
// Created		: 2006-04-15
// Revised		: 2006-07-02
// Version		: 0.1
// Target MCU	: ARM processors
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
//*****************************************************************************

// system includes
#include "at91sam7s64.h"
#include "global.h"

// local includes
#include "spi.h"
#include "spiflash.h"
/*
#define SPIFLASH_CONFIG_CS		AT91C_BASE_PIOA->PIO_PER = AT91C_PIO_PA11; \
								AT91C_BASE_PIOA->PIO_OER = AT91C_PIO_PA11
#define SPIFLASH_ASSERT_CS		AT91C_BASE_PIOA->PIO_CODR = AT91C_PIO_PA11
#define SPIFLASH_RELEASE_CS		AT91C_BASE_PIOA->PIO_SODR = AT91C_PIO_PA11
*/

#define SPIFLASH_CONFIG_CS		AT91C_BASE_PIOA->PIO_PER = AT91C_PIO_PA30; \
								AT91C_BASE_PIOA->PIO_OER = AT91C_PIO_PA30
#define SPIFLASH_ASSERT_CS		AT91C_BASE_PIOA->PIO_CODR = AT91C_PIO_PA30
#define SPIFLASH_RELEASE_CS		AT91C_BASE_PIOA->PIO_SODR = AT91C_PIO_PA30

// functions
void spiflashInit(void)
{
	// initialize spi
	//spiInit();
	// initialize chip select
	SPIFLASH_RELEASE_CS;
	SPIFLASH_CONFIG_CS;
}

unsigned short spiflashGetID(void)
{
	unsigned short id;

	SPIFLASH_ASSERT_CS;
	spiTransferByte(SPIFLASH_CMD_RDID);
	id  = spiTransferByte(0x00)<<8;
	id |= spiTransferByte(0x00);
	SPIFLASH_RELEASE_CS;

	return id;
}

void spiflashChipErase(void)
{
	// enable write
	SPIFLASH_ASSERT_CS;
	spiTransferByte(SPIFLASH_CMD_WREN);
	SPIFLASH_RELEASE_CS;

	// clock out dummy byte to waste time
	spiTransferByte(0x00);

	// do chip erase
	SPIFLASH_ASSERT_CS;
	spiTransferByte(SPIFLASH_CMD_CHIPERASE);
	SPIFLASH_RELEASE_CS;

	// clock out dummy byte to waste time
	spiTransferByte(0x00);

	// wait until write is done
	SPIFLASH_ASSERT_CS;
	spiTransferByte(SPIFLASH_CMD_RDSR);
	while(spiTransferByte(0x00) & SPIFLASH_STATUS_BUSY);
	SPIFLASH_RELEASE_CS;
}

void spiflashRead(unsigned long addr, unsigned long nbytes, unsigned char *data)
{
	// begin read
	SPIFLASH_ASSERT_CS;
	// issue read command
	spiTransferByte(SPIFLASH_CMD_READ);
	// send address
	spiTransferByte(addr>>16);
	spiTransferByte(addr>>8);
	spiTransferByte(addr>>0);
	// read data
	while(nbytes--)
		*data++ = spiTransferByte(0x00);
	// end read
	SPIFLASH_RELEASE_CS;
}

void spiflashWrite(unsigned long addr, unsigned long nbytes, unsigned char *data)
{
	unsigned int page;
	unsigned int i;
	unsigned int pagelen;

	// loop through pages to be programmed
	for(page=0; page<((nbytes+SPIFLASH_PAGESIZE-1)>>8); page++)
	{
		// program this page

		// enable write
		SPIFLASH_ASSERT_CS;
		spiTransferByte(SPIFLASH_CMD_WREN);
		SPIFLASH_RELEASE_CS;
		
		// clock out dummy byte to waste time
		spiTransferByte(0x00);

		// begin write
		SPIFLASH_ASSERT_CS;
		// issue write command
		spiTransferByte(SPIFLASH_CMD_PAGEPROG);
		// send address
		spiTransferByte(addr>>16);
		spiTransferByte(addr>>8);
		spiTransferByte(addr>>0);
		// program exactly the number of bytes requested
		if( ((page<<8)+SPIFLASH_PAGESIZE) <= nbytes)
			pagelen = SPIFLASH_PAGESIZE;
		else
			pagelen = nbytes-(page<<8);
		// transfer data
		for(i=0; i<pagelen; i++)
			spiTransferByte(*data++);
		// end write
		SPIFLASH_RELEASE_CS;

		// clock out dummy byte to waste time
		spiTransferByte(0x00);

		// wait until write is done
		SPIFLASH_ASSERT_CS;
		spiTransferByte(SPIFLASH_CMD_RDSR);
		while(spiTransferByte(0x00) & SPIFLASH_STATUS_BUSY);
		SPIFLASH_RELEASE_CS;

		// clock out dummy byte to waste time
		spiTransferByte(0x00);
	}
}
