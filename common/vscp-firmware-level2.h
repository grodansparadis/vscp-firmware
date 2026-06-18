/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2026 Ake Hedman, the VSCP Project
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
#define FRMW2_MAJOR_VERSION 2

/*!
  @brief Minor Version-

  This is the minor version of this vscp  level 2 protocol code.
*/
#define FRMW2_MINOR_VERSION 0

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

#define LEVEL1_DM_OADDR(row) (pctx->pDm[(8 * row) + VSCP_LEVEL1_DM_POS_OADDR])
#define LEVEL1_DM_FLAGS(row) (pctx->pDm[(8 * row) + VSCP_LEVEL2_DM_POS_FLAGS])
#define LEVEL1_DM_CLASS_MASK(row)                                                                                      \
  (pctx->pDm[(8 * row) + VSCP_LEVEL1_DM_POS_CLASSMASK] +                                                               \
   (((pctx->pDm[8 * row + VSCP_LEVEL1_DM_POS_FLAGS]) & 0x02) << 9))
#define LEVEL1_DM_CLASS_FILTER(row)                                                                                    \
  (pctx->pDm[(8 * row) + VSCP_LEVEL1_DM_POS_CLASSFILTER] +                                                             \
   (((pctx->pDm[8 * row + VSCP_LEVEL1_DM_POS_FLAGS]) & 0x01) << 9))
#define LEVEL1_DM_TYPE_MASK(row)    (pctx->pDm[(8 * row) + VSCP_LEVEL1_DM_POS_TYPEMASK])
#define LEVEL1_DM_TYPE_FILTER(row)  (pctx->pDm[(8 * row) + VSCP_LEVEL1_DM_POS_TYPEFILTER])
#define LEVEL1_DM_ACTION(row)       (pctx->pDm[(8 * row) + VSCP_LEVEL1_DM_POS_ACTION])
#define LEVEL1_DM_ACTION_PARAM(row) (pctx->pDm[(8 * row) + VSCP_LEVEL1_DM_POS_ACTIONPARAM])

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

#define LEVEL2_DM_FLAGS(row)                                                                                           \
  construct_unsigned32(pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL1_DM_POS_FLAGS],                                  \
                       pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL1_DM_POS_FLAGS + 1],                              \
                       pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL1_DM_POS_FLAGS + 2],                              \
                       pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL1_DM_POS_FLAGS + 3])
#define LEVEL2_DM_OADDR(row, pos) (pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL2_DM_POS_OADDR + pos])
#define LEVEL2_DM_CLASS(row)                                                                                           \
  construct_unsigned16(pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL2_DM_POS_CLASS],                                  \
                       pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL2_DM_POS_CLASS + 1])
#define LEVEL2_DM_TYPE(row)                                                                                            \
  construct_unsigned16(pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL2_DM_POS_TYPE],                                   \
                       pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL2_DM_POS_TYPE + 1])
#define LEVEL2_DM_ACTION(row)                                                                                          \
  construct_unsigned16(pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL2_DM_POS_ACTION],                                 \
                       pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL2_DM_POS_ACTION + 1])
#define LEVEL2_DM_ACTION_PARAM(row) (pctx->pDm[(pctx->sizeDmRow * row) + VSCP_LEVEL2_DM_POS_ACTIONPARAM])

/*
  Default nickname discovery settings
  - Probe timeout is the time to wait for a response from another node
  - Probe timeout count is the number of times a probe is sent when no
      response is received from node when probing with a specific nickname.
      If no response is received after this amount of tries the the nickname
      will be assigned to the node.
*/
#define VSCP_PROBE_TIMEOUT                                                                                             \
  1000                             // ms - Time after probe has been sent and a no response
                                   // is considered as a free nickname discovery
#define VSCP_PROBE_TIMEOUT_COUNT 3 // # probe timed out probe events before assigning a nickname
#define VSCP_SEGCTRL_PROBE_TIMEOUT                                                                                     \
  5000 // ms time to wait for segment controller to assign nickname
       // to us
