/*! \file uartintr.c \brief UART driver for ARM LPC2000 16550 with interrupts. */
//*****************************************************************************
//
// File Name	: 'uartintr.c'
// Title		: UART driver for ARM LPC2000 16550 with interrupts
// Author		: Pascal Stang - Copyright (C) 2004-2006
// Created		: 4/3/2004
// Revised		: 4/11/2006
// Version		: 0.1
// Target MCU	: Philips ARM LPC2000 Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <limits.h>
#include "LPC2000.h"

#include "global.h"
#include "processor.h"
#include "buffer.h"
#include "uartintr.h"

// receive and transmit buffers
cBuffer uartRxBuffer[2];			///< uart receive buffer
cBuffer uartTxBuffer[2];			///< uart transmit buffer
static unsigned char uart0RxData[UART0_RX_BUFFER_SIZE];
static unsigned char uart1RxData[UART1_RX_BUFFER_SIZE];

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
	// initialize buffers
	uart0InitBuffers();
	// attach interrupt handler
	processorVicAttach(VIC_UART0, 0, uart0Service);
	// enable receive interrupt
	U0IER |= UIER_ERBFI;
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
	// initialize buffers
	uart1InitBuffers();
	// attach interrupt handler
	processorVicAttach(VIC_UART1, 0, uart1Service);
	// enable receive interrupt
	U1IER |= UIER_ERBFI;
}

void uart0InitBuffers(void)
{
	// initialize the UART0 buffers
	bufferInit(&uartRxBuffer[0], uart0RxData, UART0_RX_BUFFER_SIZE);
	//bufferInit(&uartTxBuffer[0], uart0TxData, UART0_TX_BUFFER_SIZE);
}

void uart1InitBuffers(void)
{
	// initialize the UART1 buffers
	bufferInit(&uartRxBuffer[1], uart1RxData, UART1_RX_BUFFER_SIZE);
	//bufferInit(&uartTxBuffer[1], uart1TxData, UART1_TX_BUFFER_SIZE);
}

cBuffer* uart0GetRxBuffer(void)
{
	// return rx buffer pointer
	return &uartRxBuffer[0];
}

cBuffer* uart1GetRxBuffer(void)
{
	// return rx buffer pointer
	return &uartRxBuffer[1];
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
	if(uartRxBuffer[0].datalength)
		return bufferGetFromFront(&uartRxBuffer[0]);
	// otherwise, return failure
	return -1;
}

int uart1GetByte(void)
{	
	// if character is available, return it
	if(uartRxBuffer[1].datalength)		// check if character is available
		return bufferGetFromFront(&uartRxBuffer[1]);	// return character
	// otherwise, return failure
	return -1;
}

void uart0Service(void)
{
	uint8_t intr_flags;

//	ISR_ENTRY();
	
	// service pending interrupts until the interrupt status register is clear
	while( !((intr_flags=U0IIR) & UIIR_NO_INT) )
	{
		// check the interrupt source
		switch(intr_flags & UIIR_ID_MASK)
		{
		case UIIR_RLS_INT:	// receive line status interrupt
			U0LSR;					// clear interrupt by reading status
			break;
		case UIIR_CTI_INT:	// character timeout interrupt
		case UIIR_RDA_INT:	// receive data available interrupt
			do { bufferAddToEnd(&uartRxBuffer[1], U0RBR); } while(U0LSR & ULSR_RDR);
			break;
		case UIIR_THRE_INT:	// transmit holding register empty
			break;
		default:			// unknown/unhandled interrupt
			U0LSR;					// read registers to clear
			U0RBR;
			break;
		}
	}

	VICSoftIntClear = (1<<VIC_UART0);	// clear software interrupt
	VICVectAddr = 0x00000000;			// clear the VIC
//	ISR_EXIT();
}

void uart1Service(void)
{
	uint8_t intr_flags;

//	ISR_ENTRY();

	// service pending interrupts until the interrupt status register is clear
	while( !((intr_flags=U1IIR) & UIIR_NO_INT) )
	{
		// check the interrupt source
		switch(intr_flags & UIIR_ID_MASK)
		{
		case UIIR_RLS_INT:	// receive line status interrupt
			U1LSR;					// clear interrupt by reading status
			break;
		case UIIR_CTI_INT:	// character timeout interrupt
		case UIIR_RDA_INT:	// receive data available interrupt
			do { bufferAddToEnd(&uartRxBuffer[1], U1RBR); } while(U1LSR & ULSR_RDR);
			break;
		case UIIR_THRE_INT:	// transmit holding register empty
			break;
		case UIIR_MS_INT:	// MODEM Status
			U1MSR;					// read MSR to clear
			break;
		default:			// unknown/unhandled interrupt
			U1LSR;					// read registers to clear
			U1RBR;
			U1MSR;
			break;
		}
	}
	
	VICSoftIntClear = (1<<VIC_UART1);	// clear software interrupt
	VICVectAddr = 0x00000000;			// clear the VIC
//	ISR_EXIT();
}
