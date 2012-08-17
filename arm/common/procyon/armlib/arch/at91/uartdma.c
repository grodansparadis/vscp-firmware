/*! \file uartdma.c \brief UART driver for AT91SAM7S with DMA. */
//*****************************************************************************
//
// File Name	: 'uartdma.c'
// Title		: UART driver for AT91SAM7S with DMA
// Author		: Pascal Stang - Copyright (C) 2004-2006
// Created		: 4/3/2004
// Revised		: 10/24/2006
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
#include "uartdma.h"

// receive and transmit buffers
cBuffer UartRxBuffer[3];			///< uart receive buffers
static unsigned char Uart0RxData[UART0_RX_BUFFER_SIZE];
static unsigned char Uart1RxData[UART1_RX_BUFFER_SIZE];
static unsigned char Uart2RxData[UART2_RX_BUFFER_SIZE];
cBuffer UartTxBuffer[3];			///< uart transmit buffers
static unsigned char Uart0TxData[UART0_TX_BUFFER_SIZE];
static unsigned char Uart1TxData[UART1_TX_BUFFER_SIZE];
static unsigned char Uart2TxData[UART2_TX_BUFFER_SIZE];

// Global Pointer to USARTs
AT91S_USART* const pUSART[3] = { AT91C_BASE_US0, AT91C_BASE_US0, AT91C_BASE_DBGU };
#define pUSART0		((AT91S_USART*)AT91C_BASE_US0)
#define pUSART1		((AT91S_USART*)AT91C_BASE_US1)
#define pUSART2		((AT91S_USART*)AT91C_BASE_DBGU)
#define pPDC(dev)	((AT91S_PDC*)((unsigned char*)(dev)+0x100))


//! enable and initialize the uart
void uart0Init(uint16_t bauddiv, uint32_t mode)
{
	// enable the clock of UART0
	AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_US0);
	// enable uart pins on PIO
	*AT91C_PIOA_PDR = AT91C_PA5_RXD0 | AT91C_PA6_TXD0;
	// select peripheral connection
    *AT91C_PIOA_ASR = AT91C_PA5_RXD0 | AT91C_PA6_TXD0;
	// enable I/O pullup to avoid spurious receive if RXD pin left unconnected
    *AT91C_PIOA_PPUER = AT91C_PA5_RXD0;
	// reset the UART
	pUSART[0]->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS |AT91C_US_TXDIS;
	// set serial line mode
	pUSART[0]->US_MR =	AT91C_US_USMODE_NORMAL |// Normal Mode
						AT91C_US_CLKS_CLOCK |	// Clock = MCK
						mode;					// user-defined data parameters
	// set the baud rate
	pUSART[0]->US_BRGR = bauddiv;
	// enable the uart
	pUSART[0]->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
	
	// initialize buffers
	uart0InitBuffers();
	// setup DMA controller for transmit
	uartInitDmaTx(0);
	// setup DMA controller for receive
	//uartInitDmaRx(0);

	// attach interrupt handler
	processorAicAttach(AT91C_ID_US0, (UART0_INTERRUPT_LEVEL|AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL), uart0Service);
	// enable receive interrupt
	pUSART[0]->US_IER = AT91C_US_RXRDY;
}

void uart1Init(uint16_t bauddiv, uint32_t mode)
{
	// enable the clock of UART1
	AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_US1);
	// enable uart pins on PIO
	*AT91C_PIOA_PDR = AT91C_PA21_RXD1 | AT91C_PA22_TXD1;
	// select peripheral connection
    *AT91C_PIOA_ASR = AT91C_PA21_RXD1 | AT91C_PA22_TXD1;
	// enable I/O pullup to avoid spurious receive if RXD pin left unconnected
    *AT91C_PIOA_PPUER = AT91C_PA21_RXD1;
	// reset the UART
	pUSART[1]->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS |AT91C_US_TXDIS;
	// set serial line mode
	pUSART[1]->US_MR =	AT91C_US_USMODE_NORMAL |// Normal Mode
						AT91C_US_CLKS_CLOCK	|	// Clock = MCK
						mode;					// user-defined data parameters
	// set the baud rate
	pUSART[1]->US_BRGR = bauddiv;
	// enable the uart
	pUSART[1]->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
	
	// initialize buffers
	uart1InitBuffers();
	// setup DMA controller for transmit
	uartInitDmaTx(1);
	// setup DMA controller for receive
	//uartInitDmaRx(0);

	// attach interrupt handler
	processorAicAttach(AT91C_ID_US1, (UART1_INTERRUPT_LEVEL|AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL), uart1Service);
	// enable receive interrupt
	pUSART[1]->US_IER = AT91C_US_RXRDY;
}

