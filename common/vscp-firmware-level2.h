/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2022 Ake Hedman, Grodans Paradis AB
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
 */

#ifndef VSCP_FIRMWARE_LEVEL2_H
#define VSCP_FIRMWARE_LEVEL2_H

/*
    vscp_compiler and project defines
*/
#include "vscp-compiler.h"
#include "vscp-projdefs.h"

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

#include <inttypes.h>
#include <vscp-projdefs.h>


/*!*****************************************************************************
                                    Constants
 ******************************************************************************/

// Protocol conformance version
#define VSCP2_MAJOR_VERSION      1       // VSCP Major version
#define VSCP2_MINOR_VERSION      6       // VSCP Minor Version
#define VSCP2_SUB_VERSION        1       // VSCP Sub version


#define VSCP2_STANDARD_REGISTER_START   0xffff0000


// -----------------------------------------------------------------------------
//  			     VSCP standard register - Logical positions
// -----------------------------------------------------------------------------


#define VSCP2_STD_REG_ALARMSTATUS                 0xFFFFFF80
#define VSCP2_STD_REG_VSCP_MAJOR_VERSION          0xFFFFFF81
#define VSCP2_STD_REG_VSCP_MINOR_VERSION          0xFFFFFF82

#define VSCP2_STD_REG_NODE_ERROR_COUNTER          0xFFFFFF83

#define VSCP2_STD_REG_USERID0                     0xFFFFFF84
#define VSCP2_STD_REG_USERID1                     0xFFFFFF85
#define VSCP2_STD_REG_USERID2                     0xFFFFFF86
#define VSCP2_STD_REG_USERID3                     0xFFFFFF87
#define VSCP2_STD_REG_USERID4                     0xFFFFFF88

#define VSCP2_STD_REG_MANUFACTURER_ID0            0xFFFFFF89
#define VSCP2_STD_REG_MANUFACTURER_ID1            0xFFFFFF8A
#define VSCP2_STD_REG_MANUFACTURER_ID2            0xFFFFFF8B
#define VSCP2_STD_REG_MANUFACTURER_ID3            0xFFFFFF8C

#define VSCP2_STD_REG_MANUFACTURER_SUBID0         0xFFFFFF8D
#define VSCP2_STD_REG_MANUFACTURER_SUBID1         0xFFFFFF8E
#define VSCP2_STD_REG_MANUFACTURER_SUBID2         0xFFFFFF8F
#define VSCP2_STD_REG_MANUFACTURER_SUBID3         0xFFFFFF90

#define VSCP2_STD_REG_NICKNAME_ID                 0xFFFFFF91

#define VSCP2_STD_REG_PAGE_SELECT_MSB             0xFFFFFF92
#define VSCP2_STD_REG_PAGE_SELECT_LSB             0xFFFFFF93

#define VSCP2_STD_REG_FIRMWARE_MAJOR_VERSION		  0xFFFFFF94
#define VSCP2_STD_REG_FIRMWARE_MINOR_VERSION		  0xFFFFFF95
#define VSCP2_STD_REG_FIRMWARE_RELEASE_VERSION	  0xFFFFFF96

#define VSCP2_STD_REG_BOOT_LOADER_ALGORITHM       0xFFFFFF97
#define VSCP2_STD_REG_BUFFER_SIZE                 0xFFFFFF98
#define VSCP2_STD_REG_PAGES_USED                  0xFFFFFF99

// 32-bit
#define VSCP2_STD_REG_STANDARD_DEVICE_FAMILY_CODE 0xFFFFFF9A

// 32-bit
#define VSCP2_STD_REG_STANDARD_DEVICE_TYPE_CODE   0xFFFFFF9E

#define VSCP2_STD_REG_DEFAULT_CONFIG_RESTORE      0xFFFFFFA2

// 16-bit
#define VSCP2_STD_REG_FIRMWARE_CODE               0xFFFFFFA3

#define VSCP2_STD_REG_GUID                        0xFFFFFFD0
#define VSCP2_STD_REG_DEVICE_URL                  0xFFFFFFE0

// -----------------------------------------------------------------------------

