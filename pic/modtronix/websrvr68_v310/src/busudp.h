/**
 * @brief           Part of the Modtronix Configurable Buses. This modules uses buffers from
 *                  the buses module for it's transmit and receive buffers.
 * @file            busudp.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v3.21 or higher
 * @ingroup         mod_buses
 *
 *
 * @section description Description
 **********************************
 * This module implements UDP port(s) that can be used for reading and writing
 * data to a remote host.
 *
 *
 * @subsection busudp_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------- BusUDP Configuration --------------------
 //*********************************************************************
 //The default UDP port to use for the UDP 1 Port.
 //#define BUS_UDP1_PORT        (54125)
 //
 //The default UDP port to use for the UDP 2 Port.
 //#define BUS_UDP2_PORT        (54126)

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
 * 2008-07-18, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

/**
 * @defgroup mod_buses_busudp UDP Buses
 * @ingroup mod_buses
 *********************************************************************/
#ifndef _BUSUDP_H_
#define _BUSUDP_H_

#include "buses.h"

/////////////////////////////////////////////////
//Global defines


/////////////////////////////////////////////////
//udp1Stat defines
#define UDP1_RXBUF_OVERRUN    0x01    //The receive buffer had overrun. Must be cleared by the user
#define UDP1_TXBUF_OVERRUN    0x10    //The transmit buffer had overrun. Must be cleared by the user

//udp2Stat defines
#define UDP2_RXBUF_OVERRUN    0x01    //The receive buffer had overrun. Must be cleared by the user
#define UDP2_TXBUF_OVERRUN    0x10    //The transmit buffer had overrun. Must be cleared by the user


/////////////////////////////////////////////////
//Global data
#if !defined(_BUSUDP_H_)
extern BYTE udp1Stat;
extern BYTE udp2Stat;
#endif

/////////////////////////////////////////////////
//Function prototypes


/**
 * Initialize this module
 */
void udpBusInit(void);

/**
 * Service this module
 */
void udpBusService(void);


/**
 * Get the current status of the UDP 1 Port
 *
 * @return The status of this module. Consists out of UDP1_XXX flags
 */
BYTE udp1BusGetStat(void);


/**
 * Get the current status of the UDP 2 Port
 *
 * @return The status of this module. Consists out of UDP2_XXX flags
 */
BYTE udp2BusGetStat(void);


/**
 * Are there any bytes in the receive buffer.
 * @return 0 if not empty, 1 if empty
 */
#define udp1BusRxBufEmpty() busIsRxBufEmpty(BUSID_UDP1)


/**
 * Are there any bytes in the receive buffer.
 * @return 0 if not empty, 1 if empty
 */
#define udp2BusRxBufEmpty() busIsRxBufEmpty(BUSID_UDP2)


/**
 * Are there any bytes in the UDP1 Bus receive buffer.
 * @return 1 if true, else 0
 */
#define udp1BusIsGetReady() busRxBufHasData(BUSID_UDP1)


/**
 * Are there any bytes in the UDP1 Bus receive buffer.
 * @return 1 if true, else 0
 */
#define udp2BusIsGetReady() busRxBufHasData(BUSID_UDP2)


/**
 * Get the next byte in the RX buffer. Before calling this function, the
 * udp1IsGetReady() function should be called to check if there is any
 * data available.
 *
 * @return Returns next byte in receive buffer.
 */
BYTE udp1BusGetByte(void);


/**
 * Get the next byte in the RX buffer. Before calling this function, the
 * udp1IsGetReady() function should be called to check if there is any
 * data available.
 *
 * @return Returns next byte in receive buffer.
 */
BYTE udp2BusGetByte(void);


/**
 * Add a byte to the TX buffer.
 *
 * @param c     Byte to write out on the serial port
 */
void udp1BusPutByte(BYTE c);


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


#endif    //_SERINT_H_




