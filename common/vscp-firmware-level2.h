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

/*****************************************************************************
                                    Constants
 *****************************************************************************/

/*!
  Used to mark config items that should be
  initialized to default values
*/
#define VSCP_FRMW2_UNASSIGNED -1

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
typedef enum probe_state_t {
  FRMW2_STATE_NONE,      // No state
  FRMW2_STATE_PROBE,     // A node that is searching for a nickname
  FRMW2_STATE_PREACTIVE, // Happens when segment controller take probe. Wait for nickname
  FRMW2_STATE_ACTIVE,    // Node that in it's working state
  FRMW2_STATE_ERROR,     // This node has failed (probing). Waiting for a reset
} probe_state_t;

/* Sub states */
typedef enum probe_substate_t {
  FRMW2_SUBSTATE_NONE,
  FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING, // Probe sent for segment controller
  FRMW2_SUBSTATE_PROBE_WAITING,         // Waiting for response after probe has been sent
} probe_substate_t;

// Defines for probe message
#define OPT_NEW_NODE_ONLINE 1
#define OPT_PROBE           0

// Level I decision matrix

#define VSCP_LEVEL1_DM_POS_OADDR       0
#define VSCP_LEVEL1_DM_POS_FLAGS       1
#define VSCP_LEVEL1_DM_POS_CLASSMASK   2
#define VSCP_LEVEL1_DM_POS_CLASSFILTER 3
#define VSCP_LEVEL1_DM_POS_TYPEMASK    4
#define VSCP_LEVEL1_DM_POS_TYPEFILTER  5
#define VSCP_LEVEL1_DM_POS_ACTION      6
#define VSCP_LEVEL1_DM_POS_ACTIONPARAM 7

#define VSCP_LEVEL1_DM_FLAG_ENABLED       0x80
#define VSCP_LEVEL1_DM_FLAG_CHECK_OADDR   0x40
#define VSCP_LEVEL1_DM_FLAG_HARDCODED     0x20
#define VSCP_LEVEL1_DM_FLAG_CHECK_ZONE    0x10
#define VSCP_LEVEL1_DM_FLAG_CHECK_SUBZONE 0x08
#define VSCP_LEVEL1_DM_FLAG_CLASS_MASK    0x02
#define VSCP_LEVEL1_DM_FLAG_CLASS_FILTER  0x01

#define LEVEL1_DM_OADDR(row)        (g_pconfig->m_pDm[(8 * row) + VSCP_LEVEL1_DM_POS_OADDR])
#define LEVEL1_DM_FLAGS(row)        (g_pconfig->m_pDm[(8 * row) + VSCP_LEVEL2_DM_POS_FLAGS])
#define LEVEL1_DM_CLASS_MASK(row)   (g_pconfig->m_pDm[(8 * row) + VSCP_LEVEL1_DM_POS_CLASSMASK] + (((g_pconfig->m_pDm[8 * row + VSCP_LEVEL1_DM_POS_FLAGS]) & 0x02) << 9))
#define LEVEL1_DM_CLASS_FILTER(row) (g_pconfig->m_pDm[(8 * row) + VSCP_LEVEL1_DM_POS_CLASSFILTER] + (((g_pconfig->m_pDm[8 * row + VSCP_LEVEL1_DM_POS_FLAGS]) & 0x01) << 9))
#define LEVEL1_DM_TYPE_MASK(row)    (g_pconfig->m_pDm[(8 * row) + VSCP_LEVEL1_DM_POS_TYPEMASK])
#define LEVEL1_DM_TYPE_FILTER(row)  (g_pconfig->m_pDm[(8 * row) + VSCP_LEVEL1_DM_POS_TYPEFILTER])
#define LEVEL1_DM_ACTION(row)       (g_pconfig->m_pDm[(8 * row) + VSCP_LEVEL1_DM_POS_ACTION])
#define LEVEL1_DM_ACTION_PARAM(row) (g_pconfig->m_pDm[(8 * row) + VSCP_LEVEL1_DM_POS_ACTIONPARAM])

// Level II decision matrix

#define VSCP_LEVEL2_DM_POS_FLAGS       0
#define VSCP_LEVEL2_DM_POS_CLASS       4
#define VSCP_LEVEL2_DM_POS_TYPE        6
#define VSCP_LEVEL2_DM_POS_OADDR       8
#define VSCP_LEVEL2_DM_POS_ACTION      24
#define VSCP_LEVEL2_DM_POS_ACTIONPARAM 26

