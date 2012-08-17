/*! \file po1030.h \brief PO1030 Imager Interface Driver. */
//*****************************************************************************
//
// File Name	: 'po1030.h'
// Title		: PO1030 Imager Interface Driver
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
#include "i2c.h"
#include "po1030.h"

// functions

void po1030Init(void)
{

}

u16 po1030GetID(void)
{
	return po1030RegRead(PO1030_REG_DEVID);
}

// read/write register
u16 po1030RegRead(u08 regaddr)
{
	u08 packet[2];
	// set the register to access
	packet[0] = (regaddr & PO1030_REG_MASK);
	i2cMasterSend(PO1030_I2C_BASE_ADDR, 1, &packet[0]);
	// read the value
	if(regaddr&PO1030_REG_16BIT)
	{
		i2cMasterReceive(PO1030_I2C_BASE_ADDR, 2, &packet[0]);
		// return value
		return (packet[0]<<8)|packet[1];
	}
	else
	{
		i2cMasterReceive(PO1030_I2C_BASE_ADDR, 1, &packet[0]);
		// return value
		return packet[0];
	}
}

void po1030RegWrite(u08 regaddr, u16 value)
{
	u08 packet[3];
	// set the register to access
	packet[0] = (regaddr & PO1030_REG_MASK);
	// read the value
	if(regaddr&PO1030_REG_16BIT)
	{
		// set value to write
		packet[1] = (value>>8);
		packet[2] = (value);
		// write the value
		i2cMasterSend(PO1030_I2C_BASE_ADDR, 3, &packet[0]);
	}
	else
	{
		// set value to write
		packet[1] = (value);
		// write the value
		i2cMasterSend(PO1030_I2C_BASE_ADDR, 2, &packet[0]);
	}
}