#define VSCP_SEGCTRL_RESPONSE_TIMEOUT                                                                                  \
  300000                              // ms time after a segment ctrl response has been received
                                      // and when nickname discovery process will be taken up again.
#define VSCP_INIT_BUTTON_TIMEOUT 2000 // Button press and hold timeout for init button.
#define VSCP_GUID_RESET_TIMEOUT  1000 // All four GUID reset event must be received withing this time

/* Forward declaration so vscp_frmw2_firmware_context_t can hold a pointer to
   the ops table that is defined later in this same header. */
struct vscp_frmw2_ops;

/*!
  Global structure for protocol functionality

  Used internally
*/
typedef struct vscp_frmw2_firmware_context {

  probe_state_t state;       // State machine state
  probe_substate_t substate; // state machine substate
  uint32_t timer;           // Timer used for probe/config restore and other timing tasks

  // Level I nickname discovery
  uint16_t probe_nickname; // 0-253
  int probe_timeout;       // If set to -1 on init will be set to VSCP_PROBE_TIMEOUT
  int probe_timeout_count; // If set to -1 on init will be set to VSCP_PROBE_TIMEOUT_COUNT

  /*!
    Holders for proxy event information. Set when received.
  */
  uint8_t offset;      // offset in data 0 or 16
  uint16_t vscp_class; // real VSCP class (vscp_class - 512 if proxy event)
  uint8_t ifguid[16];  // interface GUID

  // See CLASS1.PROTOCOL, VSCP_TYPE_PROTOCOL_RESET_DEVICE
  uint8_t reset_device_flags;

  /////////////////////////////////////////////////////////////////////////////
  //                Configuration settings below this point
  //
  // (x)      - x is default value
  // (init=y) - Initiated to y.
  /////////////////////////////////////////////////////////////////////////////

  uint8_t level; // 0=Level I, 1 = Level II

  /*
    This may (recommended) be a register positions on a device
    Set to zero if not used.
  */
  uint8_t index;   // index for device
  uint8_t zone;    // zone for device
  uint8_t subzone; // subzone for device

  // Functionality switches
  int bEnableErrorReporting;          // Send error reporting events (FALSE)
  int bEnableLogging;                 // Enable logging events (FALSE)
  uint8_t log_id;                     // Identifies log channel
  uint8_t log_level;                  // Level for logs
  int bHighEndServerResponse;         // React on high end server probe. Only level II (FALSE)
  int bEnableWriteProtectedLocations; // GUID/manufacturer id (FALSE)
  int bUse16BitNickname;              // 16-bit nickname. Default is false. Only for level I (FALSE)
  int bInterestedInAllEvents;         // TRUE if interested in all events. If FALSE
  // the callback vscp_frmw2_callback_report_events_of_interest
  // will be called (TRUE)

  uint32_t interval_heartbeat; // Interval for heartbeats in milli-seconds (0=off)
  uint32_t last_heartbeat;     // Time for last heartbeat send
  uint32_t interval_caps;      // Interval for capabilities events in milli-seconds (0=off)
  uint32_t last_caps;          // Time for last caps send

  // Decision matrix
  uint8_t *pDm;         // Pointer to decision matrix storage (NULL if no DM).
  uint8_t nDmRows;      // Number of DM rows (0 if no DM).
  uint8_t sizeDmRow;    // Size for one DM row.
  uint32_t regOffsetDm; // Register offset for DM (normally zero)
  uint16_t pageDm;      // Register page where DM definition starts

  // MDF
  const char *pInternalMdf; // If the device use internal MDF point to it here (NULL)
                            // If no internal MDF set to NULL
                            // !!! The MDF data pointed to, MUST be NULL terminated.

  // Events of interest
  const uint32_t *pEventsOfInterest; // List with events of interest or NULL
                                     // if all events are of interest set to null.
                                     // array = (int*) malloc(n * sizeof(int));
                                     // Last event should be 0

  // For high end server response
  uint16_t high_end_srv_caps;   // High end server capabilities
  uint32_t high_end_ip_address; // High end server ip-address
  uint16_t high_end_srv_port;   // High end server port

  // Standard registers (persistent storage)
  // standard register change callback is called for registers marked with
  // [P] and [*/P]
  // [*] = Set in init routine
  // [C] = Set from define (constant)
  // [P] = Persistent (Write to persistent storage
  // [I] = Init to value on startup. May be constant or may change.
  // [*/P] = Persistent if bEnableWriteProtectedLocations is true.
  uint8_t alarm_status;                 // [I] Alarm. Read only for clients. (init=0)
  uint8_t vscp_major_version;           // [C] VSCP protocol major version. (init=1)
  uint8_t vscp_minor_version;           // [C] VSCP protocol minor version. (init=4)
  uint8_t errorCounter;                 // [I] Error counter. Clear on read. Read only for clients. (init=0)
  uint32_t userId;                      // [P] User id.
  uint32_t manufacturerId;              // [*/P] Manufacturer id.Read only for clients.
  uint32_t manufacturerSubId;           // [*/P] Manufacturer sub id.Read only for clients.
  uint16_t nickname;                    // [P] Device nickname (init=0xff)
  uint16_t page_select;                 // [I] Page select register. (Init = 0)
  uint8_t firmware_major_version;       // [*] This software version. Read only for clients.
  uint8_t firmware_minor_version;       // [*] This software version. Read only for clients.
  uint8_t firmware_sub_minor_version;   // [*] This software version. Read only for clients.
  uint8_t bootloader_algorithm;         // [*] Boot loader algorithm we use.
  uint32_t standard_device_family_code; // [*] Family code. Read only for clients.
  uint32_t standard_device_type_code;   // [*] Family type. Read only for clients.
  uint16_t firmware_device_code;        // [*] Identifier for hardware so correct firmware can be loaded
  uint8_t guid[16];                     // [*/P] GUID for device. Read only for clients
  uint8_t mdfurl[32];                   // [*] URL for MDF. Read only for clients. Make 33 bytes and nill
                                        // all unused byte so it is null terminated even if it has
                                        // the maximum length of 32 bytes.
  // Level II devices
  uint8_t ipaddr[16];     // IP address (ipv4/ipv6)
  uint8_t deviceName[64]; // Name of the device

  /** Operations table — pointer to the user-supplied callbacks. Must not be NULL. */
  const struct vscp_frmw2_ops *ops;

  // Pointer to user data.
  void *puserdata;

} vscp_frmw2_firmware_context_t;

