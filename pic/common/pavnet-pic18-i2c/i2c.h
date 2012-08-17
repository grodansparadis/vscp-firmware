/**************************************************************************
 *
 * FILE NAME:			i2c.h
 * FILE DESCRIPTION:	I2C for the PIC1687x header
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
 * 01a,26may03 erd	written
 *
 ***************************************************************************/

#ifndef __I2C_H_
#define __I2C_H_

// ==================================================================
// types

// operation mode sent as the LSb in the control byte
typedef enum
{
	I2C_OPM_WRITE = 0,	// perform a write
	I2C_OPM_READ  = 1	// perform a read

} I2C_OPERATION_MODE;

// an i2c address
typedef uint_8 I2C_ADDRESS;

// ==========================================================================
// External interface

// i2c_init()
//
// Initialize the I2C mode. Currently, this initializes as master
//

extern BOOL i2c_init();


// ==================================================================
// i2c_sendStop()
//
// Send the STOP sequence - required for when the last byte is written
// 

#define i2c_sendStop() BIT_SET_WAIT_CLR(PEN)


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

extern BOOL i2c_sendByte(uint_8 byte);


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

extern uint_8 i2c_readByte(BOOL last);


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

extern BOOL i2c_initiateSession(I2C_ADDRESS address, I2C_OPERATION_MODE mode, BOOL restarting);


#endif /* __I2C_H_ */
