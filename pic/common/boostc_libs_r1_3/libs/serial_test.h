/*************************************************************
* Test suite for the serial interface lib
*
* (c) 2005, Lieven Hollevoet, boostc compiler.
*************************************************************/
#ifndef _SERIAL_TEST_H_
#define _SERIAL_TEST_H_

#include <system.h>

// Define oscillator frequency
#pragma CLOCK_FREQ 10000000

// Define port directions
#define PortAConfig  11111111b  //  1=input
#define PortBConfig  11000000b  //  1=input 
#define PortCConfig  11010000b	

// Variables
char serialReceived;

// Function declarations
void init(void);

#endif //_SERIAL_TEST_H_
