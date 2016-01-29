/**
 * @brief           VSCP Level II common functionality
 * @file            vscp_firmware_level2.c
 * @author          Ake Hedman, eurosource,
 *                  www.vscp.org - VSCP Project
 * @dependencies    -
 * @ingroup         mod_vscp
 *
 *
 * @section description Description
 **********************************
 * This module contains the code that implements the common
 * VSCP level II functionality.
 *
 *********************************************************************/


/* *****************************************************************************
 * VSCP (Very Simple Control Protocol) 
 * http://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2015 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This module defined general functions for a Level II low resource module. Many 
 * methods are must be implemented by the user application to make a full implemention.
 *
 * ******************************************************************************
 */

#include <vscp_compiler.h>
#include <vscp_projdefs.h>
#include "string.h"
#include "inttypes.h"
#include "vscp_class.h"      
#include "vscp_type.h"
#include "vscp_firmware_level2.h"

// Constants

// Globals

// VSCP Data
//static vscpEvent inEvent;

// VSCP function flags deterine how the software behaves.
//  bit 0: 0 == UDP interfase. 1 == TCP interface.
//  bit 1: Set if Autodiscovery of server.
//  bit 2: Set if Fallback to hard ip address to UDP
//  bit 3: Set if Hard coded IP address.
//  bit 4: Set if Probe has been sent
//  bit 5: 
//  bit 6: Set if incoming message object is in use.
//  bit 7: Set if outgoing transmission is in progress.
//  bit 8: Reserved. 
//  bit 9: Reserved. 
//  bit 10: Reserved. 
//  bit 11: Reserved. 
//  bit 12: Reserved. 
//  bit 13: Reserved. 
//  bit 14: Reserved. . 
//  bit 15: Set if raw ethernet activated
uint16_t vscp_function_flags;

uint8_t vscp_alarmstatus; // VSCP Alarm Status

uint8_t vscp_node_state; // State machine state
uint8_t vscp_node_substate; // State machine substate

// page selector
uint16_t vscp_page_select;

// This is stored globally here to save memory
vscpEvent wrkEvent;

///////////////////////////////////////////////////////////////////////////////
// vscp_init
//

void vscp_init(void) {

    vscp_function_flags = 0;

    // If server discovery is enabled we should
    // always start out in UDP mode.
#if defined( VSCP_DISCOVER_SERVER ) && defined( VSCP_USE_UDP )
    vscp_function_flags |= VSCP_FUNCTION_AUTODISCOVER;
#else
#if defined( VSCP_USE_TCP )
    vscp_function_flags |= VSCP_FUNCTION_USE_TCP;
#endif
#endif

    // Fallback from TCP to UDP if failure to connect to TCP server
#if defined( VSCP_TCP_FALLBACK )
    vscp_function_flags |= VSCP_FUNCTION_FLAG_FALLBACK;
#endif

    // Server IP address is hardcoded. If fallback and discovery is enables
    // this address will be used after a failed TCP discovery. If it also fails
    // UDP will be used.
#if defined( VSCP_SERVERIP_HARDCODE_IPADDRD )
    vscp_function_flags |= VSCP_FUNCTION_HARDCODED_IPADDR;
#endif

#if defined( VSCP_USE_UDP )
    vscp_udpinit();
#endif	

#if defined( VSCP_ENABLE_BOOTLOADER )
    bBootLoadMode = FALSE; // Not in bootload mode
#endif

    vscp_page_select = 0; // Default page

}

///////////////////////////////////////////////////////////////////////////////
// vscp_sendEvent
//

int8_t vscp_sendEvent() {
    
    // Fill in GUID
    fillGUID();

#if defined(VSCP_USE_RAW_ETHERNET)
    vscp_sendRawPacket();
#endif

#if defined(VSCP_USE_UDP) && defined(VSCP_USE_TCP)	

    if (vscp_function_flags & VSCP_FUNCTION_USE_TCP) {
        return vscp_sendTCPEvent(pevent);
    } else {
        return vscp_sendUDPEvent(pevent);
    }
#else 

#if defined(VSCP_USE_TCP)
    return vscp_sendTCPEvent(pevent);
#endif	

#if defined(VSCP_USE_UDP)
    return vscp_sendUDPEvent(pevent);
#endif	

#endif



}


