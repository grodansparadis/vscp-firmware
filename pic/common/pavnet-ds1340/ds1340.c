/**************************************************************************
 *
 * FILE NAME:			ds1340.h
 * FILE DESCRIPTION:	DS1340 (RTC) driver implementation
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
 * 01a,23nov03 erd	written
 *
 ***************************************************************************/

#include "sysdef.h"
#include "ds1340_prv.h"
#include "i2c.h"
#include "post.h"
#include <stdlib.h>

// ==================================================================
// ds1340_init()
//
// Initialize the real-time clock module
//

BOOL ds1340_init()
{
   // reset enable osc bit to enable osc
   uint_8 reg = (0 & ~DS1340_SEC_EOSC_BIT);
   
   // write this data
   if (ds1340_writeRegisters(DS1340_CONTROL_REG_OFFSET, 1, &reg) == FALSE)
   {
	   // set error
	   post_errorType = POST_ET_DS1340_NO_RESPONSE;
		
	   // error, skip everything else
		return FALSE;
   }

   // everything is ok, return test result
   return ds1340_test();
}

// ==================================================================
// ds1340_test()
//
// Test the device
//
// Return value:
//   TRUE if tested successfully, FALSE if failed
//

BOOL ds1340_test()
{
	uint_8 index, minutes;
	DS1340_DATETIME datetime;

	// take first 
	if (ds1340_getTime(&datetime) == FALSE)
	{
		// set error
		post_errorType = POST_ET_DS1340_NO_RESPONSE;

		// return error
		return FALSE;
	}

	// save minutes
	minutes = datetime.minutes;
	
	// run the test
    for (index = DS1340_TEST_ITERATIONS; index; --index)
	{
		// take temperature
		if (ds1340_getTime(&datetime) == FALSE)
		{
			// set error
			post_errorType = POST_ET_DS1340_NO_RESPONSE;

			// return error
			return FALSE;
		}

		// check deviation
		if (abs(datetime.minutes - minutes) > DS1340_TEST_MAX_RESULT_DEVIATION)
		{
			// set error
			post_errorType = POST_ET_DS1340_DEVIATION_TOO_GREAT;

			// return error
			return FALSE;
		}

		// save minutes
		minutes = datetime.minutes;
	}
	
	// success
	return TRUE;
}

// ==================================================================
// ds1340_getDecimal(num, tens, units)
//
// Extracts the units and tens from a number. E.g. 65 (tens = 6, units = 5)
//
// Arguments:
//	num: the number to extract from
//  tens: number of tens in num
//  units: the units of num

#define ds1340_getDecimal(num, tens, units)         \
    tens = (uint_8)(num / 10.0);                    \
    units = (uint_8)(num - tens * 10);

// ==================================================================
// ds1340_setTime(DS1340_DATETIME *datetime)
//
// Sets the current date and time
//
// Arguments:
//	datetime: pointer to structure holding current date and time
//
// Return value:
//	If TRUE, successfully set date and time to IC. If FALSE, 
//  device did not respond correctly and holds undefined data