#define VSCP_LEVEL2_DM_FLAG_ENABLED       0x80000000
#define VSCP_LEVEL2_DM_FLAG_CHECK_OADDR   0x40000000
#define VSCP_LEVEL2_DM_FLAG_END_MARK      0x20000000
#define VSCP_LEVEL2_DM_FLAG_CHECK_INDEX   0x00000020 // index in first data byte
#define VSCP_LEVEL2_DM_FLAG_CHECK_ZONE    0x00000010 // zone in second data byte
#define VSCP_LEVEL2_DM_FLAG_CHECK_SUBZONE 0x00000008 // subzone in third data byte

#define LEVEL2_DM_FLAGS(row) construct_unsigned32(                                 \
  g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + VSCP_LEVEL1_DM_POS_FLAGS],     \
  g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + VSCP_LEVEL1_DM_POS_FLAGS + 1], \
  g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + VSCP_LEVEL1_DM_POS_FLAGS + 2], \
  g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + VSCP_LEVEL1_DM_POS_FLAGS + 3])
#define LEVEL2_DM_OADDR(row, pos) (g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + \
                                                    VSCP_LEVEL2_DM_POS_OADDR + pos])
#define LEVEL2_DM_CLASS(row) construct_unsigned16(                             \
  g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + VSCP_LEVEL2_DM_POS_CLASS], \
  g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + VSCP_LEVEL2_DM_POS_CLASS + 1])
#define LEVEL2_DM_TYPE(row) construct_unsigned16(                             \
  g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + VSCP_LEVEL2_DM_POS_TYPE], \
  g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + VSCP_LEVEL2_DM_POS_TYPE + 1])
#define LEVEL2_DM_ACTION(row) construct_unsigned16(                             \
  g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + VSCP_LEVEL2_DM_POS_ACTION], \
  g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + VSCP_LEVEL2_DM_POS_ACTION + 1])
#define LEVEL2_DM_ACTION_PARAM(row) (g_pconfig->m_pDm[(g_pconfig->m_sizeDmRow * row) + \
                                                      VSCP_LEVEL2_DM_POS_ACTIONPARAM])

/*
  Default nickname discovery settings
  - Probe timeout is the time to wait for a response from another node
  - Probe timeout count is the number of times a probe is sent when no
      response is received from node when probing with a specific nickname.
      If no response is received after this amount of tries the the nickname
      will be assigned to the node.
*/
#define VSCP_PROBE_TIMEOUT 1000              // ms - Time after probe has been sent and a no response
                                             // is considered as a free nickname discovery
#define VSCP_PROBE_TIMEOUT_COUNT   3         // # probe timed out probe events before assigning a nickname
#define VSCP_SEGCTRL_PROBE_TIMEOUT 5000      // ms time to wait for segment controller to assign nickname
                                             // to us
#define VSCP_SEGCTRL_RESPONSE_TIMEOUT 300000 // ms time after a segment ctrl response has been received
                                             // and when nickname discovery process will be taken up again.
#define VSCP_INIT_BUTTON_TIMEOUT 2000        // Press timeout for init button.
#define VSCP_GUID_RESET_TIMEOUT  1000        // All four GUID reset event must be received withing this time

