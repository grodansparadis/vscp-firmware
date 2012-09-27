 /**
 * @brief           VSCP task for the rawEthernet reference project
 * @file            vscp_task.c
 * @author          Ake Hedman, eurosource, <a href="www.vscp.org">VSCP Project</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_vscp
 *
 *
 * @section description Description
 **********************************
 * This module contains the code that implements that 
 *
 *********************************************************************/


/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Copyright (C) 2011-2012 Ake Hedman, eurosource
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

#include "string.h"
#include "inttypes.h"
#include "projdefs.h"
#include "appcfg.h"
#include "debug.h"

#include "stacktsk.h"
#include "tick.h"
#include "delay.h"
#include "xeeprom.h"

#include "vscp_firmware_level2.h"
#include "vscp_class.h"      
#include "vscp_type.h"

#include "crc.h"
#include "eeprom.h"

#include "version.h"

#include "vscpmain.h"


extern APP_CONFIG AppConfig;  // from appcfg.c

#if defined( APP_USE_ADC8 )
extern uint8_t AdcValues[ ADC_CHANNELS ];
#elif defined( APP_USE_ADC10 )
extern uint16_t AdcValues[ ADC_CHANNELS ];
#endif

// Prototypes

#if !defined(VSCP_USE_EEPROM_FOR_MANUFACTURERE_ID)

// Manufacturer id's
// offset 0 - Manufacturer device id 0
// offset 1 - Manufacturer device id 1
// offset 2 - Manufacturer device id 2
// offset 3 - Manufacturer device id 3
// offset 4 - Manufacturer subdevice id 0
// offset 5 - Manufacturer subdevice id 1
// offset 6 - Manufacturer subdevice id 2
// offset 7 - Manufacturer subdevice id 3
const ROM unsigned char vscp_manufacturer_id[] = { 
	0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
};

#endif

// The device URL (max 32 characters including null termination)
const ROM char vscp_deviceURL[] = "www.eurosource.se/raweth_a.xml";

// From vscp_2.c
extern uint8_t vscp_function_flags;
extern uint16_t vscp_page_select;		// Current register page
extern vscpEvent wrkEvent;

#if defined(VSCP_ENABLE_BOOTLOADER)
BOOL bBootLoadMode;				// TRUE if in bootloader mode
uint8_t vscp_current_bootblock;			// Block that is loading
#endif

uint8_t pwmtemp[2];

static uint8_t lastHeartBeat = 120;		// Set 120 as value to send heatbeat directly
static uint8_t lastPeriodicOutputEvent = 0;
static uint8_t lastPeriodicInputEvent = 0;
static uint8_t lastPeriodicADEvent0 = 0;
static uint8_t lastPeriodicADEvent1 = 0;
static uint8_t lastPeriodicADEvent2 = 0;
static uint8_t lastPeriodicADEvent3 = 0;

// The outevent is so large that we need to put it in it's own memory bank.
#pragma udata outeventSection

///////////////////////////////////////////////////////////////////////////////
// feedVSCP
//
// Incoming VSCP events are feed to this routine
//

void feedVSCP()
{
    int i;
    uint32_t reg;
    uint32_t val;
	
    if ( ( VSCP_CLASS1_PROTOCOL == wrkEvent.vscp_class ) ||
        ( VSCP_CLASS2_LEVEL1_PROTOCOL == wrkEvent.vscp_class )  ) {

        // Must be addressed to us
        for ( i=0; i<16; i++ ) {
            if ( vscp_getGUID( i ) != wrkEvent.data[ i ] ) return;
        }
			
        // Handle Read register
	if ( VSCP_TYPE_PROTOCOL_READ_REGISTER == wrkEvent.vscp_type ) {

            // Read register
            reg = wrkEvent.data[ 17 ];

            if ( reg < 0x80 ) {
                val = vscp_readStdReg( reg );
            }
            else {
                // If the standard register space should be read
                // add offset to Level II standard register space
                reg += 0xFFFFFF00;
                val = vscp_readAppReg( reg );
            }

            wrkEvent.head = VSCP_PRIORITY_NORMAL;
            wrkEvent.vscp_class = VSCP_CLASS2_LEVEL1_PROTOCOL;
            wrkEvent.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
            wrkEvent.sizeData = 2;
            
            wrkEvent.data[ 0 ] = reg;
            wrkEvent.data[ 1 ] = val; // Zone
            
            vscp_sendEvent();
	}
			
	// Handle write register
	else if ( VSCP_TYPE_PROTOCOL_WRITE_REGISTER == wrkEvent.vscp_type ) {
								
            
					
	}		
			
	// Get decision matrix info
	else if ( VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO == wrkEvent.vscp_type ) {
				
            wrkEvent.head = VSCP_PRIORITY_NORMAL;
            wrkEvent.sizeData = 23;
            wrkEvent.vscp_class = VSCP_CLASS1_PROTOCOL;
            wrkEvent.vscp_type = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;
            // GUID is filled in by the send routine automatically
				
            wrkEvent.data[ 16 ] = 16;	// 16 rows in matrix
            wrkEvent.data[ 17 ] = 16;	// Matrix offset
            wrkEvent.data[ 18 ] = 0; 	// Page stat
            wrkEvent.data[ 19 ] = 0;
            wrkEvent.data[ 20 ] = 0;	// Page end
            wrkEvent.data[ 21 ] = 0;
            wrkEvent.data[ 22 ] = 0;	// Size of row for Level II
				
            vscp_sendEvent();

				
	}
			
	// Handle Get Decision Matrix Info
	else if ( VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO == wrkEvent.vscp_type ) {
				
           
	}

    } // CLASS1.PROTOCOL
			
    else if ( ( VSCP_CLASS1_ALARM == wrkEvent.vscp_class ) ||
                ( VSCP_CLASS2_LEVEL1_ALARM == wrkEvent.vscp_class )  ) {
			
    }
    else if ( ( VSCP_CLASS1_MEASUREMENT == wrkEvent.vscp_class ) ||
		( VSCP_CLASS2_LEVEL1_MEASUREMENT == wrkEvent.vscp_class )  ) {
		
    }
    else if ( ( VSCP_CLASS1_INFORMATION == wrkEvent.vscp_class ) ||
		( VSCP_CLASS2_LEVEL1_INFORMATION == wrkEvent.vscp_class )  ) {
			
    }
    else if ( ( VSCP_CLASS1_CONTROL == wrkEvent.vscp_class ) ||
                ( VSCP_CLASS2_LEVEL1_CONTROL == wrkEvent.vscp_class )  ) {
					
    }
    else if ( VSCP_CLASS2_PROTOCOL == wrkEvent.vscp_class )  {

        // Must be addressed to us
        if ( !memcmp( (void *)vscp_getGUID, (void *)wrkEvent.data, 16 ) ) return;
        //for ( i=0; i<16; i++ ) {
        //    if ( vscp_getGUID( i ) != wrkEvent.data[ i ] ) return;
        //}
			
        if ( VSCP2_TYPE_PROTOCOL_READ_REGISTER == wrkEvent.vscp_type ) {
				
            vscp_readRegister();
				
        }
        else if ( VSCP2_TYPE_PROTOCOL_WRITE_REGISTER == wrkEvent.vscp_type ) {
								
            vscp_writeRegister();
        }

    } // CLASS2.PROTOCOL
		
		
    // Feed Event into decision matrix
    vscp_feedDM();
		
}

