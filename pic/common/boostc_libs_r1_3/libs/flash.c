/*************************************************************
* Flash program memory interface for PIC16F87X devices 
*
* Allows for read and write of the program memory. Be aware
* that write operations by design are performed by four bytes 
* at once. 
*
* Warning! Interrupts are disabled during write !
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/
#include "flash.h"
#include "serial.h"

short flash_writebuf[4];
char flash_pointer = 0;

////////////////////////////////////////////////////////////
// Internal function
// Loads the (short) address in both eeadrh and eeadr
////////////////////////////////////////////////////////////
void flash_load_address(short address){
	eeadrh = (char)((address >> 8) & 0x00FF);
	eeadr  = (char)(address & 0x00FF);
}

////////////////////////////////////////////////////////////
// Internal function
// Loads the (short) data in both eeadrh and eeadr
////////////////////////////////////////////////////////////
void flash_load_data(short data){
	eedath = (char)((data >> 8) & 0x00FF);
	eedata  = (char)(data & 0x00FF);
}


////////////////////////////////////////////////////////////
// Read a flash memory location. Takes the address as input 
// and returns the data at the memory location.
//
// The function is based on the Microchip document DS39582A
// page 34.
////////////////////////////////////////////////////////////
short flash_read(short address){
	// Load address
	flash_load_address(address);
	
	// Point to program memory
	set_bit(eecon1, EEPGD);
	
	// Read
	set_bit(eecon1, RD);
	nop();
	nop();
	
	short retval;
	retval = ((short)eedath & 0x00FF);
	retval = retval << 8;
	retval |= ((short)eedata & 0x00FF);
	
	return retval;
}

////////////////////////////////////////////////////////////
// Writes a block of 4 consecutive shorts to the flash 
// program memory. A block consists of four words with 
// sequential addresses, with a lower boundary defined by 
// an address that has its two LSB's set to 0.
// The four shorts to be written have to be loaded in
// flash_writebuf using the flash_loadbuffer funtion
// 
// The function is based on the Microchip document DS39582A
// page 36.
////////////////////////////////////////////////////////////
void flash_write(short address){
	
	char i;
	
	// Mask off the lower two bits (hardware requirement for write)
	address = address & 0xFFFC;
	
	// Load the base address
	flash_load_address(address);
	
	for (i=0; i<4; i++){
		// Load data
		flash_load_data(flash_writebuf[i]);
		// Point to program memory
		set_bit(eecon1, EEPGD);
		// Write enable
		set_bit(eecon1, WREN);
		// Disable interrupts
		clear_bit(intcon, GIE);
		// Required write sequence
		eecon2 = 0x55;
		eecon2 = 0xAA;
		set_bit(eecon1, WR);
		nop();
		nop();
		// Disable write
		clear_bit(eecon1, WREN);
		// Enabel interrupts
		set_bit(intcon, GIE);
		// Increment address for next write
		eeadr++;
	}
	flash_pointer = 0;
}


void flash_loadbuffer(short data){
	flash_writebuf[flash_pointer] = data & 0x3FFF;
	flash_pointer++;
	return;
}
////////////////////////////////////////////////////////////
// Print a region of the memory to the serial port
////////////////////////////////////////////////////////////
void flash_dump(short startaddr, short stopaddr){
	short i;
	short data;
	
	serial_printf("DUMP FLASH MEMORY range 0x");
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
		data = flash_read(i);
		serial_print_hex(data);
		serial_printf(' ');
		
	}
	
	serial_print_lf();
}