/*!
  Global structure for protocol functionality

   Used internally
*/
typedef struct vscp_frmw2_firmware_config {

  uint8_t m_level;   // 0=Level I, 1 = Level II
  void* m_puserdata; // Points to user supplied data

  probe_state_t m_state;       // State machine state
  probe_substate_t m_substate; // state machine substate
  uint32_t m_timer1;           // Timer used for probe/config restore and other timing tasks
  // uint16_t m_nickname;         // Nickname (init with persistent value)

  // Level I nickname discovery
  uint16_t m_probe_nickname; // 0-253
  int m_probe_timeout;       // If set to -1 on init will be set to VSCP_PROBE_TIMEOUT
  int m_probe_timeout_count; // If set to -1 on init will be set to VSCP_PROBE_TIMEOUT_COUNT

  /*!
    Holders for proxy event information. Set when received.
  */
  uint8_t m_offset;      // offset in data 0 or 16
  uint16_t m_vscp_class; // real VSCP class (vscp_class - 512 if proxy event)
  uint8_t m_ifguid[16];  // interface GUID

  uint32_t m_interval_heartbeat; // Interval for heartbeats in milli-seconds (0=off)
  uint32_t m_last_heartbeat;     // Time for last heartbeat send
  uint32_t m_interval_caps;      // Interval for capabilities events in milli-seconds (0=off)
  uint32_t m_last_caps;          // Time for last caps send

  // See CLASS1.PROTOCOL, VSCP_TYPE_PROTOCOL_RESET_DEVICE
  uint8_t m_reset_device_flags;

  // Decision matrix
  uint8_t* m_pDm;         // Pointer to decision matrix storage (NULL if no DM).
  uint8_t m_nDmRows;      // Number of DM rows (0 if no DM).
  uint8_t m_sizeDmRow;    // Size for one DM row.
  uint32_t m_regOffsetDm; // Register offset for DM (normally zero)
  uint16_t m_pageDm;      // Register page where DM definition starts

  // MDF
  const char* m_pInternalMdf; // If the device use internal MDF point to it here (NULL)
                              // If no internal MDF set to NULL
                              // !!! The MDF pointed to MUST be NULL terminated.

  // Events of interest
  const uint32_t* m_pEventsOfInterest; // List with events of interest or NULL
                                       // if all events are of interest set to null.
                                       // array = (int*) malloc(n * sizeof(int));
                                       // Last event should be 0

  /*
    This may be register positions on a device
    Set to zero if not used.
  */
  uint8_t m_index;   // index for device
  uint8_t m_zone;    // zone for device
  uint8_t m_subzone; // subzone for device

  /////////////////////////////////////////////////////////////////////////////
  //                Configuration settings below this point
  //
  // (x)      - x is default value
  // (init=y) - Initiated to y.
  /////////////////////////////////////////////////////////////////////////////

  // Functionality switches
  int m_bEnableErrorReporting;          // Send error reporting events (FALSE)
  int m_bEnableLogging;                 // Enable logging events (FALSE)
  uint8_t m_log_id;                     // Identifies log channel
  uint8_t m_log_level;                  // Level for logs
  int m_bHighEndServerResponse;         // React on high end server probe. Only level II (FALSE)
  int m_bEnableWriteProtectedLocations; // GUID/manufacturer id (FALSE)
  int m_bUse16BitNickname;              // 16-bit nickname. Default is false. Only for level I (FALSE)
  int m_bInterestedInAllEvents;         // TRUE if interested in all events. If FALSE
                                        // the callback vscp_frmw2_callback_report_events_of_interest
                                        // will be called (TRUE)

  // For high end server response
  uint16_t m_high_end_srv_caps;   // High end server capabilities
  uint32_t m_high_end_ip_address; // High end server ip-address
  uint16_t m_high_end_srv_port;   // High end server port

  // Standard registers (persistent storage)
  // standard register change callback is called for registers marked with
  // [P] and [*/P]
  // [*] = Set in init routine
  // [C] = Set from define (constant)
  // [P] = Persistent (Write to persistent storage
  // [I] = Init to value on startup. May be constant or may change.
  // [*/P] = Persistent if m_bEnableWriteProtectedLocations is true.
  uint8_t m_alarm_status;                 // [I] Alarm. Read only for clients. (init=0)
  uint8_t m_vscp_major_version;           // [C] VSCP protocol major version. (init=1)
  uint8_t m_vscp_minor_version;           // [C] VSCP protocol minor version. (init=4)
  uint8_t m_errorCounter;                 // [I] Error counter. Clear on read. Read only for clients. (init=0)
  uint32_t m_userId;                      // [P] User id.
  uint32_t m_manufacturerId;              // [*/P] Manufacturer id.Read only for clients.
  uint32_t m_manufacturerSubId;           // [*/P] Manufacturer sub id.Read only for clients.
  uint16_t m_nickname;                    // [P] Device nickname (init=0xff)
  uint16_t m_page_select;                 // [I] Page select register. (Init = 0)
  uint8_t m_firmware_major_version;       // [*] This software version. Read only for clients.
  uint8_t m_firmware_minor_version;       // [*] This software version. Read only for clients.
  uint8_t m_firmware_sub_minor_version;   // [*] This software version. Read only for clients.
  uint8_t m_bootloader_algorithm;         // [*] Boot loader algorithm we use.
  uint32_t m_standard_device_family_code; // [*] Family code. Read only for clients.
  uint32_t m_standard_device_type_code;   // [*] Family type. Read only for clients.
  uint16_t m_firmware_device_code;        // [*] Identifier for hardware so correct firmware can be loaded
  uint8_t m_guid[16];                     // [*/P] GUID for device. Read only for clients
  uint8_t m_mdfurl[32];                   // [*] URL for MDF. Read only for clients. Make 33 bytes and nill
                                          // all unused byte so it is null terminated even if it has
                                          // the maximum length of 32 bytes.
  // Level II devices
  uint8_t m_ipaddr[16];     // IP address (ipv4/ipv6)
  uint8_t m_deviceName[64]; // Name of the device

} vscp_frmw2_firmware_config_t;

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
#define EVDTA(x) (pev->pdata[(x) + g_pconfig->m_offset])
#define EXDTA(x) (pex->data[(x) + g_pconfig->m_offset])