///////////////////////////////////////////////////////////////////////////////
// periodicVSCPWork
//
// Periodic VSCP work is done here
// Routine must be called from time to time.
//
 
void periodicVSCPWork( void )
{
    // Check if we should send periodic output events
    if ( TickGetDiff8bitSec( lastPeriodicOutputEvent ) >
                appcfgGetc( APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL ) ) {
        lastPeriodicOutputEvent = TickGet8bitSec();
        if ( appcfgGetc(APPCFG_VSCP_EEPROM_REG_MODULE_CONTROL) & 0x40 ) {
       		sendPeriodicOutputEvents();	
       	}
    }
    // Check if we should send periodic input events
    else if ( TickGetDiff8bitSec( lastPeriodicInputEvent ) >
                appcfgGetc( APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL ) ) {
    	lastPeriodicInputEvent = TickGet8bitSec();
       	if ( appcfgGetc(APPCFG_VSCP_EEPROM_REG_MODULE_CONTROL) & 0x80 ) {
       		sendPeriodicInputEvents();	
       	}
    }
    // A/D 0 Periodic events
    else if ( TickGetDiff8bitSec( lastPeriodicADEvent0 ) > appcfgGetc(APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL) ) {
        lastPeriodicADEvent0 = TickGet8bitSec();
	if ( appcfgGetc(APPCFG_VSCP_EEPROM_REG_ADCTRL_EVENT_PERIODCTRL) & 0x01 ) {
            sendPeriodicADEvents( 0 );
	}
    }
    // A/D 1 Periodic events
    else if ( TickGetDiff8bitSec( lastPeriodicADEvent1 ) >
                appcfgGetc(APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL) ) {
        lastPeriodicADEvent1 = TickGet8bitSec();
        if ( appcfgGetc(APPCFG_VSCP_EEPROM_REG_ADCTRL_EVENT_PERIODCTRL) & 0x02 ) {
            sendPeriodicADEvents( 1 );
	}
    }
    // A/D 2 Periodic events
    else if ( TickGetDiff8bitSec( lastPeriodicADEvent2 ) >
                appcfgGetc(APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL) ) {
	lastPeriodicADEvent2 = TickGet8bitSec();
	if ( appcfgGetc(APPCFG_VSCP_EEPROM_REG_ADCTRL_EVENT_PERIODCTRL) & 0x04 ) {
            sendPeriodicADEvents( 2 );
	}
    }
    // A/D 3 Periodic events
    else if ( TickGetDiff8bitSec( lastPeriodicADEvent3 ) >
                appcfgGetc(APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL) ) {
        lastPeriodicADEvent3 = TickGet8bitSec();
	if ( appcfgGetc(APPCFG_VSCP_EEPROM_REG_ADCTRL_EVENT_PERIODCTRL) & 0x08 ) {
            sendPeriodicADEvents( 3 );
	}
    }
    
#if defined(VSCP_SEND_MINUTE_HEARTBEAT)    
    // Check if we should send a heartbeat (every minute)
    else if ( TickGetDiff8bitSec( lastHeartBeat ) >= 60 ) {
        lastHeartBeat = TickGet8bitSec();
        sendHeartBeat();
    }
#endif	
		
}

///////////////////////////////////////////////////////////////////////////////
// sendHeartBeat
//

void sendHeartBeat(void )
{	
    uint8_t i;
	
    // Send heartbeat
    wrkEvent.head = VSCP_PRIORITY_NORMAL;
    wrkEvent.sizeData = 3;
    wrkEvent.vscp_class = VSCP_CLASS2_LEVEL1_INFORMATION;
    wrkEvent.vscp_type = VSCP_TYPE_INFORMATION_NODE_HEARTBEAT;
    // GUID is filled in by send routine
    wrkEvent.data[ 0 ] = 0; // No meaning
    // Zone
    wrkEvent.data[ 1 ] =
        appcfgGetc( APPCFG_VSCP_EEPROM_REG_MODULE_ZONE );
    // SubZone
    wrkEvent.data[ 2 ] =
        ( appcfgGetc( APPCFG_VSCP_EEPROM_REG_MODULE_SUBZONE ) & 0xe0 );
	
    vscp_sendEvent();
}

///////////////////////////////////////////////////////////////////////////////
// sendPeriodicOutputEvents
//

void sendPeriodicOutputEvents( void )
{	
    wrkEvent.head = VSCP_PRIORITY_NORMAL;
    wrkEvent.sizeData = 2;
    wrkEvent.vscp_class = VSCP_CLASS2_LEVEL1_DATA;
    wrkEvent.vscp_type = VSCP_TYPE_DATA_IO;
	
    // GUID is filled in by send routine
    wrkEvent.data[0] = 0x02; // Byte format
    wrkEvent.data[1] = PORTB;
	
    vscp_sendEvent();
}


///////////////////////////////////////////////////////////////////////////////
// sendPeriodicInputEvents
//