/* States */
#define VSCP2_STATE_STARTUP                       0x00
#define VSCP2_STATE_UNCONNECTED                   0x01
#define VSCP2_STATE_CONNECTED                     0x02
#define VSCP2_STATE_ERROR                         0x03

/*!****************************************************************************
                                   Globals
 ******************************************************************************/

extern uint8_t vscp_node_state;
extern uint8_t vscp_node_substate;

/*!****************************************************************************
                                   Prototypes
 ******************************************************************************/

/*!
 * \fn vscp2_init
 * @brief Init subsystem
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
 * @fn vscp2_do_periodic_work
 * @brief Do periodic VSCP protocol work
 *
 * @param pdata Pointer to user data (typical points to context)
 * @param pev Pointer to event to handle. NULL if no event to handle.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Do periodic VSCP protocol work. This function should be called from the
 * main loop on periodic intervals.
 *
 * If there is incoming events to handle a pointer to the event should be
 * sent as a parameter  in **pev**. * Once the function has bern called
 * ownership of the event is left over to the protocol stack and it is up to the
 * function to release the event. This is true also if the function returns an error.
 *
 * If there is no event to handle for the protocol stack pev should be set to NULL. 
 *
 */

int 
vscp2_do_work(vscpEvent* pev);

/*!
 * \fn vscp2_readRegister
 * @brief Read register
 * @param reg Register to write
 * @param pval Pointer to read value
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 * 
 * This function is called from the main loop.
 */

int
vscp2_read_reg(uint32_t reg, uint8_t* pval);

/*!
 * \fn vscp2_writeRegister
 * @brief Write register
 * @param reg Register to write
 * @param val Value to write
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 * 
 * This function is called from the main loop.
 */

int
vscp2_write_reg(uint32_t reg, uint8_t val);

/*!
 * \fn vscp2_send_heartbeat
 * @brief Send periodic heartbeat
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 * 
 * This function is called from the main loop.
 */

int
vscp2_send_heartbeat(void);

/*!
 * \fn vscp2_send_caps
 * @brief Send periodic capability heartbeat
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 * 
 * This function is called from the main loop.
 */

int
vscp2_send_caps(void);

/*!
 * \fn vscp2_send_high_end_server_probe
 * @brief Send high end server probe
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 * 
 * This function is called from the main loop.
 */

#ifdef THIS_FIRMWARE_VSCP_DISCOVER_SERVER
int
vscp2_send_high_end_server_probe(void);
#endif

/*!
  @fn vscp2_do_one_second_work
  @brief Do One second work
  @return VSCP_ERROR_SUCCESS on success, else error code.

  This routine should be called once a second by the
  application.
 */

int
vscp2_do_one_second_work(void);

/*!
 * \fn vscp2_do_register_read
 * @brief Do register reads
 * @param pev Pointer to event containing register read data
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int 
vscp2_do_register_read(vscpEvent* pev);

/*!
 * \fn vscp2_do_register_write
 * @brief Do register writes
 * @param pev Pointer to event containing register write data
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int 
vscp2_do_register_write(vscpEvent* pev);

/*!
  @fn vscp2_send_error_event
  @brief Send error event (CLASS=508).
  
  @param type This is the VSCP type
  @param idx Index to identify possible sub module. Normally set to zero.
  @return VSCP_ERROR_SUCCESS on success, else error code.

  http://www.vscp.org/docs/vscpspec/doku.php?id=class1.error
  idx can be used to identify the internal part ("submodule") that was the
  origin of the error. Both zone and sub zone are always set to zero.
 */
#ifdef THIS_FIRMWARE_ENABLE_ERROR_REPORTING
uint8_t
vscp2_send_error_event(uint8_t type, uint8_t idx);
#endif

/*!
  @fn vscp2_send_log_event
  @brief Send log event (CLASS=509).
  
  @param type VSCP logevent type.
  @param id Identifier for the logging channel.
  @param level Loglevel for this log event.
  @param idx index for multiframe log event starting at zero.
  @param pData Log data (Allways 5 bytes).
  @return VSCP_ERROR_SUCCESS on success, else error code.

  http://www.vscp.org/docs/vscpspec/doku.php?id=class1.log
  For loging first send Type = 2(0x01) Log Start then logging events and when
  log is closed send Type = 3 (0x03) Log Stop. To log several things use a
  unique if for each and open/close each.
 */
