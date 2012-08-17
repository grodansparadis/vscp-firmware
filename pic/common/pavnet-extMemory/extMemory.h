/**************************************************************************
 *
 * FILE NAME:			extMemory.h
 * FILE DESCRIPTION:	External memory driver header
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

#ifndef __EXT_MEM_H_
#define __EXT_MEM_H_

#include "system.h"

// ==========================================================================
// constants and types

// RAM memory info                  		
#define EXTMEM_RAM_BASE_ADDRESS				(0x10000L)
#define EXTMEM_RAM_SIZE						(0x1000L)
		
// ROM memory info                  		
#define EXTMEM_ROM_BASE_ADDRESS				(0x100000L)
#define EXTMEM_ROM_SIZE						(0x1FFFFL)

// ROM manufacture ID
#define EXTMEM_ROM_MANUF_ID_AMD				(0x0001)

// ==========================================================================
// External interface

// extmem_init
//
// Initialize external memory
//
// Return values:
//	TRUE if success, FALSE otherwise

extern BOOL extmem_init();

// extmem_ramTest
//
// Test external RAM
//
// Return values:
//	TRUE if success, FALSE otherwise

extern BOOL extmem_ramTest();

// extmem_ramRead
//
// Read external RAM
//
// Arguments:
//	offset: offset into RAM (must be aligned to 16 bit addresses)
//
// Return values:
//	16bit value read from address

extern uint_16 extmem_ramRead(const uint_32 offset);

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

extern void extmem_ramWrite(const uint_32 offset, const uint_16 data);

// extmem_romTest
//
// Test external ROM
//
// Return values:
//	TRUE if success, FALSE otherwise

extern BOOL extmem_romTest();

// extmem_romRead
//
// Read external ROM
//
// Arguments:
//	offset: offset into ROM (must be aligned to 16 bit addresses)
//
// Return values:
//	16bit value read from address

extern uint_16 extmem_romRead(const uint_32 offset);

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

extern BOOL extmem_romWrite(const uint_32 offset, const uint_8 *buffer, const uint_16 bufferLength);

// extmem_romErase
//
// Erase entire ROM
//
// Return values:
//	TRUE if success, FALSE otherwise

extern BOOL extmem_romErase();

// extmem_romManufIdRead
//
// Get the ROM manufacture ID
//
// Return values:
//	16 bit manufacture ID

extern uint_16 extmem_romManufIdRead();


#endif /* __PCA9554_H_ */
