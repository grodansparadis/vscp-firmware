// VscpNode.h - a VSCP node for Arduino
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

#ifndef VSCP_NODE_H
#define VSCP_NODE_H

#include <stdint.h>

#include "VscpStorage.h"
#include "VscpTransport.h"

#include "vscp-class.h"
#include "vscp-firmware-level2.h"
#include "vscp-type.h"
#include "vscp.h"

/*!
  Called when a register is read. Return true and set value to answer it,
  false to let VscpNode answer (from the register array or the decision
  matrix). Use it for live values (sensor readings, counters, status).
*/
typedef bool (*VscpRegisterReadCallback)(uint16_t page, uint32_t offset, uint8_t &value);

/*!
  Called when a register is written over the bus, before the value is
  stored. Return false to refuse the write (read only register).
  For a register in the register array the new value is stored (and saved)
  after the callback returns true.
*/
typedef bool (*VscpRegisterWriteCallback)(uint16_t page, uint32_t offset, uint8_t value);

/// Called when a decision matrix row matches a received event
typedef void (*VscpActionCallback)(uint8_t action, uint8_t param, const vscp_event_t &ev);

/// Called for every received event (after the node has handled it)
typedef void (*VscpEventCallback)(const vscp_event_t &ev);

/// Called on "restore defaults" (register 0xA2 written 0x55, then 0xAA)
typedef void (*VscpNotifyCallback)();

/*!
  A VSCP Level I node (CAN4VSCP) built on vscp-firmware-level2.

  VscpNode does the VSCP protocol: nickname discovery, heartbeats, the
  standard registers, register read/write, the decision matrix and saving
  the node configuration. The sketch only handles its own I/O.

  \code
  VscpCanMcp2515 can(10, MCP_8MHZ);   // CS pin, crystal of the CAN module
  VscpEepromStorage storage;
  VscpNode node(can, &storage);
  uint8_t regs[4];

  void setup() {
    node.setRegisters(regs, sizeof(regs));
    node.begin();
  }

  void loop() {
    node.loop();
  }
  \endcode
*/
class VscpNode {
public:
  /*!
    @param transport The bus (for example VscpCanMcp2515).
    @param storage Where the configuration is saved (for example
           VscpEepromStorage). Without storage nothing is saved and the node
           gets a new nickname after every start.
    @param storageBase First storage address used by the node. The node
           uses storageSize() bytes from here.
  */
  explicit VscpNode(VscpTransport &transport, VscpStorage *storage = nullptr, uint16_t storageBase = 0);

  // ---------------------------------------------------------------------
  //                 Configuration (call before begin())
  // ---------------------------------------------------------------------

  /// Set the whole GUID (16 bytes, MSB first)
  void setGuid(const uint8_t guid[16]);

  /*!
    Set the first 14 bytes of the GUID. The last two bytes are chosen at
    random the first time the node starts and are saved. Without a GUID the
    prefix FF:FF:FF:FF:FF:FF:FF:FD:00:... (for development) is used.
  */
  void setGuidPrefix(const uint8_t prefix[14]);

  /// URL of the module description file (max 32 characters, no "http://")
  void setMdfUrl(const char *url);

  /// Firmware version reported in the standard registers
  void setFirmwareVersion(uint8_t major, uint8_t minor, uint8_t subMinor);

  /// Manufacturer id and sub id (standard registers 0x89-0x90)
  void setManufacturerId(uint32_t id, uint32_t subId);

  /// Standard device family and type (standard registers 0xD0-0xD7)
  void setDeviceFamily(uint32_t family, uint32_t type);

  /// Heartbeat interval in milliseconds (default 60000, 0 = no heartbeats)
  void setHeartbeatInterval(uint32_t ms);

  /*!
    Application registers, page 0 from offset 0. The values are saved and
    restored at start. Optional defaults are used the first time and on
    "restore defaults" (otherwise 0).
    @param regs Array that holds the register values (owned by the sketch).
    @param count Number of registers (max 128 on page 0 for Level I).
    @param defaults Default values (count bytes) or nullptr.
  */
  void setRegisters(uint8_t *regs, uint16_t count, const uint8_t *defaults = nullptr);

  /*!
    Level I decision matrix with rows of eight bytes. Saved and restored.
    @param dm Array of rows * 8 bytes (owned by the sketch).
    @param rows Number of rows.
    @param page Register page of the matrix.
    @param offset Register offset of the first byte.
  */
  void setDecisionMatrix(uint8_t *dm, uint8_t rows, uint16_t page = 0, uint32_t offset = 0x60);

  /*!
    Registers that hold the zone and subzone of the node (as in most MDF
    files, registers 0 and 1). They are used in the events the node sends
    and in sendZoneEvent(). -1 = not used (zone/subzone 0).
  */
  void setZoneRegisters(int zoneRegister, int subzoneRegister);

  void onRegisterRead(VscpRegisterReadCallback cb) { m_cbRead = cb; }
  void onRegisterWrite(VscpRegisterWriteCallback cb) { m_cbWrite = cb; }
  void onAction(VscpActionCallback cb) { m_cbAction = cb; }
  void onEvent(VscpEventCallback cb) { m_cbEvent = cb; }
  void onRestoreDefaults(VscpNotifyCallback cb) { m_cbRestore = cb; }
  /// Called on "enter boot loader" (default: the board is reset)
  void onBootloader(VscpNotifyCallback cb) { m_cbBootloader = cb; }

