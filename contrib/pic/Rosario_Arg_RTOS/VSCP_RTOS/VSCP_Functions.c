///////////////////////////////////////////////////////////////////////////////
// File: vscp.c
//

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2026 Ake Hedman, eurosource, <akhe@eurosource.se>
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
// $Revision: 1.6 $
//

#include <string.h>
#include <stdlib.h>
#include <Registers.h>
#include "vscp_functions.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "VSCP_drv.h"

//#ifndef FALSE
//#define FALSE		0
//#endif
//
//#ifndef TRUE
//#define TRUE        !FALSE
//#endif
//
//#ifndef ON
//#define ON			!FALSE
//#endif
//
//#ifndef OFF
//#define OFF			FALSE
//#endif 


// Globals

// VSCP Data
uint08_t vscp_nickname;			// Node nickname

uint08_t vscp_errorcnt;			// VSCP/CAN errors
uint08_t vscp_alarmstatus;		// VSCP Alarm Status

uint08_t vscp_node_state;		// State machine state
uint08_t vscp_node_substate;		// State machine substate

uint08_t vscp_probe_cnt;			// Number of timout probes

//interchange betwen VSCP and RTOS CAN DRIVER
// Outgoing message
VSCP_msg_t  SendMsg;
// Incoming message
VSCP_msg_t  RxMsg;

uint08_t vscp_probe_address;			// Address used during initialization
uint08_t vscp_initledfunc;			// Init LED functionality

volatile uint16_t vscp_timer;		// 1 ms timer counter
volatile uint08_t vscp_initbtncnt;	// init button counter
volatile uint08_t vscp_statuscnt;	// status LED counter

// page selector
uint16_t vscp_page_select;

///////////////////////////////////////////////////////////////////////////////
// vscp_init
//

