/**
 * @brief           I2C Functions
 * @file            i2c.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 * @section description Description
 **********************************
 * This module contains code for reading and writing devices on the I2C bus.
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
 * 2005-09-06, David Hosken (DH):
 *    - Initial version
 *********************************************************************/
#ifndef I2C_H
#define I2C_H

/** Define SSP defines for some processors where it is not done */


/** Enable serial port and configures SCK, SDO, SDI*/
#define   SSPENB    0x20ul

//@{
/**
 * The following defines are used for the <i>mode</i> parameter in the i2cOpen() function:
 * - I2C_SLAVE_7    I2C Slave mode, 7-bit address
 * - I2C_SLAVE_10   I2C Slave mode, 10-bit address
 * - I2C_MASTER I2C Master mode
 */
#define   I2C_SLAVE_7   6ul     /* I2C Slave mode, 7-bit address    */
#define   I2C_SLAVE_10  7ul     /* I2C Slave mode, 10-bit address   */
#define   I2C_MASTER    8ul     /* I2C Master mode                  */
//@}

//@{
/**
 * The following defines are used for the <i>slew</i> parameter in the i2cOpen() function:
 * - I2C_SLEW_OFF       Slew rate disabled for 100kHz mode
 * - I2C_SLEW_ON        Slew rate enabled for 400kHz mode
 * - I2C_SLEW_OFF_SMBUS Slew rate disabled for 100kHz mode, SMBus inputs enabled
 * - I2C_SLEW_ON_SMBUS  Slew rate enabled for 400kHz mode, SMBus inputs enabled
 */
#define   I2C_SLEW_OFF          0x80ul      /* Slew rate disabled for 100kHz mode */
#define   I2C_SLEW_ON           0x00ul      /* Slew rate enabled for 400kHz mode  */
#define   I2C_SLEW_OFF_SMBUS    0xC0ul      /* Slew rate disabled for 100kHz mode, SMBus inputs */
#define   I2C_SLEW_ON_SMBUS     0x40ul      /* Slew rate enabled for 400kHz mode, SMBus inputs */
//@}


/////////////////////////////////////////////////
//Parts with one I2C modules
/////////////////////////////////////////////////

/**
 * Generate bus stop condition
 */
#define i2cPutStop()  (SSPCON2_PEN=1)

/**
 * Generate bus stop condition, and waits until it is finished
 */
#define i2cPutStopAndWait()  {SSPCON2_PEN=1; while(SSPCON2_PEN);}

/**
 * Generate bus start condition
 */
#define i2cPutStart()  (SSPCON2_SEN=1)

/**
 * Generate bus start condition, and waits until it is finished
 */
#define i2cPutStartAndWait()  {SSPCON2_SEN=1; while ( SSPCON2_SEN );}

/**
 * Generate bus restart condition
 */
#define i2cRestart()    (SSPCON2_RSEN=1)

/**
 * Generate bus restart condition, and waits until it is finished
 */
#define i2cRestartAndWait()    {SSPCON2_RSEN=1; while (SSPCON2_RSEN);}

/**
 * Generate bus Not ACK condition.
 */
#define i2cPutNack()     {SSPCON2_ACKDT=1; SSPCON2_ACKEN=1;}

/**
 * Generate bus Not ACK condition, and waits until it is finished
 */
#define i2cPutNackAndWait()     {SSPCON2_ACKDT=1; SSPCON2_ACKEN=1; while(SSPCON2_ACKEN); /*Wait till finished*/}

/**
 * Generate bus ACK condition
 */
#define i2cPutAck()        {SSPCON2_ACKDT=0; SSPCON2_ACKEN=1;}

/**
 * Generate bus ACK condition, and waits until it is finished
 */
#define i2cPutAckAndWait()        {SSPCON2_ACKDT=0; SSPCON2_ACKEN=1;while(SSPCON2_ACKEN); /*Wait till finished*/}

/**
 * Waits until the I2C bus is IDLE, and available to be used.
 * The i2cWaitForIdle() function is required since the
 * hardware I2C peripheral does not allow for spooling of bus sequences.
 * The I2C peripheral must be in an IDLE state before an I2C operation
 * can be initiated or a write collision will be generated.
 */
#define i2cWaitForIdle()    while ((SSPCON2 & 0x1F) | (SSPSTAT_R_W))

