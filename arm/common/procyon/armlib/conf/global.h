/*! \file global.h \brief ARMlib project global include. */
//*****************************************************************************
//
// File Name	: 'global.h'
// Title		: ARM project global include 
// Author		: Pascal Stang
// Created		: 7/12/2001
// Revised		: 4/29/2004
// Version		: 1.1
// Target MCU	: ARM
// Editor Tabs	: 4
//
//	Description : This include file is designed to contain the specific 
//					configuration for this project.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef GLOBAL_H
#define GLOBAL_H

// global ARMLIB defines
#include "armlibdefs.h"
// global ARMLIB types definitions
#include "armlibtypes.h"

// project/system dependent defines

// declare functions and values from crt0.S & the linker control file
//extern void reset(void);
//extern void exit(void);
//extern void abort(void);
// maybe add interrupt vector addresses

// PLL setup values are computed within the LPC include file
// It relies upon the following defines
#define FOSC				(14745000)		// Master Oscillator Freq.
#define PLL_MUL				(4)				// PLL Multiplier
//#define CCLK				(FOSC*PLL_MUL)	// CPU Clock Freq.

// Pheripheral Bus Speed Divider
#define PBSD				2				// MUST BE 1, 2, or 4
#define PCLK				(CCLK / PBSD)	// Pheripheal Bus Clock Freq.

// Do some value range testing
#if ((FOSC < 10000000) || (FOSC > 25000000))
#error Fosc out of range (10MHz-25MHz)
#error correct and recompile
#endif

#if ((CCLK < 10000000) || (CCLK > 60000000))
//#error cclk out of range (10MHz-60MHz)
//#error correct PLL_MUL and recompile
#endif

//#if ((FCCO < 150000000) || (FCCO > 320000000))
//#error Fcco out of range (156MHz-320MHz)
//#error internal algorithm error
//#endif

#if ((PBSD != 1) && (PBSD != 2) && (PBSD != 4))
#error Pheripheal Bus Speed Divider (PBSD) illegal value (1, 2, or 4)
#endif

#endif
