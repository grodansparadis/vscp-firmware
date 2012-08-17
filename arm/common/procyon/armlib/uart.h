/*! \file uart.h \brief UART driver for ARM LPC210x 16550. */
//*****************************************************************************
//
// File Name	: 'uart.h'
// Title		: UART driver for ARM LPC210x 16550
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 4/2/2004
// Revised		: 4/2/2004
// Version		: 0.1
// Target MCU	: Philips ARM LPC210x Series
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
#include "LPC210x.h"

// defines

// UART_BAUD calculates a division rate given a baud rate
// for use with uart0Init() and uart1Init()
// example: uart0Init(UART_BAUD(115200), UART_8N1, UART_FIFO_8);
//
// BEWARE: additional code will be generated if 'baud' is not a contant
#define UART_BAUD(baud) (uint16_t)((PCLK+baud*8L)/(baud*16))

// Definitions for UART mode settings (databits, parity, stopbits)
#define UART_8N1      (uint8_t)(ULCR_CHAR_8 + ULCR_PAR_NO   + ULCR_STOP_1)
#define UART_7N1      (uint8_t)(ULCR_CHAR_7 + ULCR_PAR_NO   + ULCR_STOP_1)
#define UART_8N2      (uint8_t)(ULCR_CHAR_8 + ULCR_PAR_NO   + ULCR_STOP_2)
#define UART_7N2      (uint8_t)(ULCR_CHAR_7 + ULCR_PAR_NO   + ULCR_STOP_2)
#define UART_8E1      (uint8_t)(ULCR_CHAR_8 + ULCR_PAR_EVEN + ULCR_STOP_1)
#define UART_7E1      (uint8_t)(ULCR_CHAR_7 + ULCR_PAR_EVEN + ULCR_STOP_1)
#define UART_8E2      (uint8_t)(ULCR_CHAR_8 + ULCR_PAR_EVEN + ULCR_STOP_2)
#define UART_7E2      (uint8_t)(ULCR_CHAR_7 + ULCR_PAR_EVEN + ULCR_STOP_2)
#define UART_8O1      (uint8_t)(ULCR_CHAR_8 + ULCR_PAR_ODD  + ULCR_STOP_1)
#define UART_7O1      (uint8_t)(ULCR_CHAR_7 + ULCR_PAR_ODD  + ULCR_STOP_1)
#define UART_8O2      (uint8_t)(ULCR_CHAR_8 + ULCR_PAR_ODD  + ULCR_STOP_2)
#define UART_7O2      (uint8_t)(ULCR_CHAR_7 + ULCR_PAR_ODD  + ULCR_STOP_2)

// Definitions for typical UART fifo mode settings
#define UART_FIFO_OFF (0x00)
#define UART_FIFO_1   (uint8_t)(UFCR_FIFO_ENABLE + UFCR_FIFO_TRIG1)
#define UART_FIFO_4   (uint8_t)(UFCR_FIFO_ENABLE + UFCR_FIFO_TRIG4)
#define UART_FIFO_8   (uint8_t)(UFCR_FIFO_ENABLE + UFCR_FIFO_TRIG8)
#define UART_FIFO_14  (uint8_t)(UFCR_FIFO_ENABLE + UFCR_FIFO_TRIG14)


// functions

//! initializes transmit and receive buffers
// called from uartInit()
//void uartInitBuffers(void);

//! initializes uart
void uart0Init(uint16_t baud, uint8_t mode, uint8_t fifomode);
void uart1Init(uint16_t baud, uint8_t mode, uint8_t fifomode);

//! sends a single byte over the uart
int uart0SendByte(int data);
int uart1SendByte(int data);

//! gets a single byte from the uart receive buffer
int uart0GetByte(void);
int uart1GetByte(void);


#endif
