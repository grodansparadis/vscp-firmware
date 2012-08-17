/*! \file spi.h \brief SPI Interface Driver for AT91SAM7S. */
//*****************************************************************************
//
// File Name	: 'spi.h'
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
///	\ingroup driver_arm_at91
/// \defgroup spi_at91 SPI Interface Driver for AT91SAM7S (armlib/arch/at91/spi.c)
/// \code #include "spi.h" \endcode
/// \par Overview
///		To be written...
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
//@{

#ifndef ARMLIB_SPI_H
#define ARMLIB_SPI_H

// functions

// initialize SPI interface
void spiInit(void);

// spiTransferByte(u08 data) waits until the SPI interface is ready
// and then sends a single byte over the SPI port.  The function also
// returns the byte that was received during transmission.
u08 spiTransferByte(u08 data);


// these functions TBD

//u16 spiTransfer16(u16 data);
//u32 spiTransfer32(u32 data);
//void spiSetClockDiv(u08 clockdiv);

// spiSendByte(u16 data) waits until the SPI interface is ready
// and then sends a single byte over the SPI port.  This command
// does not receive anything.
//void spiSend(u16 data);

#endif
//@}