/** Backward-compatible alias for vscp_frmw2_firmware_context_t. */
typedef vscp_frmw2_firmware_context_t vscp_frmw2_firmware_config_t;

/**
 * @brief VSCP level II firmware operations table.
 *
 * Populate all function pointers and assign a pointer to this struct to
 * vscp_frmw2_firmware_context_t::ops before calling vscp_frmw2_init().
 * The first argument to every callback is the configuration/context pointer.
 *
 * Mandatory fields — vscp_frmw2_init() returns VSCP_ERROR_INIT_MISSING if any
 * of these are NULL:
 *   get_milliseconds, get_timestamp, send_event, set_event_time,
 *   read_reg, write_reg, stdreg_change
 *
 * All other fields are optional; set unused pointers to NULL.
 */
typedef struct vscp_frmw2_ops {

  /* ── Timing ─────────────────────────────────────────────────── */
  /** Return current time in milliseconds. */
  uint32_t (*get_milliseconds)(vscp_frmw2_firmware_context_t *pctx);
  /** Return current timestamp in microseconds. */
  uint64_t (*get_timestamp)(vscp_frmw2_firmware_context_t *pctx);

  /* ── Transport ──────────────────────────────────────────────── */
  /** Send an event to the transport layer. The event is copied by the callback. */
  int (*send_event)(vscp_frmw2_firmware_context_t *pctx, vscp_event_t *pev);

  /* ── Decision matrix ────────────────────────────────────────── */
  /** Execute a DM action triggered by pev. */
  int (*dm_action)(vscp_frmw2_firmware_context_t *pctx, const vscp_event_t *pev, uint8_t action, const uint8_t *pparam);

  /* ── Node information ───────────────────────────────────────── */
  /** Segment controller heartbeat received. */
  int (*segment_ctrl_heartbeat)(vscp_frmw2_firmware_context_t *pctx, uint16_t segcrc, uint32_t time);
  /** Report events this node is interested in. */
  int (*report_events_of_interest)(vscp_frmw2_firmware_context_t *pctx);
  /** Fill in date/time/timestamp fields of an event. Set to zero if unavailable. */
  int (*set_event_time)(vscp_frmw2_firmware_context_t *pctx, vscp_event_t *pev);
  /** Return the IPv4 or IPv6 address of the interface (size = 4 or 16). */
  int (*get_ip_addr)(vscp_frmw2_firmware_context_t *pctx, uint8_t *ip, uint8_t size);

  /* ── Register I/O ───────────────────────────────────────────── */
  /** Read one user register (address range 0x00000000 – 0xffff0000). */
  int (*read_reg)(vscp_frmw2_firmware_context_t *pctx, uint16_t page, uint32_t reg, uint8_t *pval);
  /** Write one user register (address range 0x00000000 – 0xffff0000). */
  int (*write_reg)(vscp_frmw2_firmware_context_t *pctx, uint16_t page, uint32_t reg, uint8_t val);
  /** A standard register has changed; update persistent storage as needed. */
  int (*stdreg_change)(vscp_frmw2_firmware_context_t *pctx, uint32_t stdreg);

  /* ── System control ─────────────────────────────────────────── */
  /** Restore factory defaults. Called when 0x55/0xAA reset sequence is received. */
  int (*restore_defaults)(vscp_frmw2_firmware_context_t *pctx);
  /** Enter the bootloader. MUST NOT RETURN. */
  void (*enter_bootloader)(vscp_frmw2_firmware_context_t *pctx);
  /** Perform a warm reset. */
  void (*reset)(vscp_frmw2_firmware_context_t *pctx);
  /** Feed the hardware watchdog during lengthy operations. */
  void (*feed_watchdog)(vscp_frmw2_firmware_context_t *pctx);

} vscp_frmw2_ops_t;

