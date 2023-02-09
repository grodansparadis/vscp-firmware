/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2023 Ake Hedman, Grodans Paradis AB
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
 */

#ifndef VSCP_FIRMWARE_LEVEL2_H
#define VSCP_FIRMWARE_LEVEL2_H

/*
    vscp_compiler and project defines
*/
#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#ifdef __cplusplus
}
#endif

/*!
  @file vscp_firmware_level2.h
  @defgroup vscp-level2-firmware VSCP level II firmware

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

  @{
*/

#include <inttypes.h>
#include <vscp-projdefs.h>

/*****************************************************************************
                                    Constants
 *****************************************************************************/

/*!
  @brief Major version.

  This is the major version of the vscp link protocol code.
*/
#define VSCP2_MAJOR_VERSION 1

/*!
  @brief Minor Version.

  This is the minor version of the vscp link protocol code.
*/
#define VSCP2_MINOR_VERSION 6

/*!
  @brief Sub version.

  This is the sub version of the vscp link protocol code.
*/
#define VSCP2_SUB_VERSION 1

/*!
  @brief Standard register address start.

  VSCP level II protocol standard register abstraction start address.
*/
#define VSCP2_STANDARD_REGISTER_START 0xffff0000


// -----------------------------------------------------------------------------
//  			     VSCP standard register - Logical positions
// -----------------------------------------------------------------------------


/*!
  @brief Level II alarm standard abstraction register address.

  VSCP standard register abstraction register for alarms.
*/
#define VSCP2_STD_REG_ALARMSTATUS 0xFFFFFF80

/*!
  @brief Level II VSCP Major version standard abstraction register address.

  Protocol conformance major version. This register holds minor version of the VSCP
  specification version the firmware of the device conforms to.

  Described [here](https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_register_abstraction_model?id=level-ii-register-abstraction-model)
  in the VSCP specification.
*/
#define VSCP2_STD_REG_VSCP_MAJOR_VERSION 0xFFFFFF81

/*!
  @brief Level II VSCP Minor Version standard abstraction register address.

  Protocol conformance minor version. This register holds minor version of the VSCP
  specification version the firmware of the device conforms to.

  Described [here](https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_register_abstraction_model?id=level-ii-register-abstraction-model)
  in the VSCP specification.
*/
#define VSCP2_STD_REG_VSCP_MINOR_VERSION 0xFFFFFF82


#define VSCP2_STD_REG_NODE_ERROR_COUNTER 0xFFFFFF83

#define VSCP2_STD_REG_USERID0 0xFFFFFF84
#define VSCP2_STD_REG_USERID1 0xFFFFFF85
#define VSCP2_STD_REG_USERID2 0xFFFFFF86
#define VSCP2_STD_REG_USERID3 0xFFFFFF87
#define VSCP2_STD_REG_USERID4 0xFFFFFF88

#define VSCP2_STD_REG_MANUFACTURER_ID0 0xFFFFFF89
#define VSCP2_STD_REG_MANUFACTURER_ID1 0xFFFFFF8A
#define VSCP2_STD_REG_MANUFACTURER_ID2 0xFFFFFF8B
#define VSCP2_STD_REG_MANUFACTURER_ID3 0xFFFFFF8C

#define VSCP2_STD_REG_MANUFACTURER_SUBID0 0xFFFFFF8D
#define VSCP2_STD_REG_MANUFACTURER_SUBID1 0xFFFFFF8E
#define VSCP2_STD_REG_MANUFACTURER_SUBID2 0xFFFFFF8F
#define VSCP2_STD_REG_MANUFACTURER_SUBID3 0xFFFFFF90

#define VSCP2_STD_REG_NICKNAME_ID 0xFFFFFF91

#define VSCP2_STD_REG_PAGE_SELECT_MSB 0xFFFFFF92
#define VSCP2_STD_REG_PAGE_SELECT_LSB 0xFFFFFF93