void sendPeriodicInputEvents( void )
{
    wrkEvent.head = VSCP_PRIORITY_NORMAL;
    wrkEvent.sizeData = 2;
    wrkEvent.vscp_class = VSCP_CLASS2_LEVEL1_DATA;
    wrkEvent.vscp_type = VSCP_TYPE_DATA_IO;

    // GUID is filled in by send routine
    wrkEvent.data[0] = 0x02; // Byte format
    wrkEvent.data[1] = getInputState();
	
    vscp_sendEvent();
}


///////////////////////////////////////////////////////////////////////////////
// sendPeriodicADEvents
//

void sendPeriodicADEvents( uint8_t ch )
{
    switch ( ch ) {
	
    case 0:
        ch =5;
        break;

    case 1:
        ch =6;
        break;

    case 2:
        ch =7;
        break;
			
    case 3:
        ch =8;
        break;
			
    default:
        ch = 5;
        break;
	
    }
		
    wrkEvent.head = VSCP_PRIORITY_NORMAL;
    wrkEvent.sizeData = 3;
    wrkEvent.vscp_class = VSCP_CLASS2_LEVEL1_DATA;
    wrkEvent.vscp_type = VSCP_TYPE_DATA_AD;
	
    // GUID is filled in by send routine
    wrkEvent.data[ 0 ] = 0x02; // Byte format
    wrkEvent.data[ 1 ] = ( AdcValues[ ch ] >> 8 ) & 0xff; 	// MSB
    wrkEvent.data[ 2 ] = AdcValues [ch ] & 0xff; 			// LSB
	
    vscp_sendEvent();
}

///////////////////////////////////////////////////////////////////////////////
//                          Register callbacks
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// getVSCPMajorVersion
//

