/**************************************************************************
 *
 * FILE NAME:			sysdef.h
 * FILE DESCRIPTION:	System definitions
 *
 * FILE CREATION DATE:	24-05-2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,24may03 erd	written
 *
 ***************************************************************************/

#ifndef __SYSDEF_H_
#define __SYSDEF_H_

// include all pic related stuff
#include "pic18.h"

// include special types (uint_8 etc)
#include "types.h"

// include common utilities
#include "common.h"

// DEBUG modes - TODO: remove in final code
#define DEBUG_WDOG_BITE			(0)	// if on, watchdog should bite and reset board after ~25-30 seconds
#define DEBUG_POST_RAM_FAIL 	(0) // if on, RAM post will fail and error will be displayed
#define DEBUG_POST_I2C_FAIL 	(0) // if on, I2C post will fail and leds will flash indicated i2c error
#define DEBUG_POST_ROM_FAIL		(0) // if on, ROM post will fail and error will be displayed
#define DEBUG_POST_EEPROM_FAIL	(0) // if on, Session checksum check will fail and error will be displayed until button pressed

#endif /* __SYSDEF_H_ */