void vscp_init( void )
{
	vscp_initledfunc = VSCP_LED_BLINK1;

	// read the nickname id
	vscp_nickname = vscp_getNickname();		

	//	if zero set to uninitialized
	if ( !vscp_nickname) {
		vscp_nickname = VSCP_ADDRESS_FREE;
	}

	// Init incoming message
	RxMsg.priority = 0;
	RxMsg.vscp_class = 0;
	RxMsg.vscp_type = 0;
  RxMsg.length = 0;		

	// Init outgoing message
	SendMsg.priority = 0;	
	SendMsg.vscp_class = 0;
	SendMsg.vscp_type = 0;
  SendMsg.length = 0;		

  vscp_alarmstatus = 0;				// No alarmstatus
	vscp_probe_address = 0;
	vscp_timer = 0;
	vscp_probe_cnt = 0;
	vscp_page_select = 0;

	// Initial state
	vscp_node_state = VSCP_STATE_STARTUP;
	vscp_node_substate = VSCP_SUBSTATE_NONE;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_check_pstorage
//
// Check control position integrity and restore EEPROM
// if invalid.
//
int08_t vscp_check_pstorage( void )
{
	// controlbyte == 01xxxxxx means initialized
	// everything else is uninitialized
	if ( VSCP_CRC_FREE != vscp_getSegmentCRC()) {
		return TRUE;
	}
	// No nickname yet.
	vscp_setNickname( VSCP_ADDRESS_FREE);
	// No segment CRC yet.
	vscp_setSegmentCRC( VSCP_CRC_FREE);
	vscp_setControlByte( VSCP_CONTROL_STARTUP);		// Initial startup, write allowed

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
// vscp_handleProbeState
//
void vscp_handleProbeState( void )
{
	switch ( vscp_node_substate ) {
		case VSCP_SUBSTATE_NONE:
			if ( VSCP_ADDRESS_FREE != vscp_probe_address ) {
				SendMsg.priority = VSCP_PRIORITY_HIGH;	
				SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
				SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
				SendMsg.length =  1 ;	
				SendMsg.data[ 0 ] = vscp_probe_address;			
				// send the probe
				VSCP_enqMsgTx( &SendMsg, FALSE);
				vscp_node_substate = VSCP_SUBSTATE_INIT_PROBE_SENT;
				vscp_timer = 0;	
			}
			else {
				// No free address -> error
				vscp_node_state = VSCP_STATE_ERROR;
				// Tell system we are giving up
				SendMsg.priority = VSCP_PRIORITY_HIGH;	
				SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
				SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_PROBE_ACK;
				SendMsg.data[ 0 ] = 0xff;				// we are unassigned			
				SendMsg.length =  1 ;
				// send the error event
				VSCP_enqMsgTx( &SendMsg, FALSE);
			}
			break;	

		case VSCP_SUBSTATE_INIT_PROBE_SENT:
			if ( E_OK == VSCP_deqMsgRx( &RxMsg)) {	// incoming message?
				// Yes, incoming message
				if ( ( VSCP_CLASS1_PROTOCOL == RxMsg.vscp_class ) && 
					( VSCP_TYPE_PROTOCOL_PROBE_ACK == RxMsg.vscp_type ) ) {
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
 					vscp_probe_cnt++;	// Another timeout. Repet each probe four times ( 4 * 500ms = 2 S)
					if ( vscp_probe_cnt >= 4) { 
						// Yes we have a timeout, no answer received
						if ( !vscp_probe_address) {			// master controller probe?
							// No master controler on segment, try next node
							vscp_probe_address++;
							vscp_node_substate = VSCP_SUBSTATE_NONE;
						}
						else {
							// No answer, We have found a free address - use it
							vscp_nickname = vscp_probe_address;
							vscp_setNickname( vscp_nickname);
							vscp_setSegmentCRC( VSCP_CRC_DEFAULT);// segment code (non server segment )
							vscp_init(); // Clear Activation, with a Right Discovered NICKNAME 
						}
					}
					else {
						vscp_node_substate = VSCP_SUBSTATE_NONE;
					}
				} // Timeout
			}			
			break;

		case VSCP_SUBSTATE_INIT_PROBE_ACK:
			// Developed in function before
			break;

		default: 
			vscp_node_substate = VSCP_SUBSTATE_NONE;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// vscp_preactive
//
void vscp_handleReactive( void )
{
	if ( E_OK == VSCP_deqMsgRx( &RxMsg)) {	// incoming message?
				// Yes, incoming message

		if (( VSCP_CLASS1_PROTOCOL == RxMsg.vscp_class)  && 
				( VSCP_TYPE_PROTOCOL_SET_NICKNAME == RxMsg.vscp_type) &&
				( VSCP_ADDRESS_FREE == RxMsg.data[0])) {
						
			// Assign nickname
			vscp_nickname = RxMsg.data[2];
			vscp_setNickname( vscp_nickname);
			vscp_setSegmentCRC( VSCP_CRC_DEFAULT);// segment code (non server segment )
			vscp_init(); // Clear Activation, with a Right Asigned NICKNAME 
		}		
	}
	else {
		// Check for time out
		if ( vscp_timer > VSCP_PROBE_TIMEOUT) {
			// Yes, we have a timeout
			vscp_nickname = VSCP_ADDRESS_FREE;
			vscp_setNickname( VSCP_ADDRESS_FREE);
			vscp_init();	//No answer from Server, Clear Restart with VSCP_ADDRESS_FREE
		}				
	}
}

///////////////////////////////////////////////////////////////////////////////
// vscp_goActiveState
//
void vscp_goActiveState( void )
{
	SendMsg.priority = VSCP_PRIORITY_HIGH;	
	SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
	SendMsg.length =  1 ;	// one databyte 
	SendMsg.data[0] = vscp_nickname;			
				
	// send the message
	VSCP_enqMsgTx( &SendMsg, FALSE);
	vscp_initledfunc = VSCP_LED_ON;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_rcv_heartbeat
//
void vscp_handleHeartbeat( void )
{
	if ((5 == RxMsg.length)&&
		 ( vscp_getSegmentCRC()!= RxMsg.data[0])) {
		// Stored CRC are different than received
		// We must be on a different segment
		vscp_setSegmentCRC( RxMsg.data[0]);
		
		// Introduce ourself in the proper way and start from the beginning
		vscp_nickname = VSCP_ADDRESS_FREE;
		vscp_setNickname( VSCP_ADDRESS_FREE);
		vscp_init(); // Clear Restart with VSCP_ADDRESS_FREE
	}		
}

///////////////////////////////////////////////////////////////////////////////
// vscp_rcv_set_nickname
//
void vscp_handleSetNickname( void)
{
	if (( 2 == RxMsg.length) && 
			( vscp_nickname == RxMsg.data[0])) {

		// Yes, we are addressed
		vscp_nickname = RxMsg.data[1];
		vscp_setNickname( vscp_nickname);		
		vscp_setSegmentCRC( VSCP_CRC_DEFAULT);
	}	
}

///////////////////////////////////////////////////////////////////////////////
// vscp_handleDropNickname
//
void vscp_handleDropNickname( void )
{
	if (( 1 == RxMsg.length ) && 
			( vscp_nickname == RxMsg.data[0])) {
		
		// Yes, we are addressed
		vscp_nickname = VSCP_ADDRESS_FREE;
		vscp_setNickname( VSCP_ADDRESS_FREE );
		vscp_init(); // Clear Restart with VSCP_ADDRESS_FREE
	}
}

///////////////////////////////////////////////////////////////////////////////
// vscp_newNodeOnline
//
void vscp_newNodeOnline( void )
{
	if (( 1 == RxMsg.length) && 
			( vscp_nickname == RxMsg.data[0])) {
		// This is a probe which use our nickname we have to respond to tell the new node that
		// ths nickname is in use by US

		SendMsg.length = 0;
		SendMsg.priority = VSCP_PRIORITY_HIGH;	
		SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
		SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_PROBE_ACK;		
		VSCP_enqMsgTx( &SendMsg, FALSE);				
	}
}

///////////////////////////////////////////////////////////////////////////////
// vscp_rcv_readreg
//
void vscp_readStdReg( void )
{
	SendMsg.priority = VSCP_PRIORITY_HIGH;
	SendMsg.length =  2;
	SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
	
	SendMsg.data[0] =  RxMsg.data[1];
	SendMsg.data[1] =  0;  // Default

	if ( VSCP_REG_ALARMSTATUS == RxMsg.data[1]) {

		// * * * Read alarm status register * * * 
		SendMsg.data[1] = vscp_alarmstatus;
		vscp_alarmstatus = 0x00;		// Reset alarm status
	}

	else if ( VSCP_REG_VSCP_MAJOR_VERSION == RxMsg.data[1]) {
		// * * * VSCP Protocol Major Version * * * 
		SendMsg.data[1] = VSCP_MAJOR_VERSION;	
	}

	else if ( VSCP_REG_VSCP_MINOR_VERSION == RxMsg.data[1]) {
 		// * * * VSCP Protocol Minor Version * * * 
		SendMsg.data[1] = VSCP_MINOR_VERSION;	
	}

	else if ( VSCP_REG_NODE_CONTROL == RxMsg.data[1]) {
 		// * * * Reserved * * * 
		SendMsg.data[1] = 0;	
	}

	else if ( VSCP_FIRMWARE_MAJOR_VERSION == RxMsg.data[1]) {
 		// * * * Get firmware Major version * * * 
		SendMsg.data[1] = vscp_getMajorVersion();	
	}

	else if ( VSCP_FIRMWARE_MINOR_VERSION == RxMsg.data[1]) {
 		// * * * Get firmware Minor version * * * 
		SendMsg.data[1] = vscp_getMinorVersion();	
	}

	else if ( VSCP_FIRMWARE_SUB_MINOR_VERSION == RxMsg.data[1]) {
 		// * * * Get firmware Sub Minor version * * * 
		SendMsg.data[1] = vscp_getSubMinorVersion();	
	}

	else if ( RxMsg.data[1] < VSCP_REG_MANUFACTUR_ID0) {
		// * * * Read from persitant locations * * * 
		SendMsg.data[1] = vscp_getUserID( RxMsg.data[1] - VSCP_REG_USERID0);	
	}

	else if (( RxMsg.data[1] > VSCP_REG_USERID4)&&  
					 ( RxMsg.data[1] < VSCP_REG_NICKNAME_ID)) {
 		// * * * Manufacturer ID information * * * 
		SendMsg.data[1] = vscp_getManufacturerId( RxMsg.data[1] - VSCP_REG_MANUFACTUR_ID0); 			
	}

	else if ( VSCP_REG_NICKNAME_ID == RxMsg.data[1]) {
		// * * * nickname id * * * 
		SendMsg.data[1] = vscp_nickname;
	}

	else if ( VSCP_REG_PAGE_SELECT_LSB == RxMsg.data[1]) {
 		// * * * Page select LSB * * * 
		SendMsg.data[1] = ( vscp_page_select & 0xff );	
	}	

	else if ( VSCP_REG_PAGE_SELECT_MSB == RxMsg.data[1]) {
 		// * * * Page select MSB * * * 
		SendMsg.data[1] = ( vscp_page_select >> 8 ) & 0xff;	
	}
	
	else if ( VSCP_BOOT_LOADER_ALGORITHM == RxMsg.data[1]) {
		// * * * Boot loader algorithm * * *
		SendMsg.data[1] = vscp_getBootLoaderAlgorithm();
	}
	
	else if ( VSCP_BUFFER_SIZE == RxMsg.data[1]) {
		// * * * Buffer size * * *
		SendMsg.data[1] = vscp_getBufferSize();	
	}
			
	else if (( RxMsg.data[1] > ( VSCP_REG_GUIID - 1 ))&& 
					 ( RxMsg.data[1] < VSCP_REG_DEVICE_URL )) {
		// * * * GUID * * * 
		SendMsg.data[1] = vscp_getGUID( RxMsg.data[1] - VSCP_REG_GUIID );		
	}

	else {
		// * * * The device URL * * * 
		SendMsg.data[1] = vscp_getMDF_URL( RxMsg.data[1] - VSCP_REG_DEVICE_URL );	
	}
	VSCP_enqMsgTx( &SendMsg, FALSE);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_rcv_writereg
//
void vscp_writeStdReg( void )
{
	SendMsg.priority = VSCP_PRIORITY_MEDIUM;
	SendMsg.length =  2;
	SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
	SendMsg.data[0] = vscp_nickname; 

	if (( RxMsg.data[1] > ( VSCP_REG_VSCP_MINOR_VERSION + 1 )) && 
			( RxMsg.data[1] < VSCP_REG_MANUFACTUR_ID0 )) {
	 
		// * * * User Client ID * * *
		vscp_setUserID( ( RxMsg.data[1] - VSCP_REG_USERID0 ), RxMsg.data[2]);		
		SendMsg.data[1] = vscp_getUserID( ( RxMsg.data[1] - VSCP_REG_USERID0 ) );
	}

	else if ( VSCP_REG_PAGE_SELECT_MSB == RxMsg.data[1]) {
		// * * * Page select register MSB * * *
		vscp_page_select = ( vscp_page_select & 0xff00 ) | ( (uint16_t)RxMsg.data[2] << 8 );
		SendMsg.data[1] = ( vscp_page_select >> 8 ) & 0xff;
	}
	
	else if ( VSCP_REG_PAGE_SELECT_LSB == RxMsg.data[1]) {
		// * * * Page select register LSB * * *
		vscp_page_select = ( vscp_page_select & 0xff ) | RxMsg.data[2];
		SendMsg.data[1] = ( vscp_page_select & 0xff );
	}	

#ifdef ENABLE_WRITE_2PROTECTED_LOCATIONS

	// Write manufacturer id configuration information
	else if ( ( RxMsg.data[1] > VSCP_REG_USERID4 )  &&  ( RxMsg.data[1] < VSCP_REG_NICKNAME_ID ) ) {
		// page select must be 0xffff for writes to be possible
		if ( ( 0xff != ( ( vscp_page_select >> 8 ) & 0xff ) ) || 
				( 0xff != ( vscp_page_select & 0xff ) ) ) {
			// return complement to indicate error
			SendMsg.data[1] = ~RxMsg.data[2];	
		}
		else {
			// Write
			vscp_setManufacturerId( RxMsg.data[1] - VSCP_REG_MANUFACTUR_ID0, RxMsg.data[2] );
			SendMsg.data[1] = vscp_getManufacturerId( RxMsg.data[1] - VSCP_REG_MANUFACTUR_ID0 );
		}
	}
	// Write GUID configuration information
	else if ( ( RxMsg.data[1] > ( VSCP_REG_GUIID - 1 ) ) && ( RxMsg.data[1] < VSCP_REG_DEVICE_URL )  ) {
		// page must be 0xffff for writes to be possible
		if ( ( 0xff != ( ( vscp_page_select >> 8 ) & 0xff ) ) || 
				( 0xff != ( vscp_page_select & 0xff ) ) )  {
			// return complement to indicate error
			SendMsg.data[1] = ~RxMsg.data[2];	
		}
		else {
			vscp_setGUID( RxMsg.data[1] - VSCP_REG_GUIID, RxMsg.data[2] );
			SendMsg.data[1] = vscp_getGUID( RxMsg.data[1] - VSCP_REG_GUIID );
		}
	}
	
#endif

	else {
		// return complement to indicate error
		SendMsg.data[1] = ~RxMsg.data[2];		
	}
	VSCP_enqMsgTx( &SendMsg, FALSE);	
}

///////////////////////////////////////////////////////////////////////////////
// vscp_goBootloaderMode
//

void vscp_goBootloaderMode( void )
{	 											
	// OK, We should enter boot loader mode
	// 	First, activate bootloader mode
	writeEEPROM( VSCP_EEPROM_BOOTLOADER_FLAG, VSCP_BOOT_FLAG );
					
	//_asm goto _startup reset _endasm
	_asm reset _endasm
}