uint8_t vscp_getFirmwareMajorVersion( void )
{
    return FIRMWARE_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPMinorVersion
//

uint8_t vscp_getFirmwareMinorVersion( void )
{
    return FIRMWARE_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPSubMinorVersion
//

uint8_t vscp_getFirmwareSubMinorVersion( void )
{
    return FIRMWARE_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPBootloaderCode
//

uint8_t getVSCPBootloaderCode( void )
{
#if defined(VSCP_ENABLE_BOOTLOADER)
    return 0;
#else	
    return 0xff;	// No bootloader support
#endif	
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP_DeviceURL
//

uint8_t getVSCP_DeviceURL( uint8_t idx )
{
    if ( idx < 16 ) {
        return vscp_deviceURL[ idx ];
    }
	
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//  getVSCPControlByte
//

uint8_t vscp_getControlByte( void )
{
    return appcfgGetc( APPCFG_VSCP_EEPROM_CONTROL );
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPControlByte
//

void vscp_setControlByte( uint8_t ctrl )
{
    appcfgPutc( APPCFG_VSCP_EEPROM_CONTROL, ctrl );
}

///////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getUserID( uint8_t idx )
{
    return appcfgGetc( APPCFG_VSCP_EEPROM_REG_USERID + idx );
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPUserID
//

void vscp_setUserID( uint8_t idx, uint8_t data )
{
    appcfgPutc( APPCFG_VSCP_EEPROM_REG_USERID + idx, data );
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getManufacturerId( uint8_t idx )
{
#if defined(VSCP_USE_EEPROM_FOR_MANUFACTURERE_ID)
    return appcfgGetc( APPCFG_VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx );
#else
    return vscp_manufacturer_id[ idx ];
#endif	
}

///////////////////////////////////////////////////////////////////////////////
// vscp_setManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

void vscp_setManufacturerId( uint8_t idx, uint8_t data )
{
    appcfgPutc( APPCFG_VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx, data );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getGUID
//

uint8_t vscp_getGUID( uint8_t idx )
{
    if ( idx < 7 ) {
	return 0xff;
    }
    else if ( 7 == idx ) {
	return 0xfe;
    }
    else if ( 8 == idx ) {
        return MY_MAC_BYTE1;
    }
    else if ( 9 == idx ) {
        return MY_MAC_BYTE2;
    }
    else if ( 10 == idx ) {
        return MY_MAC_BYTE3;
    }
    else if ( 11 == idx ) {
        return MY_MAC_BYTE4;
    }
    else if ( 12 == idx ) {
        return MY_MAC_BYTE5;
    }
    else if ( 13 == idx ) {
        return MY_MAC_BYTE6;
    }
    else if ( 14 == idx  ) {
        return appcfgGetc( APPCFG_SERIAL_NUMBER0 );
    }
    else if ( 15 == idx  ) {
        return appcfgGetc( APPCFG_SERIAL_NUMBER1 );
    }
	
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm( void ) 
{
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//

uint8_t vscp_getBufferSize( void ) 
{
    return LIMITED_DEVICE_DATASIZE;
}

///////////////////////////////////////////////////////////////////////////////
// getDeviceURL
//
// Get device URL from EEPROM
//

uint8_t vscp_getMDF_URL( uint8_t idx )
{
    return vscp_deviceURL[ idx ];
}

///////////////////////////////////////////////////////////////////////////////
// 				     User Register
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// vscp_readAppReg  
//
//

uint8_t vscp_readAppReg( uint32_t reg )
{
    uint8_t rv;
    uint8_t wrk;
    uint8_t addr;
	
    if (  reg < 128 ) {
        // Standard register space
	
        if ( 0 == vscp_page_select ) {
		
            // * * * Standard register space * * *
            switch( reg ) {
		
            case VSCP_REG_MODULE_ZONE:
                rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_MODULE_ZONE );
		break;
				
            case VSCP_REG_MODULE_SUBZONE:
                rv = ( appcfgGetc( APPCFG_VSCP_EEPROM_REG_MODULE_SUBZONE ) & 0xe0 );
		break;	
				
            case VSCP_REG_MODULE_CONTROL:
                rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_MODULE_CONTROL );
		break;		
				
            case VSCP_REG_EVENT_INTERVAL:
		rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL );
		break;
				
            case VSCP_REG_INPUT_STATUS:
		rv = getInputState();
		break;	
				
            case VSCP_REG_OUTPUT_STATUS:
		rv = PORTB;
		break;	
				
            case VSCP_REG_ADCTRL_EVENT_PERIODCTRL:
		rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_ADCTRL_EVENT_PERIODCTRL );
		break;	
				
            case VSCP_REG_ADCTRL_ALARM_LOW:
		rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_ADCTRL_ALARM_LOW );
		break;	
				
            case VSCP_REG_ADCTRL_ALARM_HIGH:
		rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_ADCTRL_ALARM_HIGH );
		break;			
				
            case VSCP_REG_OUTPUT_CONTROL0:
            case VSCP_REG_OUTPUT_CONTROL1:
            case VSCP_REG_OUTPUT_CONTROL2:
            case VSCP_REG_OUTPUT_CONTROL3:
            case VSCP_REG_OUTPUT_CONTROL4:
            case VSCP_REG_OUTPUT_CONTROL5:
            case VSCP_REG_OUTPUT_CONTROL6:
            case VSCP_REG_OUTPUT_CONTROL7:
                wrk = appcfgGetc( APPCFG_PORTB );
		rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL0 + ( reg - VSCP_REG_OUTPUT_CONTROL0 ) );
		// Fix startup bit state
		rv &= ~( 1 << ( reg - VSCP_REG_OUTPUT_CONTROL0  ) );
		rv |= ( wrk & ( 1 << ( reg - VSCP_REG_OUTPUT_CONTROL0  ) ) );
		break;	
				
            case VSCP_REG_INPUT_CONTROL0:
            case VSCP_REG_INPUT_CONTROL1:
            case VSCP_REG_INPUT_CONTROL2:
            case VSCP_REG_INPUT_CONTROL3:
            case VSCP_REG_INPUT_CONTROL4:
            case VSCP_REG_INPUT_CONTROL5:
            case VSCP_REG_INPUT_CONTROL6:
            case VSCP_REG_INPUT_CONTROL7:
                rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL0 + ( reg - VSCP_REG_INPUT_CONTROL0 ) );
                break;
				
            case VSCP_REG_OUTPUT_PROTECTION_TIME0_MSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME0_LSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME1_MSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME1_LSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME2_MSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME2_LSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME3_MSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME3_LSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME4_MSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME4_LSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME5_MSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME5_LSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME6_MSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME6_LSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME7_MSB:
            case VSCP_REG_OUTPUT_PROTECTION_TIME7_LSB:
                rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME0_MSB + ( reg - VSCP_REG_OUTPUT_PROTECTION_TIME0_MSB ) );
                break;
				
            case VSCP_REG_ANALOG0_MSB:
                rv = ( AdcValues[5] >> 8 ) & 0xff;
		break;
				
            case VSCP_REG_ANALOG1_MSB:
		rv = ( AdcValues[6] >> 8 ) & 0xff;
		break;
				
            case VSCP_REG_ANALOG2_MSB:
		rv = ( AdcValues[7] >> 8 ) & 0xff;
		break;
				
            case VSCP_REG_ANALOG3_MSB:
		rv = ( AdcValues[8] >> 8 ) & 0xff;
		break;
				
            case VSCP_REG_ANALOG4_MSB:
		rv = ( AdcValues[2] >> 8 ) & 0xff;
		break;
				
            case VSCP_REG_ANALOG5_MSB:
		rv = ( AdcValues[3] >> 8 ) & 0xff;
		break;
				
            case VSCP_REG_ANALOG6_MSB:
		rv = ( AdcValues[0] >> 8 ) & 0xff;
		break;
				
            case VSCP_REG_ANALOG7_MSB:
		rv = ( AdcValues[1] >> 8 ) & 0xff;
		break;	
				
            case VSCP_REG_ANALOG0_LSB:
		rv = AdcValues[5] & 0xff;
		break;
				
            case VSCP_REG_ANALOG1_LSB:
		rv = AdcValues[6] & 0xff;
		break;
				
            case VSCP_REG_ANALOG2_LSB:
                rv = AdcValues[7] & 0xff;
		break;
				
            case VSCP_REG_ANALOG3_LSB:
		rv = AdcValues[8] & 0xff;
		break;
				
            case VSCP_REG_ANALOG4_LSB:
		rv = AdcValues[2] & 0xff;
		break;
				
            case VSCP_REG_ANALOG5_LSB:
		rv = AdcValues[3] & 0xff;
		break;
				
            case VSCP_REG_ANALOG6_LSB:
		rv = AdcValues[0] & 0xff;
		break;
				
            case VSCP_REG_ANALOG7_LSB:
		rv = AdcValues[1] & 0xff;
		break;		
						
            case VSCP_REG_PWM_MSB:
		rv = ( getPWMValue() >> 8 ) & 0xff;
		break;
				
            case VSCP_REG_PWM_LSB:
		rv = getPWMValue() & 0xff;
		break;	
				
            case VSCP_REG_SERIAL_BAUDRATE:
		rv = appcfgGetc( APPCFG_USART1_BAUD );	
		break;
				
            case VSCP_REG_SERIAL_CONTROL:
		rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_SERIAL_CONTROL );
		break;	
				
            case VSCP_REG_ANALOG_MIN0_MSB:
            case VSCP_REG_ANALOG_MIN0_LSB:
            case VSCP_REG_ANALOG_MIN1_MSB:
            case VSCP_REG_ANALOG_MIN1_LSB:
            case VSCP_REG_ANALOG_MIN2_MSB:
            case VSCP_REG_ANALOG_MIN2_LSB:
            case VSCP_REG_ANALOG_MIN3_MSB:
            case VSCP_REG_ANALOG_MIN3_LSB:
            case VSCP_REG_ANALOG_MIN4_MSB:
            case VSCP_REG_ANALOG_MIN4_LSB:
            case VSCP_REG_ANALOG_MIN5_MSB:
            case VSCP_REG_ANALOG_MIN5_LSB:
            case VSCP_REG_ANALOG_MIN6_MSB:
            case VSCP_REG_ANALOG_MIN6_LSB:
            case VSCP_REG_ANALOG_MIN7_MSB:
            case VSCP_REG_ANALOG_MIN7_LSB:
			
            case VSCP_REG_ANALOG_MAX0_MSB:
            case VSCP_REG_ANALOG_MAX0_LSB:
            case VSCP_REG_ANALOG_MAX1_MSB:
            case VSCP_REG_ANALOG_MAX1_LSB:
            case VSCP_REG_ANALOG_MAX2_MSB:
            case VSCP_REG_ANALOG_MAX2_LSB:
            case VSCP_REG_ANALOG_MAX3_MSB:
            case VSCP_REG_ANALOG_MAX3_LSB:
            case VSCP_REG_ANALOG_MAX4_MSB:
            case VSCP_REG_ANALOG_MAX4_LSB:
            case VSCP_REG_ANALOG_MAX5_MSB:
            case VSCP_REG_ANALOG_MAX5_LSB:
            case VSCP_REG_ANALOG_MAX6_MSB:
            case VSCP_REG_ANALOG_MAX6_LSB:
            case VSCP_REG_ANALOG_MAX7_MSB:
            case VSCP_REG_ANALOG_MAX7_LSB:
                rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_ANALOG_MIN0_MSB + ( reg - VSCP_REG_ANALOG_MIN0_MSB ));
		break;
				
            case VSCP_REG_ANALOG_HYSTERESIS:
                rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_ANALOG_HYSTERESIS );
		break;	
				
            case VSCP_REG_CAN_CONTROL:
		rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_CAN_CONTROL );
		break;	
				
            case VSCP_REG_CAN_CLASS_MASK_MSB:
            case VSCP_REG_CAN_CLASS_MASK_LSB:
            case VSCP_REG_CAN_TYPE_MASK_MSB:
            case VSCP_REG_CAN_TYPE_MASK_LSB:
            case VSCP_REG_CAN_CLASS_FILTER_MSB:
            case VSCP_REG_CAN_CLASS_FILTER_LSB:
            case VSCP_REG_CAN_TYPE_FILTER_MSB:
            case VSCP_REG_CAN_TYPE_FILTER_LSB:
                rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_CAN_CLASS_MASK_MSB + ( reg - VSCP_REG_CAN_CLASS_MASK_MSB ));
		break;
				
            case VSCP_REG_CAN_GUID:
            case VSCP_REG_CAN_GUID+1:
            case VSCP_REG_CAN_GUID+2:
            case VSCP_REG_CAN_GUID+3:
            case VSCP_REG_CAN_GUID+4:
            case VSCP_REG_CAN_GUID+5:
            case VSCP_REG_CAN_GUID+6:
            case VSCP_REG_CAN_GUID+7:
            case VSCP_REG_CAN_GUID+8:
            case VSCP_REG_CAN_GUID+9:
            case VSCP_REG_CAN_GUID+10:
            case VSCP_REG_CAN_GUID+11:
            case VSCP_REG_CAN_GUID+12:
            case VSCP_REG_CAN_GUID+13:
            case VSCP_REG_CAN_GUID+14:
                rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID0 + ( reg - VSCP_REG_CAN_GUID ));
		break;	
	 					
            default:
		rv = 0;
            }
	}
	else if ( 1 == vscp_page_select ) {
            // * * * DM Rows 0-3 * * *
            rv = appcfgGetc( VSCP_DM_MATRIX_BASE + reg );
	}
	else if ( 2 == vscp_page_select ) {
            // * * * DM Rows 5-7 * * *
            rv = appcfgGetc( VSCP_DM_MATRIX_BASE + 0x100 + reg );
	}
	else if ( 3 == vscp_page_select ) {
            // * * * DM Rows 8-11 * * *
            rv = appcfgGetc( VSCP_DM_MATRIX_BASE + 0x200 + reg );
	}
	else if ( 4 == vscp_page_select ) {	
            // * * * DM Rows 12-15 * * *
            rv = appcfgGetc( VSCP_DM_MATRIX_BASE + 0x300 + reg );
	}
	else if ( reg < 1024 ) {
            // Read internal EEPROM
            rv = appcfgGetc( VSCP_DM_MATRIX_BASE + reg-0x100 );
	}
	else if ( reg >= 0x10000) {
            // Read external EEPROM
            addr = ( reg & 0xffff );
                XEEBeginRead( EEPROM_CONTROL, addr );
		rv = XEERead();
		XEEEndRead();
            }
            else if ( reg >= 0x20000) {
                // Read internal RAM
		rv = *((unsigned char *)(reg & 0x0fff));
            }
            else {
                // Return zero for non mapped area read
                //addr = ( reg & 0xffff );
                rv = 0;
            }
		
	}
	
	return rv;
}



