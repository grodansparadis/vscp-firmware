/**************************************************************************
 *
 * FILE NAME:			mcp3221.h
 * FILE DESCRIPTION:	MCP3221 (High-res A2D) driver interface
 *
 * FILE CREATION DATE:	23-11-2003
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

#include "sysdef.h"
#include "mcp3221_prv.h"
#include "i2c.h"

// ==================================================================
// mcp3221_init()
//
// Initialize the high resolution A2D module
//
// Return value:
//   TRUE if initialized successfully, FALSE if failed to initialize   
//

BOOL mcp3221_init()
{
    // nothing to do, return ok
    return TRUE;
}

// ==================================================================
// mcp3221_read()
//
// Performs A2D 
//
// Arguments:
//	value: Value to be filled out with acquired value. 12 bit wide
//
// Return value:
//	TRUE if acquisition ok, FALSE if failed

BOOL mcp3221_read(uint_16 *value)
{
	// point to value
	uint_8 *p_value = (uint_8 *)value;
	BOOL result = FALSE;

	// initiate the session with the device
	if (i2c_initiateSession(MCP3221_I2C_ADDRESS,	// write address
							I2C_OPM_READ, 		// write, since we're writing a command
							FALSE) == TRUE)		// not restarting (this is the beginning)
	{
		// read register pointer
		if ((p_value[0] = i2c_readByte(FALSE)) == TRUE && (p_value[1] = i2c_readByte(TRUE)) == TRUE)
		{
			// set result to success
			result = TRUE;
		}

		// send stop
		i2c_sendStop();

		// return the result
		return result;
	}
	else
	{
		// failed to initiate
		return FALSE;
	}
}

