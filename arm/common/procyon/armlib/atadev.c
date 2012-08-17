/*! \file atadev.c \brief IDE-ATA interface device driver. */
//*****************************************************************************
//
// File Name	: 'atadev.c'
// Title		: IDE-ATA interface device driver
// Author		: Pascal Stang
// Date			: 8/15/2004
// Revised		: 8/15/2004
// Version		: 0.1
// Target MCU	: ARMmini LPC210x
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

#include "global.h"
#include "timer.h"
#include "rprintf.h"
#include "membus.h"

#include "atadev.h"

// global variables
const DevBlock_t DevBlockAta =
{
	atadevInit,
	atadevReset,
	atadevReadReg,
	atadevWriteReg,
	atadevReadData,
	atadevWriteData
};

// functions

// initialize the interface
int atadevInit(void)
{
	membusInit();
	return 0;
}

// reset the interface
int atadevReset(void)
{
	return 0;
}

// read a value from an IDE-ATA device register
u08 atadevReadReg(u16 reg)
{
	if(reg < 0x08)
		return membusRead(ATA_REG_BASE_CS1+reg);
	else
		return membusRead(ATA_REG_BASE_CS2+reg-0x08);
}

// write a value to an IDE-ATA device register
void atadevWriteReg(u16 reg, u08 data)
{
	if(reg < 0x08)
		membusWrite(ATA_REG_BASE_CS1+reg, data);
	else
		membusWrite(ATA_REG_BASE_CS2+reg-0x08, data);
}

// read data from IDE-ATA device
int atadevReadData(u08 *buffer, int numBytes)
{
	register unsigned int i;

	uint16_t* buf = (uint16_t*)buffer;

	// read data from drive
	for (i=0; i<(numBytes/16); i++)
	{
		// optimize by reading 16 bytes in-line before looping
		*buf++ = membusRead(ATA_REG_BASE_CS1+ATA_REG_DATA);
		*buf++ = membusRead(ATA_REG_BASE_CS1+ATA_REG_DATA);
		*buf++ = membusRead(ATA_REG_BASE_CS1+ATA_REG_DATA);
		*buf++ = membusRead(ATA_REG_BASE_CS1+ATA_REG_DATA);
		*buf++ = membusRead(ATA_REG_BASE_CS1+ATA_REG_DATA);
		*buf++ = membusRead(ATA_REG_BASE_CS1+ATA_REG_DATA);
		*buf++ = membusRead(ATA_REG_BASE_CS1+ATA_REG_DATA);
		*buf++ = membusRead(ATA_REG_BASE_CS1+ATA_REG_DATA);
	}
	return numBytes;
}

// write data to IDE-ATA device
int atadevWriteData(u08 *buffer, int numBytes)
{
	register unsigned int i;

	uint16_t* buf = (uint16_t*)buffer;

	// write data to drive
	for (i=0; i<(numBytes/16); i++) 	
	{
		// optimize by writing 16 bytes in-line before looping
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
	}
	return numBytes;
}
