/**************************************************************************
 *
 * FILE NAME:			eeprom.c
 * FILE DESCRIPTION:	EEPROM for the PIC1687x
 *
 * FILE CREATION DATE:	26-05-2003
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,26may03 erd
 *
 ***************************************************************************/

#include "sysdef.h"
#include "eeprom_prv.h"

// ==========================================================================
// eeprom_init()
//
// Perform the EEPROM initialization
//

void eeprom_init()
{
}

// ==========================================================================
// eeprom_writeBuffer(uint_8 *buffer, uint_8 size,  uint_8 offset)
//
// Write a buffer to the eeprom
//
// Arguments:
//	buffer: pointer to the buffer to be written to the eeprom
//	size: length, in bytes, of the buffer
//	offset: offset into the eeprom memory to write (0 = start of eeprom)

EEPROM_STATUS eeprom_writeBuffer(uint_8 *buffer, uint_8 size,  uint_8 offset)
{
	// read all bytes of the structure from eeprom to RAM
	for (; size; --size, ++offset, ++buffer)
	{
		// write the character from buffer to the eeprom
		eeprom_write(offset, (*buffer));
	}

	// success
	return EEPROM_OK;
}

// ==========================================================================
// eeprom_readBuffer( uint_8 offset, uint_8 *buffer, uint_8 size)
//
// Read data from the eeprom
//
// Arguments:
//	buffer: pointer to the buffer to which eeprom contents will be written
//	size: number of bytes to read from eeprom to buffer
//	offset: offset into the eeprom memory to read (0 = start of eeprom)

EEPROM_STATUS eeprom_readBuffer( uint_8 offset, uint_8 *buffer, uint_8 size)
{
	// read all bytes of the structure from eeprom to RAM
	for (; size; --size, ++offset, ++buffer)
	{
		// read the character from eeprom to the buffer
		(*buffer) = eeprom_read(offset);
	}

	// success
	return EEPROM_OK;
}
