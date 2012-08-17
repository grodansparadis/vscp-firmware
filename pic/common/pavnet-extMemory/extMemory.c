/**************************************************************************
 *
 * FILE NAME:			extMemory.h
 * FILE DESCRIPTION:	External memory driver implementation
 *
 * FILE CREATION DATE:	3-12-2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,3dec04 erd written
 *
 ***************************************************************************/

#include "sysdef.h"
#include "extMemory_prv.h"
#include "post.h"

// ==================================================================
// extmem_init
//
// Initialize external memory
//
// Return values:
//	TRUE if success, FALSE otherwise

BOOL extmem_init()
{
	// set device to word write mode. In this mode the PIC will output
	// data only when writes to odd addresses are performed. When a write
	// to an even address occurs - the data is latched and 'waits' for a write
	// to an odd address
	WM1 = 1;
	WM0 = 0;

	// reset upper table pointer
	TBLPTRU = 0;

	// test the memory
	return extmem_ramTest();
}

// ==================================================================
// extmem_ramFillAndVerify
//
// Fill the RAM with a pattern and verify it
//
// Return values:
//	TRUE if success, FALSE otherwise

BOOL extmem_ramFillAndVerify(const uint_16 pattern)
{
	uint_32 ramOffset;
	uint_16 ramReadbackData;
	BOOL result = TRUE;

	// iterate through the RAM and fill the pattern
	for (ramOffset = 0; ramOffset < EXTMEM_RAM_SIZE; ramOffset += 2)
	{
		// set data
		extmem_ramWrite(ramOffset, pattern);
	}

	// read back the data
	for (ramOffset = 0; ramOffset < EXTMEM_RAM_SIZE; ramOffset += 2)
	{
		// read data
		ramReadbackData = extmem_ramRead(ramOffset);

		// verify the pattern
		if (ramReadbackData != pattern)
		{
			// set error
			post_errorType = POST_ET_EXTERNAL_RAM_FAIL;
			
			// set error and cleanup
			result = FALSE;
			goto cleanup;
		}
	}

// clean up and return result
cleanup:

	// reset upper table pointer
	TBLPTRU = 0;

	// if we got here, all the bytes verified. success
	return result;
}

// ==================================================================
// extmem_ramTest
//
// Test external RAM
//
// Return values:
//	TRUE if success, FALSE otherwise

BOOL extmem_ramTest()
{
	// fill with AA and 55
	return extmem_ramFillAndVerify(0xAAAA) && extmem_ramFillAndVerify(0x5555);
}

// ==================================================================
// extmem_ramRead
//
// Read external RAM
//
// Arguments:
//	offset: offset into RAM (must be aligned to 16 bit addresses)
//
// Return values:
//	16bit value read from address

uint_16 extmem_ramRead(const uint_32 offset)
{
	// set the pointer
	extMemPtr = (far uint_16 *)(EXTMEM_RAM_BASE_ADDRESS + offset);
	
	// reset upper table pointer
	TBLPTRU = 0;

	// return the data
	return *extMemPtr;
}

// ==================================================================
// extmem_ramWrite
//
// Write to external RAM
//
// Arguments:
//	offset: offset into RAM (must be aligned to 16 bit addresses)
//	data: data to be written to address
//
// Return values:
//	TRUE if success, FALSE otherwise

void extmem_ramWrite(const uint_32 offset, const uint_16 data)
{
	// set the pointer
	extMemPtr = (far uint_16 *)(EXTMEM_RAM_BASE_ADDRESS + offset);
	
	// reset upper table pointer
	TBLPTRU = 0;

	// do the write
	*extMemPtr = (uint_16)data;
}

// ==================================================================
// extmem_romTest
//
// Test external ROM
//
// Return values:
//	TRUE if success, FALSE otherwise

BOOL extmem_romTest()
{
	// TODO
	return TRUE;
}

// ==================================================================
// extmem_romWriteDirect
//
// Write data directly out on the bus
//

void extmem_romWriteDirect(const uint_32 address, const uint_16 data)
{
	// set the pointer
	extMemPtr = (far uint_16 *)(EXTMEM_ROM_BASE_ADDRESS + address);
	
	// reset upper table pointer
	TBLPTRU = 0;

	// do the write
	*extMemPtr = (uint_16)data;
}

// ==================================================================
// extmem_romReadDirect
//
// Write a command to the external ROM (unlocks and writes)
//

uint_16 extmem_romReadDirect(const uint_32 address)
{
	// set the pointer
	extMemPtr = (far uint_16 *)(EXTMEM_ROM_BASE_ADDRESS + address);
	
	// reset upper table pointer
	TBLPTRU = 0;

	// return the data
	return *extMemPtr;
}

