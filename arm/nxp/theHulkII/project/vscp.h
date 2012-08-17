///////////////////////////////////////////////////////////////////////////////
//
// File: vscp.h
//

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	Version 0.01 2004-04-06
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2005 Ake Hedman, eurosource
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

//
// $RCSfile: vscp.h,v $
// $Revision: 1.2 $
//

extern uint8_t vscp_nodeid;


// to have GUID stored in EEPROM
#define GUID_IN_EEPROM

// to have manufacturer device id and sub device id in eeprom
// NOTE: GUID must also be defined if this switch is defined
#define MANUFACTURER_INFO_IN_EEPROM

// Force GUID EEPROM storage if MANUFACTURER info in EEPROM
#if defined(MANUFACTURER_INFO_IN_EEPROM)
#define GUID_IN_EEPROM
#endif

// to make it possible to write GUID, 
//			manufacturer id, 
//			manufacturer sub device id
// page_low (0x92) and page_high (0x93) should both contain 0xff
// NOTE the storage bust be in EEPROM also for it to work.
#define ENABLE_WRITE_2PROTECTED_LOCATIONS


#ifndef BYTE
	typedef unsigned char BYTE;
#endif

// ******************************************************************************
//  	VSCP Constants
// ******************************************************************************

#define VSCP_MAJOR_VERSION					1
#define VSCP_MINOR_VERSION					0

#define VSCP_ADDRESS_MASTER					0x00
#define VSCP_ADDRESS_FREE					0xff

#define VSCP_SIZE_GUI						16 		// # GUI bytes
#define VSCP_SIZE_DEVURL					32  	// # of device URL bytes

#define VSCP_BOOT_FLAG						0xff	// Boot flag is stored in EEPROM and if
													// there the bootloader will be activated. 

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
#define VSCP_SUBSTATE_NONE					0x00	// No state
#define VSCP_SUBSTATE_INIT_PROBE_SENT		0x01	// probe sent
#define VSCP_SUBSTATE_INIT_PROBE_ACK		0x02	// probe ACK received

// Helper consts and 
#define VSCP_VALID_MSG						0x80	// Bit 7 set in flags

#define VSCP_PRIORITY7						0x07
#define VSCP_PRIORITY_HIGH					0x07
#define VSCP_PRIORITY6						0x06
#define VSCP_PRIORITY5						0x05
#define VSCP_PRIORITY4						0x04
#define VSCP_PRIORITY_MEDIUM				0x04
#define VSCP_PRIORITY3						0x03
#define VSCP_PRIORITY2						0x02
#define VSCP_PRIORITY1						0x01
#define VSCP_PRIORITY0						0x00
#define VSCP_PRIORITY_LOW					0x00

#define VSCP_PROBE_TIMEOUT					500	// ms - half a seconds
	
// ******************************************************************************
//  	VSCP Register
// ******************************************************************************

#define VSCP_REG_ALARMSTATUS			0x80
#define VSCP_REG_VSCP_MAJOR_VERSION		0x81
#define VSCP_REG_VSCP_MINOR_VERSION		0x82

#define VSCP_REG_NODE_CONTROL			0x83

#define VSCP_REG_USERID0				0x84
#define VSCP_REG_USERID1				0x85
#define VSCP_REG_USERID2				0x86
#define VSCP_REG_USERID3				0x87
#define VSCP_REG_USERID4				0x88

#define VSCP_REG_MANUFACTUR_ID0			0x89
#define VSCP_REG_MANUFACTUR_ID1			0x8A
#define VSCP_REG_MANUFACTUR_ID2			0x8B
#define VSCP_REG_MANUFACTUR_ID3			0x8C

#define VSCP_REG_MANUFACTUR_SUBID0		0x8D
#define VSCP_REG_MANUFACTUR_SUBID1		0x8E
#define VSCP_REG_MANUFACTUR_SUBID2		0x8F
#define VSCP_REG_MANUFACTUR_SUBID3		0x90

#define VSCP_REG_NICKNAME_ID			0x91

#define VSCP_REG_PAGE_SELECT_MSB		0x92
#define VSCP_REG_PAGE_SELECT_LSB		0x93

#define VSCP_FIRMWARE_MAJOR_VERSION			0x94
#define VSCP_FIRMWARE_MINOR_VERSION			0x95
#define VSCP_FIRMWARE_SUB_MINOR_VERSION		0x96

#define VSCP_BOOT_LOADER_ALGORITHM			0x97
#define VSCP_BUFFER_SIZE					0x98

