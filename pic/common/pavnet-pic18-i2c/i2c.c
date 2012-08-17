/**************************************************************************
 *
 * FILE NAME:			i2c.h
 * FILE DESCRIPTION:	I2C for the PIC1687x
 *
 * FILE CREATION DATE:	26-05-2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 02a,3jan04  erd 	modified to work under picc18f6x2x/picc18f8x2x
 * 01a,26may03 erd	written
 *
 ***************************************************************************/

#include "sysdef.h"
#include "system.h"
#include "i2c_prv.h"

// ==================================================================
// i2c_readStatus()
//
// For debug purposes, this routine reads the status registers to temporary
// vars

/* static void i2c_readStatus()
{
	uint_8 stat, con, con2;
	stat = SSPSTAT;
	con  = SSPCON;
	con2 = SSPCON2;	
}
*/

// ==================================================================
// i2c_init()
//
// Initialize the I2C mode. Currently, this initializes as master
//

BOOL i2c_init()
{
#if SYS_LOADED_FROM_ICD
	// initialize I2C lines
	IO_I2C_I2CCLK = IO_I2C_I2CDAT = 1;

	// set the i2c clk to 400khz
	SSPADD = (SYS_CLK_RATE / (4UL * 400000UL)) - 1;
	
	// initialize the i2c
	// set the SSPCON register to enable I2C and to set master mode
	SSPCON1 = 0B00101000; // 0x28
	
	// set the 'CKP' bit - this enables the CLK
	// CKP = 1;
#endif

	// success
	return TRUE;
}
// ==================================================================
// transmit the START sequence

#define i2c_sendStart()	BIT_SET_WAIT_CLR(SEN)

// ==================================================================
// transmit the repeated START sequence

#define i2c_sendRestart() BIT_SET_WAIT_CLR(RSEN)

// ==================================================================
// transmit an ACK

#define i2c_sendAck() {BIT_SET_WAIT_CLR(ACKEN);}

// ==================================================================
// transmit the control byte

#define i2c_sendControlByte(address, mode) i2c_sendByte((address << 1) | mode)

// ==================================================================
// BOOL i2c_sendByte(uint_8 byte)
//
// Send bytes over current session. A session MUST be initiated with 
// receiver - in master mode, i2c_initiateSession() must be called. In
// slave mode TBD
//
// Arguments:
//	byte: value to be sent to slave
//
// Return value:
//	If TRUE, receiver ACK'ed transaction. If FALSE, receiver did not ACK

BOOL i2c_sendByte(uint_8 byte)
{
	// clear the SSPIF since it will be set on xfer completion
	SSPIF = 0;
	
	// set the byte data into SSPBUF, implicitly starting the transfer
	SSPBUF = byte;

	// SSPIF will be reset on xfer completion
	while (!SSPIF);

	// return whether or not byte was ACK'ed
	return !ACKSTAT;
}
// ==================================================================
// uint_8 i2c_readByte(BOOL last)
//
// read a byte from transmitter over current session. If this byte is
// the last byte to be read, no ACK is required to be sent and a STOP
// sequence is transmitted.
//
// Arguments:
//	last: Whether this is the last byte to be read (also, no writes can be performed)
//
// Return value:
//	byte value read in the transaction

uint_8 i2c_readByte(BOOL last)
{
	uint_8 rxedByte = 0;

	// reset the SSPIF since it will be set on xfer completion
	SSPIF = 0;

	// set receive mode, waiting for that byte to come
	RCEN = 1;

	// and wait until we get it
	while (!SSPIF);

	// if this is not the last byte, we need to ack it, otherwise we
	// will send a NACK (STOP sequence)
	if (!last)
	{
		// send the ACK
		i2c_sendAck();
	}
	else
	{
		// that was the last byte read, transmit the STOP sequnce
		i2c_sendStop();
	}

	// store the byte we received from the SSBUF register
	rxedByte = SSPBUF;

	// and return the byte we got
	return rxedByte;
}

// ==================================================================
// BOOL i2c_initiateSession(I2C_ADDRESS address, I2C_OPERATION_MODE mode, BOOL restarting)
//
// Perform the I2C master session initiation: send the START sequence,
// then a byte with slave address and write mode. In case this is a restart
// the restarting flag can be passed and a RESTART will be sent instead of 
// START
//
// Arguments:
//	address: 7-bit slave address (MSb is ignored)
//	mode: R/W bit 
//	restarting: If this transaction is done to change r/w mode, this flag should be set
//
// Return value: 
//	If TRUE, the slave ACK'ed the byte - if FALSE, no ACK was received

BOOL i2c_initiateSession(I2C_ADDRESS address, I2C_OPERATION_MODE mode, BOOL restarting)
{
	// check if we are restarting: a restart scenarion would be in case
	// we were writing and then decided we need to read, or vice versa. In 
	// this case we need to resent the address control byte with the restart
	// sequence
	if (restarting)
	{
		// must send restart before sending control byte
		i2c_sendRestart();
	}
	else
	{
		// we are in another state, must send start
		i2c_sendStart();
	}
	
	// send the control byte	
	return i2c_sendControlByte(address, mode);
}
// ==================================================================

