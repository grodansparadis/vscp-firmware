// VscpCanMcp2515.cpp - CAN4VSCP over an MCP2515 CAN controller (mcp_can library)
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

// Only built when the mcp_can library is installed
#if defined(__has_include)
#if __has_include(<mcp_can.h>)

#include <Arduino.h>

#include "VscpCan4Vscp.h"
#include "VscpCanMcp2515.h"

VscpCanMcp2515::VscpCanMcp2515(uint8_t csPin, uint8_t crystal, uint8_t speed)
  : m_can(csPin)
  , m_crystal(crystal)
  , m_speed(speed)
{
}

bool
VscpCanMcp2515::begin()
{
  // Accept all frames; the node filters what it needs
  if (CAN_OK != m_can.begin(MCP_ANY, m_speed, m_crystal)) {
    return false;
  }
  m_can.setMode(MCP_NORMAL);
  return true;
}

int
VscpCanMcp2515::send(const vscp_event_t &ev)
{
  if (!vscpCanFits(ev)) {
    return VSCP_ERROR_PARAMETER; // Level II event: not possible on CAN4VSCP
  }
  uint8_t buf[8];
  if (ev.sizeData) {
    memcpy(buf, ev.pdata, ev.sizeData);
  }
  return (CAN_OK == m_can.sendMsgBuf(vscpCanId(ev), 1, ev.sizeData, buf)) ? VSCP_ERROR_SUCCESS
                                                                            : VSCP_ERROR_TRM_FULL;
}

bool
VscpCanMcp2515::receive(vscp_event_t &ev, uint16_t bufsize)
{
  if (CAN_MSGAVAIL != m_can.checkReceive()) {
    return false;
  }

  unsigned long id = 0;
  uint8_t len      = 0;
  uint8_t data[8];
  if (CAN_OK != m_can.readMsgBuf(&id, &len, data)) {
    return false;
  }

  // mcp_can marks extended frames with bit 31 and remote frames with bit 30.
  // CAN4VSCP uses extended data frames only.
  if (!(id & 0x80000000UL) || (id & 0x40000000UL) || (len > 8)) {
    return false;
  }
  if (len > bufsize) {
    len = bufsize;
  }
  vscpEventFromCan(ev, id & 0x1FFFFFFFUL, data, len);
  return true;
}

#endif
#endif
