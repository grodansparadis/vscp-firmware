/**
 * @brief           Interrupt driven Serial receive and transmit handler for USART1.
 *                  Part of the Modtronix Configurable Buses. This modules uses buffers from
 *                  the buses module for it's transmit and receive buffers.
 * @file            busser1.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v3.21 or higher
 * @ingroup         mod_buses
 *
 *
 * @section description Description
 **********************************
 * This module implements an interrupt driven serial port driver on USART1
 *  To use this module:
 *  - Customize defines specified in "busser1 Configuration".
 *  - Call serInit() to initialize, but NOT enable module
 *  - Call serEnable() to enable module
 *  - Module is now active. Use funtions to write and read serial port.
 *
 *
 * @subsection busser_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------- busser1 Configuration --------------------
 //*********************************************************************
 //Comment this line if the transmit routines should NOT wait for the bytes to be send via
 //USART if tx buffer is full
 #define SER_WAIT_FOR_TXBUF

 //Uncomment this line if the application does NOT configure the USART
 //#define BAUD_RATE    9600ul
 @endcode
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/

/**
 * @defgroup mod_buses_busser1 Serial Port1
 * @ingroup mod_buses
 *********************************************************************/
#ifndef _BUSSER1_H_
#define _BUSSER1_H_

#include "buses.h"

/////////////////////////////////////////////////
//Global defines


/////////////////////////////////////////////////
//Global data


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
 * Enables the serial port AND . Can only be called after serInit() has been called.
 */
void serEnable(void);

/**
 * Disables the serial port
 */
void serDisable(void);

/**
 * Resets this module, and empties all buffers.
 */
void serReset(void);

/**
 * Are there any bytes in the receive buffer.
 * @return 0 if not empty, 1 if empty
 */
#define serRxBufEmpty() busIsRxBufEmpty(BUSID_SER1)

/**
 * Are there any bytes in the receive buffer.
 * @return 1 if true, else 0
 */
#define serIsGetReady() busRxBufHasData(BUSID_SER1)


/**
 * Get the next byte in the RX buffer. Before calling this function, the
 * serIsGetReady() function should be called to check if there is any
 * data available on the serial port.
 *
 * @return Returns byte received via the serial port.
 */
BYTE serGetByte(void);


/**
 * Send the given byte to the USART. It is added to the transmit buffer, and asynchronously
 * transmitted.
 *
 * @param c     Byte to write out on the serial port
 */
void serPutByte(BYTE c);


/**
 * Send the ASCII hex value of the given byte to the USART. It is added to the transmit buffer, and
 * asynchronously transmitted. For example, if c=11, then "0B" will be sent to the USART
 *
 * @param c     Byte to write out on the serial port
 */
void serPutByteHex(BYTE c);


#if !defined(DEBUG_OFF)
/**
 * Sent the given two bytes via the USART.
 *
 * @param b1 The first byte to be sent.
 * @param b2 The second byte to be sent.
 */
void serPut2Bytes(BYTE b1, BYTE b2);


/**
 * Sent the given debug message to the USART. The following is sent:
 * [debugCode][msgCode][msgString]
 *
 * @param debugCode The Debug Message's "Debug Code". This will be the first byte sent.
 * @param msgCode The Debug Message's "Message Code". This will be the second byte sent.
 * @param msgString The Debug Message's "Message String", has to be a NULL terminated string. This will be sent after the msgCode.
 */
void serPutDebugMsgRomStr(BYTE debugCode, BYTE msgCode, ROM char* msgString);

/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is ALSO sent!
 *
 * @param str   Null terminated string to write out on the serial port
 */
void serPutRomStringAndNull(ROM char* str);

/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is ALSO sent!
 *
 * @param str   Null terminated string to write out on the serial port
 */
void serPutStringAndNull(BYTE* str);
#endif


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT sent!
 *
 * @param s     Null terminated string to write out on the serial port
 */
void serPutString(BYTE* s);


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT sent!
 *
 * @param str   Null terminated string to write out on the serial port
 */
void serPutRomString(ROM char* str);


#endif    //_BUSSER1_H_




