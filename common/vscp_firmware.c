///////////////////////////////////////////////////////////////////////////////
// File: vscp_firmware.c
//

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
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
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
 */

#include <string.h>
#include <stdlib.h>
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_firmware.h"

#ifndef FALSE
#define FALSE  0
#endif

#ifndef TRUE
#define TRUE   !FALSE
#endif

#ifndef ON
#define ON     !FALSE
#endif

#ifndef OFF
#define OFF    FALSE
#endif 


// Globals

// VSCP Data
uint8_t vscp_nickname;      ///< Node nickname

uint8_t vscp_errorcnt;      // VSCP/CAN errors
uint8_t vscp_alarmstatus;   // VSCP Alarm Status

uint8_t vscp_node_state;    // State machine state
uint8_t vscp_node_substate; // State machine sub state

uint8_t vscp_probe_cnt;     // Number of time-out probes

// Incoming event
struct _imsg vscp_imsg;

// Outgoing event
struct _omsg vscp_omsg;

uint8_t vscp_probe_address;         // Address used during initialization
uint8_t vscp_initledfunc;           // Init. LED functionality

volatile uint16_t vscp_timer;       // 1 ms timer counter
                                    //	increase externally bye one every millisecond

// init button counter
//  increase this value externally by one each millisecond
//  the initbutton is pressed. Set to zero when button
//  is released.
volatile uint8_t vscp_initbtncnt;   


volatile uint8_t vscp_statuscnt;    // status LED counter
                                    //	increase externally bye one every millisecond

