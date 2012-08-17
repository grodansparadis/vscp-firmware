/*! \file a2d.h \brief Analog-to-Digital Converter Driver for Atmel AT91. */
//*****************************************************************************
//
// File Name	: 'buffer.h'
// Title		: Analog-to-Digital Converter Driver for Atmel AT91
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 11/5/2005
// Revised		: 4/5/2006
// Version		: 0.1
// Target MCU	: Atmel AT91 series
// Editor Tabs	: 4
//
///	\ingroup driver_arm_at91
/// \defgroup a2d_at91 Analog-to-Digital Converter Driver for Atmel AT91 (armlib/arch/at91/a2d.c)
/// \code #include "a2d.h" \endcode
/// \par Overview
///		To be written...
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
//@{

#ifndef ARMLIB_A2D_H
#define ARMLIB_A2D_H

#include "global.h"
#include "at91sam7s64.h"

void a2dInit(void);
int a2dConvert(int channel);

#endif
//@}