  // ---------------------------------------------------------------------
  //                              Running
  // ---------------------------------------------------------------------

  /*!
    Start the transport, load the saved configuration and start the
    protocol (nickname discovery if the node has no nickname).
    @return true on success.
  */
  bool begin();

  /// Call often from loop(). Receives events and runs the protocol.
  void loop();

  /*!
    Send an event. Only possible when the node has a nickname (isActive()).
    @param vscpClass Level I class (0-511).
    @param vscpType Type (0-255).
    @param data Data bytes (max 8) or nullptr.
    @param len Number of data bytes.
    @param priority 0 (highest) - 7 (lowest), default 3.
    @return VSCP_ERROR_SUCCESS or a VSCP error code.
  */
  int sendEvent(uint16_t vscpClass, uint8_t vscpType, const uint8_t *data = nullptr, uint8_t len = 0,
                uint8_t priority = 3);

  /*!
    Send an event with the common data layout index, zone, subzone
    (CLASS1.INFORMATION ON/OFF, CLASS1.CONTROL TURN-ON ...).
  */
  int sendZoneEvent(uint16_t vscpClass, uint8_t vscpType, uint8_t index = 0, uint8_t priority = 3);

  /*!
    Set a register in the register array and save it (for values the
    sketch changes itself).
  */
  void setRegister(uint16_t offset, uint8_t value);

  /// Value of a register in the register array (0 if outside it)
  uint8_t getRegister(uint16_t offset) const;

  /// Forget the nickname and find a new one (the "init button" of a node)
  void startNicknameDiscovery();

  /// Restore the register defaults and clear the decision matrix
  void restoreDefaults();

  /// True when the node has a nickname and takes part on the bus
  bool isActive() const;

  /// FRMW2_STATE_NONE/PROBE/PREACTIVE/ACTIVE/ERROR
  uint8_t state() const { return m_ctx.state; }

  /// Name of the current state ("ACTIVE" ...)
  const char *stateName() const;

  /// Nickname (0-0xFE), 0xFF/0xFFFF if none
  uint16_t nickname() const { return m_ctx.nickname; }

  uint8_t zone() const;
  uint8_t subzone() const;

  /// Number of storage bytes the node uses from its base address
  uint16_t storageSize() const;

  /// The framework context, for things this class does not cover
  vscp_frmw2_firmware_context_t &context() { return m_ctx; }

private:
  static uint32_t cbGetMilliseconds(vscp_frmw2_firmware_context_t *pctx);
  static uint64_t cbGetTimestamp(vscp_frmw2_firmware_context_t *pctx);
  static int cbSendEvent(vscp_frmw2_firmware_context_t *pctx, vscp_event_t *pev);
  static int cbDmAction(vscp_frmw2_firmware_context_t *pctx, const vscp_event_t *pev, uint8_t action,
                        const uint8_t *pparam);
  static int cbSegmentCtrlHeartbeat(vscp_frmw2_firmware_context_t *pctx, uint16_t segcrc, uint32_t time);
  static int cbReportEventsOfInterest(vscp_frmw2_firmware_context_t *pctx);
  static int cbSetEventTime(vscp_frmw2_firmware_context_t *pctx, vscp_event_t *pev);
  static int cbGetIpAddr(vscp_frmw2_firmware_context_t *pctx, uint8_t *ip, uint8_t size);
  static int cbReadReg(vscp_frmw2_firmware_context_t *pctx, uint16_t page, uint32_t reg, uint8_t *pval);
  static int cbWriteReg(vscp_frmw2_firmware_context_t *pctx, uint16_t page, uint32_t reg, uint8_t val);
  static int cbStdregChange(vscp_frmw2_firmware_context_t *pctx, uint32_t stdreg);
  static int cbRestoreDefaults(vscp_frmw2_firmware_context_t *pctx);
  static void cbEnterBootloader(vscp_frmw2_firmware_context_t *pctx);
  static void cbReset(vscp_frmw2_firmware_context_t *pctx);
  static void cbFeedWatchdog(vscp_frmw2_firmware_context_t *pctx);

  static const vscp_frmw2_ops_t s_ops;

  void loadStorage();
  void saveNickname();
  void saveUserId();
  void saveRegister(uint16_t offset);
  void saveDmByte(uint16_t index);
  void syncZone();
  bool isDmRegister(uint16_t page, uint32_t reg) const;

  VscpTransport &m_transport;
  VscpStorage *m_storage;
  uint16_t m_storageBase;

  vscp_frmw2_firmware_context_t m_ctx;

  uint8_t m_guid[16];
  bool m_bFullGuid;

  uint8_t *m_regs;
  const uint8_t *m_regDefaults;
  uint16_t m_regCount;

  uint8_t *m_dm;
  uint8_t m_dmRows;

  int m_zoneReg;
  int m_subzoneReg;

  uint8_t m_rxData[8];

  VscpRegisterReadCallback m_cbRead;
  VscpRegisterWriteCallback m_cbWrite;
  VscpActionCallback m_cbAction;
  VscpEventCallback m_cbEvent;
  VscpNotifyCallback m_cbRestore;
  VscpNotifyCallback m_cbBootloader;
};

#endif
