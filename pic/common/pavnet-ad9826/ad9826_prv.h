/**************************************************************************
 *
 * FILE NAME:			ad9826.h
 * FILE DESCRIPTION:	15MHz A2D driver private header
 * 
 * FILE CREATION DATE:	26-06-2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,26jun04 erd	written
 *
 ***************************************************************************/

#ifndef __AD9826_PRV_H_
#define __AD9826_PRV_H_

#include "ad9826.h"
#include "i2c.h"

// ==========================================================================
// types

#define AD9826_CONFIG_REG_OFFSET 			(0x0)
#define AD9826_MUX_REG_OFFSET 				(0x1)
#define AD9826_RED_PGA_REG_OFFSET 			(0x2)
#define AD9826_GREEN_PGA_REG_OFFSET 		(0x3)
#define AD9826_BLUE_PGA_REG_OFFSET 			(0x4)
#define AD9826_RED_OFFSET_REG_OFFSET 		(0x5)
#define AD9826_GREEN_OFFSET_REG_OFFSET 		(0x6)
#define AD9826_BLUE_OFFSET_REG_OFFSET 		(0x7)

// read/write mode
#define AD9826_SDATA_READ_MODE				(1)
#define AD9826_SDATA_WRITE_MODE				(0)

// test pattern written into an unused register, read back to verify
// device is communicating
#define AD9826_TEST_PATTERN					(0x015A)

// ==================================================================
// private routines 

// ad9826_cfgStartTransaction(const uint_8 readWriteMode, const uint_8 address)
//
// Starts a transaction with the device
// 
// Arguments:
//  readWriteMode: AD9826_SDATA_READ_MODE or AD9826_SDATA_WRITE_MODE
//  address: 3 bit address of register
//
// Return value:
//	None
//

extern void ad9826_cfgStartTransaction(const uint_8 readWriteMode, const uint_8 address);

// ad9826_cfgReadWord()
//
// Reads a word from the configuration memory
//
// Arguments:
//  offset: register offset to read from
//  buffer: 16bit to store read data
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

extern BOOL ad9826_cfgReadWord(const uint_8 offset, uint_16 *buffer);


// ad9826_cfgWriteWord()
//
// Writes a word to the configuration memory
//
// Arguments:
//  offset: register offset to write to
//  buffer: 16bit to write to device
//
// Return value:
//	If TRUE, successfully wrote data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly

extern BOOL ad9826_cfgWriteWord(const uint_8 offset, const uint_16 buffer);

// ad9826_test()
//
// Performs a test on the device - simply reads back a written register
// and compares it with what was previously written
//
// Return values:
//	TRUE if value matches (test passed) or FALSE if mismatch (test failed)
//
extern BOOL ad9826_test();


#endif /* __AD9826_PRV_H_ */