#define VSCP2_STD_REG_FIRMWARE_MAJOR_VERSION   0xFFFFFF94
#define VSCP2_STD_REG_FIRMWARE_MINOR_VERSION   0xFFFFFF95
#define VSCP2_STD_REG_FIRMWARE_RELEASE_VERSION 0xFFFFFF96

#define VSCP2_STD_REG_BOOT_LOADER_ALGORITHM 0xFFFFFF97
#define VSCP2_STD_REG_BUFFER_SIZE           0xFFFFFF98
#define VSCP2_STD_REG_PAGES_USED            0xFFFFFF99

// 32-bit
#define VSCP2_STD_REG_STANDARD_DEVICE_FAMILY_CODE 0xFFFFFF9A

// 32-bit
#define VSCP2_STD_REG_STANDARD_DEVICE_TYPE_CODE 0xFFFFFF9E

#define VSCP2_STD_REG_DEFAULT_CONFIG_RESTORE 0xFFFFFFA2

// 16-bit
#define VSCP2_STD_REG_FIRMWARE_CODE 0xFFFFFFA3

#define VSCP2_STD_REG_GUID       0xFFFFFFD0
#define VSCP2_STD_REG_DEVICE_URL 0xFFFFFFE0

// -----------------------------------------------------------------------------

/* States */
#define VSCP2_STATE_STARTUP     0x00
#define VSCP2_STATE_UNCONNECTED 0x01
#define VSCP2_STATE_CONNECTED   0x02
#define VSCP2_STATE_ERROR       0x03

  /*!
  ****************************************************************************
                                     Globals
  *****************************************************************************
   */

  extern uint8_t vscp_node_state;
extern uint8_t vscp_node_substate;

/*!
****************************************************************************
                                   Prototypes
*****************************************************************************
*/

/*!
 * @brief Init subsystem
 *
 * @param pdata Pointer to user data (typical points to context)
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Init VSCP subsystem. The transport layer should be up and
 * running as a New node on line is sent here when everything is
 * initialized.
 */

int
vscp2_init(const void* pdata);

/*!
 * @brief Do periodic VSCP protocol work
 *
 * @param pdata Pointer to user data (typical points to context)
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Do periodic VSCP protocol work. This function should be called from the
 * main loop on periodic intervals.
 *
 * If there is incoming events to handle a pointer to the event should be
 * sent as a parameter  in **pex**. * Once the function has bern called
 * ownership of the event is left over to the protocol stack and it is up to the
 * function to release the event. This is true also if the function returns an error.
 *
 * If there is no event to handle for the protocol stack pex should be set to NULL.
 *
 */

int
vscp2_do_work(vscpEvent* pev);

/*!
 * @brief Read VSCP Level II register
 *
 * @param reg Register to write
 * @param pval Pointer to read value
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp2_read_reg(uint32_t reg, uint8_t* pval);

/*!
 * @brief Write VSCL level II register
 *
 * @param reg Register to write
 * @param val Value to write
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Write a value to a standard or a user register.
 */

int
vscp2_write_reg(uint32_t reg, uint8_t val);

/*!
 * @brief Send periodic heartbeat
 *
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Send heartbeat event to client. Typically send one every minute.
 */

int
vscp2_send_heartbeat(void);

/*!
 * @brief Send periodic capability heartbeat
 *
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Send capabilities event to client. Typically send every minute.
 */

int
vscp2_send_caps(void);

/*!
 * @brief Send high end server probe
 *
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

#ifdef THIS_FIRMWARE_VSCP_DISCOVER_SERVER
int
vscp2_send_high_end_server_probe(void);
#endif


/*!
 * @brief Do register reads
 *
 * @param pex Pointer to event containing register read data
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp2_do_register_read(vscpEvent* pev);


/*!
 * @brief Do register writes
 *
 * @param pex Pointer to event containing register write data
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp2_do_register_write(vscpEvent* pev);


#ifdef THIS_FIRMWARE_ENABLE_ERROR_REPORTING
/*!
  @brief Send error event (CLASS=508).

  @param type This is the VSCP type
  @param idx Index to identify possible sub module. Normally set to zero.
  @return VSCP_ERROR_SUCCESS on success, else error code.

  __THIS_FIRMWARE_ENABLE_ERROR_REPORTING__ should be defined (in projdefs.h) for this function to be compiled in.

  See specification [here](https://grodansparadis.github.io/vscp-doc-spec/#/./class1.error)

  idx can be used to identify the internal part ("submodule") that was the
  origin of the error. Both zone and sub zone are always set to zero.
 */