#define VSCP_REG_GUIID0					0xD0
#define VSCP_REG_GUIID1					0xD1
#define VSCP_REG_GUIID2					0xD2
#define VSCP_REG_GUIID3					0xD3
#define VSCP_REG_GUIID4					0xD4
#define VSCP_REG_GUIID5					0xD5
#define VSCP_REG_GUIID6					0xD6
#define VSCP_REG_GUIID7					0xD7
#define VSCP_REG_GUIID8					0xD8
#define VSCP_REG_GUIID9					0xD9
#define VSCP_REG_GUIID10				0xDA
#define VSCP_REG_GUIID11				0xDB
#define VSCP_REG_GUIID12				0xDC
#define VSCP_REG_GUIID13				0xDD
#define VSCP_REG_GUIID14				0xDE
#define VSCP_REG_GUIID15				0xDF

#define VSCP_REG_DEVICE_URL				0xE0

// EEPROM Storage
#define VSCP_EEPROM_BOOTLOADER_FLAG			0x00	// Reserved for bootloader	 

#define VSCP_EEPROM_NICKNAME				0x01	// Persistant nickname id storage
#define VSCP_EEPROM_SEGMENT_CRC				0x02	// Persistant segment crc storage
#define VSCP_EEPROM_CONTROL					0x03	// Persistant control byte

#define VSCP_EEPROM_REG_USERID				0x04
#define VSCP_EEPROM_REG_USERID1				0x05
#define VSCP_EEPROM_REG_USERID2				0x06
#define VSCP_EEPROM_REG_USERID3				0x07
#define VSCP_EEPROM_REG_USERID4				0x08

// The following can be stored in flash or eeprom

#define VSCP_EEPROM_REG_MANUFACTUR_ID0		0x09
#define VSCP_EEPROM_REG_MANUFACTUR_ID1		0x0A
#define VSCP_EEPROM_REG_MANUFACTUR_ID2		0x0B
#define VSCP_EEPROM_REG_MANUFACTUR_ID3		0x0C

#define VSCP_EEPROM_REG_MANUFACTUR_SUBID0	0x0D	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID1	0x0E	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID2	0x0F	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID3	0x10

#ifndef GUID_IN_EEPROM
#define VSCP_EEPROM_VSCP_END				0x10	// marks end of VSCP EEPROM usage
#endif

// The following can be stored in program ROM (recommended) or in EEPROM 

#define VSCP_EEPROM_REG_GUIID0				0x11	// GUID MSB					
#define VSCP_EEPROM_REG_GUIID1				0x12				
#define VSCP_EEPROM_REG_GUIID2				0x13			
#define VSCP_EEPROM_REG_GUIID3				0x14			
#define VSCP_EEPROM_REG_GUIID4				0x15			
#define VSCP_EEPROM_REG_GUIID5				0x16			
#define VSCP_EEPROM_REG_GUIID6				0x17			
#define VSCP_EEPROM_REG_GUIID7				0x18			
#define VSCP_EEPROM_REG_GUIID8				0x19			
#define VSCP_EEPROM_REG_GUIID9				0x1A			
#define VSCP_EEPROM_REG_GUIID10				0x1B		
#define VSCP_EEPROM_REG_GUIID11				0x1C		
#define VSCP_EEPROM_REG_GUIID12				0x1D		
#define VSCP_EEPROM_REG_GUIID13				0x1E		
#define VSCP_EEPROM_REG_GUIID14				0x1F		
#define VSCP_EEPROM_REG_GUIID15				0x20

#ifdef GUID_IN_EEPROM
#define VSCP_EEPROM_VSCP_END				0x21	// marks end of VSCP EEPROM usage
#endif

#define VSCP_EEPROM_REG_DEVICE_URL			0x21	// Start of Device URL storage

// INIT LED function codes
#define VSCP_LED_OFF		0x00
#define VSCP_LED_ON			0x01
#define VSCP_LED_BLINK1		0x02

struct _imsg {
	// Input message flag
	// ==================
	// Bit 7 - Set if message valid
	// Bit 6 - Reserved
	// Bit 5 - Hardcoded (will never be set)
	// Bit 3 - Number of data bytes MSB
	// Bit 2 - Number of data bytes 
	// Bit 1 - Number of data bytes
	// Bit 0 - Number of data bytes LSB
	unsigned char flag;		

