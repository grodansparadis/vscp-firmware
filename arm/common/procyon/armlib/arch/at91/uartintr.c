/*! \file uartintr.c \brief UART driver for AT91SAM7S with interrupts. */
//*****************************************************************************
//
// File Name	: 'uartintr.c'
// Title		: UART driver for AT91SAM7S with interrupts
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
#include "processor.h"
#include "buffer.h"
#include "uartintr.h"

// receive and transmit buffers
cBuffer uartRxBuffer[3];			///< uart receive buffer
static unsigned char uart0RxData[UART0_RX_BUFFER_SIZE];
static unsigned char uart1RxData[UART1_RX_BUFFER_SIZE];
static unsigned char uart2RxData[UART2_RX_BUFFER_SIZE];
//cBuffer uartTxBuffer[3];			///< uart transmit buffer
//static unsigned char uart0TxData[UART0_TX_BUFFER_SIZE];
//static unsigned char uart1TxData[UART1_TX_BUFFER_SIZE];
//static unsigned char uart2TxData[UART2_TX_BUFFER_SIZE];

// Global Pointer to USARTs
//AT91S_USART * pUSART0 = AT91C_BASE_US0;
//AT91S_USART * pUSART1 = AT91C_BASE_US1;
//AT91S_USART * pUSART2 = AT91C_BASE_DBGU;
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
	// initialize buffers
	bufferInit(&uartRxBuffer[0], uart0RxData, UART0_RX_BUFFER_SIZE);
	//bufferInit(&uartTxBuffer[0], uart0TxData, UART0_TX_BUFFER_SIZE);
	// attach interrupt handler
	processorAicAttach(AT91C_ID_US0, (UART0_INTERRUPT_LEVEL|AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL), uart0Service);
	// enable receive interrupt
	pUSART0->US_IER = AT91C_US_RXRDY;
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
	// initialize buffers
	bufferInit(&uartRxBuffer[1], uart1RxData, UART1_RX_BUFFER_SIZE);
	//bufferInit(&uartTxBuffer[1], uart1TxData, UART1_TX_BUFFER_SIZE);
	// attach interrupt handler
	processorAicAttach(AT91C_ID_US1, (UART1_INTERRUPT_LEVEL|AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL), uart1Service);
	// enable receive interrupt
	pUSART1->US_IER = AT91C_US_RXRDY;
}

void uart2Init(uint16_t bauddiv, uint32_t mode)
{
	// enable the clock of DBGU
	AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_SYS);
	// enable uart pins on PIO
	*AT91C_PIOA_PDR = AT91C_PA9_DRXD | AT91C_PA10_DTXD;
	// disable I/O pullup
    *AT91C_PIOA_PPUDR = AT91C_PA9_DRXD;
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
	// initialize buffers
	bufferInit(&uartRxBuffer[2], uart2RxData, UART2_RX_BUFFER_SIZE);
	//bufferInit(&uartTxBuffer[2], uart2TxData, UART2_TX_BUFFER_SIZE);
	// attach interrupt handler
	processorAicAttachSys(SYSPID_DBGU, uart2Service);
	// enable receive interrupt
	pUSART2->US_IER = AT91C_US_RXRDY;
}

cBuffer* uart0GetRxBuffer(int dev)
{
	// return rx buffer pointer
	return &uartRxBuffer[dev];
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
	if(uartRxBuffer[0].datalength)		// check if character is available
		return bufferGetFromFront(&uartRxBuffer[0]);	// return character
	return -1;
}

int uart1GetByte(void)
{	
	if(uartRxBuffer[1].datalength)		// check if character is available
		return bufferGetFromFront(&uartRxBuffer[1]);	// return character
	return -1;
}

int uart2GetByte(void)
{	
	if(uartRxBuffer[2].datalength)		// check if character is available
		return bufferGetFromFront(&uartRxBuffer[2]);	// return character
	return -1;
}

void uart0Service(void)
{
	unsigned int status;

	// read the channel status register
	status  = pUSART0->US_CSR;
	status &= pUSART0->US_IMR;

	if(status & AT91C_US_RXRDY)
	{
		bufferAddToEnd(&uartRxBuffer[0], pUSART0->US_RHR);
	}

	if(status & AT91C_US_TXRDY)
	{
	}

	// reset error status bits
	pUSART0->US_CR = AT91C_US_RSTSTA;
	// clear AIC
	AT91C_BASE_AIC->AIC_EOICR = 0;
}

void uart1Service(void)
{
	unsigned int status;

	// read the channel status register
	status  = pUSART1->US_CSR;
	status &= pUSART1->US_IMR;

	if(status & AT91C_US_RXRDY)
	{
		bufferAddToEnd(&uartRxBuffer[1], pUSART1->US_RHR);
	}

	if(status & AT91C_US_TXRDY)
	{
	}

	// reset error status bits
	pUSART1->US_CR = AT91C_US_RSTSTA;
	// clear AIC
	AT91C_BASE_AIC->AIC_EOICR = 0;
}

void uart2Service(void)
{
	unsigned int status;

	// read the channel status register
	status  = pUSART2->US_CSR;
	status &= pUSART2->US_IMR;

	if(status & AT91C_US_RXRDY)
	{
		bufferAddToEnd(&uartRxBuffer[2], pUSART2->US_RHR);
	}

	if(status & AT91C_US_TXRDY)
	{
	}

	// reset error status bits
	pUSART2->US_CR = AT91C_US_RSTSTA;
	// clear AIC
//	AT91C_BASE_AIC->AIC_EOICR = 0;
}
