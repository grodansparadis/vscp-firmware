/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: mcp9800.h
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

// Defines for the MCP9800/1/2/3
#define MCP9800_REGISTER_TEMP       0x00
#define MCP9800_REGISTER_CONFIG     0x01
#define MCP9800_REGISTER_HYSTERESIS 0x02
#define MCP9800_REGISTER_LIMIT      0x03

#define MCP9800_CONFIG_ONESHOT_DISABLED     0x00    // Power up default
#define MCP9800_CONFIG_ONESHOT_ENABLED      0x80

#define MCP9800_CONFIG_RESOLUTION_9BIT      0x00    // Power up default
#define MCP9800_CONFIG_RESOLUTION_10BIT     0x20
#define MCP9800_CONFIG_RESOLUTION_11BIT     0x40
#define MCP9800_CONFIG_RESOLUTION_12BIT     0x60

#define MCP9800_CONFIG_FAULTQ_1BIT          0x00    // Power up default
#define MCP9800_CONFIG_FAULTQ_2BIT          0x08
#define MCP9800_CONFIG_FAULTQ_4BIT          0x10
#define MCP9800_CONFIG_FAULTQ_6BIT          0x18

#define MCP9800_CONFIG_ALERT_POL_LOW        0x00    // Power up default
#define MCP9800_CONFIG_ALERT_POL_HIGH       0x04

#define MCP9800_CONFIG_COMPARATOR_MODE      0x00    // Power up default
#define MCP9800_CONFIG_INTERRUPT_MODE       0x02

#define MCP9800_CONFIG_SHUTDOWN_DISABLE     0x00    // Power up default
#define MCP9800_CONFIG_SHITDOWN_ENABLE      0x01    


// Prototypes
/*!
    Initialize temperature sensor
    @param slaveAddress Address of module 0-7
    @param config Config byte - See manual
    @return TRUE on success.
*/
uint8_t mcp9800_initTempSensor( uint8_t slaveAddress, uint8_t config );

/*!
    Read given register as unsigned integer
    Get temperature value from sensor
    @param slaveAddress Address of module 0-7
    @param reg Register as 0-3
    @return Register content as a 16-bit integer or 0xffff
              to indicate that an error occued.  
*/
uint16_t mcp9800_readInteger( uint8_t slaveAddress, uint8_t reg );

/*!
    Get temperature value from sensor
    @param slaveAddress Address of module 0-7
    @return Temperature as a 16-bit integer or 0xffff
              to indicate that an error occued.    
*/
uint16_t mcp9800_readTempSensor( uint8_t slaveAddress );

/*!
    Get hysteresis value from sensor
    @param slaveAddress Address of module 0-7
    @return Hysteresis as a 16-bit integer or 0xffff
              to indicate that an error occued.    
*/
uint16_t mcp9800_readHysteresis( uint8_t slaveAddress );

/*!
    Get limit value from sensor
    @param slaveAddress Address of module 0-7
    @return Limit as a 16-bit integer or 0xffff
              to indicate that an error occued.    
*/
uint16_t mcp9800_readLimit( uint8_t slaveAddress );


