/*! \file spi.h \brief SPI Interface Driver for LPC2100. */
//*****************************************************************************
//
// File Name	: 'spi.h'
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

#ifndef SPI_H
#define SPI_H

// bit defines
#define SPCR_CPHA			3
#define SPCR_CPOL			4
#define SPCR_MSTR			5
#define SPCR_LSBF			6
#define SPCR_SPIE			7

#define SPSR_ABRT			3
#define SPSR_MODF			4
#define SPSR_ROVR			5
#define SPSR_WCOL			6
#define SPSR_SPIF			7

// functions

// SPI interface initializer
void spiInit(void);

void spiSetClockDiv(u08 clockdiv);

// spiSendByte(u08 data) waits until the SPI interface is ready
// and then sends a single byte over the SPI port.  This command
// does not receive anything.
void spiSendByte(u08 data);

// spiTransferByte(u08 data) waits until the SPI interface is ready
// and then sends a single byte over the SPI port.  The function also
// returns the byte that was received during transmission.
u08 spiTransferByte(u08 data);

#endif
