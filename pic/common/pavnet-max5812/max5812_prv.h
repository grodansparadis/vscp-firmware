/**************************************************************************
 *
 * FILE NAME:			max5812_prv.h
 * FILE DESCRIPTION:	MAX5812 driver private header
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

#ifndef __MAX5812_PRV_H_
#define __MAX5812_PRV_H_

#include "max5812.h"
#include "i2c.h"

// ==========================================================================
// constants

// write modes
#define MAX5812_WRITE_AND_UPDATE		(0B11000000)
#define MAX5812_WRITE_TO_INPUT_REG		(0B11010000)
#define MAX5812_WRITE_AND_UPDATE_LAST	(0B11100000)
#define MAX5812_UPDATE_FROM_INPUT		(0B11110000)

// I2C address
#define MAX5812_ADDRESS (0B00100000)

#endif /* __MAX5812_PRV_H_ */