///////////////////////////////////////////////////////////////////////////////
// vscp_readAppReg  
//
//

uint8_t vscp_writeAppReg(uint32_t reg, uint8_t data) {

    uint8_t rv;
    uint8_t wrk;
    uint8_t addr;

    if ( reg < 128 ) {
        // Standard register space
        if ( 0 == vscp_page_select ) {

            // * * * Standard register space * * *
            switch (reg) {

                case VSCP_REG_MODULE_ZONE:
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_MODULE_ZONE, data );
                    rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_MODULE_ZONE );
                    break;

                case VSCP_REG_MODULE_SUBZONE:
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_MODULE_SUBZONE, ( data & 0xe0 ) );
                    rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_MODULE_SUBZONE );
                    break;

                case VSCP_REG_MODULE_CONTROL:
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_MODULE_CONTROL, data );
                    rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_MODULE_CONTROL );
                    break;

                case VSCP_REG_EVENT_INTERVAL:
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL, data );
                    rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL );
                    break;

                case VSCP_REG_OUTPUT_STATUS:
                    LATB = data;
                    rv = PORTB;
                    break;

                case VSCP_REG_ADCTRL_EVENT_PERIODCTRL:
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_ADCTRL_EVENT_PERIODCTRL, data );
                    rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_ADCTRL_EVENT_PERIODCTRL );
                    break;

                case VSCP_REG_ADCTRL_ALARM_LOW:
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_ADCTRL_ALARM_LOW, data );
                    rv = appcfgGetc( APPCFG_VSCP_EEPROM_REG_ADCTRL_ALARM_LOW );
                    break;

                case VSCP_REG_ADCTRL_ALARM_HIGH:
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_ADCTRL_ALARM_HIGH, data );
                    rv = appcfgGetc (APPCFG_VSCP_EEPROM_REG_ADCTRL_ALARM_HIGH );
                    break;

                case VSCP_REG_OUTPUT_CONTROL0:
                case VSCP_REG_OUTPUT_CONTROL1:
                case VSCP_REG_OUTPUT_CONTROL2:
                case VSCP_REG_OUTPUT_CONTROL3:
                case VSCP_REG_OUTPUT_CONTROL4:
                case VSCP_REG_OUTPUT_CONTROL5:
                case VSCP_REG_OUTPUT_CONTROL6:
                case VSCP_REG_OUTPUT_CONTROL7:

                    // First set startup bit state
                    wrk = appcfgGetc(APPCFG_PORTB);
                    if (wrk & (1 << (reg - VSCP_REG_OUTPUT_CONTROL0))) {
                        wrk |= (1 << (reg - VSCP_REG_OUTPUT_CONTROL0)); // Initially on
                    } else {
                        wrk &= ~(1 << (reg - VSCP_REG_OUTPUT_CONTROL0)); // initially off
                    }

                    // Save the rest of the configuration
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL0 + (reg -
                                VSCP_REG_OUTPUT_CONTROL0 ), data );
                    rv = appcfgGetc(APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL0 + 
                                ( reg - VSCP_REG_OUTPUT_CONTROL0 ) );

                    // Fix startup bit state
                    rv &= ~( 1 << ( reg - VSCP_REG_OUTPUT_CONTROL0 ) );
                    rv |= ( wrk & ( 1 << ( reg - VSCP_REG_OUTPUT_CONTROL0 ) ) );
                    break;

                case VSCP_REG_INPUT_CONTROL0:
                case VSCP_REG_INPUT_CONTROL1:
                case VSCP_REG_INPUT_CONTROL2:
                case VSCP_REG_INPUT_CONTROL3:
                case VSCP_REG_INPUT_CONTROL4:
                case VSCP_REG_INPUT_CONTROL5:
                case VSCP_REG_INPUT_CONTROL6:
                case VSCP_REG_INPUT_CONTROL7:
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL0 +
                                ( reg - VSCP_REG_INPUT_CONTROL0), data );
                    rv = appcfgGetc(APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL0 + 
                                ( reg - VSCP_REG_INPUT_CONTROL0 ) );
                    break;

                case VSCP_REG_OUTPUT_PROTECTION_TIME0_MSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME0_LSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME1_MSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME1_LSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME2_MSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME2_LSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME3_MSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME3_LSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME4_MSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME4_LSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME5_MSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME5_LSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME6_MSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME6_LSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME7_MSB:
                case VSCP_REG_OUTPUT_PROTECTION_TIME7_LSB:
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME0_MSB +
                                ( reg - VSCP_REG_OUTPUT_PROTECTION_TIME0_MSB ), data );
                    rv = appcfgGetc(APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME0_MSB + 
                                ( reg - VSCP_REG_OUTPUT_PROTECTION_TIME0_MSB ) );
                    break;

                case VSCP_REG_PWM_MSB:
                    rv = data;
                    pwmtemp[0] = data;
                    if (appcfgGetc(APPCFG_VSCP_EEPROM_REG_MODULE_CONTROL) & PWM_PERSISTENT_BIT) {
                        appcfgPutc(APPCFG_VSCP_EEPROM_REG_PWM_MSB, data);
                    }
                    break;

                case VSCP_REG_PWM_LSB:
                    rv = data;
                    pwmtemp[1] = data;
                    if (appcfgGetc(APPCFG_VSCP_EEPROM_REG_MODULE_CONTROL) & PWM_PERSISTENT_BIT) {
                        appcfgPutc(APPCFG_VSCP_EEPROM_REG_PWM_LSB, data);
                    }
                    setPWMValue((uint16_t) pwmtemp[0] * 256 + pwmtemp[1]);
                    break;

                case VSCP_REG_SERIAL_BAUDRATE:
                    if (data > BAUD_921600) data = SERIAL_SPEED_DEFAULT; // Set default if out of range
                    appcfgPutc(APPCFG_USART1_BAUD, data);
                    appcfgUSART(); // Configure the USART
                    rv = appcfgGetc(APPCFG_USART1_BAUD);
                    break;

                case VSCP_REG_SERIAL_CONTROL:
                    appcfgPutc(APPCFG_VSCP_EEPROM_REG_SERIAL_CONTROL, data);
                    rv = appcfgGetc(APPCFG_VSCP_EEPROM_REG_SERIAL_CONTROL);
                    break;

                case VSCP_REG_ANALOG_MIN0_MSB:
                case VSCP_REG_ANALOG_MIN0_LSB:
                case VSCP_REG_ANALOG_MIN1_MSB:
                case VSCP_REG_ANALOG_MIN1_LSB:
                case VSCP_REG_ANALOG_MIN2_MSB:
                case VSCP_REG_ANALOG_MIN2_LSB:
                case VSCP_REG_ANALOG_MIN3_MSB:
                case VSCP_REG_ANALOG_MIN3_LSB:
                case VSCP_REG_ANALOG_MIN4_MSB:
                case VSCP_REG_ANALOG_MIN4_LSB:
                case VSCP_REG_ANALOG_MIN5_MSB:
                case VSCP_REG_ANALOG_MIN5_LSB:
                case VSCP_REG_ANALOG_MIN6_MSB:
                case VSCP_REG_ANALOG_MIN6_LSB:
                case VSCP_REG_ANALOG_MIN7_MSB:
                case VSCP_REG_ANALOG_MIN7_LSB:

                case VSCP_REG_ANALOG_MAX0_MSB:
                case VSCP_REG_ANALOG_MAX0_LSB:
                case VSCP_REG_ANALOG_MAX1_MSB:
                case VSCP_REG_ANALOG_MAX1_LSB:
                case VSCP_REG_ANALOG_MAX2_MSB:
                case VSCP_REG_ANALOG_MAX2_LSB:
                case VSCP_REG_ANALOG_MAX3_MSB:
                case VSCP_REG_ANALOG_MAX3_LSB:
                case VSCP_REG_ANALOG_MAX4_MSB:
                case VSCP_REG_ANALOG_MAX4_LSB:
                case VSCP_REG_ANALOG_MAX5_MSB:
                case VSCP_REG_ANALOG_MAX5_LSB:
                case VSCP_REG_ANALOG_MAX6_MSB:
                case VSCP_REG_ANALOG_MAX6_LSB:
                case VSCP_REG_ANALOG_MAX7_MSB:
                case VSCP_REG_ANALOG_MAX7_LSB:
                    appcfgPutc(APPCFG_VSCP_EEPROM_REG_ANALOG_MIN0_MSB + (reg - VSCP_REG_ANALOG_MIN0_MSB), data);
                    rv = appcfgGetc(APPCFG_VSCP_EEPROM_REG_ANALOG_MIN0_MSB + (reg - VSCP_REG_ANALOG_MIN0_MSB));
                    break;

                case VSCP_REG_ANALOG_HYSTERESIS:
                    appcfgPutc(APPCFG_VSCP_EEPROM_REG_ANALOG_HYSTERESIS, data);
                    rv = appcfgGetc(APPCFG_VSCP_EEPROM_REG_ANALOG_HYSTERESIS);
                    break;

                case VSCP_REG_CAN_CONTROL:
                    if ((data & 0x0f) > CAN_SPEED_1000K) {
                        data &= 0x0f;
                        data |= CAN_SPEED_DEFAULT; // Set default if out of range
                    }
                    appcfgPutc(APPCFG_VSCP_EEPROM_REG_CAN_CONTROL, data);
                    rv = appcfgGetc(APPCFG_VSCP_EEPROM_REG_CAN_CONTROL);
                    break;

                case VSCP_REG_CAN_CLASS_MASK_MSB:
                case VSCP_REG_CAN_CLASS_MASK_LSB:
                case VSCP_REG_CAN_TYPE_MASK_MSB:
                case VSCP_REG_CAN_TYPE_MASK_LSB:
                case VSCP_REG_CAN_CLASS_FILTER_MSB:
                case VSCP_REG_CAN_CLASS_FILTER_LSB:
                case VSCP_REG_CAN_TYPE_FILTER_MSB:
                case VSCP_REG_CAN_TYPE_FILTER_LSB:
                    appcfgPutc(APPCFG_VSCP_EEPROM_REG_CAN_CLASS_MASK_MSB + (reg - VSCP_REG_CAN_CLASS_MASK_MSB), data);
                    rv = appcfgGetc(APPCFG_VSCP_EEPROM_REG_CAN_CLASS_MASK_MSB + (reg - VSCP_REG_CAN_CLASS_MASK_MSB));
                    break;

                case VSCP_REG_CAN_GUID:
                case VSCP_REG_CAN_GUID + 1:
                case VSCP_REG_CAN_GUID + 2:
                case VSCP_REG_CAN_GUID + 3:
                case VSCP_REG_CAN_GUID + 4:
                case VSCP_REG_CAN_GUID + 5:
                case VSCP_REG_CAN_GUID + 6:
                case VSCP_REG_CAN_GUID + 7:
                case VSCP_REG_CAN_GUID + 8:
                case VSCP_REG_CAN_GUID + 9:
                case VSCP_REG_CAN_GUID + 10:
                case VSCP_REG_CAN_GUID + 11:
                case VSCP_REG_CAN_GUID + 12:
                case VSCP_REG_CAN_GUID + 13:
                case VSCP_REG_CAN_GUID + 14:
                    appcfgPutc( APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID0 +
                                ( reg - VSCP_REG_CAN_GUID ), data );
                    rv = appcfgGetc(APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID0 +
                            ( reg - VSCP_REG_CAN_GUID ) );
                    break;

                default:
                    rv = ~data;
            }
        }// Page select == 0
        else if (reg < 1024) {
            // Write internal EEPROM
            appcfgPutc( VSCP_DM_MATRIX_BASE + reg - 0x100, data );
            rv = appcfgGetc( VSCP_DM_MATRIX_BASE + reg - 0x100 );
        } else if ( reg >= 0x10000 ) {

            addr = ( reg & 0xffff );

            // Write data
            XEEBeginWrite( EEPROM_CONTROL, addr );
            XEEWrite( data );
            XEEEndWrite();

            // Read back
            XEEBeginRead( EEPROM_CONTROL, addr );
            rv = XEERead();
            XEEEndRead();
        } else {
            rv = ~data;
        }

    } else if ( 1 == vscp_page_select ) {

        // * * * DM Rows 0-3 * * *

        // Write data
        rv = appcfgPutc( VSCP_DM_MATRIX_BASE + reg, data );

        // Read back
        rv = appcfgGetc( VSCP_DM_MATRIX_BASE + reg );
        //XEEBeginRead( EEPROM_CONTROL, VSCP_EXTERNAL_DM_MATRIX_BASE + reg );
        //rv = XEERead();
        //XEEEndRead();

    } else if ( 2 == vscp_page_select ) {

        // * * * DM Rows 5-7 * * *

        // Write data
        rv = appcfgPutc( VSCP_DM_MATRIX_BASE + 0x100 + reg, data );

        // Read back
        rv = appcfgGetc( VSCP_DM_MATRIX_BASE + 0x100 + reg );

    } else if ( 3 == vscp_page_select ) {

        // * * * DM Rows 8-11 * * *

        // Write data
        rv = appcfgPutc( VSCP_DM_MATRIX_BASE + 0x200 + reg, data );

        // Read back
        rv = appcfgGetc( VSCP_DM_MATRIX_BASE + 0x200 + reg );

    } else if ( 4 == vscp_page_select ) {

        // * * * DM Rows 12-15 * * *

        // Write data
        rv = appcfgPutc( VSCP_DM_MATRIX_BASE + 0x300 + reg, data );


        // Read back
        rv = appcfgGetc( VSCP_DM_MATRIX_BASE + 0x300 + reg );

    }

    return rv;
}