/*!
  Used to adjust standard registers for Level I/ Level II
  Level I standard registers are at 0x80 - 0xff on page = 0
  Level II standard registers are at 0xffffff00 - 0xffffffff
*/
#define ADJSTDREG ((VSCP_LEVEL1 == pctx->level) ? 0 : 0xffffff00)

/*!
  Adjusted data for offset
  vscp_event_t and vscp_event_ex_t versions
*/
#define EVDTA(x) (pev->pdata[(x) + pctx->offset])
#define EXDTA(x) (pex->pdata[(x) + pctx->offset])

/*!
 * Ajust size for offset
 * vscp_event_t and vscp_event_ex_t versions
 */
#define ADJSIZEV (pev->sizeData - pctx->offset)
#define ADJSIZEX (pex->sizeData - pctx->offset)

/*!
****************************************************************************
                                   Prototypes
*****************************************************************************
*/

/*!
 * @brief Init subsystem
 *
 * @param pctx Structure with configuration data. If set to NULL first initialization values
 *              are used.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Init VSCP subsystem. The transport layer should be up and
 * running as a New node on line is sent here when everything is
 * initialized.
 */

int
vscp_frmw2_init(vscp_frmw2_firmware_context_t *pctx);

/*!
  @brief Initialize persistent storage

  @param pctx Pointer to context.
  @return VSCP_ERROR_SUCCESS on success, else error code.
 */