void uart2Init(uint16_t bauddiv, uint32_t mode)
{
	// enable the clock of DBGU
	AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_SYS);
	// enable uart pins on PIO
	*AT91C_PIOA_PDR = AT91C_PA9_DRXD | AT91C_PA10_DTXD;
	// enable I/O pullup to avoid spurious receive if RXD pin left unconnected
    *AT91C_PIOA_PPUER = AT91C_PA9_DRXD;
	// reset the UART
	pUSART[2]->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS |AT91C_US_TXDIS;
	// set serial line mode
	pUSART[2]->US_MR =	AT91C_US_USMODE_NORMAL |	// Normal Mode
						AT91C_US_CLKS_CLOCK	|		// Clock = MCK
						AT91C_US_CHRL_8_BITS |		// 8-bit Data (no effect on DBGU)
						AT91C_US_PAR_NONE |			// No Parity
						AT91C_US_NBSTOP_1_BIT;		// 1 Stop Bit (no effect on DBGU)
	// set the baud rate
	pUSART[2]->US_BRGR = bauddiv;
	// enable the uart
	pUSART[2]->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
	
	// initialize buffers
	uart2InitBuffers();
	// setup DMA controller for transmit
	uartInitDmaTx(2);
	// setup DMA controller for receive
	//uartInitDmaRx(2);

	// attach interrupt handler
	processorAicAttachSys(SYSPID_DBGU, uart2Service);
	// enable receive interrupt
	pUSART[2]->US_IER = AT91C_US_RXRDY;
}

void uart0InitBuffers(void)
{
	// initialize the UART0 buffers
	bufferInit(&UartRxBuffer[0], Uart0RxData, UART0_RX_BUFFER_SIZE);
	bufferInit(&UartTxBuffer[0], Uart0TxData, UART0_TX_BUFFER_SIZE);
}

void uart1InitBuffers(void)
{
	// initialize the UART0 buffers
	bufferInit(&UartRxBuffer[1], Uart1RxData, UART1_RX_BUFFER_SIZE);
	bufferInit(&UartTxBuffer[1], Uart1TxData, UART1_TX_BUFFER_SIZE);
}

void uart2InitBuffers(void)
{
	// initialize the UART0 buffers
	bufferInit(&UartRxBuffer[2], Uart2RxData, UART2_RX_BUFFER_SIZE);
	bufferInit(&UartTxBuffer[2], Uart2TxData, UART2_TX_BUFFER_SIZE);
}

void uartInitDmaTx(int dev)
{
	// clear buffer
	UartTxBuffer[dev].dataindex = 0;
	UartTxBuffer[dev].datalength = 0;
	// setup DMA controller for transmit
	pPDC(pUSART[dev])->PDC_PTCR = AT91C_PDC_TXTDIS;	// disable Tx DMA
	pPDC(pUSART[dev])->PDC_TPR = (unsigned int)UartTxBuffer[dev].dataptr;
	pPDC(pUSART[dev])->PDC_TCR = 0;
	pPDC(pUSART[dev])->PDC_TNPR = (unsigned int)UartTxBuffer[dev].dataptr;
	pPDC(pUSART[dev])->PDC_TNCR = 0;
	pPDC(pUSART[dev])->PDC_PTCR = AT91C_PDC_TXTEN;	// enable Tx DMA	
}

