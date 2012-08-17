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

#include "vscp_compiler.h"

#ifndef TRUE
#define TRUE    (-1)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef I2C_READ
#define I2C_READ                0x01
#endif

#ifndef I2C_WRITE
#define I2C_WRITE               0x00
#endif



/*!
    Wait for a while...
    @aram cnt Core ticks to wait.
*/
void i2c_wait( unsigned int cnt );

/*!
    Wite one byte to EEPROM
    @param slaveAddress Address of device 0-3.
    @param memPos Memory position to read.
    @param value Value to write to EEPROM
    @return TRUE om success, FALSE on failure.
*/
uint16_t m24lc1025_writeByte( uint8_t slaveAddress, uint16_t memPos, uint8_t value );

/*!
    Read one byte from EEPROM
    @param slaveAddress Address of device 0-3.
    @param memPos Memory position to write to.
    @return Retun value in LSB on success or 0xffff on failure.
*/
uint16_t m24lc1025_readByte( uint8_t slaveAddress, uint16_t memPos );

