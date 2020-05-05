
/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * Copyright (C) 2011-2012 Ake Hedman, Grodans Paradis AB
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

#include <vscp_compiler.h>
#include <vscp_projdefs.h>
#include "string.h"
#include "projdefs.h"
#include "checkcfg.h" 
#include "mac.h"
#include "tick.h"
#include "inttypes.h"
#include "vscp_class.h"      
#include "vscp_type.h"
#include "vscp_firmware_level2.h"
#include "vscp_raw_ethernet.h"

extern MAC_ADDR broadcastTargetMACAddr;

// Important Note!
// For some reason it has been impossible to use a pointer
// to an ebent as an argument here. I have never been able
// to find out why.  Therfore the global event is used.
extern vscpEvent wrkEvent;

//////////////////////////////////////////////////////////////////////////////////////////////
// vscp_sendRawPacket
//

int8_t vscp_sendRawPacket( void )
{
    uint8_t i;
    BUFFER MyTxBuffer;
    BYTE buf[ 60 ];
    MAC_ADDR TargetMACAddr;
    TICK tick = TickGet();
    
    MyTxBuffer = MACGetTxBuffer( TRUE );

    if ( !MACIsTxReady( FALSE ) ) return FALSE;

    // Nill it
    memset( buf, 0, sizeof( buf ) );

    // Do not respond if there is no room
    if ( MyTxBuffer == INVALID_BUFFER ) {
        return FALSE;
    }

    MACSetTxBuffer( MyTxBuffer, 0 );

    // Destination address - Always MAC broadcast address
    TargetMACAddr.v[ 0 ] = 0xff;
    TargetMACAddr.v[ 1 ] = 0xff;
    TargetMACAddr.v[ 2 ] = 0xff;
    TargetMACAddr.v[ 3 ] = 0xff;
    TargetMACAddr.v[ 4 ] = 0xff;
    TargetMACAddr.v[ 5 ] = 0xff;

    // Prepare the VSCP payload

    // Version
    buf[ 0 ] = 0x00;

    // VSCP head
    buf[ 1 ] = ( wrkEvent.head >> 24 ) & 0xff;
    buf[ 2 ] = ( wrkEvent.head >> 16 ) & 0xff;
    buf[ 3 ] = ( wrkEvent.head >> 8 ) & 0xff;
    buf[ 4 ] = wrkEvent.head & 0xff;

    // VSCP sub source address
    buf[ 5 ] = 0xaa;
    buf[ 6 ] = 0x55;

    // Timestamp
    buf[ 7 ] = tick >> 24;
    buf[ 8 ] = tick >> 16;
    buf[ 9 ] = tick >> 8;
    buf[ 10 ] = tick & 0xff;

    // OBID
    buf[ 11 ] = 0x00;
    buf[ 12 ] = 0x00;
    buf[ 13 ] = 0x00;
    buf[ 14 ] = 0x00;

    // VSCP class
    buf[ 15 ] = ( wrkEvent.vscp_class >> 8 ) & 0xff;
    buf[ 16 ] = wrkEvent.vscp_class & 0xff;

    // VSCP type
    buf[ 17 ] = ( wrkEvent.vscp_type >> 8 ) & 0xff;
    buf[ 18 ] = wrkEvent.vscp_type & 0xff;

    // DataSize
    buf[ 19 ] = ( wrkEvent.sizeData >> 8 ) & 0xff;
    buf[ 20 ] = wrkEvent.sizeData & 0xff;

    for ( i = 0; i<wrkEvent.sizeData; i++ ) {
        buf[ 21 + i ] = wrkEvent.data[ i ];
    }

    // Write the Ethernet Header to the MAC's TX buffer. The last parameter
    // (dataLen) is the length of the data to follow.
    MACPutHeader( &broadcastTargetMACAddr, MAC_VSCP, 6 + 6 + 2 + 21 + wrkEvent.sizeData );
    MACPutArray( buf, sizeof( buf ) );
    MACFlush();


    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// vscp_getRawPacket
//

int8_t vscp_getRawPacket( void  )
{
    uint8_t i;
    uint8_t buf[ 21 ];
    MAC_ADDR remoteMAC;
    uint8_t type;

    // Check for incoming data
    if ( !MACGetHeader( &remoteMAC, &type) ) {
        return FALSE;
    }

    // Ignore non VSCP frames
    if ( ETHER_VSCP != type ) {
        MACDiscardRx();
        return FALSE;
    }

    // Read the VSCP package header
    // version, head, subaddr, timestamp, obid, class, type, datasize
    MACGetArray( buf, 21 );

    // Only version 0 is accepted here
    if ( 0x00 != buf[ 0 ] ) {
        MACDiscardRx();
        return FALSE;
    }

    // Head
    wrkEvent.head = buf[ 4 ];
    
    // sender GUID
    memcpy( (void *)wrkEvent.GUID, (void *)buf, 16 );
    
    // Fill in remote MAC
    for ( i=0; i<6; i++ ) {
        wrkEvent.GUID[ 7 + i ] = remoteMAC.v[ i ];
    }

    // Timestamp 7,8,9,10
    // obdi in 11,12,13,14

    // Class
    wrkEvent.vscp_class = ( ( (uint16_t)buf[ 15 ] ) << 8 ) + buf[ 16 ];

    // Type
    wrkEvent.vscp_type =  ( ( (uint16_t)buf[ 17 ] ) << 8 ) + buf[ 18 ];

    // Size
    wrkEvent.sizeData =   ( ( (uint16_t)buf[ 19 ] ) << 8 ) + buf[ 20 ];

    // Check that size is correct and adjust if not.
    if ( wrkEvent.sizeData > LIMITED_DEVICE_DATASIZE ) {
        wrkEvent.sizeData = LIMITED_DEVICE_DATASIZE;
    }

    // Copy in data
    MACGetArray( wrkEvent.data, wrkEvent.sizeData );
  
    // Free buffer
    MACDiscardRx();

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// SendTestVSCPPacket
//

BOOL SendTestVSCPPacket( void ) 
{
    BUFFER MyTxBuffer;
    BYTE buf[ 60 ];
    MAC_ADDR TargetMACAddr;  
    MyTxBuffer = MACGetTxBuffer( TRUE );
    
    if ( !MACIsTxReady( FALSE ) ) return FALSE;
       
    // Nill it
    memset( buf, 0, sizeof( buf ) );
    
    // Do not respond if there is no room 
    if ( MyTxBuffer == INVALID_BUFFER ) {
        return FALSE;
    }    

    MACSetTxBuffer( MyTxBuffer, 0 );
    
    // Destination address - Always MAC broadcast address
    TargetMACAddr.v[ 0 ] = 0xff; 
    TargetMACAddr.v[ 1 ] = 0xff;
    TargetMACAddr.v[ 2 ] = 0xff;
    TargetMACAddr.v[ 3 ] = 0xff;
    TargetMACAddr.v[ 4 ] = 0xff;
    TargetMACAddr.v[ 5 ] = 0xff;
    
    // Prepare the VSCP payload
    
    // Version
    buf[ 0 ] = 0x00;
    
    // VSCP head
    buf[ 1 ] = 0x00;
    buf[ 2 ] = 0x00;
    buf[ 3 ] = 0x00;
    buf[ 4 ] = 0x00;
    
    // VSCP sub source address
    buf[ 5 ] = 0xaa;
    buf[ 6 ] = 0x55;
    
    // Timestamp
    buf[ 7 ] = 0x00;
    buf[ 8 ] = 0x00;
    buf[ 9 ] = 0x00;
    buf[ 10 ] = 0x00;
    
    // OBID
    buf[ 11 ] = 0x00;
    buf[ 12 ] = 0x00;
    buf[ 13 ] = 0x00;
    buf[ 14 ] = 0x00;
    
    // VSCP class
    buf[ 15 ] = 0x02;	// Class=512 (0x214) - Information
    buf[ 16 ] = 0x14;
    
    // VSCP type 
    buf[ 17 ] = 0x00;	// Type = 9 (0x09) Node heartbeat.
    buf[ 18 ] = 0x09;
    
    // DataSize
    buf[ 19 ] = 0x00;	// Size for data part
    buf[ 20 ] = 0x00;
    
    // Write the Ethernet Header to the MAC's TX buffer. The last parameter 
    // (dataLen) is the length of the data to follow.
    MACPutHeader( &TargetMACAddr, MAC_VSCP, 21  );
    MACPutArray( buf, sizeof( buf ) );
    MACFlush();

    return TRUE;		
}


