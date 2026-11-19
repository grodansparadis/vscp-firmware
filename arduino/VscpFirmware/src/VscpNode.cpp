// VscpNode.cpp - a VSCP node for Arduino
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

#include <Arduino.h>

#include <string.h>

#include "VscpNode.h"

#if defined(ARDUINO_ARCH_AVR)
#include <avr/wdt.h>
#endif

// ---------------------------------------------------------------------------
// Storage layout (from the base address)
//
//   0      magic 0x56 ('V')
//   1      layout version
//   2-3    nickname (MSB first, 0xFFFF = none)
//   4-8    user id
//   9-10   GUID bytes 14 and 15 (random, when only a GUID prefix is set)
//   11-12  number of registers (to see if the sketch changed it)
//   13     number of decision matrix rows
//   14...  registers, then the decision matrix
// ---------------------------------------------------------------------------

static const uint8_t STORAGE_MAGIC   = 0x56;
static const uint8_t STORAGE_VERSION = 1;

enum {
  ST_MAGIC    = 0,
  ST_VERSION  = 1,
  ST_NICKNAME = 2,
  ST_USERID   = 4,
  ST_GUID_LO  = 9,
  ST_REGCOUNT = 11,
  ST_DMROWS   = 13,
  ST_DATA     = 14
};

static const uint8_t DM_ROW_SIZE = 8; // Level I decision matrix row

// Development GUID prefix (replace it with a GUID range you own)
static const uint8_t DEFAULT_GUID_PREFIX[14] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD,
                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const vscp_frmw2_ops_t VscpNode::s_ops = {
  VscpNode::cbGetMilliseconds,  VscpNode::cbGetTimestamp,    VscpNode::cbSendEvent,
  VscpNode::cbDmAction,         VscpNode::cbSegmentCtrlHeartbeat,
  VscpNode::cbReportEventsOfInterest,
  VscpNode::cbSetEventTime,     VscpNode::cbGetIpAddr,       VscpNode::cbReadReg,
  VscpNode::cbWriteReg,         VscpNode::cbStdregChange,    VscpNode::cbRestoreDefaults,
  VscpNode::cbEnterBootloader,  VscpNode::cbReset,           VscpNode::cbFeedWatchdog,
};

static inline VscpNode *
nodeOf(vscp_frmw2_firmware_context_t *pctx)
{
  return static_cast<VscpNode *>(pctx->puserdata);
}

// ---------------------------------------------------------------------------
// Construction and configuration
// ---------------------------------------------------------------------------

VscpNode::VscpNode(VscpTransport &transport, VscpStorage *storage, uint16_t storageBase)
  : m_transport(transport)
  , m_storage(storage)
  , m_storageBase(storageBase)
  , m_bFullGuid(false)
  , m_regs(nullptr)
  , m_regDefaults(nullptr)
  , m_regCount(0)
  , m_dm(nullptr)
  , m_dmRows(0)
  , m_zoneReg(-1)
  , m_subzoneReg(-1)
  , m_cbRead(nullptr)
  , m_cbWrite(nullptr)
  , m_cbAction(nullptr)
  , m_cbEvent(nullptr)
  , m_cbRestore(nullptr)
  , m_cbBootloader(nullptr)
{
  memset(&m_ctx, 0, sizeof(m_ctx));
  memcpy(m_guid, DEFAULT_GUID_PREFIX, sizeof(DEFAULT_GUID_PREFIX));
  m_guid[14] = 0;
  m_guid[15] = 0;

  m_ctx.ops                        = &s_ops;
  m_ctx.puserdata                  = this;
  m_ctx.level                      = VSCP_LEVEL1;
  m_ctx.nickname                   = 0xffff;
  m_ctx.probe_timeout              = VSCP_FRMW2_UNASSIGNED;
  m_ctx.probe_timeout_count        = VSCP_FRMW2_UNASSIGNED;
  m_ctx.bInterestedInAllEvents     = 1;
  m_ctx.interval_heartbeat         = 60000UL;
  m_ctx.vscp_major_version         = 1;
  m_ctx.vscp_minor_version         = 4;
  m_ctx.bootloader_algorithm       = 0xFF; // No VSCP boot loader
  m_ctx.firmware_major_version     = 0;
  m_ctx.firmware_minor_version     = 0;
  m_ctx.firmware_sub_minor_version = 1;
}

void
VscpNode::setGuid(const uint8_t guid[16])
{
  memcpy(m_guid, guid, 16);
  m_bFullGuid = true;
}

