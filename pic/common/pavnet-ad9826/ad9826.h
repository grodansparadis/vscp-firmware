/**************************************************************************
 *
 * FILE NAME:			ad9826.h
 * FILE DESCRIPTION:	15MHz A2D driver header
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

#ifndef __AD9826_H_
#define __AD9826_H_

#include "system.h"

// ==========================================================================
// types

typedef enum
{
	AD9826_SS_STARTED,	
	AD9826_SS_STOPPED

} AD9826_SAMPLING_STATE;

// ==========================================================================
// External interface

// ad9826_init()
//
// Initialize the temperature sensor module
//
// Return value:
//   TRUE if initialized successfully, FALSE if failed to initialize   
//

extern BOOL ad9826_init();

// ad9826_sample()
//
// Read a sample
//
// Arguments:
//  sample: 16 bit sample result
//
// Return value:
//	If TRUE, successfully read data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly

extern BOOL ad9826_sample(uint_16 *sample);

// ad9826_setSampleState(const AD9826_SAMPLING_STATE state)
//
// Perform all initializations and de-initializations required to reduce
// operations in the sampling loop
//
// Arguments:
//  state: AD9826_SS_STARTED will initialize and prepare for repeated samples
//			while AD9826_SS_STOPPED will de-initialize and return to normal
//			operation
//
// Return value:
//	If TRUE, successfully read data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly

extern BOOL ad9826_setSampleState(const AD9826_SAMPLING_STATE state);

// ad9826_setPowerDown(const BOOL state)
//
// Sets power down mode
//
// Arguments:
//  state: TRUE to power down, FALSE to activate
//

extern void ad9826_setPowerDown(const BOOL state);

// ad9826_setGain(const float gain)
//
// Set A2D gain
//
// Arguments:
//	gain: required gain. Values from 1.0 - 6.0 are valid
//
// Return value:
//	If TRUE, successfully wrote data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly
//

extern BOOL ad9826_setGain(const float gain);

// ad9826_setOffset(const int_16 offset)
//
// Set A2D offset
//
// Arguments:
//	offset: injected offset (-300 - 300)
//
// Return value:
//	If TRUE, successfully wrote data to IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly
//

extern BOOL ad9826_setOffset(const int_16 offset);

// ad9826_test()
//
// Performs a test on the device - simply reads back a written register
// and compares it with what was previously written
//
// Return values:
//	TRUE if value matches (test passed) or FALSE if mismatch (test failed)
//
extern BOOL ad9826_test();

// ad9826_sampleFast()
//
// High performance sampling
//
// Arguments:
//	hi: most significant byte of 16bit sample
//	lo: least significant byte of 16bit sample
//
#define ad9826_sampleFast(lo, hi)	\
		BIT_SET(IO_A2D_CDSCLK2);	\
		BIT_CLR(IO_A2D_CDSCLK2);	\
		BIT_SET(IO_A2D_ADCLK);		\
		asm("nop");					\
		*hi = IO_DBUS_PORT;			\
		BIT_CLR(IO_A2D_ADCLK);		\
		asm("nop");					\
		*lo = IO_DBUS_PORT;


#endif /* __AD9826_H_ */