#ifdef THIS_FIRMWARE_ENABLE_LOGGING
uint8_t
vscp2_send_log_event(uint8_t type, uint8_t id, uint8_t level, uint8_t idx, uint8_t* pLogdata);
#endif

/*!
 * @fn vscp2_send_high_end_server_probe
 * @brief Send high end server probe
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 * 
 * Inform possible high end servers of our existence.
 */
#ifdef THIS_FIRMWARE_VSCP_DISCOVER_SERVER
int vscp2_send_high_end_server_probe(void);
#endif

/*
 ****************************************************************************
                                    Callbacks
 *****************************************************************************
 */

/*!
 * \fn vscp2_callback_get_ms
 * @brief Get the time in milliseconds.
 * @param pdata Pointer to user data (typical points to context)
 * @param ptime Pointer to unsigned integer that will get the time in milliseconds.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */
int vscp2_callback_get_ms(const void* pdata, uint32_t *ptime);


/*!
 * \fn vscp2_callback_get_guid
 * @brief Get pointer to GUID
 * @param pdata Pointer to user data (typical points to context)
 * @return Pointer to the device GUID.
 * 
 */

const uint8_t* 
vscp2_callback_get_guid(const void* pdata);

/*!
 * \fn vscp2_callback_read_user_reg
 * @brief Read user register
 * @param pdata Pointer to user data (typical points to context)
 * @param reg Register to read
 * @param pval Pointer to variable that will get read value
 * @return VSCP_ERROR_SUCCESS on success, else error code. VSCP_ERROR_PARAMETER is
 * returned if the register is invalid.
 * 
 */
int
vscp2_callback_read_user_reg(const void* pdata, uint32_t reg, uint8_t* pval);

/*!
 * \fn vscp2_callback_write_user_reg
 * @brief Write user register
 * @param pdata Pointer to user data (typical points to context)
 * @param reg Register to write
 * @param val Value to write
 * @return VSCP_ERROR_SUCCESS on success, else error code. VSCP_ERROR_PARAMETER is
 * returned if the register is invalid.
 */

int
vscp2_callback_write_user_reg(const void* pdata, uint32_t reg, uint8_t val);

/*!
  Send event to transport sublayer.
  @param pdata Pointer to user data (typical points to context)
  @param pev Pointer to Event to send.
  @return VSCP_ERROR_SUCCESS on success, or error code.

  - ev is taken over in the callback and it is responsible for releasing it.
  - if GUID is all zero then the GUID is set to the GUID of the node.
*/

int
vscp2_callback_send_event(const void* pdata, vscpEvent* pev);

/*!
  Send eventex to transport sublayer.
  @param pdata Pointer to user data (typical points to context)
  @param ex Pointer to EventEx to send.
  @return VSCP_ERROR_SUCCESS on success, or error code.

  - ex is copied in the callback.
  - if GUID is all zero then the GUID is set to the GUID of the node.
*/

int
vscp2_callback_send_eventex(const void* pdata, vscpEventEx* ex);

/*!
 * \fn vscp2_callback_bootloader
 * @brief Enter bootloader.
 * @param pdata Pointer to user data (typical points to context).
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 */

int
vscp2_callback_enter_bootloader(const void* pdata);

/*!
 * \fn vscp2_callback_report_dmatrix
 * @brief Reply with DM content.
 * @param pdata Pointer to user data (typical points to context).
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 * 
 * Report full content of DM back.
 */

int
vscp2_callback_report_dmatrix(const void* pdata);

/*!
 * \fn vscp2_callback_report_mdf
 * @brief Reply with embedded DM.
 * @param pdata Pointer to user data (typical points to context).
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 * 
 * Report embedded mdf back.
 */

int vscp2_callback_report_mdf(const void* pdata);

