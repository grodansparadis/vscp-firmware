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

#ifndef __CM_PRV_H_
#define __CM_PRV_H_

#include "cm.h"
#include "lcd.h"

// ==========================================================================
// Constants

// configuration version: this number indicates the version of the config structure.
// when the structure is changed, this version must be incremented to prevent cases
// in which a structure is loaded from eeprom has a different structure than that in RAM
#define CM_CONFIG_VERSION 					(1)
	
// default cm values
#define CM_CFG_DEFAULT_SOME_THRESHOLD				(201)
#define CM_CFG_DEFAULT_SOME_OTHER_CONFIG_VALUE		(0xABCD)

// the number of ticks passed after cm_save is called that that current cm is compared with
// that stored in EEPROM and decision is made if it needs to be stored
#define CM_TICKS_FOR_SAVE					(25)	// ~8 seconds

// offset of configuration in EEPROM
#define CM_CFG_OFFSET_IN_EEPROM				(0)

// ==========================================================================
// Globals

// the current setting information. This member is accessed to read the configuration
CM_CONFIG_PARAMS cm_configParams;

// contains the information stored in the EEPROM. used to check if store to
// EEPROM is required (only on change)
static CM_CONFIG_PARAMS cmStoredParams;

// number of ticks left for save
static uint_8 cmNumberOfTicksLeftForSave = 0;

// ==========================================================================
// Internal interface

// static cm_load()
//
// Loads configuration from EEPROM. If configuration is invalid - defaults are loaded
//
// Return values:
//	TRUE if loaded from EEPROM, FALSE if checksum error
//
static BOOL cm_load();

// static BOOL cm_compareCurrentAndStored()
//
// Compares the stored configuration and the stored one
//
// Return values:
//	TRUE if match, FALSE if different
//
static BOOL cm_compareCurrentAndStored();

// static void cm_copyCurrentToStored()
//
// Copies the members from current configuration -> stored configuration
//
static void cm_copyCurrentToStored();


#endif /* __CM_PRV_H_ */
