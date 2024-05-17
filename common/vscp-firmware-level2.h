/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2024 Ake Hedman, Grodans Paradis AB
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

/*!
  @file vscp_firmware_level2.h
  @defgroup vscp-level2-firmware VSCP level II firmware

  Projdefs defines project specific definitions and
  define the following variables.



  @{
*/

#ifndef __VSCP_FIRMWARE_LEVEL2_H
#define __VSCP_FIRMWARE_LEVEL2_H

#ifdef __cplusplus
extern "C" {
#endif

/*
    vscp_compiler and project defines
*/
#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include <inttypes.h>
#include <vscp-projdefs.h>

/*!
  Global structure for protocol functionality

   Used internally
*/
typedef struct frmw2_firmware_configt_t {
  int m_state;             // State machine state
  int m_substate;          // state machine substate
  uint32_t m_config_timer; // Timer used for config restore

  uint8_t m_level;   // 0=Level I, 1 = Level II
  void* m_puserdata; // Points to user supplied data

  /*!
    Holders for proxy event information. Set when received.
  */
  uint8_t m_offset;
  uint8_t m_ifguid[16];

  uint8_t m_interval_heartbeat; // Interval for hearytbeats in seconds (0=off)
  uint32_t m_last_heartbeat;    // Time for last heartbeat send
  uint8_t m_interval_caps;      // Interval for capabilities events in secons (0=off)
  uint32_t m_last_caps;         // Time for last caps send

  // Functionality switches
  int m_bEnableErrorReporting;
  int m_bEnableLogging;
  int m_bSendHighEndServerProbe;
  int m_bHighEndServerResponse;
  int m_bEnableWriteProtectedLocations; // GUID/manufacturer id

  // Standard registers (persistent storage)
  uint8_t m_alarm_status;                   // Alarm. Read only for clients.
  uint8_t m_vscp_major_version;             // VSCP protocol major version.
  uint8_t m_vscp_minor_version;             // VSCP protocol minor version.
  uint8_t m_errorCounter;                   // Error counter. Clear on read. Read only for clients.
  uint8_t m_userId[4];                      // User id.
  uint8_t m_manufacturerId[4];              // Manufacturer id.Read only for clients.
  uint8_t m_manufacturerSubId[4];           // Manufacturer sub id.Read only for clients.
  uint16_t m_nickname;                      // Device nickname
  uint16_t m_page_select;                   // Page select register.
  uint8_t m_firmware_major_version;         // This software version. Read only for clients.
  uint8_t m_firmware_minor_version;         // This software version. Read only for clients.
  uint8_t m_firmware_sub_minor_version;     // This software version. Read only for clients.
  uint8_t m_bootloader_algorithm;           // Boot oader algorithm we use.
  uint8_t m_standard_device_family_code[4]; // Family code. Read only for clients.
  uint8_t m_standard_device_type_code[4];   // Family type. Read only for clients.
  uint8_t m_firmware_device_code;           // Identifier for hardware so correct firmware can be loaded
  uint8_t m_guid[16];                       // GUID for device. Read only for clients
  uint8_t m_mdfurl[32];                     // URL for MDF. Read only for clients. Make 33 bytes and nill
                                            // all unused byte so it is null terminated even if it has
                                            // the maximum length of 32 bytes.

} frmw2_firmware_configt_t;

// /*!
//   Protocol configuration.

//   This structure is used by client to initiate the
//   protocol framework and functionality.
// */
// typedef struct frmw2_firmware_config_t {

//   uint8_t m_level;   // 0=Level I, 1 = Level II
//   void* m_puserdata; // Points to user supplied data
//                      // This pointer follows all callback code
//   // uint8_t m_errorcnt; // Counter for errors

//   uint8_t m_interval_heartbeat; // Interval for hearytbeats in seconds (0=off)
//   uint8_t m_interval_caps;      // Interval for capabilities events in secons (0=off)

//   int m_bEnableErrorReporting;   // Enable error reporting
//   int m_bEnableLogging;          // Enable logging
//   int m_bSendHighEndServerProbe; // enable server probe
//   int m_bHighEndServerResponse;
//   int m_bEnableWriteProtectedLocations; // GUID/manufacturer id

//   // Standard registers (persistent storage)
//   uint8_t m_vscp_major_version;             // VSCP protocol major version.
//   uint8_t m_vscp_minor_version;             // VSCP protocol minor version.
//   uint8_t m_userId[4];                      // User id.
//   uint8_t m_manufacturerId[4];              // Manufacturer id.Read only for clients.
//   uint8_t m_manufacturerSubId[4];           // Manufacturer sub id.Read only for clients.
//   uint16_t m_nickname;                      // Device nickname
//   uint8_t m_firmware_major_version;         // This software version. Read only for clients.
//   uint8_t m_firmware_minor_version;         // This software version. Read only for clients.
//   uint8_t m_firmware_sub_minor_version;     // This software version. Read only for clients.
//   uint8_t m_bootloader_algorithm;           // Boot oader algorithm we use.
//   uint8_t m_standard_device_family_code[4]; // Family code. Read only for clients.
//   uint8_t m_standard_device_type_code[4];   // Family type. Read only for clients.
//   uint8_t m_guid[16];                       // GUID for device. Read only for clients
//   uint8_t m_mdfurl[32];                     // URL for MDF. Read only for clients

// } frmw2_firmware_config_t;

/*!
  Used to adjust standard registers for Level I/ Level II
  Level I standard registers are at 0x80 - 0xff on page = 0
  Level II standard registers are at 0xffffff00 - 0xffffffff
*/
#define ADJSTDREG ((VSCP_LEVEL1 == g_pconfig->m_level) ? 0 : 0xffffff00)

/*!
  Adjusted data for offset
  vscpEvent and vscpEventEx versions
*/
#define EVDTA(x) (pev->data[(x) + g_pconfig->m_offset])
#define EXDTA(x) (pex->data[(x) + g_pconfig->m_offset])

/*!
 * Ajust size for offset
 * vscpEvent and vscpEventEx versions
 */
#define ADJSIZEV (pex->sizeData - g_pconfig->m_offset)
#define ADJSIZEX (pex->sizeData - g_pconfig->m_offset)

/*****************************************************************************
                                    Constants
 *****************************************************************************/

/*!
  @brief Major version.

  This is the major version of this vscp  level 2 protocol code..
*/
#define FRMW2_MAJOR_VERSION 1

/*!
  @brief Minor Version-

  This is the minor version of this vscp  level 2 protocol code.
*/
#define FRMW2_MINOR_VERSION 6

/*!
  @brief Standard register address start.

  VSCP level II protocol standard register abstraction start address.
*/
// #define FRMW2_STANDARD_REGISTER_START 0xffff0000

// -----------------------------------------------------------------------------

/* States */
#define FRMW2_STATE_STARTUP     0x00
#define FRMW2_STATE_UNCONNECTED 0x01
#define FRMW2_STATE_CONNECTED   0x02
#define FRMW2_STATE_ERROR       0x03

/*!
****************************************************************************
                                   Prototypes
*****************************************************************************
*/

/*!
 * @brief Init subsystem
 *
 * @param pcfg Structure with configuration data
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Init VSCP subsystem. The transport layer should be up and
 * running as a New node on line is sent here when everything is
 * initialized.
 */

int
frmw2_init(frmw2_firmware_configt_t* const pcfg);

/*!
  @brief Initialize persistent storage

  @param pdata Pointer to user data (typical points to context)
 */
int
frmw2_init_persistent_storage(void);

/*!
 * @brief Init VSCP ex event with standard data
 *
 * @param pex Pointer to ex event
 */

void
frmw2_init_event_ex(vscpEventEx* pex);

/*!
 * @brief Get nickname for node
 *
 * @return nickname
 */

uint16_t
frmw2_get_nickname(void);

/*!
 * @brief Do periodic VSCP protocol work when nickname is 0xffff
 *
 * This is the code that is executed during
 * the init process when a nickjname device
 * has not yet got a valid nickname.
 *
 * Never used in a LEVEL II system
 *
 * */

int
frmw2_do_nickname_discovery_work(vscpEventEx* pex);

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
frmw2_do_live_work(vscpEventEx* pex);

/*!
 * @brief Read VSCP Level II register
 *
 * If the device is a pure level I device
 * the three high bytes of reg should be masked
 * before a register read.
 *
 * @param reg Register to write
 * @param pval Pointer to read value
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
frmw2_read_reg(uint32_t reg, uint8_t* pval);

/*!
 * @brief Write VSCP level II register
 *
 * If the device is a pure level I device
 * the three high bytes of reg should be masked
 * before a register write.
 *
 * @param reg Register to write
 * @param val Value to write
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Write a value to a standard or a user register.
 */

int
frmw2_write_reg(uint32_t reg, uint8_t val);

/*!
 * @brief Send periodic heartbeat
 *
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Send heartbeat event to client. Typically send one every minute.
 */

int
frmw2_send_heartbeat(void);

/*!
 * @brief Send periodic capability heartbeat
 *
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Send capabilities event to client. Typically send every minute.
 */

int
frmw2_send_caps(void);

/*!
 * @brief Do register reads
 *
 * @param reg First register position to write to.
 * @param cnt Number of positions to write to.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
frmw2_do_vscp2_register_read(uint32_t startreg, uint16_t cnt);

/*!
 * @brief Do register writes
 *
 * @param reg First register position to write to.
 * @param cnt Number of positions to write to.
 * @param pdata Pointer to data to write.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
frmw2_do_vscp2_register_write(uint32_t startreg, uint16_t cnt, uint8_t* pdata);

/*!
  @brief Send error event (CLASS=508).

  @param type This is the VSCP type
  @param idx Index to identify possible sub module. Normally set to zero.
  @return VSCP_ERROR_SUCCESS on success, else error code.

  Enable by setting bEnableErroReporting to true in config.

  See specification [here](https://grodansparadis.github.io/vscp-doc-spec/#/./class1.error)

  idx can be used to identify the internal part ("submodule") that was the
  origin of the error. Both zone and sub zone are always set to zero.
 */

uint8_t
frmw2_send_error_event(uint8_t type, uint8_t idx);

/*!
  @brief Send log event (CLASS=509).

  @param type VSCP logevent type.
  @param id Identifier for the logging channel.
  @param level Loglevel for this log event.
  @param idx index for multiframe log event starting at zero.
  @param pData Log data (Always 5 bytes).
  @return VSCP_ERROR_SUCCESS on success, else error code.

  Enable by setting bEnableLogging to true in config.

  See specification [here](https://grodansparadis.github.io/vscp-doc-spec/#/./class1.log)

  For logging first send Type = 2(0x01) Log Start then logging events and when
  log is closed send Type = 3 (0x03) Log Stop. To log several things use a
  unique if for each and open/close each.
 */

uint8_t
frmw2_send_log_event(uint8_t type, uint8_t id, uint8_t level, uint8_t idx, uint8_t* pLogdata);

/*!
   @brief Send high end server probe

  Enable by setting bSendHighEndServerProbe to true in config

   @return VSCP_ERROR_SUCCESS on success, else error code.

   Inform possible high end servers of our existence.
 */

int
frmw2_send_high_end_server_probe(void);

/*!
  @brief Do a level I page read

  @param offset Offset into register page
  @param count Number of registers to read
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
frmw2_page_read(uint32_t offset, uint8_t count);

/*!
  @brief Do a level I page write

  @param offset Offset into register page
  @param count Number of registers to write
  @param buf Pointer to buffer of
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
frmw2_page_write(uint32_t offset, uint8_t count, const uint8_t* const pbuf);

/*!
  @brief Send whois response

  Send whois response. If level II device a level II response
  will be sent.

  @param nodeid Offset into register page
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
frmw2_whois_response(uint16_t nodeid);

/*!
  @brief Read a full page or part of memory

  @param nodeid Node id from node to read page from
  @param page Page to read.
  @param offset Offset on page to read from.
  @param cnt Number of bytes from offset to read. A zero value should be treated as 256.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
frmw2_extended_page_read(uint16_t nodeid, uint16_t page, uint8_t offset, uint16_t cnt);

/*!
  @brief Write multiple bytes to memory

  @param page Page to read.
  @param offset Offset on page to read from.
  @param cnt Number of bytes from offset to read. A zero value should be treated as 256.
  @param pbuf Pointer to buffer with data
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
frmw2_extended_page_write(uint16_t nodeid, uint16_t page, uint8_t offset, uint16_t cnt, const uint8_t* const pbuf);

/*
 ****************************************************************************
                                    Callbacks
 *****************************************************************************
 */

/**
  @name Callbacks for VSCP level II protocol
*/

/*!
 * @brief Check if this is a level I device
 *
 * @param pdata Pointer to user data (typical points to context)
 * @return Non zero is returned if this is a level I device.
 */

int
frmw2_callback_is_level1(void* const puserdata);

/*!
 * @brief Get the time in milliseconds.
 *
 * @param pdata Pointer to user data (typical points to context)
 * @param ptime Pointer to unsigned integer that will get the time in milliseconds.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
frmw2_callback_get_ms(void* const puserdata, uint32_t* ptime);

/*!
 * @brief Enter bootloader.  SHOULD NEVER RETURN
 *
 * @param pdata Pointer to user data (typical points to context).
 */

void
frmw2_callback_enter_bootloader(void* const puserdata);

/*!
 * @brief Reply with DM content.
 *
 * @param pdata Pointer to user data (typical points to context).
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 *
 * Report full content of DM back.
 */

int
frmw2_callback_report_dmatrix(void* const puserdata);

/*!
 * @brief Segment controler heartbeat received
 *
 * @param pdata Pointer to user data (typical points to context).
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 */
int
frmw2_callback_segment_ctrl_heartbeat(void* const puserdata, uint16_t segcrc, uint32_t time);

/*!
 * @brief New level I node online
 *
 * @param pdata Pointer to user data (typical points to context).
 * @param nickname nickname id of the new node.
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 */

int
frmw2_callback_new_node_online_level1(void* const puserdata, uint16_t nickname);

/*!
 * @brief New leve II node online
 *
 * The framework sends the required response if the nodeid is the same as the preobe.
 *
 * @param pdata Pointer to user data (typical points to context).
 * @param pguid Pointer to GUID of the new node.
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 */
int
frmw2_callback_new_node_online_level2(void* const puserdata, const uint8_t* const pguid);

/*!
 * @brief Report back events that this node is interested in
 *
 * The framework sends the required response if the GUID is the same as the probe.
 *
 * @param pdata Pointer to user data (typical points to context)
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
frmw2_callback_report_events_of_interest(void* const puserdata);

/*!
 * @brief Get timestamp
 *
 * @param pdata Pointer to context.
 * @return Timestamp in microseconds.
 */

uint32_t
frmw2_callback_get_timestamp(void* const puserdata);

/*!
 * @brief  Fill in event date/time information
 *
 * @param pdata
 * @param pex Event to get info
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Set all to zero or do nothing if not time information is available
 * and the time values are already set to zero.
 */

int
frmw2_callback_get_time(void* const puserdata, vscpEventEx* pex);

/*!
  @brief Feed the decision matrix with one Event

  @param pdata Pointer to user data (typical points to context)
  @param pex Event to feed the DM with.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
frmw2_callback_feed_dm(void* const puserdata, vscpEventEx* pev);

/*!
  @brief All events except level I/II protocol events is sent to the
  application for handling.

  @param pdata Pointer to user data (typical points to context)
  @param pex Event to feed the DM with.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
frmw2_callback_feed_app(void* const puserdata, vscpEventEx* pex);

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
frmw2_callback_send_dm_info(void* const puserdata, char* pDM);

/*!
  @brief Get MDF URL

  @param pdata Pointer to user data (typical points to context)
  @param purl Pointer to MDF URL.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
frmw2_callback_get_mdf_url(void* const puserdata, uint8_t* const purl);

/*!
  @brief Get embedded MDF info

  If available this routine sends an embedded MDF file
  in several events. See specification CLASS1.PROTOCOL
  Type=35/36

  @param pdata Pointer to user data (typical points to context)
  @return VSCP_ERROR_SUCCESS on success, or error code.
 */
int
frmw2_callback_send_embedded_mdf(void* const puserdata);

/*!
  @brief Restore defaults

  If 0x55/0xaa is written to register location
  162 within one second defaults should be loaded
  by the device.

  @param pdata Pointer to user data (typical points to context)
  @return VSCP_ERROR_SUCCESS on success, or error code.
 */

int
frmw2_callback_restore_defaults(void* const puserdata);

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
frmw2_callback_get_ip_addr(void* const puserdata, uint8_t* pipaddr);

/*!
  @brief Handle high end server response

  @param pdata Pointer to user data (typical points to context)
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
frmw2_callback_high_end_server_response(void* const puserdata);

/*!
  @brief Get name of the device.  Used by the device capabilities report.

  @param pdata Pointer to user data (typical points to context)
  @param pname Pointer to device name. The name should be no more than 64
  characters including terminating zero.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
frmw2_callback_get_device_name(void* const puserdata, const char* pname);

/*!
 * @brief Read register
 *
 * Read value of one of the user registers. Valid register range
 * is 0x00000000 - 0xffff0000
 *
 * @param pdata Pointer to user data (typical points to context).
 * @param page Register page used at level I
 * @param reg Register to read.
 * @param pval Pointer to variable that will get read value.
 * @return VSCP_ERROR_SUCCESS on success, else error code. VSCP_ERROR_PARAMETER is
 * returned if the register is invalid.
 *
 */
int
frmw2_callback_read_reg(void* const puserdata, uint16_t page, uint32_t reg, uint8_t* pval);

/*!
 * @brief Write register
 *
 * Write value of one of the user registers. Valid register range
 * is 0x00000000 - 0xffff0000
 *
 * @param pdata Pointer to user data (typical points to context)
 * @param page Register page used at level I
 * @param reg Register to write
 * @param val Value to write
 * @return VSCP_ERROR_SUCCESS on success, else error code. VSCP_ERROR_PARAMETER is
 * returned if the register is invalid.
 */

int
frmw2_callback_write_reg(void* const puserdata, uint16_t page, uint32_t reg, uint8_t val);

/*!
  @brief Send event to transport sublayer.

    ev is copied in the callback.
  - if GUID is all zero then the GUID should be set to the GUID of the node.
  - if requestted to send a Level II event and we are a Level I node skip the
    event and return VSCP_ERROR_ERROR

  @param pdata Pointer to user data (typical points to context)
  @param pex Pointer to Event to send.
  @return VSCP_ERROR_SUCCESS on success, or error code.

*/

int
frmw2_callback_send_event(void* const puserdata, vscpEvent* pev);

/*!
  @brief Send eventex to transport sublayer.
  - ex is copied in the callback.
  - if GUID is all zero then the GUID should be set to the GUID of the node.
  - if requestted to send a Level II event and we are a Level I node skip the
    event and return VSCP_ERROR_ERROR
  @param pdata Pointer to user data (typical points to context)
  @param pex Pointer to EventEx to send.
  @return VSCP_ERROR_SUCCESS on success, or error code.

  -
*/

int
frmw2_callback_send_event_ex(void* const puserdata, vscpEventEx* pex);

/*!
  @brief Information callback to app that standard register has been changed.

  @param pdata Pointer to user data (typical points to context)
  @param stdreg Standard register that has been changed. Level I register address
    should be used for all common registers.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
frmw2_callback_stdreg_change(void* const puserdata, uint32_t stdreg);

/*!
  @brief Feed the watchdog

  Feed the watchdog on systems that need to do
  that during lengthy operations to prevent restarts.
*/

void
frmw2_callback_feed_watchdog(void* const puserdata);

/*!
  @brief Reset the device and do a warm start
*/
void
frmw2_callback_reset(void* const puserdata);

// ============ END OF CALLBACKS ============

/**
@}
*/

#ifdef __cplusplus
}
#endif

#endif // __VSCP_FIRMWARE_LEVEL2_H