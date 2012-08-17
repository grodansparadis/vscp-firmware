/*! \file nmea.h \brief NMEA protocol function library. */
//*****************************************************************************
//
// File Name	: 'nmea.h'
// Title		: NMEA protocol function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002.08.27
// Revised		: 2002.08.27
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef NMEA_H
#define NMEA_H

#include "global.h"
#include "buffer.h"

// constants/macros/typdefs

// functions
void nmeaInit(void);
u08 nmeaProcess(cBuffer* rxBuffer);
void nmeaProcessGPGGA(u08* packet);
void nmeaProcessGPVTG(u08* packet);

#endif
