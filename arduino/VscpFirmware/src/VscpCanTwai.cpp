// VscpCanTwai.cpp - CAN4VSCP with the CAN (TWAI) controller of the ESP32
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

#if defined(ESP32)

#include <Arduino.h>
#include <driver/twai.h>

#include "VscpCan4Vscp.h"
#include "VscpCanTwai.h"

VscpCanTwai::VscpCanTwai(gpio_num_t txPin, gpio_num_t rxPin)
  : m_txPin(txPin)
  , m_rxPin(rxPin)
{
}

bool
VscpCanTwai::begin()
{
  twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(m_txPin, m_rxPin, TWAI_MODE_NORMAL);
  twai_timing_config_t t  = TWAI_TIMING_CONFIG_125KBITS();
  twai_filter_config_t f  = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  if (ESP_OK != twai_driver_install(&g, &t, &f)) {
    return false;
  }
  return (ESP_OK == twai_start());
}

int
VscpCanTwai::send(const vscp_event_t &ev)
{
  if (!vscpCanFits(ev)) {
    return VSCP_ERROR_PARAMETER; // Level II event: not possible on CAN4VSCP
  }
  twai_message_t msg;
  memset(&msg, 0, sizeof(msg));
  msg.extd             = 1;
  msg.identifier       = vscpCanId(ev);
  msg.data_length_code = ev.sizeData;
  if (ev.sizeData) {
    memcpy(msg.data, ev.pdata, ev.sizeData);
  }
  return (ESP_OK == twai_transmit(&msg, pdMS_TO_TICKS(10))) ? VSCP_ERROR_SUCCESS : VSCP_ERROR_TRM_FULL;
}

bool
VscpCanTwai::receive(vscp_event_t &ev, uint16_t bufsize)
{
  // After too many bus errors the controller goes "bus off". Recover and
  // start again so the node comes back on the bus by itself.
  twai_status_info_t status;
  if (ESP_OK == twai_get_status_info(&status)) {
    if (TWAI_STATE_BUS_OFF == status.state) {
      twai_initiate_recovery();
      return false;
    }
    if (TWAI_STATE_STOPPED == status.state) {
      twai_start();
      return false;
    }
  }

  twai_message_t msg;
  if (ESP_OK != twai_receive(&msg, 0)) {
    return false;
  }
  if (!msg.extd || msg.rtr || (msg.data_length_code > 8)) {
    return false; // CAN4VSCP uses extended data frames only
  }
  uint8_t len = msg.data_length_code;
  if (len > bufsize) {
    len = bufsize;
  }
  vscpEventFromCan(ev, msg.identifier, msg.data, len);
  return true;
}

#endif
