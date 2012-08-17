/**************************************************************************
 *
 * FILE NAME:			mc24xxyyy.h
 * FILE DESCRIPTION:	MC24XXYYY I2C EEPROM device
 *
 * FILE CREATION DATE:	12/02/2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,12feb04 erd 	written
 *
 ***************************************************************************/

#include "sysdef.h"
#include "mc24xxyyy_prv.h"
#include "i2c.h"

// ==================================================================
// mc24xxyyy_init()
//
// Initialize the real-time clock module
//

BOOL mc24xxyyy_init()
{
    // everything is ok, return ok
    return TRUE;
}

// ==================================================================
// mc24xxyyy_readBytes()
//
// Reads a byte from EEPROM device to buffer
//
// Arguments:
//  address: address of byte to read
//  buffer: one byte buffer read from device
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

BOOL mc24xxyyy_readByte(const uint_16 address, char *buffer)
{
	// initiate the session with the device
	if (mc24xxyyy_initializeSession())
	{
		// write register pointer - start by high and end with lo
		if (i2c_sendByte(MSK_HI_BYTE(address)) == TRUE && i2c_sendByte(MSK_LO_BYTE(address)) == TRUE) 
		{
			// initiate the session with the device
			if (i2c_initiateSession(MC24XXYYY_I2C_ADDRESS,	// write address
									I2C_OPM_READ, 			// write, since we're writing a command
									TRUE) == TRUE)			// restart
			{
				// read the data to the buffer
				*buffer = i2c_readByte(TRUE);
			}
		}
			
		// send stop sequence
		i2c_sendStop();
			
		// return ok
		return TRUE;
	}
	else
	{
		// failed to initiate return error 
		return FALSE;
	}
}

// ==================================================================
// mc24xxyyy_writeByte()
//
// Writes a byte to EEPROM
//
// Arguments:
//  address: address of byte to read
//  buffer: one byte buffer to write to device
//
// Return value:
//	If TRUE, successfully wrote data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly

BOOL mc24xxyyy_writeByte(const uint_16 address, char buffer)
{
	// initiate the session with the device
	if (mc24xxyyy_initializeSession())
	{
        uint_8 result = FALSE;
		
		// write register pointer - start by high and end with lo
		if (i2c_sendByte(MSK_HI_BYTE(address)) == TRUE && i2c_sendByte(MSK_LO_BYTE(address)) == TRUE)
		{
			// send the data
			result = i2c_sendByte(buffer) == TRUE;
		}
			
		// send stop sequence
		i2c_sendStop();
			
		// return the result
		return result;
	}
	else
	{
		// failed to initiate return error 
		return FALSE;
	}
}

// ==================================================================
// mc24xxyyy_initializeSession()
//
// Initializes a session with the mc24xxyyy. The device might be busy
// with a write cycle and at this stage it won't ACK. Waits a certain 
// MC24XXYYY_WRITE_WAIT_CYCLES of cycles for write to complete. If after
// this the device didn't respond, we assume failure
//
// Return value:
//	If TRUE, data was successfully written to device and device is ready for operation
//  If FALSE, device timed out on write

BOOL mc24xxyyy_initializeSession()
{
	// initialize cycles left
	uint_16 cyclesLeft = MC24XXYYY_WRITE_WAIT_CYCLES - 1;
	BOOL writeCompleted = FALSE;
	
	// attempt to initialize session
	writeCompleted = i2c_initiateSession(MC24XXYYY_I2C_ADDRESS,	// write address
										 I2C_OPM_WRITE, 		// write, since we're writing a command
										 FALSE) == TRUE;		// not restarting (this is the beginning)
	
	// while there are cycles left
	while (!writeCompleted && cyclesLeft--)
	{
		// attempt to re-initialize session. resend the start sequence (notice that the
		// restart flag is set to true this time, as opposed to last time)
		writeCompleted = i2c_initiateSession(MC24XXYYY_I2C_ADDRESS,	// write address
											 I2C_OPM_WRITE, 		// write, since we're writing a command
											 TRUE) == TRUE;			// restarting ...
	}

	// return whether write completed or not
	return writeCompleted;
}

