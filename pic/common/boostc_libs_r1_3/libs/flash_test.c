/*************************************************************
* Flash program memory interface for PIC16F87X devices 
*
* Allows for read and write of the program memory. Be aware
* that write operations by design are performed by four bytes 
* at once. 
*
* Warning! Interrupts are disabled during write !
*
* (c) 2005, L. Hollevoet, boostc compiler.
*************************************************************/
#include "flash_test.h"
#include "serial.h"
#include "flash.h"

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

	flash_dump(0x1000, 0x10FF);

	set_bit(portb, 0);
	serial_printf("Writing location 0x1000\r\n");
	flash_loadbuffer(0x0ace);
	flash_loadbuffer(0x0eef);
	flash_loadbuffer(0x0abe);
	flash_loadbuffer(0x0FFF);
	flash_write(0x1000);
	clear_bit(portb, 0);
	
	flash_write(0x1004);
	
	flash_dump(0x1F98, 0x1FFF);
	
	
	
}

