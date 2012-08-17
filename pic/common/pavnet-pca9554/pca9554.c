/**************************************************************************
 *
 * FILE NAME:			pca9554.h
 * FILE DESCRIPTION:	PCA9554 16-bit I2C and SMBus I/O port driver
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

#include "sysdef.h"
#include "system.h"
#include "pca9554_prv.h"

// ==========================================================================
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
//

BOOL pca9554_portConfigure(const uint_8 mask)
{
	// initiate the session with the device
	if (i2c_initiateSession(PCA9554_I2C_ADDRESS, 	// use address set by pca9554_setAddress()
							I2C_OPM_WRITE, 			// write, since we're writing a command
							FALSE) == TRUE)			// not restarting (this is the beginning)
	{
		// write the command
		if (i2c_sendByte(PCA9554_CMD_CONFIG_PORT) && 	// send the 'config port' command
			i2c_sendByte(mask))							// send the configuration mask
		{
			// send stop sequence
			i2c_sendStop();
			
			// all bytes sent ok
			return TRUE;
		}
	}

	// return error 
	return FALSE;
}

// ==========================================================================
// pca9554_portSetPolarity
//
// Currently not supported
//

BOOL pca9554_portSetPolarity(const uint_8 mask)
{
	return FALSE;
}

// ==========================================================================
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
//

BOOL pca9554_portRead(uint_8 *data)
{
	// initiate the session with the device
	if (i2c_initiateSession(PCA9554_I2C_ADDRESS, 	// use address set by pca9554_setAddress()
							I2C_OPM_WRITE, 			// write, since we're writing a command
							FALSE) == TRUE)			// not restarting (this is the beginning)
	{
		// write the command
		if (i2c_sendByte(PCA9554_CMD_READ_PORT))
		{
			// restart the transaction and read the value of the port
			if (i2c_initiateSession(PCA9554_I2C_ADDRESS, I2C_OPM_READ, TRUE) == TRUE)
			{
				// now, read the value of the port
				*data = i2c_readByte(TRUE);

				// return ok
				return TRUE;
			}
		}
	}

	// return error
	return FALSE;
}

// ==========================================================================
// pca9554_portWrite
//
// Write data to the output pins
//
// Arguments:
//	data: 8bit value to be written to the output ports
//
// Return values:
//	TRUE if write operation succeeded, FALSE otherwise
//

BOOL pca9554_portWrite(const uint_8 data)
{
	// initiate the session with the device
	if (i2c_initiateSession(PCA9554_I2C_ADDRESS, 	// use address set by pca9554_setAddress()
							I2C_OPM_WRITE, 			// write, since we're writing a command
							FALSE) == TRUE)			// not restarting (this is the beginning)
	{
		// write the command
		if (i2c_sendByte(PCA9554_CMD_WRITE_PORT) && 			// send the 'write port' command
			i2c_sendByte(data))									// send the configuration mask
		{
			// send stop sequence
			i2c_sendStop();
			
			// all bytes sent ok
			return TRUE;
		}
	}

	// return error 
	return FALSE;
}

