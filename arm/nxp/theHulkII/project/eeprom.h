///////////////////////////////////////////////////////////////////////////////
// File: eeprom.h
//
// I2C EEPROM handler
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: eeprom.h,v $
// $Revision: 1.3 $
//

union {

	unsigned short int Val;
    struct {
		unsigned char LSB;
        unsigned char MSB;
    } bytes;
	
} memAddress;
  
// Read address for eeprom  
#define EEPROM_ADDR		0xA8 
 
extern volatile boolean bI2c_lock;

// Initialize I2C
void i2cInit( void );

// I2C interrupt routine
void I2CISR( void );	
																			
// Background call to start master send and receive byte transfers
boolean I2CTransferByte( unsigned char I2CAddr,
							unsigned short MemAddr,
							unsigned char count,
							unsigned char *pData );

// Write eeprom
boolean writeEEPROM( uint16_t addr, uint8_t data );

// Read eeprom
int16_t readEEPROM( uint16_t addr );
