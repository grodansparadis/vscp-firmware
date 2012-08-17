/**************************************************************************
 *
 * FILE NAME:			pca9554_prv.h
 * FILE DESCRIPTION:	PCA9554 driver private header
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

#ifndef __PCA9554_PRV_H_
#define __PCA9554_PRV_H_

#include "pca9554.h"
#include "i2c.h"

// ==========================================================================
// types

// Command type
typedef enum
{
	PCA9554_CMD_READ_PORT 		= 0,	// read data from port
	PCA9554_CMD_WRITE_PORT		= 1,	// write data to port
	PCA9554_CMD_POLINVERT_PORT	= 2,	// invert the polarity on port
	PCA9554_CMD_CONFIG_PORT		= 3,	// configure port

} PCA9554_COMMAND_TYPE;

// I2C address
#define PCA9554_I2C_ADDRESS		(0B00100000)

#endif /* __PCA9554_PRV_H_ */
