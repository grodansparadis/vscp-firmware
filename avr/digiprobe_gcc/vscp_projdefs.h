/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	Project specific settings 
*******************************************************************************/

#ifndef 	_DIGIPROBE_PROJDEFS_H_
	#define _DIGIPROBE_PROJDEFS_H_

	// Module Version
	#define FIRMWARE_MAJOR_VERSION			1
	#define FIRMWARE_MINOR_VERSION			7
	#define FIRMWARE_SUB_MINOR_VERSION		3

	#define THIS_MODULE_MAJOR_VERSION		1
	#define THIS_MODULE_MINOR_VERSION		1
	#define THIS_MODULE_SUBMINOR_VERSION	3


	// The GUID can be stored in FLASH (recommended) or EEPROM
	// comment the next line out to have GUID in EEPROM
	#define GUID_IN_FLASH
	// this is the default GUID used for this node
	#define MY_GUID			0xDE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01

	// The Manufacturer ID can stored in FLASH (recommended) or EEPROM
	// comment the next line out to have Manufacturer ID in EEPROM
	#define MANUFACTID_IN_FLASH
	#define MY_MANUFACTID	0x22, 0x23, 0x24, 0x25, 0x26, 0x26, 0x27, 0x28

	// The MDF URL can be stored in FLASH (recommended) or EEPROM
	// comment the next line out to have MDF URL stored in EEPROM
	#define MDF_URL_IN_FLASH
	#define MY_MDF_URL		"datenheim.de/vscp/sglxdpa.xml"

	// Remove comment if there is an embedded MDF
	//#define EMBEDDED_MDF

	// If manufacturing ID and GUID should be possible to write when
	// page_select = 0xffff uncommenting the following
	// (which makes only sense with above two defines
	#ifndef GUID_IN_FLASH
		#ifndef MANUFACTID_IN_FLASH
			//#define ENABLE_WRITE_2PROTECTED_LOCATIONS
		#endif
	#endif

	// Disable initial nickname discovery by undefining the following
	//#define DISABLE_AUTOMATIC_NICKNAME_DISCOVERY

	// # of elements in decision matrix
	#define DECISION_MATRIX_ELEMENTS			0

	// Start register position for decision matrix
	#if DECISION_MATRIX_ELEMENTS>0
		#define STD_REG_DECISION_MATRIX		0x80-(DECISION_MATRIX_ELEMENTS * 8)
	#endif


#endif