uint8_t
vscp2_send_error_event(uint8_t type, uint8_t idx);

#endif

#ifdef THIS_FIRMWARE_ENABLE_LOGGING
/*!
  @brief Send log event (CLASS=509).

  @param type VSCP logevent type.
  @param id Identifier for the logging channel.
  @param level Loglevel for this log event.
  @param idx index for multiframe log event starting at zero.
  @param pData Log data (Always 5 bytes).
  @return VSCP_ERROR_SUCCESS on success, else error code.

  __THIS_FIRMWARE_ENABLE_LOGGING__ should be defined (in projdefs.h) for this function to be compiled in.

  See specification [here](https://grodansparadis.github.io/vscp-doc-spec/#/./class1.log)

  For logging first send Type = 2(0x01) Log Start then logging events and when
  log is closed send Type = 3 (0x03) Log Stop. To log several things use a
  unique if for each and open/close each.
 */

uint8_t
vscp2_send_log_event(uint8_t type, uint8_t id, uint8_t level, uint8_t idx, uint8_t* pLogdata);

#endif

#ifdef THIS_FIRMWARE_VSCP_DISCOVER_SERVER

/*!
   @brief Send high end server probe
  
   @return VSCP_ERROR_SUCCESS on success, else error code.
  
   __THIS_FIRMWARE_VSCP_DISCOVER_SERVER__ should be defined (in projdefs.h) for this function to be compiled in.
   
   Inform possible high end servers of our existence.
 */

int
vscp2_send_high_end_server_probe(void);

#endif

/*
 ****************************************************************************
                                    Callbacks
 *****************************************************************************
 */

/**
  @name Callbacks for VSCP level II protocol
*/

/*!
 * @brief Get the time in milliseconds.
 *
 * @param pdata Pointer to user data (typical points to context)
 * @param ptime Pointer to unsigned integer that will get the time in milliseconds.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp2_protocol_callback_get_ms(const void* pdata, uint32_t* ptime);

/*!
 * @brief Get pointer to GUID
 *
 * Get a pointer to the 16-byte GUID byte array.
 *
 * @param pdata Pointer to user data (typical points to context)
 * @return Pointer to the device GUID.
 *
 */

const uint8_t*
vscp2_protocol_callback_get_guid(const void* pdata);

/*!
 * @brief Read user register
 *
 * Read value of one of the user registers. Valid register range
 * is 0x00000000 - 0xffff0000
 *
 * @param pdata Pointer to user data (typical points to context).
 * @param reg Register to read.
 * @param pval Pointer to variable that will get read value.
 * @return VSCP_ERROR_SUCCESS on success, else error code. VSCP_ERROR_PARAMETER is
 * returned if the register is invalid.
 *
 */
int
vscp2_protocol_callback_read_user_reg(const void* pdata, uint32_t reg, uint8_t* pval);

/*!
 * @brief Write user register
 *
 * Write value of one of the user registers. Valid register range
 * is 0x00000000 - 0xffff0000
 *
 * @param pdata Pointer to user data (typical points to context)
 * @param reg Register to write
 * @param val Value to write
 * @return VSCP_ERROR_SUCCESS on success, else error code. VSCP_ERROR_PARAMETER is
 * returned if the register is invalid.
 */

int
vscp2_protocol_callback_write_user_reg(const void* pdata, uint32_t reg, uint8_t val);

/*!
  @brief Send event to transport sublayer.

  @param pdata Pointer to user data (typical points to context)
  @param pex Pointer to Event to send.
  @return VSCP_ERROR_SUCCESS on success, or error code.

  - ev is taken over in the callback and it is responsible for releasing it.
  - if GUID is all zero then the GUID is set to the GUID of the node.
*/

