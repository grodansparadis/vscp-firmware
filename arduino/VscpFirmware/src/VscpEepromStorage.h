// VscpEepromStorage.h - VscpNode storage in the Arduino EEPROM
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

#ifndef VSCP_EEPROM_STORAGE_H
#define VSCP_EEPROM_STORAGE_H

#include <EEPROM.h>

#include "VscpStorage.h"

/*!
  Storage in the EEPROM of AVR boards (Uno, Nano, Mega ...) or the EEPROM
  emulation of ESP32, ESP8266, RP2040 and STM32 boards (in flash).
*/
class VscpEepromStorage : public VscpStorage {
public:
  bool begin(uint16_t size) override;
  uint8_t read(uint16_t address) override;
  void write(uint16_t address, uint8_t value) override;
  void commit() override;
};

#endif
