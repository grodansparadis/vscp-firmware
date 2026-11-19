// VscpCan4Vscp.h - CAN4VSCP frame id <-> VSCP event (shared by the CAN transports)
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

#ifndef VSCP_CAN4VSCP_H
#define VSCP_CAN4VSCP_H

#include <stdint.h>
#include <string.h>

#include "vscp.h"

/*!
  CAN4VSCP extended frame id (29 bits):
    28-26 priority, 25 hard coded, 24-16 class, 15-8 type, 7-0 nickname
*/
static inline uint32_t
vscpCanId(const vscp_event_t &ev)
{
  return ((uint32_t) ((ev.head >> 5) & 7) << 26) | ((ev.head & VSCP_HEADER_HARD_CODED) ? (1UL << 25) : 0) |
         ((uint32_t) (ev.vscp_class & 0x1ff) << 16) | ((uint32_t) ev.vscp_type << 8) | ev.GUID[15];
}

/// True if the event can be sent as one CAN4VSCP frame
static inline bool
vscpCanFits(const vscp_event_t &ev)
{
  return (ev.vscp_class < 512) && (ev.sizeData <= 8) && ((0 == ev.sizeData) || (nullptr != ev.pdata));
}

/// Fill in an event from a received CAN4VSCP frame (data is copied to ev.pdata)
static inline void
vscpEventFromCan(vscp_event_t &ev, uint32_t id, const uint8_t *data, uint8_t len)
{
  uint8_t *pdata = ev.pdata;
  memset(&ev, 0, sizeof(ev));
  ev.head       = (uint8_t) (((id >> 26) & 7) << 5) | ((id & (1UL << 25)) ? VSCP_HEADER_HARD_CODED : 0);
  ev.year       = 0xffff; // No date/time on CAN
  ev.month      = 0xff;
  ev.vscp_class = (id >> 16) & 0x1ff;
  ev.vscp_type  = (id >> 8) & 0xff;
  ev.GUID[15]   = id & 0xff; // Nickname of the sender
  ev.sizeData   = (len > 8) ? 8 : len;
  ev.pdata      = pdata;
  if (ev.sizeData && (nullptr != pdata)) {
    memcpy(pdata, data, ev.sizeData);
  }
}

#endif