void
VscpNode::setGuidPrefix(const uint8_t prefix[14])
{
  memcpy(m_guid, prefix, 14);
  m_bFullGuid = false;
}

void
VscpNode::setMdfUrl(const char *url)
{
  memset(m_ctx.mdfurl, 0, sizeof(m_ctx.mdfurl));
  if (nullptr != url) {
    // The MDF URL registers hold 32 characters (no "http://")
    if (0 == strncmp(url, "http://", 7)) {
      url += 7;
    }
    strncpy((char *) m_ctx.mdfurl, url, sizeof(m_ctx.mdfurl));
  }
}

void
VscpNode::setFirmwareVersion(uint8_t major, uint8_t minor, uint8_t subMinor)
{
  m_ctx.firmware_major_version     = major;
  m_ctx.firmware_minor_version     = minor;
  m_ctx.firmware_sub_minor_version = subMinor;
}

void
VscpNode::setManufacturerId(uint32_t id, uint32_t subId)
{
  m_ctx.manufacturerId    = id;
  m_ctx.manufacturerSubId = subId;
}

void
VscpNode::setDeviceFamily(uint32_t family, uint32_t type)
{
  m_ctx.standard_device_family = family;
  m_ctx.standard_device_type   = type;
}

void
VscpNode::setHeartbeatInterval(uint32_t ms)
{
  m_ctx.interval_heartbeat = ms;
}

void
VscpNode::setRegisters(uint8_t *regs, uint16_t count, const uint8_t *defaults)
{
  m_regs        = regs;
  m_regCount    = (nullptr != regs) ? count : 0;
  m_regDefaults = defaults;
}

void
VscpNode::setDecisionMatrix(uint8_t *dm, uint8_t rows, uint16_t page, uint32_t offset)
{
  m_dm     = dm;
  m_dmRows = (nullptr != dm) ? rows : 0;

  // The framework runs the matrix and calls cbDmAction for matching rows
  m_ctx.pDm         = m_dm;
  m_ctx.nDmRows     = m_dmRows;
  m_ctx.sizeDmRow   = DM_ROW_SIZE;
  m_ctx.pageDm      = page;
  m_ctx.regOffsetDm = offset;
}

void
VscpNode::setZoneRegisters(int zoneRegister, int subzoneRegister)
{
  m_zoneReg    = zoneRegister;
  m_subzoneReg = subzoneRegister;
}

uint16_t
VscpNode::storageSize() const
{
  return ST_DATA + m_regCount + (uint16_t) m_dmRows * DM_ROW_SIZE;
}

// ---------------------------------------------------------------------------
// Storage
// ---------------------------------------------------------------------------

void
VscpNode::loadStorage()
{
  memcpy(m_ctx.guid, m_guid, sizeof(m_ctx.guid));

  if (nullptr == m_storage) {
    // Nothing is saved: defaults, no nickname, random end of the GUID
    restoreDefaults();
    if (!m_bFullGuid) {
      m_ctx.guid[14] = random(256);
      m_ctx.guid[15] = random(256);
    }
    return;
  }

  const uint16_t b = m_storageBase;
  bool bValid      = (STORAGE_MAGIC == m_storage->read(b + ST_MAGIC)) &&
                (STORAGE_VERSION == m_storage->read(b + ST_VERSION));
  const bool bLayout = bValid &&
                       (m_regCount == (((uint16_t) m_storage->read(b + ST_REGCOUNT) << 8) |
                                       m_storage->read(b + ST_REGCOUNT + 1))) &&
                       (m_dmRows == m_storage->read(b + ST_DMROWS));

  if (!bValid) {
    // First start: no nickname, no user id, a random end of the GUID
    m_storage->write(b + ST_NICKNAME, 0xff);
    m_storage->write(b + ST_NICKNAME + 1, 0xff);
    for (uint8_t i = 0; i < sizeof(m_ctx.userId); i++) {
      m_storage->write(b + ST_USERID + i, 0);
    }
    m_storage->write(b + ST_GUID_LO, random(256));
    m_storage->write(b + ST_GUID_LO + 1, random(256));
    m_storage->write(b + ST_VERSION, STORAGE_VERSION);
    m_storage->write(b + ST_MAGIC, STORAGE_MAGIC);
  }

  m_ctx.nickname = ((uint16_t) m_storage->read(b + ST_NICKNAME) << 8) | m_storage->read(b + ST_NICKNAME + 1);
  if ((0xffff != m_ctx.nickname) && (m_ctx.nickname > 0xfe)) {
    m_ctx.nickname = 0xffff; // Not a valid Level I nickname: find a new one
  }
  for (uint8_t i = 0; i < sizeof(m_ctx.userId); i++) {
    m_ctx.userId[i] = m_storage->read(b + ST_USERID + i);
  }
  if (!m_bFullGuid) {
    m_ctx.guid[14] = m_storage->read(b + ST_GUID_LO);
    m_ctx.guid[15] = m_storage->read(b + ST_GUID_LO + 1);
  }

  if (bLayout) {
    for (uint16_t i = 0; i < m_regCount; i++) {
      m_regs[i] = m_storage->read(b + ST_DATA + i);
    }
    for (uint16_t i = 0; i < (uint16_t) m_dmRows * DM_ROW_SIZE; i++) {
      m_dm[i] = m_storage->read(b + ST_DATA + m_regCount + i);
    }
    syncZone();
  }
  else {
    // New layout (the sketch has other registers/rows now): defaults
    m_storage->write(b + ST_REGCOUNT, m_regCount >> 8);
    m_storage->write(b + ST_REGCOUNT + 1, m_regCount & 0xff);
    m_storage->write(b + ST_DMROWS, m_dmRows);
    restoreDefaults();
  }

  m_storage->commit();
}

