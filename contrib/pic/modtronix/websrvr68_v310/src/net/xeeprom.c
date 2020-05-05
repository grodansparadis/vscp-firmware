/**
 * @brief           External Data EEPROM Access Routines
 * @file            xeeprom.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 **********************************************************************
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
 * 2005-12-21, David Hosken (DH):
 *    - External EEPROM code completely redone. Put all I2C code in seperate file
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 * 2002-05-20, Nilesh Rajbharti:
 *    - Original
 *********************************************************************/

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\xeeprom.h"
#include "net\i2c.h"

BYTE XEEClose(void);

/**
 * Overview:        Initialize I2C module to communicate to serial
 *                  EEPROM.
 * @param baud    - SSPADD value for bit rate.
 */
void XEEInit(unsigned char baud)
{
    //Open I2C port
    i2cOpen(I2C_MASTER, I2C_SLEW_ON, baud);
}


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
BYTE XEESetAddr(unsigned char control, XEE_ADDR address)
{
    union
    {
        unsigned short int Val;
        struct
        {
            unsigned char LSB;
            unsigned char MSB;
        } bytes;
    } tempAddress;

    tempAddress.Val = address;

    i2cWaitForIdle();               // ensure module is idle

    i2cPutStartAndWait();           // initiate START condition, and wait for it to complete
    
    if ( i2cBusCollision() )        //Did a bus collision occur
        return XEE_BUS_COLLISION;   // return with Bus Collision error

    if ( i2cPutByte( control ) )    // write 1 byte
        return XEE_BUS_COLLISION;   // set error for write collision

    i2cWaitForIdle();               // ensure module is idle
    
    if ( i2cWasAckReceived() )      //Was an ACK received from the slave
    {
        if ( i2cPutByte( tempAddress.bytes.MSB ) )  // WRITE word address to EEPROM
            return XEE_BUS_COLLISION;               // return with write collision error

        i2cWaitForIdle();                  // ensure module is idle

        if ( i2cPutByte( tempAddress.bytes.LSB ) )  // WRITE word address to EEPROM
            return XEE_BUS_COLLISION;               // return with write collision error

        i2cWaitForIdle();           // ensure module is idle
        
        if ( i2cWasAckReceived() )      //Was an ACK received from the slave
            return XEE_SUCCESS;
        else
            return XEE_NAK;         // return with Not Ack error
    }
    else
        return XEE_NAK;             // return with Not Ack error
}


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
BYTE XEEBeginRead(unsigned char control, XEE_ADDR address )
{
    unsigned char r;

    r = XEESetAddr(control, address);
    if ( r != XEE_SUCCESS )
        return r;

    r = XEEClose();
    if ( r != XEE_SUCCESS )
        return r;


    i2cWaitForIdle();

    i2cPutStartAndWait();           // initiate START condition, and wait for it to complete

    if ( i2cBusCollision() )        //Did a bus collision occur    
        return XEE_BUS_COLLISION;

    if ( i2cPutByte(control+1) )
        return XEE_BUS_COLLISION;

    i2cWaitForIdle();
    
    if ( i2cWasAckReceived() )      //Was an ACK received from the slave    
        return XEE_SUCCESS;
        
    return XEE_NAK;
}


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
BYTE XEEWrite(unsigned char val)
{
    i2cWaitForIdle();                  // ensure module is idle

    if ( i2cPutByte( val ) )   // data byte for EEPROM
        return XEE_BUS_COLLISION;          // set error for write collision

    i2cWaitForIdle();
    
    if ( i2cWasAckReceived() )      //Was an ACK received from the slave
        return XEE_SUCCESS;
        
    return XEE_NAK;
}


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
BYTE XEEEndWrite(void)
{
    i2cWaitForIdle();
    
    i2cPutStopAndWait();    //Put a stop condition, and wait until it is finished
    
    return XEE_SUCCESS;
}


/**
 * Reads next byte from EEPROM; internal address is incremented by one.
 *
 * This function does not release I2C bus. User must call XEEEndRead()
 * when read is not longer needed; I2C bus will released after
 * XEEEndRead() is called.
 *
 * @preCondition    XEEInit() && XEEBeginRead() are already called.
 *
 * @return          Returns the value read.
 */
unsigned char XEERead(void)
{
    i2cGetByte(1);          // Read byte from I2C slave, sent ACK (means not finished reading yet)

    return SSPBUF;
}


/**
 * Ends sequential read cycle.
 *
 * This function ends seuential cycle that was in progress. It releases I2C bus.
 *
 * @preCondition     XEEInit() && XEEBeginRead() are already called.
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
BYTE XEEEndRead(void)
{
    i2cGetByte(0);			// Read byte from I2C slave, sent NACK (means finished reading)

    i2cPutStopAndWait();    //Put a stop condition, and wait until it is finished

    return XEE_SUCCESS;
}


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
                        unsigned char length)
{
    BYTE r;

    r = XEEBeginRead(control, address);
    if ( r != XEE_SUCCESS )
        return r;

    while( length-- ) {
        i2cGetByte(1);          // Read byte from I2C slave, sent ACK (means not finished reading yet)
        *buffer++ = SSPBUF;
    }

    r = XEEEndRead();

    return r;
}


BYTE XEEClose(void)
{
    i2cWaitForIdle();
    
    i2cPutStopAndWait();    //Put a stop condition, and wait until it is finished
    
    if ( i2cBusCollision() )        //Did a bus collision occur
        return XEE_BUS_COLLISION;
        
    return XEE_SUCCESS;
}


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
BYTE XEEIsBusy(unsigned char control)
{
    BYTE r;

    i2cWaitForIdle();               // ensure module is idle
                                    
    i2cPutStartAndWait();           // initiate START condition, and wait for it to complete
    
    if ( i2cBusCollision() )        //Did a bus collision occur
    {
        return XEE_BUS_COLLISION;   // return with Bus Collision error
    }

    else
    {
        if ( i2cPutByte( control ) )      // write byte - R/W bit should be 0
            return XEE_BUS_COLLISION;              // set error for write collision

        i2cWaitForIdle();                      // ensure module is idle
        
        if ( i2cBusCollision() )        //Did a bus collision occur
            return XEE_BUS_COLLISION;   // return with Bus Collision error

        if ( i2cWasAckReceived() )      //Was an ACK received from the slave
            r = XEE_READY;
        else
            r = XEE_BUSY;

#if 0
        while ( SSPCON2_ACKSTAT )       // test for ACK condition received
        {
            i2cRestart();               // initiate Restart condition
            while ( SSPCON2_RSEN );     // wait until re-start condition is over
            
            if ( i2cBusCollision() )        //Did a bus collision occur
                return XEE_BUS_COLLISION;   // return with Bus Collision error

            if ( i2cPutByte( control ) )  // write byte - R/W bit should be 0
                return XEE_BUS_COLLISION;  // set error for write collision

            i2cWaitForIdle();                  // ensure module is idle
        }
#endif
    }

    i2cPutStopAndWait();    //Put a stop condition, and wait until it is finished
    
    if ( i2cBusCollision() )        //Did a bus collision occur
        return XEE_BUS_COLLISION;   // return with Bus Collision error

    return r;
    //return XEE_READY;             // return with no error
}