volatile uint16_t vscp_configtimer; // configuration timer
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

    // Init. incoming event
    vscp_imsg.flags = 0;
    vscp_imsg.priority = 0;
    vscp_imsg.vscp_class = 0;
    vscp_imsg.vscp_type = 0;

    // Init. outgoing event
    vscp_omsg.flags = 0;
    vscp_omsg.priority = 0;
    vscp_omsg.vscp_class = 0;
    vscp_omsg.vscp_type = 0;

    vscp_errorcnt = 0;      // No errors yet
    vscp_alarmstatus = 0;   // No alarm status

    vscp_probe_address = 0;

    // Initial state
    vscp_node_state = VSCP_STATE_STARTUP;
    vscp_node_substate = VSCP_SUBSTATE_NONE;

    vscp_probe_cnt = 0;
    vscp_page_select = 0;

    // Initialize time keeping
    vscp_timer = 0;
    vscp_configtimer = 0;
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
    // control byte == 01xxxxxx means initialized
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

                vscp_omsg.flags = VSCP_VALID_MSG + 1; // one data byte
                vscp_omsg.priority = VSCP_PRIORITY_HIGH;
                vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
                vscp_omsg.data[ 0 ] = vscp_probe_address;

                // send the probe
                vscp_sendEvent();

                vscp_node_substate = VSCP_SUBSTATE_INIT_PROBE_SENT;
                vscp_timer = 0;

            } 
            else {

                // No free address -> error
                vscp_node_state = VSCP_STATE_ERROR;

                // Tell system we are giving up
                vscp_omsg.flags = VSCP_VALID_MSG + 1;   // one data byte
                vscp_omsg.data[ 0 ] = 0xff;             // we are unassigned
                vscp_omsg.priority = VSCP_PRIORITY_LOW;
                vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_PROBE_ACK;

                // send the error event
                vscp_sendEvent();

            }
            break;

        case VSCP_SUBSTATE_INIT_PROBE_SENT:

            if (vscp_imsg.flags & VSCP_VALID_MSG) { // incoming event?

                // Yes, incoming event
                if ((VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class) &&
                        (VSCP_TYPE_PROTOCOL_PROBE_ACK == vscp_imsg.vscp_type)) {

                    // Yes it was an ack from the segment master or a node
                    if (0 == vscp_probe_address) {

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

                if (vscp_timer > VSCP_PROBE_TIMEOUT) { // Check for time-out

                    vscp_probe_cnt++; // Another time-out

                    if (vscp_probe_cnt >= VSCP_PROBE_TIMEOUT_COUNT) {

                        // Yes we have a time-out
                        if (0 == vscp_probe_address) { // master controller probe?

                            // No master controller on segment, try next node
                            vscp_probe_address++;
                            vscp_node_substate = VSCP_SUBSTATE_NONE;
                            vscp_timer = 0;
                            vscp_probe_cnt = 0;

                        } 
                        else {

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

    vscp_imsg.flags = 0;

}

///////////////////////////////////////////////////////////////////////////////
// vscp_handlePreActiveState
//

void vscp_handlePreActiveState(void)
{

    if ( vscp_imsg.flags & VSCP_VALID_MSG ) { // incoming event?

        if ((VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class) &&
                (VSCP_TYPE_PROTOCOL_SET_NICKNAME == vscp_imsg.vscp_type) &&
                (VSCP_ADDRESS_FREE == vscp_imsg.data[ 0 ])) {

            // Assign nickname
            vscp_nickname = vscp_imsg.data[ 1 ];
            vscp_writeNicknamePermanent(vscp_nickname);
            vscp_setSegmentCRC(0x40);

            // Go active state
            vscp_node_state = VSCP_STATE_ACTIVE;
        }
    } 
    else {
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
    vscp_omsg.flags = VSCP_VALID_MSG + 1; // one data byte
    vscp_omsg.priority = VSCP_PRIORITY_HIGH;
    vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
    vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
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
    vscp_omsg.flags = VSCP_VALID_MSG + 3; // three data byte
    vscp_omsg.priority = VSCP_PRIORITY_LOW;
    vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
    vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_NODE_HEARTBEAT;
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
	if ( !vscp_getSegmentCRC() ) {
		if ( ( 5 == (vscp_imsg.flags & 0x0f ) ) &&
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
	else {
		// First heartbeat seen by this node
		vscp_setSegmentCRC(vscp_imsg.data[ 0 ]);
	}
}

///////////////////////////////////////////////////////////////////////////////
// vscp_handleSetNickname
//

void vscp_handleSetNickname(void)
{
    if ( ( 2 == (vscp_imsg.flags & 0x0f ) ) &&
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
	uint8_t bytes = vscp_imsg.flags & 0x0f;

	#ifdef DROP_NICKNAME_EXTENDED_FEATURES
	uint8_t brake = 0;
	#endif
	
    if ((bytes >= 1) && (vscp_nickname == vscp_imsg.data[ 0 ])) {
        // Yes, we are addressed

#ifdef DROP_NICKNAME_EXTENDED_FEATURES
		// Optional Byte 1: 
		// bit7 - go idle do not start, bit6 - reset persistent storage
		// bit5 - reset device but keep nickname
		// bit5 and bit 7 are concurrent, here I give bit 5 higher priority
		// Optional byte 2: time in seconds before restarting (makes only sense
		// with either none of the bits or only bit 5 of byte1 set.
		if (bytes >= 2) {  // byte 1 does exist
			// bit 6 set: reset persistent storage, continue to check other
			// options in byte 1
			if (vscp_imsg.data[1] & (1<<6)) {
				// reset persistent storage here
				}
			// bit 5 set: reset device, keep nickname, disregard other option
			// below this by using 'brake'
			if ((vscp_imsg.data[1] & (1<<5)) && (brake == 0)) {
				vscp_hardreset();
				brake = 1;}

			// bit 7 set: go idle, e.g. stay in an endless loop until re-power
			if ((vscp_imsg.data[1] & (1<<7)) && (brake == 0)) { 
				vscp_nickname = VSCP_ADDRESS_FREE;
				vscp_writeNicknamePermanent(VSCP_ADDRESS_FREE);
				for (;;) {}; // wait forever
				}
			}
#endif
		// none of the options from byte 1 have been used or byte 1 itself
		// has not been transmitted at all
		if ((bytes == 1) || ((bytes > 1) && (vscp_imsg.data[1] == 0))) {
			// this is the regular behaviour without using byte 1 options
			vscp_nickname = VSCP_ADDRESS_FREE;
			vscp_writeNicknamePermanent(VSCP_ADDRESS_FREE);
	        vscp_init();
			}
#ifdef DROP_NICKNAME_EXTENDED_FEATURES
		// now check if timing was passed in byte 2
		if (bytes > 2) {
			// and waiting for options that made sense
			if ( (vscp_imsg.data[1] == 0) || ( vscp_imsg.data[1] & (1<<6)) ||
			( vscp_imsg.data[1] & (1<<5)) ) {
				// wait platform independently
				vscp_wait_s(vscp_imsg.data[2]);
				}
		}
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_newNodeOnline
//

void vscp_newNodeOnline(void)
{
    if ( ( 1 == ( vscp_imsg.flags & 0x0f ) ) &&
            (vscp_nickname == vscp_imsg.data[ 0 ])) {

        // This is a probe which use our nickname
        // we have to respond to tell the new node that
        // this nickname is in use

        vscp_omsg.flags = VSCP_VALID_MSG;
        vscp_omsg.priority = VSCP_PRIORITY_HIGH;
        vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
        vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_PROBE_ACK;
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
    } 
    else {
        return vscp_readAppReg(reg);
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_readStdReg
//

uint8_t vscp_readStdReg(uint8_t reg)
{
    uint8_t rv = 0;

    if (VSCP_REG_ALARMSTATUS == reg) {

        // * * * Read alarm status register * * *
        rv = vscp_alarmstatus;
        vscp_alarmstatus = 0x00; // Reset alarm status

    }
    else if (VSCP_REG_VSCP_MAJOR_VERSION == reg) {

        // * * * VSCP Protocol Major Version * * *
        rv = VSCP_MAJOR_VERSION;

    }
    else if (VSCP_REG_VSCP_MINOR_VERSION == reg) {

        // * * * VSCP Protocol Minor Version * * *
        rv = VSCP_MINOR_VERSION;

    }
    else if (VSCP_REG_NODE_CONTROL == reg) {

        // * * * Reserved * * *
        rv = 0;

    }
    else if (VSCP_REG_FIRMWARE_MAJOR_VERSION == reg) {

        // * * * Get firmware Major version * * *
        rv = vscp_getMajorVersion();

    }
    else if (VSCP_REG_FIRMWARE_MINOR_VERSION == reg) {

        // * * * Get firmware Minor version * * *
        rv = vscp_getMinorVersion();

    }
    else if (VSCP_REG_FIRMWARE_SUB_MINOR_VERSION == reg) {

        // * * * Get firmware Sub Minor version * * *
        rv = vscp_getSubMinorVersion();

    }
    else if (reg < VSCP_REG_MANUFACTUR_ID0) {

        // * * * Read from persistent locations * * *
        rv = vscp_getUserID(reg - VSCP_REG_USERID0);

    }
    else if ((reg > VSCP_REG_USERID4) &&
            (reg < VSCP_REG_NICKNAME_ID)) {

        // * * * Manufacturer ID information * * *
        rv = vscp_getManufacturerId(reg - VSCP_REG_MANUFACTUR_ID0);

    }
    else if (VSCP_REG_NICKNAME_ID == reg) {

        // * * * nickname id * * *
        rv = vscp_nickname;

    }
    else if (VSCP_REG_PAGE_SELECT_LSB == reg) {

        // * * * Page select LSB * * *
        rv = (vscp_page_select & 0xff);

    }

    else if (VSCP_REG_PAGE_SELECT_MSB == reg) {

        // * * * Page select MSB * * *
        rv = (vscp_page_select >> 8) & 0xff;

    }
    else if (VSCP_REG_BOOT_LOADER_ALGORITHM == reg) {
        // * * * Boot loader algorithm * * *
        rv = vscp_getBootLoaderAlgorithm();
    }
    else if (VSCP_REG_BUFFER_SIZE == reg) {
        // * * * Buffer size * * *
        rv = vscp_getBufferSize();
    }
    else if (VSCP_REG_PAGES_USED == reg) {
        // * * * Register Pages Used * * *
        rv = vscp_getRegisterPagesUsed();
    }
    else if ( ( reg >= VSCP_REG_STANDARD_DEVICE_FAMILY_CODE ) &&
              ( reg < ( VSCP_REG_STANDARD_DEVICE_FAMILY_CODE + 4 ) ) ) {

        uint32_t code = vscp_getFamilyCode();
        uint8_t idx = reg - VSCP_REG_STANDARD_DEVICE_FAMILY_CODE;
        rv = code >> ( ( ( 3 - idx ) * 8 ) & 0xff );
    }
    else if ( ( reg >= VSCP_REG_STANDARD_DEVICE_TYPE_CODE ) &&
              ( reg < ( VSCP_REG_STANDARD_DEVICE_TYPE_CODE + 4 ) ) ) {

        uint32_t code = vscp_getFamilyType();
        uint8_t idx = reg - VSCP_REG_STANDARD_DEVICE_TYPE_CODE;
        rv = code >> ( ( ( 3 - idx ) * 8 ) & 0xff );
    }
    else if ((reg > (VSCP_REG_GUID - 1)) &&
            (reg < VSCP_REG_DEVICE_URL)) {

        // * * * GUID * * *
        rv = vscp_getGUID(reg - VSCP_REG_GUID);

    }
    else if ( reg >= VSCP_REG_DEVICE_URL ) {

        // * * * The device URL * * *
        rv = vscp_getMDF_URL(reg - VSCP_REG_DEVICE_URL);

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
    } 
    else {
        return vscp_writeAppReg(reg, value);
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeStdReg
//

uint8_t vscp_writeStdReg(uint8_t reg, uint8_t value)
{
    uint8_t rv = ~value;

    if ((reg > (VSCP_REG_VSCP_MINOR_VERSION + 1)) &&
            (reg < VSCP_REG_MANUFACTUR_ID0)) {

        // * * * User Client ID * * *
        vscp_setUserID((reg - VSCP_REG_USERID0), value);
        rv = vscp_getUserID((reg - VSCP_REG_USERID0));

    }
    else if (VSCP_REG_PAGE_SELECT_MSB == reg) {

        // * * * Page select register MSB * * *
        vscp_page_select = (vscp_page_select & 0xff) | ((uint16_t) value << 8);
        rv = (vscp_page_select >> 8) & 0xff;
    }
    else if (VSCP_REG_PAGE_SELECT_LSB == reg) {

        // * * * Page select register LSB * * *
        vscp_page_select = (vscp_page_select & 0xff00) | value;
        rv = (vscp_page_select & 0xff);
    }

#ifdef ENABLE_WRITE_2PROTECTED_LOCATIONS

        // Write manufacturer id configuration information
    else if ((reg > VSCP_REG_USERID4) && (reg < VSCP_REG_NICKNAME_ID)) {
        // page select must be 0xffff for writes to be possible
        if ((0xff != ((vscp_page_select >> 8) & 0xff)) ||
                (0xff != (vscp_page_select & 0xff))) {
            // return complement to indicate error
            rv = ~value;
        } 
        else {
            // Write
            vscp_setManufacturerId(reg - VSCP_REG_MANUFACTUR_ID0, value);
            rv = vscp_getManufacturerId(reg - VSCP_REG_MANUFACTUR_ID0);
        }
    }        // Write GUID configuration information
    else if ((reg > (VSCP_REG_GUID - 1)) && (reg < VSCP_REG_DEVICE_URL)) {
        // page must be 0xffff for writes to be possible
        if ((0xff != ((vscp_page_select >> 8) & 0xff)) ||
                (0xff != (vscp_page_select & 0xff))) {
            // return complement to indicate error
            rv = ~value;
        }
        else {
            vscp_setGUID(reg - VSCP_REG_GUID, value);
            rv = vscp_getGUID(reg - VSCP_REG_GUID);
        }
    }
#endif	
	else if ( VSCP_REG_DEFAULT_CONFIG_RESTORE == reg ) {
		if ( 0x55 == value ) {
			vscp_configtimer = 0;
			rv = 0x55;
		}
		else if ( 0xaa == value ) {
			if ( vscp_configtimer < 1000 ) {
				vscp_restoreDefaults();
				rv = 0xaa;	
			}
			else {
				rv = 0;	// false		
			}
		}
			
	}
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

    if (VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class) {


        switch (vscp_imsg.vscp_type) {

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

            if ( ( 2 == (vscp_imsg.flags & 0x0f) ) &&
                (vscp_nickname == vscp_imsg.data[ 0 ])) {

                if (vscp_imsg.data[ 1 ] < 0x80) {

                    // Read application specific register
                    vscp_omsg.data[ 1 ] = vscp_readAppReg(vscp_imsg.data[ 1 ]);

                    // Register to read
                    vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];

                    vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
                    vscp_omsg.flags = VSCP_VALID_MSG + 2;
                    vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

                    // Send reply data
                    vscp_sendEvent();
                } 
                else {

                    // Read VSCP register
                    vscp_omsg.data[ 1 ] =
                        vscp_readStdReg(vscp_imsg.data[ 1 ]);

                    // Register to read
                    vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];

                    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                    vscp_omsg.flags = VSCP_VALID_MSG + 2;
                    vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

                    // Send event
                    vscp_sendEvent();
                }
            }
            break;

        case VSCP_TYPE_PROTOCOL_WRITE_REGISTER:

            if ((3 == (vscp_imsg.flags & 0x0f)) &&
                (vscp_nickname == vscp_imsg.data[ 0 ])) {

                if (vscp_imsg.data[ 1 ] < 0x80) {

                    // Write application specific register
                    vscp_omsg.data[ 1 ] =
                        vscp_writeAppReg(vscp_imsg.data[ 1 ], vscp_imsg.data[ 2 ]);

                    // Register read
                    vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];

                    vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
                    vscp_omsg.flags = VSCP_VALID_MSG + 2;
                    vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

                    // Send reply
                    vscp_sendEvent();

                } 
                else {

                    // Write VSCP register
                    vscp_omsg.data[ 1 ] =
                        vscp_writeStdReg(vscp_imsg.data[ 1 ], vscp_imsg.data[ 2 ]);

                    // Register read
                    vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];

                    vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
                    vscp_omsg.flags = VSCP_VALID_MSG + 2;
                    vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                    vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

                    // Write event
                    vscp_sendEvent();
                }
            }
            break;

        case VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER:

            if ((vscp_nickname == vscp_imsg.data[ 0 ]) &&
                // byte 1 contains algorithm. Handle in callback.
                (vscp_getGUID(0) == vscp_imsg.data[ 2 ]) &&
                (vscp_getGUID(3) == vscp_imsg.data[ 3 ]) &&
                (vscp_getGUID(5) == vscp_imsg.data[ 4 ]) &&
                (vscp_getGUID(7) == vscp_imsg.data[ 5 ]) &&
                (((vscp_page_select >> 8) & 0xff) == vscp_imsg.data[ 6 ]) &&
                ((vscp_page_select & 0xff) == vscp_imsg.data[ 7 ])) {

                vscp_goBootloaderMode( vscp_imsg.data[ 1 ] );
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

                    if ( (i % 7) == 6 || i == (len - 1) ) {

                        uint8_t bytes;

                        if ( ( i % 7 ) == 6 ) {
                            bytes = 7;
                        }
                        else {
                            bytes = ( i % 7 ) + 1;
                        }

                        vscp_omsg.flags = VSCP_VALID_MSG + bytes + 1;
                        vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                        vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                        vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE;
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
                vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE;
                vscp_omsg.data[ 0 ] = 0; // index
                vscp_omsg.flags = VSCP_VALID_MSG + len + 1;

                // send the event
                vscp_sendEvent();

            }
            break;

        case VSCP_TYPE_PROTOCOL_INCREMENT_REGISTER:

            if (vscp_nickname == vscp_imsg.data[ 0 ]) {

                vscp_omsg.data[ 1 ] = vscp_writeAppReg(
                    vscp_imsg.data[ 1 ],
                    vscp_readAppReg(vscp_imsg.data[ 1 ]) + 1);

                vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];

                vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                vscp_omsg.flags = VSCP_VALID_MSG + 2;
                vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

                // send the event
                vscp_sendEvent();
            }
            break;

        case VSCP_TYPE_PROTOCOL_DECREMENT_REGISTER:

            if (vscp_nickname == vscp_imsg.data[ 0 ]) {

                vscp_omsg.data[ 1 ] = vscp_writeAppReg(
                    vscp_imsg.data[ 1 ],
                    vscp_readAppReg(vscp_imsg.data[ 1 ]) - 1);

                vscp_omsg.data[ 0 ] = vscp_imsg.data[ 1 ];

                vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                vscp_omsg.flags = VSCP_VALID_MSG + 2;
                vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

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
                vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE;

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

                for (j = 0; j < 5; j++) // fill up previous event with MDF
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
                vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;

                vscp_getMatrixInfo((char *) vscp_omsg.data);

                // send the event
                vscp_sendEvent();
            }
            break;

#ifdef EMBEDDED_MDF
        case VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF:

            vscp_getEmbeddedMdfInfo();
            break;
#endif

        case VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_READ:

            if ( vscp_nickname == vscp_imsg.data[0] ) {

                uint16_t page_save;
                uint8_t byte = 0, bytes = 0;
                uint8_t bytes_this_time, cb;

                // if data byte 4 of the request is present probably more than 1 register should be
                // read/written, therefore check lower 4 bits of the flags and decide
                if ( ( vscp_imsg.flags & 0x0f) > 3 ) {

                    // Number of registers was specified', thus take that value
                    bytes = vscp_imsg.data[4];
                    // if number of bytes was zero we read one byte 
                    if ( 0 == bytes ) {
                        bytes = 1;
					} 
				}
				else {
					bytes = 1;
				}

				// Save the current page
				page_save = vscp_page_select;

				// Assign the requested page, this variable is used in the implementation
				// specific function 'vscp_readAppReg()' and 'vscp_writeAppReg()' to actually
				// switch pages there
				vscp_page_select = ((vscp_imsg.data[1] << 8) | vscp_imsg.data[2]);

				// Construct response event
				vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
				vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
				vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;
				vscp_omsg.data[0] = 0; // index of event, this is the first
				vscp_omsg.data[1] = vscp_imsg.data[1]; // mirror page msb
				vscp_omsg.data[2] = vscp_imsg.data[2]; // mirror page lsb

				do {
                    // calculate bytes to transfer in this event
                    if ( ( bytes - byte ) >= 4 ) {
                        bytes_this_time = 4;
					} 
					else {
						bytes_this_time = (bytes - byte);
					}

					// define length of this event
					vscp_omsg.flags = VSCP_VALID_MSG + 4 + bytes_this_time;
					vscp_omsg.data[3] =
                    vscp_imsg.data[3] + byte; // first register in this event

					// Put up to four registers to data space
					for ( cb = 0; cb < bytes_this_time; cb++ ) {
                        vscp_omsg.data[ (4 + cb) ] =
							vscp_readRegister( ( vscp_imsg.data[3] + byte + cb ) );
					}

					// send the event
					vscp_sendEvent();

					// increment byte by bytes_this_time and the event number by one
					byte += bytes_this_time;

					// increment the index
					vscp_omsg.data[0] += 1;
						
				} while (byte < bytes);

				// Restore the saved page
				vscp_page_select = page_save;

			}
            break;

        case VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE:

            if ( vscp_nickname == vscp_imsg.data[ 0 ] ) {

                uint8_t i;
                uint16_t page_save;

                // Save the current page
                page_save = vscp_page_select;

                // Assign the requested page
                // specific function 'vscp_readAppReg()' and 'vscp_writeAppReg()' to actually
                vscp_page_select = (vscp_imsg.data[1] << 8) | vscp_imsg.data[2];

                for (i = vscp_imsg.data[ 3 ]; // register to write
                        // number of registers to write comes from byte length of write event
                        // reduced by four bytes
                        i < (vscp_imsg.data[ 3 ] + ((vscp_imsg.flags & 0x0f) - 4));
                        i++) {
                    vscp_omsg.data[ 4 + (i - vscp_imsg.data[ 3 ]) ] =
                        vscp_writeRegister(i, vscp_imsg.data[ 4 + (i - vscp_imsg.data[ 3 ]) ]);
                }

                // Restore the saved page
                vscp_page_select = page_save;

                vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
                vscp_omsg.flags = VSCP_VALID_MSG + 4 + ((vscp_imsg.flags & 0x0f) - 4);
                vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
                vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;
                vscp_omsg.data[0] = 0; // index of event, this is the first and only
                vscp_omsg.data[1] = vscp_imsg.data[1]; // mirror page msb
                vscp_omsg.data[2] = vscp_imsg.data[2]; // mirror page lsb
                vscp_omsg.data[3] = vscp_imsg.data[3]; // Register

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

    if ( !(rv = sendVSCPFrame( vscp_omsg.vscp_class,
                                vscp_omsg.vscp_type,
                                vscp_nickname,
                                vscp_omsg.priority,
                                (vscp_omsg.flags & 0x0f),
                                vscp_omsg.data ) ) ) {
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


    if ((rv = getVSCPFrame(&vscp_imsg.vscp_class,
            &vscp_imsg.vscp_type,
            &vscp_imsg.oaddr,
            &vscp_imsg.priority,
            &vscp_imsg.flags,
            vscp_imsg.data))) {

        vscp_imsg.flags |= VSCP_VALID_MSG;
    }

    return rv;

}
