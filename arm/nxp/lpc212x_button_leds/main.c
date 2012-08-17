// This program is created for LPC-E2129 board
// It shows how to works with leds and buttons

#include "config.h"
#include "system.h"
#include "led.h"
#include "buttons.h"

// simple delay
void DelayS (unsigned long a) { while (--a!=0); };

int main()
{
	/**** INITIALIZATION ****/
	// Frequency initialization
	//FrecInit();

	sysInit();
	
	// Led initialization
	//LedsInit();
	
	// Buttons initialization
	//InitButtons();

	Led1On();
	
	// loop forever
	while ( TRUE ) {

		// if button 1 is pressed
		if ( !BUT1_PRESSED ) {
			Led1Off();
			Led1Blink();
			Led2Blink();
			Led3Blink();
		}

		// if button 2 is pressed
		if ( !BUT2_PRESSED ) {
			Led3Blink();
			Led2Blink();
			Led1Blink();
		}
	}

}
