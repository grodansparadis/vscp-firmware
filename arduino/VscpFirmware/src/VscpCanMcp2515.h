// VscpCanMcp2515.h - CAN4VSCP over an MCP2515 CAN controller (mcp_can library)
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

#ifndef VSCP_CAN_MCP2515_H
#define VSCP_CAN_MCP2515_H

#include <mcp_can.h> // "mcp_can" by coryjfowler (Library Manager)

#include "VscpTransport.h"

/*!
  CAN4VSCP (VSCP Level I over CAN, extended frames, 125 kbit/s) with an
  MCP2515 CAN controller: Arduino CAN-BUS shields and the common
  MCP2515 + TJA1050 modules. Needs the "mcp_can" library by coryjfowler.

  \code
  VscpCanMcp2515 can(10, MCP_8MHZ);   // standalone module: CS D10, 8 MHz crystal
  VscpCanMcp2515 can(9, MCP_16MHZ);   // Arduino CAN-BUS shield: CS D9, 16 MHz
  \endcode
*/
class VscpCanMcp2515 : public VscpTransport {
public:
  /*!
    @param csPin SPI chip select pin of the MCP2515.
    @param crystal MCP_8MHZ, MCP_16MHZ or MCP_20MHZ (the crystal on the board).
    @param speed CAN bit rate, CAN_125KBPS for CAN4VSCP.
  */
  VscpCanMcp2515(uint8_t csPin, uint8_t crystal = MCP_16MHZ, uint8_t speed = CAN_125KBPS);

  bool begin() override;
  int send(const vscp_event_t &ev) override;
  bool receive(vscp_event_t &ev, uint16_t bufsize) override;

  /// The mcp_can object (for filters, error counters ...)
  MCP_CAN &can() { return m_can; }

private:
  MCP_CAN m_can;
  uint8_t m_crystal;
  uint8_t m_speed;
};

#endif