int
vscp2_protocol_callback_send_event(const void* pdata, vscpEvent* pev);

/*!
  @brief Send eventex to transport sublayer.

  @param pdata Pointer to user data (typical points to context)
  @param ex Pointer to EventEx to send.
  @return VSCP_ERROR_SUCCESS on success, or error code.

  - ex is copied in the callback.
  - if GUID is all zero then the GUID is set to the GUID of the node.
*/

int
vscp2_protocol_callback_send_eventEx(const void* pdata, vscpEvent* pev);

/*!
 * @brief Enter bootloader.
 *
 * @param pdata Pointer to user data (typical points to context).
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 */

int
vscp2_protocol_callback_enter_bootloader(const void* pdata);

/*!
 * @brief Reply with DM content.
 *
 * @param pdata Pointer to user data (typical points to context).
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 *
 * Report full content of DM back.
 */

int
vscp2_protocol_callback_report_dmatrix(const void* pdata);

/*!
 * @brief Reply with embedded DM.
 *
 * @param pdata Pointer to user data (typical points to context).
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 *
 * Report embedded mdf back.
 */

int
vscp2_protocol_callback_report_mdf(const void* pdata);

/*!
 * @brief Report back events that this node is interested in
 *
 * @param pdata Pointer to user data (typical points to context)
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp2_protocol_callback_report_events_of_interest(const void* pdata);

/*!
 * @brief Get timestamp
 *
 * @param pdata Pointer to context.
 * @return Timestamp in microseconds.
 */

uint32_t
vscp2_protocol_callback_get_timestamp(const void* pdata);

/*!
 * @brief  Fill in event time information
 *
 * @param pdata
 * @param pex Event to get info
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Set all to zero or do nothing if not time information is available
 * and the time values are already set to zero.
 */

int
vscp2_protocol_callback_get_time(const void* pdata, const vscpEvent* pev);

/*!
  @brief Get user id

  @param pdata Pointer to context
  @param pos Position 0-4 of user id to read
  @param pval Pointer to variable that will get user id
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_protocol_callback_get_user_id(const void* pdata, uint8_t pos, uint8_t* pval);

/*!
  @brief Write user id

  @param pdata Pointer to context
  @param pos Position 0-8 of manufacturer id to write
  @param val Value to write
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_protocol_callback_write_user_id(const void* pdata, uint8_t pos, uint8_t val);

#ifdef THIS_FIRMWARE_ENABLE_WRITE_2PROTECTED_LOCATIONS

/*!
  @brief Get manufacturer id

  @param pdata Pointer to context
  @param pos Position 0-7 of manufacturer id to read
  @param pval Pointer to variable that will get manufacturer id
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_protocol_callback_get_manufacturer_id(const void* pdata, uint8_t pos, uint8_t* pval);

/*!
  @brief Write manufacturer id

  @param pdata Pointer to context
  @param pos Position 0-7 of user id to write
  @param val Value to write
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_protocol_callback_write_manufacturer_id(const void* pdata, uint8_t pos, uint8_t val);

/*!
  @brief Write guid byte

  @param pdata Pointer to user data (typical points to context)
  @param pos Position 0-15 of GUID byte to write
  @param val Value to write
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_protocol_callback_write_guid(const void* pdata, uint8_t pos, uint8_t val);

#endif

/*!
  @brief Get bootloader algorithm used to bootload this
  device.

  @param pdata Pointer to user data (typical points to context)
  @return bootloader algorithm (see vscp.h)
*/

int
vscp2_protocol_callback_get_bootloader_algorithm(const void* pdata);

/*!
  @brief Get persistent storage control byte

  @param pdata Pointer to user data (typical points to context)
  @param pval Pointer to variable that will get bootloader version
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_protocol_callback_get_control_byte(const void* pdata);

/*!
  @brief Set persistent storage control byte

  @param pdata Pointer to user data (typical points to context)
  @param pval Pointer to variable that will get bootloader version
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_protocol_callback_set_control_byte(const void* pdata, uint8_t ctrl);

/*!
  @brief Initialize persistent storage

  @param pdata Pointer to user data (typical points to context)
 */