int
vscp_frmw2_init_persistent_storage(vscp_frmw2_firmware_context_t *pctx);

/*!
 * @brief Init VSCP ex event with standard data
 *
 * @param pctx Pointer to context.
 * @param pex Pointer to ex event.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

void
vscp_frmw2_setup_event(vscp_frmw2_firmware_context_t *pctx, vscp_event_t *pev);

/*!
 * @brief Send nickname probe
 *
 * @param pctx Pointer to context.
 * @param bNewNodeOnLine If nonzero dataSize will be set to zero (new node on-line).
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_send_probe(vscp_frmw2_firmware_context_t *pctx, int bNewNodeOnLine);

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
 * @param pctx Pointer to context.
 * @param pex Received event ex or NULL if no event received.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 */

int
vscp_frmw2_nickname_discovery(vscp_frmw2_firmware_context_t *pctx, const vscp_event_t *pev);

/*!
  Waiting for segment controller to give us a nickname

  @param pctx Pointer to context.
  @param pex Received event or NULL if no event received.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_nickname_wait(vscp_frmw2_firmware_context_t *pctx, const vscp_event_t *pev);

/*!
 * @brief Do periodic VSCP protocol work
 *
 * @param pctx Pointer to context.
 * @param pex Pointer to event ex or a null pointer if no event has been received.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 *
 * Do periodic VSCP protocol work. This function should be called from the
 * main loop on periodic intervals.
 *
 * If there is incoming events to handle a pointer to the event should be
 * sent as a parameter  in **pex**. Once the function has been called
 * ownership of the event is left over to the protocol stack and it is up to the
 * function to release the event. This is true also if the function returns an error.
 *
 * If there is no event to handle for the protocol stack pex should be set to NULL.
 *
 */

int
vscp_frmw2_work(vscp_frmw2_firmware_context_t *pctx, const vscp_event_t *pev);

/*!
  Handle protocol events

  @param pex Incoming event
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_handle_protocol_event(vscp_frmw2_firmware_context_t *pctx, const vscp_event_t *pev);

/*!
 * @brief Read VSCP Level II register
 *
 * @param pctx Pointer to context.
 * @param reg Register to read.
 * @param pval Pointer to read value.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_read_reg(vscp_frmw2_firmware_context_t *pctx, uint32_t reg, uint8_t *pval);

/*!
 * @brief Write VSCP level II register
 *
 * @param pctx Pointer to context.
 * @param reg Register to write.
 * @param val Value to write.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_write_reg(vscp_frmw2_firmware_context_t *pctx, uint32_t reg, uint8_t val);

/*!
 * @brief Send periodic heartbeat
 *
 * @param pctx Pointer to context.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_send_heartbeat(vscp_frmw2_firmware_context_t *pctx);

/*!
 * @brief Send periodic capability heartbeat
 *
 * @param pctx Pointer to context.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_send_caps(vscp_frmw2_firmware_context_t *pctx);

/*!
 * @brief Do register reads
 *
 * @param pctx Pointer to context.
 * @param startreg First register position to read.
 * @param cnt Number of registers to read.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_vscp2_register_read(vscp_frmw2_firmware_context_t *pctx, uint32_t startreg, uint16_t cnt);

/*!
 * @brief Do register writes
 *
 * @param pctx Pointer to context.
 * @param startreg First register position to write.
 * @param cnt Number of positions to write.
 * @param pdata Pointer to data to write.
 * @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_vscp2_register_write(vscp_frmw2_firmware_context_t *pctx,
                                uint32_t startreg,
                                uint16_t cnt,
                                const uint8_t *pdata);

/*!
  @brief Send error event (CLASS=508).

  @param pctx Pointer to context.
  @param type This is the VSCP type.
  @param idx Index to identify possible sub module. Normally set to zero.
  @return VSCP_ERROR_SUCCESS on success, else error code.
 */

