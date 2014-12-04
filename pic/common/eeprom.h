///////////////////////////////////////////////////////////////////////////////
// EEPROM access routines
//
// Code collected from the Microchip C18 forum
// Copyright (C) 2003 JasonK
//
// Copyright (C) 2005-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
//

#ifndef EEPROM_H
#define EEPROM_H

#include <inttypes.h>

// define for string read/write support
#define EEPROM_STRING

// define for Unsigned long read/write support
#define EEPROM_UL

// Prototypes

uint8_t readEEPROM( uint16_t address );
void writeEEPROM( uint16_t address, uint8_t data );

#ifdef EEPROM_UL
uint32_t readULongEEPROM( uint16_t address );
void writeULongEEPROM( uint16_t address, uint8_t data );
#endif

#ifdef EEPROM_STRING
void readStringEEPROM( uint16_t address, 
                            uint8_t string[],
                            uint8_t final_character );
void writeStringEEPROM( uint16_t address, 
                            uint8_t string[] ) ;
#endif


#endif
