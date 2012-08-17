/*! \file uartintr.h \brief UART driver for AT91SAM7S with interrupts. */
//*****************************************************************************
//
// File Name	: 'uartintr.h'
// Title		: UART driver for AT91SAM7S with interrupts
// Author		: Pascal Stang - Copyright (C) 2004-2006
// Created		: 4/3/2004
// Revised		: 7/4/2006
// Version		: 0.1
// Target MCU	: Atmel ARM AT91SAM7S Series
// Editor Tabs	: 4
//
///	\ingroup driver_arm_at91
/// \defgroup uartintr_at91 UART driver for AT91SAM7S with interrupts (armlib/arch/at91/uartintr.c)
/// \code #include "uartintr.h" \endcode
/// \par Overview
///		To be written...
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
//@{

#ifndef ARMLIB_UARTINTR_H
#define ARMLIB_UARTINTR_H

#include "global.h"
#include "buffer.h"

// defines
#ifndef UART0_INTERRUPT_LEVEL
#define UART0_INTERRUPT_LEVEL	6
#endif
#ifndef UART1_INTERRUPT_LEVEL
#define UART1_INTERRUPT_LEVEL	6
#endif

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

//! get receive buffer structure
cBuffer* uartGetRxBuffer(int dev);

//! sends a single byte over the uart
int uart0SendByte(int data);
int uart1SendByte(int data);
int uart2SendByte(int data);

//! gets a single byte from the uart receive buffer
int uart0GetByte(void);
int uart1GetByte(void);
int uart2GetByte(void);

//! interrupt service handlers
void uart0Service(void);
void uart1Service(void);
void uart2Service(void);


#endif
//@}
