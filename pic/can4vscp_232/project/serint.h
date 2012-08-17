/* ****************************************************************************
 * 	CAN4VSCP - RS232 CAN interface for VSCP
 * 	VSCP - Very Simple Control Protocol
 * 	http://www.vscp.org
 *  Copyright (C) 2000-2010 Ake Hedman, eurosource
 *                          <ake@eurosource.se>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * ****************************************************************************
 * $Id: serint.h 835 2008-02-08 13:58:32Z jaguarondi $
 */

/*
 *	Interrupt driven Serial receive and transmit handler.
 *
 *  To use this module:
 *  - Customize defines specified in "Serinit Configuration".
 *  - Call serInit() to initialize, but NOT enable module
 *  - Call serEnable() to enable module
 *  - Module is now active. Use funtions to write and read serial port.
 */

//********************************************************
//                 Serint Configuration
//********************************************************

#define SER_RXBUF_SIZE 64 // Size of Rx buffer, must be 8,16,32,64,128 or 256
#define SER_TXBUF_SIZE 64 // Size of TX buffer, must be 8,16,32,64,128 or 256

// Comment this line if the transmit routines should NOT wait for the bytes to
// be send via USART if tx buffer is full
#define SER_WAIT_FOR_TXBUF

// Uncomment this line if the application does NOT configure the USART
//#define BAUD_RATE	57600		//USART BAUD rate

// Comment this line if the application does NOT configures the USART
//#define APP_CONFIGURES_SERPORT	//Our application does all serial port
									//configuration!

//********************************************************

#ifndef _SERINT_H_
#define _SERINT_H_

#include "can4vscp.h"

//ANSI commands
static ROM char AnsiEscClearScreen[] = "\x1B[2J";

/////////////////////////////////////////////////
//Global defines
#define SER_USE_INTERRUPT

#ifndef SER_RXBUF_SIZE
#define SER_RXBUF_SIZE 64 //Size of Rx buffer's data area
#endif

#ifndef SER_TXBUF_SIZE
#define SER_TXBUF_SIZE 64 //Size of Tx buffer's data area
#endif


/////////////////////////////////////////////////
//USART CONFIG - only done if application does not do it
#if !defined(APP_CONFIGURES_SERPORT)

#if defined(USART_USE_BRGH_LOW)
#define SPBRG_VAL ( ((CLOCK_FREQ/BAUD_RATE)/64) - 1)
#else
#define SPBRG_VAL ( ((CLOCK_FREQ/BAUD_RATE)/16) - 1)
#endif

#if SPBRG_VAL > 255
#error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
#endif
#endif


/////////////////////////////////////////////////
//serStat defines
#define SERINT_RXBUF_OVERRUN	0x01	// The receive buffer had overrun.
										// Must be cleared by the user
#define SERINT_TXBUF_OVERRUN	0x10	// The transmit buffer had overrun.
										// Must be cleared by the user


/////////////////////////////////////////////////
//Global data
extern volatile BYTE idxSerRxbufGet; //get index
extern volatile BYTE idxSerRxbufPut; //put index


/////////////////////////////////////////////////
//Function prototypes


/**
 * Called in the USART TX ISR
 */
void serTxIsr(void);

/**
 * Called in the USART RX ISR
 */
void serRxIsr(void);

/**
 * Service this module
 */
void serService(void);

/**
 * Initialize this module
 */
void serInit(void);

/**
 * Enables the serial port AND . Can only be called after serInit() has been
 * called.
 */
void serEnable(void);

/**
 * Resets this module, and empties all buffers.
 */
void serReset(void);

/**
 * Get the current status of this module
 * @return The status of this module. Consists out of SER_STAT_XXX flags
 */
BYTE serGetStat(void);

/**
 * Are there any bytes in the receive buffer.
 * @return 0 if not empty, 1 if empty
 */
#define serRxBufEmpty() (idxSerRxbufPut == idxSerRxbufGet)

/**
 * Are there any bytes in the receive buffer.
 * @return 1 if true, else 0
 */
#define serIsGetReady() (idxSerRxbufPut != idxSerRxbufGet)

/**
 * Get the next byte in the RX buffer.
 */
BYTE serGetByte(void);

/**
 * Transmit the given byte on the USART. It is added to the transmit buffer,
 * and asynchronously
 * transmitted.
 */
void serPutByte(BYTE c);

/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and
 * asynchronously
 * transmitted.
 */
void serPutString(BYTE* s);

/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and
 * asynchronously
 * transmitted.
 */
void serPutRomString(ROM char* str);


#endif	//_SERINT_H_