///////////////////////////////////////////////////////////////////////////////
// vscp_getEvent
//

int8_t vscp_getEvent(void) {
#if defined(VSCP_USE_RAW_ETHERNET)
    return vscp_getRawPacket();
#endif

#if defined(VSCP_USE_UDP) && defined(VSCP_USE_TCP)		
    if (vscp_function_flags & VSCP_FUNCTION_USE_TCP) {
        return vscp_getTCPEvent(pevent);
    } else {
        return vscp_getUDPEvent(pevent);
    }
#else
#if defined(VSCP_USE_TCP)
    return vscp_getTCPEvent(pevent);
#endif
#if defined(VSCP_USE_UDP)
    return vscp_getUDPEvent(pevent);
#endif
#endif	
}



///////////////////////////////////////////////////////////////////////////////
// vscp_readRegister
//

void vscp_readRegister( void )
{
    uint32_t i;
    uint32_t reg = ((uint32_t) wrkEvent.data[ 16 ] << 24) +
            ((uint32_t) wrkEvent.data[ 17 ] << 16) +
            ((uint32_t) wrkEvent.data[ 18 ] << 8) +
            (uint32_t) wrkEvent.data[ 19 ];

    uint32_t cnt = ((uint32_t) wrkEvent.data[ 20 ] << 8) +
            (uint32_t)wrkEvent.data[ 21 ];

    if (cnt > ( LIMITED_DEVICE_DATASIZE - 8 ) ) {
        cnt = ( LIMITED_DEVICE_DATASIZE - 8 );
    }

    for ( i = 0; i < cnt; i++ ) {

        if ( ( reg + i) > VSCP_LEVEL2_COMMON_REGISTER_START ) {
            // Common register
            wrkEvent.data[ 4 + i ] =
                    vscp_readStdReg( reg + i );
        } else {
            // User register
            wrkEvent.data[ 4 + i ] = vscp_readAppReg( reg + i );
        }

    }

    wrkEvent.sizeData = 4 + cnt;
    wrkEvent.data[ 0 ] = reg >> 24;
    wrkEvent.data[ 1 ] = reg >> 16;
    wrkEvent.data[ 2 ] = reg >> 8;
    wrkEvent.data[ 3 ] = reg & 0xff;
    wrkEvent.head = VSCP_PRIORITY_NORMAL;
    wrkEvent.vscp_class = VSCP_CLASS2_PROTOCOL;
    wrkEvent.vscp_type = VSCP2_TYPE_PROTOCOL_READ_WRITE_RESPONSE;

    vscp_sendEvent();

}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeRegister
//

void vscp_writeRegister( void )
{
    uint8_t saveData[ 4 ];
    uint32_t i;
    uint32_t idx = ((uint32_t) wrkEvent.data[ 0 ] << 24) +
            ((uint32_t) wrkEvent.data[ 1 ] << 16) +
            ((uint32_t) wrkEvent.data[ 2 ] << 8) +
            (uint32_t) wrkEvent.data[ 3 ];

    uint16_t cnt = wrkEvent.sizeData - 16 - 4 - 4; // Take away GUID + addr + reserved

    if (cnt > (LIMITED_DEVICE_DATASIZE - 24)) {
        cnt = (LIMITED_DEVICE_DATASIZE - 24);
    }

    for (i = 0; i < cnt; i++) {

        if ((idx + i) > VSCP_LEVEL2_COMMON_REGISTER_START) {
            // Common register
            wrkEvent.data[ 8 + i ] =
                    vscp_writeStdReg((idx & 0xff) + i,
                    wrkEvent.data[ 24 + i ]);
        } else {
            // User register
            wrkEvent.data[ 8 + i ] =
                    vscp_writeAppReg(idx + i,
                    wrkEvent.data[ 24 + i ]);
        }

    }

    // Save address
    memcpy((void *) saveData, (void *) wrkEvent.data, 4);

    wrkEvent.sizeData = 8 + cnt;
    wrkEvent.data[ 0 ] = saveData[ 0 ];
    wrkEvent.data[ 1 ] = saveData[ 1 ];
    wrkEvent.data[ 2 ] = saveData[ 2 ];
    wrkEvent.data[ 3 ] = saveData[ 3 ];
    wrkEvent.data[ 4 ] = 0;
    wrkEvent.data[ 5 ] = 0;
    wrkEvent.data[ 6 ] = 0;
    wrkEvent.data[ 7 ] = 0;
    wrkEvent.head = (VSCP_PRIORITY_HIGH << 5);
    wrkEvent.vscp_class = VSCP_CLASS2_PROTOCOL;
    wrkEvent.vscp_type = VSCP2_TYPE_PROTOCOL_WRITE_REGISTER;
    vscp_sendEvent();

}