void uartInitDmaRx(int dev)
{
	// clear buffer
	UartRxBuffer[dev].dataindex = 0;
	UartRxBuffer[dev].datalength = 0;
	// setup DMA controller for receive
	pPDC(pUSART[dev])->PDC_PTCR = AT91C_PDC_RXTDIS;	// disable Rx DMA
	pPDC(pUSART[dev])->PDC_RPR = (unsigned int)UartTxBuffer[dev].dataptr;
	pPDC(pUSART[dev])->PDC_RCR = UartTxBuffer[dev].size;
	pPDC(pUSART[dev])->PDC_RNPR = (unsigned int)UartTxBuffer[dev].dataptr;
	pPDC(pUSART[dev])->PDC_RNCR = 0;
	pPDC(pUSART[dev])->PDC_PTCR = AT91C_PDC_RXTEN;	// enable Rx DMA
}

cBuffer* uartGetRxBuffer(int dev)
{
	// return rx buffer pointer
	return &UartRxBuffer[dev];
}

int uartSendByte(int dev, int data)
{
	#if defined(UART_DMA_BLOCK_IF_BUSY)
		// block if DMA buffer has no room
		while( (pPDC(pUSART[dev])->PDC_TCR + pPDC(pUSART[dev])->PDC_TNCR) >= UartTxBuffer[dev].size);
	#else
		// return failure if DMA buffer has no room
		if( (pPDC(pUSART[dev])->PDC_TCR + pPDC(pUSART[dev])->PDC_TNCR) >= UartTxBuffer[dev].size)
			return -1;
	#endif
	// is a circular buffer wrap necessary?
	if(UartTxBuffer[dev].dataindex == UartTxBuffer[dev].size)
	{
		//rprintf("Wrapping\n");
		// do write pointer wrap
		UartTxBuffer[dev].dataindex = 0;
		// add byte to dma buffer
		//rprintf("DMA WrPtr=%d\n", TxBufferWr);
		UartTxBuffer[dev].dataptr[UartTxBuffer[dev].dataindex] = data;
		UartTxBuffer[dev].dataindex++;
		// switch to "next" DMA
		pPDC(pUSART[dev])->PDC_TNPR = (unsigned int)UartTxBuffer[dev].dataptr;
		// increment byte count on "next" DMA
		pPDC(pUSART[dev])->PDC_TNCR++;
	}
	else
	{
		// no write pointer wrap
		// add byte to dma buffer
		//rprintf("DMA WrPtr=%d\n", TxBufferWr);
		UartTxBuffer[dev].dataptr[UartTxBuffer[dev].dataindex] = data;
		UartTxBuffer[dev].dataindex++;
		// if "next" DMA is already busy...
		if(pPDC(pUSART[dev])->PDC_TNCR)
			pPDC(pUSART[dev])->PDC_TNCR++;	// increment byte count on "next" DMA
		else
			pPDC(pUSART[dev])->PDC_TCR++;	// else, increment byte count on "this" DMA
	}

	return data;
}

int uartGetByte(int dev)
{
	if(UartRxBuffer[dev].datalength)		// check if character is available
		return bufferGetFromFront(&UartRxBuffer[dev]);	// return character
	return -1;

/*	
	int data;

	// are any bytes available?
	if( (pPDC(pUSART[dev])->PDC_RCR + pPDC(pUSART[dev])->PDC_RNCR) >= UartRxBuffer[dev].size)
		return -1;

	// is a wrap necessary?
	if(UartRxBuffer[dev].dataindex == UartRxBuffer[dev].size)
	{
		//rprintf("Wrapping\n");
		// do pointer wrap
		UartRxBuffer[dev].dataindex = 0;
		// read byte from dma buffer
		//rprintf("DMA RdPtr=%d  ", UartRxBuffer[dev].dataindex);
		data = UartRxBuffer[dev].dataptr[UartTxBuffer[dev].dataindex];
		UartRxBuffer[dev].dataindex++;
		// switch to "next" DMA
		//pUSART_PDC->PDC_RNPR = (unsigned int)RxBuffer;
		// increment byte count on "next" DMA
		pPDC(pUSART[dev])->PDC_RCR++;
	}
	else
	{
		// no pointer wrap
		// read byte from dma buffer
		//rprintf("DMA RdPtr=%d  ", UartRxBuffer[dev].dataindex);
		data = UartRxBuffer[dev].dataptr[UartTxBuffer[dev].dataindex];
		UartRxBuffer[dev].dataindex++;
		// if "next" DMA is already busy...
		pPDC(pUSART[dev])->PDC_RNPR = (unsigned int)UartRxBuffer[dev].dataptr;
		if( ((pPDC(pUSART[dev])->PDC_RPR-(unsigned int)UartRxBuffer[dev].dataptr) + pPDC(pUSART[dev])->PDC_RCR) < UartRxBuffer[dev].size)
			pPDC(pUSART[dev])->PDC_RCR++;	// increment byte count on "this" DMA
		else
			pPDC(pUSART[dev])->PDC_RNCR++;	// else, increment byte count on "next" DMA
	}

	//rprintf("RPR =0x%x  ", pPDC(pUSART[dev])->PDC_RPR-(int)UartRxBuffer[dev].dataptr);
	//rprintf("RCR =0x%x  ", pPDC(pUSART[dev])->PDC_RCR);
	//rprintf("RNPR=0x%x  ", pPDC(pUSART[dev])->PDC_RNPR-(int)UartRxBuffer[dev].dataptr);
	//rprintf("RNCR=0x%x\n", pPDC(pUSART[dev])->PDC_RNCR);

	return data;
*/
}

