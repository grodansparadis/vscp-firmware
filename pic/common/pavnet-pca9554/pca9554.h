/**************************************************************************
 *
 * FILE NAME:			pca9554.h
 * FILE DESCRIPTION:	PCA9554 driver header
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
 * 02a,3jan04  erd 	adapted from pca9555 to pca9554
 * 01a,26may03 erd	written
 *
 ***************************************************************************/

#ifndef __PCA9554_H_
#define __PCA9554_H_

#include "system.h"

// ==========================================================================
// External interface

// pca9554_checkInterrupt
//
// Check the interrupt line
//
// Return values:
//	TRUE if iterrupt line is high, FALSE otherwise
#define pca9554_checkInterrupt() (0)

// pca9554_portConfigure
//
// Configure the output pins to input / output
//
// Arguments:
//	mask: 8bit input/output mask where LSb configures bit 0. 
//		  input = 1, output 0.
//
// Return values:
//	TRUE if iterrupt line is high, FALSE otherwise

extern BOOL pca9554_portConfigure(const uint_8 mask);

// pca9554_portSetPolarity
//
// Currently not supported
//

extern BOOL pca9554_portSetPolarity(const uint_8 mask);

// pca9554_portRead
//
// Read the value of the ports
//
// Arguments:
//	data: Pointer to an 8bit variable which will hold the data
//		returned by the device. If the read fails, the value
//		of this variable remains unchanged.
//
// Return values:
//	TRUE if read operation succeeded, FALSE otherwise

extern BOOL pca9554_portRead(uint_8 *data);

// pca9554_portWrite
//
// Write data to the output pins
//
// Arguments:
//	data: 8bit value to be written to the output ports
//
// Return values:
//	TRUE if write operation succeeded, FALSE otherwise

extern BOOL pca9554_portWrite(const uint_8 data);


#endif /* __PCA9554_H_ */
