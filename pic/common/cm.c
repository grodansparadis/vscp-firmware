/**************************************************************************
 *
 * FILE NAME:			cm.h
 * FILE DESCRIPTION:	Interface to the Configuration manager module
 *
 * FILE CREATION DATE:	24-05-2003
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

#include "sysdef.h"
#include "cm_prv.h"
#include "eeprom.h"
#include "string.h"
#include "post.h"

// ==========================================================================
// BOOL cm_init()
//
// Perform the configuration initialization: loads last configuration from eeprom
//
// Return values:
//	TRUE if loaded from EEPROM, FALSE if checksum error
//

BOOL cm_init()
{
	// just load from EEPROM
	BOOL integrity = cm_load();

	// if integrity check failed, raise warning
	if (integrity == FALSE)
	{
		// set the warning type
		post_warningType |= POST_WT_CM_DEFAULTS_LOADED;
	}

	// copy current configuration to stored configuration 
	cm_copyCurrentToStored();

	// return whether or not data is ok
	return integrity;
}

// ==========================================================================
// cm_tick()
//
// Perform an operation per tick. Checks if configuration needs to be stored.
//

void cm_tick()
{
	// check if we need to flush changes to EEPROM
	if (cmNumberOfTicksLeftForSave && --cmNumberOfTicksLeftForSave == 0)
	{
		// flush the changes to EEPROM
		cm_flushChanges();
	}
}

// ==========================================================================
// cm_load()
//
// Loads configuration from EEPROM. If configuration is invalid - defaults are loaded
//
// Return values:
//	TRUE if loaded from EEPROM, FALSE if checksum error
//

static BOOL cm_load()
{
	BOOL integrity = TRUE;
	
	// load configuration information from EEPROM (beginning - offset is 0)
	eeprom_readBuffer(CM_CFG_OFFSET_IN_EEPROM, (uint_8 *)&cm_configParams, sizeof(cm_configParams));

	// check the version
	if (cm_configParams.version == CM_CONFIG_VERSION)
	{
		// now check the checksum over the structure. If stored checksum, the sum
		// of the members will be equal to zero
        if (sys_calculateChecksum((uint_8 *)&cm_configParams, sizeof(cm_configParams)) == 0)
		{
			// everything is fine
			return TRUE;
		}
		else
		{
			// data checksum is incorrect
			integrity = FALSE;
		}
	}

	// if we got here, there was a problem loading structure (bad version / checksum)
	// must load defaults
	cm_configParams.someThreshold					= CM_CFG_DEFAULT_SOME_THRESHOLD;
	cm_configParams.someOtherConfigValue			= CM_CFG_DEFAULT_SOME_OTHER_CONFIG_VALUE;

	// save in EEPROM
	cm_flushChanges();

	// return whether we loaded defaults due to checksum error, or data not in EEPROM
	return integrity;
}

// ==========================================================================
// cm_save()
//
// Save the current configuration settings to eeprom. This will not save the cm
// now - it will start a timer after which a check is made: if configuration changed
// we need to save to EEPROM

void cm_save()
{
	// check if timer is not set (we dont want to reset it on each click)
	if (cmNumberOfTicksLeftForSave == 0)
	{
		// just set it. After DLS_CM_TICKS_FOR_SAVE ticks, cm_flushChanges
		// is called
		cmNumberOfTicksLeftForSave = CM_TICKS_FOR_SAVE;
	}
}

// ==========================================================================
// cm_flushChanges()
//
// Flushes the changes in thes ession to EEPROM. If there are no changes, 
// nothing is stored to EEPROM
//

void cm_flushChanges()
{
	// compare the structures - if they are different we need to store to EEPROM
	if (!cm_compareCurrentAndStored())
	{
		// they differ - we must save them to EEPROM
		
		// zero out the checksum
		cm_configParams.checksum = 0;
		
		// now, calculate the checksum over the structure and set in member. Note that 
		// the checksum byte is excluded from the calculation
		cm_configParams.checksum = sys_calculateChecksum((uint_8 *)&cm_configParams, sizeof(cm_configParams));

		// and store to EEPROM (at beginning, offset 0)
		eeprom_writeBuffer(CM_CFG_OFFSET_IN_EEPROM, (uint_8 *)&cm_configParams, sizeof(cm_configParams));
	
		// copy current configuration to stored configuration 
		cm_copyCurrentToStored();
	}
}

// ==========================================================================
// static BOOL cm_compareCurrentAndStored()
//
// Compares the stored configuration and the stored one
//
// Return values:
//	TRUE if match, FALSE if different
//

static BOOL cm_compareCurrentAndStored()
{
	// compare the members
	BOOL match = (memcmp(&cm_configParams, &cmStoredParams, sizeof(CM_CONFIG_PARAMS)) == 0 ? TRUE : FALSE;

	// return the match
	return (match);
}

// ==========================================================================
// static void cm_copyCurrentToStored()
//
// Copies the members from current configuration -> stored configuration
//

static void cm_copyCurrentToStored()
{
	// copy the members
	memcpy(&cmStoredParams, &cm_configParams, sizeof(CM_CONFIG_PARAMS));
}
