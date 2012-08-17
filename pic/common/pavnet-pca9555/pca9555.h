/**************************************************************************
 *
 * FILE NAME:			pca9555.h
 * FILE DESCRIPTION:	PCA9555 driver header
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

#ifndef __PCA9555_H_
#define __PCA9555_H_

#include "system.h"

// ==================================================================
// Types

// port types
typedef enum
{
	PCA9555_PORT0 = 0,
	PCA9555_PORT1

} PCA9555_PORT;

// ==========================================================================
// External interface

// void pca9555_init()
//
// Initialize the pca9555 driver
//
// Arguments:
//	address: 7-bit I2C address sent in I2C control byte. If this is unknown at
//			 init time, may be zero but pca9555_setAddress() MUST be called before
//			 any transactions
//
extern void pca9555_init(const uint_8 address);


// pca9555_setAddress
//
// Sets the address for the pca9555 module in the next transactions.
// MUST be called at least once before any transactions. Simply stores
// value for later use in pca9555_xxx routines.
//
// Arguments:
//	address: 7-bit I2C address sent in I2C control byte
//
extern void pca9555_setAddress(const uint_8 address);


// pca9555_checkInterrupt
//
// Check the interrupt line
//
// Return values:
//	TRUE if iterrupt line is high, FALSE otherwise
#define pca9555_checkInterrupt() (IO_PCA9555_INT)

// void pca9555_portZeroConfigure()
extern BOOL pca9555_portConfigure(const PCA9555_PORT port, const uint_8 mask);

// void pca9555_portZeroConfigure()
extern BOOL pca9555_portSetPolarity(const PCA9555_PORT port, const uint_8 mask);

// void pca9555_portRead()
extern BOOL pca9555_portRead(const PCA9555_PORT port, uint_8 *data);

// void pca9555_portWrite()
extern BOOL pca9555_portWrite(const PCA9555_PORT port, const uint_8 data);


#endif /* __PCA9555_H_ */
