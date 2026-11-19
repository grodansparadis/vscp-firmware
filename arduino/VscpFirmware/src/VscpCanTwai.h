// VscpCanTwai.h - CAN4VSCP with the CAN (TWAI) controller of the ESP32
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

#ifndef VSCP_CAN_TWAI_H
#define VSCP_CAN_TWAI_H

#if defined(ESP32)

#include <driver/gpio.h>

#include "VscpTransport.h"

/*!
  CAN4VSCP (VSCP Level I over CAN, 125 kbit/s) with the CAN controller built
  into the ESP32 (called TWAI by Espressif). Only a CAN transceiver
  (SN65HVD230, TJA1051 ...) is needed between the ESP32 and the bus.

  \code
  VscpCanTwai can(GPIO_NUM_5, GPIO_NUM_4);   // TX pin, RX pin
  \endcode
*/
class VscpCanTwai : public VscpTransport {
public:
  /*!
    @param txPin GPIO connected to the transceiver's TX (CTX) input.
    @param rxPin GPIO connected to the transceiver's RX (CRX) output.
  */
  VscpCanTwai(gpio_num_t txPin, gpio_num_t rxPin);

  bool begin() override;
  int send(const vscp_event_t &ev) override;
  bool receive(vscp_event_t &ev, uint16_t bufsize) override;

private:
  gpio_num_t m_txPin;
  gpio_num_t m_rxPin;
};

#endif
#endif
