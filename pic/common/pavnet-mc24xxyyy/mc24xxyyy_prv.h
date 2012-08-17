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

#ifndef __MC24XXYYY_PRV_H_
#define __MC24XXYYY_PRV_H_

#include "mc24xxyyy.h"

// ==================================================================
// constants

// number of bytes in each page (varies between chip types)
#define MC24XXYYY_PAGE_SIZE					(128)

// number of cycles to wait for write completion
#define MC24XXYYY_WRITE_WAIT_CYCLES			(0xFFFF)

// I2C address
#define MC24XXYYY_I2C_ADDRESS				(0B1010000)

// ==================================================================
// private routines 

// mc24xxyyy_readBytes()
//
// Reads a byte from EEPROM device to buffer
//
// Arguments:
//  address: address of byte to read
//  buffer: one byte buffer read from device
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

extern BOOL mc24xxyyy_readByte(const uint_16 address, char *buffer);


// mc24xxyyy_writeByte()
//
// Writes a byte to EEPROM
//
// Arguments:
//  address: address of byte to read
//  buffer: one byte buffer to write to device
//
// Return value:
//	If TRUE, successfully wrote data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly

extern BOOL mc24xxyyy_writeByte(const uint_16 address, char buffer);


// mc24xxyyy_writePage()
//
// Writes a page from buffer to EEPROM device . Note that if the buffer size exceeds the
// page boundaries, the data will wrap to current page. To do this, use the mc24xxyyy_write routine.
// It is slower, but it will handle inter-page transactions.
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

extern BOOL mc24xxyyy_writePage(const uint_16 address, char *buffer, uint_16 bufferSize);


// mc24xxyyy_initializeSession()
//
// Initializes a session with the mc24xxyyy. The device might be busy
// with a write cycle and at this stage it won't ACK. Waits a certain 
// MC24XXYYY_WRITE_WAIT_CYCLES of cycles for write to complete. If after
// this the device didn't respond, we assume failure
//
// Return value:
//	If TRUE, data was successfully written to device and device is ready for operation
//  If FALSE, device timed out on write

extern BOOL mc24xxyyy_initializeSession();


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


#endif /* __MC24XXYYY_PRV_H_ */
