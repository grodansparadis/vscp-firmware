/**
 * @brief           VSCP UDP code for the nova reference project
 * @file            vscp_udp.c
 * @author          Ake Hedman, eurosource, <a href="www.vscp.org">VSCP Project</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_vscp
 *
 *
 * @section description Description
 **********************************
 * This module contains code that implements that VSCP UDP event handling.
 *
 *********************************************************************/


/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Copyright (C) 1995-2011 Ake Hedman, eurosource
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

#include <string.h>
#include <inttypes.h>
#include <projdefs.h>
#include "debug.h"
#include <appcfg.h>

#include <net/stacktsk.h>
#include <net/tick.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <net/delay.h>
#include <net/xeeprom.h>

#include <vscp_2.h>
#include "i:/src/vscp/common/vscp_class.h"      // Must use batch script i VSCP root
#include "i:/src/vscp/common/vscp_type.h"

#include <crc.h>
#include <eeprom.h>

#include "version.h"
#include "vscp_udp.h"


// From vscp_task.c
extern BOOL bBootLoadMode;
extern uint16_t vscp_page_select;


#if defined(VSCP_USE_UDP )


// Socket for incoming VSCP events
UDP_SOCKET vscp_udp_receivesocket; 

// Socket for outgoing VSCP events
UDP_SOCKET vscp_udp_transmitsocket;


///////////////////////////////////////////////////////////////////////////////
// vscp_udpinit
//

int8_t vscp_udpinit( void )
{
	NODE_INFO remote_node;
	
	remote_node.IPAddr.v[ 0 ] = 0xff;	// Broadcast
	remote_node.IPAddr.v[ 1 ] = 0xff;
	remote_node.IPAddr.v[ 2 ] = 0xff;
	remote_node.IPAddr.v[ 3 ] = 0xff;
	remote_node.MACAddr.v[ 0 ] = 0xff;
	remote_node.MACAddr.v[ 1 ] = 0xff;
	remote_node.MACAddr.v[ 2 ] = 0xff;
	remote_node.MACAddr.v[ 3 ] = 0xff;
	remote_node.MACAddr.v[ 4 ] = 0xff;
	remote_node.MACAddr.v[ 5 ] = 0xff;
	
	// setup receive socket
	vscp_udp_receivesocket = UDPOpen( VSCP_LEVEL2_UDP_PORT, &remote_node, NULL );
	if ( INVALID_SOCKET == vscp_udp_receivesocket ) return FALSE;
	
	// Setup transmit socket	
	vscp_udp_transmitsocket = UDPOpen( 30200, &remote_node, VSCP_LEVEL2_UDP_PORT );
	if ( INVALID_SOCKET == vscp_udp_transmitsocket ) {
		UDPClose( vscp_udp_receivesocket );
		return FALSE;
	}
}

#endif

#if defined(VSCP_USE_UDP )

///////////////////////////////////////////////////////////////////////////////
// vscp_sendUDPEvent
//

int8_t vscp_sendUDPEvent( PVSCPMSG pmsg )
{
	int i;
	
	if ( UDPIsPutReady( vscp_udp_transmitsocket ) ) {
		
		pmsg->crc = crcSlow( (unsigned char *)pmsg, sizeof( vscpMsg ) - 2  );
		
		UDPPut( pmsg->head );
		
		UDPPut( ( pmsg->vscp_class >> 8 ) & 0xff );
		UDPPut( pmsg->vscp_class & 0xff );
		
		UDPPut( ( pmsg->vscp_type >> 8 ) & 0xff );
		UDPPut( pmsg->vscp_type & 0xff );
		
		for ( i=0; i<16; i++ ) {
			UDPPut( vscp_getGUID( i ) );
		}
		
		UDPPut( ( pmsg->sizeData >> 8 ) & 0xff );
		UDPPut( pmsg->sizeData & 0xff );
		
		for ( i=0; i<pmsg->sizeData; i++ ) {
			UDPPut( pmsg->data[ i ] );
		}
		
		UDPPut( ( pmsg->crc >> 8 ) & 0xff );
		UDPPut( pmsg->crc & 0xff );
		
		UDPFlush();
		return TRUE;
	}	
	
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// vscp_getUDPEvent
//

int8_t vscp_getUDPEvent( PVSCPMSG pmsg )
{
	int i;
	BYTE b1, b2;
	crc  remainder = INITIAL_REMAINDER;

	
	// Must be something to receive
	if ( !UDPIsGetReady( vscp_udp_receivesocket ) ) return FALSE;
	
	// head
	if ( !UDPGet( &b1 ) ) return FALSE;
	crcSlow_bithandler( b1, &remainder );	// CRC calculation
	pmsg->head = b1;
	
	// class
	if ( !UDPGet( &b1 ) ) return FALSE;
	crcSlow_bithandler( b1, &remainder );	// CRC calculation
	if ( !UDPGet( &b2 ) ) return FALSE;
	crcSlow_bithandler( b2, &remainder );	// CRC calculation
	pmsg->vscp_class = ( b1 << 8 ) + b2;
	
	// type
	if ( !UDPGet( &b1 ) ) return FALSE;
	crcSlow_bithandler( b1, &remainder );	// CRC calculation
	if ( !UDPGet( &b2 ) ) return FALSE;
	crcSlow_bithandler( b2, &remainder );	// CRC calculation
	pmsg->vscp_type = ( b1 << 8 ) + b2;
	
	// GUID
	for ( i=0; i<16; i++ ) {
		if ( !UDPGet( &b1 ) ) return FALSE;
		crcSlow_bithandler( b1, &remainder );	// CRC calculation
		pmsg->GUID[ i ] = b1;	
	}
	
	// Size
	if ( !UDPGet( &b1 ) ) return FALSE;
	crcSlow_bithandler( b1, &remainder );	// CRC calculation
	if ( !UDPGet( &b2 ) ) return FALSE;
	crcSlow_bithandler( b2, &remainder );	// CRC calculation
	pmsg->sizeData = ( b1 << 8 ) + b2;
	
#ifdef VSCP_LEVEL2_LIMITED_DEVICE
	if ( pmsg->sizeData > LIMITED_DEVICE_DATASIZE ) return FALSE;
#else
	if ( pmsg->sizeData > (512-25) ) return FALSE;
#endif	
	
	// Data
	for ( i=0; i<pmsg->sizeData; i++ ) {
		if ( !UDPGet( &b1 ) ) return FALSE;
		crcSlow_bithandler( b1, &remainder );	// CRC calculation
		pmsg->data[ i ] = b1;	
	}
	
	// crc
	if ( !UDPGet( &b1 ) ) return FALSE;
	crcSlow_bithandler( b1, &remainder );	// CRC calculation
	if ( !UDPGet( &b2 ) ) return FALSE;
	crcSlow_bithandler( b2, &remainder );	// CRC calculation
	pmsg->crc = ( b1 << 8 ) + b2;
			
	if ( ( remainder ^ FINAL_XOR_VALUE ) ) return FALSE;
	
	// we have the information we need
	UDPDiscard();
	    
	return TRUE;
}
#endif