/*!
 * Ajust size for offset
 * vscpEvent and vscpEventEx versions
 */
#define ADJSIZEV (pev->sizeData - g_pconfig->m_offset)
#define ADJSIZEX (pex->sizeData - g_pconfig->m_offset)

/*!
****************************************************************************
                                   Prototypes
*****************************************************************************
*/

/*!
 * @brief Init subsystem
 *
 * @param pcfg Structure with configuration data. If set to NULL first initialization values
 *              are used.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Init VSCP subsystem. The transport layer should be up and
 * running as a New node on line is sent here when everything is
 * initialized.
 */

int
vscp_frmw2_init(vscp_frmw2_firmware_config_t* const pcfg);

/*!
  @brief Initialize persistent storage

  @param pdata Pointer to user data (typical points to context)
  @return VSCP_ERROR_SUCCESS on success, else error code.
 */
int
vscp_frmw2_init_persistent_storage(void);

/*!
 * @brief Init VSCP ex event with standard data

   @param pex Pointer to ex event
   @return VSCP_ERROR_SUCCESS on success, else error code.
 */

void
vscp_frmw2_setup_event_ex(vscpEventEx* const pex);

/*!
 * @brief Send nickname probe
 *
 * @param bNewNodeOnLine If nonzero dataSize will be set to zero (new node on-line)
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_send_probe(int bNewNodeOnLine);

/*!
 * @brief Do periodic VSCP protocol work when nickname is 0xffff
 *
 * This is the code that is executed during
 * the init process when a nickname device
 * has not yet got a valid nickname.
 * https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_level_i_specifics?id=address-or-nickname-assignment-for-level-i-nodes
 *
 * Never used in a LEVEL II system
 *
 * @param pex Received event ex or NULL if no event received.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 */

int
vscp_frmw2_nickname_discovery(const vscpEventEx* const pex);

/*!
  Waiting for segment controller to give us a nickname

  @param pex Received event or NULL if no event received.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_nickname_wait(const vscpEventEx* const pex);

/*!
 * @brief Do periodic VSCP protocol work
 *
 * @param pex Pointer to event ex or a null pointer if no event has been received
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Do periodic VSCP protocol work. This function should be called from the
 * main loop on periodic intervals.
 *
 * If there is incoming events to handle a pointer to the event should be
 * sent as a parameter  in **pex**. * Once the function has been called
 * ownership of the event is left over to the protocol stack and it is up to the
 * function to release the event. This is true also if the function returns an error.
 *
 * If there is no event to handle for the protocol stack pex should be set to NULL.
 *
 */

int
vscp_frmw2_work(const vscpEventEx* const pex);

/*!
  Handle protocol events

  @param pex Incoming event
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_handle_protocol_event(const vscpEventEx* const pex);

/*!
 * @brief Read VSCP Level II register
 *
 * If the device is a pure level I device
 * the three high bytes of reg should be masked
 * before a register read.
 *
 * @param reg Register to write
 * @param pval Pointer to read value
 * @return VSCP_ERROR_SUCCESS on success, else error code. VSCP_ERROR_INDEX_OOB is returned when trying
    to read a register that does not exist. This error is actually a warning.
 */

int
vscp_frmw2_read_reg(uint32_t reg, uint8_t* pval);

/*!
 * @brief Write VSCP level II register
 *
 * If the device is a pure level I device
 * the three high bytes of reg should be masked
 * before a register write.
 *
 * @param reg Register to write
 * @param val Value to write
 * @return VSCP_ERROR_SUCCESS on success, else error code. VSCP_ERROR_INDEX_OOB is returned when trying
    to read a register that does not exist. This error is actually a warning.
 *
 * Write a value to a standard or a user register.
 */

int
vscp_frmw2_write_reg(uint32_t reg, uint8_t val);

/*!
 * @brief Send periodic heartbeat
 *
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Send heartbeat event to client. Typically send one every minute.
 */

int
vscp_frmw2_send_heartbeat(void);

