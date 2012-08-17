/**************************************************************************
 *
 * FILE NAME:			lm75_prv.h
 * FILE DESCRIPTION:	LM75 9 bit temperature sensor driver private header
 * 
 * FILE CREATION DATE:	03-01-2003
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 02a,3jan04  erd 	adapted from pca9555 to lm75
 * 01a,26may03 erd	written
 *
 ***************************************************************************/

#ifndef __LM75_PRV_H_
#define __LM75_PRV_H_

#include "lm75.h"
#include "i2c.h"

// ==========================================================================
// types

#define LM75_TEMP_REG_OFFSET 		(0x0)
#define LM75_CONFIG_REG_OFFSET 		(0x1)
#define LM75_THYST_REG_OFFSET 		(0x2)
#define LM75_TOS_REG_OFFSET 		(0x3)

// I2C address
#define LM75_I2C_ADDRESS			(0B01001000)

// test configuration
#define LM75_TEST_MAX_RESULT_DEVIATION	(10)	// temperature test max result deviation in celsius
#define LM75_TEST_ITERATIONS			(5)		// temperature test iteration count


// ==================================================================
// private routines 

// lm75_readRegisters()
//
// Reads N amount of registers to buffer
//
// Arguments:
//  offset: register offset to read from
//	regCount: Number of registers to read (device will wrap if exceeds 
//            total number of registers)
//  buffer: buffer to store read data - MUST be at least regCount bytes in size   
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

extern BOOL lm75_readRegisters(const uint_8 offset, const uint_8 regCount, char *buffer);


// lm75_writeRegisters()
//
// Writes N amount of registers from buffer to device
//
// Arguments:
//  offset: register offset to write to
//	regCount: Number of registers to write (device will wrap if exceeds 
//            total number of registers)
//  buffer: 1:1 mapping of registers to write
//
// Return value:
//	If TRUE, successfully wrote data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly

extern BOOL lm75_writeRegisters(const uint_8 offset, const uint_8 regCount, const char *buffer);


#endif /* __LM75_PRV_H_ */