int
vscp2_protocol_callback_init_persistent_storage(const void* pdata);

/*!
  @brief Feed the decision matrix with one Event

  @param pdata Pointer to user data (typical points to context)
  @param ev Event to feed the DM with.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp2_protocol_callback_feed_dm(const void* pdata, vscpEventEx* ev);

/*!
  @brief All events except level I/II protocol events is sent to the
  application for handling.

  @param pdata Pointer to user data (typical points to context)
  @param pex Event to feed the DM with.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_protocol_callback_feed_app(const void* pdata, vscpEvent* pev);

/*!
  @brief Get DM matrix info

  The output message data structure should be filled with
  the following data by this routine.
      byte 0 - Number of DM rows. 0 if none.
      byte 1 - offset in register space.
      byte 2 - start page MSB
      byte 3 - start page LSB
      byte 4 - End page MSB
      byte 5 - End page LSB
      byte 6 - Level II size of DM row (Just for Level II nodes).

  @param pdata Pointer to user data (typical points to context)
  @param pDM Pointer to DM info structure.
  @return VSCP_ERROR_SUCCESS on success, or error code.
 */

int
vscp2_protocol_callback_send_dm_info(const void* pdata, char* pDM);

/*!
  @brief Get embedded MDF info

  If available this routine sends an embedded MDF file
  in several events. See specification CLASS1.PROTOCOL
  Type=35/36

  @param pdata Pointer to user data (typical points to context)
  @return VSCP_ERROR_SUCCESS on success, or error code.
 */
int
vscp2_protocol_callback_send_embedded_mdf(const void* pdata);

/*!
  @brief Go boot loader mode

  This routine force the system into boot loader mode according
  to the selected protocol.

  @param pdata Pointer to user data (typical points to context)
  @param[out] palgorithm Pointer to bootloader algorithm to use.
  @return VSCP_ERROR_SUCCESS on success, or error code.
 */

int
vscp2_protocol_callback_go_bootloader(const void* pdata, uint8_t* palgorithm);

/*!
  @brief Get Zone for device

  Just return zero if not used.

  @param pdata Pointer to user data (typical points to context)
  @return Zone number.
 */

int
vscp2_protocol_callback_get_zone(const void* pdata);

/*!
  @brief Get Subzone for device

  Just return zero if not used.

  @param pdata Pointer to user data (typical points to context)
  @return Subzone number.
 */

int
vscp2_protocol_callback_get_subzone(const void* pdata);

/*!
  @brief Restore defaults

  If 0x55/0xaa is written to register location
  162 within one second defaults should be loaded
  by the device.

  @param pdata Pointer to user data (typical points to context)
  @return VSCP_ERROR_SUCCESS on success, or error code.
 */

int
vscp2_protocol_callback_restore_defaults(const void* pdata);

/**
 * @brief Return ipv6 or ipv4 address
 *
 * Return the ipv6 or ipv4 address of the interface. If the
 * interface is not tcp/ip based just return a positive
 * response or a valid address for the underlying transport protocol.
 *
 * The address is always sixteen bytes long.
 *
 * @param pdata Pointer to context.
 * @param pipaddr Pointer to 16 byte address space for (ipv6 or ipv4) address
 *                return value.
 * @return VSCP_ERROR_SUCCESS on success, error code on failure
 */

int
vscp2_protocol_callback_get_ip_addr(const void* pUserData, uint8_t* pipaddr);

#ifdef THIS_FIRMWARE_VSCP_DISCOVER_SERVER

/*!
  @brief Handle high end server response

  @param pdata Pointer to user data (typical points to context)
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_protocol_callback_high_end_server_response(const void* pUserData);

#endif

// ============ END OF CALLBACKS ============

/**
@}
*/

#ifdef __cplusplus
}
#endif

#endif