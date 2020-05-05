/**
 * @brief           Interrupt driven Serial receive and transmit handler.
 * @file            serint.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_serint
 *
 *
 * @section description Description
 **********************************
 * This module implements an interrupt driven serial port driver.
 *  To use this module:
 *  - Customize defines specified in "Serinit Configuration".
 *  - Call serInit() to initialize, but NOT enable module
 *  - Call serEnable() to enable module
 *  - Module is now active. Use funtions to write and read serial port.
 *
 *
 * @subsection serint_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------- Serint Configuration --------------------
 //*********************************************************************
 #define SER_RXBUF_SIZE  32  //Size of Rx buffer, must be 8,16,32,64,128 or 256
 #define SER_TXBUF_SIZE  32    //Size of TX buffer, must be 8,16,32,64,128 or 256

 //Comment this line if the transmit routines should NOT wait for the bytes to be send via
 //USART if tx buffer is full
 #define SER_WAIT_FOR_TXBUF

 //Uncomment this line if the application does NOT configure the USART
 //#define BAUD_RATE    9600    //USART BAUD rate
 //Comment this line if the application does NOT configures the USART
 #define APP_CONFIGURES_SERPORT  //Our application does all serial port configuration!
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
 * @defgroup mod_sys_serint Serial Port
 * @ingroup mod_sys
 *********************************************************************/


#ifndef _SERINT_H_
#define _SERINT_H_

/////////////////////////////////////////////////
//Global defines
#define SER_USE_INTERRUPT

#ifndef SER_RXBUF_SIZE
#define SER_RXBUF_SIZE  32  //Size of Rx buffer's data area
#endif

#ifndef SER_TXBUF_SIZE
#define SER_TXBUF_SIZE  32  //Size of Tx buffer's data area
#endif


/////////////////////////////////////////////////
//USART CONFIG - only done if application does not do it
#if !defined(APP_CONFIGURES_SERPORT)
    #define USART_USE_BRGH_LOW
    #if defined(USART_USE_BRGH_LOW)
        #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/64) - 1)
    #else
        #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/16) - 1)
    #endif

    #if SPBRG_VAL > 255
        #error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
    #endif
#endif


/////////////////////////////////////////////////
//serStat defines
#define SERINT_RXBUF_OVERRUN    0x01    //The receive buffer had overrun. Must be cleared by the user
#define SERINT_TXBUF_OVERRUN    0x10    //The transmit buffer had overrun. Must be cleared by the user


/////////////////////////////////////////////////
//Global data
extern BYTE idxSerRxbufGet;   //get index
extern BYTE idxSerRxbufPut;   //put index


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
 * Resets this module, and empties all buffers.
 */
void serReset(void);

/**
 * Get the current status of this module
 *
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


#endif    //_SERINT_H_