/*!
 * @brief Send periodic capability heartbeat
 *
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Send capabilities event to client. Typically send every minute.
 */

int
vscp_frmw2_send_caps(void);

/*!
 * @brief Do register reads
 *
 * @param reg First register position to write to.
 * @param cnt Number of positions to write to.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_vscp2_register_read(uint32_t startreg, uint16_t cnt);

/*!
 * @brief Do register writes
 *
 * @param reg First register position to write to.
 * @param cnt Number of positions to write to.
 * @param pdata Pointer to data to write.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_vscp2_register_write(uint32_t startreg, uint16_t cnt, const uint8_t* const pdata);

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
vscp_frmw2_send_error_event(uint8_t type, uint8_t idx);

/*!
  @brief Send log event (CLASS=509).

  @param type VSCP logevent type.
  @param id Identifier for the logging channel.
  @param level Loglevel for this log event.
  @param idx index for multi-frame log event starting at zero.
  @param pData Log data (Always 5 bytes).
  @return VSCP_ERROR_SUCCESS on success, else error code.

  Enable by setting bEnableLogging to true in config.

  See specification [here](https://grodansparadis.github.io/vscp-doc-spec/#/./class1.log)

  For logging first send Type = 2(0x01) Log Start then logging events and when
  log is closed send Type = 3 (0x03) Log Stop. To log several things use a
  unique if for each and open/close each.
 */

uint8_t
vscp_frmw2_send_log_event(uint8_t type, uint8_t id, uint8_t level, uint8_t idx, uint8_t* pLogdata);

/*!
   @brief Send high end server probe

  Enable by setting bSendHighEndServerProbe to true in config

   @return VSCP_ERROR_SUCCESS on success, else error code.

   Inform possible high end servers of our existence.
 */

int
vscp_frmw2_send_high_end_server_probe(void);

/*!
  @brief Do a level I page read

  @param offset Offset into register page
  @param count Number of registers to read
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp_frmw2_page_read(uint32_t offset, uint8_t count);

/*!
  @brief Do a level I page write

  @param offset Offset into register page
  @param count Number of registers to write
  @param buf Pointer to buffer of
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_page_write(uint32_t offset, uint8_t count, const uint8_t* const pbuf);

/*!
  @brief Send whois response

  Send whois response. If level II device a level II response
  will be sent.

  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_whois_response(void);

/*!
  @brief Read a full page or part of memory

  @param nodeid Node id from node to read page from
  @param page Page to read.
  @param offset Offset on page to read from.
  @param cnt Number of bytes from offset to read. A zero value should be treated as 256.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp_frmw2_extended_page_read(uint16_t nodeid, uint16_t page, uint8_t offset, uint16_t cnt);

/*!
  @brief Write multiple bytes to memory

  @param page Page to read.
  @param offset Offset on page to read from.
  @param cnt Number of bytes from offset to read. A zero value should be treated as 256.
  @param pbuf Pointer to buffer with data
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_extended_page_write(uint16_t nodeid, uint16_t page, uint8_t offset, uint16_t cnt, const uint8_t* const pbuf);

/*!
  @brief Feed the level I decision matrix with one Event

  @param pex Event to feed the DM with.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_feed_level1_dm(const vscpEventEx* const pex);

/*!
  @brief Feed the level II decision matrix with one Event

  @param pex Event to feed the DM with.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_feed_leve2_dm(const vscpEventEx* const pex);

/*!
  @brief Feed the decision matrix with one Event

  @param pex Event to feed the DM with.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp_frmw2_feed_dm(const vscpEventEx* const pex);

/*!
  @brief Send DM info on request

  Response to CLASS1_PROTOCOL, VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp_frmw2_report_dmatrix(void);

/*
 ****************************************************************************
                                    Callbacks
 *****************************************************************************
 */

/**
  @name Callbacks for VSCP level II protocol
*/

/*!
 * @brief Get timestamp
 *
 * @param pdata Pointer to context.
 * @return Timestamp in microseconds.
 */

uint64_t
vscp_frmw2_callback_get_timestamp(void* const puserdata);

/*!
 * @brief Enter bootloader.  !!!SHOULD NEVER RETURN!!!
 *
 * @param pdata Pointer to user data (typical points to context).
 */

void
vscp_frmw2_callback_enter_bootloader(void* const puserdata);

/*!
 * @brief Handle DM action
 *
 * @param puserdata Pointer to user data (typical points to context).
 * @param pex Pointer to event that triggered the action
 * @param action Code for action
 * @param pparam Pointer to action parameter data.
 *    On level II devices size for this data is always (DM-rowsize - 7)
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 */