/**
 * Checks if the I2C bus is IDLE, and available to be used.
 * The i2cIsIdle() function is required since the
 * hardware I2C peripheral does not allow for spooling of bus sequences.
 * The I2C peripheral must be in an IDLE state before an I2C operation
 * can be initiated or a write collision will be generated.
 *
 * @return  TRUE if I2C bus is idle and can be used <br>
 *          FALSE if I2C bus is NOT idle. User has to wait until bus is idle before using it.
 */
#define i2cIsIdle()    (((SSPCON2 & 0x1F) | (SSPSTAT_R_W)) ? FALSE : TRUE)


/**
 * Tests if an ACK was received from the slave. Returns true if it was, else false.
 */
#define i2cWasAckReceived() (!SSPCON2_ACKSTAT)


/**
 * Test if a bus collision has occured. Return TRUE if it did, else FALSE
 */
#define i2cBusCollision() PIR2_BCLIF


/**
 * Disable I2C module
 */
#define i2cClose()  (SSPCON1 &= 0xDF)


/**
 * I2C Task Function. Must be called on a regular basis
 */
void i2cTask(void);


/**
 * This function resets the SSP module to the POR state and then configures
 * the module for Master/Slave mode and the selected slew rate.
 *
 * @param mode   One of the following values defined in i2c.h:
 *              - I2C_SLAVE_7       I2C Slave mode, 7-bit address
 *              - I2C_SLAVE_10      I2C Slave mode, 10-bit address
 *              - I2C_MASTER I2C    Master mode
 * @param slew   One of the following values, defined in i2c.h:
 *              - I2C_SLEW_OFF Slew rate disabled for 100 kHz mode
 *              - I2C_SLEW_ON Slew rate enabled for 400 kHz mode
 * @param baud  The baud rate of the I2C unit when configured for I2C Master mode.
 *              This is the value that will be assigned to the SSPADD register.
 *              It is calculated as follows: <br>
 *              SSPADD = ( ((CLOCK_FREQ / BAUD) / 4) - 1 ) <br><br>
 *              For example, if the CPU clock frequency is 40MHz and we need a I2C baud rate of 400KHz: <br>
 *              SSPADD = ( ((40,000,000/400,000) / 4) - 1 ) = 24<br><br>
 *              Some common values for SSPADD are:<br>
 *              - 400 KHz at 40 MHz system clock = 24
 *              - 400 KHz at 20 MHz system clock = 12
 *              - 400 KHz at 10 MHz system clock = 5
 */
void i2cOpen(unsigned char mode, unsigned char slew, unsigned char baud);


/**
 * Checks if the SSP buffer contains any data that can be read.
 *
 * Example:
 * @code
 *  if (i2cDataRdy())
 *  {
 *      var = i2cGetByte();
 *  }
 * @endcode
 *
 * @return  1 if there is data in the SSP buffer
 *          0 if there is no data in the SSP buffer
 */
#define i2cDataRdy()    (SSPSTAT_BF)


/**
 * Read single byte from I2C bus
 *
 * @param ack Indicates if a ACK is sent after reading the byte. During sequencial read
 * the master will usually send an ACK after each byte read except the last byte. Not
 * sending the ack will indicate to the slave that this is the end of sequential read
 *
 * @return Contents of SSPBUF register
 */
unsigned char i2cGetByte(BOOL ack);

/**
 * This routine writes a single byte to the I2C bus,
 * and waits until the bus is idle before returning.
 *
 * @param data_out  Single data byte for I2C bus
 *
 * @return    Status byte for WCOL detection.<br>
 *                  Returns 0 if OK, else 1 if error
 */
unsigned char i2cPutByte(unsigned char data_out);

/**
 * This routine writes a single byte to the I2C bus,
 * and waits until the bus is idle before returning.
 *
 * @param data_out  Single data byte for I2C bus
 *
 * @return      Status byte for WCOL detection.<br>
 *              Returns 0 if OK, else 1 if error
 */
unsigned char i2cPutByteAndWait(unsigned char data_out);

/**
 * This routine reads a predetemined string length in from the I2C bus.
 * The routine is developed for the Master mode. The bus ACK
 * condition is generated within this routine.
 *
 * @param rdptr     address of read string storage location
 * @param length    length of string bytes to read
 *
 * @return          error condition status
 */
//unsigned char i2cGetString( unsigned char *rdptr, unsigned char length );


/**
 * This routine writes a string to the I2C bus, until a null character
 * is reached. If Master function putcI2C is called. When transmission
 * is complete then test for acknowledge bit. If Slave transmitter wait
 * for null character or not ACK received from bus device.
 *
 * @param wrprt     address of write string storage location
 *
 * @return          error condition status
 */
