// all hardware related routines should be placed here

#ifndef hardware_H
#define hardware_H

#include "hardware.c"



void ini_hardware();



	#define LED_STATUS_ON       ((PORTF &= ~_BV(0)))
	#define LED_STATUS_OFF      ((PORTF |= _BV(0)))
	#define LED_STATUS_TOGGLE   ((PORTF ^= _BV(0)))
	#define BTN_INIT_PRESSED    (!(PINF & _BV(2)))  //button S2 : re-initiate node

	#define LED_IND_ON       ((PORTF &= ~_BV(1)))
	#define LED_IND_OFF      ((PORTF |= _BV(1)))
	#define LED_IND_TOGGLE   ((PORTF ^= _BV(1)))
	#define BTN_SW1_PRESSED     (!(PINF & _BV(3))) //button S1: perform selftest on startup


	#define outputport1			PORTA
	#define outputpin1			PINA
	#define read_output1		(PINA)

	#define outputport2			PORTC
	#define outputpin2			PINC
	#define read_output2		(PINC)
	
	#define outputport3			PORTE
	#define outputpin3			PINE
	#define read_output3		(PINE)

	#define outputport4			PORTB
	#define outputpin4			PINB
	#define read_output4		(PINB)



#endif