/*! \file d2a.h \brief Digital-to-Analog Converter Driver for ADuC7000. */
//*****************************************************************************
//
// File Name	: 'd2a.h'
// Title		: Digital-to-Analog Converter Driver for ADuC7000
// Author		: Pascal Stang - Copyright (C) 2005
// Created		: 2/5/2005
// Revised		: 2/5/2005
// Version		: 0.1
// Target MCU	: Analog Devices ADuC7000 ARM Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef D2A_H
#define D2A_H

#include "global.h"
#include "aduc7026.h"

// defines

// functions

//! initializes DACs
void d2aInit(void);

//! set DAC output level
void d2aSet(int channel, int level);

#endif
