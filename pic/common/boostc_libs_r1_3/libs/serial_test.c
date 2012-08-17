/*************************************************************
* Test suite for the serial interface lib
*
* (c) 2005, Lieven Hollevoet, boostc compiler.
*************************************************************/
#include "serial_test.h"
#include "serial.h"

#pragma DATA 0x2007, _CP_OFF & _DEBUG_OFF & _WRT_OFF & _CPD_OFF & _LVP_OFF & _BODEN_ON & _PWRTE_ON & _WDT_OFF & _HS_OSC

void main()
{
	init();

	while (1){
	
		char input;
		// Check if something was received from the serial port
		if (serial_peek()){
			// If it was, go get it...
			input = serial_getch();
			
			// Print it as 'char'
			serial_printf("You have pressed the key: ");
			serial_printf(input);
			serial_print_lf();
			
			serial_printf("It has the following value:\r\n");
			
			// Print it as hex value
			serial_printf(" * hex: ");
			serial_print_hex(input);
			serial_print_lf();
			
			// Print it as decimal value
			serial_printf(" * dec: ");
			serial_print_dec(input);
			serial_print_lf();
				
			// Print it as binary value
			serial_printf(" * bin: ");
			serial_print_bin(input);
			serial_print_lf();

		    // Show off the function overloading  ;-)
			serial_printf("\r\nYou can also print shorts in hex format: 0x");
			serial_print_hex((short)0xBABE);
			serial_print_lf();
		}
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
	serial_printf("Hello world...\r\n");
	serial_printf("Init sequence complete...\r\n");
	serial_printf("Press a key and examine the returned output.\r\n");
	
}

