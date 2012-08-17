/*************************************************************
* Test program for the ADC interface of PIC16F devices
*
* (c) Lieven Hollevoet, boostc compiler.
*************************************************************/

#include "adc_test.h"
#include "serial.h"
#include "adc.h"

#pragma DATA 0x2007, _CP_OFF & _DEBUG_OFF & _WRT_OFF & _CPD_OFF & _LVP_OFF & _BODEN_ON & _PWRTE_ON & _WDT_OFF & _HS_OSC


void main()
{
	init();

	while (1){

		char input;
		char channel;

		// Peek the serial interface (check if a char was received)
		if (serial_peek()){
			// If it was, get it from the UART interface
			input = serial_getch();
			// Echo it back
			serial_printf(input);
			// Convert it to the channel number
			channel = input - '0';
			// If it is in the valid range, change the ADC channel selector
			if ((channel >= 0) && (channel <6)){
				serial_printf("Switching ADC channel to:");
				serial_print_dec(channel);
				serial_print_lf();
			} 
			
		}
		
		// Wait a while
		delay_s(1);

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

		

}

