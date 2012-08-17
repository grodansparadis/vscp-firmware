/**************************************************************************
 *
 * FILE NAME:			lm75.c
 * FILE DESCRIPTION:	LM75 9 bit temperature sensor driver
 *
 * FILE CREATION DATE:	03-01-2003
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,3jan04  erd written
 *
 ***************************************************************************/

#include "sysdef.h"
#include "system.h"
#include "lm75_prv.h"
#include "post.h"
#include <stdlib.h>

// ==================================================================
// lm75_readRegisters()
//
// Reads N amount of registers to buffer
//
// Arguments:
//  offset: register offset to read from
//	regCount: Number of registers to read (device will wrap if exceeds 
//            total number of registers)
//  buffer: buffer to store read data - MUST be at least regCount bytes in size   
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

BOOL lm75_readRegisters(const uint_8 offset, const uint_8 regCount, char *buffer)
{
	
	// initiate the session with the device
	if (i2c_initiateSession(LM75_I2C_ADDRESS,	// write address
							I2C_OPM_WRITE, 		// write, since we're writing a command
							FALSE) == TRUE)		// not restarting (this is the beginning)
	{
		uint_8 currentByte;
		
		// write register pointer
		if (i2c_sendByte(offset) == TRUE) 
		{
			// initiate the session with the device
			if (i2c_initiateSession(LM75_I2C_ADDRESS,	// write address
									I2C_OPM_READ, 		// write, since we're writing a command
									TRUE) == TRUE)		// restart
			{
				// start writing bytes
				for (currentByte = 0; currentByte < regCount; ++currentByte)
				{
					// read the data - stop reading at regCount
					buffer[currentByte] = i2c_readByte(currentByte == (regCount - 1));
				}
			}
		}
			
		// send stop sequence
		i2c_sendStop();
			
		// return TRUE if we wrote all registers
		return (currentByte == regCount ? TRUE : FALSE);
	}
	else
	{
		// failed to initiate return error 
		return FALSE;
	}
}

// ==================================================================
// lm75_writeRegisters()
//
// Writes N amount of registers from buffer to device
//
// Arguments:
//  offset: register offset to write to
//	regCount: Number of registers to write (device will wrap if exceeds 
//            total number of registers)
//  buffer: 1:1 mapping of registers to write
//
// Return value:
//	If TRUE, successfully wrote data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly

BOOL lm75_writeRegisters(const uint_8 offset, const uint_8 regCount, const char *buffer)
{
	// initiate the session with the device
	if (i2c_initiateSession(LM75_I2C_ADDRESS,	// write address
							I2C_OPM_WRITE, 		// write, since we're writing a command
							FALSE) == TRUE)		// not restarting (this is the beginning)
	{
		uint_8 currentByte;
		
		// write register pointer
		if (i2c_sendByte(offset) == TRUE) 
		{
			// start writing bytes
			for (currentByte = 0; currentByte < regCount; ++currentByte)
			{
				// write the data
				if (i2c_sendByte(buffer[currentByte]) == FALSE)
				{
					// we failed writing.. stop
					break;
				}
			}
		}
			
		// send stop sequence
		i2c_sendStop();
			
		// return TRUE if we wrote all registers
		return (currentByte == regCount ? TRUE : FALSE);
	}
	else
	{
		// failed to initiate return error 
		return FALSE;
	}
}

// ==================================================================
// lm75_init()
//
// Initialize the temperature sensor module
//
// Return value:
//   TRUE if initialized successfully, FALSE if failed to initialize   
//

BOOL lm75_init()
{
	// no need to do anything on initialization since we're not using
	// the lm75 os line mechanism. Just test the device and return the
	// test result
	return lm75_test();
}

// ==================================================================
// lm75_test()
//
// Read three consecutive reads of the temperature and compare results
//
// Return value:
//   TRUE if test is a success, FALSE if failed
//

BOOL lm75_test()
{
	uint_8 index;
	uint_16 prevTemp, currTemp;

	// take first 
	if (lm75_getTemp(&prevTemp) == FALSE)
	{
		// set error
		post_errorType = POST_ET_LM75_NO_RESPONSE;

		// return error
		return FALSE;
	}
	
	// run the test
    for (index = LM75_TEST_ITERATIONS; index; --index)
	{
		// take temperature
		if (lm75_getTemp(&currTemp) == FALSE)
		{
			// set error
			post_errorType = POST_ET_LM75_NO_RESPONSE;

			// return error
			return FALSE;
		}

		// check deviation
		if (abs(currTemp - prevTemp) > LM75_TEST_MAX_RESULT_DEVIATION)
		{
			// set error
			post_errorType = POST_ET_LM75_DEVIATION_TOO_GREAT;

			// return error
			return FALSE;
		}

		// current is now previous
		prevTemp = currTemp;
	}
	
	// success
	return TRUE;
}

// ==================================================================
// lm75_getTemp(uint_16 *temp)
//
// Gets the temperature (rounds to lowest decimal) 
//
// Arguments:
//	temp: pointer to a buffer which will receive the 9 bit temperature value.
//
// Return value:
//	If TRUE, successfully received date and time from IC. If FALSE, 
//  device did not respond correctly and datetime does not hold proper
//  data

BOOL lm75_getTemp(uint_16 *temp)
{
	BOOL result;
	char buffer[2];

	// just read the temperature register
	result = lm75_readRegisters(LM75_TEMP_REG_OFFSET, 2, buffer);

	// copy back to 16bit counter, swapped
	((uint_8 *)temp)[0] = buffer[1];
	((uint_8 *)temp)[1] = buffer[0];

	// shift right, as the 7 LSbs are garbage
	*temp >>= 8;

	// return the result
	return result;
}

