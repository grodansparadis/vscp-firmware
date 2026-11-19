// VscpTransport.h - interface between VscpNode and the bus hardware
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

#ifndef VSCP_TRANSPORT_H
#define VSCP_TRANSPORT_H

#include <stdint.h>

#include "vscp.h"

/*!
  A transport moves VSCP events between a VscpNode and the bus.

  VscpCanMcp2515 (MCP2515 CAN controller, CAN4VSCP) and VscpCanTwai (the
  CAN controller in the ESP32) are included. Implement this interface to use
  something else (another CAN controller, RS-485, a radio ...).

  For CAN4VSCP (Level I) an event is one extended CAN frame:
    id bits 28-26 priority, 25 hard coded, 24-16 class (0-511),
    15-8 type, 7-0 nickname of the sender, data 0-8 bytes.
*/
class VscpTransport {
public:
  virtual ~VscpTransport() {}

  /*!
    Start the hardware. Called by VscpNode::begin().
    @return true on success.
  */
  virtual bool begin() = 0;

  /*!
    Send an event. For Level I the nickname of the sender is in
    ev.GUID[15] and the priority in bits 7-5 of ev.head.
    @return VSCP_ERROR_SUCCESS or a VSCP error code.
  */
  virtual int send(const vscp_event_t &ev) = 0;

  /*!
    Receive one event if one is waiting. Never blocks.
    @param ev Event to fill in. ev.pdata points to a buffer of bufsize bytes
              that the data is copied to. For Level I the nickname of the
              sender is put in ev.GUID[15].
    @param bufsize Size of the data buffer.
    @return true if an event was received.
  */
  virtual bool receive(vscp_event_t &ev, uint16_t bufsize) = 0;
};

#endif
