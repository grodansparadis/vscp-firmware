///////////////////////////////////////////////////////////////////////////////
// File: VSCP_Functions.h
//

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	akhe@eurosource.se
 *
 * Copyright (C) 1995-2006 Ake Hedman, eurosource, <akhe@eurosource.se>
 *
 * 060508 Cleaned up
 * 060323 Added DM struct. 
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

#ifndef VSCP_FUNCTIONS_H
#define VSCP_FUNCTIONS_H


//
// $RCSfile: vscp.h,v $
// $Revision: 1.6 $
//

//
// Notes about defines
// -------------------
//
// Normaly make the following defines in the prjcfg.h file
//
// ENABLE_WRITE_2PROTECTED_LOCATIONS
// =================================
// to make it possible to write GUID, 
//			manufacturer id, 
//			manufacturer sub device id
// page_low (0x92) and page_high (0x93) should both contain 0xff
// NOTE the storage must be in EEPROM also for it to work.
//
// DISABLE_AUTOMATIC_NICKNAME_DISCOVERY
// ====================================
// The node will not try to find a nickname on startup.


#include <TypeDefs.h>


// ******************************************************************************
//  	VSCP Constants
// ******************************************************************************

#define VSCP_MAJOR_VERSION					1		// VSCP Major version
#define VSCP_MINOR_VERSION					4		// VSCP Minor Version

#define VSCP_ADDRESS_MASTER					0x00
#define VSCP_ADDRESS_FREE						0xff
#define VSCP_CRC_FREE								0x00
#define VSCP_CRC_DEFAULT						0x40
#define VSCP_CONTROL_STARTUP				0x80
#define VSCP_CONTROL_RUN_WENABLE		0x60
#define VSCP_CONTROL_RUN_WDISABLE		0x40


#define VSCP_SIZE_GUI								16 		// # GUI bytes
#define VSCP_SIZE_DEVURL						32  	// # of device URL bytes

#define VSCP_BOOT_FLAG							0xff	// Boot flag is stored in persistent storage
																					// and if there the bootloader will be 
																					// activated. 

// Bootloaders
#define VSCP_BOOTLOADER_VSCP				0x00	// VSCP bootloader algorithm
#define VSCP_BOOTLOADER_PIC1				0x01	// PIC
#define VSCP_BOOTLOADER_LPC1				0x10	// Philips LPC algortithm
#define VSCP_BOOTLOADER_NONE				0xff
													
// State machine states 
#define VSCP_STATE_STARTUP					0x00	// Cold/warm reset
#define VSCP_STATE_INIT						0x01	// Assigning nickname
#define VSCP_STATE_PREACTIVE				0x02	// Waiting for host initialixation
#define VSCP_STATE_ACTIVE					0x03	// The normal state
#define VSCP_STATE_ERROR					0x04	// error state. Big problems.

// State machine sub states 
#define VSCP_SUBSTATE_NONE				0x00	// No state
#define VSCP_SUBSTATE_INIT_PROBE_SENT		0x01	// probe sent
#define VSCP_SUBSTATE_INIT_PROBE_ACK		0x02	// probe ACK received

// Helper consts and 

#define VSCP_PRIORITY7						0x07
#define VSCP_PRIORITY_LOW					0x07
#define VSCP_PRIORITY6						0x06
#define VSCP_PRIORITY5						0x05
#define VSCP_PRIORITY4						0x04
#define VSCP_PRIORITY_MEDIUM			0x04
#define VSCP_PRIORITY3						0x03
#define VSCP_PRIORITY2						0x02
#define VSCP_PRIORITY1						0x01
#define VSCP_PRIORITY0						0x00
#define VSCP_PRIORITY_HIGH				0x00

#define VSCP_PROBE_TIMEOUT				 250	// each tic is 2 ms - half a seconds
	
// ******************************************************************************
//  	VSCP Register - Logical positions
// ******************************************************************************

