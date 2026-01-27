///////////////////////////////////////////////////////////////////////////////
// EEPROM access routines
//
// Code collected from the Microchip C18 forum
// Copyright (C) 2003 JasonK
//
// Copyright (C) 2005-2026 Ake Hedman, eurosource, <akhe@eurosource.se>
//

#ifndef EEPROM_H
#define EEPROM_H

#include <typedefs.h>

// Prototypes

uint08_t readEEPROM( uint16_t address );

void writeEEPROM( uint16_t address, uint08_t data );

#endif
