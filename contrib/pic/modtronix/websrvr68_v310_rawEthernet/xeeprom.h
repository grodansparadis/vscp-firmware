/**
 * @brief           External serial I2C EEPROM code for Modtronix TCP/IP Stack
 * @file            xeeprom.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 * @section description Description
 **********************************
 * This module contains code for reading and writing to the external serial
 * I2C EEPROM memory. This memory is used to store the web pages contained on the
 * web server.
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2002-05-20, Nilesh Rajbharti:
 *    - Original
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/
#ifndef XEEPROM_H
#define XEEPROM_H

#define EE_BAUD(CLOCK, BAUD)      ( ((CLOCK / BAUD) / 4) - 1 )

typedef enum _XEE_RESULT
{
    XEE_SUCCESS = 0,
    XEE_READY = 0,
    XEE_BUS_COLLISION,
    XEE_NAK,
    XEE_VERIFY_ERR,
    XEE_BUSY
} XEE_RESULT;

typedef unsigned short int XEE_ADDR;


/**
 * Overview:        Initialize I2C module to communicate to serial
 *                  EEPROM.
 * @param baud    - SSPADD value for bit rate.
 */
void XEEInit(unsigned char baud);


/**
 * Modifies internal address counter of EEPROM.
 *
 * This function does not release the I2C bus. User must close
 * XEEClose() after this function is called.
 *
 * @preCondition     XEEInit() is already called.
 *
 * @param control   data EEPROM control code
 * @param address   address to be set
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
BYTE XEESetAddr(unsigned char control, XEE_ADDR address);



/**
 * Sets up internal address counter of EEPROM.
 *
 * This function does not release the I2C bus. User must close XEEEndWrite()
 * after this function is called to relase the I2C bus.
 *
 * @preCondition    XEEInit() is already called.
 *
 * @param control   data EEPROM control code
 * @param address   address to where to write
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
#define XEEBeginWrite(control, address)     XEESetAddr(control, address)


/**
 * Writes given value 'val' at current address. Current address is set
 * by XEEBeginWrite() and is incremented by every XEEWrite().
 *
 * This function does not initiate the write cycle; it simply loads given
 * value into internal page buffer. This function does not release the
 * I2C bus. User must close XEEEndWrite() after this function
 * is called to relase the I2C bus.
 *
 * @preCondition    XEEInit() && XEEBeginWrite() are already called.
 *
 * @param val       Value to be written
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
BYTE XEEWrite(unsigned char val);


/**
 * Instructs EEPROM to begin write cycle.
 *
 * Call this function after either page full of bytes written or no
 * more bytes are left to load. This function initiates the write cycle.
 * User must execute following code after calling this function to ensure that
 * write cycle is finished before calling any other routine:
 * while( XEEIsBusy(EEPROM_CONTROL) ); //Wait until write delay is finished - takes up to 5ms
 *
 * @preCondition     XEEInit() && XEEBeginWrite() are already called.
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
BYTE XEEEndWrite(void);


/**
 * Sets internal address counter to given address.
 * Puts EEPROM in sequential read mode.
 *
 * This function does not release I2C bus. User must call XEEEndRead()
 * when read is not longer needed; I2C bus will released after
 * XEEEndRead() is called.
 *
 * @preCondition    XEEInit() is already called.
 *
 * @param control   EEPROM control and address code.
 * @param address   Address at which read is to be performed.
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
BYTE XEEBeginRead(unsigned char control, XEE_ADDR address);


/**
 * Reads next byte from EEPROM; internal address is incremented by one.
 *
 * This function does not release I2C bus. User must call XEEEndRead()
 * when read is not longer needed; I2C bus will released after
 * XEEEndRead() is called.
 *
 * @preCondition    XEEInit() && XEEBeginRead() are already called.
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
unsigned char XEERead(void);


/**
 * Ends sequential read cycle.
 *
 * This function ends seuential cycle that was in progress. It releases I2C bus.
 *
 * @preCondition     XEEInit() && XEEBeginRead() are already called.
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
BYTE XEEEndRead(void);


/**
 * Reads desired number of bytes in sequential mode. This function
 * performs all necessary steps and releases the bus when finished.
 *
 * @preCondition    XEEInit() is already called.
 *
 * @param control   EEPROM control and address code.
 * @param address   Address from where array is to be read
 * @param buffer    Caller supplied buffer to hold the data
 * @param length    Number of bytes to read.
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
BYTE XEEReadArray(unsigned char control,
                        XEE_ADDR address,
                        unsigned char *buffer,
                        unsigned char length);


/**
 * Requests ack from EEPROM.
 *
 * @preCondition    XEEInit() is already called.
 *
 * @param control   EEPROM control and address code.
 *
 * @return          XEE_READY if EEPROM is not busy <br>
 *                  XEE_BUSY if EEPROM is busy <br>
 *                  other value if failed.
 */
BYTE XEEIsBusy(unsigned char control);

#endif  //#ifndef XEEPROM_H
