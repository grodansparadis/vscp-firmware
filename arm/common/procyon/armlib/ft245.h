/*! \file ft245.h \brief FTDI FT245 USB Inferface Driver Library. */
//*****************************************************************************
//
// File Name	: 'ft245.h'
// Title		: FTDI FT245 USB Inferface Driver Library
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

#ifndef FT245_H
#define FT245_H

#include "global.h"
#include "buffer.h"

//#include "ft245conf.h"

// constants/macros/typdefs
#define FT245_RX_BUFFER_SIZE	384

// functions
void ft245Init(void);
void ft245Reset(void);
u08 ft245ReadByte(void);
void ft245WriteByte(u08 data);
u08 ft245CheckRxFifo(void);
u08 ft245CheckTxFifo(void);
void ft245SendByte(u08 data);
cBuffer* ft245GetRxBuffer(void);
#endif