int uartSendBlock(int dev, unsigned char* data, unsigned int len)
{
	#if defined(UART_DMA_BLOCK_IF_BUSY)
		// block if DMA is not ready
		while( (pPDC(pUSART[dev])->PDC_TCR + pPDC(pUSART[dev])->PDC_TNCR) > 0);
	#else
		// return failure if DMA is not ready
		if( (pPDC(pUSART[dev])->PDC_TCR + pPDC(pUSART[dev])->PDC_TNCR) > 0)
			return -1;
	#endif

	// setup DMA block transfer
	pPDC(pUSART[dev])->PDC_PTCR = AT91C_PDC_TXTDIS;	// disable Tx DMA
	pPDC(pUSART[dev])->PDC_TPR = (unsigned int)data;// set pointer to data buffer
	pPDC(pUSART[dev])->PDC_TCR = len;				// set number of bytes to transfer
	pPDC(pUSART[dev])->PDC_PTCR = AT91C_PDC_TXTEN;	// enable Tx DMA (starts DMA output)
	// return success
	return 0;
}

int uartGetBlock(int dev, unsigned char* data, unsigned int len)
{
	#if defined(UART_DMA_BLOCK_IF_BUSY)
		// block if DMA is not ready
		while( (pPDC(pUSART[dev])->PDC_RCR + pPDC(pUSART[dev])->PDC_RNCR) > 0);
	#else
		// return failure if DMA is not ready
		if( (pPDC(pUSART[dev])->PDC_RCR + pPDC(pUSART[dev])->PDC_RNCR) > 0)
			return -1;
	#endif

	if(len)
	{
		// setup new DMA block transfer
		pPDC(pUSART[dev])->PDC_PTCR = AT91C_PDC_TXTDIS;	// disable Rx DMA
		pPDC(pUSART[dev])->PDC_RPR = (unsigned int)data;// set pointer to data buffer
		pPDC(pUSART[dev])->PDC_RCR = len;				// set number of bytes to transfer
		pPDC(pUSART[dev])->PDC_PTCR = AT91C_PDC_TXTEN;	// enable Rx DMA (starts DMA input)
		// return success
		return len;
	}
	else
	{
		// return remaining byte count on current DMA transfer
		return pPDC(pUSART[dev])->PDC_TCR;
	}
}

void uart0Service(void)
{
	unsigned int status;

	// read the channel status register
	status  = pUSART0->US_CSR;
	status &= pUSART0->US_IMR;

	if(status & AT91C_US_RXRDY)
	{
		bufferAddToEnd(&UartRxBuffer[0], pUSART0->US_RHR);
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
		bufferAddToEnd(&UartRxBuffer[1], pUSART1->US_RHR);
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
		bufferAddToEnd(&UartRxBuffer[2], pUSART2->US_RHR);
	}

	if(status & AT91C_US_TXRDY)
	{
	}

	// reset error status bits
	pUSART2->US_CR = AT91C_US_RSTSTA;
	// clear AIC
	AT91C_BASE_AIC->AIC_EOICR = 0;
}