int
vscp_frmw2_callback_dm_action(void* const puserdata,
                              const vscpEventEx* const pex,
                              uint8_t action,
                              const uint8_t* const pparam);

/*!
 * @brief Segment controller heartbeat received
 *
 * @param pdata Pointer to user data (typical points to context).
 * @return VSCP_ERROR_SUCCESS on success, or error code.
 */
int
vscp_frmw2_callback_segment_ctrl_heartbeat(void* const puserdata, uint16_t segcrc, uint32_t time);

/*!
 * @brief Report back events that this node is interested in
 *
 * The framework sends the required response if the GUID is the same as the probe.
 *
 * @param pdata Pointer to user data (typical points to context)
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_callback_report_events_of_interest(void* const puserdata);

/*!
 * @brief  Fill in event date/time/timestamp information
 *
 * @param pdata
 * @param pex Event to get info
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Set all to zero or do nothing if not time information is available
 * and the time values are already set to zero.
 */

int
vscp_frmw2_callback_set_event_time(void* const puserdata, vscpEventEx* const pex);

/*!
  @brief Restore defaults

  If 0x55/0xaa is written to register location
  162 within one second defaults should be loaded
  by the device.

  @param pdata Pointer to user data (typical points to context)
  @return VSCP_ERROR_SUCCESS on success, or error code.
 */

int
vscp_frmw2_callback_restore_defaults(void* const puserdata);

/*!
  @brief Reset the device and do a warm start
*/
void
vscp_frmw2_callback_reset(void* const puserdata);

/**
 * @brief Get ipv6 or ipv4 address
 *
 * Return the ipv6 or ipv4 address of the interface. If the
 * interface is not tcp/ip based just return a positive
 * response or a valid address for the underlying transport protocol.
 *
 * @param pdata Pointer to context.
 * @param pipaddr Pointer to 16 byte address space for (ipv6 or ipv4) address
 *                return value.
 * @param size This is the size of the address, 4 or 16 bytes.
 * @return VSCP_ERROR_SUCCESS on success, error code on failure
 */

int
vscp_frmw2_callback_get_ip_addr(void* const puserdata, uint8_t* pipaddr, uint8_t size);

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
 * returned if the register is invalid. VSCP_ERROR_INDEX_OOB is returned when trying
    to read a register that does not exist. This error is actually a warning.
 *
 */
int
vscp_frmw2_callback_read_reg(void* const puserdata, uint16_t page, uint32_t reg, uint8_t* pval);

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
 * returned if the register is invalid. VSCP_ERROR_INDEX_OOB is returned when trying
    to read a register that does not exist. This error is actually a warning.
 */

int
vscp_frmw2_callback_write_reg(void* const puserdata, uint16_t page, uint32_t reg, uint8_t val);

/*!
  @brief Send eventex to transport sublayer.
  -
    ex is copied in the callback.

  - if GUID is all zero then the GUID (if Level II) should be set to the GUID of the node.
  -
    if requested to send a full Level II event and we are a Level I node skip the
    event and return VSCP_ERROR_ERROR

    GUID is not used for LEVEL I nodes. Instead 8-bit nickname is stored
    in the lowest LSB and 16-bit nickname in the two lower LSB's.

  @param pdata Pointer to user data (typical points to context)
  @param pex Pointer to EventEx to send.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_callback_send_event_ex(void* const puserdata, vscpEventEx* pex);

/*!
  @brief Information callback to app that standard register has been changed. Typically
    persistent storage is updated.
    VSCP_STD_REGISTER_USER_ID is used for all positions of userid.
    VSCP_STD_REGISTER_USER_MANDEV_ID is used for all positions of manufacturer id and sub
      manufacturer id.
    VSCP_STD_REGISTER_GUID is used for all positions of GUID.

  @param pdata Pointer to user data (typical points to context)
  @param stdreg Standard register that has been changed. Level I register address
    is used for all common registers.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp_frmw2_callback_stdreg_change(void* const puserdata, uint32_t stdreg);

/*!
  @brief Feed the watchdog

  Feed the watchdog on systems that need to do
  that during lengthy operations to prevent restarts.
*/

void
vscp_frmw2_callback_feed_watchdog(void* const puserdata);

// ============ END OF CALLBACKS ============

/**
@}
*/

#ifdef __cplusplus
}
#endif

#endif // __VSCP_FIRMWARE_LEVEL2_H
