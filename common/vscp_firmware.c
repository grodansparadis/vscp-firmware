///////////////////////////////////////////////////////////////////////////////
// File: vscp_firmware.c
//

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Copyright (C) 1995-2012 Ake Hedman, 
 *  Grodans Paradis AB, <akhe@grodansparadis.com> 
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
#include "vscp_firmware.h"
#include "../../src/vscp/common/vscp_class.h"	
#include "../../src/vscp/common/vscp_type.h"	


#ifndef FALSE
#define FALSE           0
#endif

#ifndef TRUE
#define TRUE            !FALSE
#endif

#ifndef ON
#define ON              !FALSE
#endif

#ifndef OFF
#define OFF             FALSE
#endif 


// Globals

// VSCP Data
uint8_t vscp_nickname; ///< Node nickname

uint8_t vscp_errorcnt; // VSCP/CAN errors
uint8_t vscp_alarmstatus; // VSCP Alarm Status

uint8_t vscp_node_state; // State machine state
uint8_t vscp_node_substate; // State machine substate

uint8_t vscp_probe_cnt; // Number of timout probes

// Incoming event
struct _imsg vscp_imsg;

// Outgoing event
struct _omsg vscp_omsg;

uint8_t vscp_probe_address; // Address used during initialization
uint8_t vscp_initledfunc; // Init LED functionality

volatile uint16_t vscp_timer; // 1 ms timer counter
//	Shold be externally updated.

volatile uint8_t vscp_initbtncnt; // init button counter
//  increase this value externally by one each millisecond
//  the initbutton is pressed. Set to sero when button
// is released.

volatile uint8_t vscp_statuscnt; // status LED counter
//	increase externally bye one every millisecond

// page selector
uint16_t vscp_page_select;

// The GUID reset is used when the VSCP_TYPE_PROTOCOL_RESET_DEVICE
// is received. Bit 4,5,6,7 is set for each received frame with 
// GUID data. Bit 4 is for index = 0, bit 5 is for index = 1 etc.
// This means that a bit is set if a frame with correct GUID is
// received.  
uint8_t vscp_guid_reset;

// Timekeeping
uint8_t vscp_second;
uint8_t vscp_minute;
uint8_t vscp_hour;


///////////////////////////////////////////////////////////////////////////////
// vscp_init
//

