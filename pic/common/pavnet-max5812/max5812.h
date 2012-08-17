/**************************************************************************
 *
 * FILE NAME:			max5812.h
 * FILE DESCRIPTION:	MAX5812 driver header
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
 * 01a,23nov03 erd
 *
 ***************************************************************************/

#ifndef __MAX5812_H_
#define __MAX5812_H_

#include "system.h"

// ==========================================================================
// External interface

// BOOL max5812_init()
//
// Initialize the high resolution DAC driver
//
// Return value:
//	If TRUE, initialized successfully. FALSE if failed to init

extern BOOL max5812_init();

// BOOL max5812_write(const uint_16 value)
//
// Set the high resolution DAC to a value
//
// Arguments:
//	value: the value to be set to DAC
//
// Return value:
//	If TRUE, receiver ACK'ed transaction. If FALSE, receiver did not ACK

extern BOOL max5812_write(const uint_16 value);


#endif /* __MAX5812_H_ */