///////////////////////////////////////////////////////////////////////////////
// vscp_writeUserRegister_Level2
//
// 	This is register writes to registers with address > 0x7f in the Level II
//	register map where the common registers are located at 0xffffff80 and upwards.
//
// In the Nova implemention we have DM enteries in this space.
//

uint8_t vscp_writeUserRegister_Level2(uint32_t reg, uint8_t data) {
    uint8_t rv;
    uint8_t addr;


    if (reg < 1024) {
        // Write internal EEPROM
        appcfgPutc(VSCP_DM_MATRIX_BASE + reg - 0x100, data);
        rv = appcfgGetc(VSCP_DM_MATRIX_BASE + reg - 0x100);
    } else if (reg >= 0x10000) {

        addr = (reg & 0xffff);

        // Write data
        XEEBeginWrite(EEPROM_CONTROL, addr);
        XEEWrite(data);
        XEEEndWrite();

        // Read back
        XEEBeginRead(EEPROM_CONTROL, addr);
        rv = XEERead();
        XEEEndRead();
    } else {
        rv = ~data;
    }

    return rv;
}


///////////////////////////////////////////////////////////////////////////////
// 								Decision Matrix
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// vscp_feedDM
//

void vscp_feedDM(void) {
    uint8_t i, j;
    uint8_t *p;

    // Not that we can't use the stucture as the endian may be different
    uint8_t dm[ 14 + VSCP_ACTION_PARAM_SIZE ]; // Full DM row
    uint32_t control, mask, filter;
    uint16_t action;

    // Search the DM enteries for a match
    for (i = 0; i < DM_ENTERIES; i++) {

        // Read in DM row
        for (j = 0; j < sizeof (dm); j++) {
            dm[j] = appcfgGetc(i * (14 + VSCP_ACTION_PARAM_SIZE) + j);
        }

        control = (uint32_t) dm[ VSCP_DM_POS_CONTROL ] << 24 +
                (uint32_t) dm[ VSCP_DM_POS_CONTROL + 1 ] << 16 +
                (uint32_t) dm[ VSCP_DM_POS_CONTROL + 2 ] << 8 +
                (uint32_t) dm[ VSCP_DM_POS_CONTROL + 3 ];

        // Check if enabled
        if (!(VSCP_DM_CONTROL_ENABLED & control)) continue;

        // Check if GUID should me matched
        if ((VSCP_DM_CONTROL_MATCH_GUID & control)) {
            if (memcmp((void *) wrkEvent.GUID, (void *) (dm + VSCP_DM_POS_PARAM), VSCP_SIZE_GUID)) {
                continue; // No match
            }
        }

        // Check mask/filter
        if (((filter ^ ((wrkEvent.vscp_class << 16) + wrkEvent.vscp_class)) & mask)) continue;

        // Should zone be checked
        if (VSCP_DM_CONTROL_MATCH_ZONE & control) {
            if (wrkEvent.vscp_class >= 1024) continue; // not valid for Level II events
            if (appcfgGetc(APPCFG_VSCP_EEPROM_REG_MODULE_ZONE) != wrkEvent.data[1]) continue;
        }

        // Should subzone be checked
        if (VSCP_DM_CONTROL_MATCH_SUBZONE & control) {
            if (wrkEvent.vscp_class >= 1024) continue; // not valid for Level II events
            if (appcfgGetc(APPCFG_VSCP_EEPROM_REG_MODULE_SUBZONE) != wrkEvent.data[2]) continue;
        }

        // We have an event that should trigger this row if no extra
        // conditions are required by the action itself.
        action = (uint16_t) dm[ VSCP_DM_POS_ACTION ] << 8 +
                (uint16_t) dm[ VSCP_DM_POS_ACTION ];

        doAction(action);
    }
}