// ==================================================================
// mc24xxyyy_readPage()
//
// Reads a data from EEPROM device to buffer. 
//
// Arguments:
//  address: Address of page
//  buffer: buffer to store data
//  bufferSize: Amount of bytes to read
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

BOOL mc24xxyyy_read(const uint_16 address, char *buffer, uint_16 bufferSize)
{
	// initiate the session with the device
	if (mc24xxyyy_initializeSession())
	{
        uint_8 result = FALSE;
		
		// write register pointer - start by high and end with lo
		if (i2c_sendByte(MSK_HI_BYTE(address)) == TRUE && i2c_sendByte(MSK_LO_BYTE(address)) == TRUE)
		{
			// initiate the session with the device
			if (i2c_initiateSession(MC24XXYYY_I2C_ADDRESS,	// write address
									I2C_OPM_READ, 			// write, since we're writing a command
									TRUE) == TRUE)			// restart
			{
				// initialize pointer to buffer
				uint_8 *p_currentPosition;

				// start writing bytes
				for (p_currentPosition = buffer; bufferSize; --bufferSize, ++p_currentPosition)
				{
					// read the data - stop reading at the last byte
					*p_currentPosition = i2c_readByte(bufferSize == 1);
				}

				// set result to success
				result = TRUE;
			}
		}
			
		// send stop sequence
		i2c_sendStop();
			
		// return the result
		return result;
	}
	else
	{
		// failed to initiate return error 
		return FALSE;
	}
}

// ==================================================================
// mc24xxyyy_writePage()
//
// Writes a page from buffer to EEPROM device . Note that if the buffer size exceeds the
// page boundaries, the data will wrap to current page. To do this, use the mc24xxyyy_write routine.
// It is slower, but it will handle cross-page transactions.
//
// Arguments:
//  address: Address of page
//  buffer: buffer to store data
//  bufferSize: Amount of bytes to write
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

BOOL mc24xxyyy_writePage(const uint_16 address, char *buffer, uint_16 bufferSize)
{
	// initiate the session with the device
	if (mc24xxyyy_initializeSession())
	{
		// write register pointer - start by high and end with lo
		if (i2c_sendByte(MSK_HI_BYTE(address)) == TRUE && i2c_sendByte(MSK_LO_BYTE(address)) == TRUE)
		{
			// initialize pointer to buffer
			uint_8 *p_currentPosition;
			
			// start writing bytes
			for (p_currentPosition = buffer; bufferSize; --bufferSize, ++p_currentPosition)
			{
				// write the data
				if (i2c_sendByte(*p_currentPosition) == FALSE)
				{
					// we failed writing.. stop
					break;
				}
			}
		}

		// success ?
		if (bufferSize == 0) 
		{
			// send stop sequence
			i2c_sendStop();
			
			// return ok
			return TRUE;
		}
		else
		{
			// send stop sequence
			i2c_sendStop();

			// return error
			return FALSE;
		}
	}
	else
	{
		// failed to initiate return error 
		return FALSE;
	}
}

// ==================================================================
// mc24xxyyy_write()
//
// Write to random address of the mc24xxyyy. Since the device has an
// internal write buffer of size MC24XXYYY_PAGE_SIZE, it is not possible
// to write more than a page size of data at a time. Moreover, it is not
// possible to write data which resides at addresses on different pages.
// Therefore, this routine splits up the write request to descrete page
// writes.
//
// Arguments:
//  address: Address of page
//  buffer: buffer to store data
//  bufferSize: Amount of bytes to write
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

BOOL mc24xxyyy_write(uint_16 address, char *buffer, uint_16 bufferSize)
{
    uint_16 currentWriteSize;

    // while there is no more bytes to write
    while (bufferSize)
    {
        // check how many bytes we can write for this page
        currentWriteSize = MC24XXYYY_PAGE_SIZE - (address % MC24XXYYY_PAGE_SIZE);

        // don't write more than we have to
        if (currentWriteSize > bufferSize) currentWriteSize = bufferSize; 

        // perform the write to page
        if (mc24xxyyy_writePage(address, buffer, currentWriteSize) != TRUE)
		{
			// return error
			return FALSE;
		}

        // increment current address
        address += currentWriteSize;
        buffer += currentWriteSize;

        // decrement bytes written
        bufferSize -= currentWriteSize;
    }

	// return success 
	return TRUE;
}

