/**************************************************************************
 *
 * FILE NAME:			mc24xxyyy.h
 * FILE DESCRIPTION:	MC24XXYYY I2C EEPROM device
 *
 * FILE CREATION DATE:	12/02/2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,12feb04 erd	written
 *
 ***************************************************************************/

#ifndef __MC24XXYYY_H_
#define __MC24XXYYY_H_

// ==================================================================
// constants

#define MC24XXYYY_SIZE			(64UL * 1024UL)			// size of eeprom in bytes

// ==================================================================
// interfaces

// mc24xxyyy_init()
//
// Initialize the real-time clock module
//
// Return value:
//   TRUE if initialized successfully, FALSE if failed to initialize   
//

extern BOOL mc24xxyyy_init();

// mc24xxyyy_read()
//
// Reads a data from EEPROM device to buffer. Note that it currently 
// only 255 bytes of data can be read at a time. If this changes, just
// change bufferSize to uint_16
//
// Arguments:
//  address: Address of page
//  buffer: buffer to store data
//  bufferSize: Amount of bytes to read
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

extern BOOL mc24xxyyy_read(const uint_16 address, char *buffer, uint_16 bufferSize);

// mc24xxyyy_write()
//
// Write to random address of the mc24xxyyy. Since the device has an
// internal write buffer of size MC24XXYYY_PAGE_SIZE, it is not possible
// to write more than a page size of data at a time. Moreover, it is not
// possible to write data which resides at addresses on different pages.
// Therefore, this routine splits up the write request to descrete page
// writes.
//
// Arguments:
//  address: Address of page
//  buffer: buffer to store data
//  bufferSize: Amount of bytes to write
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

BOOL mc24xxyyy_write(uint_16 address, char *buffer, uint_16 bufferSize);


#endif /* __MC24XXYYY_H_ */
