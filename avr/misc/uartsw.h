/*! \file uartsw.h \brief Software Interrupt-driven UART function library. */
//*****************************************************************************
//
// File Name	: 'uartsw.c'
// Title		: Software Interrupt-driven UART function library
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 7/20/2002
// Revised		: 6/03/2003
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// Description :
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef UARTSW_H
#define UARTSW_H

#include "global.h"
#include "buffer.h"

// constants/macros/typdefs
#ifndef UARTSW_RX_BUFFER_SIZE
#define UARTSW_RX_BUFFER_SIZE		0x20
#endif

// functions

//! enable and initialize the software uart
void uartswInit(void);
//! create and initialize the uart buffers
void uartswInitBuffers(void);
//! turns off software UART
void uartswOff(void);
//! returns the receive buffer structure 
cBuffer* uartswGetRxBuffer(void);

void uartswSetBaudRate(u32 baudrate);
void uartswSendByte(u08 data);
u08 uartswReceiveByte(u08* rxData);

void uartswTxBitService(void);
void uartswRxBitService(void);

#endif
