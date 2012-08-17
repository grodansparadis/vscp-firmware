/*! \file uart.h \brief UART driver for ARM ADuC7000 16450. */
//*****************************************************************************
//
// File Name	: 'uart.h'
// Title		: UART driver for ARM ADuC7000 16450
// Author		: Pascal Stang - Copyright (C) 2005
// Created		: 2/5/2005
// Revised		: 2/5/2005
// Version		: 0.1
// Target MCU	: Analog Devices ARM ADuC7000 Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef UART_H
#define UART_H

#include "global.h"
//#include "buffer.h"
#include "aduc7026.h"

// defines

// UART_BAUD calculates a division rate given a baud rate
// for use with uartInit()
// example: uartInit(UART_BAUD(115200), UART_8N1);
//
// BEWARE: additional code will be generated if 'baud' is not a contant
#define UART_BAUD(baud) (uint16_t)((PCLK+baud*8L)/(baud*16))

// Definitions for UART mode settings (databits, parity, stopbits)
#define UART_8N1
#define UART_7N1
#define UART_8N2
#define UART_7N2
#define UART_8E1
#define UART_7E1
#define UART_8E2
#define UART_7E2
#define UART_8O1
#define UART_7O1
#define UART_8O2
#define UART_7O2

// functions

//! initializes uart
void uartInit(uint16_t baud, uint32_t mode);

//! sends a single byte over the uart
int uartSendByte(int data);

//! gets a single byte from the uart receive buffer
int uartGetByte(void);


#endif