#define VSCP_REG_ALARMSTATUS						0x80
#define VSCP_REG_VSCP_MAJOR_VERSION			0x81
#define VSCP_REG_VSCP_MINOR_VERSION			0x82
                                    		
#define VSCP_REG_NODE_CONTROL						0x83
                                    		
#define VSCP_REG_USERID0								0x84
#define VSCP_REG_USERID1								0x85
#define VSCP_REG_USERID2								0x86
#define VSCP_REG_USERID3								0x87
#define VSCP_REG_USERID4								0x88
                                    		
#define VSCP_REG_MANUFACTUR_ID0					0x89
#define VSCP_REG_MANUFACTUR_ID1					0x8A
#define VSCP_REG_MANUFACTUR_ID2					0x8B
#define VSCP_REG_MANUFACTUR_ID3					0x8C
                                    		
#define VSCP_REG_MANUFACTUR_SUBID0			0x8D
#define VSCP_REG_MANUFACTUR_SUBID1			0x8E
#define VSCP_REG_MANUFACTUR_SUBID2			0x8F
#define VSCP_REG_MANUFACTUR_SUBID3			0x90
                                    		
#define VSCP_REG_NICKNAME_ID						0x91
                                    		
#define VSCP_REG_PAGE_SELECT_MSB				0x92
#define VSCP_REG_PAGE_SELECT_LSB				0x93
                                    		
#define VSCP_FIRMWARE_MAJOR_VERSION			0x94
#define VSCP_FIRMWARE_MINOR_VERSION			0x95
#define VSCP_FIRMWARE_SUB_MINOR_VERSION	0x96

#define VSCP_BOOT_LOADER_ALGORITHM			0x97
#define VSCP_BUFFER_SIZE								0x98

#define VSCP_REG_GUIID									0xD0
#define VSCP_REG_DEVICE_URL							0xE0

// INIT LED function codes
#define VSCP_LED_OFF			0x00
#define VSCP_LED_ON				0x01
#define VSCP_LED_BLINK1		0x02


//
// Decision Matrix - definitions
//
// A matrix row consist of 8 bytes and have the following format
//
// | oaddr | flags | class-mask | class-filter | type-mask | type-filter | action | action-param | 
//
// oaddr is the originating address.
//
#define DMATRIX_LEN								8	// Leng of Decision Matrix Structure (8)

#define DMATRIX_FLAG_ENABLED				0x80
#define DMATRIX_FLAG_CHECK_OADDR		0x40
#define DMATRIX_FLAG_HARDCODED			0x20
#define DMATRIX_FLAG_CHECK_ZONE			0x10
#define DMATRIX_FLAG_CHECK_SUBZONE	0x08
#define DMATRIX_FLAG_CLASS_MASK			0x02
#define DMATRIX_FLAG_CLASS_FILTER		0x01

// flag
// ====
//	bit # Description
//	7 		Enabled if set to 1.
//	6 		origination address should be checked (=1) or not checked (=0)
//	5 		Indicates hardcoded originating address.
//	4 		Match Zone to trigger DM entry.
//	3 		Match Subzone to trigger DM entry
//	2 		Reserved
//	1 		Class-mask bit 8.
//	0 		Class-filter bit 8.
//
// Action = 0 is always NOOP, "no operation".
//

#define DMATRIX_POS_OADDR					0
#define DMATRIX_POS_FLAGS					1
#define DMATRIX_POS_CLASSMASK			2
#define DMATRIX_POS_CLASSFILTER		3
#define DMATRIX_POS_TYPEMASK			4
#define DMATRIX_POS_TYPEFILTER		5
#define DMATRIX_POS_ACTION				6
#define DMATRIX_POS_ACTIONPARAM		7

// -----------------------------------------------------------------------------