///////////////////////////////////////////////////////////////////////////////
// doAction
//

void doAction(uint16_t action) {
    switch (action) {

        case VSCP_ACTION_NOOP: // Oh well we no nothing here jut use some code space... ;-)
            break;

        case VSCP_ACTION_OUTPUTS_ACTIVATE:
            if (!wrkEvent.data[0]) return; // nothing to do
            break;

        case VSCP_ACTION_OUTPUTS_DEACTIVATE:
            if (!wrkEvent.data[0]) return; // nothing to do
            break;

        case VSCP_ACTION_SERIAL_SEND_ARGUMENT:
            break;

        case VSCP_ACTION_SERIAL_SEND_ROM:
            break;

        case VSCP_ACTION_SERIAL_OPEN:
            break;

        case VSCP_ACTION_SERIAL_CLOSE:
            break;

        case VSCP_ACTION_CAN_OPEN:
            break;

        case VSCP_ACTION_CAN_CLOSE:
            break;

        case VSCP_ACTION_CAN_SEND:
            break;

        case VSCP_ACTION_OUTPUT_SEND_STATUS:
            break;

        case VSCP_ACTION_INPUT_SEND_STATUS:
            break;

        case VSCP_ACTION_ANALOG_SEND_STATUS:
            break;

        case VSCP_ACTION_INPUT_SEND_STATUS_IO:
            break;

        case VSCP_ACTION_ANALOG_SEND_STATUS_IO:
            break;

    }

}

