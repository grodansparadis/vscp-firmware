/*! \file ft245conf.h \brief FTDI FT245 USB Inferface Driver Configuration. */
//*****************************************************************************
//
// File Name	: 'ft245conf.h'
// Title		: FTDI FT245 USB Inferface Driver Configuration
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.02.10
// Revised		: 2004.02.19
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

#ifndef FT245CONF_H
#define FT245CONF_H

// FT245 I/O PORT Defines

// Control lines
#define FT245_CTRL_PORT		PORTD
#define FT245_CTRL_DDR		DDRD
#define FT245_CTRL_PORTIN	PIND

#define FT245_RD			PD4
#define FT245_WR			PD5
#define FT245_TXE			PD6
#define FT245_RXF			PD7
#define FT245_RESET			PD0
//#define FT245_SUSPEND		PC4

// Data lines
#define FT245_DATA_PORT		PORTA
#define FT245_DATA_DDR		DDRA
#define FT245_DATA_PORTIN	PINA

#endif
