/*! \file avrlibdefs.h \brief AVRlib global defines and macros. */
//*****************************************************************************
//
// File Name	: 'avrlibdefs.h'
// Title		: AVRlib global defines and macros include file
// Author		: Pascal Stang
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//	Description : This include file is designed to contain items useful to all
//					code files and projects, regardless of specific implementation.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#ifndef AVRLIBDEFS_H
#define AVRLIBDEFS_H

// AVR-GCC compiler compatibility
// avr-gcc compiler v3.1.x and older doesn't support outb() and inb()
//		if necessary, convert outb and inb to outp and inp
#ifndef outb
	#define	outb(addr, data)	outp(data, addr)
#endif
#ifndef inb
	#define	inb(addr)			inp(addr)
#endif

// support for individual port pin naming in the mega128
// see port128.h for details
#ifdef __AVR_ATmega128__
	#include "port128.h"
#endif

// port address helpers
#define DDR(x) ((x)-1)    // address of data direction register of port x
#define PIN(x) ((x)-2)    // address of input register of port x

// MIN/MAX/ABS macros
#define MIN(a,b)			((a<b)?(a):(b))
#define MAX(a,b)			((a>b)?(a):(b))
#define ABS(x)				((x>0)?(x):(-x))

// macro for reading 16bit words from program memory
#define PRG_RDW(a)		( (PRG_RDB((unsigned char*)(a)) & 0x00FF) | ((PRG_RDB((unsigned char*)(a)+1))<<8) )

// constants
#define PI		3.14159265359

#endif