void
VscpNode::saveNickname()
{
  if (nullptr != m_storage) {
    m_storage->write(m_storageBase + ST_NICKNAME, m_ctx.nickname >> 8);
    m_storage->write(m_storageBase + ST_NICKNAME + 1, m_ctx.nickname & 0xff);
    m_storage->commit();
  }
}

void
VscpNode::saveUserId()
{
  if (nullptr != m_storage) {
    for (uint8_t i = 0; i < sizeof(m_ctx.userId); i++) {
      m_storage->write(m_storageBase + ST_USERID + i, m_ctx.userId[i]);
    }
    m_storage->commit();
  }
}

void
VscpNode::saveRegister(uint16_t offset)
{
  if ((nullptr != m_storage) && (offset < m_regCount)) {
    m_storage->write(m_storageBase + ST_DATA + offset, m_regs[offset]);
    m_storage->commit();
  }
}

void
VscpNode::saveDmByte(uint16_t index)
{
  if ((nullptr != m_storage) && (index < (uint16_t) m_dmRows * DM_ROW_SIZE)) {
    m_storage->write(m_storageBase + ST_DATA + m_regCount + index, m_dm[index]);
    m_storage->commit();
  }
}

void
VscpNode::syncZone()
{
  m_ctx.zone    = zone();
  m_ctx.subzone = subzone();
}

// ---------------------------------------------------------------------------
// Running
// ---------------------------------------------------------------------------

bool
VscpNode::begin()
{
  // A random end of the GUID on boards without a hardware RNG
#ifdef A0
  randomSeed(analogRead(A0) ^ micros());
#else
  randomSeed(micros());
#endif

  if ((nullptr != m_storage) && !m_storage->begin(m_storageBase + storageSize())) {
    return false;
  }
  if (!m_transport.begin()) {
    return false;
  }

  loadStorage();

  return (VSCP_ERROR_SUCCESS == vscp_frmw2_init(&m_ctx));
}

void
VscpNode::loop()
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));
  ev.pdata = m_rxData;

  bool bReceived = m_transport.receive(ev, sizeof(m_rxData));
  if (bReceived) {
    ev.pdata = m_rxData;
    if (ev.sizeData > sizeof(m_rxData)) {
      ev.sizeData = sizeof(m_rxData);
    }
  }

  // Protocol, timers (heartbeat) and decision matrix
  vscp_frmw2_work(&m_ctx, bReceived ? &ev : nullptr);

  if (bReceived && (nullptr != m_cbEvent)) {
    m_cbEvent(ev);
  }
}

int
VscpNode::sendEvent(uint16_t vscpClass, uint8_t vscpType, const uint8_t *data, uint8_t len, uint8_t priority)
{
  if (!isActive()) {
    return VSCP_ERROR_NOT_CONNECTED;
  }
  if ((len > 8) || ((len > 0) && (nullptr == data))) {
    return VSCP_ERROR_PARAMETER;
  }

  uint8_t buf[8];
  if (len) {
    memcpy(buf, data, len);
  }

  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));
  ev.head       = (priority & 7) << 5;
  ev.vscp_class = vscpClass;
  ev.vscp_type  = vscpType;
  memcpy(ev.GUID, m_ctx.guid, sizeof(ev.GUID));
  ev.GUID[14] = (m_ctx.nickname >> 8) & 0xff;
  ev.GUID[15] = m_ctx.nickname & 0xff;
  ev.sizeData = len;
  ev.pdata    = buf;
  return m_transport.send(ev);
}