///////////////////////////////////////////////////////////////////////////////
// 								Core Functionality
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// getInputState
//

uint8_t getInputState() {
    uint8_t rv = 0;

    // Bit 0
    if (PORTC & 1) {
        rv |= 0x01;
    }

    // Bit 1
    if (PORTC & (1 << 1)) {
        rv |= 0x02;
    }

    // Bit 2
    if (PORTC & (1 << 5)) {
        rv |= 0x04;
    }

    // Bit 3
    if (PORTG & (1 << 3)) {
        rv |= 0x08;
    }

    // Bit 4
    if (PORTA & (1 << 4)) {
        rv |= 0x10;
    }

    // Bit 5
    if (PORTA & (1 << 5)) {
        rv |= 0x20;
    }

    // Bit 6
    if (PORTF & (1 << 7)) {
        rv |= 0x40;
    }

    // Bit 7
    if (PORTA & (1 << 1)) {
        rv |= 0x80;
    }

    return rv;
}



///////////////////////////////////////////////////////////////////////////////
// getPWMValue
//

uint16_t getPWMValue(void) {
    uint16_t tmp;

    tmp = (CCP1CON >> 4);
    tmp |= ((uint16_t) CCPR1L << 2);

    return tmp;
}

///////////////////////////////////////////////////////////////////////////////
// setPWMValue
//

void setPWMValue(uint16_t value) {
    value &= 0x3ff; // max 1023
    CCP1CON &= 0x0f;
    CCP1CON |= (value & 3) << 4; // bits 0,1
    CCPR1L = (value >> 2); // rest of the bits
}