// External - VSCP Data
//extern uint08_t vscp_nodeid;
extern uint08_t vscp_nickname;		// Assigned node nickname
extern uint08_t vscp_errorcnt;		// VSCP error counter	
extern uint08_t vscp_alarmstatus;	// VSCP alarm status register
extern uint08_t vscp_node_state;	// VSCP state machine main state
extern uint08_t vscp_node_substate; // VSCP state machine sub state
extern uint08_t vscp_initledfunc;	// 
//           The following are defined in vscp.c

extern volatile uint16_t vscp_timer;		// 1 ms timer counter
extern uint08_t vscp_probe_address;			// Probe address for nicknam discovery
extern uint16_t vscp_page_select;			// Selected Register Page		

// Prototypes
void vscp_init( void );
void vscp_error( void );
void vscp_handleProbeState( void );
void vscp_handleReactive( void );
void vscp_goActiveState( void );
void vscp_handleHeartbeat( void );
void vscp_handleSetNickname( void );
void vscp_handleDropNickname( void );
void vscp_newNodeOnline( void );
void vscp_readStdReg( void );
void vscp_writeStdReg( void );
int08_t vscp_check_pstorage( void );
void vscp_goBootloaderMode( void );


int08_t getVSCPFrame( uint16_t *pvscpclass, uint16_t *pvscptype, uint08_t *pNodeId, uint08_t *psize, uint08_t *pData );
int08_t sendVSCPFrame( uint16_t vscpclass, uint16_t vscptype, uint08_t size, uint08_t *pData );

// --------------------------- External Functions -----------------------------
//
// The following methods must be defined
// somewhere and should return firmware version
// information
//
uint08_t vscp_getMajorVersion( void );
uint08_t vscp_getMinorVersion( void );
uint08_t vscp_getSubMinorVersion( void );

// Get GUID from permament storage
uint08_t vscp_getGUID( uint08_t idx );
void vscp_setGUID( uint08_t idx, uint08_t data );

// User ID 0 idx=0
// User ID 1 idx=1
// User ID 2 idx=2
// User ID 3 idx=3
uint08_t vscp_getUserID( uint08_t idx ); 
void vscp_setUserID( uint08_t idx, uint08_t data ); 

// Not that both main and sub id are fetched here
// 		Manufacturer device ID byte 0 - idx=0
// 		Manufacturer device ID byte 1 - idx=1
// 		Manufacturer device ID byte 2 - idx=2
// 		Manufacturer device ID byte 3 - idx=3
// 		Manufacturer device sub ID byte 0 - idx=4
// 		Manufacturer device sub ID byte 1 - idx=5
// 		Manufacturer device sub ID byte 2 - idx=6
// 		Manufacturer device sub ID byte 3 - idx=7
uint08_t vscp_getManufacturerId( uint08_t idx );
void vscp_setManufacturerId( uint08_t idx, uint08_t data );

// Get bootloader algorithm from permanent storage
uint08_t vscp_getBootLoaderAlgorithm( void );

// Get buffer size from permanent storage
uint08_t vscp_getBufferSize( void );

// Get URL from device from permanent storage
// index 0-15
uint08_t vscp_getMDF_URL( uint08_t idx );

// Fetch nickname from permanent storage
uint08_t vscp_getNickname( void );

// Write nickname to permanent storage
void vscp_setNickname( uint08_t nickname );

// Fetch segment CRC from permanent storage
uint08_t vscp_getSegmentCRC( void );

// Write segment CRC to permanent storage
void vscp_setSegmentCRC( uint08_t crc );

// Write control byte permanent storage
void vscp_setControlByte( uint08_t ctrl );

// Fetch control byte from permanent storage
uint08_t vscp_getControlByte( void );

// Get page select bytes
//	idx=0 - byte 0 MSB
//	idx=1 - byte 1 LSB
uint08_t vscp_getPageSelect( uint08_t idx );
void vscp_setPageSelect( uint08_t idx, uint08_t data );

void write_app_register( uint08_t reg, uint08_t val );
void read_app_register( uint08_t reg );

void vscp_init_app( void);
void vscp_setDefaultEeprom( void);
void doDecisionMatrix( void);

#endif