///////////////////////////////////////////////////////////////////////////////
// vscp_readStdReg
//

uint8_t vscp_readStdReg( uint32_t reg )
{

    uint8_t rv;

    if ( VSCP_REG_ALARMSTATUS == reg ) {

        // * * * Read alarm status register * * *
        rv = vscp_alarmstatus;
        vscp_alarmstatus = 0x00; // Reset alarm status

    }
    else if ( VSCP_REG_VSCP_MAJOR_VERSION == reg ) {

        // * * * VSCP Protocol Major Version * * *
        rv = VSCP_MAJOR_VERSION;

    }
    else if ( VSCP_REG_VSCP_MINOR_VERSION == reg ) {

        // * * * VSCP Protocol Minor Version * * *
        rv = VSCP_MINOR_VERSION;

    }
    else if ( VSCP_REG_NODE_CONTROL == reg ) {

        // * * * Reserved * * *
        rv = 0;

    }
    else if ( ( reg >= VSCP_REG_USERID0 ) &&
            ( reg < VSCP_REG_MANUFACTUR_ID0 ) )  {

        // * * * Read from persitant locations * * *
        rv = vscp_getUserID( reg - VSCP_REG_USERID0 );

    }
    else if ( ( reg >= VSCP_REG_MANUFACTUR_ID0  ) &&
            ( reg < VSCP_REG_NICKNAME_ID ) ) {

        // * * * Manufacturer ID information * * *
        rv = vscp_getManufacturerId( reg - VSCP_REG_MANUFACTUR_ID0 );

    }
    else if ( VSCP_REG_NICKNAME_ID == reg ) {

        // * * * nickname id * * *
        rv = 0xff; // Always undefined  for Level II

    }
    else if ( VSCP_REG_PAGE_SELECT_MSB == reg ) {

        // * * * Page select MSB * * *
        rv = ( vscp_page_select >> 8 ) & 0xff;

    }
    else if ( VSCP_REG_PAGE_SELECT_LSB == reg ) {

        // * * * Page select LSB * * *
        rv = ( vscp_page_select & 0xff );

    }
    else if ( VSCP_REG_FIRMWARE_MAJOR_VERSION == reg ) {

        // * * * Get firmware Major version * * *
        rv = vscp_getFirmwareMajorVersion();

    }
    else if ( VSCP_REG_FIRMWARE_MINOR_VERSION == reg ) {

        // * * * Get firmware Minor version * * *
        rv = vscp_getFirmwareMinorVersion();

    }
    else if ( VSCP_REG_FIRMWARE_SUB_MINOR_VERSION == reg ) {

        // * * * Get firmware Sub Minor version * * *
        rv = vscp_getFirmwareSubMinorVersion();

    }
    else if ( VSCP_REG_BOOT_LOADER_ALGORITHM == reg ) {

        // * * * Boot loader algorithm * * *
        rv = vscp_getBootLoaderAlgorithm();

    }
    else if ( VSCP_REG_BUFFER_SIZE == reg ) {

        // * * * Buffer size * * *
        rv = vscp_getBufferSize();

    }

    else if ( ( reg >= VSCP_REG_GUID ) &&
            ( reg < VSCP_REG_DEVICE_URL  ) ) {

        // * * * GUID * * *
        rv = vscp_getGUID( reg - VSCP_REG_GUID );

    }

    else if ( reg >= VSCP_REG_DEVICE_URL ) {

        // * * * The device URL * * *
        rv = vscp_getMDF_URL( reg - VSCP_REG_DEVICE_URL );

    }

    return rv;
}



