// all hardware related routines should be placed here

void ini_hardware()
{

	PORTA   = 0xFF;     // all off
	DDRA    = 0xFF;     // all output

	PORTB   = 0x00;		// not used
	DDRB    = 0x00;		// not used

	PORTC   = 0xFF;     // all off
	DDRC    = 0xFF;     // all output


	PORTD   = 0x00;
	DDRD    = 0x00;

	PORTE   = 0x00;
	DDRE    = 0x00;

	PORTF   = 0x0C;		//0&1 LED / 2&3 BUTTON
	DDRF    = 0x03;

	PORTG   = 0x00;
	DDRG    = 0x00;



}


