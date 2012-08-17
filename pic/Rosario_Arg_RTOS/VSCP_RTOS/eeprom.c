///////////////////////////////////////////////////////////////////////////////
// EEPROM access routines
//
// Code collected from the Microchip C18 forum
// Copyright (C) 2003 JasonK
//
// Changes (C) 2005 Ake Hedman, eurosource <akhe@eurosource.se>
//

#include <p18cxxx.h>
#include <eeprom.h>


///////////////////////////////////////////////////////////////////////////////
// writeEEPROM
//

void writeEEPROM( uint16_t address, uint08_t data ) 
{ 
	EEADR = address & 0xff;	// Data Memory Address to write 
	EEDATA = data; 			// Data Memory Value to write 
	EECON1bits.EEPGD = 0; 	// Point to data memory 
	EECON1bits.CFGS = 0; 	// Access program FLASH or Data EEPROM Memory 
	EECON1bits.WREN = 1; 	// Enable writes 

	//required sequence 
	INTCONbits.GIE = 0; 	// Disable interrupts 
	EECON2 = 0x55; 			// Write 55h 
	EECON2 = 0xaa; 			// Write AAh 
	EECON1bits.WR = 1; 		// Set WR bit to begin write 
	INTCONbits.GIE = 1; 	// Enable Interrupts 

	while (!PIR2bits.EEIF); // wait for interrupt to signal write complete 
	PIR2bits.EEIF = 0; 		// clear EEPROM write operation interrupt flag 
	EECON1bits.WREN = 0; 	// disable writes on write complete 
} 
 

///////////////////////////////////////////////////////////////////////////////
// readEEPROM
//
// Read_EEPROM 
//

uint08_t readEEPROM( uint16_t address ) 
{ 
	uint08_t data; 
	
	EEADR = address & 0xff; // Data Memory Address to write 
	EECON1bits.EEPGD= 0; 	// Point to data memory 
	EECON1bits.CFGS = 0; 	// Access program FLASH or Data EEPROM Memory 
	EECON1bits.RD = 1; 		// Enable read 
	data = EEDATA; 

	return data; 
} 
 