/*!
 * @brief Report back events that this node is interested in
 * @param pdata Pointer to user data (typical points to context)
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp2_callback_report_events_of_interest(const void* pdata);


/*!
 * @brief Get timestamp
 * @param pdata Pointer to context.
 * @return Timestamp in microseconds.
 */
 
uint32_t 
vscp2_callback_get_timestamp(const void* pdata);

/*!
 * @brief  Fill in event time information
 * 
 * @param pdata 
 * @param pev Event to get info
 *
 * Set all to zero or do nothing if not time information is available
 * and the time values are already set to zero.
 */

int
vscp2_callback_get_time(const void* pdata, const vscpEvent *pev);

/*!
  @brief Get user id
  @param pdata Pointer to context
  @param pos Position 0-4 of user id to read
  @param pval Pointer to variable that will get user id
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_callback_get_user_id(const void *pdata, uint8_t pos, uint8_t *pval);

/*!
  @brief Write user id
  @param pdata Pointer to context
  @param pos Position 0-8 of manufacturer id to write
  @param val Value to write
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_callback_write_user_id(const void *pdata, uint8_t pos, uint8_t val);

#ifdef THIS_FIRMWARE_ENABLE_WRITE_2PROTECTED_LOCATIONS

/*!
  Get manufacturer id
  @param pdata Pointer to context
  @param pos Position 0-7 of manufacturer id to read
  @param pval Pointer to variable that will get manufacturer id
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_callback_get_manufacturer_id(const void *pdata, uint8_t pos, uint8_t *pval);


/*!
  Write manufacturer id
  @param pdata Pointer to context
  @param pos Position 0-7 of user id to write
  @param val Value to write
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_callback_write_manufacturer_id(const void *pdata, uint8_t pos, uint8_t val);

/*!
  Get GUID bye
  @param pdata Pointer to context
  @param pos Position 0-15 of GUID byte to read
  @param pval Pointer to variable that will get GUID byte
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_callback_get_guid(const void *pdata, uint8_t pos, uint8_t *pval);

/*!
  Write guid byte
  @param pdata Pointer to context
  @param pos Position 0-15 of GUID byte to write
  @param val Value to write
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp2_callback_write_guid(const void *pdata, uint8_t pos, uint8_t val);

#endif

/*!
  Get bootloader algorithm used to bootload this
  device.
  @return bootloader algorithm (see vscp.h)
*/

int
vscp2_callback_get_bootloader_algorithm(const void *pdata);



int
vscp2_callback_get_control_byte(const void *pdata);

int
vscp2_callback_set_control_byte(const void *pdata, uint8_t ctrl);


/*!
  Initialize persistent storage
 */
int
vscp2_callback_init_persistent_storage(const void *pdata);

/*!
  Fedd the decision matrix with one Event
  @param ev Event to feed the DM with.
*/
int
vscp2_callback_feed_dm(const void *pdata, vscpEvent* ev);


/*!
  All events except level I/II protocol events is sent to the
  application for handling.
  @param ev Event to feed the DM with.
*/
int
vscp2_callback_feed_app(const void *pdata, vscpEvent* ev);

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
int
vscp2_callback_send_dm_info(const void *pdata, char* pData);

/*!
  Get embedded MDF info
  If available this routine sends an embedded MDF file
  in several events. See specification CLASS1.PROTOCOL
  Type=35/36
 */
int
vscp2_callback_send_embedded_mdf(const void *pdata);

/*!
  Go boot loader mode
  This routine force the system into boot loader mode according
  to the selected protocol.
 */
int
vscp2_callback_go_bootloader(const void *pdata, uint8_t algorithm);

/*!
    Get Zone for device
    Just return zero if not used.
 */
int
vscp2_callback_get_zone(const void *pdata);

/*!
    Get Subzone for device
    Just return zero if not used.
 */
int
vscp2_callback_get_subzone(const void *pdata);

/*!
    Restore defaults
    If 0x55/0xaa is written to register location
    162 within one second defaults should be loaded
    by the device.
 */
int
vscp2_callback_restore_defaults(const void *pdata);



// ============ END OF CALLBACKS ============

/**
@}
*/

#ifdef __cplusplus
}
#endif

#endif