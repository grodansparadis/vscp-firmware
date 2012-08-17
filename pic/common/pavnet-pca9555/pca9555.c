/**************************************************************************
 *
 * FILE NAME:			pca9555.h
 * FILE DESCRIPTION:	PCA9555 16-bit I2C and SMBus I/O port driver
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
#include "pca9555_prv.h"


// ==========================================================================
// void pca9555_init()
//
// Initialize the pca9555 driver
//
// Arguments:
//	address: 7-bit I2C address sent in I2C control byte. If this is unknown at
//			 init time, may be zero but pca9555_setAddress() MUST be called before
//			 any transactions
//

void pca9555_init(const uint_8 address)
{
	// save the address
	i2cAddress = address;
}

// ==========================================================================
// pca9555_setAddress
//
// Sets the address for the pca9555 module in the next transactions.
// MUST be called at least once before any transactions. Simply stores
// value for later use in pca9555_xxx routines.
//
// Arguments:
//	address: 7-bit I2C address sent in I2C control byte
//

void pca9555_setAddress(const uint_8 address)
{
	// just set in member
	i2cAddress = address;
}

// ==========================================================================
// BOOL pca9555_portZeroConfigure()

BOOL pca9555_portConfigure(const PCA9555_PORT port, const uint_8 mask)
{
	// initiate the session with the device
	if (i2c_initiateSession(i2cAddress, 		// use address set by pca9555_setAddress()
							I2C_OPM_WRITE, 		// write, since we're writing a command
							FALSE) == TRUE)		// not restarting (this is the beginning)
	{
		// write the command
		if (i2c_sendByte(PCA9555_CMD_CONFIG_PORT0 + port) && 	// send the 'config port' command
			i2c_sendByte(mask))									// send the configuration mask
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
// BOOL pca9555_portSetPolarity()

BOOL pca9555_portSetPolarity(const PCA9555_PORT port, const uint_8 mask)
{
	return FALSE;
}

// ==========================================================================
// uint_8 pca9555_portRead()

BOOL pca9555_portRead(const PCA9555_PORT port, uint_8 *data)
{
	// initiate the session with the device
	if (i2c_initiateSession(i2cAddress, 		// use address set by pca9555_setAddress()
							I2C_OPM_WRITE, 		// write, since we're writing a command
							FALSE) == TRUE)		// not restarting (this is the beginning)
	{
		// write the command
		if (i2c_sendByte(PCA9555_CMD_READ_PORT0 + port))
		{
			// restart the transaction and read the value of the port
			if (i2c_initiateSession(i2cAddress, I2C_OPM_READ, TRUE) == TRUE)
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
// BOOL pca9555_portWrite()

BOOL pca9555_portWrite(const PCA9555_PORT port, const uint_8 data)
{
	// initiate the session with the device
	if (i2c_initiateSession(i2cAddress, 		// use address set by pca9555_setAddress()
							I2C_OPM_WRITE, 		// write, since we're writing a command
							FALSE) == TRUE)		// not restarting (this is the beginning)
	{
		// write the command
		if (i2c_sendByte(PCA9555_CMD_WRITE_PORT0 + port) && 	// send the 'write port' command
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

