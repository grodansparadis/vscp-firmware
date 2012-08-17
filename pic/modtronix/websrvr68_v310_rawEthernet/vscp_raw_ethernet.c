
/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * Copyright (C) 2011 Ake Hedman, eurosource
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
#include "inttypes.h"
#include "vscp_class.h"      
#include "vscp_type.h"
#include "vscp_firmware_level2.h"
#include "vscp_raw_ethernet.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// vscp_sendRawPacket
//

int8_t vscp_sendRawPacket( vscpEvent *pevent )
{
	return -1;
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
    
    // Do not respond if there is no room to generate the ARP reply
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
    buf[ 15 ] = 0x02;	// Class=512 (0x200) - Control
    buf[ 16 ] = 0x00; 
    
    // VSCP type 
    buf[ 17 ] = 0x00;	// Type = 2 (0x02) New node on line / Probe.
    buf[ 18 ] = 0x02;
    
    // DataSize
    buf[ 19 ] = 0x00;	// Size for data part
    buf[ 20 ] = 0x00;
    
    // Write the Ethernet Header to the MAC's TX buffer. The last parameter (dataLen) is the length
    // of the data to follow.
    MACPutHeader( &TargetMACAddr, MAC_VSCP, 21 );
    MACPutArray( buf, sizeof( buf ) );
    MACFlush();

    return TRUE;		
}


