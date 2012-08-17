/*! \file rprintfconf.h \brief printf library configuration. */
//****************************************************************************
//
// File Name	: 'rprintfconf.h'
// Title		: printf library configruation
// Author		: Pascal Stang - Copyright (C) 2000-2003
// Created		: 2000.12.26
// Revised		: 2003.5.01
// Version		: 0.8
// Target MCU	: Atmel AVR series and other targets
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//****************************************************************************

#ifndef RPRINTFCONF_H
#define RPRINTFCONF_H

// select a printf function

//! RPRINTF_SIMPLE will compile a smaller, simpler, and faster printf() function
#define RPRINTF_SIMPLE

//! RPRINTF_COMPLEX will compile a larger, more capable, and slower printf() function
//#define RPRINTF_COMPLEX

// Enable/disable the floating-point printf function: rprintfFloat()
// (adds +4600bytes or 2.2Kwords of code)
//#define RPRINTF_FLOAT

#endif