// ==================================================================
// extmem_romWriteCmd
//
// Write a command to the external ROM (unlocks and writes)
//
// Arguments:
//	cmd: Command to write to device after unlocking it. If cmd
//		 is EXTMEM_ROM_CMD_NONE, only unlocking is performed
//

void extmem_romWriteCmd(const uint_16 cmd)
{
	// write first unlock
	extmem_romWriteDirect(EXTMEM_ROM_UNLOCK_ADDR_1, EXTMEM_ROM_UNLOCK_DATA_1);

	// write second unlock
	extmem_romWriteDirect(EXTMEM_ROM_UNLOCK_ADDR_2, EXTMEM_ROM_UNLOCK_DATA_2);

	// check if there was a command to be write
	if (cmd != EXTMEM_ROM_CMD_NONE)
	{
		// write the command	
		extmem_romWriteDirect(EXTMEM_ROM_UNLOCK_ADDR_1, cmd);
	}
}

// ==================================================================
// extmem_romRead
//
// Read external ROM
//
// Arguments:
//	offset: offset into ROM (must be aligned to 16 bit addresses)
//
// Return values:
//	16bit value read from address

uint_16 extmem_romRead(const uint_32 offset)
{
	// just read directly
	return extmem_romReadDirect(offset);
}

// ==================================================================
// extmem_romWrite
//
// Write to external ROM
//
// Arguments:
//	offset: offset into ROM (must be aligned to 16 bit addresses)
//	buffer: pointer to buffer being written
//	bufferLength: length of buffer, in bytes. Writes will always be 2 byte aligned
//
// Return values:
//	TRUE if success, FALSE otherwise

BOOL extmem_romWrite(const uint_32 offset, const uint_8 *buffer, const uint_16 bufferLength)
{
	uint_16 waitCtr = 1, *p_data = (uint_16 *)buffer, bytesWritten = 0;
	
	// disable interrupts
	sys_disableInterrupts();
	
	// do the write while there is data left to write and previous write was
	// verified
	while (bytesWritten < bufferLength && waitCtr)
	{
		// prepare write 
		extmem_romWriteCmd(EXTMEM_ROM_CMD_WRITE);

		// do the write
		extmem_romWriteDirect(offset + bytesWritten, *p_data);

		// make sure data was written
		for (waitCtr = EXTMEM_ROM_WRITE_TIMEOUT_MS; waitCtr; --waitCtr)
		{
			// check if data was written
			if (extmem_romRead(offset + bytesWritten) == *p_data)
			{
				// stop waiting
				break;
			}
		}
        
		// increment pointer and decrement bytes left
		p_data++;
		bytesWritten += 2;
	}

	// enable interrupts
	sys_enableInterrupts();

	// return if success - if a write failed, waitCtr will be equal to 0
	// as the timeout expired
	return (waitCtr);
}

// ==================================================================
// extmem_romErase
//
// Erase entire ROM
//
// Return values:
//	TRUE if success, FALSE otherwise

BOOL extmem_romErase()
{
	uint_16 waitCtr;
	
	// disable interrupts
	sys_disableInterrupts();
	
    // do the erase
	extmem_romWriteCmd(EXTMEM_ROM_CMD_ERASE);
	extmem_romWriteCmd(EXTMEM_ROM_CMD_CHIP_ERASE);

	// wait until the first address is FFFF which indicates erase completion, or a timeout occurs
	for (waitCtr = EXTMEM_ROM_CHIP_ERASE_TIMEOUT_MS; waitCtr; --waitCtr)
	{
		// check
		if (extmem_romRead(0) == 0xFFFF)
		{
			// stop waiting
			break;
		}

		// wait 1 ms
		time_delayMs(100);
	}
	
	// enable interrupts
	sys_enableInterrupts();

	// return if success - if erase failed, waitCtr will be equal to 0
	// as the timeout expired
	return (waitCtr);
}
// ==================================================================
// extmem_romManufIdRead
//
// Get the ROM manufacture ID
//
// Return values:
//	16 bit manufacture ID

uint_16 extmem_romManufIdRead()
{
	uint_16 manufId;
	
	// disable interrupts
	sys_disableInterrupts();
	
    // do the autoselect
	extmem_romWriteCmd(EXTMEM_ROM_AUTOSELECT);
	
	// read the device ID
	manufId = extmem_romReadDirect(0);

	// exit autoselect mode
	extmem_romWriteCmd(0xF0);
	extmem_romWriteCmd(0xF0);

	// enable interrupts
	sys_enableInterrupts();

	// return the device ID
	return manufId;
}


