/**************************************************************************
 *
 * FILE NAME:			cm.h
 * FILE DESCRIPTION:	Interface to the Configuration manager module
 *
 * FILE CREATION DATE:	24-05-2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,24may03 erd written
 *
 ***************************************************************************/

#ifndef __CM_H_
#define __CM_H_

// ==========================================================================
// Constants

// the number of cycles in the main loop between each call to cm_tick()
#define CM_CYCLES_PER_TICK 				(17000L) // ~1second

// ==========================================================================
// structures

// Configuration structure
typedef struct
{
	uint_8				version;					// the version of the stored structure. MUST be first (used in handlePacket())
	uint_8				someThreshold;				// some foo threshold value
	uint_16				someOtherConfigValue;		// just another configuration value
	uint_8				checksum;					// checksum over the structure, MUST be last

} CM_CONFIG_PARAMS;

// ==========================================================================
// External interface

// the current setting information
extern CM_CONFIG_PARAMS cm_configParams;

// cm_init()
//
// Perform the configuration manager initialization
//
// Return values:
//	TRUE if loaded from EEPROM, FALSE if checksum error
//
extern BOOL cm_init();

// cm_tick()
//
// Perform an operation per tick
//
extern void cm_tick();

// cm_save()
//
// Save the current cm settings to eeprom. This does not necessarily save the
// the settings now. It will trigger the change accumulator mechanism.
//
extern void cm_save();

// cm_flushChanges()
//
// Flushes the changes in the configuration to EEPROM. If there are no changes, 
// nothing is stored to EEPROM
//
extern void cm_flushChanges();

#endif /* __CM_H_ */