///////////////////////////////////////////////////////////////////////////////
// vscp_writeStdReg
//

uint8_t vscp_writeStdReg(uint32_t reg, uint8_t data)
{
    uint8_t rv = ~data;

    if ((reg > (VSCP_REG_VSCP_MINOR_VERSION + 1)) &&
            (reg < VSCP_REG_MANUFACTUR_ID0)) {

        // * * * User Client ID * * *
        vscp_setUserID((reg - VSCP_REG_USERID0), data);
        rv = vscp_getUserID((reg - VSCP_REG_USERID0));

    }
    else if (VSCP_REG_PAGE_SELECT_MSB == reg) {

        // * * * Page select register MSB * * *
        vscp_page_select = (vscp_page_select & 0x00ff) + ((uint16_t) data << 8);
        rv = (vscp_page_select >> 8) & 0xff;
    }
    else if (VSCP_REG_PAGE_SELECT_LSB == reg) {

        // * * * Page select register LSB * * *
        vscp_page_select = (vscp_page_select & 0xff00) + data;
        rv = (vscp_page_select & 0xff);
    }

#ifdef ENABLE_WRITE_2PROTECTED_LOCATIONS

        // Write manufacturer id configuration information
    else if ((vscp_readStdReg > VSCP_REG_USERID4) &&
            (vscp_readStdReg < VSCP_REG_NICKNAME_ID)) {
        // page select must be 0xffff for writes to be possible
        if ((0xff != ((vscp_page_select >> 8) & 0xff)) ||
                (0xff != (vscp_page_select & 0xff))) {
            // return complement to indicate error
            rv = ~data;
        } else {
            // Write
            vscp_setManufacturerId(vscp_readStdReg - VSCP_REG_MANUFACTUR_ID0, data);
            rv = vscp_getManufacturerId(vscp_readStdReg - VSCP_REG_MANUFACTUR_ID0);
        }
    }        // Write GUID configuration information
    else if ((vscp_readStdReg > (VSCP_REG_GUIID - 1)) &&
            (vscp_readStdReg < VSCP_REG_DEVICE_URL)) {
        // page must be 0xffff for writes to be possible
        if ((0xff != ((vscp_page_select >> 8) & 0xff)) ||
                (0xff != (vscp_page_select & 0xff))) {
            // return complement to indicate error
            rv = ~data;
        } else {
            vscp_setGUID(vscp_readStdReg - VSCP_REG_GUIID, data);
            rv = vscp_getGUID(vscp_readStdReg - VSCP_REG_GUIID);
        }
    }
#endif

    else {
        // return complement to indicate error
        rv = ~data;
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// sendServerProbe
//

#ifdef VSCP_DISCOVER_SERVER

void sendHighEndServerProbe(void)
{
    outEvent.head = (VSCP_PRIORITY_HIGH << 5);
    outEvent.sizeData = 5;
    outEvent.vscp_class = VSCP_CLASS1_PROTOCOL;
    outEvent.vscp_type = VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_PROBE;
    // GUID is filled in by send routine
    outEvent.data[0] = 0; // TCP interface
    outEvent.data[1] = vscp_getIPsddress( 0 );
    outEvent.data[2] = vscp_getIPsddress( 1 );
    outEvent.data[3] = vscp_getIPsddress( 2 );
    outEvent.data[4] = vscp_getIPsddress( 3 );

    vscp_sendEvent(&outEvent);
}

#endif


///////////////////////////////////////////////////////////////////////////////
// fillGUID
//

void fillGUID(void)
{
    uint8_t i;

    for (i = 0; i < 16; i++) {
        wrkEvent.GUID[i] = vscp_getGUID(i);
    }
}
