/*************************************************************
* EEPROM data memory interface for PIC16F87X devices 
*
* (c) 2005, L. Hollevoet, boostc compiler.
*************************************************************/
#include "eeprom_test.h"
#include "serial.h"
#include "eeprom.h"

#pragma DATA 0x2007, _CP_OFF & _DEBUG_OFF & _WRT_OFF & _CPD_OFF & _LVP_OFF & _BODEN_ON & _PWRTE_ON & _WDT_OFF & _HS_OSC

void main()
{
	init();

	while (1){
	}
	
}

void interrupt( void )
{
}

void init(void) 
{
	// Port configuration
	
	//adcon1     = 0x06; // all digital pins on PORTA
	 
	porta = 0;
	portb = 0;
	portc = 0x04;

	trisa = PortAConfig;    
	trisb = PortBConfig;
	trisc = PortCConfig;
	
	option_reg = 01000101b;
	
	// Serial interface
	serial_init(0x0A);
	serial_print_lf();
	serial_printf("Booting...\r\n");

	// Read EEPROM location
	char value = eeprom_read(0x00);

	serial_printf("EEPROM[0] has value 0x");
	serial_print_hex(value);
	serial_print_lf();
	
	// Write EEPROM location
	eeprom_write(0x00, 0xAB);
	
	value = eeprom_read(0x00);
	serial_printf("EEPROM[0] has value 0x");
	serial_print_hex(value);
	serial_print_lf();
	
	// Dump entire EEPROM memory range
	eeprom_dump(0x00, 0xFF);
	
}

