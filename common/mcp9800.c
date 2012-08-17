/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: mcp9800.c
 *
 *  SweetBox Module
 * 	Version information in version.h
 * 	akhe@eurosource.se
 *
 * Copyright (C) 1995-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
*/

#include <p32xxxx.h>
#include <plib.h> 

#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include "FreeRTOS.h"
#include <inttypes.h>                       // Include path set to .../firmware/common/

#include "mcp9800.h"

///////////////////////////////////////////////////////////////////////////////
// mcp9800_initTempSensor
//

uint8_t mcp9800_initTempSensor( uint8_t slaveAddress, uint8_t config )
{
    uint8_t i2cData[ 3 ];
	uint8_t sizeData;
	
	portENTER_CRITICAL();
    slaveAddress &= 0x07;   // Only the tree LSB bits are valid
    i2cData[ 0 ] = ( ( slaveAddress | 0x48 ) << 1 ) | I2C_WRITE; // Device Address (0b01001xxx)
	i2cData[ 1 ] = MCP9800_REGISTER_CONFIG;
	i2cData[ 2 ] = config;
	sizeData = 3;    
	
	StartI2C1();		// Send the Start Bit
	IdleI2C1();			// Wait to complete

	uint8_t index = 0;
	while( sizeData ) {
    	
		MasterWriteI2C1( i2cData[ index++ ] );
		IdleI2C1();		// Wait to complete

		sizeData--;

		// ACKSTAT is 0 when slave acknowledge. if 1 then slave has not acknowledge the data.
		if ( I2C1STATbits.ACKSTAT ) {
    		StopI2C1();	    // Send the Stop condition
    		portEXIT_CRITICAL();
			return FALSE;
		}
	}
	
	StopI2C1();			// Send the Stop condition
	IdleI2C1();			// Wait to complete
	portEXIT_CRITICAL();

	return TRUE;
}    


///////////////////////////////////////////////////////////////////////////////
// mcp9800_readInteger
//

uint16_t mcp9800_readInteger( uint8_t slaveAddress, uint8_t reg )
{
    uint8_t i2cData[ 2 ];
	uint8_t sizeData;
	
	portENTER_CRITICAL();
    slaveAddress &= 0x07;   // Only the tree LSB bits are valid
    reg &= 0x03;       // Only the two LSB bits are valid
    i2cData[ 0 ] = ( ( slaveAddress | 0x48 ) << 1 ) | I2C_WRITE; // Device Address (0b01001xxx)
	i2cData[ 1 ] = reg;
	sizeData = 2;
	
	StartI2C1();	// Send the Start Bit
	IdleI2C1();		// Wait to complete

	// send the address to read from the serial eeprom
	uint8_t index = 0;
	while( sizeData ) {
    	
		MasterWriteI2C1( i2cData[ index++ ] );
		IdleI2C1();		//Wait to complete

		sizeData--;

		// ACKSTAT is 0 when slave acknowledge. if 1 then slave has not acknowledge the data.
		if( I2C1STATbits.ACKSTAT ) {
    		StopI2C1();	    // Send the Stop condition
    		portEXIT_CRITICAL();
			return 0xffff;
		}	
	}   
	
	//StopI2C1();     // Send the Stop condition
    //IdleI2C1();     // Wait to complete
	
	// now send a start sequence again
	StartI2C1();	// Send the Restart condition
	IdleI2C1();	    // Wait to complete

    MasterWriteI2C1( ( ( slaveAddress | 0x48 ) << 1 ) | I2C_READ );
	IdleI2C1();		//Wait to complete
	
	// ACKSTAT is 0 when slave acknowledge. if 1 then slave has not acknowledge the data.
	if( I2C1STATbits.ACKSTAT ) {
        StopI2C1();	    // Send the Stop condition
        portEXIT_CRITICAL();
		return 0xffff;
	}

	uint8_t i2cbyte[ 10 ];
			
	i2cbyte[ 0 ] = MasterReadI2C1();
	AckI2C1();
	IdleI2C1();	    // Wait to complete
	
	i2cbyte[ 1 ] = MasterReadI2C1();
	NotAckI2C1();
	IdleI2C1();	    // Wait to complete
    
	StopI2C1();	    // Send the Stop condition
	IdleI2C1();	    // Wait to complete
	
	portEXIT_CRITICAL();	
	return ( ( (uint16_t)i2cbyte[ 0 ] << 8 ) | i2cbyte[ 1 ] );
}

///////////////////////////////////////////////////////////////////////////////
// mcp9800_readTempSensor
//

uint16_t mcp9800_readTempSensor( uint8_t slaveAddress )
{
    return mcp9800_readInteger( slaveAddress, MCP9800_REGISTER_TEMP );
}


///////////////////////////////////////////////////////////////////////////////
// mcp9800_readHysteresis
//

uint16_t mcp9800_readHysteresis( uint8_t slaveAddress )
{
    return mcp9800_readInteger( slaveAddress, MCP9800_REGISTER_HYSTERESIS );
}

///////////////////////////////////////////////////////////////////////////////
// mcp9800_readLimit
//

uint16_t mcp9800_readLimit( uint8_t slaveAddress )
{
    return mcp9800_readInteger( slaveAddress, MCP9800_REGISTER_LIMIT );
}
