/**************************************************************************
 *
 * FILE NAME:			pca9555_prv.h
 * FILE DESCRIPTION:	PCA9555 driver private header
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

#ifndef __PCA9555_PRV_H_
#define __PCA9555_PRV_H_

#include "pca9555.h"
#include "i2c.h"

// ==========================================================================
// types

// Command type
typedef enum
{
	PCA9555_CMD_READ_PORT0 		= 0,	// read data from port 0
	PCA9555_CMD_READ_PORT1		= 1,	// read data from port 1
	PCA9555_CMD_WRITE_PORT0		= 2,	// write data to port 0
	PCA9555_CMD_WRITE_PORT1		= 3,	// write data to port 1
	PCA9555_CMD_POLINVERT_PORT0	= 4,	// invert the polarity on port 0
	PCA9555_CMD_POLINVERT_PORT1 = 5,	// invert the polarity on port 1
	PCA9555_CMD_CONFIG_PORT0	= 6,	// configure port 0
	PCA9555_CMD_CONFIG_PORT1	= 7		// configure port 1

} PCA9555_COMMAND_TYPE;

// I2C address
static uint_8 i2cAddress = 0;

#endif /* __PCA9555_PRV_H_ */
