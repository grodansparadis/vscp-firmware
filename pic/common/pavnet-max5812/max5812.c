/**************************************************************************
 *
 * FILE NAME:			max5812.h
 * FILE DESCRIPTION:	MAX5812 16-bit I2C and SMBus I/O port driver
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
#include "max5812_prv.h"

// ==========================================================================
// BOOL max5812_init()
//
// Initialize the high resolution DAC driver
//

BOOL max5812_init()
{
	// nothing to do
	return TRUE;
}

// ==========================================================================
// BOOL max5812_write(const uint_16 value)
//
// Set the high resolution DAC to a value
//
// Arguments:
//	value: the value to be set to DAC
//
// Return value:
//	If TRUE, receiver ACK'ed transaction. If FALSE, receiver did not ACK

BOOL max5812_write(const uint_16 value)
{
	uint_8 *p_value = (uint_8 *)&value;
	BOOL result = FALSE;
	
	// initiate the session with the device
	if (i2c_initiateSession(MAX5812_ADDRESS, 	// use static address
							I2C_OPM_WRITE, 		// write, since we're writing a command
							FALSE) == TRUE)		// not restarting (this is the beginning)
	{
		// write the command
		if (i2c_sendByte(MAX5812_WRITE_AND_UPDATE | (p_value[0] & 0xF)) == TRUE && 	// write and update, and set the 4 MSb
			i2c_sendByte(p_value[1]) == TRUE)										// set the 8 LSb
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
