/*************************************************************
* 7-pins LCD interface code test program
*
* (c) 2005, Lieven Hollevoet, boostc compiler.
*************************************************************/
#include "lcd_test.h"
#include "serial.h"
#include "lcd.h"

#pragma DATA 0x2007, _CP_OFF & _DEBUG_OFF & _WRT_OFF & _CPD_OFF & _LVP_OFF & _BODEN_ON & _PWRTE_ON & _WDT_OFF & _HS_OSC


void main()
{
	init();

	while (1){
	
		char input;
		
		// Print any chars received through the serial port
		// to the LCD
		if (serial_peek()){
			input = serial_getch();
			serial_printf(input);
			
			// Clear the display when 0 is received
			if (input == '0'){
				lcd_send_cmd(DISP_CLR);
			} 

			lcd_printf(input);
			
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
	serial_print_lf();
	serial_printf("Booting...\r\n");

	// LCD interface
	lcd_init();
	serial_printf("LCD init done");

	lcd_send_cmd(LINE1);
	lcd_printf("LCD interface");
		
	lcd_send_cmd(LINE2);
	lcd_printf("L. Hollevoet");
	
	return;
}

