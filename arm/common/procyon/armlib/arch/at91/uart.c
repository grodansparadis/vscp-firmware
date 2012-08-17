/*! \file uart.c \brief UART driver for AT91SAM7S. */
//*****************************************************************************
//
// File Name	: 'uart.c'
// Title		: UART driver for AT91SAM7S
// Author		: Pascal Stang - Copyright (C) 2004-2006
// Created		: 4/3/2004
// Revised		: 7/4/2006
// Version		: 0.1
// Target MCU	: Atmel ARM AT91SAM7S Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

// AT91SAM7S definitions
#include "at91sam7s64.h"
#include "global.h"

#include "uart.h"

// Global Pointer to USARTs
//AT91S_USART * pUSART0 = AT91C_BASE_US0;
//AT91S_USART * pUSART1 = AT91C_BASE_US1;
#define pUSART0  ((AT91S_USART*)AT91C_BASE_US0)
#define pUSART1  ((AT91S_USART*)AT91C_BASE_US1)
#define pUSART2  ((AT91S_USART*)AT91C_BASE_DBGU)

//! enable and initialize the uart
void uart0Init(uint16_t bauddiv, uint32_t mode)
{
	// enable the clock of UART0
	AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_US0);
	// enable uart pins on PIO
	*AT91C_PIOA_PDR = AT91C_PA5_RXD0 | AT91C_PA6_TXD0;
	// select peripheral connection
    *AT91C_PIOA_ASR = AT91C_PA5_RXD0 | AT91C_PA6_TXD0;
	// disable I/O pullup
     *AT91C_PIOA_PPUDR = AT91C_PA5_RXD0;
	// reset the UART
	pUSART2->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS |AT91C_US_TXDIS;
	// set serial line mode
	pUSART0->US_MR =	AT91C_US_USMODE_NORMAL |// Normal Mode
						AT91C_US_CLKS_CLOCK |	// Clock = MCK
						mode;					// user-defined data parameters
	// set the baud rate
	pUSART0->US_BRGR = bauddiv;
	// enable the uart
	pUSART0->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
}

void uart1Init(uint16_t bauddiv, uint32_t mode)
{
	// enable the clock of UART1
	AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_US1);
	// enable uart pins on PIO
	*AT91C_PIOA_PDR = AT91C_PA21_RXD1 | AT91C_PA22_TXD1;
	// select peripheral connection
    *AT91C_PIOA_ASR = AT91C_PA21_RXD1 | AT91C_PA22_TXD1;
	// disable I/O pullup
    *AT91C_PIOA_PPUDR = AT91C_PA21_RXD1;
	// reset the UART
	pUSART2->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS |AT91C_US_TXDIS;
	// set serial line mode
	pUSART1->US_MR =	AT91C_US_USMODE_NORMAL |// Normal Mode
						AT91C_US_CLKS_CLOCK	|	// Clock = MCK
						mode;					// user-defined data parameters
	// set the baud rate
	pUSART1->US_BRGR = bauddiv;
	// enable the uart
	pUSART1->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
}

void uart2Init(uint16_t bauddiv, uint32_t mode)
{
	// enable the clock of DBGU
	AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_SYS);
	// enable uart pins on PIO
	*AT91C_PIOA_PDR = AT91C_PA9_DRXD | AT91C_PA10_DTXD;
	// reset the UART
	pUSART2->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS |AT91C_US_TXDIS;
	// set serial line mode
	pUSART2->US_MR =	AT91C_US_USMODE_NORMAL |	// Normal Mode
						AT91C_US_CLKS_CLOCK	|		// Clock = MCK
						AT91C_US_CHRL_8_BITS |		// 8-bit Data (no effect on DBGU)
						AT91C_US_PAR_NONE |			// No Parity
						AT91C_US_NBSTOP_1_BIT;		// 1 Stop Bit (no effect on DBGU)
	// set the baud rate
	pUSART2->US_BRGR = bauddiv;
	// enable the uart
	pUSART2->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
}

int uart0SendByte(int data)
{
	// wait for tx buffer ready
	while( !(pUSART0->US_CSR & AT91C_US_TXRDY) );
	// send the character
	return (pUSART0->US_THR = data);
}

int uart1SendByte(int data)
{
	// wait for tx buffer ready
	while( !(pUSART1->US_CSR & AT91C_US_TXRDY) );
	// send the character
	return (pUSART1->US_THR = data);
}

int uart2SendByte(int data)
{
	// wait for tx buffer ready
	while( !(pUSART2->US_CSR & AT91C_US_TXRDY) );
	// send the character
	return (pUSART2->US_THR = data);
}

int uart0GetByte(void)
{	
	// if character is available, return it
	if(pUSART0->US_CSR & AT91C_US_RXRDY)
		return (pUSART0->US_RHR);
	// otherwise, return failure
	return -1;
}

int uart1GetByte(void)
{	
	// if character is available, return it
	if(pUSART1->US_CSR & AT91C_US_RXRDY)
		return (pUSART1->US_RHR);
	// otherwise, return failure
	return -1;
}

int uart2GetByte(void)
{
	// if character is available, return it
	if(pUSART2->US_CSR & AT91C_US_RXRDY)
		return (pUSART2->US_RHR);
	// otherwise, return failure
	return -1;
}

