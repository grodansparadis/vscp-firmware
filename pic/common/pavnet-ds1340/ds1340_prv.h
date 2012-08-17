/**************************************************************************
 *
 * FILE NAME:			ds1340.h
 * FILE DESCRIPTION:	DS1340 (RTC) driver private header
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

#ifndef __DS1340_PRV_H_
#define __DS1340_PRV_H_

#include "ds1340.h"

// ==================================================================
// constants

// register offsets
#define DS1340_SECONDS_REG_OFFSET      	(0x0)
#define DS1340_MINUTES_REG_OFFSET      	(0x1)
#define DS1340_HOURS_REG_OFFSET        	(0x2)
#define DS1340_DAY_REG_OFFSET          	(0x3)
#define DS1340_DATE_REG_OFFSET         	(0x4)
#define DS1340_MONTH_REG_OFFSET        	(0x5)
#define DS1340_YEAR_REG_OFFSET         	(0x6)
#define DS1340_CONTROL_REG_OFFSET      	(0x7)
#define DS1340_TCHARGE_REG_OFFSET      	(0x8)
#define DS1340_FLAG_REG_OFFSET			(0x9)

// seconds register - clock halt bit
#define DS1340_SEC_EOSC_BIT      		(1 << 7)

// hours register
#define DS1340_HOURS_CB_BIT          	(1 << 6) // century bit
#define DS1340_HOURS_CEB_BIT          	(1 << 7) // century enable bit

// control register bits
#define DS1340_CTRL_CAL0_BIT           	(1 << 0) // calibration bit 0
#define DS1340_CTRL_CAL1_BIT           	(1 << 1) // calibration bit 1
#define DS1340_CTRL_CAL2_BIT           	(1 << 2) // calibration bit 2
#define DS1340_CTRL_CAL3_BIT           	(1 << 3) // calibration bit 3
#define DS1340_CTRL_CAL4_BIT           	(1 << 4) // calibration bit 4
#define DS1340_CTRL_S_BIT              	(1 << 5) // Calibration sign
#define DS1340_CTRL_FT_BIT             	(1 << 6) // frequency test
#define DS1340_CTRL_OUT_BIT            	(1 << 7) // output control

// flag register
#define DS1340_FLAG_OSF_BIT			   	(1 << 7) // Oscilator state bit

// I2C address
#define DS1340_I2C_ADDRESS				(0B01101000)

// test configuration
#define DS1340_TEST_MAX_RESULT_DEVIATION	(1)		// max difference between minutes of consecutive reads
#define DS1340_TEST_ITERATIONS				(5)		// number of iterations of read


// ==================================================================
// private routines 

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

extern BOOL ds1340_readRegisters(const uint_8 offset, const uint_8 regCount, char *buffer);


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

extern BOOL ds1340_writeRegisters(const uint_8 offset, const uint_8 regCount, const char *buffer);

#endif /* __DS1340_PRV_H_ */
