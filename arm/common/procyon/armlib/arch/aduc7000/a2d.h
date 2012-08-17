/*! \file a2d.h \brief Analog-to-digital Converter Driver for ADuC7000. */
//*****************************************************************************
//
// File Name	: 'a2d.h'
// Title		: Analog-to-digital Converter Driver for ADuC7000
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

#ifndef A2D_H
#define A2D_H

#include "global.h"
#include "aduc7026.h"

// defines

// functions

//! initializes uart
void a2dInit(void);

//! start conversion on [channel] and return result
int a2dConvert(int channel);

#endif
