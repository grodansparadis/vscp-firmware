//led.c
#include <lpc21xx.h>
#include "config.h"
#include "led.h"

//it's a simple delay
void Delay (unsigned long a) {
	while (--a!=0);
}

// init led port
void LedsInit(void) {
	//IO0DIR_bit.P0_8 = 1;     //set port0.8 to output
	//IO0DIR_bit.P0_10 = 1;    //set port0.10 to output
	//IO0DIR_bit.P0_11 = 1;    //set port0.11 to output
	IODIR0 = PIO0_LED1 | PIO0_LED1 | PIO0_LED1;
	
	//IO0SET_bit.P0_8 = 1;     //led1 off
	//IO0SET_bit.P0_10 = 1;    //led2 off
	//IO0SET_bit.P0_11 = 1;    //led3 off
	IOSET0 = PIO0_LED1 | PIO0_LED1 | PIO0_LED1;
}

// single blink led 1
void Led1Blink(void) {
	//IO0CLR_bit.P0_8 = 1;    	//on led
	IOCLR0 = PIO0_LED1;
	Delay(500000);      		//delay
	//IO0SET_bit.P0_8 = 1;     	//off led
	IOSET0 = PIO0_LED1;
	//Delay(500000);           	//delay
}

// turn on led 1
void Led1On(void) {
	//IO0CLR_bit.P0_8 = 1;     //on led
	IOCLR0 = PIO0_LED1;
}

// turn off led 1
void Led1Off(void) {
	//IO0SET_bit.P0_8 = 1;     //off led
	IOSET0 = PIO0_LED1;
}


// single blink led 2
void Led2Blink(void) {
	//IO0CLR_bit.P0_10 = 1;    //on led
	IOCLR0 = PIO0_LED2;
	Delay(500000);           //delay
	//IO0SET_bit.P0_10 = 1;    //off led
	IOSET0 = PIO0_LED2;
	//Delay(500000);           //delay
}

// turn on led 2
void Led2On(void) {
	//IO0CLR_bit.P0_10 = 1;    //on led
	IOCLR0 = PIO0_LED2;
}

// turn off led 2
void Led2Off(void) {
	//IO0SET_bit.P0_10 = 1;    //off led
	IOSET0 = PIO0_LED2;
}


// single blink led 3
void Led3Blink(void) {
	//IO0CLR_bit.P0_11 = 1;    //on led
	IOCLR0 = PIO0_LED3;
	Delay(500000);           //delay
	//IO0SET_bit.P0_11 = 1;    //off led
	IOSET0 = PIO0_LED3;
	//Delay(500000);           //delay
}

// turn on led 3
void Led3On(void) {
	//IO0CLR_bit.P0_11 = 1;    //on led
	IOCLR0 = PIO0_LED3;
}

// turn off led 3
void Led3Off(void) {
	//IO0SET_bit.P0_11 = 1;    //off led
	IOSET0 = PIO0_LED3;
}

