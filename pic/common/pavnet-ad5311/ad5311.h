/**************************************************************************
 *
 * FILE NAME:			ad5311.h
 * FILE DESCRIPTION:	AD5311 driver header
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

#ifndef __AD5311_H_
#define __AD5311_H_

#include "system.h"

// ==========================================================================
// External interface

// BOOL ad5311_init()
//
// Initialize the high resolution DAC driver
//
// Return value:
//	If TRUE, initialized successfully. FALSE if failed to init

extern BOOL ad5311_init();

// BOOL ad5311_write(const uint_16 value)
//
// Set the high resolution DAC to a value
//
// Arguments:
//	value: the value to be set to DAC
//
// Return value:
//	If TRUE, receiver ACK'ed transaction. If FALSE, receiver did not ACK

extern BOOL ad5311_write(const uint_16 value);


#endif /* __AD5311_H_ */
