// VscpEepromStorage.cpp - VscpNode storage in the Arduino EEPROM
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

// Only built on boards that have the EEPROM library
#if defined(__has_include)
#if __has_include(<EEPROM.h>)

#include <EEPROM.h>

#include "VscpEepromStorage.h"

// Boards that emulate the EEPROM in flash need begin(size) and commit()
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_RP2040)
#define VSCP_EEPROM_EMULATED 1
#endif

bool
VscpEepromStorage::begin(uint16_t size)
{
#ifdef VSCP_EEPROM_EMULATED
  // At least 512 bytes, so the sketch can use the space after the node
  EEPROM.begin((size < 512) ? 512 : size);
#endif
  (void) size;
  return true;
}

uint8_t
VscpEepromStorage::read(uint16_t address)
{
  return EEPROM.read(address);
}

void
VscpEepromStorage::write(uint16_t address, uint8_t value)
{
  // Only when it differs (EEPROM cells wear out)
  if (EEPROM.read(address) != value) {
    EEPROM.write(address, value);
  }
}

void
VscpEepromStorage::commit()
{
#ifdef VSCP_EEPROM_EMULATED
  EEPROM.commit();
#endif
}

#endif
#endif
