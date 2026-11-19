// VscpStorage.h - persistent storage used by VscpNode
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

#ifndef VSCP_STORAGE_H
#define VSCP_STORAGE_H

#include <stdint.h>

/*!
  Byte wide persistent storage for the node configuration (nickname, user
  id, registers, decision matrix). VscpEepromStorage uses the Arduino EEPROM
  library. Implement this interface for other storage (FRAM, flash ...).
*/
class VscpStorage {
public:
  virtual ~VscpStorage() {}

  /*!
    Prepare the storage. size is the number of bytes VscpNode needs from
    its base address.
  */
  virtual bool begin(uint16_t size) { (void) size; return true; }

  /// Read one byte
  virtual uint8_t read(uint16_t address) = 0;

  /// Write one byte (only when it differs, to save write cycles)
  virtual void write(uint16_t address, uint8_t value) = 0;

  /// Make written bytes permanent (needed on boards that emulate EEPROM in flash)
  virtual void commit() {}
};

#endif