uint8_t
vscp_frmw2_send_error_event(vscp_frmw2_firmware_context_t *pctx, uint8_t type, uint8_t idx);

/*!
  @brief Send log event (CLASS=509).

  @param pctx Pointer to context.
  @param type VSCP logevent type.
  @param id Identifier for the logging channel.
  @param level Loglevel for this log event.
  @param idx index for multi-frame log event starting at zero.
  @param pData Log data (Always 5 bytes).
  @return VSCP_ERROR_SUCCESS on success, else error code.
 */

uint8_t
vscp_frmw2_send_log_event(vscp_frmw2_firmware_context_t *pctx,
                          uint8_t type,
                          uint8_t id,
                          uint8_t level,
                          uint8_t idx,
                          uint8_t *pLogdata);

/*!
   @brief Send high end server probe

  @param pctx Pointer to context.
  @return VSCP_ERROR_SUCCESS on success, else error code.
 */

int
vscp_frmw2_send_high_end_server_probe(vscp_frmw2_firmware_context_t *pctx);

/*!
  @brief Do a level I page read

  @param pctx Pointer to context.
  @param offset Offset into register page.
  @param count Number of registers to read.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp_frmw2_page_read(vscp_frmw2_firmware_context_t *pctx, uint32_t offset, uint8_t count);

/*!
  @brief Do a level I page write

  @param pctx Pointer to context.
  @param offset Offset into register page.
  @param count Number of registers to write.
  @param pbuf Pointer to buffer of data.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_page_write(vscp_frmw2_firmware_context_t *pctx, uint32_t offset, uint8_t count, const uint8_t *pbuf);

/*!
  @brief Send whois response

  @param pctx Pointer to context.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_whois_response(vscp_frmw2_firmware_context_t *pctx);

/*!
  @brief Read a full page or part of memory

  @param pctx Pointer to context.
  @param nodeid Node id from node to read page from.
  @param page Page to read.
  @param offset Offset on page to read from.
  @param cnt Number of bytes from offset to read. A zero value should be treated as 256.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp_frmw2_extended_page_read(vscp_frmw2_firmware_context_t *pctx,
                              uint16_t nodeid,
                              uint16_t page,
                              uint8_t offset,
                              uint16_t cnt);

/*!
  @brief Write multiple bytes to memory

  @param pctx Pointer to context.
  @param nodeid Node id.
  @param page Page to write.
  @param offset Offset on page to write from.
  @param cnt Number of bytes to write.
  @param pbuf Pointer to buffer with data.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_extended_page_write(vscp_frmw2_firmware_context_t *pctx,
                               uint16_t nodeid,
                               uint16_t page,
                               uint8_t offset,
                               uint16_t cnt,
                               const uint8_t *pbuf);

/*!
  @brief Feed the level I decision matrix with one Event

  @param pctx Pointer to context.
  @param pex Event to feed the DM with.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_feed_level1_dm(vscp_frmw2_firmware_context_t *pctx, const vscp_event_t *pev);

/*!
  @brief Feed the level II decision matrix with one Event

  @param pctx Pointer to context.
  @param pex Event to feed the DM with.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/

int
vscp_frmw2_feed_level2_dm(vscp_frmw2_firmware_context_t *pctx, const vscp_event_t *pev);

/*!
  @brief Feed the decision matrix with one Event

  @param pctx Pointer to context.
  @param pex Event to feed the DM with.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp_frmw2_feed_dm(vscp_frmw2_firmware_context_t *pctx, const vscp_event_t *pev);

/*!
  @brief Send DM info on request

  @param pctx Pointer to context.
  @return VSCP_ERROR_SUCCESS on success, or error code.
*/
int
vscp_frmw2_report_dmatrix(vscp_frmw2_firmware_context_t *pctx);

/**
@}
*/

#ifdef __cplusplus
}
#endif

#endif // __VSCP_FIRMWARE_LEVEL2_H
