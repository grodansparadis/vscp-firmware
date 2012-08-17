/*************************************************************
* EEPROM data memory interface for PIC16F87X devices 
*
* Allows for read and write of the program memory. 
*
* Warning! Interrupts are disabled during write !
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/
#include "eeprom.h"
#include "serial.h"

////////////////////////////////////////////////////////////
// Read an EEPROM memory location. Takes the address as input 
// and returns the data at the memory location.
//
// The function is based on the Microchip document DS39582A
// page 33.
////////////////////////////////////////////////////////////
char eeprom_read(char address){

    // Load address
    eeadr = address;

	// Point to EEPROM memory
	eecon1.EEPGD = 0;
	
	// 18F devices also need the CFGS bit to be cleared
#ifdef CFGS
	eecon1.CFGS = 0;
#endif

	// Read, data is available in eedata the next cycle.
	eecon1.RD = 1;
	
	// Return value
	return eedata;

}

////////////////////////////////////////////////////////////
// Writes a byte to the data EEPROM. 
// Interrupts are disabled during this routine.
//
// The function is based on the Microchip document DS39582A
// page 33.
////////////////////////////////////////////////////////////
void eeprom_write(char address, char data){
	
	// 18F devices also need the CFGS bit to be cleared
#ifdef CFGS
	eecon1.CFGS = 0;
#endif	

	// Wait for write to complete
	while (test_bit(eecon1, WR)){};

	// Load address and data
	eeadr = address;
	eedata = data;

	// Point to EEPROM memory
	eecon1.EEPGD = 0;
	// Write enable
	set_bit(eecon1, WREN);
	// Disable interrupts
	clear_bit(intcon, GIE);

	// Required write sequence
	eecon2 = 0x55;
	eecon2 = 0xAA;
	set_bit(eecon1, WR);

	// Disable write
	clear_bit(eecon1, WREN);
	// Enable interrupts
	set_bit(intcon, GIE);
}


////////////////////////////////////////////////////////////
// Print a region of the memory to the serial port
////////////////////////////////////////////////////////////
void eeprom_dump(char startaddr, char stopaddr){
	short i;
	char data;
	
	serial_printf("DUMP EEPROM MEMORY ");
	serial_print_hex(startaddr);
	serial_printf(" - 0x");
	serial_print_hex(stopaddr);
	serial_print_lf();
	serial_printf("Address Data");
	
	for (i=startaddr; i<=stopaddr; i++){
		if ((i & 0x0007)==0){
			serial_print_lf();
			serial_printf("0x");
			serial_print_hex(i);
			serial_printf("  ");
		}
		data = eeprom_read(i);
		serial_print_hex(data);
		serial_printf(' ');
		
	}
	
	serial_print_lf();
}
