/**
 * @brief           I2C Functions
 * @file            i2c.c
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
 * 2006-02-03, David Hosken (DH):
 *    - Implemented code required for second I2C module present on new PIC chips
 * 2005-09-06, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#include "projdefs.h"
#include "net\checkcfg.h" 
#include "net\i2c.h"


/**
 * I2C Task Function. Must be called on a regular basis
 */
void i2cTask(void)
{
    /////////////////////////////////////////////////
    //Check for SPI write collision - bit must be cleared in software
    //
    if (SSPCON1_WCOL) {
        SSPCON1_WCOL = 0;
    }
}


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
 *              SSPADD = ((CLOCK_FREQ / BAUD) / 4) - 1<br>
 *              BAUD = CLOCK_FREQ / (4 * (SSPADR + 1))<br><br>
 *              For example, if the CPU clock frequency is 40MHz and we need a I2C baud rate of 400KHz: <br>
 *              SSPADD = ( ((40,000,000/400,000) / 4) - 1 ) = 24<br><br>
 *              Some common values for SSPADD are:<br>
 *              - 400 KHz at 40 MHz system clock = 24
 *              - 400 KHz at 20 MHz system clock = 12
 *              - 400 KHz at 10 MHz system clock = 5
 *              - 100 KHz at 20 MHz system clock = 49
 */
void i2cOpen(unsigned char mode, unsigned char slew, unsigned char baud)
{
  SSPSTAT &= 0x3F;              // power on state
  SSPCON1 = 0x00;               // power on state
  SSPCON2 = 0x00;               // power on state
  SSPCON1 |= mode;              // select serial mode
  SSPSTAT |= slew;              // slew rate on/off

  TRISC_RC3 = 1;                // Set SCL (PORTC,3) pin to input
  TRISC_RC4 = 1;                // Set SDA (PORTC,4) pin to input. Is controlled by I2C hardware

  SSPCON1 |= SSPENB;            // enable synchronous serial port

  SSPADD = baud;
}

/**
 * Read single byte from I2C bus
 *
 * @param ack Indicates if a ACK is sent after reading the byte. During sequencial read
 * the master will usually send an ACK after each byte read except the last byte. Not
 * sending the ack will indicate to the slave that this is the end of sequential read
 *
 * @return Contents of SSPBUF register
 */
unsigned char i2cGetByte(BOOL ack)
{
    SSPCON2_RCEN = 1;         	// enable master for 1 byte reception

    //At this stage, the slave can use clock streching to give it more time to collect the requested data.
    while ( !SSPSTAT_BF ) FAST_USER_PROCESS();  // wait until byte received
	while ( SSPCON2_RCEN );                     // check that receive sequence is over

    //Send ACK or NACK
    SSPCON2_ACKDT = ack ? 0 : 1;	//Send ACK when 0 and NACK when 1
    SSPCON2_ACKEN = 1;
    while(SSPCON2_ACKEN);           // Wait till finished
    
    return ( SSPBUF );      	// return with read byte
}


/**
 * This routine writes a single byte to the I2C bus,
 * and waits until the bus is idle before returning.
 *
 * @param data_out  Single data byte for I2C bus
 *
 * @param return    Status byte for WCOL detection.<br>
 *                  Returns 0 if OK, else -1 if error
 */
unsigned char i2cPutByte( unsigned char data_out )
{
    SSPBUF = data_out;          // write single byte to SSPBUF
    if ( SSPCON1_WCOL )         // test if write collision occurred
        return ( -1 );          // if WCOL bit is set return negative #
    else
    {
        while( SSPSTAT_BF );    // wait until write cycle is complete
        return ( 0 );           // if WCOL bit is not set return non-negative #
    }
}

/**
 * This routine writes a single byte to the I2C bus,
 * and waits until the bus is idle before returning.
 *
 * @param data_out  Single data byte for I2C bus
 *
 * @return      Status byte for WCOL detection.<br>
 *              Returns 0 if OK, else 1 if error
 */
unsigned char i2cPutByteAndWait(unsigned char data_out) {
    SSPBUF = data_out;          // write single byte to SSPBUF
    if ( SSPCON1_WCOL ) {       // test if write collision occurred
        return ( 1 );           // if WCOL bit is set return negative #
    }

    while( SSPSTAT_BF );        // wait until write cycle is complete

    i2cWaitForIdle();           // wait until bus goes idle

    return ( 0 );               // if WCOL bit is not set return non-negative #
}

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
 /*
unsigned char i2cGetString( unsigned char *rdptr, unsigned char length )
{
    while ( length -- )           // perform getcI2C() for 'length' number of bytes
    {
      *rdptr++ = getcI2C();       // save byte received
      while ( SSPCON2_RCEN );     // check that receive sequence is over    

      if ( PIR2_BCLIF )           // test for bus collision
      {
        return ( -1 );            // return with Bus Collision error 
      }

      if ( length )               // test if 'length' bytes have been read
      {
        SSPCON2_ACKDT = 0;    // set acknowledge bit state for ACK        
        SSPCON2_ACKEN = 1;    // initiate bus acknowledge sequence
        while ( SSPCON2_ACKEN ); // wait until ACK sequence is over 
      } 
    }
    return ( 0 );                 // last byte received so don't send ACK      
}
*/


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
 /*
signed char i2cPutString( unsigned char *wrptr )
{
  while ( *wrptr )                // transmit data until null character 
  {
    if ( SSPCON1_SSPM3 )          // if Master transmitter then execute the following
    {
      if ( putcI2C ( *wrptr ) )   // write 1 byte
      {
        return ( -3 );            // return with write collision error
      }
      IdleI2C();                  // test for idle condition
      if ( SSPCON2_ACKSTAT )      // test received ack bit state
      {
        return ( -2 );            // bus device responded with  NOT ACK
      }                           // terminate putsI2C() function
    }

    else                          // else Slave transmitter
    {
      PIR1_SSPIF = 0;             // reset SSPIF bit
      SSPBUF = *wrptr;            // load SSPBUF with new data
      SSPCON1_CKP = 1;            // release clock line 
      while ( !PIR1_SSPIF );      // wait until ninth clock pulse received

      if ( ( !SSPSTAT_R_W ) && ( !SSPSTAT_BF ) )// if R/W=0 and BF=0, NOT ACK was received
      {
        return ( -2 );            // terminate PutsI2C() function
      }
    }

  wrptr ++;                       // increment pointer

  }                               // continue data writes until null character

  return ( 0 );
}
*/



