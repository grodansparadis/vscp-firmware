/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2019 Ake Hedman, Grodans Paradis AB
 * <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is part of VSCP - Very Simple Control Protocol
 * https://www.vscp.org
 *
 * ******************************************************************************
 *
 * Transport layers
 * ================
 * Raw Ethernet - Encrypted and unencrypted.
 * UDP - Encrypted and unencrypted.
 * Multicast - Encrypted and unencrypted.
 * TCP/IP - Node connect to server.
 * TCP/IP - Node is connected to by server.
 *
 */

#ifndef VSCP_FIRMWARE_LEVEL2_H
#define VSCP_FIRMWARE_LEVEL2_H

/*
    vscp_compiler defines compiler specific definitons
*/
#include <vscp_compiler.h>

/*
    Projdefs defines project specific definitions and
    define the following variables.

    THIS_FIRMWARE_MAJOR_VERSION             x
    THIS_FIRMWARE_MINOR_VERSION             y
    THIS_FIRMWARE_RELEASE_VERSION           z
    THIS_FIRMWARE_INTERVAL_HEARTBEATS       60
    THIS_FIRMWARE_INTERVAL_CAPS             60
    THIS_FIRMWARE_BUFFER_SIZE               VSCP_MAX (vscp.h)
    THIS_FIRMWARE_ENABLE_LOGGING            1
    THIS_FIRMWARE_ENABLE_ERROR_REPORTING    1
*/

#include <vscp_projdefs.h>

#include <inttypes.h>

/*******************************************************************************
                                    Constants
 ******************************************************************************/

#define VSCP2_MAJOR_VERSION 1
#define VSCP2_MINOR_VERSION 0
#define VSCP2_RELEASE_VERSION 0

/* States */
#define VSCP2_STATE_UNCONNECTED 0
#define VSCP2_STATE_CONNECTED 1
#define VSCP2_STATE_ERROR 2

#define VSCP2_DEFAULT_HEARTBEAT_INTERVAL 60
#define VSCP2_DEFAULT_CAPS_INTERVAL 60

/******************************************************************************
                                   Globals
 ******************************************************************************/

extern uint8_t vscp_alarmstatus;
extern uint8_t vscp_node_state;
extern uint8_t vscp_node_substate;

/******************************************************************************
                                   Prototypes
 ******************************************************************************/

/*!
    Init VSCP subsystem. The transport layer should be up and
    running as the New node on line is sent here when everything is
    initialized.
*/
void
vscp2_init(void);

/*!
    Set heartbeat interval. By default it is set to 60 seconds. Set to
    zero to turn off.
    @param interval Interval for heartbeat event. 0 is off.
*/
void
vscp2_setHeartBeatInterval(uint8_t interval);
void
vscp2_error(void);
void
vscp2_rcv_new_node_online(void);

void
vscp2_readRegister(void);
void
vscp2_writeRegister(void);
uint8_t
vscp2_readStdReg(uint32_t reg);
uint8_t
vscp2_writeStdReg(uint32_t reg, uint8_t data);

void
vscp2_sendHighEndServerProbe(void);

/*!
    Do One second work

    This routine should be called once a second by the
    application.
 */
void
vscp2_doOneSecondWork(void);

/*!
    Get page select bytes
        idx=0 - byte 0 MSB
        idx=1 - byte 1 LSB
 */
uint8_t
vscp_getPageSelect(uint8_t idx);

/*!
    Set page select registers
    @param idx 0 for LSB, 1 for MSB
    @param data Byte to set of page select registers
 */
void
vscp_setPageSelect(uint8_t idx, uint8_t data);

/*!
    Send error event (CLASS=508).
    http://www.vscp.org/docs/vscpspec/doku.php?id=class1.error
    idx can be used to identify the internal part ("submodule") that was the
    origin of the error. Both zone and sub zone are always set to zero.
    @param type This is the VSCP type
    @param idx Index to identify possible sub module. Normally set to zero.
    @return True if event was sent.
 */
#ifdef THIS_FIRMWARE_ENABLE_ERROR_REPORTING
uint8_t
vscp2_sendErrorEvent(uint8_t type, uint8_t idx);
#endif

/*!
    Send log event (CLASS=509).
    http://www.vscp.org/docs/vscpspec/doku.php?id=class1.log
    For loging first send Type = 2(0x01) Log Start then logging events and when
    log is closed send Type = 3 (0x03) Log Stop. To log several things use a
    unique if for each and open/close each.
    @param type VSCP logevent type.
    @param id Identifier for the logging channel.
    @param level Loglevel for this log event.
    @param idx index for multiframe log event starting at zero.
    @param pData Log data (Allways 5 bytes).
    @return TRUE if event was sent.
 */
#ifdef THIS_FIRMWARE_ENABLE_LOGGING
uint8_t
vscp2_sendLogEvent(
  uint8_t type, uint8_t id, uint8_t level, uint8_t idx, uint8_t *pdata);
#endif

/******************************************************************************
                                    Callbacks
 ******************************************************************************/

/*!
    Get major firmware version number.
    @return Major version mumber.
*/
uint8_t
vscp2_getFirmwareMajorVersion(void);

/*!
    Get minor version number.
    @return Minor version number.
*/
uint8_t
vscp2_getFirmwareMinorVersion(void);

/*!
    Get sub minor (release) version number.
    @return subminor version number.
*/
uint8_t
vscp2_getFirmwareSubMinorVersion(void);

/*!
    Send event to transport sublayer.
    @param e Pointer to Event to send.
    @return VSCP_ERROR_SUCCESS on sucess, or error code.
*/
int8_t
vscp2_sendEvent(vscpEvent *e);

