/*! \file uart.c \brief UART driver for ARM LPC2000 16550. */
//*****************************************************************************
//
// File Name	: 'uart.c'
// Title		: UART driver for ARM LPC2000 16550
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 4/2/2004
// Revised		: 4/2/2004
// Version		: 0.1
// Target MCU	: Philips ARM LPC2000 Series
// Editor Tabs	: 4
//
// Thanks to Bill Knight for providing the excellent processor/uart
//  #defines used in this driver.
// 
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <limits.h>
#include "LPC2000.h"

#include "global.h"
#include "uart.h"

//! enable and initialize the uart
void uart0Init(uint16_t baud, uint8_t mode, uint8_t fifomode)
{
	// set pin-select register to connect to UART0
	PINSEL0 = (PINSEL0 & ~U0_PINMASK) | U0_PINSEL;
	// reset and initialize the UART
	U0IER = 0;		// disable all interrupt sources
	U0IIR;			// clear any pending interrupts
	U0FCR = (UFCR_TX_FIFO_RESET|UFCR_RX_FIFO_RESET);	// reset the Tx/Rx FIFOs
	// set the baudrate divisor
	U0LCR = ULCR_DLAB_ENABLE;	// set divisor access bit (permits modification of divisors)
	U0DLL = (baud);				// set the baud division
	U0DLM = (baud>>8);
	U0LCR = 0;					// reset DLAB (normal operation)
	// set line parameters and fifo mode
	U0LCR = mode;
	U0FCR = fifomode;
	// with uart configured, clear line status and receive registers
	U0RBR; U0LSR;
}

void uart1Init(uint16_t baud, uint8_t mode, uint8_t fifomode)
{
	// set pin-select register to connect to UART1
	PINSEL0 = (PINSEL0 & ~U1_PINMASK) | U1_PINSEL;
	// reset and initialize the UART
	U1IER = 0;		// disable all interrupt sources
	U1IIR;			// clear any pending interrupts
	U1FCR = (UFCR_TX_FIFO_RESET|UFCR_RX_FIFO_RESET);	// reset the Tx/Rx FIFOs
	// set the baudrate divisor
	U1LCR = ULCR_DLAB_ENABLE;	// set divisor access bit (permits modification of divisors)
	U1DLL = (baud);				// set the baud division
	U1DLM = (baud>>8);
	U1LCR = 0;					// reset DLAB (normal operation)
	// set line parameters and fifo mode
	U1LCR = mode;
	U1FCR = fifomode;
	// with uart configured, clear line status and receive registers
	U1RBR; U1LSR;
}

int uart0SendByte(int data)
{
	// wait for tx buffer ready
	while( !(U0LSR & ULSR_THRE) );
	// send the character
	return (U0THR = data);
}

int uart1SendByte(int data)
{
	// wait for tx buffer ready
	while( !(U1LSR & ULSR_THRE) );
	// send the character
	return (U1THR = data);
}

int uart0GetByte(void)
{
	// if character is available, return it
	if(U0LSR & ULSR_RDR)
		return (U0RBR);
	// otherwise, return failure
	return -1;
}

int uart1GetByte(void)
{	
	// if character is available, return it
	if(U1LSR & ULSR_RDR)
		return (U1RBR);
	// otherwise, return failure
	return -1;
}
