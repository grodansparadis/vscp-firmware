// all hardware related routines should be placed here

void ini_hardware()
{
#ifndef BOARD
#error You must define the used board in the makefile

#elif BOARD == 0 //olimex
	PORTA   = 0xff;     // Activate pull-ups
	DDRA    = 0x00;     // Inputs

	PORTB   = 0x00;		// leds on
	DDRB    = 0xFF;		// all output

	PORTC   = 0x00;
	DDRC    = 0x00;

	PORTD   = 0x00;
	DDRD    = 0x20;

	PORTE   = 0x80;     // LED on
	DDRE    = 0xDF;     // all output except button

	PORTF   = 0x00;
	DDRF    = 0x00;

	PORTG   = 0x00;
	DDRG    = 0x00;

#elif BOARD == 1 //aamini0
	PORTA   = 0xff;     // Activate pull-ups
	DDRA    = 0x00;     // Inputs K1

	PORTB   = 0xFF;		// all leds off
	DDRB    = 0xFF;		// all output K2

	PORTC   = 0x00;	// below not used for now
	DDRC    = 0x00;

	PORTD   = 0x00;
	DDRD    = 0x00;

	PORTE   = 0x00;
	DDRE    = 0x00;

	PORTF   = 0x00;
	DDRF    = 0x00;

	PORTG   = 0x00;
	DDRG    = 0x00;
#else
  #error This board is currently not supported, add it to hardware.c
#endif


}

unsigned char ReadSwitch (unsigned char i)
{
return (!(PINA & _BV(i-1)));
}


