/**************************************************************************
 *
 * FILE NAME:			eeprom.h
 * FILE DESCRIPTION:	EEPROM for the PIC1687x private header
 *
 * FILE CREATION DATE:	26-05-2003
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,26may03 erd
 *
 ***************************************************************************/

#ifndef __EEPROM_H_
#define __EEPROM_H_

// ==========================================================================
// types

typedef enum
{
	EEPROM_OK,

} EEPROM_STATUS;

// ==========================================================================
// External interface

// eeprom_init()
//
// Perform the EEPROM initialization
//
extern void eeprom_init();

// eeprom_writeByte(byte)
//
// Write a byte to the eeprom
//
#define eeprom_writeByte eeprom_write

// eeprom_writeBuffer(uint_8 *buffer, uint_8 size,  uint_8 offset)
//
// Write a buffer to the eeprom
//
extern EEPROM_STATUS eeprom_writeBuffer(uint_8 *buffer, uint_8 size,  uint_8 offset);

// eeprom_readByte(byte)
//
// Read a byte from the eeprom
//
#define eeprom_readByte eeprom_read

// eeprom_readBuffer( uint_8 offset, uint_8 *buffer, uint_8 size)
//
// Read data from the eeprom
//
extern EEPROM_STATUS eeprom_readBuffer( uint_8 offset, uint_8 *buffer, uint_8 size);


#endif /* __EEPROM_H_ */
