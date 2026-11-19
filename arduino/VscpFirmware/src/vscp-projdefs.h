// vscp-projdefs.h - project settings for vscp-firmware on Arduino
//
// The vscp-firmware sources include this file. Binary frames, encryption,
// JSON and XML support in vscp-firmware-helper.c are left out so the code
// fits small boards (an Arduino Uno has 32 KB flash and 2 KB RAM).
//
// The MIT License (MIT)
// Copyright (c) 2000-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
// This file is part of VSCP - Very Simple Control Protocol - https://www.vscp.org
// (See LICENSE text in the vscp-firmware repository.)

#ifndef VSCP_PROJDEFS_H
#define VSCP_PROJDEFS_H

// GUID and manufacturer id are not writable over the bus
// #define THIS_FIRMWARE_ENABLE_WRITE_2PROTECTED_LOCATIONS

#endif
