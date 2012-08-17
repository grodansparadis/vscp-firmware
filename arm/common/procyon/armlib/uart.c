/*! \file uart.c \brief UART driver for ARM LPC210x 16550. */
//*****************************************************************************
//
// File Name	: 'uart.c'
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

#include <limits.h>
#include "LPC210x.h"

#include "global.h"
#include "uart.h"

//! enable and initialize the uart
void uart0Init(uint16_t baud, uint8_t mode, uint8_t fifomode)
{
	// set port pins for UART0
	PINSEL0 = (PINSEL0 & ~U0_PINMASK) | U0_PINSEL;

	U0IER = 0x00;                         // disable all interrupts
	U0IIR;                                // clear interrupt ID
	U0RBR;                                // clear receive register
	U0LSR;                                // clear line status register

	// set the baudrate
	U0LCR = ULCR_DLAB_ENABLE;             // select divisor latches 
	U0DLL = (uint8_t)baud;                // set for baud low byte
	U0DLM = (uint8_t)(baud >> 8);         // set for baud high byte

	// set the number of characters and other
	// user specified operating parameters
	U0LCR = (mode & ~ULCR_DLAB_ENABLE);
	U0FCR = fifomode;
}

int uart0SendByte(int data)
{
	while(!(U0LSR & ULSR_THRE))		// wait for TX buffer to empty
		continue;						// also either WDOG() or swap()

	U0THR = (uint8_t)data;
	return (uint8_t)data;
}

int uart0GetByte(void)
{
	if(U0LSR & ULSR_RDR)				// check if character is available
		return U0RBR;					// return character
	return -1;
}

void uart1Init(uint16_t baud, uint8_t mode, uint8_t fifomode)
{
  // set port pins for UART1
  PINSEL0 = (PINSEL0 & ~U1_PINMASK) | U1_PINSEL;

  U1IER = 0x00;                         // disable all interrupts
  U1IIR;                                // clear interrupt ID
  U1RBR;                                // clear receive register
  U1LSR;                                // clear line status register

  // set the baudrate
  U1LCR = ULCR_DLAB_ENABLE;             // select divisor latches 
  U1DLL = (uint8_t)baud;                // set for baud low byte
  U1DLM = (uint8_t)(baud >> 8);         // set for baud high byte

  // set the number of characters and other
  // user specified operating parameters
  U1LCR = (mode & ~ULCR_DLAB_ENABLE);
  U1FCR = fifomode;
}

int uart1SendByte(int data)
{
	while(!(U1LSR & ULSR_THRE))		// wait for TX buffer to empty
		continue;						// also either WDOG() or swap()
	U1THR = (uint8_t)data;
	return (uint8_t)data;
}

int uart1GetByte(void)
{	
	if(U1LSR & ULSR_RDR)				// check if character is available
		return U1RBR;					// return character
	return -1;
}
