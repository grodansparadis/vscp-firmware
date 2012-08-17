///////////////////////////////////////////////////////////////////////////////
// EEPROM access routines
//
// Code collected from the Microchip C18 forum
// Copyright (C) 2003 JasonK
//
// Changes (C) 2005-2012 Ake Hedman, eurosource <akhe@eurosource.se>
//

#include <p18cxxx.h>
#include <vscp_compiler.h> 	// This file should be in your project folder
#include <vscp_projdefs.h>	// This file should be in your project folder
#ifdef EEPROM_STRING
#include <string.h>
#endif
#include <inttypes.h>

///////////////////////////////////////////////////////////////////////////////
// writeEEPROM
//

void writeEEPROM( uint16_t address, uint8_t data ) 
{
    
#if defined(EEADRH) 	
	EEADRH = ( address >> 8 ) & 0xff;
#endif

	EEADR = address & 0xff;	// Data Memory Address to write 
	EEDATA = data; 		// Data Memory Value to write 
	EECON1bits.EEPGD = 0; 	// Point to data memory 
	EECON1bits.CFGS = 0; 	// Access program FLASH or Data EEPROM Memory 
	EECON1bits.WREN = 1; 	// Enable writes 

	//required sequence 
	INTCONbits.GIE = 0; 	// Disable interrupts 
	EECON2 = 0x55; 		// Write 55h 
	EECON2 = 0xaa; 		// Write AAh 
	EECON1bits.WR = 1; 	// Set WR bit to begin write
        INTCONbits.GIE = 1; 	// Enable Interrupts

	while (!PIR2bits.EEIF); // wait for interrupt to signal write complete 
	PIR2bits.EEIF = 0; 	// clear EEPROM write operation interrupt flag 
	EECON1bits.WREN = 0; 	// disable writes on write complete

} 
 
///////////////////////////////////////////////////////////////////////////////
// writeULongEEPROM
//
// Write_ULong_EEPROM 

#ifdef EEPROM_UL
void writeULongEEPROM( uint16_t address, uint8_t data ) 
{ 
	uint8_t i; 

	for ( i=0; i<4; i++ ) 
	writeEEPROM( address+i, *(((char*)(&data))+i) ); 
} 
#endif

///////////////////////////////////////////////////////////////////////////////
// writeStringEEPROM
//
// Write_String_EEPROM 
//

#ifdef EEPROM_STRING
void writeStringEEPROM( uint16_t address, uint8_t string[] ) 
{ 
	uint8_t i,string_len; 

	string_len = strlen( string ); 
	for ( i=0; i<string_len; i++ ) 
	writeEEPROM( address+i, *( string+i ) ); 
} 
#endif

///////////////////////////////////////////////////////////////////////////////
// readEEPROM
//
// Read_EEPROM 
//

uint8_t readEEPROM( uint16_t address ) 
{ 
	uint8_t data; 

#if defined(EEADRH)	
	EEADRH = ( address >> 8 ) & 0xff;
#endif	
	EEADR = address & 0xff; // Data Memory Address to write 
	EECON1bits.EEPGD= 0; 	// Point to data memory 
	EECON1bits.CFGS = 0; 	// Access program FLASH or Data EEPROM Memory 
	EECON1bits.RD = 1; 		// Enable read 
	data = EEDATA; 

	return data; 
} 
 

///////////////////////////////////////////////////////////////////////////////
// readULongEEPROM
//
// Read_ULong_EEPROM //long EEPROM 
//

#ifdef EEPROM_UL
uint32_t readULongEEPROM( uint16_t address ) 
{ 
	unsigned long long_read; 
	unsigned char i; 
	
	long_read = 0x00000000; 
	for ( i=0; i<4; i++ ) { 
		long_read = long_read << 8; 
		long_read |= readEEPROM( address + i ); 
	} 
	
	return long_read; 
} 
#endif 

///////////////////////////////////////////////////////////////////////////////
// readStringEEPROM
//

#ifdef EEPROM_STRING
void readStringEEPROM( uint16_t address, 
						uint8_t string[], 
						uint8_t final_character ) 
{ 
	uint8_t char_read; 
	uint8_t i; 

	char_read = 0; 
	i = 0; 
	while( char_read != final_character ) {
		char_read = readEEPROM(address+i); 
		*string = char_read; 
		string++;
		i++;
	} 
}
#endif