//signed char i2cPutString( unsigned char *wrptr );



/////////////////////////////////////////////////
//These devices have two I2C modules
/////////////////////////////////////////////////
#if defined(__18F64J15) || defined(__18F65J10) || defined(__18F65J15) || \
    defined(__18F66J10) || defined(__18F66J15) || defined(__18F67J10) || \
    defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || \
    defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || \
    defined(__18F6527)  || defined(__18F6622)  || \
    defined(__18F6627)  || defined(__18F6722)  || \
    defined(__18F8527)  || defined(__18F8622)  || \
    defined(__18F8627)  || defined(__18F8722)  || \
    defined(__18F44J10) || defined(__18F45J10)


/* i2cStop2
 * Generate bus stop condition
 */
#define i2c2PutStop()  (SSP2CON2_PEN=1)

/**
 * Generate bus stop condition, and waits until it is finished
 */
#define i2c2PutStopAndWait()  {SSP2CON2_PEN=1; while(SSP2CON2_PEN);}

/* i2cStart2
 * Generate bus start condition
 */
#define i2c2PutStart()  (SSP2CON2_SEN=1)

/**
 * Generate bus start condition, and waits until it is finished
 */
#define i2c2PutStartAndWait()  {SSP2CON2_SEN=1; while ( SSP2CON2_SEN );}

/* i2cRestart2
 * Generate bus restart condition
 */
#define i2c2Restart()  (SSP2CON2_RSEN=1)

/*
 * Generate bus Not ACK condition
 */
#define i2c2PutNack()     {SSP2CON2_ACKDT=1; SSP2CON2_ACKEN=1;}

/* i2cAck2
 * Generate bus ACK condition
 */
#define i2c2PutAck()        {SSP2CON2_ACKDT=0; SSP2CON2_ACKEN=1;}

/* Idle I2C2
 * Test if I2C2 module is idle
 */
#define i2c2WaitForIdle()    while ((SSP2CON2 & 0x1F) | (SSP2STAT2_R_W))

/**
 * Tests if an ACK was received from the slave. Returns true if it was, else false.
 */
#define i2c2WasAckReceived() (!SSP2CON2_ACKSTAT)

/**
 * Test if a bus collision has occured. Return TRUE if it did, else FALSE
 */
#define i2c2BusCollision() PIR3_BCL2IF

/* i2c2Close
 * Disable SPI module
 */
#define i2c2Close()  (SSP2CON1 &=0xDFul)

/* i2cOpen2
 * Configure SSP module for use with I2C2 EEPROM or I2C2 bus device.
 */
void i2c2Open( unsigned char sync_mode, unsigned char slew );

/* i2cDataRdy2
 * Test if SSPBUF register is full
 */
#define i2c2DataRdy()    (SSP2STAT_BF)

/**
 * Read single byte from I2C bus
 *
 * @return Contents of SSPBUF register
 */
unsigned char i2c2GetByte(void);

/**
 * This routine writes a single byte to the I2C bus.
 *
 * @param data_out  Single data byte for I2C bus
 *
 * @param return    Status byte for WCOL detection.<br>
 *                  Returns 0 if OK, else 1 if error
 */
unsigned char i2c2PutByte( unsigned char data_out );

/**
 * This routine writes a single byte to the I2C bus,
 * and waits until the bus is idle before returning.
 *
 * @param data_out  Single data byte for I2C bus
 *
 * @return      Status byte for WCOL detection.<br>
 *              Returns 0 if OK, else 1 if error
 */
unsigned char i2c2PutByteAndWait(unsigned char data_out);

/**
 * This routine reads a predetemined string length in from the I2C bus.
 * The routine is developed for the Master mode. The bus ACK
 * condition is generated within this routine.
 *
 * @param rdptr     address of read string storage location
 * @param length    length of string bytes to read
 *
 * @return          error condition status
 */
//unsigned char i2c2GetString( unsigned char *rdptr, unsigned char length );


/**
 * This routine writes a string to the I2C bus, until a null character
 * is reached. If Master function putcI2C is called. When transmission
 * is complete then test for acknowledge bit. If Slave transmitter wait
 * for null character or not ACK received from bus device.
 *
 * @param wrprt     address of write string storage location
 *
 * @return          error condition status
 */
//signed char i2c2PutString( unsigned char *wrptr );
#endif  //#if defined(processors................

#endif  //#ifndef I2C_H
