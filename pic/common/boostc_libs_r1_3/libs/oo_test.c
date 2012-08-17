/*************************************************************
* Test suite for the One Wire lib
*
* (c) 2005, Lieven Hollevoet, boostc compiler.
*************************************************************/
#include "oo_test.h"
#include "serial.h"
#include "oo.h"

#pragma DATA 0x2007, _CP_OFF & _DEBUG_OFF & _WRT_OFF & _CPD_OFF & _LVP_OFF & _BODEN_ON & _PWRTE_ON & _WDT_OFF & _HS_OSC

void main()
{
	init();

	while (1){
	
		// Allow wakeup
		delay_ms(100);
	
		// Reset the one wire bus
		oo_busreset();
		
		// Start the temparature conversion (non-blocking function)
	    oo_start_conversion();
	
		// Wait for completion, you could do other stuff here
		// But make sure that this function returns zero before 
		// reading the scratchpad
		if (oo_wait_for_completion() == 1){
			serial_printf("Temperature conversion timed out");
		}
	
		// Read the scratchpad
		if (oo_read_scratchpad()){
			serial_printf("OO read scratchpad failed");
			while(1);
		}
	
		// And extract the temperature information
		short temp = oo_get_temp();
		
		// The temperature is 
		serial_printf("Temperature: ");
		serial_print_hex(temp);
		serial_printf(" -- ");
		
		// And for positive temps you can simply convert this to
		char tmp_work = (char)(temp >> 1);
		serial_print_dec(tmp_work);
		if (temp & 0x0001){
			serial_printf(".5");
		} else {
			serial_printf(".0");
		}
		serial_printf(" degrees C.\r\n");
		
		// A more accurate conversion is described in the DS1820 datasheet.
		
		delay_s(1);
	}
	
}

void interrupt( void )
{
}

void init(void) 
{
	// Port configuration
	char test = 0x03 ^ 0x06;
	
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