void vscp_init(void)
{
    vscp_initledfunc = VSCP_LED_BLINK1;

    // read the nickname id
    vscp_nickname = vscp_readNicknamePermanent();

    //	if zero set to uninitialized
    if (!vscp_nickname) vscp_nickname = VSCP_ADDRESS_FREE;

    // Init incoming event
    vscp_imsg.flags = 0;
    vscp_imsg.priority = 0;
    vscp_imsg.class = 0;
    vscp_imsg.type = 0;

    // Init outgoing event
    vscp_omsg.flags = 0;
    vscp_omsg.priority = 0;
    vscp_omsg.class = 0;
    vscp_omsg.type = 0;

    vscp_errorcnt = 0; // No errors yet
    vscp_alarmstatus = 0; // No alarmstatus

    vscp_probe_address = 0;

    // Initial state
    vscp_node_state = VSCP_STATE_STARTUP;
    vscp_node_substate = VSCP_SUBSTATE_NONE;

    vscp_probe_cnt = 0;
    vscp_page_select = 0;

    // Initialize time keeping
    vscp_timer = 0;
    vscp_second = 0;
    vscp_minute = 0;
    vscp_hour = 0;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_check_pstorage
//
// Check control position integrity and restore EEPROM
// if invalid.
//

int8_t vscp_check_pstorage(void)
{
    // controlbyte == 01xxxxxx means initialized
    // everything else is uninitialized
    if ((vscp_getSegmentCRC() & 0xc0) == 0x40) {
        return TRUE;
    }

    // No nickname yet.
    vscp_writeNicknamePermanent(0xff);

    // No segment CRC yet.
    vscp_setSegmentCRC(0x00);

    // Initial startup
    // write allowed
    vscp_setControlByte(0xA0);

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_error
//

void vscp_error(void)
{
    vscp_initledfunc = VSCP_LED_OFF;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_handleProbe
//

void vscp_handleProbeState(void)
{
    switch (vscp_node_substate) {

        case VSCP_SUBSTATE_NONE:

            if (VSCP_ADDRESS_FREE != vscp_probe_address) {

                vscp_omsg.flags = VSCP_VALID_MSG + 1; // one databyte
                vscp_omsg.priority = VSCP_PRIORITY_HIGH;
                vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                vscp_omsg.type = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
                vscp_omsg.data[ 0 ] = vscp_probe_address;

                // send the probe
                vscp_sendEvent();

                vscp_node_substate = VSCP_SUBSTATE_INIT_PROBE_SENT;
                vscp_timer = 0;

            } else {

                // No free address -> error
                vscp_node_state = VSCP_STATE_ERROR;

                // Tell system we are giving up
                vscp_omsg.flags = VSCP_VALID_MSG + 1; // one databyte
                vscp_omsg.data[ 0 ] = 0xff; // we are unassigned
                vscp_omsg.priority = VSCP_PRIORITY_LOW;
                vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                vscp_omsg.type = VSCP_TYPE_PROTOCOL_PROBE_ACK;

                // send the error event
                vscp_sendEvent();

            }
            break;

        case VSCP_SUBSTATE_INIT_PROBE_SENT:

            if (vscp_imsg.flags & VSCP_VALID_MSG) { // incoming event?

                // Yes, incoming event
                if ((VSCP_CLASS1_PROTOCOL == vscp_imsg.class) &&
                        (VSCP_TYPE_PROTOCOL_PROBE_ACK == vscp_imsg.type)) {

                    // Yes it was an ack from the segment master or a node
                    if (0 == vscp_probe_address) {

                        // Master controller answered
                        // wait for address
                        vscp_node_state = VSCP_STATE_PREACTIVE;
                        vscp_timer = 0; // reset timer

                    } else {

                        // node answered, try next address
                        vscp_probe_address++;
                        vscp_node_substate = VSCP_SUBSTATE_NONE;
                        vscp_probe_cnt = 0;

                    }
                }
            } else {

                if (vscp_timer > VSCP_PROBE_TIMEOUT) { // Check for timeout

                    vscp_probe_cnt++; // Another timeout

                    if (vscp_probe_cnt >= VSCP_PROBE_TIMEOUT_COUNT) {

                        // Yes we have a timeout
                        if (0 == vscp_probe_address) { // master controller probe?

                            // No master controler on segment, try next node
                            vscp_probe_address++;
                            vscp_node_substate = VSCP_SUBSTATE_NONE;
                            vscp_timer = 0;
                            vscp_probe_cnt = 0;

                        } else {

                            // We have found a free address - use it
                            vscp_nickname = vscp_probe_address;
                            vscp_node_state = VSCP_STATE_ACTIVE;
                            vscp_node_substate = VSCP_SUBSTATE_NONE;
                            vscp_writeNicknamePermanent(vscp_nickname);
                            vscp_setSegmentCRC(0x40); // segment code (non server segment )

                            // Report success
                            vscp_probe_cnt = 0;
                            vscp_goActiveState();

                        }
                    } else {
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

    vscp_imsg.flags = 0;

}

///////////////////////////////////////////////////////////////////////////////
// vscp_handlePreActiveState
//

void vscp_handlePreActiveState(void)
{

    if ( vscp_imsg.flags & VSCP_VALID_MSG ) { // incoming event?

        if ((VSCP_CLASS1_PROTOCOL == vscp_imsg.class) &&
                (VSCP_TYPE_PROTOCOL_SET_NICKNAME == vscp_imsg.type) &&
                (VSCP_ADDRESS_FREE == vscp_imsg.data[ 0 ])) {

            // Assign nickname
            vscp_nickname = vscp_imsg.data[ 1 ];
            vscp_writeNicknamePermanent(vscp_nickname);
            vscp_setSegmentCRC(0x40);

            // Go active state
            vscp_node_state = VSCP_STATE_ACTIVE;
        }
    } else {
        // Check for time out
        if (vscp_timer > VSCP_PROBE_TIMEOUT) {
            // Yes, we have a timeout
            vscp_nickname = VSCP_ADDRESS_FREE;
            vscp_writeNicknamePermanent(VSCP_ADDRESS_FREE);
            vscp_init();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_goActiveState
//

void vscp_goActiveState(void)
{
    vscp_omsg.flags = VSCP_VALID_MSG + 1; // one databyte
    vscp_omsg.priority = VSCP_PRIORITY_HIGH;
    vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
    vscp_omsg.type = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
    vscp_omsg.data[ 0 ] = vscp_nickname;

    // send the event
    vscp_sendEvent();

    vscp_initledfunc = VSCP_LED_ON;
}



///////////////////////////////////////////////////////////////////////////////
// vscp_sendHeartbeat
//

void vscp_sendHeartBeat(uint8_t zone, uint8_t subzone)
{
    vscp_omsg.flags = VSCP_VALID_MSG + 3; // three databyte
    vscp_omsg.priority = VSCP_PRIORITY_LOW;
    vscp_omsg.class = VSCP_CLASS1_INFORMATION;
    vscp_omsg.type = VSCP_TYPE_INFORMATION_NODE_HEARTBEAT;
    vscp_omsg.data[ 0 ] = 0;
    vscp_omsg.data[ 1 ] = zone;
    vscp_omsg.data[ 2 ] = subzone;

    // send the event
    vscp_sendEvent();
}

///////////////////////////////////////////////////////////////////////////////
// vscp_handleHeartbeat
//

void vscp_handleHeartbeat(void)
{
    if ((5 == (vscp_imsg.flags & 0x0f)) &&
            (vscp_getSegmentCRC() != vscp_imsg.data[ 0 ])) {

        // Stored CRC are different than received
        // We must be on a different segment
        vscp_setSegmentCRC(vscp_imsg.data[ 0 ]);

        // Introduce ourself in the proper way and start from the beginning
        vscp_nickname = VSCP_ADDRESS_FREE;
        vscp_writeNicknamePermanent(VSCP_ADDRESS_FREE);
        vscp_node_state = VSCP_STATE_INIT;

    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_handleSetNickname
//

void vscp_handleSetNickname(void)
{
    if ((2 == (vscp_imsg.flags & 0x0f)) &&
            (vscp_nickname == vscp_imsg.data[ 0 ])) {

        // Yes, we are addressed
        vscp_nickname = vscp_imsg.data[ 1 ];
        vscp_writeNicknamePermanent(vscp_nickname);
        vscp_setSegmentCRC(0x40);
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_handleDropNickname
//

void vscp_handleDropNickname(void)
{
    if ((1 == (vscp_imsg.flags & 0x0f)) &&
            (vscp_nickname == vscp_imsg.data[ 0 ])) {

        // Yes, we are addressed
        vscp_nickname = VSCP_ADDRESS_FREE;
        vscp_writeNicknamePermanent(VSCP_ADDRESS_FREE);
        vscp_init();

    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_newNodeOnline
//

void vscp_newNodeOnline(void)
{
    if ((1 == (vscp_imsg.flags & 0x0f)) &&
            (vscp_nickname == vscp_imsg.data[ 0 ])) {

        // This is a probe which use our nickname
        // we have to respond to tell the new node that
        // ths nickname is in use

        vscp_omsg.flags = VSCP_VALID_MSG;
        vscp_omsg.priority = VSCP_PRIORITY_HIGH;
        vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
        vscp_omsg.type = VSCP_TYPE_PROTOCOL_PROBE_ACK;
        vscp_sendEvent();

    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_doOneSecondWork
//

void vscp_doOneSecondWork(void)
{

    ++vscp_second;

    if (vscp_second > 59) {
        vscp_second = 0;
        vscp_minute++;

        // send periodic heartbeat
        if (VSCP_STATE_ACTIVE == vscp_node_state) {
            vscp_sendHeartBeat(vscp_getZone(),
                    vscp_getSubzone());
        }
    }

    if (vscp_minute > 59) {
        vscp_minute = 0;
        vscp_hour++;
    }

    if (vscp_hour > 23) vscp_hour = 0;

    // See declaration of vscp_guid_reset

    if (VSCP_STATE_ACTIVE == vscp_node_state) {
        vscp_guid_reset++;
        if ((vscp_guid_reset & 0x0f) >= 2) {
            vscp_guid_reset = 0;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_readRegister
//

uint8_t vscp_readRegister(uint8_t reg)
{
    if (reg >= 0x80) {
        return vscp_readStdReg(reg);
    } else {
        return vscp_readAppReg(reg);
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_readStdReg
//

uint8_t vscp_readStdReg(uint8_t reg)
{
    uint8_t rv = 0;

    if (VSCP_REG_ALARMSTATUS == vscp_imsg.data[ 1 ]) {

        // * * * Read alarm status register * * *
        rv = vscp_alarmstatus;
        vscp_alarmstatus = 0x00; // Reset alarm status

    }
    else if (VSCP_REG_VSCP_MAJOR_VERSION == vscp_imsg.data[ 1 ]) {

        // * * * VSCP Protocol Major Version * * *
        rv = VSCP_MAJOR_VERSION;

    }
    else if (VSCP_REG_VSCP_MINOR_VERSION == vscp_imsg.data[ 1 ]) {

        // * * * VSCP Protocol Minor Version * * *
        rv = VSCP_MINOR_VERSION;

    }
    else if (VSCP_REG_NODE_CONTROL == vscp_imsg.data[ 1 ]) {

        // * * * Reserved * * *
        rv = 0;

    }
    else if (VSCP_REG_FIRMWARE_MAJOR_VERSION == vscp_imsg.data[ 1 ]) {

        // * * * Get firmware Major version * * *
        rv = vscp_getMajorVersion();

    }
    else if (VSCP_REG_FIRMWARE_MINOR_VERSION == vscp_imsg.data[ 1 ]) {

        // * * * Get firmware Minor version * * *
        rv = vscp_getMinorVersion();

    }
    else if (VSCP_REG_FIRMWARE_SUB_MINOR_VERSION == vscp_imsg.data[ 1 ]) {

        // * * * Get firmware Sub Minor version * * *
        rv = vscp_getSubMinorVersion();

    }
    else if (vscp_imsg.data[ 1 ] < VSCP_REG_MANUFACTUR_ID0) {

        // * * * Read from persitant locations * * *
        rv = vscp_getUserID(vscp_imsg.data[ 1 ] - VSCP_REG_USERID0);

    }
    else if ((vscp_imsg.data[ 1 ] > VSCP_REG_USERID4) &&
            (vscp_imsg.data[ 1 ] < VSCP_REG_NICKNAME_ID)) {

        // * * * Manufacturer ID information * * *
        rv = vscp_getManufacturerId(vscp_imsg.data[ 1 ] - VSCP_REG_MANUFACTUR_ID0);

    }
    else if (VSCP_REG_NICKNAME_ID == vscp_imsg.data[ 1 ]) {

        // * * * nickname id * * *
        rv = vscp_nickname;

    }
    else if (VSCP_REG_PAGE_SELECT_LSB == vscp_imsg.data[ 1 ]) {

        // * * * Page select LSB * * *
        rv = (vscp_page_select & 0xff);

    }

    else if (VSCP_REG_PAGE_SELECT_MSB == vscp_imsg.data[ 1 ]) {

        // * * * Page select MSB * * *
        rv = (vscp_page_select >> 8) & 0xff;

    }
    else if (VSCP_REG_BOOT_LOADER_ALGORITHM == vscp_imsg.data[ 1 ]) {
        // * * * Boot loader algorithm * * *
        rv = vscp_getBootLoaderAlgorithm();
    }
    else if (VSCP_REG_BUFFER_SIZE == vscp_imsg.data[ 1 ]) {
        // * * * Buffer size * * *
        rv = vscp_getBufferSize();
    }
    else if (VSCP_REG_PAGES_USED == vscp_imsg.data[ 1 ]) {
        // * * * Register Pages Used * * *
        rv = vscp_getRegisterPagesUsed();
    }
    else if ((vscp_imsg.data[ 1 ] > (VSCP_REG_GUID - 1)) &&
            (vscp_imsg.data[ 1 ] < VSCP_REG_DEVICE_URL)) {

        // * * * GUID * * *
        rv = vscp_getGUID(vscp_imsg.data[ 1 ] - VSCP_REG_GUID);

    }
    else {

        // * * * The device URL * * *
        rv = vscp_getMDF_URL(vscp_imsg.data[ 1 ] - VSCP_REG_DEVICE_URL);

    }

    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeRegister
//

uint8_t vscp_writeRegister(uint8_t reg, uint8_t value)
{
    if (reg >= 0x80) {
        return vscp_writeStdReg(reg, value);
    } else {
        return vscp_writeAppReg(reg, value);
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeStdReg
//

uint8_t vscp_writeStdReg(uint8_t reg, uint8_t value)
{
    uint8_t rv = ~value;

    if ((vscp_imsg.data[ 1 ] > (VSCP_REG_VSCP_MINOR_VERSION + 1)) &&
            (vscp_imsg.data[ 1 ] < VSCP_REG_MANUFACTUR_ID0)) {

        // * * * User Client ID * * *
        vscp_setUserID((vscp_imsg.data[ 1 ] - VSCP_REG_USERID0), vscp_imsg.data[ 2 ]);
        rv = vscp_getUserID((vscp_imsg.data[ 1 ] - VSCP_REG_USERID0));

    }
    else if (VSCP_REG_PAGE_SELECT_MSB == vscp_imsg.data[ 1 ]) {

        // * * * Page select register MSB * * *
        vscp_page_select = (vscp_page_select & 0xff00) | ((uint16_t) vscp_imsg.data[ 2 ] << 8);
        rv = (vscp_page_select >> 8) & 0xff;
    }
    else if (VSCP_REG_PAGE_SELECT_LSB == vscp_imsg.data[ 1 ]) {

        // * * * Page select register LSB * * *
        vscp_page_select = (vscp_page_select & 0xff) | vscp_imsg.data[ 2 ];
        rv = (vscp_page_select & 0xff);
    }

#ifdef ENABLE_WRITE_2PROTECTED_LOCATIONS

        // Write manufacturer id configuration information
    else if ((vscp_imsg.data[ 1 ] > VSCP_REG_USERID4) && (vscp_imsg.data[ 1 ] < VSCP_REG_NICKNAME_ID)) {
        // page select must be 0xffff for writes to be possible
        if ((0xff != ((vscp_page_select >> 8) & 0xff)) ||
                (0xff != (vscp_page_select & 0xff))) {
            // return complement to indicate error
            rv = ~vscp_imsg.data[ 2 ];
        } else {
            // Write
            vscp_setManufacturerId(vscp_imsg.data[ 1 ] - VSCP_REG_MANUFACTUR_ID0, vscp_imsg.data[ 2 ]);
            rv = vscp_getManufacturerId(vscp_imsg.data[ 1 ] - VSCP_REG_MANUFACTUR_ID0);
        }
    }        // Write GUID configuration information
    else if ((vscp_imsg.data[ 1 ] > (VSCP_REG_GUID - 1)) && (vscp_imsg.data[ 1 ] < VSCP_REG_DEVICE_URL)) {
        // page must be 0xffff for writes to be possible
        if ((0xff != ((vscp_page_select >> 8) & 0xff)) ||
                (0xff != (vscp_page_select & 0xff))) {
            // return complement to indicate error
            rv = ~vscp_imsg.data[ 2 ];
        } else {
            vscp_setGUID(vscp_imsg.data[ 1 ] - VSCP_REG_GUID, vscp_imsg.data[ 2 ]);
            rv = vscp_getGUID(vscp_imsg.data[ 1 ] - VSCP_REG_GUID);
        }
    }
#endif

    else {
        // return complement to indicate error
        rv = ~value;
    }

    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeStdReg
//

void vscp_handleProtocolEvent(void)
{

    if (VSCP_CLASS1_PROTOCOL == vscp_imsg.class) {


        switch (vscp_imsg.type) {

            case VSCP_TYPE_PROTOCOL_SEGCTRL_HEARTBEAT:

                vscp_handleHeartbeat();
                break;

            case VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE:

                vscp_newNodeOnline();
                break;

            case VSCP_TYPE_PROTOCOL_SET_NICKNAME:

                vscp_handleSetNickname();
                break;

            case VSCP_TYPE_PROTOCOL_DROP_NICKNAME:

                vscp_handleDropNickname();
                break;

            case VSCP_TYPE_PROTOCOL_READ_REGISTER:

                if ((2 == (vscp_imsg.flags & 0x0f)) &&
                        (vscp_nickname == vscp_imsg.data[ 0 ])) {

                    if (vscp_imsg.data[ 1 ] < 0x80) {

                        vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
                        vscp_omsg.flags = VSCP_VALID_MSG + 2;
                        vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                        vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

                        // Register to read
                        vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];

                        // Read application specific register
                        vscp_omsg.data[ 1 ] = vscp_readAppReg(vscp_imsg.data[ 1 ]);

                        // Send reply data
                        vscp_sendEvent();
                    } else {
                        vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                        vscp_omsg.flags = VSCP_VALID_MSG + 2;
                        vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                        vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

                        // Register to read
                        vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];

                        // Read VSCP register
                        vscp_omsg.data[ 1 ] =
                                vscp_readStdReg(vscp_imsg.data[ 1 ]);

                        // Send event
                        vscp_sendEvent();
                    }
                }
                break;

            case VSCP_TYPE_PROTOCOL_WRITE_REGISTER:

                if ((3 == (vscp_imsg.flags & 0x0f)) &&
                        (vscp_nickname == vscp_imsg.data[ 0 ])) {

                    if (vscp_imsg.data[ 1 ] < 0x80) {

                        vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
                        vscp_omsg.flags = VSCP_VALID_MSG + 2;
                        vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                        vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

                        // Register read
                        vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];

                        // Write application specific register
                        vscp_omsg.data[ 1 ] =
                                vscp_writeAppReg(vscp_imsg.data[ 1 ], vscp_imsg.data[ 2 ]);

                        // Send reply
                        vscp_sendEvent();
                    } else {
                        vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
                        vscp_omsg.flags = VSCP_VALID_MSG + 2;
                        vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                        vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
                        vscp_omsg.data[ 0 ] = vscp_nickname;

                        // Register read
                        vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];

                        // Write VSCP register
                        vscp_omsg.data[ 1 ] =
                                vscp_writeStdReg(vscp_imsg.data[ 1 ], vscp_imsg.data[ 2 ]);

                        // Write event
                        vscp_sendEvent();
                    }
                }
                break;

            case VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER:
                if ((vscp_nickname == vscp_imsg.data[ 0 ]) &&
                        (9 == vscp_imsg.data[ 1 ])) // AVR algorithm 0
                {
                    vscp_goBootloaderMode();
                }

                if ((vscp_nickname == vscp_imsg.data[ 0 ]) &&
                        (1 == vscp_imsg.data[ 1 ]) && // microchip PIC algorithm
                        (vscp_getGUID(0) == vscp_imsg.data[ 2 ]) &&
                        (vscp_getGUID(3) == vscp_imsg.data[ 3 ]) &&
                        (vscp_getGUID(5) == vscp_imsg.data[ 4 ]) &&
                        (vscp_getGUID(7) == vscp_imsg.data[ 5 ]) &&
                        ((vscp_page_select >> 8) == vscp_imsg.data[ 6 ]) &&
                        ((vscp_page_select & 0xff) == vscp_imsg.data[ 7 ])) {

                    vscp_goBootloaderMode();

                }
                break;

            case VSCP_TYPE_PROTOCOL_RESET_DEVICE:

                switch (vscp_imsg.data[ 0 ] >> 4) {

                    case 0:
                        if ((vscp_getGUID(0) == vscp_imsg.data[ 1 ]) &&
                                (vscp_getGUID(1) == vscp_imsg.data[ 2 ]) &&
                                (vscp_getGUID(2) == vscp_imsg.data[ 3 ]) &&
                                (vscp_getGUID(3) == vscp_imsg.data[ 4 ])) {
                            vscp_guid_reset |= 0x10;
                        }
                        break;

                    case 1:
                        if ((vscp_getGUID(4) == vscp_imsg.data[ 1 ]) &&
                                (vscp_getGUID(5) == vscp_imsg.data[ 2 ]) &&
                                (vscp_getGUID(6) == vscp_imsg.data[ 3 ]) &&
                                (vscp_getGUID(7) == vscp_imsg.data[ 4 ])) {
                            vscp_guid_reset |= 0x20;
                        }
                        break;

                    case 2:
                        if ((vscp_getGUID(8) == vscp_imsg.data[ 1 ]) &&
                                (vscp_getGUID(9) == vscp_imsg.data[ 2 ]) &&
                                (vscp_getGUID(10) == vscp_imsg.data[ 3 ]) &&
                                (vscp_getGUID(11) == vscp_imsg.data[ 4 ])) {
                            vscp_guid_reset |= 0x40;
                        }
                        break;

                    case 3:
                        if ((vscp_getGUID(12) == vscp_imsg.data[ 1 ]) &&
                                (vscp_getGUID(13) == vscp_imsg.data[ 2 ]) &&
                                (vscp_getGUID(14) == vscp_imsg.data[ 3 ]) &&
                                (vscp_getGUID(15) == vscp_imsg.data[ 4 ])) {
                            vscp_guid_reset |= 0x80;
                        }
                        break;

                    default:
                        vscp_guid_reset = 0;
                        break;
                }

                if (0xf0 == (vscp_guid_reset & 0xf0)) {
                    // Do a reset
                    vscp_init();
                }
                break;

            case VSCP_TYPE_PROTOCOL_PAGE_READ:

                if (vscp_nickname == vscp_imsg.data[ 0 ]) {

                    uint8_t i;
                    uint8_t pos = 0;
                    uint8_t offset = vscp_imsg.data[ 1 ];
                    uint8_t len = vscp_imsg.data[ 2 ];

                    for (i = 0; i < len; i++) {
                        vscp_omsg.data[ (i % 7) + 1 ] = vscp_readRegister(offset + i);

                        if (i % 7 == 6 || i == (len - 1)) {
                            uint8_t bytes;

                            if (i % 7 == 6) bytes = 7;
                            else bytes = i % 7 + 1;

                            vscp_omsg.flags = VSCP_VALID_MSG + bytes + 1;
                            vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                            vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                            vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE;
                            vscp_omsg.data[ 0 ] = pos; // index

                            // send the event
                            vscp_sendEvent();
                            pos++;
                        }
                    }
                }
                break;

            case VSCP_TYPE_PROTOCOL_PAGE_WRITE:

                if (vscp_nickname == vscp_imsg.data[ 0 ]) {
                    uint8_t i;
                    uint8_t pos = vscp_imsg.data[ 1 ];
                    uint8_t len = (vscp_imsg.flags - 2) & 0x07;

                    for (i = 0; i < len; i++) {
                        // Write VSCP register
                        vscp_writeRegister(pos + i, vscp_imsg.data[ 2 + i ]);
                        vscp_omsg.data[ 1 + i ] = vscp_readRegister(pos + i);
                    }

                    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                    vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE;
                    vscp_omsg.data[ 0 ] = 0; // index
                    vscp_omsg.flags = VSCP_VALID_MSG + len + 1;

                    // send the event
                    vscp_sendEvent();

                }
                break;

            case VSCP_TYPE_PROTOCOL_INCREMENT_REGISTER:

                if (vscp_nickname == vscp_imsg.data[ 0 ]) {

                    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                    vscp_omsg.flags = VSCP_VALID_MSG + 2;
                    vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

                    vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];
                    vscp_omsg.data[ 1 ] = vscp_writeAppReg(
                            vscp_imsg.data[ 1 ],
                            vscp_readAppReg(vscp_imsg.data[ 1 ]) + 1);

                    // send the event
                    vscp_sendEvent();
                }
                break;

            case VSCP_TYPE_PROTOCOL_DECREMENT_REGISTER:

                if (vscp_nickname == vscp_imsg.data[ 0 ]) {

                    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                    vscp_omsg.flags = VSCP_VALID_MSG + 2;
                    vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

                    vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];
                    vscp_omsg.data[ 1 ] = vscp_writeAppReg(
                            vscp_imsg.data[ 1 ],
                            vscp_readAppReg(vscp_imsg.data[ 1 ]) - 1);

                    // send the event
                    vscp_sendEvent();
                }
                break;

            case VSCP_TYPE_PROTOCOL_WHO_IS_THERE:

                if ((vscp_nickname == vscp_imsg.data[ 0 ]) ||
                        (0xff == vscp_imsg.data[ 0 ])) {

                    uint8_t i, j, k = 0;

                    // Send data

                    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                    vscp_omsg.flags = VSCP_VALID_MSG + 8;
                    vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.type = VSCP_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE;

                    for (i = 0; i < 3; i++) // fill up with GUID
                    {
                        vscp_omsg.data[0] = i;

                        for (j = 1; j < 8; j++) {
                            vscp_omsg.data[j] = vscp_getGUID(15 - k++);
                            if (k > 16)
                                break;
                        }

                        if (k > 16)
                            break;

                        vscp_sendEvent();
                    }

                    for (j = 0; j < 5; j++) // fillup previous event with MDF
                    {
                        if (vscp_getMDF_URL(j) > 0)
                            vscp_omsg.data[3 + j] = vscp_getMDF_URL(j);
                        else
                            vscp_omsg.data[3 + j] = 0;
                    }

                    vscp_sendEvent();

                    k = 5; // start offset
                    for (i = 3; i < 7; i++) // fill up with the rest of GUID
                    {
                        vscp_omsg.data[0] = i;

                        for (j = 1; j < 8; j++) {
                            vscp_omsg.data[j] = vscp_getMDF_URL(k++);
                        }
                        vscp_sendEvent();
                    }

                }
                break;

            case VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO:

                if (vscp_nickname == vscp_imsg.data[ 0 ]) {
                    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                    vscp_omsg.flags = VSCP_VALID_MSG + 7;
                    vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.type = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;

                    vscp_getMatrixInfo((char *) vscp_omsg.data);

                    // send the event
                    vscp_sendEvent();
                }
                break;

            case VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF:

                vscp_getEmbeddedMdfInfo();
                break;

            case VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_READ:

                if (vscp_nickname == vscp_imsg.data[ 0 ]) {
                    uint8_t i;
                    uint16_t page_save;
                    uint8_t page_msb = vscp_imsg.data[ 1 ];
                    uint8_t page_lsb = vscp_imsg.data[ 2 ];

                    // Save the current page
                    page_save = vscp_page_select;

                    // Check for valid count
                    if (vscp_imsg.data[ 3 ] > 6) vscp_imsg.data[ 3 ] = 6;

                    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                    vscp_omsg.flags = VSCP_VALID_MSG + 3 + vscp_imsg.data[ 3 ];
                    vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.type = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;

                    for (i = vscp_imsg.data[ 2 ];
                            i < (vscp_imsg.data[ 2 ] + vscp_imsg.data[ 3 ]);
                            i++) {
                        vscp_omsg.data[ 3 + (i - vscp_imsg.data[ 2 ]) ] =
                                vscp_readRegister(i);
                    }

                    // Restore the saved page
                    vscp_page_select = page_save;

                    // send the event
                    vscp_sendEvent();

                }
                break;

            case VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE:

                if (vscp_nickname == vscp_imsg.data[ 0 ]) {
                    uint8_t i;
                    uint16_t page_save;
                    uint8_t page_msb = vscp_imsg.data[ 1 ];
                    uint8_t page_lsb = vscp_imsg.data[ 2 ];

                    // Save the current page
                    page_save = vscp_page_select;

                    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                    vscp_omsg.flags = VSCP_VALID_MSG + 3 + vscp_imsg.data[ 3 ];
                    vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.type = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;

                    for (i = vscp_imsg.data[ 2 ];
                            i < (vscp_imsg.data[ 2 ] + vscp_imsg.data[ 3 ]);
                            i++) {
                        vscp_omsg.data[ 3 +
                                (i - vscp_imsg.data[ 2 ]) ] =
                                vscp_writeRegister(i,
                                vscp_imsg.data[ 4 +
                                (i - vscp_imsg.data[ 2 ]) ]);
                    }

                    // Restore the saved page
                    vscp_page_select = page_save;

                    // send the event
                    vscp_sendEvent();

                }
                break;


            default:
                // Do work load
                break;

        } // switch

    } // CLASS1.PROTOCOL event

}


///////////////////////////////////////////////////////////////////////////////
// vscp_sendEvent
//

int8_t vscp_sendEvent(void)
{
    int8_t rv;

    if (!(rv = sendVSCPFrame(vscp_omsg.class,
            vscp_omsg.type,
            vscp_nickname,
            vscp_omsg.priority,
            (vscp_omsg.flags & 0x0f),
            vscp_omsg.data))) {
        vscp_errorcnt++;
    }

    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getEvent
//

int8_t vscp_getEvent(void)
{
    int8_t rv;


    // Dont read in new event if there already is an event
    // in the input buffer. We return TRUE though to indicate there is
    // a valid event.
    if (vscp_imsg.flags & VSCP_VALID_MSG) return TRUE;


    if ((rv = getVSCPFrame(&vscp_imsg.class,
            &vscp_imsg.type,
            &vscp_imsg.oaddr,
            &vscp_imsg.priority,
            &vscp_imsg.flags,
            vscp_imsg.data))) {

        vscp_imsg.flags |= VSCP_VALID_MSG;
    }

    return rv;

}
