///////////////////////////////////////////////////////////////////////////////
//
// File: eeprom.c
//
// I2C EEPROM handler
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: eeprom.c,v $
// $Revision: 1.8 $
//

#include "types.h"
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lpc213x.h"
#include "config.h"
#include "armVIC.h"
#include "eeprom.h"
#include "sysTime.h"

#define MAX_I2C_DATA	8

unsigned char	I2CData[ MAX_I2C_DATA ];
unsigned char	*pI2CData;
uint8_t	I2Counter;
uint8_t	I2CAddress;
volatile boolean bI2c_lock;

///////////////////////////////////////////////////////////////////////////////
// i2c1Init
//

void i2cInit( void )
{
	bI2c_lock = FALSE;						// Initilise the lock flag

	//PINSEL0  |= 0x30c00000; 				// Activate I2C 1
	PINSEL0 = ( PINSEL0 & ~0xF0 ) | 0x50; 	// Activate I2C 0
	
	// initialize the interrupt vector
	VICIntSelect &= ~VIC_BIT (VIC_I2C0 );	// I2C selected as IRQ
	VICVectCntl2 = VIC_ENABLE | VIC_I2C0;
	VICVectAddr2 = (uint32_t)I2CISR;    	// address of the ISR
	VICIntEnable = VIC_BIT( VIC_I2C0 );    	// I2C interrupt enabled
	
	I20SCLH = 75;	// Set bit rate 60.0000hz / (SCLH + SCLL ) = 60.0000/(75+75) = 400.0Khz
	I20SCLL = 75;
}

///////////////////////////////////////////////////////////////////////////////
// writeEEPROM
//
// return: True on success.

boolean writeEEPROM( uint16_t addr, uint8_t data )
{
	boolean rv;
	
	rv = I2CTransferByte( EEPROM_ADDR, addr, 1, (unsigned char *)&data );
	pause( TEN_MS );	// HACK!!! TODO Remove
	
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
// readEEPROM
//
// Return: -1 on failure, data on success.

int16_t readEEPROM( uint16_t addr )
{
	unsigned mycpsr;
	uint32_t start;
	
	// Write address
	if ( !I2CTransferByte( EEPROM_ADDR, addr, 0, NULL ) ) {
		return -1;	
	}
	
	//  Read content of address
	if ( !I2CTransferByte( EEPROM_ADDR + 1, addr, 1, NULL ) ) {	
		return -1;
	}
	
	// Wait until data is available
	start = getSysTICs();
	while ( bI2c_lock ) {			// Wait for interrupt to signal end of I2C activity
		
		if ( getElapsedSysTICs( start ) > HUNDRED_MS ) { 
			return -1;
		}
		
		mycpsr = disableIRQ();
		WDFEED = 0xAA; WDFEED = 0x55;
		restoreIRQ( mycpsr );
	}
	
	return I2CData[ 0 ];
}

///////////////////////////////////////////////////////////////////////////////
// I2CTransferByte
//

boolean I2CTransferByte( unsigned char I2CAddr,
							unsigned short memAddr,
							unsigned char count,
							unsigned char *pData )
{
	unsigned mycpsr;
	unsigned char i;
	uint32_t start;

	// CAN only hadle MAX_I2C_DATA + sizeof( addr )
	if ( I2CAddr & 1 ) {
		if ( count > MAX_I2C_DATA ) return FALSE;
	}
	else {
		if ( count > ( MAX_I2C_DATA - 2 ) ) return FALSE;
	}
	
	// Wait until last I2C operation has finished.
	start = getSysTICs();
	while ( bI2c_lock ) {			// Wait for interrupt to signal end of I2C activity
		
		if ( getElapsedSysTICs( start ) > HUNDRED_MS ) {
			return FALSE;
		}
		
		mycpsr = disableIRQ();
		WDFEED = 0xAA; WDFEED = 0x55;
		restoreIRQ( mycpsr );
		
	}
	
	bI2c_lock = TRUE;                  		// Set I2C bus as active
	I2CAddress = I2CAddr;					// Sava I2C address

	// If write operation the address should also be sent
	if ( I2CAddr & 1 ) {
	
		// Read
		I2Counter = count;
		pI2CData = pData;					// Point at user buffer
		
	}
	else {
	
		// Write
		I2Counter = count + 2;				// Add address length to count
		I2CData[ 0 ] = ( memAddr >> 8 );	// MSB of memeory address
		I2CData[ 1 ] = memAddr & 0xff;		// LSB of address
		
		for ( i=0; i<count; i++ ) {
			I2CData[ i + 2 ] = pData[ i ];
		}
		
		pI2CData = I2CData;		// Point at internal buffer

	}
	
	I20CONCLR 	= 0x000000FF;	// Clear all I2C settings
	I20CONSET 	= 0x00000040; 	// Enable the I2C interface
	I20CONSET 	= 0x00000020; 	// Start condition
			
	return TRUE;
}





///////////////////////////////////////////////////////////////////////////////
// I2CISR
//

void I2CISR( void )					// I2C interrupt routine
{
	// perform proper ISR entry so thumb-interwork works properly
	ISR_ENTRY();
	
	switch ( I20STAT ) {				// Read result code and switch to next action
	
		// Start and Send byte conditions
		case 0x00:						// Bus errror
			I20CONSET = 0x10;			// STOP 
			bI2c_lock = FALSE;
			break;
		
		case 0x08:						// Start bit sent
			I20DAT = I2CAddress; 		// Send address and write bit
			I20CONCLR = 0x20;			// Clear start flag
			I20CONSET = 0x04;			// Set ACK bit
			break;

		case 0x10:
			I20DAT = I2CAddress; 		// Send address and write bit
			I20CONSET = 0x04;			// Set ACK bit
			break;

		case 0x18:						// Start Data
			I20DAT = *pI2CData++;		// Write data to tx register
			break;
			
		case 0x20:						// Slave address+W, Not ACK
			I20CONSET = 0x14;			// Set STOP and ACK bits
			bI2c_lock = FALSE;
			break;
		
		case 0x28:						// Data sent, ACK
			I2Counter--;
			if ( 0 == I2Counter ) {	
				I20CONSET = 0x14;		// Set STOP
				bI2c_lock = FALSE;
			}
			else {
				I20DAT = *pI2CData++;	// Write data to tx register
				I20CONSET = 0x04;		// Set AA bit
			}
			break;

		case 0x30:						// Data sent, NOT Ack
			I20CONSET = 0x14;			// Set STOP and ACK bits
			bI2c_lock = FALSE;
			break;

		case 0x38:						// Arbitration lost
			I20CONSET = 0x24;			// Set STAT and ACK bits
			break;
			
		// * * * Receive byte conditions
		case 0x40:						// Slave Address + R, ACK
			I20CONSET = 0x04;			// Enable ACK for data byte
			break;

		case 0x48:						// Slave Address +R, Not Ack
			I20CONSET = 0x10;			// Set STOP and ACK bits
			bI2c_lock = FALSE;
			break;

		case 0x50:						// Data Received, ACK 
			I2CData[ 0 ] = I20DAT;
			I20CONSET = 0x10;			// Set STOP bit
			bI2c_lock = FALSE;
			break;

		case 0x58:						// Data Received, Not Ack
			I2CData[ 0 ] = I20DAT;
			I20CONSET = 0x10;			// Set STOP bit
			bI2c_lock = FALSE;
			break;

		default:
			break;

	}

	I20CONCLR = 0x08;					// Clear I2C interrupt flag
	VICVectAddr = 0x00000000;			// Clear interrupt in 

	ISR_EXIT();                         // recover registers and return
}
