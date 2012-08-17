/*************************************************************
* 7-pins LCD interface code test program
*
* (c) 2005, Lieven Hollevoet, boostc compiler.
*************************************************************/
#ifndef _LCD_TEST_H_
#define _LCD_TEST_H_

#include <system.h>

// Define oscillator frequency
#pragma CLOCK_FREQ 10000000

// Define port directions
#define PortAConfig  11111111b  //  1=input
#define PortBConfig  11000000b  //  1=input 
#define PortCConfig  11010000b	

// Function declarations
void init(void);

#endif //_LCD_TEST_H_
