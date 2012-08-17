/**************************************************************************
 *
 * FILE NAME:			ad5311.h
 * FILE DESCRIPTION:	AD5311 16-bit I2C and SMBus I/O port driver
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

#include "sysdef.h"
#include "system.h"
#include "ad5311_prv.h"

// ==========================================================================
// BOOL ad5311_init()
//
// Initialize the high resolution DAC driver
//

BOOL ad5311_init()
{
	// nothing to do
	return TRUE;
}

// ==========================================================================
// BOOL ad5311_write(const uint_16 value)
//
// Set the high resolution DAC to a value
//
// Arguments:
//	value: the value to be set to DAC
//
// Return value:
//	If TRUE, receiver ACK'ed transaction. If FALSE, receiver did not ACK

BOOL ad5311_write(uint_16 value)
{
	BOOL result = FALSE;
	uint_16 *p_value = (uint_16 *)&value;

	// shift the value left two bits, since the 2 LSbs are pad
	value <<= 2;
	
	// initiate the session with the device
	if (i2c_initiateSession(AD5311_ADDRESS, 	// use static address
							I2C_OPM_WRITE, 		// write, since we're writing a command
							FALSE) == TRUE)		// not restarting (this is the beginning)
	{
		// write the command
		if (i2c_sendByte((p_value[0] & 0x3F)) == TRUE && 	// set 6 MSb
			i2c_sendByte(p_value[1]) == TRUE)				// set the 6 LSb
		{
			// all bytes sent ok
			result = TRUE;
		}
	
		// send stop sequence
		i2c_sendStop();

		// return the result
		return result;
	}
	else
	{
		// return error 
		return FALSE;
	}
}
