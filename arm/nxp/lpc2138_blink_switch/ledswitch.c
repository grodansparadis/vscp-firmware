//////////////////////////////////////////////////////////////////////////////
//
//             Philips LPC213X LED/Switch Example
//               for the Keil MBC2130 board
//
// This example demonstrates writing to and reading from 
// the GPIO port.
// (1) flash LED1 P1.16 10 times
// (2) wait for key-press (INT1 switch), turn on LED of key is pressed
//
// by Martin THOMAS, Kaiserslautern, Germany
// (eversmith@heizung-thomas.de)
// Ported to the MBC2130 by Ake Hedman
// akhe@eurosource.se
//
// based on an example by Rowley Associates Limited found somewhere
// in the "net"
//
// (all compiler optimizations should be disabled - see makefile)
//////////////////////////////////////////////////////////////////////////////

// Add path to this in makefile EXTRAINCDIRS
#include "lpc213x.h"

// MBC-2130 LPC-P2106: one led on P1.16 (active high)
#define LEDPIN  16
// MBC-2130 LPC-P2106: one switch on P0.31 (active low)
#define SWPIN 	14

static void ledInit(void)
{
	IODIR1 |= 0xff0000;			// define LED-Pins as output
	IODIR0 &= ~(1<<SWPIN);		// define Switch-Pin as input	
	IOCLR1 = (1<<LEDPIN);		// clr Bit = LED off (active high)
}

static void delay(void)
{
	volatile int i,j;

	for (i=0;i<100;i++) {
		for (j=0;j<1000;j++);
	}
}
 
int main(void)
{
	int i;
	
	MAMCR = 2;		// MAM functions fully enabled (?)
	
	ledInit();
	
	i=0;
	
	while ( i < 10 ) {
		IOCLR1 = ( 1 << LEDPIN );			//  LED off
		delay();
		IOSET1 = ( 1 << LEDPIN );			//  LED on
		delay();
		i++;
	}
	
	while ( 1 ) {
	
		if ( IOPIN0 & (1 << SWPIN ) ) {	// true if button released (active low)
			IOCLR1 = ( 1 << LEDPIN );		// LED off
		}
		else {
			IOSET1 = ( 1 << LEDPIN );		// LED on
		}
		
	}

	return 0;
}