int
VscpNode::sendZoneEvent(uint16_t vscpClass, uint8_t vscpType, uint8_t index, uint8_t priority)
{
  uint8_t data[3] = { index, zone(), subzone() };
  return sendEvent(vscpClass, vscpType, data, sizeof(data), priority);
}

void
VscpNode::setRegister(uint16_t offset, uint8_t value)
{
  if (offset < m_regCount) {
    m_regs[offset] = value;
    saveRegister(offset);
    syncZone();
  }
}

uint8_t
VscpNode::getRegister(uint16_t offset) const
{
  return (offset < m_regCount) ? m_regs[offset] : 0;
}

void
VscpNode::startNicknameDiscovery()
{
  m_ctx.nickname       = 0xffff;
  m_ctx.probe_nickname = 0;
  m_ctx.state          = FRMW2_STATE_NONE;
  m_ctx.substate       = FRMW2_SUBSTATE_NONE;
  saveNickname();
}

void
VscpNode::restoreDefaults()
{
  for (uint16_t i = 0; i < m_regCount; i++) {
    m_regs[i] = (nullptr != m_regDefaults) ? m_regDefaults[i] : 0;
    if (nullptr != m_storage) {
      m_storage->write(m_storageBase + ST_DATA + i, m_regs[i]);
    }
  }
  for (uint16_t i = 0; i < (uint16_t) m_dmRows * DM_ROW_SIZE; i++) {
    m_dm[i] = 0; // All rows disabled
    if (nullptr != m_storage) {
      m_storage->write(m_storageBase + ST_DATA + m_regCount + i, 0);
    }
  }
  if (nullptr != m_storage) {
    m_storage->commit();
  }
  syncZone();
}

bool
VscpNode::isActive() const
{
  return (FRMW2_STATE_ACTIVE == m_ctx.state) && (m_ctx.nickname <= 0xfe);
}

const char *
VscpNode::stateName() const
{
  switch (m_ctx.state) {
    case FRMW2_STATE_NONE:
      return "NONE";
    case FRMW2_STATE_PROBE:
      return "PROBE";
    case FRMW2_STATE_PREACTIVE:
      return "PREACTIVE";
    case FRMW2_STATE_ACTIVE:
      return "ACTIVE";
    case FRMW2_STATE_ERROR:
      return "ERROR";
    default:
      return "?";
  }
}

uint8_t
VscpNode::zone() const
{
  return ((m_zoneReg >= 0) && (m_zoneReg < (int) m_regCount)) ? m_regs[m_zoneReg] : 0;
}

uint8_t
VscpNode::subzone() const
{
  return ((m_subzoneReg >= 0) && (m_subzoneReg < (int) m_regCount)) ? m_regs[m_subzoneReg] : 0;
}

bool
VscpNode::isDmRegister(uint16_t page, uint32_t reg) const
{
  return (nullptr != m_dm) && (page == m_ctx.pageDm) && (reg >= m_ctx.regOffsetDm) &&
         (reg < m_ctx.regOffsetDm + (uint32_t) m_dmRows * DM_ROW_SIZE);
}

// ---------------------------------------------------------------------------
// Framework callbacks
// ---------------------------------------------------------------------------

uint32_t
VscpNode::cbGetMilliseconds(vscp_frmw2_firmware_context_t *)
{
  return millis();
}

uint64_t
VscpNode::cbGetTimestamp(vscp_frmw2_firmware_context_t *)
{
  return micros(); // The framework only uses differences
}

int
VscpNode::cbSendEvent(vscp_frmw2_firmware_context_t *pctx, vscp_event_t *pev)
{
  if (nullptr == pev) {
    return VSCP_ERROR_PARAMETER;
  }
  return nodeOf(pctx)->m_transport.send(*pev);
}

int
VscpNode::cbDmAction(vscp_frmw2_firmware_context_t *pctx, const vscp_event_t *pev, uint8_t action,
                     const uint8_t *pparam)
{
  VscpNode *node = nodeOf(pctx);
  if ((nullptr != node->m_cbAction) && (nullptr != pev)) {
    node->m_cbAction(action, (nullptr != pparam) ? *pparam : 0, *pev);
  }
  return VSCP_ERROR_SUCCESS;
}

