/*! \file uartdma.h \brief UART driver for AT91SAM7S with DMA. */
//*****************************************************************************
//
// File Name	: 'uartintr.h'
// Title		: UART driver for AT91SAM7S with DMA
// Author		: Pascal Stang - Copyright (C) 2004-2006
// Created		: 4/3/2004
// Revised		: 10/24/2006
// Version		: 0.1
// Target MCU	: Atmel ARM AT91SAM7S Series
// Editor Tabs	: 4
//
///	\ingroup driver_arm_at91
/// \defgroup uartdma_at91 UART driver for AT91SAM7S with DMA (armlib/arch/at91/uartdma.c)
/// \code #include "uartdma.h" \endcode
/// \par Overview
///		To be written...
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
//@{

#ifndef ARMLIB_UARTDMA_H
#define ARMLIB_UARTDMA_H

#include "global.h"
#include "buffer.h"

// defines
#ifndef UART0_INTERRUPT_LEVEL
#define UART0_INTERRUPT_LEVEL	6
#endif
#ifndef UART1_INTERRUPT_LEVEL
#define UART1_INTERRUPT_LEVEL	6
#endif
// NOTE: interrupt level of UART2 (DBGU) is not selectable


// UART_BAUD calculates a division rate given a baud rate
// for use with uart0Init() and uart1Init()
// example: uart0Init(UART_BAUD(115200), UART_8N1);
//
// BEWARE: additional code will be generated if 'baud' is not a constant
#define UART_BAUD(baud) (uint16_t)((F_CPU+baud*8L)/(baud*16))

// Definitions for UART mode settings (databits, parity, stopbits)
#define UART_8N1      (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT)
#define UART_8N2      (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_2_BIT)
#define UART_7N1      (AT91C_US_CHRL_7_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT)
#define UART_7N2      (AT91C_US_CHRL_7_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_2_BIT)

#define UART_8E1      (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_EVEN | AT91C_US_NBSTOP_1_BIT)
#define UART_8E2      (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_EVEN | AT91C_US_NBSTOP_2_BIT)
#define UART_7E1      (AT91C_US_CHRL_7_BITS | AT91C_US_PAR_EVEN | AT91C_US_NBSTOP_1_BIT)
#define UART_7E2      (AT91C_US_CHRL_7_BITS | AT91C_US_PAR_EVEN | AT91C_US_NBSTOP_2_BIT)

#define UART_8O1      (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_ODD  | AT91C_US_NBSTOP_1_BIT)
#define UART_8O2      (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_ODD  | AT91C_US_NBSTOP_2_BIT)
#define UART_7O1      (AT91C_US_CHRL_7_BITS | AT91C_US_PAR_ODD  | AT91C_US_NBSTOP_1_BIT)
#define UART_7O2      (AT91C_US_CHRL_7_BITS | AT91C_US_PAR_ODD  | AT91C_US_NBSTOP_2_BIT)


// buffer memory allocation defines
// buffer sizes
#ifndef UART0_TX_BUFFER_SIZE
#define UART0_TX_BUFFER_SIZE		0x0010	///< number of bytes for uart0 transmit buffer
#endif
#ifndef UART0_RX_BUFFER_SIZE
#define UART0_RX_BUFFER_SIZE		0x0080	///< number of bytes for uart0 receive buffer
#endif
#ifndef UART1_TX_BUFFER_SIZE
#define UART1_TX_BUFFER_SIZE		0x0010	///< number of bytes for uart1 transmit buffer
#endif
#ifndef UART1_RX_BUFFER_SIZE
#define UART1_RX_BUFFER_SIZE		0x0080	///< number of bytes for uart1 receive buffer
#endif
#ifndef UART2_TX_BUFFER_SIZE
#define UART2_TX_BUFFER_SIZE		0x0010	///< number of bytes for uart2 transmit buffer
#endif
#ifndef UART2_RX_BUFFER_SIZE
#define UART2_RX_BUFFER_SIZE		0x0080	///< number of bytes for uart2 receive buffer
#endif

// functions

//! initializes uart
void uart0Init(uint16_t bauddiv, uint32_t mode);
void uart1Init(uint16_t bauddiv, uint32_t mode);
void uart2Init(uint16_t bauddiv, uint32_t mode);

//! initializes transmit and receive buffers
// called from uartInit()
void uart0InitBuffers(void);
void uart1InitBuffers(void);
void uart2InitBuffers(void);

//! setup DMA for circular-buffer transmit
// called from uartInit();
void uartInitDmaTx(int dev);

//! setup DMA for circular-buffer receive
// called from uartInit();
void uartInitDmaRx(int dev);

// returns the serial port buffer for doing buffer-based processing
// THIS WILL LIKELY BE ELIMINATED WHEN RX IS HANDLED BY DMA
cBuffer* uartGetRxBuffer(int dev);

//! sends a single byte over the uart
// These functions automatically circular-buffer the data,
// resulting in a small amount of CPU usage at the time of function call.
// No CPU time is used during actual transmit.
int uartSendByte(int dev, int data);
inline int uart0SendByte(int data) { return uartSendByte(0, data); };
inline int uart1SendByte(int data) { return uartSendByte(1, data); };
inline int uart2SendByte(int data) { return uartSendByte(2, data); };

//! gets a single byte from the uart receive buffer
int uartGetByte(int dev);
inline int uart0GetByte(void) { return uartGetByte(0); };
inline int uart1GetByte(void) { return uartGetByte(1); };
inline int uart2GetByte(void) { return uartGetByte(2); };

//! sends a block of data over the uart
// To maintain data integrity, the user must not modify the data buffer until the transfer is complete.
int uartSendBlock(int dev, unsigned char* data, unsigned int len);
inline int uart0SendBlock(unsigned char* data, unsigned int len) { return uartSendBlock(0,data,len); };
inline int uart1SendBlock(unsigned char* data, unsigned int len) { return uartSendBlock(1,data,len); };
inline int uart2SendBlock(unsigned char* data, unsigned int len) { return uartSendBlock(2,data,len); };

//! gets a block of data from over the uart
// if len>0, a new transfer is set up, and returns <len>
// if len=0, returns the number of bytes left to transfer (can be used to poll for completion)
int uartGetBlock(int dev, unsigned char* data, unsigned int len);
inline int uart0GetBlock(unsigned char* data, unsigned int len) { return uartGetBlock(0,data,len); };
inline int uart1GetBlock(unsigned char* data, unsigned int len) { return uartGetBlock(1,data,len); };
inline int uart2GetBlock(unsigned char* data, unsigned int len) { return uartGetBlock(2,data,len); };

//! interrupt service handlers
void uart0Service(void);
void uart1Service(void);
void uart2Service(void);

#endif
//@}
