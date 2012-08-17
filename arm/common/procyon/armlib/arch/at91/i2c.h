/*! \file i2c.h \brief I2C Interface Driver for Atmel AT91. */
//*****************************************************************************
//
// File Name	: 'i2c.h'
// Title		: I2C Interface Driver for Atmel AT91
// Author		: Pascal Stang - Copyright (C) 2005-2006
// Created		: 12/10/2005
// Revised		: 3/22/2006
// Version		: 0.1
// Target MCU	: Atmel AT91 series
// Editor Tabs	: 4
//
///	\ingroup driver_arm_at91
/// \defgroup i2c_at91 I2C Interface Driver for Atmel AT91 (armlib/arch/at91/i2c.c)
/// \code #include "i2c.h" \endcode
/// \par Overview
///		To be written...
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
//@{

#ifndef ARMLIB_I2C_H
#define ARMLIB_I2C_H

#include "global.h"

// return values
#define I2C_OK				0x00
#define I2C_ERROR_NODEV		0x01


void i2cInit(void);
int i2cMasterSend(u08 deviceAddr, u08 length, u08 *data);
int i2cMasterReceive(u08 deviceAddr, u08 length, u08 *data);
void i2cScanBus(void);

#endif
//@}
