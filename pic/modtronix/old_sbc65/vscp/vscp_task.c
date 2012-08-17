/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	Version 0.01 2004-04-06
 * 	akhe@eurosource.se
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
#include "projdefs.h"
#include "debug.h"
#include "stacktsk.h"
#include "appcfg.h"
#include "tick.h"
#include <vscp_2.h>
#include <vscp_class.h>
#include <vscp_type.h>
#include <udp.h>
#include <crc.h>
#include <xeeprom.h>
#include <eeprom.h>
#include "vscp_task.h"

// For debug only.
#include "tcp.h"
#include "udp.h"
#include "icmp.h"
#include "delay.h"

// VSCP data

// Socket for incoming VSCP events
UDP_SOCKET VSCP_receivesocket; 

// Socket for outgoing VSCP events
UDP_SOCKET VSCP_transmitsocket; 

// GUID for this node (MSB - LSB)
/*
const rom unsigned char vscp_guid[] = { 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
	MY_DEFAULT_MAC_BYTE6, 
	MY_DEFAULT_MAC_BYTE5, 
	MY_DEFAULT_MAC_BYTE4, 
	MY_DEFAULT_MAC_BYTE3, 
	MY_DEFAULT_MAC_BYTE2, 
	MY_DEFAULT_MAC_BYTE1, 
	0x90, 0x01
};
*/

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

// The device URL (max 32 characters including null termination)
const rom char vscp_deviceURL[] = "www.eurosource.se/sbc65nova1.xml";


// From udp.c
extern UDP_SOCKET_INFO  UDPSocketInfo[ MAX_UDP_SOCKETS ];
extern UDP_SOCKET activeUDPSocket;


_u16 vscp_page_select;		// Current register page
BOOL bBootLoadMode;			// TRUE if in bootloader mode
_u8 vscp_current_bootblock;	// Block that is loading

///////////////////////////////////////////////////////////////////////////////
// vscp2_udpinit
//

int vscp2_udpinit( void )
{
	vscpMsg outMsg;
	NODE_INFO remote_node;
	
	bBootLoadMode = FALSE;	// Not in bootload mode
	vscp_page_select = 0;	// Default page
	
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
	VSCP_receivesocket = UDPOpen( VSCP_LEVEL2_UDP_PORT, &remote_node, NULL );
	if ( INVALID_SOCKET == VSCP_receivesocket ) return FALSE;
	
	// Setup transmit socket	
	VSCP_transmitsocket = UDPOpen( 30200, &remote_node, VSCP_LEVEL2_UDP_PORT );
	if ( INVALID_SOCKET == VSCP_transmitsocket ) {
		UDPClose( VSCP_receivesocket );
		return FALSE;
	}
	
	// Send heartbeat
	outMsg.head = ( VSCP2_PRIORITY_MEDIUM << 5 );
	outMsg.sizeData = 0;
	outMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	outMsg.vscp_type = VSCP_TYPE_SEGCTRL_HEARTBEAT;
	
	vscp2_sendMsg( &outMsg );
}

///////////////////////////////////////////////////////////////////////////////
// vscp2_sendMsg
//