int
VscpNode::cbSegmentCtrlHeartbeat(vscp_frmw2_firmware_context_t *, uint16_t, uint32_t)
{
  return VSCP_ERROR_SUCCESS;
}

int
VscpNode::cbReportEventsOfInterest(vscp_frmw2_firmware_context_t *)
{
  return VSCP_ERROR_SUCCESS;
}

int
VscpNode::cbSetEventTime(vscp_frmw2_firmware_context_t *, vscp_event_t *pev)
{
  pev->timestamp_ns = 0; // No wall clock (and not sent on CAN)
  return VSCP_ERROR_SUCCESS;
}

int
VscpNode::cbGetIpAddr(vscp_frmw2_firmware_context_t *, uint8_t *ip, uint8_t size)
{
  memset(ip, 0, size);
  return VSCP_ERROR_SUCCESS;
}

int
VscpNode::cbReadReg(vscp_frmw2_firmware_context_t *pctx, uint16_t page, uint32_t reg, uint8_t *pval)
{
  VscpNode *node = nodeOf(pctx);

  // Live values from the sketch first
  uint8_t value = 0;
  if ((nullptr != node->m_cbRead) && node->m_cbRead(page, reg, value)) {
    *pval = value;
    return VSCP_ERROR_SUCCESS;
  }

  if (node->isDmRegister(page, reg)) {
    *pval = node->m_dm[reg - pctx->regOffsetDm];
    return VSCP_ERROR_SUCCESS;
  }

  if ((0 == page) && (reg < node->m_regCount)) {
    *pval = node->m_regs[reg];
    return VSCP_ERROR_SUCCESS;
  }

  return VSCP_ERROR_INDEX_OOB; // Reads as zero
}

int
VscpNode::cbWriteReg(vscp_frmw2_firmware_context_t *pctx, uint16_t page, uint32_t reg, uint8_t val)
{
  VscpNode *node = nodeOf(pctx);

  // The sketch can refuse (read only register). The answer then holds the
  // unchanged value, as VSCP expects.
  if ((nullptr != node->m_cbWrite) && !node->m_cbWrite(page, reg, val)) {
    return VSCP_ERROR_SUCCESS;
  }

  if (node->isDmRegister(page, reg)) {
    uint16_t idx     = reg - pctx->regOffsetDm;
    node->m_dm[idx] = val;
    node->saveDmByte(idx);
    return VSCP_ERROR_SUCCESS;
  }

  if ((0 == page) && (reg < node->m_regCount)) {
    node->m_regs[reg] = val;
    node->saveRegister(reg);
    node->syncZone();
    return VSCP_ERROR_SUCCESS;
  }

  return (nullptr != node->m_cbWrite) ? VSCP_ERROR_SUCCESS : VSCP_ERROR_INDEX_OOB;
}

int
VscpNode::cbStdregChange(vscp_frmw2_firmware_context_t *pctx, uint32_t stdreg)
{
  VscpNode *node = nodeOf(pctx);
  if ((VSCP_STD_REGISTER_NICKNAME_ID_LSB == stdreg) || (VSCP_STD_REGISTER_NICKNAME_ID_MSB == stdreg)) {
    node->saveNickname();
  }
  else if ((stdreg >= VSCP_STD_REGISTER_USER_ID) && (stdreg < VSCP_STD_REGISTER_USER_ID + 5)) {
    node->saveUserId();
  }
  return VSCP_ERROR_SUCCESS;
}

int
VscpNode::cbRestoreDefaults(vscp_frmw2_firmware_context_t *pctx)
{
  VscpNode *node = nodeOf(pctx);
  node->restoreDefaults();
  if (nullptr != node->m_cbRestore) {
    node->m_cbRestore();
  }
  return VSCP_ERROR_SUCCESS;
}

void
VscpNode::cbEnterBootloader(vscp_frmw2_firmware_context_t *pctx)
{
  VscpNode *node = nodeOf(pctx);
  if (nullptr != node->m_cbBootloader) {
    node->m_cbBootloader();
    return;
  }
  cbReset(pctx); // No CAN boot loader: restart
}

void
VscpNode::cbReset(vscp_frmw2_firmware_context_t *)
{
#if defined(ARDUINO_ARCH_AVR)
  wdt_enable(WDTO_15MS);
  for (;;) {
  }
#elif defined(ESP32) || defined(ESP8266)
  ESP.restart();
#endif
  // Other boards: the reset request is ignored (call the board's reset
  // function from onBootloader() if needed)
}

void
VscpNode::cbFeedWatchdog(vscp_frmw2_firmware_context_t *)
{
}
