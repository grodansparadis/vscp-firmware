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

#include "lpc210x.h"
//#include <avr/io.h>
//#include <avr/signal.h>
//#include <avr/interrupt.h>

#include "global.h"
#include "timer.h"
#include "membus.h"
#include "ft245.h"

// global variables
static char ft245RxData[FT245_RX_BUFFER_SIZE];
cBuffer ft245RxBuffer;

// Functions
void ft245Init(void)
{
#ifdef FT245_PORT_INTERFACE
	// initialize port pins
	// configure input pins
	cbi(FT245_CTRL_DDR, FT245_TXE);
	cbi(FT245_CTRL_DDR, FT245_RXF);
	//cbi(FT245_CTRL_DDR, FT245_SUSPEND);
	// use pullups
	sbi(FT245_CTRL_PORT, FT245_TXE);
	sbi(FT245_CTRL_PORT, FT245_RXF);
	//sbi(FT245_CTRL_PORT, FT245_SUSPEND);

	// configure output pins
	sbi(FT245_CTRL_DDR, FT245_RESET);
	sbi(FT245_CTRL_DDR, FT245_WR);
	sbi(FT245_CTRL_DDR, FT245_RD);
	// preset the outputs
	sbi(FT245_CTRL_PORT, FT245_RESET);
	cbi(FT245_CTRL_PORT, FT245_WR);
	sbi(FT245_CTRL_PORT, FT245_RD);

	// configure data bus
	outb(FT245_DATA_DDR, 0x00);
	outb(FT245_DATA_PORT, 0xFF);
#endif
#ifdef FT245_PORT_INTERFACE
	// initialize the memory bus
	membusInit();
	// configure status pins
	IODIR &= FT245_TXE;
	IODIR &= FT245_RXF;
#endif
	// initialize the RX buffer
	bufferInit(&ft245RxBuffer, ft245RxData, FT245_RX_BUFFER_SIZE);

	// reset the FT245
	//ft245Reset();

}

void ft245Reset(void)
{
#ifdef FT245_PORT_INTERFACE
	// activate reset line
	cbi(FT245_CTRL_PORT, FT245_RESET);
	// hold for 1ms
	timerPause(1);
	// release reset
	sbi(FT245_CTRL_PORT, FT245_RESET);
#endif
#ifdef FT245_MEMBUS_INTERFACE

#endif
}

u08 ft245ReadByte(void)
{
#ifdef FT245_PORT_INTERFACE
	u08 data;
	// data bus set for input
	outb(FT245_DATA_DDR, 0x00);
	// assert RD line
	cbi(FT245_CTRL_PORT, FT245_RD);
	// wait
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	// read data
	data = inb(FT245_DATA_PORTIN);
	// release RD line
	sbi(FT245_CTRL_PORT, FT245_RD);
	// return data
	return data;
#endif
#ifdef FT245_MEMBUS_INTERFACE
	return membusRead(FT245_BASE_ADDRESS);
#endif
}

void ft245WriteByte(u08 data)
{
#ifdef FT245_PORT_INTERFACE
	// wait for transmit fifo to be ready
	while(!ft245CheckTxFifo());
	// assert WR line
	sbi(FT245_CTRL_PORT, FT245_WR);
	// data bus set for output
	outb(FT245_DATA_DDR, 0xFF);
	// write data
	outb(FT245_DATA_PORT, data);
	// wait
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	// release WR line
	cbi(FT245_CTRL_PORT, FT245_WR);
	// return data bus to input + pullups
	outb(FT245_DATA_DDR, 0x00);
	outb(FT245_DATA_PORT, 0xFF);
#endif
#ifdef FT245_MEMBUS_INTERFACE
	return membusWrite(FT245_BASE_ADDRESS, data);
#endif
}

u08 ft245CheckRxFifo(void)
{
	// check state of RXF line
	// RXF = L  => Data in fifo ready to be read
	// RXF = H  => No data, or not ready
#ifdef FT245_PORT_INTERFACE
	if( inb(FT245_CTRL_PORTIN) & (1<<FT245_RXF) )
		return FALSE;	// no data
	else
		return TRUE;	// data is available
#endif
#ifdef FT245_MEMBUS_INTERFACE
	if(IOPIN & FT245_RXF)
		return FALSE;	// no data
	else
		return TRUE;	// data is available
#endif
}

u08 ft245CheckTxFifo(void)
{
	// check state of TXE line
	// TXE = L  => Tx fifo is ready to be written
	// TXE = H  => Tx fifo full or not ready
#ifdef FT245_PORT_INTERFACE
	if( inb(FT245_CTRL_PORTIN) & (1<<FT245_TXE) )
		return FALSE;	// full or not ready
	else
		return TRUE;	// tx fifo space is available
#endif
#ifdef FT245_MEMBUS_INTERFACE
	if(IOPIN & FT245_TXE)
		return FALSE;	// full or not ready
	else
		return TRUE;	// tx fifo space is available
#endif
}

/*
u08 ft245IsSuspended(void)
{
	// check state of SUSPEND line
	// SUSPEND = L  => Data in fifo ready to be read
	// SUSPEND = H  => No data, or not ready
	if( inb(FT245_CTRL_PORTIN) & (1<<FT245_RXF) )
		return FALSE;	// no data
	else
		return TRUE;	// data is available
}
*/

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
