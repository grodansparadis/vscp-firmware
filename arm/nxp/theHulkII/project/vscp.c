///////////////////////////////////////////////////////////////////////////////
//
// File: vscp.c
//

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
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
// $RCSfile: vscp.c,v $
// $Revision: 1.2 $
//

#include <string.h>
#include <stdlib.h>
#include "lawicel232can.h"
#include "uart.h"
#include "eeprom.h"
#include "vscp.h"
#include <vscp_class.h>
#include <vscp_type.h>

// Constants

#ifndef GUID_IN_EEPROM
// GUID for this node MSB - LSB
const rom unsigned char vscp_guid[] = { 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};
#endif

#ifndef MANUFACTURER_INFO_IN_EEPROM
// Manufacturer id's
// offset 0 - Manufacturer device id 0
// offset 1 - Manufacturer device id 1
// offset 2 - Manufacturer device id 2
// offset 3 - Manufacturer device id 3
// offset 4 - Manufacturer subdevice id 0
// offset 5 - Manufacturer subdevice id 1
// offset 6 - Manufacturer subdevice id 2
// offset 7 - Manufacturer subdevice id 3
const rom unsigned char vscp_manufacturer_id[] = { 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

extern const char vscp_deviceURL[];

// Globals

// VSCP Data
BYTE vscp_nickname;			// Node nickname

BYTE vscp_errorcnt;			// VSCP/CAN errors
BYTE vscp_alarmstatus;		// VSCP Alarm Status

BYTE vscp_node_state;		// State machine state
BYTE vscp_node_substate;	// State machine substate

BYTE vscp_probe_cnt;		// Number of timout probes

// Incoming message
struct _imsg  vscp_imsg;

// Outgoing message
struct _omsg vscp_omsg;

BYTE vscp_probe_address;				// Address used during initialization
BYTE vscp_initledfunc;					// Init LED functionality
volatile unsigned short vscp_timer;		// 1 ms timer counter
volatile BYTE vscp_initbtncnt;			// init button counter
volatile BYTE vscp_statuscnt;			// status LED counter

// page selector
unsigned short vscp_page_select;


///////////////////////////////////////////////////////////////////////////////
// sendVSCP
//

boolean sendVSCPFrame( uint16_t vscpclass, 
					uint16_t vscptype, 
					uint8_t size, 
					uint8_t *pData )
{
	uint32_t id;
	
	id = ( vscpclass << 16 ) + ( vscptype << 8 ) +  vscp_nickname;
	
	if ( !SendCANFrame( id, size, pData ) ) {
		return false;
	}
	

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// getVSCP
//

boolean getVSCPFrame( uint16_t *pvscpclass, 
					uint16_t *pvscptype, 
					uint8_t *pNodeId, 
					uint8_t *psize, 
					uint8_t *pData )
{
	uint32_t id;
	
	if ( !getCANFrame( &id, psize, pData ) ) {
		return false;
	}

	*pNodeId = id & 0x0ff;
	*pvscptype = ( id >> 8 ) & 0xff;
	*pvscpclass = ( id >> 16 ) & 0x1ff;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_init
//

void vscp_init( void )
{
	vscp_initledfunc = VSCP_LED_BLINK1;

	// read the nickname id
	vscp_nickname = readEEPROM( VSCP_EEPROM_NICKNAME );		

	//	if zero set to uninitialized
	if ( !vscp_nickname ) vscp_nickname = VSCP_ADDRESS_FREE;

	// Init incoming message
	vscp_imsg.flag = 0;	
	vscp_imsg.priority = 0;
	vscp_imsg.class = 0;
	vscp_imsg.type = 0;

	// Init outgoing message
	vscp_omsg.flag = 0;
	vscp_omsg.priority = 0;	
	vscp_omsg.class = 0;
	vscp_omsg.type = 0;

    vscp_errorcnt = 0;					// No errors yet
    vscp_alarmstatus = 0;				// No alarmstatus

	vscp_probe_address = 0;

	// Initial state
	vscp_node_state = VSCP_STATE_STARTUP;
	vscp_node_substate = VSCP_SUBSTATE_NONE;

	vscp_timer = 0;
	vscp_probe_cnt = 0;
	vscp_page_select = 0;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_check_pstorage
//
// Check control position integrity and restore EEPROM
// if invalid.
//

int vscp_check_pstorage( void )
{
	// controlbyte == 01xxxxxx means initialized
	// everything else is uninitialized
	if ( ( readEEPROM( VSCP_EEPROM_SEGMENT_CRC ) & 0xc0 ) == 0x40 ) {
		return TRUE;
	}

	// No nickname yet.
	writeEEPROM( VSCP_EEPROM_NICKNAME, 0xff );

	// No segment CRC yet.
	writeEEPROM( VSCP_EEPROM_SEGMENT_CRC, 0x00 );

	// Initial startup
	// write allowed
	writeEEPROM( VSCP_EEPROM_CONTROL, 0x90 );

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_error
//

void vscp_error( void )
{
	vscp_initledfunc = VSCP_LED_OFF;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_probe
//

void vscp_probe( void )
{
	switch ( vscp_node_substate ) {
	
		case VSCP_SUBSTATE_NONE:

			if ( VSCP_ADDRESS_FREE != vscp_probe_address ) {
				
				vscp_omsg.flag = VSCP_VALID_MSG + 1 ;	// one databyte 
				vscp_omsg.priority = VSCP_PRIORITY_HIGH;	
				vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
				vscp_omsg.type = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
				vscp_omsg.data[ 0 ] = vscp_probe_address;			
				
				// send the message
				vscp_sendMsg();

				vscp_node_substate = VSCP_SUBSTATE_INIT_PROBE_SENT;
				vscp_timer = 0;	
				
			}
			else {

				// No free address -> error
				vscp_node_state = VSCP_STATE_ERROR;
				
				// Tell system we are giving up
				vscp_omsg.flag = VSCP_VALID_MSG + 1 ;	// one databyte 
				vscp_omsg.priority = VSCP_PRIORITY_HIGH;	
				vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
				vscp_omsg.type = VSCP_TYPE_PROTOCOL_NO_NICKNAME;			
				
				// send the message
				vscp_sendMsg();

			}
			break;	

		case VSCP_SUBSTATE_INIT_PROBE_SENT:
			
			if ( vscp_imsg.flag & VSCP_VALID_MSG ) {	// incoming message?

				// Yes, incoming message
				if ( ( VSCP_CLASS1_PROTOCOL == vscp_imsg.class ) && 
					( VSCP_TYPE_PROTOCOL_PROBE_ACK == vscp_imsg.type ) ) {

					// Yes it was an ack from the segment master or a node
					if ( !vscp_probe_address ) {

						// Master controller answered
						// wait for address
						vscp_node_state = VSCP_STATE_PREACTIVE;
						vscp_timer = 0; // reset timer
						
					}
					else {

						// node answered, try next address
						vscp_probe_address++;
						vscp_node_substate = VSCP_SUBSTATE_NONE;
						vscp_probe_cnt = 0;
						
					}	
				}
			}
			else {

				if ( vscp_timer > VSCP_PROBE_TIMEOUT ) {	// Check for timeout

					vscp_probe_cnt++;	// Another timeout
					
					if ( vscp_probe_cnt > 3 ) { 
					
						// Yes we have a timeout
						if ( !vscp_probe_address ) {			// master controller probe?
	
							// No master controler on segment, try next node
							vscp_probe_address++;
							vscp_node_substate = VSCP_SUBSTATE_NONE;

						}
						else {
						
							// We have found a free address - use it
							vscp_nickname = vscp_probe_address;
							vscp_node_state = VSCP_STATE_ACTIVE;
							vscp_node_substate = VSCP_SUBSTATE_NONE;
							writeEEPROM( VSCP_EEPROM_NICKNAME, vscp_nickname );
							writeEEPROM( VSCP_EEPROM_SEGMENT_CRC, 0x40 );	// segment code (non server segment )

							// Report success
							vscp_probe_cnt = 0;
							vscp_active();
						
						}
					}
					else {
						vscp_node_substate = VSCP_SUBSTATE_NONE;
					}
				} // Timeout

			}			
			break;

		case VSCP_SUBSTATE_INIT_PROBE_ACK:
			break;

		default:
			vscp_node_substate = VSCP_SUBSTATE_NONE;
			break;
	}

	vscp_imsg.flag = 0;	
	
}

///////////////////////////////////////////////////////////////////////////////
// vscp_preactive
//

void vscp_rcv_preactive( void )
{
	if ( vscp_imsg.flag & VSCP_VALID_MSG ) {	// incoming message?

		if ( ( VSCP_CLASS1_PROTOCOL == vscp_imsg.class )  && 
						(  VSCP_TYPE_PROTOCOL_SET_NICKNAME == vscp_imsg.type ) &&
						(  VSCP_ADDRESS_FREE == vscp_imsg.data[ 0 ] ) ) {
						
			// Assign nickname
			vscp_nickname = vscp_imsg.data[ 2 ];
			writeEEPROM( VSCP_EEPROM_NICKNAME, vscp_nickname );		
			writeEEPROM( VSCP_EEPROM_SEGMENT_CRC, 0x40 );
	
			// Go active state
			vscp_node_state = VSCP_STATE_ACTIVE;						
		}		
	}
	else {
		// Check for time out
		if ( vscp_timer > VSCP_PROBE_TIMEOUT ) {
			// Yes, we have a timeout
			vscp_nickname = VSCP_ADDRESS_FREE;
			writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
			vscp_init();	
		}				
	}
}

///////////////////////////////////////////////////////////////////////////////
// vscp_active
//

void vscp_active( void )
{
	vscp_omsg.flag = VSCP_VALID_MSG + 1 ;	// one databyte 
	vscp_omsg.priority = VSCP_PRIORITY_HIGH;	
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
	vscp_omsg.data[ 0 ] = vscp_nickname;			
				
	// send the message
	vscp_sendMsg();

	vscp_initledfunc = VSCP_LED_ON;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_sendMsg
//

int vscp_sendMsg( void )
{
    unsigned long id = ( (unsigned long)vscp_omsg.priority << 25 ) |
						( (unsigned long  )vscp_omsg.class  << 16 ) |
						( (unsigned long)vscp_omsg.type  << 8) |
							 vscp_nickname;		// nodeaddress (our address)

	if ( !SendCANFrame( id, vscp_omsg.flag & 0x0f, vscp_omsg.data ) ) {
		// Failed to send message
		return FALSE;
	}

	vscp_omsg.flag = 0;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_receiveMsg
//

int vscp_receiveMsg( void )
{
	unsigned long id;
	BYTE dlc;

	// Dont read in new message if there already is a message
	// in the input buffer
	if ( vscp_imsg.flag & VSCP_VALID_MSG ) return FALSE;

	if ( getCANFrame( &id, &dlc, vscp_imsg.data ) ) {

		vscp_imsg.flag = dlc;
        vscp_imsg.priority = (unsigned short)( 0x07 & ( id >> 25 ) );
		vscp_imsg.class = (unsigned short)( 0x1ff & ( id >> 16 ) );
		vscp_imsg.type = (unsigned char)( 0xff & ( id >> 8 ) ); 
		vscp_imsg.oaddr = (unsigned char)( 0xff & id );

		vscp_imsg.flag |= VSCP_VALID_MSG;

		return TRUE;
	}	
	
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_rcv_heartbeat
//

void vscp_rcv_heartbeat( void )
{
	if ( ( 5 == ( vscp_imsg.flag & 0x0f ) ) && 
			( readEEPROM( VSCP_EEPROM_SEGMENT_CRC ) != vscp_imsg.data[ 0 ] ) ) {

		// Stored CRC are different than received
		// We must be on a different segment
		writeEEPROM( VSCP_EEPROM_SEGMENT_CRC, vscp_imsg.data[ 0 ] );
		
		// Introduce ourself in the proper way and start from the beginning
		vscp_nickname = VSCP_ADDRESS_FREE;
		writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
		vscp_node_state = VSCP_STATE_INIT;

	}		
}

///////////////////////////////////////////////////////////////////////////////
// vscp_rcv_set_nickname
//

void vscp_rcv_set_nickname( void )
{
	if ( ( 2 == ( vscp_imsg.flag & 0x0f ) ) && 
					( vscp_nickname == vscp_imsg.data[ 0 ] ) ) {

		// Yes, we are addressed
		vscp_nickname = vscp_imsg.data[ 1 ];
		writeEEPROM( VSCP_EEPROM_NICKNAME, vscp_nickname );		
		writeEEPROM( VSCP_EEPROM_SEGMENT_CRC, 0x40 );
	}	
}

///////////////////////////////////////////////////////////////////////////////
// vscp_rcv_drop_nickname
//

void vscp_rcv_drop_nickname( void )
{
	if ( ( 1 == ( vscp_imsg.flag & 0x0f ) ) && 
					( vscp_nickname == vscp_imsg.data[ 0 ] ) ) {
		
		// Yes, we are addressed
		vscp_nickname = VSCP_ADDRESS_FREE;
		writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
		vscp_init();

	}
}

///////////////////////////////////////////////////////////////////////////////
// vscp_rcv_new_node_online
//

void vscp_rcv_new_node_online( void )
{
	if ( ( 1 == ( vscp_imsg.flag & 0x0f ) ) && 
					( vscp_nickname == vscp_imsg.data[ 0 ] ) ) {

		// This is a probe which use our nickname
		// we have to respond to tell the new node that
		// ths nickname is in use

		vscp_omsg.flag = VSCP_VALID_MSG;
		vscp_omsg.priority = VSCP_PRIORITY_HIGH;	
		vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
		vscp_omsg.type = VSCP_TYPE_PROTOCOL_PROBE_ACK;		
		vscp_sendMsg();				

	}
}

///////////////////////////////////////////////////////////////////////////////
// vscp_rcv_readreg
//

void vscp_rcv_readreg( void )
{
	vscp_omsg.priority = VSCP_PRIORITY_HIGH;
	vscp_omsg.flag = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
	
	vscp_omsg.data[ 0 ] =  vscp_imsg.data[ 1 ];
	vscp_omsg.data[ 1 ] =  0;  // Default

	if ( VSCP_REG_ALARMSTATUS == vscp_imsg.data[ 1 ] ) {

		// * * * Read alarm status register * * * 
		vscp_omsg.data[ 1 ] = vscp_alarmstatus;
		vscp_alarmstatus = 0x00;		// Reset alarm status

	}

	else if ( VSCP_REG_VSCP_MAJOR_VERSION == vscp_imsg.data[ 1 ] ) {

		// * * * VSCP Protocol Major Version * * * 
		vscp_omsg.data[ 1 ] = 1;	

	}

	else if ( VSCP_REG_VSCP_MINOR_VERSION == vscp_imsg.data[ 1 ] ) {
		
 		// * * * VSCP Protocol Minor Version * * * 
		vscp_omsg.data[ 1 ] = 0;	

	}

	else if ( VSCP_REG_NODE_CONTROL == vscp_imsg.data[ 1 ] ) {
		
 		// * * * Reserved * * * 
		vscp_omsg.data[ 1 ] = 0;	

	}

	else if ( VSCP_FIRMWARE_MAJOR_VERSION == vscp_imsg.data[ 1 ] ) {
		
 		// * * * Get firmware Major version * * * 
		vscp_omsg.data[ 1 ] = getMajorVersion();	

	}

	else if ( VSCP_FIRMWARE_MINOR_VERSION == vscp_imsg.data[ 1 ] ) {
		
 		// * * * Get firmware Minor version * * * 
		vscp_omsg.data[ 1 ] = getMinorVersion();	

	}

	else if ( VSCP_FIRMWARE_SUB_MINOR_VERSION == vscp_imsg.data[ 1 ] ) {
		
 		// * * * Get firmware Sub Minor version * * * 
		vscp_omsg.data[ 1 ] = getSubMinorVersion();	

	}

	else if ( vscp_imsg.data[ 1 ] < VSCP_REG_MANUFACTUR_ID0 ) {

		// * * * Read from persitant locations * * * 
		vscp_omsg.data[ 1 ] = 
			readEEPROM( ( vscp_imsg.data[ 1 ] - VSCP_REG_USERID0 ) + VSCP_EEPROM_REG_USERID ); 		

	}

	else if ( ( vscp_imsg.data[ 1 ] > VSCP_REG_USERID4 )  &&  ( vscp_imsg.data[ 1 ] < VSCP_REG_NICKNAME_ID ) ) {
		
 		// * * * Manufacturer ID information * * * 
#ifdef MANUFACTURER_INFO_IN_EEPROM
		vscp_omsg.data[ 1 ] = getManufacturerIdFromEEprom( vscp_imsg.data[ 1 ] - VSCP_REG_MANUFACTUR_ID0 ); 
#else 		
		vscp_omsg.data[ 1 ] = 
			vscp_manufacturer_id[ vscp_imsg.data[ 1 ] - VSCP_REG_MANUFACTUR_ID0 ];	
#endif			

	}

	else if ( VSCP_REG_NICKNAME_ID == vscp_imsg.data[ 1 ] ) {

		// * * * nickname id * * * 
		vscp_omsg.data[ 1 ] = vscp_nickname;

	}

	else if ( VSCP_REG_PAGE_SELECT_LSB == vscp_imsg.data[ 1 ] ) {
		
 		// * * * Page select LSB * * * 
		vscp_omsg.data[ 1 ] = ( vscp_page_select & 0xff );	

	}	

	else if ( VSCP_REG_PAGE_SELECT_MSB == vscp_imsg.data[ 1 ] ) {
		
 		// * * * Page select MSB * * * 
		vscp_omsg.data[ 1 ] = ( vscp_page_select >> 16 );	

	}
	
	else if ( VSCP_BOOT_LOADER_ALGORITHM == vscp_imsg.data[ 1 ] ) {
		// * * * Boot loader algorithm * * *
		vscp_omsg.data[ 1 ] = VSCP_BOOTLOADER_PIC1;
	}
	
	else if ( VSCP_BUFFER_SIZE == vscp_imsg.data[ 1 ] ) {
		// * * * Buffer size * * *
		vscp_omsg.data[ 1 ] = getBufferSize();	
	}
			
	else if ( ( vscp_imsg.data[ 1 ] > ( VSCP_REG_GUIID0 - 1 ) ) && ( vscp_imsg.data[ 1 ] < VSCP_REG_DEVICE_URL )  ) {

		// * * * GUID * * * 
#ifdef	GUID_IN_EEPROM
		vscp_omsg.data[ 1 ] = getGuidFromEEprom( vscp_imsg.data[ 1 ] - VSCP_REG_GUIID0 );		
#else
		vscp_omsg.data[ 1 ] = vscp_guid[ vscp_imsg.data[ 1 ] - VSCP_REG_GUIID0 ];		
#endif

	}

	else {

		// * * * The device URL * * * 
		vscp_omsg.data[ 1 ] = vscp_deviceURL[ vscp_imsg.data[ 1 ] - VSCP_REG_DEVICE_URL ];	

	}

	vscp_sendMsg();
}

///////////////////////////////////////////////////////////////////////////////
// vscp_rcv_writereg
//

void vscp_rcv_writereg( void )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flag = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
	vscp_omsg.data[ 0 ] = vscp_nickname; 

	if ( ( vscp_imsg.data[ 1 ] > ( VSCP_REG_VSCP_MINOR_VERSION + 1 ) ) && 
		( vscp_imsg.data[ 1 ] < VSCP_REG_MANUFACTUR_ID0 ) ) {
	 
		// * * * User Client ID * * *
		writeEEPROM( ( vscp_imsg.data[ 1 ] - VSCP_REG_USERID0 ) + VSCP_EEPROM_REG_USERID, vscp_imsg.data[ 2 ] );
		vscp_omsg.data[ 1 ] = readEEPROM( ( vscp_imsg.data[ 1 ] - VSCP_REG_USERID0 ) + VSCP_EEPROM_REG_USERID ); 

	}

	else if ( VSCP_REG_PAGE_SELECT_MSB == vscp_imsg.data[ 1 ] ) {

		// * * * Page select register MSB * * *
		vscp_page_select = ( vscp_page_select & 0xff00 ) | ( vscp_imsg.data[ 2 ] << 16 );
		vscp_omsg.data[ 1 ] = ( vscp_page_select >> 16 );
	}
	
	else if ( VSCP_REG_PAGE_SELECT_LSB == vscp_imsg.data[ 1 ] ) {

		// * * * Page select register LSB * * *
		vscp_page_select = ( vscp_page_select & 0xff ) | vscp_imsg.data[ 2 ];
		vscp_omsg.data[ 1 ] = ( vscp_page_select & 0xff );
	}	

#ifdef ENABLE_WRITE_2PROTECTED_LOCATIONS
	// Write manufacturer id configuration information
	else if ( ( vscp_imsg.data[ 1 ] > VSCP_REG_USERID4 )  &&  ( vscp_imsg.data[ 1 ] < VSCP_REG_NICKNAME_ID ) ) {
		// page must be 0xffff for writes to be possible
		if ( ( 0xff != readEEPROM( VSCP_REG_PAGE_SELECT_LSB ) ) || 
				( 0xff != readEEPROM( VSCP_REG_PAGE_SELECT_MSB ) ) ) {
			// return complement to indicate error
			vscp_omsg.data[ 1 ] = ~vscp_imsg.data[ 2 ];	
		}
		else {
			writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + vscp_imsg.data[ 1 ] - VSCP_REG_MANUFACTUR_ID0, vscp_imsg.data[ 2 ]  );
			vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + vscp_imsg.data[ 1 ] - VSCP_REG_MANUFACTUR_ID0 );
		}
	}
	// Write GUID configuration information
	else if ( ( vscp_imsg.data[ 1 ] > ( VSCP_REG_GUIID0 - 1 ) ) && ( vscp_imsg.data[ 1 ] < VSCP_REG_DEVICE_URL )  ) {
		// page must be 0xffff for writes to be possible
		if ( ( 0xff != readEEPROM( VSCP_REG_PAGE_SELECT_LSB ) ) || 
				( 0xff != readEEPROM( VSCP_REG_PAGE_SELECT_MSB ) ) ) {
			// return complement to indicate error
			vscp_omsg.data[ 1 ] = ~vscp_imsg.data[ 2 ];	
		}
		else {
			writeEEPROM( VSCP_EEPROM_REG_GUIID0 + vscp_imsg.data[ 1 ] - VSCP_REG_GUIID0, vscp_imsg.data[ 2 ] );
			vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_REG_GUIID0 + vscp_imsg.data[ 1 ] - VSCP_REG_GUIID0 );
		}
	}
#endif

	else {
		// return complement to indicate error
		vscp_omsg.data[ 1 ] = ~vscp_imsg.data[ 2 ];		
	}

	vscp_sendMsg();	
}