	unsigned char priority;	// Priority for the message 0-7	
	unsigned short class;	// VSCP class
	unsigned char type;		// VSCP type
	unsigned char oaddr;	// Packet originating address
	unsigned char data[8];	// data bytes 	
};

struct _omsg {
	// Output message ( Message to send )
	// ==================================
	// Bit 7 - Set if message should be sent (cleard when sent)
	// Bit 6 - Reserved
	// Bit 5 - Reserved
	// Bit 2 - Number of data bytes MSB
	// Bit 2 - Number of data bytes 
	// Bit 1 - Number of data bytes
	// Bit 0 - Number of data bytes LSB
	unsigned char flag;	

	unsigned char priority;			// Priority for the message 0-7	
	unsigned short class;			// VSCP class
	unsigned char type;				// VSCP type
	// Originating address is always *this* node
	unsigned char data[8];			// data bytes
};

// External - VSCP Data

extern BYTE vscp_nickname;

extern struct _imsg vscp_imsg;
extern struct _omsg vscp_omsg;		 

extern BYTE vscp_errorcnt;			
extern BYTE vscp_alarmstatus;	
	
extern BYTE vscp_node_state;	
extern BYTE vscp_node_substate;

extern BYTE vscp_initledfunc;	

extern volatile unsigned short vscp_timer;

extern volatile BYTE vscp_initbtncnt;

extern volatile BYTE vscp_statuscnt;

extern BYTE vscp_probe_address;

extern unsigned short vscp_page_select;

//
// Decision Matrix - definitions
//
// A matrix row consist of 8 bytes and have the following format
//
// | oaddr | flags | class-mask | class-filter | type-mask | type-filter | action | action-param | 
//
// oaddr is the originating address.
//
// flag
// ====
// bit 7 - Enabled (==1).
// bit 6 - oaddr should be checked (==1) or not checked (==0) 
// bit 5 - Reserved
// bit 4 - Reserved
// bit 3 - Reserved
// bit 2 - Reserved
// bit 1 - Classmask bit 8
// bit 0 - Classfilter bit 8
//
// Action = 0 is always NOOP, "no operation".

#define VSCP_DM_POS_OADDR			0
#define VSCP_DM_POS_FLAGS			1
#define VSCP_DM_POS_CLASSMASK		2
#define VSCP_DM_POS_CLASSFILTER		3
#define VSCP_DM_POS_TYPEMASK		4
#define VSCP_DM_POS_TYPEFILTER		5
#define VSCP_DM_POS_ACTION			6
#define VSCP_DM_POS_ACTIONPARAM		7

#define VSCP_DM_FLAG_ENABLED		0x80
#define VSCP_DM_FLAG_CHECK_OADDR	0x40
#define VSCP_DM_FLAG_HARDCODED		0x20
#define VSCP_DM_FLAG_CHECK_ZONE		0x10
#define VSCP_DM_FLAG_CHECK_SUBZONE	0x08
#define VSCP_DM_FLAG_CLASS_MASK		0x02
#define VSCP_DM_FLAG_CLASS_FILTER	0x01

// Prototypes
boolean sendVSCPFrame( uint16_t vscpclass, uint16_t vscptype, uint8_t size, uint8_t *pData );
boolean getVSCPFrame( uint16_t *pvscpclass, uint16_t *pvscptype, uint8_t *pNodeId, uint8_t *psize, uint8_t *pData );

void vscp_init( void );
void vscp_error( void );
void vscp_probe( void );
void vscp_rcv_preactive( void );
void vscp_active( void );
void vscp_rcv_heartbeat( void );
void vscp_rcv_set_nickname( void );
void vscp_rcv_drop_nickname( void );
void vscp_rcv_new_node_online( void );
void vscp_rcv_readreg( void );
void vscp_rcv_writereg( void );
int vscp_sendMsg( void );
int vscp_receiveMsg( void );
int vscp_check_pstorage( void );

//
// The following methods must be defined
// somewhere and should return firmware version
// information
//
unsigned char getMajorVersion( void );
unsigned char getMinorVersion( void );
unsigned char getSubMinorVersion( void );

#ifdef GUID_IN_EEPROM
unsigned char getGuidFromEEprom( unsigned char idx );
#endif

#ifdef MANUFACTURER_INFO_IN_EEPROM
// Not that both main and sub id are fetched here
unsigned char getManufacturerIdFromEEprom( unsigned char idx ); 
#endif

unsigned char getBootLoaderAlgorithm( void );
unsigned char getBufferSize( void );







