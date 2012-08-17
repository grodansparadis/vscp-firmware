/*! \file ft245.c \brief FTDI FT245 USB Inferface Driver Library. */
//*****************************************************************************
//
// File Name	: 'ft245.c'
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

#include "lpc2000.h"
//#include <avr/io.h>
//#include <avr/signal.h>
//#include <avr/interrupt.h>

#include "global.h"
#include "timer.h"
#include "ft245.h"

#include "mem.h"

#define FT245_REG_CTRL		(*((volatile u08*)(MEMBANK0+0x006)))
#define FT245_REG_DATA		(*((volatile u08*)(MEMBANK0+0x008)))
#define FT245_REG_CTRL_RXF	1
#define FT245_REG_CTRL_TXE	2

// global variables
static char ft245RxData[FT245_RX_BUFFER_SIZE];
cBuffer ft245RxBuffer;

// Functions
void ft245Init(void)
{
	// initialize the RX buffer
	bufferInit(&ft245RxBuffer, ft245RxData, FT245_RX_BUFFER_SIZE);

	// reset the FT245
	//ft245Reset();
}

u08 ft245ReadByte(void)
{
	return FT245_REG_DATA;
}

void ft245WriteByte(u08 data)
{
	FT245_REG_DATA = data;
}

u08 ft245CheckRxFifo(void)
{
	// check state of RXF line
	// RXF = L  => Data in fifo ready to be read
	// RXF = H  => No data, or not ready
	if(FT245_REG_CTRL & FT245_REG_CTRL_RXF)
		return FALSE;	// no data
	else
		return TRUE;	// data is available
}

u08 ft245CheckTxFifo(void)
{
	// check state of TXE line
	// TXE = L  => Tx fifo is ready to be written
	// TXE = H  => Tx fifo full or not ready
	if(FT245_REG_CTRL & FT245_REG_CTRL_TXE)
		return FALSE;	// full or not ready
	else
		return TRUE;	// tx fifo space is available
}

void ft245SendByte(u08 data)
{
	// wait for FT245 to become ready
	while(!ft245CheckTxFifo());
	// send byte
	ft245WriteByte(data);
}

cBuffer* ft245GetRxBuffer(void)
{
	// since there is an internal buffer in the FT245 chip
	// we don't read in bytes immediately on reception.
	// instead, the FT245 buffer is read in when the 
	// receive buffer is requested

	// read in receive buffer
	while(ft245CheckRxFifo())
	{
		bufferAddToEnd(&ft245RxBuffer, ft245ReadByte());
	}

	return &ft245RxBuffer;
}