/*!
    Send eventex to transport sublayer.
    @param ex Pointer to EventEx to send.
    @return VSCP_ERROR_SUCCESS on sucess, or error code.
*/
int8_t
vscp2_sendEventEx(vscpEventEx *ex);

/*!
    Get event from transport sublayer.
    @param e Pointer to Event to fill data in.
    @return VSCP_ERROR_SUCCESS on sucess, VSCP_ERROR_RCV_EMPTY if
    there is no data else error code.
*/
int8_t
vscp2_getEvent(vscpEvent *e);

/*!
    Get eventex from transport sublayer.
    @param ex Pointer to EventEx to fill data in.
    @return VSCP_ERROR_SUCCESS on sucess, VSCP_ERROR_RCV_EMPTY if
    there is no data else error code.
*/
int8_t
vscp2_getEventEx(vscpEventEx *e);

/*!
    Fill in capabilities information
    @param pCaps Pointer to capabilities array
*/
void
vscp2_setCaps(uit8_t *pCaps);

/*!
    Get bootloader algorithm used to bootload this
    device.
    @return bootloader algorithm (see vscp.h)
*/
uint8_t
vscp2_getBootLoaderAlgorithm(void);

/*!
    Read data from application register.
    @param reg Register to write.
    @return content of application regsister or -1
    if the application register is not defined.
*/
int16_t
vscp2_readAppReg(uint32_t reg);

/*!
    Write data to application register.
    @param reg Register to write.
    @return content of application regsister or -1
    if the application register is not defined.
*/
int
vscp2_writeAppReg(uint32_t reg, uint8_t data);

uint8_t
vscp2_getControlByte(void);
void
vscp2_setControlByte(uint8_t ctrl);

/*!
    Get user id byte
    @param idx Index into user id array  (0-4).
    @return user id byte
*/
uint8_t
vscp2_getUserID(uint8_t idx);

/*!
    Write user id byte
    @param idx Index into user id array  (0-4).
    @param data Data for position.
*/
void
vscp2_setUserID(uint8_t idx, uint8_t data);

/*!
    Get manufacturer id byte
    @param idx Index into manufacturer id array  (0-4).
    @return manufacturer id byte.
*/
uint8_t
vscp2_getManufacturerId(uint8_t idx);

/*!
    Set manufacturer id byte
    @param idx Index into manufacturer id array  (0-4).
    @param data Data for position.
*/
void
vscp2_setManufacturerId(uint8_t idx, uint8_t data);

/*!
    Buffer size is a way to determine how much data a Level II
    device can handle. A constraint device can handle Level II events
    but maybe not the full data size (VSCP_MAX_DATA in vscp.h).
    @return the buffer size (<= 487 bytes) that can be handled.
*/
uint16_t
vscp2_getBufferSize(void);

/*!
    Get GUID for this device.
    @param idx Index into GUID 0=MSB, 15LSB
    @return GUID byte.
*/
uint8_t
vscp2_getGUID(uint8_t idx);

/*!
    Get MDF URL for this device.
    @param idx Index into MDF 0-31
    @return MDF URL byte.
*/
uint8_t
vscp2_getMDF_URL(uint8_t idx);

/*!
    Fetch control byte from permanent storage (idx=0/1)
 */
uint8_t
vscp_getControlByte(uint8_t idx);

/*!
    Write control byte permanent storage (idx=0/1)
 */
void
vscp_setControlByte(uint8_t idx, uint8_t ctrl);

/*!
    Initialize persistent storage
 */
void
vscp_init_pstorage(void);

/*!
    Fedd the decision matrix with one Event
    @param e Event to feed the DM with.
*/
void
vscp2_feedDM(vscpEvent *e);

/*!
    Fedd the decision matrix with one EventEx
    @param xz EventEz to feed the DM with.
*/
void
vscp2_feedDM(vscpEventEx *ex);

/*!
    Read application register (lower part)
    @param reg Register to read (<0x80)
    @return Register content or 0x00 for non valid register
 */
uint8_t
vscp_readAppReg(uint8_t reg);

/*!
    Write application register (lower part)
    @param reg Register to read (<0x80)
    @param value Value to write to register.
    @return Register content or 0xff for non valid register
 */
uint8_t
vscp_writeAppReg(uint8_t reg, uint8_t value);

/*!
    Get DM matrix info
    The output message data structure should be filled with
    the following data by this routine.
        byte 0 - Number of DM rows. 0 if none.
        byte 1 - offset in register space.
        byte 2 - start page MSB
        byte 3 - start page LSB
        byte 4 - End page MSB
        byte 5 - End page LSB
        byte 6 - Level II size of DM row (Just for Level II nodes).
 */
void
vscp_getMatrixInfo(char *pData);

/*!
    Get embedded MDF info
    If available this routine sends an embedded MDF file
    in several events. See specification CLASS1.PROTOCOL
    Type=35/36
 */
void
vscp_getEmbeddedMdfInfo(void);

/*!
    Go boot loader mode
    This routine force the system into boot loader mode according
    to the selected protocol.
 */
void
vscp_goBootloaderMode(uint8_t algorithm);

/*!
    Get Zone for device
    Just return zero if not used.
 */
uint8_t
vscp_getZone(void);

/*!
    Get Subzone for device
    Just return zero if not used.
 */
uint8_t
vscp_getSubzone(void);

/*!
    Get device family code
    return zero for not known.
*/
uint32_t
vscp_getFamilyCode(void);

/*!
    Get device family type
    return zero for not known.
*/
uint32_t
vscp_getFamilyType(void);

/*!
    Restore defaults
    If 0x55/0xaa is written to register location
    162 within one second defaults should be loaded
    by the device.
 */
void
vscp_restoreDefaults(void);

// ============ END OF CALLBACKS ============

#endif