BOOL ds1340_setTime(const DS1340_DATETIME *datetime)
{
    // buffer which contains data in the device's format
    char buffer[7] = {0, 0, 0, 0, 0, 0, 0};
    uint_8 ten, unit;
    
    //
    // Prepare the buffer to write
    //
    
    // set the seconds (also sets clock hold to 0)
    ds1340_getDecimal(datetime->seconds, ten, unit);
        buffer[0] = unit;
        buffer[0] |= ((ten << 4) & (0x7 << 4));
    
    // set the minutes
    ds1340_getDecimal(datetime->minutes, ten, unit);
        buffer[1] = unit;
        buffer[1] |= ((ten << 4) & (0x7 << 4));

    // set the hours
    ds1340_getDecimal(datetime->hours, ten, unit);
        buffer[2] = unit;
        buffer[2] |= ((ten << 4) & (0x3 << 4));

    // set the day
    buffer[3] = datetime->day;

    // set the date
    ds1340_getDecimal(datetime->date, ten, unit);
        buffer[4] = unit;
        buffer[4] |= ((ten << 4) & (0x3 << 4));
    
    // set the month
    ds1340_getDecimal(datetime->month, ten, unit);
        buffer[5] = unit;
        buffer[5] |= ((ten << 4) & (0x1 << 4));

    // set the year
    ds1340_getDecimal(datetime->year, ten, unit);
        buffer[6] = unit;
        buffer[6] |= ((ten << 4) & (0xF << 4));

    //
    // Write it
    //
    
	// now, write the buffer
	if (ds1340_writeRegisters(DS1340_SECONDS_REG_OFFSET, sizeof(buffer), buffer) == TRUE)
	{
		// success
		return TRUE;
	}
	else
	{
		// we failed, return error
		return FALSE;
	}
}

// ==================================================================
// ds1340_getTime(DS1340_DATETIME *datetime)
//
// Gets the current date and time
//
// Arguments:
//	datetime: pointer to structure to be filled with the current date and time
//
// Return value:
//	If TRUE, successfully received date and time from IC. If FALSE, 
//  device did not respond correctly and datetime does not hold proper
//  data

BOOL ds1340_getTime(DS1340_DATETIME *datetime)
{
    // buffer which contains data in the device's format
    char buffer[7];

    //
    // Read the time to the buffer
    //
    
	// read to buffer
	if (ds1340_readRegisters(DS1340_SECONDS_REG_OFFSET, sizeof(buffer), buffer) == FALSE)
	{
		// error reading
		return FALSE;
	}
    
    //
    // Parse read information to datetime structure
    //
    
    // get seconds
	datetime->seconds =   	(buffer[0] & 0xF);
    datetime->seconds +=  	((buffer[0] >> 4) & 0x7) * 10;
 
    // get minutes        	
    datetime->minutes =   	(buffer[1] & 0xF);
    datetime->minutes +=  	(buffer[1] >> 4) * 10;
 
    // get hours          	
    datetime->hours =     	(buffer[2] & 0xF);
    datetime->hours +=    	((buffer[2] >> 4) & 0x3) * 10;
 
    // get day            	
    datetime->day =       	(buffer[3]);
 
    // get date           	
    datetime->date =      	(buffer[4] & 0xF);
    datetime->date +=     	((buffer[4] >> 4) & 0x3) * 10;
 
    // get month          	
    datetime->month =     	(buffer[5] & 0xF);
    datetime->month +=    	((buffer[5] >> 4) & 0x1) * 10;
 
    // get year           	
    datetime->year =      	(buffer[6] & 0xF);
    datetime->year +=     	((buffer[6] >> 4) & 0xF) * 10;
 
	// read correctly     	
	return TRUE;          	
}

// ==================================================================
// ds1340_readRegisters()
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

BOOL ds1340_readRegisters(const uint_8 offset, const uint_8 regCount, char *buffer)
{
	// initiate the session with the device
	if (i2c_initiateSession(DS1340_I2C_ADDRESS,	// write address
							I2C_OPM_WRITE, 		// write, since we're writing a command
							FALSE) == TRUE)		// not restarting (this is the beginning)
	{
		uint_8 currentByte;
		
		// write register pointer
		if (i2c_sendByte(offset) == TRUE) 
		{
			// initiate the session with the device
			if (i2c_initiateSession(DS1340_I2C_ADDRESS,	// write address
									I2C_OPM_READ, 		// write, since we're writing a command
									TRUE) == TRUE)		// restart
			{
				// start reading bytes
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
// ds1340_writeRegisters()
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

BOOL ds1340_writeRegisters(const uint_8 offset, const uint_8 regCount, const char *buffer)
{
	// initiate the session with the device
	if (i2c_initiateSession(DS1340_I2C_ADDRESS,	// write address
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