int vscp2_sendMsg( PVSCPMSG pmsg )
{
	int i;
	
	if ( UDPIsPutReady( VSCP_transmitsocket ) ) {
		
		pmsg->crc = crcSlow( (unsigned char *)pmsg, sizeof( vscpMsg ) - 2  );
		
		UDPPut( pmsg->head );
		
		UDPPut( ( pmsg->vscp_class >> 8 ) & 0xff );
		UDPPut( pmsg->vscp_class & 0xff );
		
		UDPPut( ( pmsg->vscp_type >> 8 ) & 0xff );
		UDPPut( pmsg->vscp_type & 0xff );
		
		for ( i=0; i<16; i++ ) {
			UDPPut( getVSCP_GUID( i ) );
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
// vscp2_receiveMsg
//

int vscp2_receiveMsg( PVSCPMSG pmsg )
{
	int i;
	BYTE b1, b2;
	
	// Must be something to receive
	if ( !UDPIsGetReady( VSCP_receivesocket ) ) return FALSE;
	
	// head
	if ( !UDPGet( &b1 ) ) return FALSE;
	pmsg->head = b1;
	
	// class
	if ( !UDPGet( &b1 ) ) return FALSE;
	if ( !UDPGet( &b2 ) ) return FALSE;
	pmsg->vscp_class = ( b1 << 8 ) + b2;
	
	// type
	if ( !UDPGet( &b1 ) ) return FALSE;
	if ( !UDPGet( &b2 ) ) return FALSE;
	pmsg->vscp_type = ( b1 << 8 ) + b2;
	
	// GUID
	for ( i=0; i<16; i++ ) {
		if ( !UDPGet( &b1 ) ) return FALSE;
		pmsg->GUID[ i ] = b1;	
	}
	
	// Size
	if ( !UDPGet( &b1 ) ) return FALSE;
	if ( !UDPGet( &b2 ) ) return FALSE;
	pmsg->sizeData = ( b1 << 8 ) + b2;
	
	// Data
	for ( i=0; i<pmsg->sizeData; i++ ) {
		if ( !UDPGet( &b1 ) ) return FALSE;
		pmsg->data[ i ] = b1;	
	}
	
	// crc
	if ( !UDPGet( &b1 ) ) return FALSE;
	if ( !UDPGet( &b2 ) ) return FALSE;
	pmsg->crc = ( b1 << 8 ) + b2;
	
	if ( !crcSlow( (unsigned char *)pmsg, sizeof( vscpMsg ) ) ) return FALSE;
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//	vscp2_Task
//

void vscp2_Task( void )
{
	int i;
	static TICK lastTick = 0;
	unsigned short remainder= INITIAL_REMAINDER;
	unsigned short eeprompos;
	vscpMsg inMsg;
	vscpMsg outMsg;
	
	// Check if we should send a heartbeat (every minute)
	if ( TickGetDiff( TickGet(), lastTick ) >= ( TICK_SECOND * 10 ) ) {
		
    	lastTick = TickGet();
       	
       	// Send heartbeat
		outMsg.head = ( VSCP2_PRIORITY_MEDIUM << 5 );
		outMsg.sizeData = 0;
		outMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
		outMsg.vscp_type = VSCP_TYPE_SEGCTRL_HEARTBEAT;
	
		vscp2_sendMsg( &outMsg );
		return; // We must give other processes a chance to work.
    }
    
    // Incoming event
    if ( UDPIsGetReady( VSCP_receivesocket ) && vscp2_receiveMsg( &inMsg ) ) {
	    
	    //UDPSocketInfo[ activeUDPSocket ].remoteNode.IPAddr.v[ 0 ];
	    //UDPSocketInfo[ activeUDPSocket ].remoteNode.IPAddr.v[ 1 ];
	   	//UDPSocketInfo[ activeUDPSocket ].remoteNode.IPAddr.v[ 2 ];
	   	//UDPSocketInfo[ activeUDPSocket ].remoteNode.IPAddr.v[ 3 ];
	   
	    // we have the information we need
	    UDPDiscard();
	    
		if ( ( VSCP_CLASS1_PROTOCOL == inMsg.vscp_class ) || 
			( VSCP_CLASS2_LEVEL1_PROTOCOL == inMsg.vscp_class )  ) {
			
			// Handle Read register
			if ( VSCP_TYPE_READ_REGISTER == inMsg.vscp_type ) {
				
				// Must be addressed to us
				for ( i=0; i<16; i++ ) {
					if ( getVSCP_GUID( i ) != inMsg.data[ i ] ) return;
				}
				
				vscp2_ReadReg_Level1( &inMsg, &outMsg );
				
			}
			
			// Handle write register
			else if ( VSCP_TYPE_WRITE_REGISTER == inMsg.vscp_type ) {
				
				// Must be addressed to us
				for ( i=0; i<16; i++ ) {
					if ( getVSCP_GUID( i ) != inMsg.data[ i ] ) return;
				}
				
				vscp2_WriteReg_Level1( &inMsg, &outMsg );
					
			}
			
			// Handle Enter Boot Loader Mode
			else if ( VSCP_TYPE_ENTER_BOOT_LOADER == inMsg.vscp_type ) {
				
				// Must be addressed to us
				for ( i=0; i<16; i++ ) {
					if ( getVSCP_GUID( i ) != inMsg.data[ i ] ) return;
				}
				
				// Bootloader algorithm must be VSCP 
				if ( 0 != inMsg.data[ 16 ] ) {
 	
					outMsg.head = ( VSCP2_PRIORITY_HIGH << 5 );
					outMsg.sizeData = 1;
					outMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
					outMsg.vscp_type = VSCP_TYPE_NACK_BOOT_LOADER;
					outMsg.data[ 0 ] = 1; // Not supported algorithm
					vscp2_sendMsg( &outMsg );
					return;
				}
					
				// Enter bootloader mode
				bBootLoadMode = TRUE;
				
				// Erase the internal EEPROM and bootload part
				// of the external EEPROM
				XEEBeginRead( EEPROM_CONTROL, i );
				for ( i=1024; i<=0xffff; i++) {
					XEEWrite( 0xff );
				}
				
 				outMsg.head = ( VSCP2_PRIORITY_HIGH << 5 );
				outMsg.sizeData = 24;
				outMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
				outMsg.vscp_type = VSCP_TYPE_ACK_BOOT_LOADER;
				
	
				// Fill in GUID
				for ( i=0; i<16; i++ ) {
					outMsg.data[ i ] = getVSCP_GUID( i );
				}	
				
				// We have 64K flash on this chip but the first
				// two are used for the bootloader code so
				// we have 62K for program. This has been 
				// organized as 248 block of 256 bytes each.
				// Not that this has nothing to do with the actual 
				// flash pages of the chip which in this case are
				// handled by the low level bootloader code. 	
				outMsg.data[ 16 ] =  0;   
				outMsg.data[ 17 ] =  0;
				outMsg.data[ 18 ] =  0;
				outMsg.data[ 19 ] =  0xff;	// 256 byte blocks
				outMsg.data[ 20 ] =  0;
				outMsg.data[ 21 ] =  0;
				outMsg.data[ 22 ] =  0;
				outMsg.data[ 23 ] =  0xf8;
					
				vscp2_sendMsg( &outMsg );
				return;
				
			}
			
			// Get EDA matrix info
			else if ( VSCP_TYPE_GET_MATRIX_INFO == inMsg.vscp_type ) {
				
				outMsg.head = ( VSCP2_PRIORITY_MEDIUM << 5 );
				outMsg.sizeData = 23;
				outMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
				outMsg.vscp_type = VSCP_TYPE_GET_MATRIX_INFO_RESPONSE;
				
				// Fill in GUID
				for ( i=0; i<16; i++ ) {
					 getVSCP_GUID( i );
				}
				
				outMsg.data[ 16 ] = 16;	// 16 rows in matrix
				outMsg.data[ 17 ] = 16;	// Matrix offset
				outMsg.data[ 18 ] = 0; 	// Page stat
				outMsg.data[ 19 ] = 0;
				outMsg.data[ 20 ] = 0;	// Page end
				outMsg.data[ 21 ] = 0;
				outMsg.data[ 22 ] = 0;	// Size of row for Level II
				
				vscp2_sendMsg( &outMsg );
				return;
				
			}
			
			// Handle Get EDA Matrix Info
			else if ( VSCP_TYPE_GET_MATRIX_INFO == inMsg.vscp_type ) {
				
				// Must be addressed to us
				for ( i=0; i<16; i++ ) {
					if ( getVSCP_GUID( i ) != inMsg.data[ i ] ) return;
				}		
			}
			
			//**************************************************************************
			//                        B O O T L O A D  M O D E
			//**************************************************************************
	
			// Bootloader: Start bootloader BLOCK Data transfer
			else if ( bBootLoadMode && VSCP_TYPE_START_BLOCK == inMsg.vscp_type ) {
					
				// Must be a valid block number
				if ( ( 0 != inMsg.data[ 16 ] ) ||
						( 0 != inMsg.data[ 17 ] ) || 
						( 0 != inMsg.data[ 18 ] ) ||
						( ( 247 + 2 ) < inMsg.data[ 19 ] )  ) {
					return;			
				}
				
				// Blockno for 64K device:
				// =======================
				//		0 - 0x0800
				//		1 -	0x0900 
				//		...
				//		247 - 0xff00
				//		248 - 0x0400
				//		249 - 0x0500
				vscp_current_bootblock = inMsg.data[ 19 ];	// Save block
				
				if ( vscp_current_bootblock < 248 ) {					
					eeprompos = 0x400 + ( vscp_current_bootblock - 248 ) * 0xff;
				}
				else {
					eeprompos = 0x800 + vscp_current_bootblock * 0xff;
				}
				return;
				
			}
			
			// Bootloader: Data block
			else if ( bBootLoadMode && VSCP_TYPE_BLOCK_DATA == inMsg.vscp_type ) {
				
				outMsg.head = ( VSCP2_PRIORITY_MEDIUM << 5 );
				outMsg.sizeData = 22;
				outMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
				
				remainder = INITIAL_REMAINDER;
				XEEBeginRead( EEPROM_CONTROL, eeprompos );
    			for ( i = 0; i<( inMsg.sizeData - 16 ); i++ ) {
					crcSlow_bithandler( inMsg.data[ i + 16 ], &remainder );
					XEEWrite( inMsg.data[ i + 16 ] );
					eeprompos++;
				}
				
				outMsg.sizeData = 6;
				outMsg.head = ( VSCP2_PRIORITY_HIGH << 5 );
				outMsg.vscp_type = VSCP_TYPE_BLOCK_DATA_ACK;
				outMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
				outMsg.vscp_type = VSCP_TYPE_BLOCK_DATA_ACK;
				
				outMsg.data[ 0 ] = (  getCRC( &remainder ) >> 8 ) & 0xff;
				outMsg.data[ 1 ] = getCRC( &remainder ) & 0xff;
				outMsg.data[ 2 ] = 0;
				outMsg.data[ 3 ] = 0;
				outMsg.data[ 4 ] = ( eeprompos >> 8 ) & 0xff;
				outMsg.data[ 5 ] = eeprompos & 0xff;
				
				vscp2_sendMsg( &outMsg );
				return;
			}
			
			// Bootloader: Program data block
			else if ( bBootLoadMode && VSCP_TYPE_PROGRAM_BLOCK_DATA == inMsg.vscp_type ) {
			
				//  Does nothing here - we just ACK
				outMsg.sizeData = 4;
				outMsg.head = ( VSCP2_PRIORITY_HIGH << 5 );
				outMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
				outMsg.vscp_type = VSCP_TYPE_PROGRAM_BLOCK_DATA_ACK;
				
				outMsg.data[ 0 ] = 0;
				outMsg.data[ 1 ] = 0;
				outMsg.data[ 2 ] = ( eeprompos >> 8 ) & 0xff;
				outMsg.data[ 3 ] = eeprompos & 0xff;
				
				vscp2_sendMsg( &outMsg );
				return;
					
			}
			
			// Bootloader: Activate the image
			else if ( bBootLoadMode && VSCP_TYPE_ACTIVATE_NEW_IMAGE == inMsg.vscp_type ) {
				
			}	   	    	
	    	
	    } // CLASS1.PROTOCOL
			
		else if ( ( VSCP_CLASS1_ALARM == inMsg.vscp_class ) || 
				( VSCP_CLASS2_LEVEL1_ALARM == inMsg.vscp_class )  ) {
			
		}
 		else if ( ( VSCP_CLASS1_MEASUREMENT == inMsg.vscp_class ) || 
				( VSCP_CLASS2_LEVEL1_MEASUREMENT == inMsg.vscp_class )  ) {
		
		}
		else if ( ( VSCP_CLASS1_INFORMATION == inMsg.vscp_class ) || 
				( VSCP_CLASS2_LEVEL1_INFORMATION == inMsg.vscp_class )  ) {
			
		}
		else if ( ( VSCP_CLASS1_CONTROL == inMsg.vscp_class ) || 
				( VSCP_CLASS2_LEVEL1_CONTROL == inMsg.vscp_class )  ) {
			
		}
		
		// Handle EDA
		vscp2_DM_Handler( &inMsg );
				    
    }
    
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP2MajorVersion
//

_u8 getVSCP2MajorVersion( void )
{
	return FIRMWARE_MAJOR_VERSION;	
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP2MinorVersion
//

_u8 getVSCP2MinorVersion( void )
{
	return FIRMWARE_MINOR_VERSION;	
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP2SubMinorVersion
//

_u8 getVSCP2SubMinorVersion( void )
{
	return FIRMWARE_SUB_MINOR_VERSION;	
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP_GUID
//

_u8 getVSCP_GUID( _u8 idx )
{
	if ( idx < 7 ) {
		return 0xff;
	}
	else if ( 7 == idx ) {
		return 0xfe;
	}
	else if ( idx <	14 ) {
		return AppConfig.MyMACAddr.v[ idx - 8 ];	
	}
	else if ( 14 == idx  ) {
		return MSB( AppConfig.SerialNumber );
	}
 	else if ( 15 == idx  ) {
		return LSB( AppConfig.SerialNumber );
	}
	
 	return 0;	
}

///////////////////////////////////////////////////////////////////////////////
// vscp2_DM_Handler
//

_u8 getVSCP_DeviceURL( _u8 idx )
{
	if ( idx < 16 ) {
		return vscp_deviceURL[ idx ];
	}	
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP_ManufacturerID
//

_u8 getVSCP_ManufacturerID( _u8 idx )
{
	if ( idx < 8 ) {
		return vscp_manufacturer_id[ idx ];
	}	
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP_ManufacturerID
//

_u8 getVSCP_UserID( _u8 idx )
{
	if ( idx < 5 ) {
		return readEEPROM( idx + VSCP_EEPROM_REG_USERID );	
	}	
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// writeVSCP_ManufacturerID
//

_u8 writeVSCP_UserID( _u8 idx, _u8 data )
{
	writeEEPROM( idx + VSCP_EEPROM_REG_USERID, data );		
	return getVSCP_UserID( idx );
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP_ControlByte
//
	
_u8 getVSCP_ControlByte( void )
{
	return readEEPROM( EEPROM_CONTROL );	
}

///////////////////////////////////////////////////////////////////////////////
// writeVSCP_ControlByte
//

_u8 writeVSCP_ControlByte( _u8 data )
{
	writeEEPROM( EEPROM_CONTROL, data );
	return readEEPROM( EEPROM_CONTROL );
}

///////////////////////////////////////////////////////////////////////////////
// readVSCP_UserRegister_Level1
//

_u8 readVSCP_UserRegister_Level1( _u8 idx )
{
	_u8 rv;
	WORD wtmp;
	
	if ( idx < sizeof(AppConfig) ) {
		// This is a read of configuration data
		rv = *( (_u8 *)&AppConfig + idx );	
	}
	
	switch( idx ) {
	
		case VSCP_REGISTER_PORTA_STATE: 
			rv = PORTA;
			break;
			
		case VSCP_REGISTER_PORTB_STATE: 
			rv = PORTB;
			break;	
			
		case VSCP_REGISTER_PORTC_STATE: 
			rv = PORTC;
			break;	
			
		case VSCP_REGISTER_PORTF_STATE: 
			rv = PORTF;
			break;	
			
		case VSCP_REGISTER_PORTG_STATE: 
			rv = PORTG;
			break;	
			
		case VSCP_ADC0_MSB: 
			rv = AdcValues[ 0 ] >> 8;
			break;
			
		case VSCP_ADC0_LSB:
			rv = AdcValues[ 0 ] & 0xff;
			break;	
			
		case VSCP_ADC1_MSB:
			rv = AdcValues[ 1 ] >> 8;;
			break;
			
		case VSCP_ADC1_LSB:
			rv = AdcValues[ 1 ] & 0xff;
			break;	
			
		case VSCP_ADC2_MSB:
			rv = AdcValues[ 2 ] >> 8;;
			break;
			
		case VSCP_ADC2_LSB:
			rv = AdcValues[ 2 ] & 0xff;
			break;	
			
		case VSCP_ADC3_MSB:
			rv = AdcValues[ 3 ] >> 8;;
			break;
			
		case VSCP_ADC3_LSB:
			rv = AdcValues[ 3 ] & 0xff;
			break;	
			
		case VSCP_ADC4_MSB:
			rv = AdcValues[ 4 ] >> 8;;
			break;
			
		case VSCP_ADC4_LSB:
			rv = AdcValues[ 4 ] & 0xff;
			break;	
			
		case VSCP_ADC5_MSB:
			rv = AdcValues[ 5 ] >> 8;;
			break;
			
		case VSCP_ADC5_LSB:
			rv = AdcValues[ 5 ] & 0xff;
			break;
				
		case VSCP_ADC6_MSB:
			rv = AdcValues[ 6 ] >> 8;;
			break;
			
		case VSCP_ADC6_LSB:
			rv = AdcValues[ 6 ] & 0xff;
			break;	
			
		case VSCP_ADC7_MSB:
			rv = AdcValues[ 7 ] >> 8;;
			break;
			
		case VSCP_ADC7_LSB:
			rv = AdcValues[ 7 ] & 0xff;
			break;	
			
		case VSCP_ADC8_MSB:
			rv = AdcValues[ 8 ] >> 8;;
			break;
			
		case VSCP_ADC8_LSB:
			rv = AdcValues[ 8 ] & 0xff;
			break;
				
		case VSCP_ADC9_MSB:
			rv = AdcValues[ 9 ] >> 8;;
			break;
			
		case VSCP_ADC9_LSB:
			rv = AdcValues[ 9 ] & 0xff;
			break;	
			
		case VSCP_ADC10_MSB:
			rv = AdcValues[ 10 ] >> 8;;
			break;
			
		case VSCP_ADC10_LSB:
			rv = AdcValues[ 10 ] & 0xff;
			break;
				
		case VSCP_ADC11_MSB:
			rv = AdcValues[ 11 ] >> 8;;
			break;	
			
		case VSCP_ADC11_LSB:
			rv = AdcValues[ 11 ] & 0xff;
			break;			
			
		case VSCP_PWM_MSB:
			wtmp = ( CCP1CON >> 4 );
			wtmp |= ( (WORD)CCPR1L << 2 );
			rv = wtmp >> 8;
			break;
			
		case VSCP_PWM_LSB:
			wtmp = ( CCP1CON >> 4 );
			wtmp |= ( (WORD)CCPR1L << 2 );
			rv = wtmp & 0xff;
			break;
			
		case VSCP_ADC_CONTROL:
			break;		
						
		default:
			rv = 0;
	}
	
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
// writeVSCP_UserRegister_Level1
//

_u8 writeVSCP_UserRegister_Level1( _u8 idx, _u8 data )
{
	_u8 rv;
	
	switch( idx ) {
	
		default:
			rv = ~data;
	}
	
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp2_DM_Handler
//

void vscp2_DM_Handler( PVSCPMSG pmsg )
{
		
}





