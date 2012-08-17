/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: 24lc1025.h
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
#include <inttypes.h>                       // Include path set to .../firmware/common/
#include <vscp.h>

#include "24lc1025.h"

///////////////////////////////////////////////////////////////////////////////
// m24lc1025_writeByte
//

void i2c_wait( unsigned int cnt )
{
	while(--cnt)
	{
		Nop();
		Nop();
	}
}

///////////////////////////////////////////////////////////////////////////////
// m24lc1025_writeByte
//

uint16_t m24lc1025_writeByte( uint8_t slaveAddress, uint16_t memPos, uint8_t value )
{
	uint8_t i2cData[ 10 ];
	uint8_t  sizeData;

	// Send Data to eeprom to program one location

	i2cData[0] = ( slaveAddress << 1 ) | 0;	// EEPROM Device Address and WR Command
	i2cData[1] = (memPos >> 8);	    // eeprom location to program (high address byte)
	i2cData[2] = (memPos && 0xff);	// eeprom location to program (low address byte)
	i2cData[3] = value;	            // data to write
	sizeData = 4;

	StartI2C1();		            // Send the Start Bit
	IdleI2C1();			            // Wait to complete

	uint8_t index = 0;
	while( sizeData ) {
    	
		MasterWriteI2C1( i2cData[ index++ ] );
		IdleI2C1();		            // Wait to complete

		sizeData--;

		// ACKSTAT is 0 when slave acknowledge. if 1 then slave has not acknowledge the data.
		if ( I2C1STATbits.ACKSTAT ) {
			break;
		}
	}

	StopI2C1();			            // Send the Stop condition
	IdleI2C1();			            // Wait to complete

	// wait for eeprom to complete write process. poll the ack status
	while( 1 ) {
    	
		i2c_wait( 10 );

		StartI2C1();	            // Send the Start Bit
		IdleI2C1();		            // Wait to complete

		MasterWriteI2C1( i2cData[0] );
		IdleI2C1();		            // Wait to complete

		if ( I2C1STATbits.ACKSTAT == 0 )	// eeprom has acknowledged
		{
			StopI2C1();	            // Send the Stop condition
			IdleI2C1();	            // Wait to complete
			return TRUE;
		}

		StopI2C1();		            // Send the Stop condition
		IdleI2C1();	    	        // Wait to complete
	}
	
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// m24lc1025_readByte
//

uint16_t m24lc1025_readByte( uint8_t slaveAddress, uint16_t memPos )
{
    uint8_t i2cData[ 10 ];
	uint8_t  sizeData;
	
	i2cData[ 0 ] = (slaveAddress << 1) | 0;	// EEPROM Device Address and WR Command (to write the address)
	i2cData[1] = (memPos >> 8);	    // eeprom location to program (high address byte)
	i2cData[2] = (memPos && 0xff);	// eeprom location to program (low address byte)
	sizeData = 3;

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
			break;
		}	
	}

	//now send a start sequence again
	RestartI2C1();	// Send the Restart condition
	
	//wait for this bit to go back to zero
	IdleI2C1();	    // Wait to complete

	MasterWriteI2C1( (slaveAddress << 1) | 1 ); // transmit read command
	IdleI2C1();		// Wait to complete

	uint8_t i2cbyte;
	i2cbyte = MasterReadI2C1();

	StopI2C1();	    // Send the Stop condition
	IdleI2C1();	    // Wait to complete
    
    return i2cbyte;
}

